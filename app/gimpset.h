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

#ifndef __GIMP_SET_H__
#define __GIMP_SET_H__


#include "gimpobject.h"


/* GimpSet - a (usually) typed set of objects with signals for adding
 * and removing of stuff. If it is weak, destroyed objects get removed
 * automatically. If it is not, it refs them so they won't be freed
 * till they are removed. (Though they can be destroyed, of course).
 *
 * If GTK_TYPE_NONE is specified at gimpset creation time, no type
 * checking is performed by gimp_set_add() and the
 * gimp_set_{add,remove}_handler() functions should not be used.  It
 * is also illegal to ask for a weak untyped gimpset.
*/

#define GIMP_TYPE_SET            (gimp_set_get_type ())
#define GIMP_SET(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_SET, GimpSet))
#define GIMP_SET_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_SET, GimpSetClass))
#define GIMP_IS_SET(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_SET))
#define GIMP_IS_SET_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_SET))


typedef struct _GimpSetClass GimpSetClass;

struct _GimpSet
{
  GimpObject  parent_instance;

  GtkType     type;
  GSList     *list;
  GArray     *handlers;
  gboolean    weak;
  gpointer    active_element;
};

struct _GimpSetClass
{
  GimpObjectClass  parent_class;

  void (* add)            (GimpSet  *gimpset,
			   gpointer  object);
  void (* remove)         (GimpSet  *gimpset,
			   gpointer  object);
  void (* active_changed) (GimpSet  *gimpset,
			   gpointer  object);
};


GtkType            gimp_set_get_type       (void);
GimpSet          * gimp_set_new            (GtkType           type,
					    gboolean          weak);

GtkType		   gimp_set_type	   (GimpSet          *set);

gboolean       	   gimp_set_add	           (GimpSet          *gimpset,
					    gpointer          object);
gboolean       	   gimp_set_remove	   (GimpSet          *gimpset,
					    gpointer          object);

gboolean	   gimp_set_have	   (GimpSet          *gimpset,
					    gpointer          object);
void		   gimp_set_foreach        (GimpSet          *gimpset,
					    GFunc             func,
					    gpointer          user_data);
gint		   gimp_set_size	   (GimpSet          *gimpset);

void		   gimp_set_set_active     (GimpSet          *gimpset,
					    gpointer          object);
gpointer	   gimp_set_get_active     (GimpSet          *gimpset);

GimpSetHandlerId   gimp_set_add_handler    (GimpSet          *set,
					    const gchar      *signame,
					    GtkSignalFunc     handler,
					    gpointer          user_data);
void		   gimp_set_remove_handler (GimpSet          *set,
					    GimpSetHandlerId  id);


#endif /* __GIMP_SET_H__ */
