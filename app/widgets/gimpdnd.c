/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1997 Spencer Kimball and Peter Mattis
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
#include <string.h>

#include <gtk/gtk.h>

#include "libgimpcolor/gimpcolor.h"
#include "libgimpwidgets/gimpwidgets.h"
#include "libgimp/gimplimits.h"

#include "widgets-types.h"

#include "core/gimpimage.h"
#include "core/gimpbrush.h"
#include "core/gimpchannel.h"
#include "core/gimpcontainer.h"
#include "core/gimpdatafactory.h"
#include "core/gimpdrawable.h"
#include "core/gimpgradient.h"
#include "core/gimplayer.h"
#include "core/gimplayermask.h"
#include "core/gimppalette.h"
#include "core/gimppattern.h"
#include "core/gimptoolinfo.h"

#include "tools/gimptool.h"
#include "tools/tool_manager.h"

#include "gui/file-open-dialog.h"

#include "context_manager.h"
#include "gimprc.h"

#include "gimpdnd.h"
#include "gimppreview.h"


#define DRAG_PREVIEW_SIZE 32
#define DRAG_ICON_OFFSET  -8


typedef enum
{
  GIMP_DND_DATA_NONE,
  GIMP_DND_DATA_FIRST,

  GIMP_DND_DATA_FILE_URI_LIST = GIMP_DND_DATA_FIRST,
  GIMP_DND_DATA_FILE_TEXT_PLAIN,
  GIMP_DND_DATA_FILE_NETSCAPE_URL,
  GIMP_DND_DATA_COLOR,
  GIMP_DND_DATA_IMAGE,
  GIMP_DND_DATA_LAYER,
  GIMP_DND_DATA_CHANNEL,
  GIMP_DND_DATA_LAYER_MASK,
  GIMP_DND_DATA_BRUSH,
  GIMP_DND_DATA_PATTERN,
  GIMP_DND_DATA_GRADIENT,
  GIMP_DND_DATA_PALETTE,
  GIMP_DND_DATA_TOOL,

  GIMP_DND_DATA_LAST = GIMP_DND_DATA_TOOL
} GimpDndDataType;


typedef GtkWidget * (* GimpDndGetIconFunc)  (GtkWidget     *widget,
					     GtkSignalFunc  get_data_func,
					     gpointer       get_data_data);
typedef guchar    * (* GimpDndDragDataFunc) (GtkWidget     *widget,
					     GtkSignalFunc  get_data_func,
					     gpointer       get_data_data,
					     gint          *format,
					     gint          *length);
typedef void        (* GimpDndDropDataFunc) (GtkWidget     *widget,
					     GtkSignalFunc  set_data_func,
					     gpointer       set_data_data,
					     guchar        *vals,
					     gint           format,
					     gint           length);


typedef struct _GimpDndDataDef GimpDndDataDef;

struct _GimpDndDataDef
{
  GtkTargetEntry       target_entry;

  gchar               *set_data_func_name;
  gchar               *set_data_data_name;

  GimpDndGetIconFunc   get_icon_func;
  GimpDndDragDataFunc  get_data_func;
  GimpDndDropDataFunc  set_data_func;
};


static GtkWidget * gimp_dnd_get_viewable_icon (GtkWidget     *widget,
					       GtkSignalFunc  get_viewable_func,
					       gpointer       get_viewable_data);
static GtkWidget * gimp_dnd_get_color_icon    (GtkWidget     *widget,
					       GtkSignalFunc  get_color_func,
					       gpointer       get_color_data);

static guchar    * gimp_dnd_get_color_data    (GtkWidget     *widget,
					       GtkSignalFunc  get_color_func,
					       gpointer       get_color_data,
					       gint          *format,
					       gint          *length);
static guchar    * gimp_dnd_get_image_data    (GtkWidget     *widget,
					       GtkSignalFunc  get_image_func,
					       gpointer       get_image_data,
					       gint          *format,
					       gint          *length);
static guchar    * gimp_dnd_get_drawable_data (GtkWidget     *widget,
					       GtkSignalFunc  get_drawable_func,
					       gpointer       get_drawable_data,
					       gint          *format,
					       gint          *length);
static guchar    * gimp_dnd_get_data_data     (GtkWidget     *widget,
					       GtkSignalFunc  get_data_func,
					       gpointer       get_data_data,
					       gint          *format,
					       gint          *length);
static guchar    * gimp_dnd_get_tool_data     (GtkWidget     *widget,
					       GtkSignalFunc  get_tool_func,
					       gpointer       get_tool_data,
					       gint          *format,
					       gint          *length);

static void        gimp_dnd_set_file_data     (GtkWidget     *widget,
					       GtkSignalFunc  set_color_func,
					       gpointer       set_color_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_color_data    (GtkWidget     *widget,
					       GtkSignalFunc  set_color_func,
					       gpointer       set_color_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_image_data    (GtkWidget     *widget,
					       GtkSignalFunc  set_image_func,
					       gpointer       set_image_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_drawable_data (GtkWidget     *widget,
					       GtkSignalFunc  set_drawable_func,
					       gpointer       set_drawable_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_brush_data    (GtkWidget     *widget,
					       GtkSignalFunc  set_brush_func,
					       gpointer       set_brush_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_pattern_data  (GtkWidget     *widget,
					       GtkSignalFunc  set_pattern_func,
					       gpointer       set_pattern_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_gradient_data (GtkWidget     *widget,
					       GtkSignalFunc  set_gradient_func,
					       gpointer       set_gradient_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_palette_data  (GtkWidget     *widget,
					       GtkSignalFunc  set_palette_func,
					       gpointer       set_palette_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);
static void        gimp_dnd_set_tool_data     (GtkWidget     *widget,
					       GtkSignalFunc  set_tool_func,
					       gpointer       set_tool_data,
					       guchar        *vals,
					       gint           format,
					       gint           length);


static GimpDndDataDef dnd_data_defs[] =
{
  {
    { NULL, 0, -1 },

    NULL,
    NULL,

    NULL,
    NULL,
    NULL
  },

  {
    GIMP_TARGET_URI_LIST,

    "gimp_dnd_set_file_func",
    "gimp_dnd_set_file_data",

    NULL,
    NULL,
    gimp_dnd_set_file_data
  },

  {
    GIMP_TARGET_TEXT_PLAIN,

    "gimp_dnd_set_file_func",
    "gimp_dnd_set_file_data",

    NULL,
    NULL,
    gimp_dnd_set_file_data
  },

  {
    GIMP_TARGET_NETSCAPE_URL,

    "gimp_dnd_set_file_func",
    "gimp_dnd_set_file_data",

    NULL,
    NULL,
    gimp_dnd_set_file_data
  },

  {
    GIMP_TARGET_COLOR,

    "gimp_dnd_set_color_func",
    "gimp_dnd_set_color_data",

    gimp_dnd_get_color_icon,
    gimp_dnd_get_color_data,
    gimp_dnd_set_color_data
  },

  {
    GIMP_TARGET_IMAGE,

    "gimp_dnd_set_image_func",
    "gimp_dnd_set_image_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_image_data,
    gimp_dnd_set_image_data,
  },

  {
    GIMP_TARGET_LAYER,

    "gimp_dnd_set_layer_func",
    "gimp_dnd_set_layer_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_drawable_data,
    gimp_dnd_set_drawable_data,
  },

  {
    GIMP_TARGET_CHANNEL,

    "gimp_dnd_set_channel_func",
    "gimp_dnd_set_channel_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_drawable_data,
    gimp_dnd_set_drawable_data,
  },

  {
    GIMP_TARGET_LAYER_MASK,

    "gimp_dnd_set_layer_mask_func",
    "gimp_dnd_set_layer_mask_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_drawable_data,
    gimp_dnd_set_drawable_data,
  },

  {
    GIMP_TARGET_BRUSH,

    "gimp_dnd_set_brush_func",
    "gimp_dnd_set_brush_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_data_data,
    gimp_dnd_set_brush_data
  },

  {
    GIMP_TARGET_PATTERN,

    "gimp_dnd_set_pattern_func",
    "gimp_dnd_set_pattern_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_data_data,
    gimp_dnd_set_pattern_data
  },

  {
    GIMP_TARGET_GRADIENT,

    "gimp_dnd_set_gradient_func",
    "gimp_dnd_set_gradient_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_data_data,
    gimp_dnd_set_gradient_data
  },

  {
    GIMP_TARGET_PALETTE,

    "gimp_dnd_set_palette_func",
    "gimp_dnd_set_palette_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_data_data,
    gimp_dnd_set_palette_data
  },

  {
    GIMP_TARGET_TOOL,

    "gimp_dnd_set_tool_func",
    "gimp_dnd_set_tool_data",

    gimp_dnd_get_viewable_icon,
    gimp_dnd_get_tool_data,
    gimp_dnd_set_tool_data
  }
};


/********************************/
/*  general data dnd functions  */
/********************************/

static void
gimp_dnd_data_drag_begin (GtkWidget      *widget,
			  GdkDragContext *context,
			  gpointer        data)
{
  GimpDndDataType data_type;
  GtkSignalFunc   get_data_func;
  gpointer        get_data_data;
  GtkWidget      *icon_widget;

  data_type =
    (GimpDndDataType) gtk_object_get_data (GTK_OBJECT (widget),
					   "gimp_dnd_get_data_type");

  if (! data_type)
    return;

  get_data_func =
    (GtkSignalFunc) gtk_object_get_data (GTK_OBJECT (widget),
					 "gimp_dnd_get_data_func");
  get_data_data =
    (gpointer) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_get_data_data");

  if (! get_data_func)
    return;

  icon_widget = (* dnd_data_defs[data_type].get_icon_func) (widget,
							    get_data_func,
							    get_data_data);

  if (icon_widget)
    {
      GtkWidget *frame;
      GtkWidget *window;

      window = gtk_window_new (GTK_WINDOW_POPUP);
      gtk_widget_realize (window);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
      gtk_container_add (GTK_CONTAINER (window), frame);
      gtk_widget_show (frame);

      gtk_container_add (GTK_CONTAINER (frame), icon_widget);
      gtk_widget_show (icon_widget);

      gtk_object_set_data_full (GTK_OBJECT (widget),
				"gimp-dnd-data-widget",
				window,
				(GtkDestroyNotify) gtk_widget_destroy);

      gtk_drag_set_icon_widget (context, window,
				DRAG_ICON_OFFSET, DRAG_ICON_OFFSET);
    }
}

static void
gimp_dnd_data_drag_end (GtkWidget      *widget,
			GdkDragContext *context)
{
  gtk_object_set_data (GTK_OBJECT (widget),
		       "gimp-dnd-data-widget", NULL);
}

static void
gimp_dnd_data_drag_handle (GtkWidget        *widget, 
			   GdkDragContext   *context,
			   GtkSelectionData *selection_data,
			   guint             info,
			   guint             time,
			   gpointer          data)
{
  GimpDndDataType data_type;
  GtkSignalFunc   get_data_func;
  gpointer        get_data_data;
  gint    format;
  guchar *vals;
  gint    length;

  data_type =
    (GimpDndDataType) gtk_object_get_data (GTK_OBJECT (widget),
					   "gimp_dnd_get_data_type");

  if (! data_type)
    return;

  get_data_func =
    (GtkSignalFunc) gtk_object_get_data (GTK_OBJECT (widget),
					 "gimp_dnd_get_data_func");
  get_data_data =
    (gpointer) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_get_data_data");

  if (! get_data_func)
    return;

  vals = (* dnd_data_defs[data_type].get_data_func) (widget,
						     get_data_func,
						     get_data_data,
						     &format,
						     &length);

  if (vals)
    {
      gtk_selection_data_set
	(selection_data,
	 gdk_atom_intern (dnd_data_defs[data_type].target_entry.target, FALSE),
	 format, vals, length);

      g_free (vals);
    }
}

static void
gimp_dnd_data_drop_handle (GtkWidget        *widget, 
			   GdkDragContext   *context,
			   gint              x,
			   gint              y,
			   GtkSelectionData *selection_data,
			   guint             info,
			   guint             time,
			   gpointer          data)
{
  GtkSignalFunc   set_data_func;
  gpointer        set_data_data;
  GimpDndDataType data_type;

  if (selection_data->length < 0)
    return;

  for (data_type = GIMP_DND_DATA_FIRST;
       data_type <= GIMP_DND_DATA_LAST;
       data_type++)
    {
      if (dnd_data_defs[data_type].target_entry.info == info)
	{
	  set_data_func = (GtkSignalFunc)
	    gtk_object_get_data (GTK_OBJECT (widget),
				 dnd_data_defs[data_type].set_data_func_name);
	  set_data_data = (gpointer)
	    gtk_object_get_data (GTK_OBJECT (widget),
				 dnd_data_defs[data_type].set_data_data_name);

	  if (! set_data_func)
	    return;

	  (* dnd_data_defs[data_type].set_data_func) (widget,
						      set_data_func,
						      set_data_data,
						      selection_data->data,
						      selection_data->format,
						      selection_data->length);

	  return;
	}
    }
}

static void
gimp_dnd_data_source_set (GimpDndDataType  data_type,
			  GtkWidget       *widget,
			  GtkSignalFunc    get_data_func,
			  gpointer         get_data_data)
{
  gboolean drag_connected;

  drag_connected =
    (gboolean) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_drag_connected");

  if (! drag_connected)
    {
      gtk_signal_connect (GTK_OBJECT (widget), "drag_begin",
			  GTK_SIGNAL_FUNC (gimp_dnd_data_drag_begin),
			  NULL);
      gtk_signal_connect (GTK_OBJECT (widget), "drag_end",
			  GTK_SIGNAL_FUNC (gimp_dnd_data_drag_end),
			  NULL);
      gtk_signal_connect (GTK_OBJECT (widget), "drag_data_get",
			  GTK_SIGNAL_FUNC (gimp_dnd_data_drag_handle),
			  NULL);

      gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_drag_connected",
			   (gpointer) TRUE);
    }

  gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_get_data_type",
		       (gpointer) data_type);
  gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_get_data_func",
		       get_data_func);
  gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_get_data_data",
		       get_data_data);
}

static void
gimp_dnd_data_source_unset (GtkWidget *widget)
{
  gboolean drag_connected;

  drag_connected =
    (gboolean) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_drag_connected");

  if (! drag_connected)
    return;

  gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_get_data_type", NULL);
  gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_get_data_func", NULL);
  gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_get_data_data", NULL);
}

static void
gimp_dnd_data_dest_set (GimpDndDataType  data_type,
			GtkWidget       *widget,
			gpointer         set_data_func,
			gpointer         set_data_data)
{
  gboolean drop_connected;

  drop_connected =
    (gboolean) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_drop_connected");

  if (! drop_connected)
    {
      gtk_signal_connect (GTK_OBJECT (widget), "drag_data_received",
			  GTK_SIGNAL_FUNC (gimp_dnd_data_drop_handle),
			  NULL);

      gtk_object_set_data (GTK_OBJECT (widget), "gimp_dnd_drop_connected",
			   (gpointer) TRUE);
    }

  gtk_object_set_data (GTK_OBJECT (widget),
		       dnd_data_defs[data_type].set_data_func_name,
		       set_data_func);
  gtk_object_set_data (GTK_OBJECT (widget),
		       dnd_data_defs[data_type].set_data_data_name,
		       set_data_data);
}

static void
gimp_dnd_data_dest_unset (GimpDndDataType  data_type,
			  GtkWidget       *widget)
{
  gboolean drop_connected;

  drop_connected =
    (gboolean) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_drop_connected");

  if (! drop_connected)
    return;

  gtk_object_set_data (GTK_OBJECT (widget),
		       dnd_data_defs[data_type].set_data_func_name,
		       NULL);
  gtk_object_set_data (GTK_OBJECT (widget),
		       dnd_data_defs[data_type].set_data_data_name,
		       NULL);
}


/************************/
/*  file dnd functions  */
/************************/

static void
gimp_dnd_set_file_data (GtkWidget     *widget,
			GtkSignalFunc  set_file_func,
			gpointer       set_file_data,
			guchar        *vals,
			gint           format,
			gint           length)
{
  GList *files = NULL;
  gchar *buffer;

  if (format != 8)
    {
      g_warning ("Received invalid file data\n");
      return;
    }

  buffer = (gchar *) vals;

  {
    gchar  name_buffer[1024];
    const gchar *data_type = "file:";
    const gint   sig_len = strlen (data_type);

    while (*buffer)
      {
	gchar *name = name_buffer;
	gint len = 0;

	while ((*buffer != 0) && (*buffer != '\n') && len < 1024)
	  {
	    *name++ = *buffer++;
	    len++;
	  }
	if (len == 0)
	  break;

	if (*(name - 1) == 0xd)   /* gmc uses RETURN+NEWLINE as delimiter */
	  *(name - 1) = '\0';
	else
	  *name = '\0';

	name = name_buffer;

	if ((sig_len < len) && (! strncmp (name, data_type, sig_len)))
	  name += sig_len;

	if (name && strlen (name) > 2)
	  files = g_list_append (files, name);

	if (*buffer)
	  buffer++;
      }
  }

  if (files)
    {
      (* (GimpDndDropFileFunc) set_file_func) (widget, files,
					       set_file_data);

      g_list_free (files);
    }
}

void
gimp_dnd_file_dest_set (GtkWidget           *widget,
			GimpDndDropFileFunc  set_file_func,
			gpointer             data)
{
  gimp_dnd_data_dest_set (GIMP_DND_DATA_FILE_URI_LIST, widget,
			  GTK_SIGNAL_FUNC (set_file_func),
			  data);
  gimp_dnd_data_dest_set (GIMP_DND_DATA_FILE_TEXT_PLAIN, widget,
			  GTK_SIGNAL_FUNC (set_file_func),
			  data);
  gimp_dnd_data_dest_set (GIMP_DND_DATA_FILE_NETSCAPE_URL, widget,
			  GTK_SIGNAL_FUNC (set_file_func),
			  data);
}

void
gimp_dnd_file_dest_unset (GtkWidget *widget)
{
  gimp_dnd_data_dest_unset (GIMP_DND_DATA_FILE_URI_LIST, widget);
  gimp_dnd_data_dest_unset (GIMP_DND_DATA_FILE_TEXT_PLAIN, widget);
  gimp_dnd_data_dest_unset (GIMP_DND_DATA_FILE_NETSCAPE_URL, widget);
}

void
gimp_dnd_open_files (GtkWidget *widget,
		     GList     *files,
		     gpointer   data)
{
  GList *list;

  for (list = files; list; list = g_list_next (list))
    {
      gchar *filename = (gchar *) list->data;

      file_open_with_display (filename, filename);
    }
}


/*************************/
/*  color dnd functions  */
/*************************/

static GtkWidget *
gimp_dnd_get_color_icon (GtkWidget     *widget,
			 GtkSignalFunc  get_color_func,
			 gpointer       get_color_data)
{
  GtkWidget *color_area;
  GimpRGB    color;

  (* (GimpDndDragColorFunc) get_color_func) (widget, &color,
					     get_color_data);

  color_area = gimp_color_area_new (&color, TRUE, 0);
  gtk_widget_set_usize (color_area, DRAG_PREVIEW_SIZE, DRAG_PREVIEW_SIZE);

  return color_area;
}

static guchar *
gimp_dnd_get_color_data (GtkWidget     *widget,
			 GtkSignalFunc  get_color_func,
			 gpointer       get_color_data,
			 gint          *format,
			 gint          *length)
{
  guint16 *vals;
  GimpRGB  color;
  guchar   r, g, b, a;

  (* (GimpDndDragColorFunc) get_color_func) (widget, &color,
					     get_color_data);

  vals = g_new (guint16, 4);

  gimp_rgba_get_uchar (&color, &r, &g, &b, &a);

  vals[0] = r + (r << 8);
  vals[1] = g + (g << 8);
  vals[2] = b + (b << 8);
  vals[3] = a + (a << 8);

  *format = 16;
  *length = 8;

  return (guchar *) vals;
}

static void
gimp_dnd_set_color_data (GtkWidget     *widget,
			 GtkSignalFunc  set_color_func,
			 gpointer       set_color_data,
			 guchar        *vals,
			 gint           format,
			 gint           length)
{
  guint16 *color_vals;
  GimpRGB  color;

  if ((format != 16) || (length != 8))
    {
      g_warning ("Received invalid color data\n");
      return;
    }

  color_vals = (guint16 *) vals;

  gimp_rgba_set_uchar (&color,
		       (guchar) (color_vals[0] >> 8),
		       (guchar) (color_vals[1] >> 8),
		       (guchar) (color_vals[2] >> 8),
		       (guchar) (color_vals[3] >> 8));

  (* (GimpDndDropColorFunc) set_color_func) (widget, &color,
					     set_color_data);
}

void
gimp_dnd_color_source_set (GtkWidget            *widget,
			   GimpDndDragColorFunc  get_color_func,
			   gpointer              data)
{
  gimp_dnd_data_source_set (GIMP_DND_DATA_COLOR, widget,
			    GTK_SIGNAL_FUNC (get_color_func),
			    data);
}

void
gimp_dnd_color_dest_set (GtkWidget            *widget,
			 GimpDndDropColorFunc  set_color_func,
			 gpointer              data)
{
  gimp_dnd_data_dest_set (GIMP_DND_DATA_COLOR, widget,
			  GTK_SIGNAL_FUNC (set_color_func),
			  data);
}

void
gimp_dnd_color_dest_unset (GtkWidget *widget)
{
  gimp_dnd_data_dest_unset (GIMP_DND_DATA_COLOR, widget);
}


/*********************************************/
/*  GimpViewable (by GtkType) dnd functions  */
/*********************************************/

static GtkWidget *
gimp_dnd_get_viewable_icon (GtkWidget     *widget,
			    GtkSignalFunc  get_viewable_func,
			    gpointer       get_viewable_data)
{
  GtkWidget    *preview;
  GimpViewable *viewable;

  viewable = (* (GimpDndDragViewableFunc) get_viewable_func) (widget,
							      get_viewable_data);

  if (! viewable)
    return NULL;

  preview = gimp_preview_new (viewable, DRAG_PREVIEW_SIZE, 0, TRUE);

  return preview;
}

static GimpDndDataType
gimp_dnd_data_type_get_by_gtk_type (GtkType  type)
{
  GimpDndDataType dnd_type = GIMP_DND_DATA_NONE;

  if (gtk_type_is_a (type, GIMP_TYPE_IMAGE))
    {
      dnd_type = GIMP_DND_DATA_IMAGE;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_LAYER))
    {
      dnd_type = GIMP_DND_DATA_LAYER;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_LAYER_MASK))
    {
      dnd_type = GIMP_DND_DATA_LAYER_MASK;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_CHANNEL))
    {
      dnd_type = GIMP_DND_DATA_CHANNEL;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_BRUSH))
    {
      dnd_type = GIMP_DND_DATA_BRUSH;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_PATTERN))
    {
      dnd_type = GIMP_DND_DATA_PATTERN;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_GRADIENT))
    {
      dnd_type = GIMP_DND_DATA_GRADIENT;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_PALETTE))
    {
      dnd_type = GIMP_DND_DATA_PALETTE;
    }
  else if (gtk_type_is_a (type, GIMP_TYPE_TOOL_INFO))
    {
      dnd_type = GIMP_DND_DATA_TOOL;
    }
  else
    {
      g_warning ("%s(): unsupported GtkType \"%s\"",
		 G_GNUC_FUNCTION, gtk_type_name (type));
    }

  return dnd_type;
}

void
gimp_gtk_drag_source_set_by_type (GtkWidget       *widget,
				  GdkModifierType  start_button_mask,
				  GtkType          type,
				  GdkDragAction    actions)
{
  GimpDndDataType dnd_type;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WIDGET (widget));

  dnd_type = gimp_dnd_data_type_get_by_gtk_type (type);

  if (dnd_type == GIMP_DND_DATA_NONE)
    return;

  gtk_drag_source_set (widget, start_button_mask,
		       &dnd_data_defs[dnd_type].target_entry,
		       1,
		       actions);
}

void
gimp_gtk_drag_dest_set_by_type (GtkWidget       *widget,
				GtkDestDefaults  flags,
				GtkType          type,
				GdkDragAction    actions)
{
  GimpDndDataType dnd_type;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WIDGET (widget));

  dnd_type = gimp_dnd_data_type_get_by_gtk_type (type);

  if (dnd_type == GIMP_DND_DATA_NONE)
    return;

  gtk_drag_dest_set (widget, flags,
		     &dnd_data_defs[dnd_type].target_entry,
		     1,
		     actions);
}

void
gimp_dnd_viewable_source_set (GtkWidget               *widget,
			      GtkType                  type,
			      GimpDndDragViewableFunc  get_viewable_func,
			      gpointer                 data)
{
  GimpDndDataType dnd_type;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (get_viewable_func != NULL);

  dnd_type = gimp_dnd_data_type_get_by_gtk_type (type);

  if (dnd_type == GIMP_DND_DATA_NONE)
    return;

  gimp_dnd_data_source_set (dnd_type, widget,
			    GTK_SIGNAL_FUNC (get_viewable_func),
			    data);
}

void
gimp_dnd_viewable_source_unset (GtkWidget *widget,
				GtkType    type)
{
  GimpDndDataType dnd_type;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WIDGET (widget));

  dnd_type = gimp_dnd_data_type_get_by_gtk_type (type);

  if (dnd_type == GIMP_DND_DATA_NONE)
    return;

  gimp_dnd_data_source_unset (widget);
}

void
gimp_dnd_viewable_dest_set (GtkWidget               *widget,
			    GtkType                  type,
			    GimpDndDropViewableFunc  set_viewable_func,
			    gpointer                 data)
{
  GimpDndDataType dnd_type;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (set_viewable_func != NULL);

  dnd_type = gimp_dnd_data_type_get_by_gtk_type (type);

  if (dnd_type == GIMP_DND_DATA_NONE)
    return;

  gimp_dnd_data_dest_set (dnd_type, widget,
			  GTK_SIGNAL_FUNC (set_viewable_func),
			  data);  
}

void
gimp_dnd_viewable_dest_unset (GtkWidget *widget,
			      GtkType    type)
{
  GimpDndDataType dnd_type;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_WIDGET (widget));

  dnd_type = gimp_dnd_data_type_get_by_gtk_type (type);

  if (dnd_type == GIMP_DND_DATA_NONE)
    return;

  gimp_dnd_data_dest_unset (dnd_type, widget);
}

GimpViewable *
gimp_dnd_get_drag_data (GtkWidget *widget)
{
  GimpDndDataType          data_type;
  GimpDndDragViewableFunc  get_data_func;
  gpointer                 get_data_data;

  g_return_val_if_fail (widget != NULL, NULL);
  g_return_val_if_fail (GTK_IS_WIDGET (widget), NULL);

  data_type =
    (GimpDndDataType) gtk_object_get_data (GTK_OBJECT (widget),
					   "gimp_dnd_get_data_type");

  if (! data_type)
    return NULL;

  get_data_func =
    (GimpDndDragViewableFunc) gtk_object_get_data (GTK_OBJECT (widget),
						   "gimp_dnd_get_data_func");
  get_data_data =
    (gpointer) gtk_object_get_data (GTK_OBJECT (widget),
				    "gimp_dnd_get_data_data");

  if (! get_data_func)
    return NULL;

  return (GimpViewable *) (* get_data_func) (widget, get_data_data);
 
}


/*************************/
/*  image dnd functions  */
/*************************/

static guchar *
gimp_dnd_get_image_data (GtkWidget     *widget,
			 GtkSignalFunc  get_image_func,
			 gpointer       get_image_data,
			 gint          *format,
			 gint          *length)
{
  GimpImage *gimage;
  gchar     *id;

  gimage = (GimpImage *)
    (* (GimpDndDragViewableFunc) get_image_func) (widget, get_image_data);

  if (! gimage)
    return NULL;

  id = g_strdup_printf ("%d", gimp_image_get_ID (gimage));

  *format = 8;
  *length = strlen (id) + 1;

  return (guchar *) id;
}

static void
gimp_dnd_set_image_data (GtkWidget     *widget,
			 GtkSignalFunc  set_image_func,
			 gpointer       set_image_data,
			 guchar        *vals,
			 gint           format,
			 gint           length)
{
  GimpImage *gimage;
  gchar     *id;
  gint       ID;

  if ((format != 8) || (length < 1))
    {
      g_warning ("%s(): received invalid image ID data", G_GNUC_FUNCTION);
      return;
    }

  id = (gchar *) vals;

  ID = atoi (id);

  if (! ID)
    return;

  gimage = gimp_image_get_by_ID (ID);

  if (gimage)
    (* (GimpDndDropViewableFunc) set_image_func) (widget,
						  GIMP_VIEWABLE (gimage),
						  set_image_data);
}


/****************************/
/*  drawable dnd functions  */
/****************************/

static guchar *
gimp_dnd_get_drawable_data (GtkWidget     *widget,
			    GtkSignalFunc  get_drawable_func,
			    gpointer       get_drawable_data,
			    gint          *format,
			    gint          *length)
{
  GimpDrawable *drawable;
  gchar        *id;

  drawable = (GimpDrawable *)
    (* (GimpDndDragViewableFunc) get_drawable_func) (widget, get_drawable_data);

  if (! drawable)
    return NULL;

  id = g_strdup_printf ("%d", gimp_drawable_get_ID (drawable));

  *format = 8;
  *length = strlen (id) + 1;

  return (guchar *) id;
}

static void
gimp_dnd_set_drawable_data (GtkWidget     *widget,
			    GtkSignalFunc  set_drawable_func,
			    gpointer       set_drawable_data,
			    guchar        *vals,
			    gint           format,
			    gint           length)
{
  GimpDrawable *drawable;
  gchar        *id;
  gint          ID;

  if ((format != 8) || (length < 1))
    {
      g_warning ("Received invalid drawable ID data");
      return;
    }

  id = (gchar *) vals;

  ID = atoi (id);

  if (! ID)
    return;

  drawable = gimp_drawable_get_by_ID (ID);

  if (drawable)
    (* (GimpDndDropViewableFunc) set_drawable_func) (widget,
						     GIMP_VIEWABLE (drawable),
						     set_drawable_data);
}


/****************************/
/*  GimpData dnd functions  */
/****************************/

static guchar *
gimp_dnd_get_data_data (GtkWidget     *widget,
			GtkSignalFunc  get_data_func,
			gpointer       get_data_data,
			gint          *format,
			gint          *length)
{
  GimpData *data;
  gchar    *name;

  data = (GimpData *)
    (* (GimpDndDragViewableFunc) get_data_func) (widget, get_data_data);

  if (! data)
    return NULL;

  name = g_strdup (gimp_object_get_name (GIMP_OBJECT (data)));

  if (! name)
    return NULL;

  *format = 8;
  *length = strlen (name) + 1;

  return (guchar *) name;
}


/*************************/
/*  brush dnd functions  */
/*************************/

static void
gimp_dnd_set_brush_data (GtkWidget     *widget,
			 GtkSignalFunc  set_brush_func,
			 gpointer       set_brush_data,
			 guchar        *vals,
			 gint           format,
			 gint           length)
{
  GimpBrush *brush;
  gchar     *name;

  if ((format != 8) || (length < 1))
    {
      g_warning ("Received invalid brush data\n");
      return;
    }

  name = (gchar *) vals;

  if (strcmp (name, "Standard") == 0)
    brush = GIMP_BRUSH (gimp_brush_get_standard ());
  else
    brush = (GimpBrush *)
      gimp_container_get_child_by_name (global_brush_factory->container,
					name);

  if (brush)
    (* (GimpDndDropViewableFunc) set_brush_func) (widget,
						  GIMP_VIEWABLE (brush),
						  set_brush_data);
}


/***************************/
/*  pattern dnd functions  */
/***************************/

static void
gimp_dnd_set_pattern_data (GtkWidget     *widget,
			   GtkSignalFunc  set_pattern_func,
			   gpointer       set_pattern_data,
			   guchar        *vals,
			   gint           format,
			   gint           length)
{
  GimpPattern *pattern;
  gchar       *name;

  if ((format != 8) || (length < 1))
    {
      g_warning ("Received invalid pattern data\n");
      return;
    }

  name = (gchar *) vals;

  if (strcmp (name, "Standard") == 0)
    pattern = GIMP_PATTERN (gimp_pattern_get_standard ());
  else
    pattern = (GimpPattern *)
      gimp_container_get_child_by_name (global_pattern_factory->container,
					name);

  if (pattern)
    (* (GimpDndDropViewableFunc) set_pattern_func) (widget,
						    GIMP_VIEWABLE (pattern),
						    set_pattern_data);
}


/****************************/
/*  gradient dnd functions  */
/****************************/

static void
gimp_dnd_set_gradient_data (GtkWidget     *widget,
			    GtkSignalFunc  set_gradient_func,
			    gpointer       set_gradient_data,
			    guchar        *vals,
			    gint           format,
			    gint           length)
{
  GimpGradient *gradient;
  gchar        *name;

  if ((format != 8) || (length < 1))
    {
      g_warning ("Received invalid gradient data\n");
      return;
    }

  name = (gchar *) vals;

  if (strcmp (name, "Standard") == 0)
    gradient = GIMP_GRADIENT (gimp_gradient_get_standard ());
  else
    gradient = (GimpGradient *)
      gimp_container_get_child_by_name (global_gradient_factory->container,
					name);

  if (gradient)
    (* (GimpDndDropViewableFunc) set_gradient_func) (widget,
						     GIMP_VIEWABLE (gradient),
						     set_gradient_data);
}


/***************************/
/*  palette dnd functions  */
/***************************/

static void
gimp_dnd_set_palette_data (GtkWidget     *widget,
			   GtkSignalFunc  set_palette_func,
			   gpointer       set_palette_data,
			   guchar        *vals,
			   gint           format,
			   gint           length)
{
  GimpPalette *palette;
  gchar       *name;

  if ((format != 8) || (length < 1))
    {
      g_warning ("Received invalid palette data\n");
      return;
    }

  name = (gchar *) vals;

  if (strcmp (name, "Standard") == 0)
    palette = GIMP_PALETTE (gimp_palette_get_standard ());
  else
    palette = (GimpPalette *)
      gimp_container_get_child_by_name (global_palette_factory->container,
					name);

  if (palette)
    (* (GimpDndDropViewableFunc) set_palette_func) (widget,
						    GIMP_VIEWABLE (palette),
						    set_palette_data);
}


/************************/
/*  tool dnd functions  */
/************************/

static guchar *
gimp_dnd_get_tool_data (GtkWidget     *widget,
			GtkSignalFunc  get_tool_func,
			gpointer       get_tool_data,
			gint          *format,
			gint          *length)
{
  GimpToolInfo *tool_info;
  gchar        *name;

  tool_info = (GimpToolInfo *)
    (* (GimpDndDragViewableFunc) get_tool_func) (widget, get_tool_data);

  if (! tool_info)
    return NULL;

  name = g_strdup (gimp_object_get_name (GIMP_OBJECT (tool_info)));

  if (! name)
    return NULL;

  *format = 8;
  *length = strlen (name) + 1;

  return (guchar *) name;
}

static void
gimp_dnd_set_tool_data (GtkWidget     *widget,
			GtkSignalFunc  set_tool_func,
			gpointer       set_tool_data,
			guchar        *vals,
			gint           format,
			gint           length)
{
  GimpToolInfo *tool_info;
  gchar        *name;

  if ((format != 8) || (length < 1))
    {
      g_warning ("Received invalid tool data\n");
      return;
    }

  name = (gchar *) vals;

  if (strcmp (name, "gimp:standard_tool") == 0)
    tool_info = gimp_tool_info_get_standard ();
  else
    tool_info = (GimpToolInfo *)
      gimp_container_get_child_by_name (global_tool_info_list,
					name);

  if (tool_info)
    (* (GimpDndDropViewableFunc) set_tool_func) (widget,
						 GIMP_VIEWABLE (tool_info),
						 set_tool_data);
}
