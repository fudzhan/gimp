/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2000 Spencer Kimball and Peter Mattis
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

#include "libgimpbase/gimpbasetypes.h"

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/core-types.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage-mask-select.h"
#include "core/gimpimage.h"

#include "libgimpcolor/gimpcolor.h"

static ProcRecord by_color_select_proc;
static ProcRecord ellipse_select_proc;
static ProcRecord free_select_proc;
static ProcRecord fuzzy_select_proc;
static ProcRecord rect_select_proc;

void
register_selection_tools_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &by_color_select_proc);
  procedural_db_register (gimp, &ellipse_select_proc);
  procedural_db_register (gimp, &free_select_proc);
  procedural_db_register (gimp, &fuzzy_select_proc);
  procedural_db_register (gimp, &rect_select_proc);
}

static Argument *
by_color_select_invoker (Gimp     *gimp,
                         Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  GimpRGB color;
  gint32 threshold;
  gint32 operation;
  gboolean antialias;
  gboolean feather;
  gdouble feather_radius;
  gboolean sample_merged;
  GimpImage *gimage;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  color = args[1].value.pdb_color;

  threshold = args[2].value.pdb_int;
  if (threshold < 0 || threshold > 255)
    success = FALSE;

  operation = args[3].value.pdb_int;
  if (operation < CHANNEL_OP_ADD || operation > CHANNEL_OP_INTERSECT)
    success = FALSE;

  antialias = args[4].value.pdb_int ? TRUE : FALSE;

  feather = args[5].value.pdb_int ? TRUE : FALSE;

  feather_radius = args[6].value.pdb_float;

  sample_merged = args[7].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      gimage = gimp_item_get_image (GIMP_ITEM (drawable));
    
      gimp_image_mask_select_by_color (gimage, drawable,
				       sample_merged,
				       &color,
				       threshold,
				       FALSE /* don't select transparent */,
				       operation,
				       antialias,
				       feather,
				       feather_radius,
				       feather_radius);
    }

  return procedural_db_return_args (&by_color_select_proc, success);
}

static ProcArg by_color_select_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_COLOR,
    "color",
    "The color to select"
  },
  {
    GIMP_PDB_INT32,
    "threshold",
    "Threshold in intensity levels 0 <= threshold <= 255"
  },
  {
    GIMP_PDB_INT32,
    "operation",
    "The selection operation: { CHANNEL_OP_ADD (0), CHANNEL_OP_SUBTRACT (1), CHANNEL_OP_REPLACE (2), CHANNEL_OP_INTERSECT (3) }"
  },
  {
    GIMP_PDB_INT32,
    "antialias",
    "Antialiasing (TRUE or FALSE)"
  },
  {
    GIMP_PDB_INT32,
    "feather",
    "Feather option for selections"
  },
  {
    GIMP_PDB_FLOAT,
    "feather_radius",
    "Radius for feather operation"
  },
  {
    GIMP_PDB_INT32,
    "sample_merged",
    "Use the composite image, not the drawable"
  }
};

static ProcRecord by_color_select_proc =
{
  "gimp_by_color_select",
  "Create a selection by selecting all pixels (in the specified drawable) with the same (or similar) color to that specified.",
  "This tool creates a selection over the specified image. A by-color selection is determined by the supplied color under the constraints of the specified threshold. Essentially, all pixels (in the drawable) that have color sufficiently close to the specified color (as determined by the threshold value) are included in the selection. The antialiasing parameter allows the final selection mask to contain intermediate values based on close misses to the threshold bar. Feathering can be enabled optionally and is controlled with the \"feather_radius\" parameter. If the sample_merged parameter is non-zero, the data of the composite image will be used instead of that for the specified drawable. This is equivalent to sampling for colors after merging all visible layers. In the case of a merged sampling, the supplied drawable is ignored.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  8,
  by_color_select_inargs,
  0,
  NULL,
  { { by_color_select_invoker } }
};

static Argument *
ellipse_select_invoker (Gimp     *gimp,
                        Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;
  gdouble x;
  gdouble y;
  gdouble width;
  gdouble height;
  gint32 operation;
  gboolean antialias;
  gboolean feather;
  gdouble feather_radius;

  gimage = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  x = args[1].value.pdb_float;

  y = args[2].value.pdb_float;

  width = args[3].value.pdb_float;
  if (width <= 0.0)
    success = FALSE;

  height = args[4].value.pdb_float;
  if (height <= 0.0)
    success = FALSE;

  operation = args[5].value.pdb_int;
  if (operation < CHANNEL_OP_ADD || operation > CHANNEL_OP_INTERSECT)
    success = FALSE;

  antialias = args[6].value.pdb_int ? TRUE : FALSE;

  feather = args[7].value.pdb_int ? TRUE : FALSE;

  feather_radius = args[8].value.pdb_float;

  if (success)
    gimp_image_mask_select_ellipse (gimage,
				    (gint) x, (gint) y,
				    (gint) width, (gint) height,
				    operation,
				    antialias,
				    feather,
				    feather_radius,
				    feather_radius);

  return procedural_db_return_args (&ellipse_select_proc, success);
}

static ProcArg ellipse_select_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_FLOAT,
    "x",
    "x coordinate of upper-left corner of ellipse bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "y",
    "y coordinate of upper-left corner of ellipse bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "width",
    "The width of the ellipse: 0 < width"
  },
  {
    GIMP_PDB_FLOAT,
    "height",
    "The height of the ellipse: 0 < height"
  },
  {
    GIMP_PDB_INT32,
    "operation",
    "The selection operation: { CHANNEL_OP_ADD (0), CHANNEL_OP_SUBTRACT (1), CHANNEL_OP_REPLACE (2), CHANNEL_OP_INTERSECT (3) }"
  },
  {
    GIMP_PDB_INT32,
    "antialias",
    "Antialiasing (TRUE or FALSE)"
  },
  {
    GIMP_PDB_INT32,
    "feather",
    "Feather option for selections"
  },
  {
    GIMP_PDB_FLOAT,
    "feather_radius",
    "Radius for feather operation"
  }
};

static ProcRecord ellipse_select_proc =
{
  "gimp_ellipse_select",
  "Create an elliptical selection over the specified image.",
  "This tool creates an elliptical selection over the specified image. The elliptical region can be either added to, subtracted from, or replace the contents of the previous selection mask. If antialiasing is turned on, the edges of the elliptical region will contain intermediate values which give the appearance of a sharper, less pixelized edge. This should be set as TRUE most of the time. If the feather option is enabled, the resulting selection is blurred before combining. The blur is a gaussian blur with the specified feather radius.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  9,
  ellipse_select_inargs,
  0,
  NULL,
  { { ellipse_select_invoker } }
};

static Argument *
free_select_invoker (Gimp     *gimp,
                     Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;
  gint32 num_segs;
  gdouble *segs;
  gint32 operation;
  gboolean antialias;
  gboolean feather;
  gdouble feather_radius;

  gimage = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  num_segs = args[1].value.pdb_int;
  if (!(num_segs < 2))
    num_segs /= 2;
  else
    success = FALSE;

  segs = (gdouble *) args[2].value.pdb_pointer;

  operation = args[3].value.pdb_int;
  if (operation < CHANNEL_OP_ADD || operation > CHANNEL_OP_INTERSECT)
    success = FALSE;

  antialias = args[4].value.pdb_int ? TRUE : FALSE;

  feather = args[5].value.pdb_int ? TRUE : FALSE;

  feather_radius = args[6].value.pdb_float;

  if (success)
    gimp_image_mask_select_polygon (gimage,
				    num_segs,
				    (GimpVector2 *) segs, 
				    operation,
				    antialias,
				    feather, 
				    feather_radius,
				    feather_radius);

  return procedural_db_return_args (&free_select_proc, success);
}

static ProcArg free_select_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_INT32,
    "num_segs",
    "Number of points (count 1 coordinate as two points)"
  },
  {
    GIMP_PDB_FLOATARRAY,
    "segs",
    "Array of points: { p1.x, p1.y, p2.x, p2.y, ..., pn.x, pn.y}"
  },
  {
    GIMP_PDB_INT32,
    "operation",
    "The selection operation: { CHANNEL_OP_ADD (0), CHANNEL_OP_SUBTRACT (1), CHANNEL_OP_REPLACE (2), CHANNEL_OP_INTERSECT (3) }"
  },
  {
    GIMP_PDB_INT32,
    "antialias",
    "Antialiasing (TRUE or FALSE)"
  },
  {
    GIMP_PDB_INT32,
    "feather",
    "Feather option for selections"
  },
  {
    GIMP_PDB_FLOAT,
    "feather_radius",
    "Radius for feather operation"
  }
};

static ProcRecord free_select_proc =
{
  "gimp_free_select",
  "Create a polygonal selection over the specified image.",
  "This tool creates a polygonal selection over the specified image. The polygonal region can be either added to, subtracted from, or replace the contents of the previous selection mask. The polygon is specified through an array of floating point numbers and its length. The length of array must be 2n, where n is the number of points. Each point is defined by 2 floating point values which correspond to the x and y coordinates. If the final point does not connect to the starting point, a connecting segment is automatically added. If the feather option is enabled, the resulting selection is blurred before combining. The blur is a gaussian blur with the specified feather radius.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  7,
  free_select_inargs,
  0,
  NULL,
  { { free_select_invoker } }
};

static Argument *
fuzzy_select_invoker (Gimp     *gimp,
                      Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gdouble x;
  gdouble y;
  gint32 threshold;
  gint32 operation;
  gboolean antialias;
  gboolean feather;
  gdouble feather_radius;
  gboolean sample_merged;
  GimpImage *gimage;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  x = args[1].value.pdb_float;

  y = args[2].value.pdb_float;

  threshold = args[3].value.pdb_int;
  if (threshold < 0 || threshold > 255)
    success = FALSE;

  operation = args[4].value.pdb_int;
  if (operation < CHANNEL_OP_ADD || operation > CHANNEL_OP_INTERSECT)
    success = FALSE;

  antialias = args[5].value.pdb_int ? TRUE : FALSE;

  feather = args[6].value.pdb_int ? TRUE : FALSE;

  feather_radius = args[7].value.pdb_float;

  sample_merged = args[8].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      gimage = gimp_item_get_image (GIMP_ITEM (drawable));
    
      gimp_image_mask_select_fuzzy (gimage,
				    drawable,
				    sample_merged,
				    x, y, 
				    threshold,
				    FALSE /* don't select transparent */,
				    operation,
				    antialias,
				    feather,
				    feather_radius,
				    feather_radius);
    }

  return procedural_db_return_args (&fuzzy_select_proc, success);
}

static ProcArg fuzzy_select_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "x",
    "x coordinate of initial seed fill point: (image coordinates)"
  },
  {
    GIMP_PDB_FLOAT,
    "y",
    "y coordinate of initial seed fill point: (image coordinates)"
  },
  {
    GIMP_PDB_INT32,
    "threshold",
    "Threshold in intensity levels 0 <= threshold <= 255"
  },
  {
    GIMP_PDB_INT32,
    "operation",
    "The selection operation: { CHANNEL_OP_ADD (0), CHANNEL_OP_SUBTRACT (1), CHANNEL_OP_REPLACE (2), CHANNEL_OP_INTERSECT (3) }"
  },
  {
    GIMP_PDB_INT32,
    "antialias",
    "Antialiasing (TRUE or FALSE)"
  },
  {
    GIMP_PDB_INT32,
    "feather",
    "Feather option for selections"
  },
  {
    GIMP_PDB_FLOAT,
    "feather_radius",
    "Radius for feather operation"
  },
  {
    GIMP_PDB_INT32,
    "sample_merged",
    "Use the composite image, not the drawable"
  }
};

static ProcRecord fuzzy_select_proc =
{
  "gimp_fuzzy_select",
  "Create a fuzzy selection starting at the specified coordinates on the specified drawable.",
  "This tool creates a fuzzy selection over the specified image. A fuzzy selection is determined by a seed fill under the constraints of the specified threshold. Essentially, the color at the specified coordinates (in the drawable) is measured and the selection expands outwards from that point to any adjacent pixels which are not significantly different (as determined by the threshold value). This process continues until no more expansion is possible. The antialiasing parameter allows the final selection mask to contain intermediate values based on close misses to the threshold bar at pixels along the seed fill boundary. Feathering can be enabled optionally and is controlled with the \"feather_radius\" paramter. If the sample_merged parameter is non-zero, the data of the composite image will be used instead of that for the specified drawable. This is equivalent to sampling for colors after merging all visible layers. In the case of a merged sampling, the supplied drawable is ignored."
  "If the sample is merged, the specified coordinates are relative to the image origin; otherwise, they are relative to the drawable's origin.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  9,
  fuzzy_select_inargs,
  0,
  NULL,
  { { fuzzy_select_invoker } }
};

static Argument *
rect_select_invoker (Gimp     *gimp,
                     Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;
  gdouble x;
  gdouble y;
  gdouble width;
  gdouble height;
  gint32 operation;
  gboolean feather;
  gdouble feather_radius;

  gimage = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  x = args[1].value.pdb_float;

  y = args[2].value.pdb_float;

  width = args[3].value.pdb_float;
  if (width <= 0.0)
    success = FALSE;

  height = args[4].value.pdb_float;
  if (height <= 0.0)
    success = FALSE;

  operation = args[5].value.pdb_int;
  if (operation < CHANNEL_OP_ADD || operation > CHANNEL_OP_INTERSECT)
    success = FALSE;

  feather = args[6].value.pdb_int ? TRUE : FALSE;

  feather_radius = args[7].value.pdb_float;

  if (success)
    gimp_image_mask_select_rectangle (gimage,
				      (gint) x, (gint) y,
				      (gint) width, (gint) height,
				      operation,
				      feather,
				      feather_radius,
				      feather_radius);

  return procedural_db_return_args (&rect_select_proc, success);
}

static ProcArg rect_select_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_FLOAT,
    "x",
    "x coordinate of upper-left corner of rectangle"
  },
  {
    GIMP_PDB_FLOAT,
    "y",
    "y coordinate of upper-left corner of rectangle"
  },
  {
    GIMP_PDB_FLOAT,
    "width",
    "The width of the rectangle: 0 < width"
  },
  {
    GIMP_PDB_FLOAT,
    "height",
    "The height of the rectangle: 0 < height"
  },
  {
    GIMP_PDB_INT32,
    "operation",
    "The selection operation: { CHANNEL_OP_ADD (0), CHANNEL_OP_SUBTRACT (1), CHANNEL_OP_REPLACE (2), CHANNEL_OP_INTERSECT (3) }"
  },
  {
    GIMP_PDB_INT32,
    "feather",
    "Feather option for selections"
  },
  {
    GIMP_PDB_FLOAT,
    "feather_radius",
    "Radius for feather operation"
  }
};

static ProcRecord rect_select_proc =
{
  "gimp_rect_select",
  "Create a rectangular selection over the specified image;",
  "This tool creates a rectangular selection over the specified image. The rectangular region can be either added to, subtracted from, or replace the contents of the previous selection mask. If the feather option is enabled, the resulting selection is blurred before combining. The blur is a gaussian blur with the specified feather radius.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  8,
  rect_select_inargs,
  0,
  NULL,
  { { rect_select_invoker } }
};
