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

#include "libgimpcolor/gimpcolor.h"

#include "pdb-types.h"
#include "procedural_db.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"
#include "core/gimpcontainer-filter.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"
#include "core/gimppalette.h"

static ProcRecord palettes_refresh_proc;
static ProcRecord palettes_get_list_proc;
static ProcRecord palettes_get_palette_proc;
static ProcRecord palettes_get_palette_entry_proc;

void
register_palettes_procs (Gimp *gimp)
{
  /*
   * palettes_refresh
   */
  procedural_db_init_proc (&palettes_refresh_proc, 0, 0);
  procedural_db_register (gimp, &palettes_refresh_proc);

  /*
   * palettes_get_list
   */
  procedural_db_init_proc (&palettes_get_list_proc, 1, 2);
  procedural_db_add_argument (&palettes_get_list_proc,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("filter",
                                                      "filter",
                                                      "An optional regular expression used to filter the list",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&palettes_get_list_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("num-palettes",
                                                    "num palettes",
                                                    "The number of palettes in the list",
                                                    0, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&palettes_get_list_proc,
                                  GIMP_PDB_STRINGARRAY,
                                  g_param_spec_pointer ("palette-list",
                                                        "palette list",
                                                        "The list of palette names",
                                                        GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &palettes_get_list_proc);

  /*
   * palettes_get_palette
   */
  procedural_db_init_proc (&palettes_get_palette_proc, 0, 2);
  procedural_db_add_return_value (&palettes_get_palette_proc,
                                  GIMP_PDB_STRING,
                                  gimp_param_spec_string ("name",
                                                          "name",
                                                          "The palette name",
                                                          FALSE, TRUE,
                                                          NULL,
                                                          GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&palettes_get_palette_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("num-colors",
                                                    "num colors",
                                                    "The palette num_colors",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &palettes_get_palette_proc);

  /*
   * palettes_get_palette_entry
   */
  procedural_db_init_proc (&palettes_get_palette_entry_proc, 2, 3);
  procedural_db_add_argument (&palettes_get_palette_entry_proc,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("name",
                                                      "name",
                                                      "The palette name (\"\" means currently active palette)",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (&palettes_get_palette_entry_proc,
                              GIMP_PDB_INT32,
                              g_param_spec_int ("entry-num",
                                                "entry num",
                                                "The entry to retrieve",
                                                G_MININT32, G_MAXINT32, 0,
                                                GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&palettes_get_palette_entry_proc,
                                  GIMP_PDB_STRING,
                                  gimp_param_spec_string ("actual-name",
                                                          "actual name",
                                                          "The palette name",
                                                          FALSE, FALSE,
                                                          NULL,
                                                          GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&palettes_get_palette_entry_proc,
                                  GIMP_PDB_INT32,
                                  g_param_spec_int ("num-colors",
                                                    "num colors",
                                                    "The palette num_colors",
                                                    G_MININT32, G_MAXINT32, 0,
                                                    GIMP_PARAM_READWRITE));
  procedural_db_add_return_value (&palettes_get_palette_entry_proc,
                                  GIMP_PDB_COLOR,
                                  gimp_param_spec_rgb ("color",
                                                       "color",
                                                       "The color requested",
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, &palettes_get_palette_entry_proc);

}

static Argument *
palettes_refresh_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gimp_data_factory_data_refresh (gimp->palette_factory);
  return procedural_db_return_values (proc_record, TRUE);
}

static ProcRecord palettes_refresh_proc =
{
  "gimp-palettes-refresh",
  "gimp-palettes-refresh",
  "Refreshes current palettes. This function always succeeds.",
  "This procedure retrieves all palettes currently in the user's palette path and updates the palette dialogs accordingly.",
  "Adrian Likins <adrian@gimp.org>",
  "Adrian Likins",
  "1998",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_refresh_invoker } }
};

static Argument *
palettes_get_list_invoker (ProcRecord   *proc_record,
                           Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *filter;
  gint32 num_palettes = 0;
  gchar **palette_list = NULL;

  filter = (gchar *) args[0].value.pdb_pointer;
  if (filter && !g_utf8_validate (filter, -1, NULL))
    success = FALSE;

  if (success)
    {
      palette_list = gimp_container_get_filtered_name_array (gimp->palette_factory->container,
                                                             filter, &num_palettes);
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_int = num_palettes;
      return_vals[2].value.pdb_pointer = palette_list;
    }

  return return_vals;
}

static ProcRecord palettes_get_list_proc =
{
  "gimp-palettes-get-list",
  "gimp-palettes-get-list",
  "Retrieves a list of all of the available palettes",
  "This procedure returns a complete listing of available palettes. Each name returned can be used as input to the command 'gimp-context-set-palette'.",
  "Nathan Summers <rock@gimp.org>",
  "Nathan Summers",
  "2001",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_get_list_invoker } }
};

static Argument *
palettes_get_palette_invoker (ProcRecord   *proc_record,
                              Gimp         *gimp,
                              GimpContext  *context,
                              GimpProgress *progress,
                              Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *name = NULL;
  gint32 num_colors = 0;

  GimpPalette *palette = gimp_context_get_palette (context);

  if (palette)
    {
      name       = g_strdup (gimp_object_get_name (GIMP_OBJECT (palette)));
      num_colors = palette->n_colors;
    }
  else
    success = FALSE;

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_pointer = name;
      return_vals[2].value.pdb_int = num_colors;
    }

  return return_vals;
}

static ProcRecord palettes_get_palette_proc =
{
  "gimp-palettes-get-palette",
  "gimp-palettes-get-palette",
  "This procedure is deprecated! Use 'gimp-context-get-palette' instead.",
  "This procedure is deprecated! Use 'gimp-context-get-palette' instead.",
  "",
  "",
  "",
  "gimp-context-get-palette",
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_get_palette_invoker } }
};

static Argument *
palettes_get_palette_entry_invoker (ProcRecord   *proc_record,
                                    Gimp         *gimp,
                                    GimpContext  *context,
                                    GimpProgress *progress,
                                    Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_vals;
  gchar *name;
  gint32 entry_num;
  gchar *actual_name = NULL;
  gint32 num_colors = 0;
  GimpRGB color = { 0.0, 0.0, 0.0, 1.0 };

  name = (gchar *) args[0].value.pdb_pointer;
  if (name && !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  if (success)
    {
      GimpPalette *palette;

      if (name && strlen (name))
        {
          palette = (GimpPalette *)
            gimp_container_get_child_by_name (gimp->palette_factory->container,
                                              name);
        }
      else
        {
          palette = gimp_context_get_palette (context);
        }

      if (palette)
        {
          if (entry_num >= 0 && entry_num < palette->n_colors)
            {
              GimpPaletteEntry *entry = g_list_nth_data (palette->colors, entry_num);

              actual_name = g_strdup (gimp_object_get_name (GIMP_OBJECT (palette)));
              num_colors  = palette->n_colors;
              color       = entry->color;
            }
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return_vals = procedural_db_return_values (proc_record, success);

  if (success)
    {
      return_vals[1].value.pdb_pointer = actual_name;
      return_vals[2].value.pdb_int = num_colors;
      return_vals[3].value.pdb_color = color;
    }

  return return_vals;
}

static ProcRecord palettes_get_palette_entry_proc =
{
  "gimp-palettes-get-palette-entry",
  "gimp-palettes-get-palette-entry",
  "This procedure is deprecated! Use 'gimp-palette-entry-get-color' instead.",
  "This procedure is deprecated! Use 'gimp-palette-entry-get-color' instead.",
  "",
  "",
  "",
  "gimp-palette-entry-get-color",
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_get_palette_entry_invoker } }
};
