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
#include "gimpprocedure.h"
#include "procedural_db.h"
#include "core/gimpparamspecs.h"

#include "base/temp-buf.h"
#include "core/gimp.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"
#include "core/gimppattern.h"

static GimpProcedure pattern_get_info_proc;
static GimpProcedure pattern_get_pixels_proc;

void
register_pattern_procs (Gimp *gimp)
{
  GimpProcedure *procedure;

  /*
   * pattern_get_info
   */
  procedure = gimp_procedure_init (&pattern_get_info_proc, 1, 3);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("name",
                                                       "name",
                                                       "The pattern name.",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("width",
                                                          "width",
                                                          "The pattern width",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("height",
                                                          "height",
                                                          "The pattern height",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("bpp",
                                                          "bpp",
                                                          "The pattern bpp",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * pattern_get_pixels
   */
  procedure = gimp_procedure_init (&pattern_get_pixels_proc, 1, 5);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("name",
                                                       "name",
                                                       "The pattern name.",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("width",
                                                          "width",
                                                          "The pattern width",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("height",
                                                          "height",
                                                          "The pattern height",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("bpp",
                                                          "bpp",
                                                          "The pattern bpp",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("num-color-bytes",
                                                          "num color bytes",
                                                          "Number of pattern bytes",
                                                          0, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int8_array ("color-bytes",
                                                               "color bytes",
                                                               "The pattern data.",
                                                               GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

}

static GValueArray *
pattern_get_info_invoker (GimpProcedure     *procedure,
                          Gimp              *gimp,
                          GimpContext       *context,
                          GimpProgress      *progress,
                          const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  const gchar *name;
  gint32 width = 0;
  gint32 height = 0;
  gint32 bpp = 0;

  name = g_value_get_string (&args->values[0]);

  if (success)
    {
      GimpPattern *pattern = (GimpPattern *)
        gimp_container_get_child_by_name (gimp->pattern_factory->container, name);

      if (pattern)
        {
          width  = pattern->mask->width;
          height = pattern->mask->height;
          bpp    = pattern->mask->bytes;
        }
      else
        success = FALSE;
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    {
      g_value_set_int (&return_vals->values[1], width);
      g_value_set_int (&return_vals->values[2], height);
      g_value_set_int (&return_vals->values[3], bpp);
    }

  return return_vals;
}

static GimpProcedure pattern_get_info_proc =
{
  TRUE, TRUE,
  "gimp-pattern-get-info",
  "gimp-pattern-get-info",
  "Retrieve information about the specified pattern.",
  "This procedure retrieves information about the specified pattern. This includes the pattern extents (width and height).",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { pattern_get_info_invoker } }
};

static GValueArray *
pattern_get_pixels_invoker (GimpProcedure     *procedure,
                            Gimp              *gimp,
                            GimpContext       *context,
                            GimpProgress      *progress,
                            const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  const gchar *name;
  gint32 width = 0;
  gint32 height = 0;
  gint32 bpp = 0;
  gint32 num_color_bytes = 0;
  guint8 *color_bytes = NULL;

  name = g_value_get_string (&args->values[0]);

  if (success)
    {
      GimpPattern *pattern = (GimpPattern *)
        gimp_container_get_child_by_name (gimp->pattern_factory->container, name);

      if (pattern)
        {
          width           = pattern->mask->width;
          height          = pattern->mask->height;
          bpp             = pattern->mask->bytes;
          num_color_bytes = pattern->mask->height * pattern->mask->width *
                            pattern->mask->bytes;
          color_bytes     = g_memdup (temp_buf_data (pattern->mask),
                                      num_color_bytes);
        }
      else
        success = FALSE;
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    {
      g_value_set_int (&return_vals->values[1], width);
      g_value_set_int (&return_vals->values[2], height);
      g_value_set_int (&return_vals->values[3], bpp);
      g_value_set_int (&return_vals->values[4], num_color_bytes);
      gimp_value_take_int8array (&return_vals->values[5], color_bytes, num_color_bytes);
    }

  return return_vals;
}

static GimpProcedure pattern_get_pixels_proc =
{
  TRUE, TRUE,
  "gimp-pattern-get-pixels",
  "gimp-pattern-get-pixels",
  "Retrieve information about the specified pattern (including pixels).",
  "This procedure retrieves information about the specified. This includes the pattern extents (width and height), its bpp and its pixel data.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { pattern_get_pixels_invoker } }
};
