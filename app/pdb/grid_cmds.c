/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"


#include <glib-object.h>

#include "libgimpbase/gimpbaseenums.h"
#include "libgimpcolor/gimpcolor.h"

#include "pdb-types.h"
#include "procedural_db.h"
#include "core/gimpparamspecs.h"

#include "core/gimpgrid.h"
#include "core/gimpimage-grid.h"
#include "core/gimpimage.h"

static ProcRecord image_grid_get_spacing_proc;
static ProcRecord image_grid_set_spacing_proc;
static ProcRecord image_grid_get_offset_proc;
static ProcRecord image_grid_set_offset_proc;
static ProcRecord image_grid_get_foreground_color_proc;
static ProcRecord image_grid_set_foreground_color_proc;
static ProcRecord image_grid_get_background_color_proc;
static ProcRecord image_grid_set_background_color_proc;
static ProcRecord image_grid_get_style_proc;
static ProcRecord image_grid_set_style_proc;

void
register_grid_procs (Gimp *gimp)
{
  /*
   * image_grid_get_spacing
   */
  procedural_db_init_proc (&image_grid_get_spacing_proc, 1, 2);
  procedural_db_add_argument (&image_grid_get_spacing_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_spacing_proc,
                                  GIMP_PDB_FLOAT,
                                  g_param_spec_double ("xspacing",
                                                       "xspacing",
                                                       "The image's grid horizontal spacing",
                                                       -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_spacing_proc,
                                  GIMP_PDB_FLOAT,
                                  g_param_spec_double ("yspacing",
                                                       "yspacing",
                                                       "The image's grid vertical spacing",
                                                       -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_get_spacing_proc);

  /*
   * image_grid_set_spacing
   */
  procedural_db_init_proc (&image_grid_set_spacing_proc, 3, 0);
  procedural_db_add_argument (&image_grid_set_spacing_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_spacing_proc,
                              GIMP_PDB_FLOAT,
                              g_param_spec_double ("xspacing",
                                                   "xspacing",
                                                   "The image's grid horizontal spacing",
                                                   -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                   GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_spacing_proc,
                              GIMP_PDB_FLOAT,
                              g_param_spec_double ("yspacing",
                                                   "yspacing",
                                                   "The image's grid vertical spacing",
                                                   -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                   GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_set_spacing_proc);

  /*
   * image_grid_get_offset
   */
  procedural_db_init_proc (&image_grid_get_offset_proc, 1, 2);
  procedural_db_add_argument (&image_grid_get_offset_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_offset_proc,
                                  GIMP_PDB_FLOAT,
                                  g_param_spec_double ("xoffset",
                                                       "xoffset",
                                                       "The image's grid horizontal offset",
                                                       -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_offset_proc,
                                  GIMP_PDB_FLOAT,
                                  g_param_spec_double ("yoffset",
                                                       "yoffset",
                                                       "The image's grid vertical offset",
                                                       -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_get_offset_proc);

  /*
   * image_grid_set_offset
   */
  procedural_db_init_proc (&image_grid_set_offset_proc, 3, 0);
  procedural_db_add_argument (&image_grid_set_offset_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_offset_proc,
                              GIMP_PDB_FLOAT,
                              g_param_spec_double ("xoffset",
                                                   "xoffset",
                                                   "The image's grid horizontal offset",
                                                   -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                   GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_offset_proc,
                              GIMP_PDB_FLOAT,
                              g_param_spec_double ("yoffset",
                                                   "yoffset",
                                                   "The image's grid vertical offset",
                                                   -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                   GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_set_offset_proc);

  /*
   * image_grid_get_foreground_color
   */
  procedural_db_init_proc (&image_grid_get_foreground_color_proc, 1, 1);
  procedural_db_add_argument (&image_grid_get_foreground_color_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_foreground_color_proc,
                                  GIMP_PDB_COLOR,
                                  gimp_param_spec_rgb ("fgcolor",
                                                       "fgcolor",
                                                       "The image's grid foreground color",
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_get_foreground_color_proc);

  /*
   * image_grid_set_foreground_color
   */
  procedural_db_init_proc (&image_grid_set_foreground_color_proc, 2, 0);
  procedural_db_add_argument (&image_grid_set_foreground_color_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_foreground_color_proc,
                              GIMP_PDB_COLOR,
                              gimp_param_spec_rgb ("fgcolor",
                                                   "fgcolor",
                                                   "The new foreground color",
                                                   NULL,
                                                   GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_set_foreground_color_proc);

  /*
   * image_grid_get_background_color
   */
  procedural_db_init_proc (&image_grid_get_background_color_proc, 1, 1);
  procedural_db_add_argument (&image_grid_get_background_color_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_background_color_proc,
                                  GIMP_PDB_COLOR,
                                  gimp_param_spec_rgb ("bgcolor",
                                                       "bgcolor",
                                                       "The image's grid background color",
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_get_background_color_proc);

  /*
   * image_grid_set_background_color
   */
  procedural_db_init_proc (&image_grid_set_background_color_proc, 2, 0);
  procedural_db_add_argument (&image_grid_set_background_color_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_background_color_proc,
                              GIMP_PDB_COLOR,
                              gimp_param_spec_rgb ("bgcolor",
                                                   "bgcolor",
                                                   "The new background color",
                                                   NULL,
                                                   GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_set_background_color_proc);

  /*
   * image_grid_get_style
   */
  procedural_db_init_proc (&image_grid_get_style_proc, 1, 1);
  procedural_db_add_argument (&image_grid_get_style_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&image_grid_get_style_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_enum ("style",
                                                     "style",
                                                     "The image's grid style: { GIMP_GRID_DOTS (0), GIMP_GRID_INTERSECTIONS (1), GIMP_GRID_ON_OFF_DASH (2), GIMP_GRID_DOUBLE_DASH (3), GIMP_GRID_SOLID (4) }",
                                                     GIMP_TYPE_GRID_STYLE,
                                                     GIMP_GRID_DOTS,
                                                     GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_get_style_proc);

  /*
   * image_grid_set_style
   */
  procedural_db_init_proc (&image_grid_set_style_proc, 2, 0);
  procedural_db_add_argument (&image_grid_set_style_proc,
                              GIMP_PDB_IMAGE,
                              gimp_param_spec_image_id ("image",
                                                        "image",
                                                        "The image",
                                                        gimp,
                                                        GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&image_grid_set_style_proc,
                              GIMP_PDB_INT32,
                              g_param_spec_enum ("style",
                                                 "style",
                                                 "The image's grid style: { GIMP_GRID_DOTS (0), GIMP_GRID_INTERSECTIONS (1), GIMP_GRID_ON_OFF_DASH (2), GIMP_GRID_DOUBLE_DASH (3), GIMP_GRID_SOLID (4) }",
                                                 GIMP_TYPE_GRID_STYLE,
                                                 GIMP_GRID_DOTS,
                                                 GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &image_grid_set_style_proc);

}

static Argument *
image_grid_get_spacing_invoker (ProcRecord   *proc_record,
                                Gimp         *gimp,
                                GimpContext  *context,
                                GimpProgress *progress,
                                Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  GimpImage *image;
  gdouble xspacing = 0.0;
  gdouble yspacing = 0.0;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_get (grid,
                      "xspacing", &xspacing,
                      "yspacing", &yspacing,
                      NULL);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_float = xspacing;
      return_vals[2].value.pdb_float = yspacing;
    }

  return return_vals;
}

static ProcRecord image_grid_get_spacing_proc =
{
  "gimp-image-grid-get-spacing",
  "gimp-image-grid-get-spacing",
  "Gets the spacing of an image's grid.",
  "This procedure retrieves the horizontal and vertical spacing of an image's grid. It takes the image as parameter.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_get_spacing_invoker } }
};

static Argument *
image_grid_set_spacing_invoker (ProcRecord   *proc_record,
                                Gimp         *gimp,
                                GimpContext  *context,
                                GimpProgress *progress,
                                Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gdouble xspacing;
  gdouble yspacing;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  xspacing = args[1].value.pdb_float;

  yspacing = args[2].value.pdb_float;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_set (grid,
                      "xspacing", xspacing,
                      "yspacing", yspacing,
                      NULL);
      else
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord image_grid_set_spacing_proc =
{
  "gimp-image-grid-set-spacing",
  "gimp-image-grid-set-spacing",
  "Sets the spacing of an image's grid.",
  "This procedure sets the horizontal and vertical spacing of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_set_spacing_invoker } }
};

static Argument *
image_grid_get_offset_invoker (ProcRecord   *proc_record,
                               Gimp         *gimp,
                               GimpContext  *context,
                               GimpProgress *progress,
                               Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  GimpImage *image;
  gdouble xoffset = 0.0;
  gdouble yoffset = 0.0;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_get (grid,
                      "xoffset", &xoffset,
                      "yoffset", &yoffset,
                      NULL);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_float = xoffset;
      return_vals[2].value.pdb_float = yoffset;
    }

  return return_vals;
}

static ProcRecord image_grid_get_offset_proc =
{
  "gimp-image-grid-get-offset",
  "gimp-image-grid-get-offset",
  "Gets the offset of an image's grid.",
  "This procedure retrieves the horizontal and vertical offset of an image's grid. It takes the image as parameter.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_get_offset_invoker } }
};

static Argument *
image_grid_set_offset_invoker (ProcRecord   *proc_record,
                               Gimp         *gimp,
                               GimpContext  *context,
                               GimpProgress *progress,
                               Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gdouble xoffset;
  gdouble yoffset;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  xoffset = args[1].value.pdb_float;

  yoffset = args[2].value.pdb_float;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_set (grid,
                      "xoffset", xoffset,
                      "yoffset", yoffset,
                      NULL);
      else
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord image_grid_set_offset_proc =
{
  "gimp-image-grid-set-offset",
  "gimp-image-grid-set-offset",
  "Sets the offset of an image's grid.",
  "This procedure sets the horizontal and vertical offset of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_set_offset_invoker } }
};

static Argument *
image_grid_get_foreground_color_invoker (ProcRecord   *proc_record,
                                         Gimp         *gimp,
                                         GimpContext  *context,
                                         GimpProgress *progress,
                                         Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  GimpImage *image;
  GimpRGB fgcolor = { 0.0, 0.0, 0.0, 1.0 };

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        fgcolor = grid->fgcolor;
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_color = fgcolor;

  return return_vals;
}

static ProcRecord image_grid_get_foreground_color_proc =
{
  "gimp-image-grid-get-foreground-color",
  "gimp-image-grid-get-foreground-color",
  "Sets the foreground color of an image's grid.",
  "This procedure gets the foreground color of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_get_foreground_color_invoker } }
};

static Argument *
image_grid_set_foreground_color_invoker (ProcRecord   *proc_record,
                                         Gimp         *gimp,
                                         GimpContext  *context,
                                         GimpProgress *progress,
                                         Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  GimpRGB fgcolor;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  fgcolor = args[1].value.pdb_color;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_set (grid, "fgcolor", &fgcolor, NULL);
      else
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord image_grid_set_foreground_color_proc =
{
  "gimp-image-grid-set-foreground-color",
  "gimp-image-grid-set-foreground-color",
  "Gets the foreground color of an image's grid.",
  "This procedure sets the foreground color of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_set_foreground_color_invoker } }
};

static Argument *
image_grid_get_background_color_invoker (ProcRecord   *proc_record,
                                         Gimp         *gimp,
                                         GimpContext  *context,
                                         GimpProgress *progress,
                                         Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  GimpImage *image;
  GimpRGB bgcolor = { 0.0, 0.0, 0.0, 1.0 };

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        bgcolor = grid->bgcolor;
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_color = bgcolor;

  return return_vals;
}

static ProcRecord image_grid_get_background_color_proc =
{
  "gimp-image-grid-get-background-color",
  "gimp-image-grid-get-background-color",
  "Sets the background color of an image's grid.",
  "This procedure gets the background color of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_get_background_color_invoker } }
};

static Argument *
image_grid_set_background_color_invoker (ProcRecord   *proc_record,
                                         Gimp         *gimp,
                                         GimpContext  *context,
                                         GimpProgress *progress,
                                         Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  GimpRGB bgcolor;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  bgcolor = args[1].value.pdb_color;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_set (grid, "bgcolor", &bgcolor, NULL);
      else
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord image_grid_set_background_color_proc =
{
  "gimp-image-grid-set-background-color",
  "gimp-image-grid-set-background-color",
  "Gets the background color of an image's grid.",
  "This procedure sets the background color of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_set_background_color_invoker } }
};

static Argument *
image_grid_get_style_invoker (ProcRecord   *proc_record,
                              Gimp         *gimp,
                              GimpContext  *context,
                              GimpProgress *progress,
                              Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  GimpImage *image;
  gint32 style = 0;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_get (grid, "style", &style, NULL);
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_int = style;

  return return_vals;
}

static ProcRecord image_grid_get_style_proc =
{
  "gimp-image-grid-get-style",
  "gimp-image-grid-get-style",
  "Gets the style of an image's grid.",
  "This procedure retrieves the style of an image's grid.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_get_style_invoker } }
};

static Argument *
image_grid_set_style_invoker (ProcRecord   *proc_record,
                              Gimp         *gimp,
                              GimpContext  *context,
                              GimpProgress *progress,
                              Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gint32 style;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  style = args[1].value.pdb_int;
  if (style < GIMP_GRID_DOTS || style > GIMP_GRID_SOLID)
    success = FALSE;

  if (success)
    {
      GimpGrid *grid = gimp_image_get_grid (image);

      if (grid)
        g_object_set (grid, "style", style, NULL);
      else
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord image_grid_set_style_proc =
{
  "gimp-image-grid-set-style",
  "gimp-image-grid-set-style",
  "Sets the style unit of an image's grid.",
  "This procedure sets the style of an image's grid. It takes the image and the new style as parameters.",
  "Sylvain Foret",
  "Sylvain Foret",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { image_grid_set_style_invoker } }
};
