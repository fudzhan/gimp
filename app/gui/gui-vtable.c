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

#include <string.h>

#include <gtk/gtk.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "gui-types.h"

#include "config/gimpguiconfig.h"

#include "core/gimp.h"
#include "core/gimp-utils.h"
#include "core/gimpbrush.h"
#include "core/gimpcontainer.h"
#include "core/gimpcontext.h"
#include "core/gimpgradient.h"
#include "core/gimpimage.h"
#include "core/gimppalette.h"
#include "core/gimppattern.h"
#include "core/gimpprogress.h"

#include "text/gimpfont.h"

#include "plug-in/plug-ins.h"
#include "plug-in/plug-in-proc.h"

#include "widgets/gimpactiongroup.h"
#include "widgets/gimpbrushselect.h"
#include "widgets/gimpdialogfactory.h"
#include "widgets/gimperrorconsole.h"
#include "widgets/gimperrordialog.h"
#include "widgets/gimpfontselect.h"
#include "widgets/gimpgradientselect.h"
#include "widgets/gimphelp.h"
#include "widgets/gimphelp-ids.h"
#include "widgets/gimpmenufactory.h"
#include "widgets/gimppaletteselect.h"
#include "widgets/gimppatternselect.h"
#include "widgets/gimpprogressdialog.h"
#include "widgets/gimpuimanager.h"

#include "display/gimpdisplay.h"
#include "display/gimpdisplay-foreach.h"
#include "display/gimpdisplayshell.h"

#include "actions/plug-in-actions.h"

#include "menus/menus.h"
#include "menus/plug-in-menus.h"

#include "dialogs.h"
#include "dialogs-constructors.h"
#include "themes.h"


/*  local function prototypes  */

static void           gui_threads_enter        (Gimp          *gimp);
static void           gui_threads_leave        (Gimp          *gimp);
static void           gui_set_busy             (Gimp          *gimp);
static void           gui_unset_busy           (Gimp          *gimp);
static void           gui_message              (Gimp          *gimp,
                                                const gchar   *domain,
                                                const gchar   *message);
static void           gui_help                 (Gimp          *gimp,
                                                const gchar   *help_domain,
                                                const gchar   *help_id);
static const gchar  * gui_get_program_class    (Gimp          *gimp);
static gchar        * gui_get_display_name     (Gimp          *gimp,
                                                gint           gdisp_ID,
                                                gint          *monitor_number);
static const gchar  * gui_get_theme_dir        (Gimp          *gimp);
static GimpObject   * gui_display_get_by_ID    (Gimp          *gimp,
                                                gint           ID);
static gint           gui_display_get_ID       (GimpObject    *display);
static GimpObject   * gui_display_create       (GimpImage     *gimage,
                                                GimpUnit       unit,
                                                gdouble        scale);
static void           gui_display_delete       (GimpObject    *display);
static void           gui_displays_reconnect   (Gimp          *gimp,
                                                GimpImage     *old_image,
                                                GimpImage     *new_image);
static void           gui_menus_init           (Gimp          *gimp,
                                                GSList        *plug_in_defs,
                                                const gchar   *plugins_domain);
static void           gui_menus_create_entry   (Gimp          *gimp,
                                                PlugInProcDef *proc_def);
static void           gui_menus_delete_entry   (Gimp          *gimp,
                                                PlugInProcDef *proc_def);
static GimpProgress * gui_new_progress         (Gimp          *gimp,
                                                gint           display_ID);
static void           gui_free_progress        (Gimp          *gimp,
                                                GimpProgress  *progress);
static gboolean       gui_pdb_dialog_new       (Gimp          *gimp,
                                                GimpContext   *context,
                                                GimpContainer *container,
                                                const gchar   *title,
                                                const gchar   *callback_name,
                                                const gchar   *object_name,
                                                va_list        args);
static gboolean       gui_pdb_dialog_set       (Gimp          *gimp,
                                                GimpContainer *container,
                                                const gchar   *callback_name,
                                                const gchar   *object_name,
                                                va_list        args);
static gboolean       gui_pdb_dialog_close     (Gimp          *gimp,
                                                GimpContainer *container,
                                                const gchar   *callback_name);
static void           gui_pdb_dialogs_check    (Gimp          *gimp);


/*  public functions  */

void
gui_vtable_init (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  gimp->gui.threads_enter      = gui_threads_enter;
  gimp->gui.threads_leave      = gui_threads_leave;
  gimp->gui.set_busy           = gui_set_busy;
  gimp->gui.unset_busy         = gui_unset_busy;
  gimp->gui.message            = gui_message;
  gimp->gui.help               = gui_help;
  gimp->gui.get_program_class  = gui_get_program_class;
  gimp->gui.get_display_name   = gui_get_display_name;
  gimp->gui.get_theme_dir      = gui_get_theme_dir;
  gimp->gui.display_get_by_id  = gui_display_get_by_ID;
  gimp->gui.display_get_id     = gui_display_get_ID;
  gimp->gui.display_create     = gui_display_create;
  gimp->gui.display_delete     = gui_display_delete;
  gimp->gui.displays_reconnect = gui_displays_reconnect;
  gimp->gui.menus_init         = gui_menus_init;
  gimp->gui.menus_create       = gui_menus_create_entry;
  gimp->gui.menus_delete       = gui_menus_delete_entry;
  gimp->gui.progress_new       = gui_new_progress;
  gimp->gui.progress_free      = gui_free_progress;
  gimp->gui.pdb_dialog_new     = gui_pdb_dialog_new;
  gimp->gui.pdb_dialog_set     = gui_pdb_dialog_set;
  gimp->gui.pdb_dialog_close   = gui_pdb_dialog_close;
  gimp->gui.pdb_dialogs_check  = gui_pdb_dialogs_check;
}


/*  private functions  */

static void
gui_threads_enter (Gimp *gimp)
{
  GDK_THREADS_ENTER ();
}

static void
gui_threads_leave (Gimp *gimp)
{
  GDK_THREADS_LEAVE ();
}

static void
gui_set_busy (Gimp *gimp)
{
  gimp_displays_set_busy (gimp);
  gimp_dialog_factories_set_busy ();

  gdk_flush ();
}

static void
gui_unset_busy (Gimp *gimp)
{
  gimp_displays_unset_busy (gimp);
  gimp_dialog_factories_unset_busy ();

  gdk_flush ();
}

static void
gui_message (Gimp        *gimp,
             const gchar *domain,
             const gchar *message)
{
  switch (gimp->message_handler)
    {
    case GIMP_ERROR_CONSOLE:
      {
        GtkWidget *dockable;

        dockable = gimp_dialog_factory_dialog_raise (global_dock_factory,
                                                     gdk_screen_get_default (),
                                                     "gimp-error-console", -1);

        if (dockable)
          {
            GimpErrorConsole *console;

            console = GIMP_ERROR_CONSOLE (GTK_BIN (dockable)->child);

            gimp_error_console_add (console,
                                    GIMP_STOCK_WARNING, domain, message);

            return;
          }

        gimp->message_handler = GIMP_MESSAGE_BOX;
      }
      /*  fallthru  */

    case GIMP_MESSAGE_BOX:
      {
        GtkWidget *dialog;

        dialog = gimp_dialog_factory_dialog_raise (global_dialog_factory,
                                                   gdk_screen_get_default (),
                                                   "gimp-error-dialog", -1);
        if (dialog)
          {
            gimp_error_dialog_add (GIMP_ERROR_DIALOG (dialog),
                                   GIMP_STOCK_WARNING, domain, message);

            return;
          }

        gimp->message_handler = GIMP_CONSOLE;
      }
      /*  fallthru  */

    case GIMP_CONSOLE:
      g_printerr ("%s: %s\n\n", domain, message);
      break;
    }
}

void
gui_help (Gimp        *gimp,
          const gchar *help_domain,
          const gchar *help_id)
{
  gimp_help_show (gimp, help_domain, help_id);
}

static const gchar *
gui_get_program_class (Gimp *gimp)
{
  return gdk_get_program_class ();
}

static gchar *
gui_get_display_name (Gimp *gimp,
                      gint  gdisp_ID,
                      gint *monitor_number)
{
  GimpDisplay *gdisp = NULL;
  GdkScreen   *screen;
  gint         monitor;

  if (gdisp_ID > 0)
    gdisp = gimp_display_get_by_ID (gimp, gdisp_ID);

  if (gdisp)
    {
      screen  = gtk_widget_get_screen (gdisp->shell);
      monitor = gdk_screen_get_monitor_at_window (screen,
                                                  gdisp->shell->window);
    }
  else
    {
      gint x, y;

      gdk_display_get_pointer (gdk_display_get_default (),
                               &screen, &x, &y, NULL);
      monitor = gdk_screen_get_monitor_at_point (screen, x, y);
    }

  *monitor_number = monitor;

  if (screen)
    return gdk_screen_make_display_name (screen);

  return NULL;
}

static const gchar *
gui_get_theme_dir (Gimp *gimp)
{
  return themes_get_theme_dir (gimp, GIMP_GUI_CONFIG (gimp->config)->theme);
}

static GimpObject *
gui_display_get_by_ID (Gimp *gimp,
                       gint  ID)
{
  return (GimpObject *) gimp_display_get_by_ID (gimp, ID);
}

static gint
gui_display_get_ID (GimpObject *display)
{
  return gimp_display_get_ID (GIMP_DISPLAY (display));
}

static GimpObject *
gui_display_create (GimpImage *gimage,
                    GimpUnit   unit,
                    gdouble    scale)
{
  GimpDisplayShell *shell;
  GimpDisplay      *gdisp;
  GList            *image_managers;

  image_managers = gimp_ui_managers_from_name ("<Image>");

  gdisp = gimp_display_new (gimage, unit, scale,
                            global_menu_factory,

                            image_managers->data);

  shell = GIMP_DISPLAY_SHELL (gdisp->shell);

  gimp_context_set_display (gimp_get_user_context (gimage->gimp), gdisp);

  gimp_ui_manager_update (shell->menubar_manager, shell);

  return GIMP_OBJECT (gdisp);
}

static void
gui_display_delete (GimpObject *display)
{
  gimp_display_delete (GIMP_DISPLAY (display));
}

static void
gui_displays_reconnect (Gimp      *gimp,
                        GimpImage *old_image,
                        GimpImage *new_image)
{
  gimp_displays_reconnect (gimp, old_image, new_image);
}

static void
gui_menus_init (Gimp        *gimp,
                GSList      *plug_in_defs,
                const gchar *std_plugins_domain)
{
  plug_in_menus_init (gimp, plug_in_defs, std_plugins_domain);
}

static void
gui_menus_create_entry (Gimp          *gimp,
                        PlugInProcDef *proc_def)
{
  GList *list;

  for (list = gimp_action_groups_from_name ("plug-in");
       list;
       list = g_list_next (list))
    {
      plug_in_actions_add_proc (list->data, proc_def);
    }

  for (list = gimp_ui_managers_from_name ("<Image>");
       list;
       list = g_list_next (list))
    {
      GList *path;

      for (path = proc_def->menu_paths; path; path = g_list_next (path))
        {
          if (! strncmp (path->data, "<Toolbox>", 9))
            {
              plug_in_menus_add_proc (list->data, "/toolbox-menubar",
                                      proc_def, path->data);
            }
          else if (! strncmp (path->data, "<Image>", 7))
            {
              plug_in_menus_add_proc (list->data, "/image-menubar",
                                      proc_def, path->data);
              plug_in_menus_add_proc (list->data, "/dummy-menubar/image-popup",
                                      proc_def, path->data);
            }
        }
    }
}

static void
gui_menus_delete_entry (Gimp          *gimp,
                        PlugInProcDef *proc_def)
{
  GList *list;

  for (list = gimp_ui_managers_from_name ("<Image>");
       list;
       list = g_list_next (list))
    {
      plug_in_menus_remove_proc (list->data, proc_def);
    }

  for (list = gimp_action_groups_from_name ("plug-in");
       list;
       list = g_list_next (list))
    {
      plug_in_actions_remove_proc (list->data, proc_def);
    }
}

static GimpProgress *
gui_new_progress (Gimp *gimp,
                  gint  display_ID)
{
  GimpDisplay *display = NULL;

  if (display_ID > 0)
    display = gimp_display_get_by_ID (gimp, display_ID);

  if (display)
    return GIMP_PROGRESS (display);

  return GIMP_PROGRESS (gimp_progress_dialog_new ());
}

static void
gui_free_progress (Gimp          *gimp,
                   GimpProgress  *progress)
{
  g_return_if_fail (GIMP_IS_PROGRESS_DIALOG (progress));

  if (GIMP_IS_PROGRESS_DIALOG (progress))
    gtk_widget_destroy (GTK_WIDGET (progress));
}

static gboolean
gui_pdb_dialog_new (Gimp          *gimp,
                    GimpContext   *context,
                    GimpContainer *container,
                    const gchar   *title,
                    const gchar   *callback_name,
                    const gchar   *object_name,
                    va_list        args)
{
  GType             dialog_type = G_TYPE_NONE;
  const gchar      *dialog_role = NULL;
  const gchar      *help_id     = NULL;
  GimpDataEditFunc  edit_func   = NULL;

  if (container->children_type == GIMP_TYPE_BRUSH)
    {
      dialog_type = GIMP_TYPE_BRUSH_SELECT;
      dialog_role = "gimp-brush-selection";
      help_id     = GIMP_HELP_BRUSH_DIALOG;
      edit_func   = dialogs_edit_brush_func;
    }
  else if (container->children_type == GIMP_TYPE_FONT)
    {
      dialog_type = GIMP_TYPE_FONT_SELECT;
      dialog_role = "gimp-font-selection";
      help_id     = GIMP_HELP_FONT_DIALOG;
    }
  else if (container->children_type == GIMP_TYPE_GRADIENT)
    {
      dialog_type = GIMP_TYPE_GRADIENT_SELECT;
      dialog_role = "gimp-gradient-selection";
      help_id     = GIMP_HELP_GRADIENT_DIALOG;
      edit_func   = dialogs_edit_gradient_func;
    }
  else if (container->children_type == GIMP_TYPE_PALETTE)
    {
      dialog_type = GIMP_TYPE_PALETTE_SELECT;
      dialog_role = "gimp-palette-selection";
      help_id     = GIMP_HELP_PALETTE_DIALOG;
      edit_func   = dialogs_edit_palette_func;
    }
  else if (container->children_type == GIMP_TYPE_PATTERN)
    {
      dialog_type = GIMP_TYPE_PATTERN_SELECT;
      dialog_role = "gimp-pattern-selection";
      help_id     = GIMP_HELP_PATTERN_DIALOG;
    }

  if (dialog_type != G_TYPE_NONE)
    {
      GimpObject *object = NULL;

#ifdef __GNUC__
#warning FIXME: re-enable gimp->no_data case
#endif
#if 0
      if (gimp->no_data)
        {
          static gboolean first_call = TRUE;

          if (first_call)
            gimp_data_factory_data_init (gimp->pattern_factory, FALSE);

          first_call = FALSE;
        }
#endif

      if (object_name && strlen (object_name))
        object = gimp_container_get_child_by_name (container, object_name);

      if (! object)
        object = gimp_context_get_by_type (context, container->children_type);

      if (object)
        {
          GParameter *params   = NULL;
          gint        n_params = 0;
          GtkWidget  *dialog;

          params = gimp_parameters_append (dialog_type, params, &n_params,
                                           "title",          title,
                                           "role",           dialog_role,
                                           "help-func",      gimp_standard_help_func,
                                           "help-id",        help_id,
                                           "context",        context,
                                           "select-type",    container->children_type,
                                           "initial-object", object,
                                           "callback-name",  callback_name,
                                           "menu-factory",   global_menu_factory,
                                           NULL);

          if (edit_func)
            params = gimp_parameters_append (dialog_type, params, &n_params,
                                             "edit-func", edit_func,
                                             NULL);

          params = gimp_parameters_append_valist (dialog_type,
                                                  params, &n_params,
                                                  args);

          dialog = g_object_newv (dialog_type, n_params, params);

          gimp_parameters_free (params, n_params);

          gtk_widget_show (dialog);

          return TRUE;
        }
    }

  return FALSE;
}

static gboolean
gui_pdb_dialog_set (Gimp          *gimp,
                    GimpContainer *container,
                    const gchar   *callback_name,
                    const gchar   *object_name,
                    va_list        args)
{
  GimpPdbDialogClass *klass = NULL;

  if (container->children_type == GIMP_TYPE_BRUSH)
    klass = g_type_class_peek (GIMP_TYPE_BRUSH_SELECT);
  else if (container->children_type == GIMP_TYPE_FONT)
    klass = g_type_class_peek (GIMP_TYPE_FONT_SELECT);
  else if (container->children_type == GIMP_TYPE_GRADIENT)
    klass = g_type_class_peek (GIMP_TYPE_GRADIENT_SELECT);
  else if (container->children_type == GIMP_TYPE_PALETTE)
    klass = g_type_class_peek (GIMP_TYPE_PALETTE_SELECT);
  else if (container->children_type == GIMP_TYPE_PATTERN)
    klass = g_type_class_peek (GIMP_TYPE_PATTERN_SELECT);

  if (klass)
    {
      GimpPdbDialog *dialog;

      dialog = gimp_pdb_dialog_get_by_callback (klass, callback_name);

      if (dialog && dialog->select_type == container->children_type)
        {
          GimpObject *object;

          object = gimp_container_get_child_by_name (container, object_name);

          if (object)
            {
              const gchar *prop_name;

              gimp_context_set_by_type (dialog->context, dialog->select_type,
                                        object);

              prop_name = va_arg (args, const gchar *);

              if (prop_name)
                g_object_set_valist (G_OBJECT (dialog), prop_name, args);

              gtk_window_present (GTK_WINDOW (dialog));
              return TRUE;
            }
        }
    }

  return FALSE;
}

static gboolean
gui_pdb_dialog_close (Gimp          *gimp,
                      GimpContainer *container,
                      const gchar   *callback_name)
{
  GimpPdbDialogClass *klass = NULL;

  if (container->children_type == GIMP_TYPE_BRUSH)
    klass = g_type_class_peek (GIMP_TYPE_BRUSH_SELECT);
  else if (container->children_type == GIMP_TYPE_FONT)
    klass = g_type_class_peek (GIMP_TYPE_FONT_SELECT);
  else if (container->children_type == GIMP_TYPE_GRADIENT)
    klass = g_type_class_peek (GIMP_TYPE_GRADIENT_SELECT);
  else if (container->children_type == GIMP_TYPE_PALETTE)
    klass = g_type_class_peek (GIMP_TYPE_PALETTE_SELECT);
  else if (container->children_type == GIMP_TYPE_PATTERN)
    klass = g_type_class_peek (GIMP_TYPE_PATTERN_SELECT);

  if (klass)
    {
      GimpPdbDialog *dialog;

      dialog = gimp_pdb_dialog_get_by_callback (klass, callback_name);

      if (dialog && dialog->select_type == container->children_type)
        {
          gtk_widget_destroy (GTK_WIDGET (dialog));
          return TRUE;
        }
    }

  return FALSE;
}

static void
gui_pdb_dialogs_check (Gimp *gimp)
{
  GimpPdbDialogClass *klass;

  if ((klass = g_type_class_peek (GIMP_TYPE_BRUSH_SELECT)))
    gimp_pdb_dialogs_check_callback (klass);

  if ((klass = g_type_class_peek (GIMP_TYPE_FONT_SELECT)))
    gimp_pdb_dialogs_check_callback (klass);

  if ((klass = g_type_class_peek (GIMP_TYPE_GRADIENT_SELECT)))
    gimp_pdb_dialogs_check_callback (klass);

  if ((klass = g_type_class_peek (GIMP_TYPE_PALETTE_SELECT)))
    gimp_pdb_dialogs_check_callback (klass);

  if ((klass = g_type_class_peek (GIMP_TYPE_PATTERN_SELECT)))
    gimp_pdb_dialogs_check_callback (klass);
}
