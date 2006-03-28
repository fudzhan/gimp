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

#include <string.h>

#include <glib-object.h>

#include "pdb-types.h"
#include "procedural_db.h"
#include "core/gimpparamspecs.h"

#include "base/temp-buf.h"
#include "core/gimp.h"
#include "core/gimpbrush.h"
#include "core/gimpcontainer-filter.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"

static ProcRecord brushes_refresh_proc;
static ProcRecord brushes_get_list_proc;
static ProcRecord brushes_get_brush_proc;
static ProcRecord brushes_get_spacing_proc;
static ProcRecord brushes_set_spacing_proc;
static ProcRecord brushes_get_brush_data_proc;

void
register_brushes_procs (Gimp *gimp)
{
  /*
   * brushes_refresh
   */
  procedural_db_init_proc (&brushes_refresh_proc, 0, 0);
  procedural_db_register (gimp, &brushes_refresh_proc);

  /*
   * brushes_get_list
   */
  procedural_db_init_proc (&brushes_get_list_proc, 1, 2);
  procedural_db_add_argument (&brushes_get_list_proc,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("filter",
                                                      "filter",
                                                      "An optional regular expression used to filter the list",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_list_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("num-brushes",
                                                    "num brushes",
                                                    "The number of brushes in the brush list",
                                                    0, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_list_proc,
                                  GIMP_PDB_STRINGARRAY,
                                  g_param_spec_pointer ("brush-list",
                                                        "brush list",
                                                        "The list of brush names",
                                                        GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &brushes_get_list_proc);

  /*
   * brushes_get_brush
   */
  procedural_db_init_proc (&brushes_get_brush_proc, 0, 4);
  procedural_db_add_return_value (&brushes_get_brush_proc,
                                  GIMP_PDB_STRING,
                                  gimp_param_spec_string ("name",
                                                          "name",
                                                          "The brush name",
                                                          FALSE, FALSE,
                                                          NULL,
                                                          GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("width",
                                                    "width",
                                                    "The brush width",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("height",
                                                    "height",
                                                    "The brush height",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("spacing",
                                                    "spacing",
                                                    "The brush spacing (0 <= spacing <= 1000)",
                                                    0, 1000, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &brushes_get_brush_proc);

  /*
   * brushes_get_spacing
   */
  procedural_db_init_proc (&brushes_get_spacing_proc, 0, 1);
  procedural_db_add_return_value (&brushes_get_spacing_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("spacing",
                                                    "spacing",
                                                    "The brush spacing (0 <= spacing <= 1000)",
                                                    0, 1000, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &brushes_get_spacing_proc);

  /*
   * brushes_set_spacing
   */
  procedural_db_init_proc (&brushes_set_spacing_proc, 1, 0);
  procedural_db_add_argument (&brushes_set_spacing_proc,
                              GIMP_PDB_INT32,
                              g_param_spec_int ("spacing",
                                                "spacing",
                                                "The brush spacing (0 <= spacing <= 1000)",
                                                0, 1000, 0,
                                                GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &brushes_set_spacing_proc);

  /*
   * brushes_get_brush_data
   */
  procedural_db_init_proc (&brushes_get_brush_data_proc, 1, 8);
  procedural_db_add_argument (&brushes_get_brush_data_proc,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("name",
                                                      "name",
                                                      "The brush name (\"\" means current active brush)",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_STRING,
                                  gimp_param_spec_string ("actual-name",
                                                          "actual name",
                                                          "The brush name",
                                                          FALSE, FALSE,
                                                          NULL,
                                                          GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_FLOAT,
                                  g_param_spec_double ("opacity",
                                                       "opacity",
                                                       "The brush opacity (0 <= opacity <= 100)",
                                                       0, 100, 0,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("spacing",
                                                    "spacing",
                                                    "The brush spacing (0 <= spacing <= 1000)",
                                                    0, 1000, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_enum ("paint-mode",
                                                     "paint mode",
                                                     "The paint mode: { GIMP_NORMAL_MODE (0), GIMP_DISSOLVE_MODE (1), GIMP_BEHIND_MODE (2), GIMP_MULTIPLY_MODE (3), GIMP_SCREEN_MODE (4), GIMP_OVERLAY_MODE (5), GIMP_DIFFERENCE_MODE (6), GIMP_ADDITION_MODE (7), GIMP_SUBTRACT_MODE (8), GIMP_DARKEN_ONLY_MODE (9), GIMP_LIGHTEN_ONLY_MODE (10), GIMP_HUE_MODE (11), GIMP_SATURATION_MODE (12), GIMP_COLOR_MODE (13), GIMP_VALUE_MODE (14), GIMP_DIVIDE_MODE (15), GIMP_DODGE_MODE (16), GIMP_BURN_MODE (17), GIMP_HARDLIGHT_MODE (18), GIMP_SOFTLIGHT_MODE (19), GIMP_GRAIN_EXTRACT_MODE (20), GIMP_GRAIN_MERGE_MODE (21), GIMP_COLOR_ERASE_MODE (22) }",
                                                     GIMP_TYPE_LAYER_MODE_EFFECTS,
                                                     GIMP_NORMAL_MODE,
                                                     GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("width",
                                                    "width",
                                                    "The brush width",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("height",
                                                    "height",
                                                    "The brush height",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("length",
                                                    "length",
                                                    "Length of brush mask data",
                                                    0, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&brushes_get_brush_data_proc,
                                  GIMP_PDB_INT8ARRAY,
                                  g_param_spec_pointer ("mask-data",
                                                        "mask data",
                                                        "The brush mask data",
                                                        GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &brushes_get_brush_data_proc);

}

static Argument *
brushes_refresh_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gimp_data_factory_data_refresh (gimp->brush_factory);
  return procedural_db_return_values (proc_record, TRUE);
}

static ProcRecord brushes_refresh_proc =
{
  "gimp-brushes-refresh",
  "gimp-brushes-refresh",
  "Refresh current brushes. This function always succeeds.",
  "This procedure retrieves all brushes currently in the user's brush path and updates the brush dialogs accordingly.",
  "Seth Burgess",
  "Seth Burgess",
  "1997",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { brushes_refresh_invoker } }
};

static Argument *
brushes_get_list_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *filter;
  gint32 num_brushes = 0;
  gchar **brush_list = NULL;

  filter = (gchar *) args[0].value.pdb_pointer;
  if (filter && !g_utf8_validate (filter, -1, NULL))
    success = FALSE;

  if (success)
    {
      brush_list = gimp_container_get_filtered_name_array (gimp->brush_factory->container,
                                                           filter, &num_brushes);
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_int = num_brushes;
      return_vals[2].value.pdb_pointer = brush_list;
    }

  return return_vals;
}

static ProcRecord brushes_get_list_proc =
{
  "gimp-brushes-get-list",
  "gimp-brushes-get-list",
  "Retrieve a complete listing of the available brushes.",
  "This procedure returns a complete listing of available GIMP brushes. Each name returned can be used as input to the 'gimp-context-set-brush' procedure.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { brushes_get_list_invoker } }
};

static Argument *
brushes_get_brush_invoker (ProcRecord   *proc_record,
                           Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *name = NULL;
  gint32 width = 0;
  gint32 height = 0;
  gint32 spacing = 0;

  GimpBrush *brush = gimp_context_get_brush (context);

  if (brush)
    {
      name    = g_strdup (gimp_object_get_name (GIMP_OBJECT (brush)));
      width   = brush->mask->width;
      height  = brush->mask->height;
      spacing = gimp_brush_get_spacing (brush);
    }
  else
    success = FALSE;

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_pointer = name;
      return_vals[2].value.pdb_int = width;
      return_vals[3].value.pdb_int = height;
      return_vals[4].value.pdb_int = spacing;
    }

  return return_vals;
}

static ProcRecord brushes_get_brush_proc =
{
  "gimp-brushes-get-brush",
  "gimp-brushes-get-brush",
  "This procedure is deprecated! Use 'gimp-context-get-brush' instead.",
  "This procedure is deprecated! Use 'gimp-context-get-brush' instead.",
  "",
  "",
  "",
  "gimp-context-get-brush",
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { brushes_get_brush_invoker } }
};

static Argument *
brushes_get_spacing_invoker (ProcRecord   *proc_record,
                             Gimp         *gimp,
                             GimpContext  *context,
                             GimpProgress *progress,
                             Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gint32 spacing = 0;

  GimpBrush *brush = gimp_context_get_brush (context);

  if (brush)
    spacing = gimp_brush_get_spacing (brush);
  else
    success = FALSE;

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    return_vals[1].value.pdb_int = spacing;

  return return_vals;
}

static ProcRecord brushes_get_spacing_proc =
{
  "gimp-brushes-get-spacing",
  "gimp-brushes-get-spacing",
  "This procedure is deprecated! Use 'gimp-brush-get-spacing' instead.",
  "This procedure is deprecated! Use 'gimp-brush-get-spacing' instead.",
  "",
  "",
  "",
  "gimp-brush-get-spacing",
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { brushes_get_spacing_invoker } }
};

static Argument *
brushes_set_spacing_invoker (ProcRecord   *proc_record,
                             Gimp         *gimp,
                             GimpContext  *context,
                             GimpProgress *progress,
                             Argument     *args)
{
  gboolean success = TRUE;
  gint32 spacing;

  spacing = args[0].value.pdb_int;
  if (spacing < 0 || spacing > 1000)
    success = FALSE;

  if (success)
    {
      gimp_brush_set_spacing (gimp_context_get_brush (context), spacing);
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord brushes_set_spacing_proc =
{
  "gimp-brushes-set-spacing",
  "gimp-brushes-set-spacing",
  "This procedure is deprecated! Use 'gimp-brush-set-spacing' instead.",
  "This procedure is deprecated! Use 'gimp-brush-set-spacing' instead.",
  "",
  "",
  "",
  "gimp-brush-set-spacing",
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { brushes_set_spacing_invoker } }
};

static Argument *
brushes_get_brush_data_invoker (ProcRecord   *proc_record,
                                Gimp         *gimp,
                                GimpContext  *context,
                                GimpProgress *progress,
                                Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *name;
  gchar *actual_name = NULL;
  gdouble opacity = 0.0;
  gint32 spacing = 0;
  gint32 paint_mode = 0;
  gint32 width = 0;
  gint32 height = 0;
  gint32 length = 0;
  guint8 *mask_data = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name && !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBrush *brush;

      if (name && strlen (name))
        {
          brush = (GimpBrush *)
            gimp_container_get_child_by_name (gimp->brush_factory->container, name);
        }
      else
        {
          brush = gimp_context_get_brush (context);
        }

      if (brush)
        {
          actual_name = g_strdup (gimp_object_get_name (GIMP_OBJECT (brush)));
          opacity     = 1.0;
          spacing     = gimp_brush_get_spacing (brush);
          paint_mode  = 0;
          width       = brush->mask->width;
          height      = brush->mask->height;
          length      = brush->mask->height * brush->mask->width;
          mask_data   = g_memdup (temp_buf_data (brush->mask), length);
        }
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_pointer = actual_name;
      return_vals[2].value.pdb_float = opacity;
      return_vals[3].value.pdb_int = spacing;
      return_vals[4].value.pdb_int = paint_mode;
      return_vals[5].value.pdb_int = width;
      return_vals[6].value.pdb_int = height;
      return_vals[7].value.pdb_int = length;
      return_vals[8].value.pdb_pointer = mask_data;
    }

  return return_vals;
}

static ProcRecord brushes_get_brush_data_proc =
{
  "gimp-brushes-get-brush-data",
  "gimp-brushes-get-brush-data",
  "This procedure is deprecated! Use 'gimp-brush-get-pixels' instead.",
  "This procedure is deprecated! Use 'gimp-brush-get-pixels' instead.",
  "",
  "",
  "",
  "gimp-brush-get-pixels",
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { brushes_get_brush_data_invoker } }
};
