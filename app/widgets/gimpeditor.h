/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpeditor.h
 * Copyright (C) 2002 Michael Natterer <mitch@gimp.org>
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

#ifndef __GIMP_EDITOR_H__
#define __GIMP_EDITOR_H__


#include <gtk/gtkvbox.h>


#define GIMP_TYPE_EDITOR            (gimp_editor_get_type ())
#define GIMP_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_EDITOR, GimpEditor))
#define GIMP_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_EDITOR, GimpEditorClass))
#define GIMP_IS_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_EDITOR))
#define GIMP_IS_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_EDITOR))
#define GIMP_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_EDITOR, GimpEditorClass))


typedef struct _GimpEditorClass  GimpEditorClass;

struct _GimpEditor
{
  GtkVBox    parent_instance;

  GtkWidget *button_box;
};

struct _GimpEditorClass
{
  GtkVBoxClass  parent_class;
};


GType       gimp_editor_get_type   (void) G_GNUC_CONST;

GtkWidget * gimp_editor_add_button (GimpEditor  *editor,
                                    const gchar *stock_id,
                                    const gchar *tooltip,
                                    const gchar *help_data,
                                    GCallback    callback,
                                    GCallback    extended_callback,
                                    gpointer     callback_data);


#endif  /*  __GIMP_EDITOR_H__  */
