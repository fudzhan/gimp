/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimptooloptionseditor.h
 * Copyright (C) 2003 Michael Natterer <mitch@gimp.org>
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

#ifndef __GIMP_TOOL_OPTIONS_EDITOR_H__
#define __GIMP_TOOL_OPTIONS_EDITOR_H__


#include "gimpeditor.h"


#define GIMP_TYPE_TOOL_OPTIONS_EDITOR            (gimp_tool_options_editor_get_type ())
#define GIMP_TOOL_OPTIONS_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_TOOL_OPTIONS_EDITOR, GimpToolOptionsEditor))
#define GIMP_TOOL_OPTIONS_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_TOOL_OPTIONS_EDITOR, GimpToolOptionsEditorClass))
#define GIMP_IS_TOOL_OPTIONS_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_TOOL_OPTIONS_EDITOR))
#define GIMP_IS_TOOL_OPTIONS_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_TOOL_OPTIONS_EDITOR))
#define GIMP_TOOL_OPTIONS_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_TOOL_OPTIONS_EDITOR, GimpToolOptionsEditorClass))


typedef struct _GimpToolOptionsEditorClass  GimpToolOptionsEditorClass;

struct _GimpToolOptionsEditor
{
  GimpEditor       parent_instance;

  Gimp            *gimp;

  GtkWidget       *scrolled_window;
  GtkWidget       *options_vbox;

  GtkWidget       *save_button;
  GtkWidget       *restore_button;
  GtkWidget       *delete_button;
  GtkWidget       *reset_button;

  GimpToolOptions *visible_tool_options;
};

struct _GimpToolOptionsEditorClass
{
  GimpEditorClass  parent_class;
};


GType       gimp_tool_options_editor_get_type  (void) G_GNUC_CONST;

GtkWidget * gimp_tool_options_editor_new       (Gimp            *gimp,
                                                GimpMenuFactory *menu_factory);


#endif  /*  __GIMP_TOOL_OPTIONS_EDITOR_H__  */
