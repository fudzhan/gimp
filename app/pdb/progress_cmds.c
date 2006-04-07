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

#include "pdb-types.h"
#include "gimp-pdb.h"
#include "gimpprocedure.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"
#include "plug-in/plug-in-progress.h"
#include "plug-in/plug-in.h"


static GValueArray *
progress_init_invoker (GimpProcedure     *procedure,
                       Gimp              *gimp,
                       GimpContext       *context,
                       GimpProgress      *progress,
                       const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *message;
  GimpObject *gdisplay;

  message = g_value_get_string (&args->values[0]);
  gdisplay = gimp_value_get_display (&args->values[1], gimp);

  if (success)
    {
      if (gimp->current_plug_in && gimp->current_plug_in->open)
        {
          if (! gimp->no_interface)
            plug_in_progress_start (gimp->current_plug_in, message, gdisplay);
        }
      else
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
progress_update_invoker (GimpProcedure     *procedure,
                         Gimp              *gimp,
                         GimpContext       *context,
                         GimpProgress      *progress,
                         const GValueArray *args)
{
  gboolean success = TRUE;
  gdouble percentage;

  percentage = g_value_get_double (&args->values[0]);

  if (success)
    {
      if (gimp->current_plug_in && gimp->current_plug_in->open)
        {
          if (! gimp->no_interface)
            plug_in_progress_set_value (gimp->current_plug_in, percentage);
        }
      else
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
progress_pulse_invoker (GimpProcedure     *procedure,
                        Gimp              *gimp,
                        GimpContext       *context,
                        GimpProgress      *progress,
                        const GValueArray *args)
{
  gboolean success = TRUE;
  if (gimp->current_plug_in && gimp->current_plug_in->open)
    {
      if (! gimp->no_interface)
        plug_in_progress_pulse (gimp->current_plug_in);
    }
  else
    success = FALSE;
  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
progress_set_text_invoker (GimpProcedure     *procedure,
                           Gimp              *gimp,
                           GimpContext       *context,
                           GimpProgress      *progress,
                           const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *message;

  message = g_value_get_string (&args->values[0]);

  if (success)
    {
      if (gimp->current_plug_in && gimp->current_plug_in->open)
        {
          if (! gimp->no_interface)
            plug_in_progress_set_text (gimp->current_plug_in, message);
        }
      else
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
progress_get_window_handle_invoker (GimpProcedure     *procedure,
                                    Gimp              *gimp,
                                    GimpContext       *context,
                                    GimpProgress      *progress,
                                    const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  gint32 window = 0;

  if (gimp->current_plug_in && gimp->current_plug_in->open)
    {
      if (! gimp->no_interface)
        window = plug_in_progress_get_window (gimp->current_plug_in);
    }
  else
    success = FALSE;

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    g_value_set_int (&return_vals->values[1], window);

  return return_vals;
}

static GValueArray *
progress_install_invoker (GimpProcedure     *procedure,
                          Gimp              *gimp,
                          GimpContext       *context,
                          GimpProgress      *progress,
                          const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *progress_callback;

  progress_callback = g_value_get_string (&args->values[0]);

  if (success)
    {
      if (gimp->current_plug_in && gimp->current_plug_in->open)
        success = plug_in_progress_install (gimp->current_plug_in,
                                            progress_callback);
      else
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
progress_uninstall_invoker (GimpProcedure     *procedure,
                            Gimp              *gimp,
                            GimpContext       *context,
                            GimpProgress      *progress,
                            const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *progress_callback;

  progress_callback = g_value_get_string (&args->values[0]);

  if (success)
    {
      if (gimp->current_plug_in && gimp->current_plug_in->open)
        success = plug_in_progress_uninstall (gimp->current_plug_in,
                                              progress_callback);
      else
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
progress_cancel_invoker (GimpProcedure     *procedure,
                         Gimp              *gimp,
                         GimpContext       *context,
                         GimpProgress      *progress,
                         const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *progress_callback;

  progress_callback = g_value_get_string (&args->values[0]);

  if (success)
    {
      if (gimp->current_plug_in && gimp->current_plug_in->open)
        success = plug_in_progress_cancel (gimp->current_plug_in,
                                           progress_callback);
      else
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

void
register_progress_procs (Gimp *gimp)
{
  GimpProcedure *procedure;

  /*
   * gimp-progress-init
   */
  procedure = gimp_procedure_new (progress_init_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-init");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-init",
                                     "Initializes the progress bar for the current plug-in.",
                                     "Initializes the progress bar for the current plug-in. It is only valid to call this procedure from a plug-in.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1995-1996",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("message",
                                                       "message",
                                                       "Message to use in the progress dialog",
                                                       FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_display_id ("gdisplay",
                                                           "gdisplay",
                                                           "GimpDisplay to update progressbar in, or -1 for a seperate window",
                                                           gimp,
                                                           GIMP_PARAM_READWRITE | GIMP_PARAM_NO_VALIDATE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-update
   */
  procedure = gimp_procedure_new (progress_update_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-update");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-update",
                                     "Updates the progress bar for the current plug-in.",
                                     "Updates the progress bar for the current plug-in. It is only valid to call this procedure from a plug-in.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1995-1996",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               g_param_spec_double ("percentage",
                                                    "percentage",
                                                    "Percentage of progress completed which must be between 0.0 and 1.0",
                                                    -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                                    GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-pulse
   */
  procedure = gimp_procedure_new (progress_pulse_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-pulse");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-pulse",
                                     "Pulses the progress bar for the current plug-in.",
                                     "Updates the progress bar for the current plug-in. It is only valid to call this procedure from a plug-in. Use this function instead of gimp_progress_update() if you cannot tell how much progress has been made. This usually causes the the progress bar to enter \"activity mode\", where a block bounces back and forth.",
                                     "Sven Neumann <sven@gimp.org>",
                                     "Sven Neumann",
                                     "2005",
                                     NULL);

  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-set-text
   */
  procedure = gimp_procedure_new (progress_set_text_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-set-text");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-set-text",
                                     "Changes the text in the progress bar for the current plug-in.",
                                     "This function allows to change the text in the progress bar for the current plug-in. Unlike gimp_progress_init() it does not change the displayed value.",
                                     "Sven Neumann <sven@gimp.org>",
                                     "Sven Neumann",
                                     "2005",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("message",
                                                       "message",
                                                       "Message to use in the progress dialog",
                                                       FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-get-window-handle
   */
  procedure = gimp_procedure_new (progress_get_window_handle_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-get-window-handle");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-get-window-handle",
                                     "Returns the native window ID of the toplevel window this plug-in's progress is displayed in.",
                                     "This function returns the native window ID of the toplevel window this plug-in\'s progress is displayed in.",
                                     "Michael Natterer <mitch@gimp.org>",
                                     "Michael Natterer",
                                     "2004",
                                     NULL);

  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_int32 ("window",
                                                          "window",
                                                          "The progress bar's toplevel window",
                                                          G_MININT32, G_MAXINT32, 0,
                                                          GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-install
   */
  procedure = gimp_procedure_new (progress_install_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-install");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-install",
                                     "Installs a progress callback for the current plug-in.",
                                     "This function installs a temporary PDB procedure which will handle all progress calls made by this plug-in and any procedure it calls. Calling this function multiple times simply replaces the old progress callbacks.",
                                     "Michael Natterer <mitch@gimp.org>",
                                     "Michael Natterer",
                                     "2004",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("progress-callback",
                                                       "progress callback",
                                                       "The callback PDB proc to call",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-uninstall
   */
  procedure = gimp_procedure_new (progress_uninstall_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-uninstall");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-uninstall",
                                     "Uninstalls the progress callback for the current plug-in.",
                                     "This function uninstalls any progress callback installed with gimp_progress_install() before.",
                                     "Michael Natterer <mitch@gimp.org>",
                                     "Michael Natterer",
                                     "2004",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("progress-callback",
                                                       "progress callback",
                                                       "The name of the callback registered for this progress",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

  /*
   * gimp-progress-cancel
   */
  procedure = gimp_procedure_new (progress_cancel_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-progress-cancel");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-progress-cancel",
                                     "Cancels a running progress.",
                                     "This function cancels the currently running progress.",
                                     "Michael Natterer <mitch@gimp.org>",
                                     "Michael Natterer",
                                     "2004",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("progress-callback",
                                                       "progress callback",
                                                       "The name of the callback registered for this progress",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register (gimp, procedure);
  g_object_unref (procedure);

}
