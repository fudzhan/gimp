/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <stdlib.h>
#include <string.h>

#include "appenv.h"
#include "gimprc.h"
#include "gximage.h"
#include "image_render.h"
#include "pixel_region.h"
#include "scale.h"

#include "tile.h"			/* ick. */

typedef struct _RenderInfo  RenderInfo;
typedef void (*RenderFunc) (RenderInfo *info);

struct _RenderInfo
{
  GDisplay *gdisp;
  TileManager *src_tiles;
  guint *alpha;
  guchar *scale;
  guchar *src;
  guchar *dest;
  int x, y;
  int w, h;
  float scalex;
  float scaley;
  int src_x, src_y;
  int src_bpp;
  int dest_bpp;
  int dest_bpl;
  int dest_width;
  int byte_order;
};


/*  accelerate transparency of image scaling  */
guchar *blend_dark_check = NULL;
guchar *blend_light_check = NULL;
guchar *tile_buf = NULL;
guchar *check_buf = NULL;
guchar *empty_buf = NULL;
guchar *temp_buf = NULL;

static guint   check_mod;
static guint   check_shift;
static guchar  check_combos[6][2] =
{
  { 204, 255 },
  { 102, 153 },
  { 0, 51 },
  { 255, 255 },
  { 127, 127 },
  { 0, 0 }
};



void
render_setup (int check_type,
	      int check_size)
{
  int i, j;

  /*  based on the tile size, determine the tile shift amount
   *  (assume here that tile_height and tile_width are equal)
   */

  /*  allocate a buffer for arranging information from a row of tiles  */
  if (!tile_buf)
    tile_buf = g_new (guchar, GXIMAGE_WIDTH * MAX_CHANNELS);

  if (check_type < 0 || check_type > 5)
    g_error ("invalid check_type argument to render_setup: %d", check_type);
  if (check_size < 0 || check_size > 2)
    g_error ("invalid check_size argument to render_setup: %d", check_size);

  if (!blend_dark_check)
    blend_dark_check = g_new (guchar, 65536);
  if (!blend_light_check)
    blend_light_check = g_new (guchar, 65536);

  for (i = 0; i < 256; i++)
    for (j = 0; j < 256; j++)
      {
	blend_dark_check [(i << 8) + j] = (guchar)
	  ((j * i + check_combos[check_type][0] * (255 - i)) / 255);
	blend_light_check [(i << 8) + j] = (guchar)
	  ((j * i + check_combos[check_type][1] * (255 - i)) / 255);
      }

  switch (check_size)
    {
    case SMALL_CHECKS:
      check_mod = 0x3;
      check_shift = 2;
      break;
    case MEDIUM_CHECKS:
      check_mod = 0x7;
      check_shift = 3;
      break;
    case LARGE_CHECKS:
      check_mod = 0xf;
      check_shift = 4;
      break;
    }

  /*  calculate check buffer for previews  */
  if (preview_size)
    {
      if (check_buf)
	g_free (check_buf);
      if (empty_buf)
	g_free (empty_buf);
      if (temp_buf)
	g_free (temp_buf);

      check_buf = (unsigned char *) g_malloc ((preview_size + 4) * 3);
      for (i = 0; i < (preview_size + 4); i++)
	{
	  if (i & 0x4)
	    {
	      check_buf[i * 3 + 0] = blend_dark_check[0];
	      check_buf[i * 3 + 1] = blend_dark_check[0];
	      check_buf[i * 3 + 2] = blend_dark_check[0];
	    }
	  else
	    {
	      check_buf[i * 3 + 0] = blend_light_check[0];
	      check_buf[i * 3 + 1] = blend_light_check[0];
	      check_buf[i * 3 + 2] = blend_light_check[0];
	    }
	}
      empty_buf = (unsigned char *) g_malloc ((preview_size + 4) * 3);
      memset (empty_buf, 0, (preview_size + 4) * 3);
      temp_buf = (unsigned char *) g_malloc ((preview_size + 4) * 3);
    }
  else
    {
      check_buf = NULL;
      empty_buf = NULL;
      temp_buf = NULL;
    }
}

void
render_free (void)
{
  g_free (tile_buf);
  g_free (check_buf);
}

/*  Render Image functions  */

static void    render_image_rgb       (RenderInfo *info);
static void    render_image_rgb_a     (RenderInfo *info);
static void    render_image_gray      (RenderInfo *info);
static void    render_image_gray_a    (RenderInfo *info);
static void    render_image_indexed   (RenderInfo *info);
static void    render_image_indexed_a (RenderInfo *info);

static void    render_image_init_info          (RenderInfo   *info,
						GDisplay     *gdisp,
						int           x,
						int           y,
						int           w,
						int           h);
static guint*  render_image_init_alpha         (int           mult);
static guchar* render_image_accelerate_scaling (int           width,
						int           start,
						float         scalex);
static guchar* render_image_tile_fault         (RenderInfo   *info);


static RenderFunc render_funcs[6] =
{
    render_image_rgb,
    render_image_rgb_a,
    render_image_gray,
    render_image_gray_a,
    render_image_indexed,
    render_image_indexed_a,
};


/*****************************************************************/
/*  This function is the core of the display--it offsets and     */
/*  scales the image according to the current parameters in the  */
/*  gdisp object.  It handles color, grayscale, 8, 15, 16, 24,   */
/*  & 32 bit output depths.                                      */
/*****************************************************************/

void
render_image (GDisplay *gdisp,
	      int       x,
	      int       y,
	      int       w,
	      int       h)
{
  RenderInfo info;
  int image_type;

  render_image_init_info (&info, gdisp, x, y, w, h);

  image_type = gimage_projection_type (gdisp->gimage);
  if ((image_type < 0) || (image_type > 5))
    {
      g_message ("unknown gimage projection type: %d",
		 gimage_projection_type (gdisp->gimage));
      return;
    }

  if ((info.dest_bpp < 1) || (info.dest_bpp > 4))
    {
      g_message ("unsupported destination bytes per pixel: %d", info.dest_bpp);
      return;
    }

  /* Currently, only RGBA and GRAYA projection types are used - the rest
   * are in case of future need.  -- austin, 28th Nov 1998. */
  if (image_type != RGBA_GIMAGE && image_type != GRAYA_GIMAGE)
      g_warning ("using untested projection type %d", image_type);

  (* render_funcs[image_type]) (&info);
}



/*************************/
/*  8 Bit functions      */
/*************************/

static void
render_image_indexed (RenderInfo *info)
{
  guchar *src;
  guchar *dest;
  guchar *cmap;
  gulong val;
  int byte_order;
  int y, ye;
  int x, xe;
  int initial;
  float error;
  float step;

  cmap = gimage_cmap (info->gdisp->gimage);

  y = info->y;
  ye = info->y + info->h;
  xe = info->x + info->w;

  step = 1.0 / info->scaley;

  error = y * step;
  error -= ((int)error) - step;

  initial = TRUE;
  byte_order = info->byte_order;
  info->src = render_image_tile_fault (info);

  for (; y < ye; y++)
    {
      if (!initial && (error < 1.0))
	memcpy (info->dest, info->dest - info->dest_bpl, info->dest_width);
      else
	{
	  src = info->src;
	  dest = info->dest;

	  g_return_if_fail (src != NULL);
	  
	  for (x = info->x; x < xe; x++)
	    {
	      val = src[INDEXED_PIX] * 3;
	      src += 1;

	      dest[0] = cmap[val+0];
	      dest[1] = cmap[val+1];
	      dest[2] = cmap[val+2];
	      dest += 3;
	    }
	}

      info->dest += info->dest_bpl;

      initial = FALSE;

      if (error >= 1.0)
	{
	  info->src_y += (int)error;
	  error -= (int)error;
	  info->src = render_image_tile_fault (info);
	  initial = TRUE;
	}

      error += step;
    }
}

static void
render_image_indexed_a (RenderInfo *info)
{
  guchar *src;
  guchar *dest;
  guint *alpha;
  guchar *cmap;
  gulong r, g, b;
  gulong val;
  guint a;
  int dark_light;
  int byte_order;
  int y, ye;
  int x, xe;
  int initial;
  float error;
  float step;

  cmap = gimage_cmap (info->gdisp->gimage);
  alpha = info->alpha;

  y = info->y;
  ye = info->y + info->h;
  xe = info->x + info->w;

  step = 1.0 / info->scaley;

  error = y * step;
  error -= ((int)error) - step;

  initial = TRUE;
  byte_order = info->byte_order;
  info->src = render_image_tile_fault (info);

  for (; y < ye; y++)
    {
      if (!initial && (error < 1.0) && (y & check_mod))
	memcpy (info->dest, info->dest - info->dest_bpl, info->dest_width);
      else
	{
	  src = info->src;
	  dest = info->dest;

	  dark_light = (y >> check_shift) + (info->x >> check_shift);

	  g_return_if_fail (src != NULL);

	  for (x = info->x; x < xe; x++)
	    {
	      a = alpha[src[ALPHA_I_PIX]];
	      val = src[INDEXED_PIX] * 3;
	      src += 2;

	      if (dark_light & 0x1)
		{
		  r = blend_dark_check[(a | cmap[val+0])];
		  g = blend_dark_check[(a | cmap[val+1])];
		  b = blend_dark_check[(a | cmap[val+2])];
		}
	      else
		{
		  r = blend_light_check[(a | cmap[val+0])];
		  g = blend_light_check[(a | cmap[val+1])];
		  b = blend_light_check[(a | cmap[val+2])];
		}

		dest[0] = r;
		dest[1] = g;
		dest[2] = b;
		dest += 3;

		if (((x + 1) & check_mod) == 0)
		  dark_light += 1;
	      }
	}

      info->dest += info->dest_bpl;

      initial = FALSE;

      if (error >= 1.0)
	{
	  info->src_y += (int)error;
	  error -= (int)error;
	  info->src = render_image_tile_fault (info);
	  initial = TRUE;
	}

      error += step;
    }
}

static void
render_image_gray (RenderInfo *info)
{
  guchar *src;
  guchar *dest;
  gulong val;
  int byte_order;
  int y, ye;
  int x, xe;
  int initial;
  float error;
  float step;

  y = info->y;
  ye = info->y + info->h;
  xe = info->x + info->w;

  step = 1.0 / info->scaley;

  error = y * step;
  error -= ((int)error) - step;

  initial = TRUE;
  byte_order = info->byte_order;
  info->src = render_image_tile_fault (info);

  for (; y < ye; y++)
    {
      if (!initial && (error < 1.0))
	memcpy (info->dest, info->dest - info->dest_bpl, info->dest_width);
      else
	{
	  src = info->src;
	  dest = info->dest;
	  
	  g_return_if_fail (src != NULL);

	  for (x = info->x; x < xe; x++)
	    {
	      val = src[GRAY_PIX];
	      src += 1;

	      dest[0] = val;
	      dest[1] = val;
	      dest[2] = val;
	      dest += 3;
	    }
	}

      info->dest += info->dest_bpl;

      initial = FALSE;

      if (error >= 1.0)
	{
	  info->src_y += (int)error;
	  error -= (int)error;
	  info->src = render_image_tile_fault (info);
	  initial = TRUE;
	}

      error += step;
    }
}

static void
render_image_gray_a (RenderInfo *info)
{
  guchar *src;
  guchar *dest;
  guint *alpha;
  gulong val;
  guint a;
  int dark_light;
  int byte_order;
  int y, ye;
  int x, xe;
  int initial;
  float error;
  float step;

  alpha = info->alpha;

  y = info->y;
  ye = info->y + info->h;
  xe = info->x + info->w;

  step = 1.0 / info->scaley;

  error = y * step;
  error -= ((int)error) - step;

  initial = TRUE;
  byte_order = info->byte_order;
  info->src = render_image_tile_fault (info);

  for (; y < ye; y++)
    {
      if (!initial && (error < 1.0) && (y & check_mod))
	memcpy (info->dest, info->dest - info->dest_bpl, info->dest_width);
      else
	{
	  src = info->src;
	  dest = info->dest;

	  dark_light = (y >> check_shift) + (info->x >> check_shift);

	  g_return_if_fail (src != NULL);

	  for (x = info->x; x < xe; x++)
	    {
	      a = alpha[src[ALPHA_G_PIX]];
	      if (dark_light & 0x1)
		val = blend_dark_check[(a | src[GRAY_PIX])];
	      else
		val = blend_light_check[(a | src[GRAY_PIX])];
	      src += 2;

	      dest[0] = val;
	      dest[1] = val;
	      dest[2] = val;
	      dest += 3;

	      if (((x + 1) & check_mod) == 0)
		dark_light += 1;
	    }
	}

      info->dest += info->dest_bpl;

      initial = FALSE;

      if (error >= 1.0)
	{
	  info->src_y += (int)error;
	  error -= (int)error;
	  info->src = render_image_tile_fault (info);
	  initial = TRUE;
	}

      error += step;
    }
}

static void
render_image_rgb (RenderInfo *info)
{
  guchar *src;
  guchar *dest;
  int y, x;
  gboolean initial;
  float error;
  const int ye = info->y + info->h;
  const int xe = info->x + info->w;
  const float step = 1.0 / info->scaley;

  y = info->y;

  error = y * step;
  error -= (int)error - step;

  initial = TRUE;
  info->src = render_image_tile_fault (info);

  for (; y < ye; y++)
    {
      if (!initial && (error < 1.0))
	memcpy (info->dest, info->dest - info->dest_bpl, info->dest_width);
      else
	{
	  src = info->src;
	  dest = info->dest;

	  g_return_if_fail (src != NULL);
	  
	  /* replace this with memcpy, or better yet, avoid it altogether? */
	  for (x = info->x; x < xe; x++)
	    {
	      dest[0] = src[0];
	      dest[1] = src[1];
	      dest[2] = src[2];

	      src += 3;
	      dest += 3;
	    }
	}

      info->dest += info->dest_bpl;

      initial = FALSE;

      if (error >= 1.0)
	{
	  info->src_y += (int)error;
	  error -= (int)error;
	  info->src = render_image_tile_fault (info);
	  initial = TRUE;
	}

      error += step;
    }
}



static void
render_image_rgb_a (RenderInfo *info)
{
  guchar *src;
  guchar *dest;
  guint *alpha;
  gulong r, g, b;
  guint a;
  int dark_light;
  gboolean byte_order;
  int y, x;
  int initial;
  float error;
  const int ye = info->y + info->h;
  const int xe = info->x + info->w;
  const float step = 1.0 / info->scaley;

  alpha = info->alpha;

  y = info->y;

  error = y * step;
  error -= ((int)error) - step;

  initial = TRUE;
  byte_order = info->byte_order;
  info->src = render_image_tile_fault (info);

  for (; y < ye; y++)
    {
      if (!initial && (error < 1.0) && (y & check_mod))
	memcpy (info->dest, info->dest - info->dest_bpl, info->dest_width);
      else
	{
	  src = info->src;
	  dest = info->dest;

	  dark_light = (y >> check_shift) + (info->x >> check_shift);

	  g_return_if_fail (src != NULL);

	  for (x = info->x; x < xe; x++)
	    {
	      a = alpha[src[ALPHA_PIX]];
	      if (dark_light & 0x1)
		{
		  r = blend_dark_check[(a | src[RED_PIX])];
		  g = blend_dark_check[(a | src[GREEN_PIX])];
		  b = blend_dark_check[(a | src[BLUE_PIX])];
		}
	      else
		{
		  r = blend_light_check[(a | src[RED_PIX])];
		  g = blend_light_check[(a | src[GREEN_PIX])];
		  b = blend_light_check[(a | src[BLUE_PIX])];
		}

	      src += 4;

	      dest[0] = r;
	      dest[1] = g;
	      dest[2] = b;
	      dest += 3;

	      if (((x + 1) & check_mod) == 0)
		dark_light += 1;
	    }
	}

      info->dest += info->dest_bpl;

      initial = FALSE;

      if (error >= 1.0)
	{
	  info->src_y += (int)error;
	  error -= (int)error;
	  info->src = render_image_tile_fault (info);
	  initial = TRUE;
	}

      error += step;
    }
}

static void
render_image_init_info (RenderInfo *info,
			GDisplay   *gdisp,
			int         x,
			int         y,
			int         w,
			int         h)
{
  info->gdisp = gdisp;
  info->src_tiles = gimage_projection (gdisp->gimage);
  info->x = x + gdisp->offset_x;
  info->y = y + gdisp->offset_y;
  info->w = w;
  info->h = h;
  info->scalex = SCALEFACTOR_X (gdisp);
  info->scaley = SCALEFACTOR_Y (gdisp);
  info->src_x = UNSCALEX (gdisp, info->x);
  info->src_y = UNSCALEY (gdisp, info->y);
  info->src_bpp = gimage_projection_bytes (gdisp->gimage);
  info->dest = gximage_get_data ();
  info->dest_bpp = gximage_get_bpp ();
  info->dest_bpl = gximage_get_bpl ();
  info->dest_width = info->w * info->dest_bpp;
  info->byte_order = gximage_get_byte_order ();
  info->scale = render_image_accelerate_scaling (w, info->x, info->scalex);
  info->alpha = NULL;

  switch (gimage_projection_type (gdisp->gimage))
    {
    case RGBA_GIMAGE:
    case GRAYA_GIMAGE:
    case INDEXEDA_GIMAGE:
      info->alpha = render_image_init_alpha (gimage_projection_opacity (gdisp->gimage));
      break;
    default:
      /* nothing special needs doing */
      break;
    }
}

static guint*
render_image_init_alpha (int mult)
{
  static guint *alpha_mult = NULL;
  static int alpha_val = -1;
  int i;

  if (alpha_val != mult)
    {
      if (!alpha_mult)
	alpha_mult = g_new (guint, 256);

      alpha_val = mult;
      for (i = 0; i < 256; i++)
	alpha_mult[i] = ((mult * i) / 255) << 8;
    }

  return alpha_mult;
}

static guchar*
render_image_accelerate_scaling (int   width,
				 int   start,
				 float scalex)
{
  static guchar *scale = NULL;
  float error;
  float step;
  int i;

  if (!scale)
    scale = g_new (guchar, GXIMAGE_WIDTH + 1);

  step = 1.0 / scalex;

  error = start * step;
  error -= ((int)error) - step;

  for (i = 0; i <= width; i++)
  {
    scale[i] = ((int)error);
    error += step - (int)error;
  }    

  return scale;
}

static guchar*
render_image_tile_fault (RenderInfo *info)
{
  Tile *tile;
  guchar *data;
  guchar *dest;
  guchar *scale;
  int width;
  int tilex;
  int tiley;
  int srctilex, srctiley;
  int step;
  int bpp = info->src_bpp;
  int x, b;

  tilex = info->src_x / TILE_WIDTH;
  tiley = info->src_y / TILE_HEIGHT;

  tile = tile_manager_get_tile (info->src_tiles,
				srctilex=info->src_x, srctiley=info->src_y,
				TRUE, FALSE);
  if (!tile)
    return NULL;

  data = tile_data_pointer (tile, 
			    info->src_x % TILE_WIDTH,
			    info->src_y % TILE_HEIGHT);
  scale = info->scale;
  dest = tile_buf;

  x = info->src_x;
  width = info->w;

  while (width--)
    {
      for (b = 0; b < bpp; b++)
	*dest++ = data[b];

      step = *scale++;
      if (step != 0)
	{
	  x += step;
	  data += step * bpp;

	  if ((x >> TILE_SHIFT) != tilex)
	    {
	      tile_release (tile, FALSE);
	      tilex += 1;

	      tile = tile_manager_get_tile (info->src_tiles, srctilex=x,
					    srctiley=info->src_y, TRUE, FALSE);
	      
	      data = tile_data_pointer (tile, 
			    x % TILE_WIDTH,
			    info->src_y % TILE_HEIGHT);
	    }
	}
    }

  tile_release (tile, FALSE);
  return tile_buf;
}
