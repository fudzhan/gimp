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

#include <gtk/gtk.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "apptypes.h"

#include "dialog_handler.h"
#include "gimpcontext.h"
#include "gimpdnd.h"
#include "gimplist.h"
#include "gimppreview.h"
#include "gimpui.h"
#include "session.h"

#include "gimptool.h"
#include "gimptoolinfo.h"
#include "tool_options.h"
#include "tool_options_dialog.h"
#include "tool_manager.h"

#include "libgimp/gimpintl.h"


/*  local function prototypes  */

static void           tool_options_dialog_tool_changed   (GimpContext  *context,
							  GimpToolInfo *tool_info,
							  gpointer      data);
static GimpViewable * tool_options_dialog_drag_tool      (GtkWidget    *widget,
							  gpointer      data);
static void           tool_options_dialog_drop_tool      (GtkWidget    *widget,
							  GimpViewable *viewable,
							  gpointer      data);
static void           tool_options_dialog_reset_callback (GtkWidget    *widget,
							  gpointer      data);
static void           tool_options_dialog_close_callback (GtkWidget    *widget,
							  gpointer      data);


/*  private variables  */

static GtkWidget   *options_shell        = NULL;
static GtkWidget   *options_vbox         = NULL;
static GtkWidget   *options_label        = NULL;
static GtkWidget   *options_preview      = NULL;
static GtkWidget   *options_eventbox     = NULL;
static GtkWidget   *options_reset_button = NULL;

static ToolOptions *visible_tool_options = NULL;


/*  public functions  */

void
tool_options_dialog_new (void)
{
  GimpToolInfo *tool_info;
  GtkWidget    *frame;
  GtkWidget    *hbox;
  GtkWidget    *vbox;
  GList        *list;

  tool_info = gimp_context_get_tool (gimp_context_get_user ());

  if (! tool_info)
    {
      g_warning ("%s(): no tool info registered for active tool",
                 G_GNUC_FUNCTION);
    }

  /*  The shell and main vbox  */
  options_shell =
    gimp_dialog_new (_("Tool Options"), "tool_options",
		     tool_manager_help_func,
		     "dialogs/tool_options.html",
		     GTK_WIN_POS_NONE,
		     FALSE, TRUE, TRUE,

		     _("Reset"), tool_options_dialog_reset_callback,
		     NULL, NULL, &options_reset_button, FALSE, FALSE,

		     _("Close"), tool_options_dialog_close_callback,
		     NULL, NULL, NULL, TRUE, TRUE,

		     NULL);

  /*  hide the separator between the dialog's vbox and the action area  */
  gtk_widget_hide (GTK_WIDGET (g_list_nth_data (gtk_container_children (GTK_CONTAINER (GTK_BIN (options_shell)->child)), 0)));

  /*  Register dialog  */
  dialog_register (options_shell);
  session_set_window_geometry (options_shell, &tool_options_session_info,
			       FALSE );

  /*  The outer frame  */
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (options_shell)->vbox), frame);
  gtk_widget_show (frame);

  /*  The vbox containing the title frame and the options vbox  */
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (frame), vbox);
  gtk_widget_show (vbox);

  /*  The title frame  */
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  options_eventbox = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (frame), options_eventbox);
  gtk_widget_show (options_eventbox);

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER (options_eventbox), hbox);
  gtk_widget_show (hbox);

  options_preview = gimp_preview_new (GIMP_VIEWABLE (tool_info), 22, 0, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), options_preview, FALSE, FALSE, 0);
  gtk_widget_show (options_preview);

  options_label = gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (hbox), options_label, FALSE, FALSE, 1);
  gtk_widget_show (options_label);

  options_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (options_vbox), 2);
  gtk_box_pack_start (GTK_BOX (vbox), options_vbox, FALSE, FALSE, 0);

  gtk_widget_show (options_vbox);

  /*  dnd stuff  */
  gimp_gtk_drag_dest_set_by_type (options_shell,
				  GTK_DEST_DEFAULT_HIGHLIGHT |
				  GTK_DEST_DEFAULT_MOTION    |
				  GTK_DEST_DEFAULT_DROP,
				  GIMP_TYPE_TOOL_INFO,
				  GDK_ACTION_COPY);
  gimp_dnd_viewable_dest_set (options_shell,
			      GIMP_TYPE_TOOL_INFO,
			      tool_options_dialog_drop_tool, NULL);

  gimp_gtk_drag_source_set_by_type (options_eventbox,
				    GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
				    GIMP_TYPE_TOOL_INFO,
				    GDK_ACTION_COPY); 
  gimp_dnd_viewable_source_set (options_eventbox,
				GIMP_TYPE_TOOL_INFO,
				tool_options_dialog_drag_tool, NULL);

  for (list = GIMP_LIST (global_tool_info_list)->list;
       list;
       list = g_list_next (list))
    {
      tool_info = GIMP_TOOL_INFO (list->data);

      if (tool_info->tool_options)
	tool_options_dialog_add (tool_info->tool_options);
    }

  gtk_signal_connect_while_alive
    (GTK_OBJECT (gimp_context_get_user ()), "tool_changed",
     GTK_SIGNAL_FUNC (tool_options_dialog_tool_changed),
     NULL,
     GTK_OBJECT (options_shell));

  tool_info = gimp_context_get_tool (gimp_context_get_user ());

  tool_options_dialog_tool_changed (gimp_context_get_user (),
				    tool_info,
				    NULL);
}

void
tool_options_dialog_show (void)
{
  if (!GTK_WIDGET_VISIBLE (options_shell))
    {
      gtk_widget_show (options_shell);
    }
  else
    {
      gdk_window_raise (options_shell->window);
    }
}

void
tool_options_dialog_free (void)
{
  session_get_window_info (options_shell, &tool_options_session_info);
  gtk_widget_destroy (options_shell);
}

void
tool_options_dialog_add (ToolOptions *tool_options)
{
  g_return_if_fail (tool_options != NULL);

  if (options_vbox)
    {
      if (! GTK_WIDGET_VISIBLE (tool_options->main_vbox))
	{
	  gtk_box_pack_start (GTK_BOX (options_vbox), tool_options->main_vbox,
			      FALSE, FALSE, 0);
	}
    }
}


/*  private functions  */

static void
tool_options_dialog_tool_changed (GimpContext  *context,
				  GimpToolInfo *tool_info,
				  gpointer      data)
{
  if (visible_tool_options &&
      (! tool_info || tool_info->tool_options != visible_tool_options))
    {
      gtk_widget_hide (visible_tool_options->main_vbox);

      visible_tool_options = NULL;
    }

  if (tool_info)
    {
      if (tool_info->tool_options)
	{
	  gtk_widget_show (tool_info->tool_options->main_vbox);

	  visible_tool_options = tool_info->tool_options;

	  gtk_label_set_text (GTK_LABEL (options_label),
			      tool_info->blurb);

	  if (tool_info->tool_options->reset_func)
	    gtk_widget_set_sensitive (options_reset_button, TRUE);
	  else
	    gtk_widget_set_sensitive (options_reset_button, FALSE);
	}
      else
	{
	  gtk_widget_set_sensitive (options_reset_button, FALSE);
	}

      gimp_preview_set_viewable (GIMP_PREVIEW (options_preview),
				 GIMP_VIEWABLE (tool_info));

      gimp_help_set_help_data (options_label->parent->parent,
			       tool_info->help,
			       tool_info->help_data);

    }
}

static void
tool_options_dialog_drop_tool (GtkWidget    *widget,
			       GimpViewable *viewable,
			       gpointer      data)
{
  gimp_context_set_tool (gimp_context_get_user (), GIMP_TOOL_INFO (viewable));
}

GimpViewable *
tool_options_dialog_drag_tool (GtkWidget *widget,
			       gpointer   data)
{
  return (GimpViewable *) gimp_context_get_tool (gimp_context_get_user ());
}

static void
tool_options_dialog_close_callback (GtkWidget *widget,
				    gpointer   data)
{
  GtkWidget *shell;

  shell = (GtkWidget *) data;

  gimp_dialog_hide (shell);
}

static void
tool_options_dialog_reset_callback (GtkWidget *widget,
				    gpointer   data)
{
  GimpToolInfo *tool_info;
  GtkWidget    *shell;

  shell = (GtkWidget *) data;

  if (! active_tool)
    return;

  tool_info = tool_manager_get_info_by_tool (active_tool);

  if (! tool_info)
    {
      g_warning ("%s(): no tool info registered for %s",
                 G_GNUC_FUNCTION,
		 gtk_type_name (GTK_OBJECT (active_tool)->klass->type));
    }

  if (tool_info->tool_options->reset_func)
    tool_info->tool_options->reset_func (tool_info->tool_options);
}
