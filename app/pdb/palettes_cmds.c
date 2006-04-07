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
#include "gimp-pdb.h"
#include "gimpprocedure.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"
#include "core/gimpcontainer-filter.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"
#include "core/gimppalette.h"


static GValueArray *
palettes_refresh_invoker (GimpProcedure     *procedure,
                          Gimp              *gimp,
                          GimpContext       *context,
                          GimpProgress      *progress,
                          const GValueArray *args)
{
  gimp_data_factory_data_refresh (gimp->palette_factory);
  return gimp_procedure_get_return_values (procedure, TRUE);
}

static GValueArray *
palettes_get_list_invoker (GimpProcedure     *procedure,
                           Gimp              *gimp,
                           GimpContext       *context,
                           GimpProgress      *progress,
                           const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  const gchar *filter;
  gint32 num_palettes = 0;
  gchar **palette_list = NULL;

  filter = g_value_get_string (&args->values[0]);

  if (success)
    {
      palette_list = gimp_container_get_filtered_name_array (gimp->palette_factory->container,
                                                             filter, &num_palettes);
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    {
      g_value_set_int (&return_vals->values[1], num_palettes);
      gimp_value_take_stringarray (&return_vals->values[2], palette_list, num_palettes);
    }

  return return_vals;
}

static GValueArray *
palettes_get_palette_invoker (GimpProcedure     *procedure,
                              Gimp              *gimp,
                              GimpContext       *context,
                              GimpProgress      *progress,
                              const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
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

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    {
      g_value_take_string (&return_vals->values[1], name);
      g_value_set_int (&return_vals->values[2], num_colors);
    }

  return return_vals;
}

static GValueArray *
palettes_get_palette_entry_invoker (GimpProcedure     *procedure,
                                    Gimp              *gimp,
                                    GimpContext       *context,
                                    GimpProgress      *progress,
                                    const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  const gchar *name;
  gint32 entry_num;
  gchar *actual_name = NULL;
  gint32 num_colors = 0;
  GimpRGB color = { 0.0, 0.0, 0.0, 1.0 };

  name = g_value_get_string (&args->values[0]);
  entry_num = g_value_get_int (&args->values[1]);

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

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    {
      g_value_take_string (&return_vals->values[1], actual_name);
      g_value_set_int (&return_vals->values[2], num_colors);
      gimp_value_set_rgb (&return_vals->values[3], &color);
    }

  return return_vals;
}

void
register_palettes_procs (Gimp *gimp)
{
  GimpProcedure *procedure;

  /*
   * gimp-palettes-refresh
   */
  procedure = gimp_procedure_new (palettes_refresh_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-palettes-refresh");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-palettes-refresh",
                                     "Refreshes current palettes. This function always succeeds.",
                                     "This procedure retrieves all palettes currently in the user's palette path and updates the palette dialogs accordingly.",
                                     "Adrian Likins <adrian@gimp.org>",
                                     "Adrian Likins",
                                     "1998",
                                     NULL);

  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-palettes-get-list
   */
  procedure = gimp_procedure_new (palettes_get_list_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-palettes-get-list");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-palettes-get-list",
                                     "Retrieves a list of all of the available palettes",
                                     "This procedure returns a complete listing of available palettes. Each name returned can be used as input to the command 'gimp-context-set-palette'.",
                                     "Nathan Summers <rock@gimp.org>",
                                     "Nathan Summers",
                                     "2001",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("filter",
                                                       "filter",
                                                       "An optional regular expression used to filter the list",
                                                       FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("num-palettes",
                                                          "num palettes",
                                                          "The number of palettes in the list",
                                                          0, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_string_array ("palette-list",
                                                                 "palette list",
                                                                 "The list of palette names",
                                                                 GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-palettes-get-palette
   */
  procedure = gimp_procedure_new (palettes_get_palette_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-palettes-get-palette");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-palettes-get-palette",
                                     "This procedure is deprecated! Use 'gimp-context-get-palette' instead.",
                                     "This procedure is deprecated! Use 'gimp-context-get-palette' instead.",
                                     "",
                                     "",
                                     "",
                                     "gimp-context-get-palette");

  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_string ("name",
                                                           "name",
                                                           "The palette name",
                                                           FALSE, TRUE,
                                                           NULL,
                                                           GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("num-colors",
                                                          "num colors",
                                                          "The palette num_colors",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-palettes-get-palette-entry
   */
  procedure = gimp_procedure_new (palettes_get_palette_entry_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-palettes-get-palette-entry");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-palettes-get-palette-entry",
                                     "This procedure is deprecated! Use 'gimp-palette-entry-get-color' instead.",
                                     "This procedure is deprecated! Use 'gimp-palette-entry-get-color' instead.",
                                     "",
                                     "",
                                     "",
                                     "gimp-palette-entry-get-color");

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("name",
                                                       "name",
                                                       "The palette name (\"\" means currently active palette)",
                                                       FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_int32 ("entry-num",
                                                      "entry num",
                                                      "The entry to retrieve",
                                                      G_MININT32, G_MAXINT32, 0,
                                                      GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_string ("actual-name",
                                                           "actual name",
                                                           "The palette name",
                                                           FALSE, FALSE,
                                                           NULL,
                                                           GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("num-colors",
                                                          "num colors",
                                                          "The palette num_colors",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_rgb ("color",
                                                        "color",
                                                        "The color requested",
                                                        NULL,
                                                        GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

}
