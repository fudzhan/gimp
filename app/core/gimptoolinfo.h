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

#ifndef __GIMP_TOOL_INFO_H__
#define __GIMP_TOOL_INFO_H__


#include <gdk-pixbuf/gdk-pixbuf.h>

#include "gimpdata.h"


#define GIMP_TYPE_TOOL_INFO            (gimp_tool_info_get_type ())
#define GIMP_TOOL_INFO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_TOOL_INFO, GimpToolInfo))
#define GIMP_TOOL_INFO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_TOOL_INFO, GimpToolInfoClass))
#define GIMP_IS_TOOL_INFO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_TOOL_INFO))
#define GIMP_IS_TOOL_INFO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_TOOL_INFO))
#define GIMP_TOOL_INFO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_TOOL_INFO, GimpToolInfoClass))


typedef struct _GimpToolInfoClass GimpToolInfoClass;


struct _GimpToolInfo
{
  GimpData          parent_instance;

  Gimp             *gimp;

  GType             tool_type;

  gchar            *blurb;
  gchar            *help;

  gchar            *menu_path;  
  gchar            *menu_accel; 

  gchar            *help_domain;
  gchar            *help_data;

  gchar            *pdb_string;

  const gchar      *stock_id;
  GdkPixbuf        *stock_pixbuf;

  GimpContext      *context;

  GimpToolOptions  *tool_options;
};

struct _GimpToolInfoClass
{
  GimpDataClass  parent_class;
};


GType          gimp_tool_info_get_type     (void) G_GNUC_CONST;

GimpToolInfo * gimp_tool_info_new          (Gimp         *gimp,
                                            GimpContext  *context,
					    GType         tool_type,
					    gboolean      tool_context,
					    const gchar  *identifier,
					    const gchar  *blurb,
					    const gchar  *help,
					    const gchar  *menu_path,
					    const gchar  *menu_accel,
					    const gchar  *help_domain,
					    const gchar  *help_data,
					    const gchar  *pdb_string,
					    const gchar  *stock_id,
					    GdkPixbuf    *stock_pixbuf);

void           gimp_tool_info_set_standard (Gimp         *gimp,
					    GimpToolInfo *tool_info);
GimpToolInfo * gimp_tool_info_get_standard (Gimp         *gimp);


#endif  /*  __GIMP_TOOL_INFO_H__  */
