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

#include "config.h"

#include <stdlib.h>

#include <gtk/gtk.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"
#include "libgimpwidgets/gimpwidgets-private.h"

#include "gui-types.h"

#include "config/gimpguiconfig.h"

#include "core/gimp.h"
#include "core/gimp-utils.h"
#include "core/gimpcontainer.h"
#include "core/gimpcontext.h"
#include "core/gimpenvirontable.h"
#include "core/gimpimage.h"
#include "core/gimptoolinfo.h"

#include "display/gimpdisplay.h"
#include "display/gimpdisplay-foreach.h"
#include "display/gimpdisplayshell.h"
#include "display/gimpdisplayshell-render.h"

#include "tools/gimp-tools.h"

#include "widgets/gimpclipboard.h"
#include "widgets/gimpcontrollers.h"
#include "widgets/gimpdevices.h"
#include "widgets/gimpdevicestatus.h"
#include "widgets/gimpdialogfactory.h"
#include "widgets/gimpdnd.h"
#include "widgets/gimphelp.h"
#include "widgets/gimphelp-ids.h"
#include "widgets/gimpmenufactory.h"
#include "widgets/gimpmessagebox.h"
#include "widgets/gimpsessioninfo.h"
#include "widgets/gimpuimanager.h"
#include "widgets/gimpwidgets-utils.h"

#include "actions/actions.h"
#include "actions/dialogs-commands.h"

#include "menus/menus.h"

#include "color-history.h"
#include "dialogs.h"
#include "gui.h"
#include "gui-vtable.h"
#include "quit-dialog.h"
#include "session.h"
#include "splash.h"
#include "themes.h"

#include "gimp-intl.h"


/*  local function prototypes  */

static gchar    * gui_sanity_check              (void);
static void       gui_help_func                 (const gchar        *help_id,
                                                 gpointer            help_data);
static gboolean   gui_get_background_func       (GimpRGB            *color);
static gboolean   gui_get_foreground_func       (GimpRGB            *color);

static void       gui_initialize_after_callback (Gimp               *gimp,
                                                 GimpInitStatusFunc  callback);

static void       gui_restore_callback          (Gimp               *gimp,
                                                 GimpInitStatusFunc  callback);
static void       gui_restore_after_callback    (Gimp               *gimp,
                                                 GimpInitStatusFunc  callback);

static gboolean   gui_exit_callback             (Gimp               *gimp,
                                                 gboolean            force);
static gboolean   gui_exit_after_callback       (Gimp               *gimp,
                                                 gboolean            force);

static void       gui_show_tooltips_notify      (GimpGuiConfig      *gui_config,
                                                 GParamSpec         *pspec,
                                                 Gimp               *gimp);
static void       gui_tearoff_menus_notify      (GimpGuiConfig      *gui_config,
                                                 GParamSpec         *pspec,
                                                 GtkUIManager       *manager);
static void       gui_device_change_notify      (Gimp               *gimp);

static void       gui_display_changed           (GimpContext        *context,
                                                 GimpDisplay        *display,
                                                 Gimp               *gimp);
static void       gui_image_disconnect          (GimpImage          *gimage,
                                                 Gimp               *gimp);


/*  private variables  */

static Gimp          *the_gui_gimp                = NULL;
static GQuark         image_disconnect_handler_id = 0;
static GimpUIManager *image_ui_manager            = NULL;


/*  public functions  */

gboolean
gui_libs_init (gint    *argc,
	       gchar ***argv)
{
  gchar *abort_message;

  g_return_val_if_fail (argc != NULL, FALSE);
  g_return_val_if_fail (argv != NULL, FALSE);

  if (! gtk_init_check (argc, argv))
    return FALSE;

  abort_message = gui_sanity_check ();
  if (abort_message)
    gui_abort (abort_message);

  gimp_widgets_init (gui_help_func,
                     gui_get_foreground_func,
                     gui_get_background_func,
                     NULL);

  g_type_class_ref (GIMP_TYPE_COLOR_SELECT);

  return TRUE;
}

void
gui_abort (const gchar *abort_message)
{
  GtkWidget *dialog;
  GtkWidget *box;

  g_return_if_fail (abort_message != NULL);

  dialog = gimp_dialog_new (_("GIMP Message"), "gimp-abort",
                            NULL, GTK_DIALOG_MODAL, NULL, NULL,
                            GTK_STOCK_OK, GTK_RESPONSE_OK,
                            NULL);

  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

  box = g_object_new (GIMP_TYPE_MESSAGE_BOX,
                      "stock_id",     GIMP_STOCK_WILBER_EEK,
                      "border_width", 12,
                      NULL);

  gimp_message_box_set_text (GIMP_MESSAGE_BOX (box), abort_message);

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), box);
  gtk_widget_show (box);

  gimp_dialog_run (GIMP_DIALOG (dialog));

  exit (EXIT_FAILURE);
}

GimpInitStatusFunc
gui_init (Gimp     *gimp,
          gboolean  no_splash)
{
  GimpInitStatusFunc  status_callback = NULL;
  GdkScreen          *screen;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (the_gui_gimp == NULL, NULL);

  the_gui_gimp = gimp;

  gimp_dnd_init (gimp);

  themes_init (gimp);

  gdk_rgb_set_min_colors (CLAMP (gimp->config->min_colors, 27, 256));
  gdk_rgb_set_install (gimp->config->install_cmap);

  screen = gdk_screen_get_default ();
  gtk_widget_set_default_colormap (gdk_screen_get_rgb_colormap (screen));

  if (! no_splash)
    {
      splash_create ();
      status_callback = splash_update;
    }

  g_signal_connect_after (gimp, "initialize",
                          G_CALLBACK (gui_initialize_after_callback),
                          NULL);

  g_signal_connect (gimp, "restore",
                    G_CALLBACK (gui_restore_callback),
                    NULL);
  g_signal_connect_after (gimp, "restore",
                          G_CALLBACK (gui_restore_after_callback),
                          NULL);

  g_signal_connect (gimp, "exit",
                    G_CALLBACK (gui_exit_callback),
                    NULL);
  g_signal_connect_after (gimp, "exit",
                          G_CALLBACK (gui_exit_after_callback),
                          NULL);

  return status_callback;
}

void
gui_post_init (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (GIMP_GUI_CONFIG (gimp->config)->show_tips)
    gimp_dialog_factory_dialog_new (global_dialog_factory,
                                    gdk_screen_get_default (),
                                    "gimp-tips-dialog", -1);

  if (gimp_check_glib_version (2, 4, 4) == NULL &&
      gimp_check_glib_version (2, 4, 5) != NULL)
    {
      g_message ("You are using GLib version 2.4.4.\n\n"
                 "This version of GLib contains a bug "
                 "affecting filename conversions. "
                 "The GIMP won't be able to open any file "
                 "with non-ASCII characters (e.g. umlauts) "
                 "in its filename.\n\n"
                 "Please upgrade to GLib 2.4.5 or newer.");
    }
}


/*  private functions  */

static gchar *
gui_sanity_check (void)
{
  const gchar *mismatch;

#define GTK_REQUIRED_MAJOR 2
#define GTK_REQUIRED_MINOR 4
#define GTK_REQUIRED_MICRO 1

  mismatch = gtk_check_version (GTK_REQUIRED_MAJOR,
                                GTK_REQUIRED_MINOR,
                                GTK_REQUIRED_MICRO);

  if (mismatch)
    return g_strdup_printf
      ("%s\n\n"
       "The GIMP requires Gtk+ version %d.%d.%d or later.\n"
       "Installed Gtk+ version is %d.%d.%d.\n\n"
       "Somehow you or your software packager managed\n"
       "to install The GIMP with an older Gtk+ version.\n\n"
       "Please upgrade to Gtk+ version %d.%d.%d or later.",
       mismatch,
       GTK_REQUIRED_MAJOR, GTK_REQUIRED_MINOR, GTK_REQUIRED_MICRO,
       gtk_major_version, gtk_minor_version, gtk_micro_version,
       GTK_REQUIRED_MAJOR, GTK_REQUIRED_MINOR, GTK_REQUIRED_MICRO);

#undef GTK_REQUIRED_MAJOR
#undef GTK_REQUIRED_MINOR
#undef GTK_REQUIRED_MICRO

  return NULL;
}


static void
gui_help_func (const gchar *help_id,
               gpointer     help_data)
{
  g_return_if_fail (GIMP_IS_GIMP (the_gui_gimp));

  gimp_help (the_gui_gimp, NULL, help_id);
}

static gboolean
gui_get_foreground_func (GimpRGB *color)
{
  g_return_val_if_fail (color != NULL, FALSE);
  g_return_val_if_fail (GIMP_IS_GIMP (the_gui_gimp), FALSE);

  gimp_context_get_foreground (gimp_get_user_context (the_gui_gimp), color);

  return TRUE;
}

static gboolean
gui_get_background_func (GimpRGB *color)
{
  g_return_val_if_fail (color != NULL, FALSE);
  g_return_val_if_fail (GIMP_IS_GIMP (the_gui_gimp), FALSE);

  gimp_context_get_background (gimp_get_user_context (the_gui_gimp), color);

  return TRUE;
}

static void
gui_initialize_after_callback (Gimp               *gimp,
                               GimpInitStatusFunc  status_callback)
{
  const gchar *name = NULL;

  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (gimp->be_verbose)
    g_print ("INIT: gui_initialize_after_callback\n");

#if defined (GDK_WINDOWING_X11)
  name = "DISPLAY";
#elif defined (GDK_WINDOWING_DIRECTFB) || defined (GDK_WINDOWING_FB)
  name = "GDK_DISPLAY";
#endif

  /* TODO: Need to care about display migration with GTK+ 2.2 at some point */

  if (name)
    {
      gchar *display;

      display = gdk_get_display ();
      gimp_environ_table_add (gimp->environ_table, name, display, NULL);
      g_free (display);
    }

  gimp_tools_init (gimp);

  gimp_context_set_tool (gimp_get_user_context (gimp),
                         gimp_tool_info_get_standard (gimp));
}

static void
gui_restore_callback (Gimp               *gimp,
                      GimpInitStatusFunc  status_callback)
{
  GimpDisplayConfig *display_config = GIMP_DISPLAY_CONFIG (gimp->config);
  GimpGuiConfig     *gui_config     = GIMP_GUI_CONFIG (gimp->config);

  if (gimp->be_verbose)
    g_print ("INIT: gui_restore_callback\n");

  gui_vtable_init (gimp);

  image_disconnect_handler_id =
    gimp_container_add_handler (gimp->images, "disconnect",
				G_CALLBACK (gui_image_disconnect),
				gimp);

  if (! gui_config->show_tool_tips)
    gimp_help_disable_tooltips ();

  g_signal_connect (gui_config, "notify::show-tool-tips",
                    G_CALLBACK (gui_show_tooltips_notify),
                    gimp);

  g_signal_connect (gimp_get_user_context (gimp), "display_changed",
		    G_CALLBACK (gui_display_changed),
		    gimp);

  /* make sure the monitor resolution is valid */
  if (display_config->monitor_res_from_gdk               ||
      display_config->monitor_xres < GIMP_MIN_RESOLUTION ||
      display_config->monitor_yres < GIMP_MIN_RESOLUTION)
    {
      gdouble xres, yres;

      gimp_get_screen_resolution (NULL, &xres, &yres);

      g_object_set (gimp->config,
		    "monitor-xresolution",                      xres,
		    "monitor-yresolution",                      yres,
		    "monitor-resolution-from-windowing-system", TRUE,
		    NULL);
    }

  actions_init (gimp);
  menus_init (gimp);
  render_init (gimp);

  dialogs_init (gimp);

  gimp_clipboard_init (gimp);
  gimp_devices_init (gimp, gui_device_change_notify);
  gimp_controllers_init (gimp);
  session_init (gimp);

  (* status_callback) (NULL, _("Tool Options"), 1.0);
  gimp_tools_restore (gimp);
}

static void
gui_restore_after_callback (Gimp               *gimp,
                            GimpInitStatusFunc  status_callback)
{
  GimpGuiConfig *gui_config = GIMP_GUI_CONFIG (gimp->config);

  if (gimp->be_verbose)
    g_print ("INIT: gui_restore_after_callback\n");

  gimp->message_handler = GIMP_MESSAGE_BOX;

  if (gui_config->restore_accels)
    menus_restore (gimp);

  image_ui_manager = gimp_menu_factory_manager_new (global_menu_factory,
                                                    "<Image>",
                                                    gimp,
                                                    gui_config->tearoff_menus);

  g_signal_connect_object (gui_config, "notify::tearoff-menus",
                           G_CALLBACK (gui_tearoff_menus_notify),
                           image_ui_manager, 0);

  gimp_devices_restore (gimp);
  gimp_controllers_restore (gimp, image_ui_manager);

  if (status_callback == splash_update)
    splash_destroy ();

  color_history_restore ();

  if (gui_config->restore_session)
    session_restore (gimp);

  dialogs_show_toolbox ();
}

static gboolean
gui_exit_callback (Gimp     *gimp,
                   gboolean  force)
{
  GimpGuiConfig  *gui_config = GIMP_GUI_CONFIG (gimp->config);

  if (gimp->be_verbose)
    g_print ("EXIT: gui_exit_callback\n");

  if (! force && gimp_displays_dirty (gimp))
    {
      GtkWidget *dialog = quit_dialog_new (gimp);

      gtk_widget_show (dialog);

      return TRUE; /* stop exit for now */
    }

  gimp->message_handler = GIMP_CONSOLE;

  if (gui_config->save_session_info)
    session_save (gimp, FALSE);

  color_history_save ();

  if (gui_config->save_accels)
    menus_save (gimp, FALSE);

  if (gui_config->save_device_status)
    gimp_devices_save (gimp, FALSE);

  if (TRUE /* gui_config->save_controllers */)
    gimp_controllers_save (gimp);

  gimp_displays_delete (gimp);

  gimp_tools_save (gimp);
  gimp_tools_exit (gimp);

  return FALSE; /* continue exiting */
}

static gboolean
gui_exit_after_callback (Gimp     *gimp,
                         gboolean  force)
{
  if (gimp->be_verbose)
    g_print ("EXIT: gui_exit_after_callback\n");

  g_signal_handlers_disconnect_by_func (gimp->config,
                                        gui_show_tooltips_notify,
                                        gimp);

  gimp_container_remove_handler (gimp->images, image_disconnect_handler_id);
  image_disconnect_handler_id = 0;

  g_object_unref (image_ui_manager);
  image_ui_manager = NULL;

  session_exit (gimp);
  menus_exit (gimp);
  actions_exit (gimp);
  render_exit (gimp);

  dialogs_exit (gimp);
  gimp_controllers_exit (gimp);
  gimp_devices_exit (gimp);
  gimp_clipboard_exit (gimp);

  themes_exit (gimp);

  g_type_class_unref (g_type_class_peek (GIMP_TYPE_COLOR_SELECT));

  return FALSE; /* continue exiting */
}

static void
gui_show_tooltips_notify (GimpGuiConfig *gui_config,
                          GParamSpec    *param_spec,
                          Gimp          *gimp)
{
  if (gui_config->show_tool_tips)
    gimp_help_enable_tooltips ();
  else
    gimp_help_disable_tooltips ();
}

static void
gui_tearoff_menus_notify (GimpGuiConfig *gui_config,
                          GParamSpec    *pspec,
                          GtkUIManager  *manager)
{
  gtk_ui_manager_set_add_tearoffs (manager, gui_config->tearoff_menus);
}

static void
gui_device_change_notify (Gimp *gimp)
{
  GimpSessionInfo *session_info;

  session_info = gimp_dialog_factory_find_session_info (global_dock_factory,
                                                        "gimp-device-status");

  if (session_info && session_info->widget)
    {
      GtkWidget *device_status;

      device_status = GTK_BIN (session_info->widget)->child;

      gimp_device_status_update (GIMP_DEVICE_STATUS (device_status));
    }
}


#ifdef __GNUC__
#warning FIXME: this junk should mostly go to the display subsystem
#endif

static void
gui_display_changed (GimpContext *context,
		     GimpDisplay *display,
		     Gimp        *gimp)
{
  gimp_ui_manager_update (image_ui_manager, display);
}

static void
gui_image_disconnect (GimpImage *gimage,
		      Gimp      *gimp)
{
  /*  check if this is the last image and if it had a display  */
  if (gimp_container_num_children (gimp->images) == 1 &&
      gimage->instance_count                      > 0)
    {
      dialogs_show_toolbox ();
    }
}
