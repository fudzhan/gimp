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

#ifdef __GNUC__
#warning GTK_DISABLE_DEPRECATED
#endif
#undef GTK_DISABLE_DEPRECATED

#include <gtk/gtk.h>

#include "libgimpcolor/gimpcolor.h"
#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "widgets-types.h"

#include "core/gimp.h"
#include "core/gimpcontainer.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimppalette.h"

#include "gimpdnd.h"
#include "gimppaletteeditor.h"
#include "gimppreview.h"

#include "gui/color-area.h"
#include "gui/color-notebook.h"

#include "libgimp/gimpintl.h"


#define ENTRY_WIDTH  12
#define ENTRY_HEIGHT 10
#define SPACING       1
#define COLUMNS      16
#define ROWS         11

#define PREVIEW_WIDTH  ((ENTRY_WIDTH  * COLUMNS) + (SPACING * (COLUMNS + 1)))
#define PREVIEW_HEIGHT ((ENTRY_HEIGHT * ROWS)    + (SPACING * (ROWS    + 1)))

#define PALETTE_EVENT_MASK (GDK_EXPOSURE_MASK     | \
                            GDK_BUTTON_PRESS_MASK | \
                            GDK_ENTER_NOTIFY_MASK)


/*  local function prototypes  */

static void   gimp_palette_editor_class_init (GimpPaletteEditorClass *klass);
static void   gimp_palette_editor_init       (GimpPaletteEditor      *editor);

static void   gimp_palette_editor_destroy          (GtkObject         *object);
static void   gimp_palette_editor_unmap            (GtkWidget         *widget);

static void   gimp_palette_editor_set_data         (GimpDataEditor    *editor,
                                                    GimpData          *data);

static void   palette_editor_create_popup_menu     (GimpPaletteEditor *editor);
static void   palette_editor_new_entry_callback    (GtkWidget         *widget,
						    GimpPaletteEditor *editor);
static void   palette_editor_edit_entry_callback   (GtkWidget         *widget,
						    GimpPaletteEditor *editor);
static void   palette_editor_delete_entry_callback (GtkWidget         *widget,
						    GimpPaletteEditor *editor);
static void   palette_editor_color_notebook_callback (ColorNotebook      *color_notebook,
						      const GimpRGB      *color,
						      ColorNotebookState  state,
						      gpointer            data);

static gint   palette_editor_eventbox_button_press (GtkWidget         *widget,
						    GdkEventButton    *bevent,
						    GimpPaletteEditor *editor);
static gint   palette_editor_color_area_events     (GtkWidget         *widget,
						    GdkEvent          *event,
						    GimpPaletteEditor *editor);
static void   palette_editor_draw_entries          (GimpPaletteEditor *editor,
                                                    gint               row_start,
                                                    gint               column_highlight);
static void   palette_editor_redraw                (GimpPaletteEditor *editor);
static void   palette_editor_scroll_top_left       (GimpPaletteEditor *editor);

static void   palette_editor_color_name_changed    (GtkWidget         *widget,
                                                    GimpPaletteEditor *editor);
static void   palette_editor_zoom_in_callback      (GtkWidget         *widget,
                                                    GimpPaletteEditor *editor);
static void   palette_editor_zoom_out_callback     (GtkWidget         *widget,
                                                    GimpPaletteEditor *editor);
static void   palette_editor_zoom_all_callback     (GtkWidget         *widget,
                                                    GimpPaletteEditor *editor);
static void   palette_editor_redraw_zoom           (GimpPaletteEditor *editor);

static void   palette_editor_drag_color            (GtkWidget         *widget,
                                                    GimpRGB           *color,
                                                    gpointer           data);
static void   palette_editor_drop_color            (GtkWidget         *widget,
                                                    const GimpRGB     *color,
                                                    gpointer           data);
static void   palette_editor_drop_palette          (GtkWidget         *widget,
                                                    GimpViewable      *viewable,
                                                    gpointer           data);
static void   palette_editor_invalidate_preview    (GimpPalette       *palette,
                                                    GimpPaletteEditor *editor);


/*  dnd stuff  */
static GtkTargetEntry color_palette_target_table[] =
{
  GIMP_TARGET_COLOR,
  GIMP_TARGET_PALETTE
};


static GimpDataEditorClass *parent_class = NULL;


GType
gimp_palette_editor_get_type (void)
{
  static GType type = 0;

  if (! type)
    {
      static const GTypeInfo info =
      {
        sizeof (GimpPaletteEditorClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) gimp_palette_editor_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GimpPaletteEditor),
        0,              /* n_preallocs */
        (GInstanceInitFunc) gimp_palette_editor_init,
      };

      type = g_type_register_static (GIMP_TYPE_DATA_EDITOR,
                                     "GimpPaletteEditor",
                                     &info, 0);
    }

  return type;
}

static void
gimp_palette_editor_class_init (GimpPaletteEditorClass *klass)
{
  GtkObjectClass      *object_class;
  GtkWidgetClass      *widget_class;
  GimpDataEditorClass *editor_class;

  object_class = GTK_OBJECT_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  editor_class = GIMP_DATA_EDITOR_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->destroy  = gimp_palette_editor_destroy;

  widget_class->unmap    = gimp_palette_editor_unmap;

  editor_class->set_data = gimp_palette_editor_set_data;
}

static void
gimp_palette_editor_init (GimpPaletteEditor *editor)
{
  GtkWidget *scrolledwindow;
  GtkWidget *palette_region;
  GtkWidget *entry;
  GtkWidget *eventbox;
  GtkWidget *alignment;

  editor->zoom_factor   = 1.0;
  editor->columns       = COLUMNS;
  editor->columns_valid = TRUE;
  editor->freeze_update = FALSE;

  editor->scrolled_window = scrolledwindow =
    gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request (scrolledwindow, -1, PREVIEW_HEIGHT);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_ALWAYS);
  gtk_box_pack_start (GTK_BOX (editor), scrolledwindow, TRUE, TRUE, 0);
  gtk_widget_show (scrolledwindow);

  eventbox = gtk_event_box_new ();
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolledwindow),
					 eventbox);
  gtk_widget_show (eventbox);

  g_signal_connect (G_OBJECT (eventbox), "button_press_event",
		    G_CALLBACK (palette_editor_eventbox_button_press),
		    editor);

  alignment = gtk_alignment_new (0.5, 0.5, 0.0, 0.0); 
  gtk_container_add (GTK_CONTAINER (eventbox), alignment);
  gtk_widget_show (alignment);

  editor->color_area = palette_region = gtk_preview_new (GTK_PREVIEW_COLOR);
  gtk_preview_set_dither (GTK_PREVIEW (editor->color_area), GDK_RGB_DITHER_MAX);
  gtk_preview_size (GTK_PREVIEW (palette_region), PREVIEW_WIDTH, PREVIEW_HEIGHT);

  gtk_widget_set_events (palette_region, PALETTE_EVENT_MASK);
  gtk_container_add (GTK_CONTAINER (alignment), palette_region);
  gtk_widget_show (palette_region);

  g_signal_connect (G_OBJECT (editor->color_area), "event",
		    G_CALLBACK (palette_editor_color_area_events),
		    editor);

  /*  dnd stuff  */
  gtk_drag_source_set (palette_region,
                       GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
                       color_palette_target_table,
                       G_N_ELEMENTS (color_palette_target_table),
                       GDK_ACTION_COPY | GDK_ACTION_MOVE);
  gimp_dnd_color_source_set (palette_region,
                             palette_editor_drag_color,
			     editor);

  gtk_drag_dest_set (eventbox,
                     GTK_DEST_DEFAULT_HIGHLIGHT |
                     GTK_DEST_DEFAULT_MOTION |
                     GTK_DEST_DEFAULT_DROP,
                     color_palette_target_table,
                     G_N_ELEMENTS (color_palette_target_table),
                     GDK_ACTION_COPY);
  gimp_dnd_color_dest_set (eventbox, palette_editor_drop_color, editor);
  gimp_dnd_viewable_dest_set (eventbox, GIMP_TYPE_PALETTE,
			      palette_editor_drop_palette,
                              editor);

  /*  The color name entry  */
  entry = editor->color_name = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (editor), entry, FALSE, FALSE, 0);
  gtk_entry_set_text (GTK_ENTRY (entry), _("Undefined"));
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_show (entry);

  editor->entry_sig_id =
    g_signal_connect (G_OBJECT (entry), "changed",
		      G_CALLBACK (palette_editor_color_name_changed),
		      editor);

  /*  + and - buttons  */
  gimp_editor_add_button (GIMP_EDITOR (editor),
                          GTK_STOCK_ZOOM_FIT,
                          _("Zoom All"), NULL,
                          G_CALLBACK (palette_editor_zoom_all_callback),
                          NULL,
                          editor);

  gimp_editor_add_button (GIMP_EDITOR (editor),
                          GTK_STOCK_ZOOM_IN,
                          _("Zoom In"), NULL,
                          G_CALLBACK (palette_editor_zoom_in_callback),
                          NULL,
                          editor);

  gimp_editor_add_button (GIMP_EDITOR (editor),
                          GTK_STOCK_ZOOM_OUT,
                          _("Zoom Out"), NULL,
                          G_CALLBACK (palette_editor_zoom_out_callback),
                          NULL,
                          editor);

  palette_editor_create_popup_menu (editor);
}

static void
gimp_palette_editor_destroy (GtkObject *object)
{
  GimpPaletteEditor *editor;

  editor = GIMP_PALETTE_EDITOR (object);

  if (editor->color_notebook)
    {
      color_notebook_free (editor->color_notebook);
      editor->color_notebook        = NULL;
      editor->color_notebook_active = FALSE;
    }

  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gimp_palette_editor_unmap (GtkWidget *widget)
{
  GimpPaletteEditor *editor;

  editor = GIMP_PALETTE_EDITOR (widget);

  if (editor->color_notebook_active)
    {
      color_notebook_hide (editor->color_notebook);
      editor->color_notebook_active = FALSE;
    }

  GTK_WIDGET_CLASS (parent_class)->unmap (widget);
}

static void
gimp_palette_editor_set_data (GimpDataEditor *editor,
                              GimpData       *data)
{
  GimpPaletteEditor *palette_editor;

  palette_editor = GIMP_PALETTE_EDITOR (editor);

  if (editor->data)
    {
      if (palette_editor->color_notebook)
        {
          color_notebook_free (palette_editor->color_notebook);
          palette_editor->color_notebook        = NULL;
          palette_editor->color_notebook_active = FALSE;
        }

      g_signal_handlers_disconnect_by_func (G_OBJECT (editor->data),
                                            palette_editor_invalidate_preview,
                                            editor);

      palette_editor->columns_valid = FALSE;
      palette_editor->color         = NULL;

      g_signal_handler_block (G_OBJECT (palette_editor->color_name),
                              palette_editor->entry_sig_id);

      gtk_entry_set_text (GTK_ENTRY (palette_editor->color_name),
                          _("Undefined")); 

      g_signal_handler_unblock (G_OBJECT (palette_editor->color_name),
                                palette_editor->entry_sig_id);

      gtk_widget_set_sensitive (palette_editor->color_name, FALSE);
    }

  GIMP_DATA_EDITOR_CLASS (parent_class)->set_data (editor, data);

  if (editor->data)
    {
      g_signal_connect (G_OBJECT (editor->data), "invalidate_preview",
                        G_CALLBACK (palette_editor_invalidate_preview),
                        editor);

      palette_editor_invalidate_preview (GIMP_PALETTE (editor->data),
                                         palette_editor);

      palette_editor_scroll_top_left (palette_editor);
    }
}

/*  public functions  */

GimpDataEditor *
gimp_palette_editor_new (Gimp *gimp)
{
  GimpPaletteEditor *palette_editor;

  palette_editor = g_object_new (GIMP_TYPE_PALETTE_EDITOR, NULL);

  gimp_data_editor_construct (GIMP_DATA_EDITOR (palette_editor),
                              gimp,
                              GIMP_TYPE_PALETTE);

  return GIMP_DATA_EDITOR (palette_editor);
}

void
gimp_palette_editor_update_color (GimpContext          *context,
                                  const GimpRGB        *color,
                                  GimpUpdateColorState  state)
{
#ifdef __GNUC__
#warning FIXME: palette_set_active_color()
#endif
#if 0
  if (top_level_edit_palette)
    {
      GimpPalette *palette;

      palette = gimp_context_get_palette (top_level_edit_palette->context);

      if (palette)
	{
	  switch (state)
	    {
	    case GIMP_UPDATE_COLOR_STATE_NEW:
	      top_level_edit_palette->color = gimp_palette_add_entry (palette,
								      NULL,
								      color);
	      break;

	    case GIMP_UPDATE_COLOR_STATE_UPDATE_NEW:
	      top_level_edit_palette->color->color = *color;

	      gimp_data_dirty (GIMP_DATA (palette));
	      break;

	    default:
	      break;
	    }
	}
    }
#endif
}


/*  private functions  */

/*  the palette dialog popup menu & callbacks  *******************************/

static void
palette_editor_create_popup_menu (GimpPaletteEditor *palette_editor)
{
  GtkWidget *menu;
  GtkWidget *menu_item;

  palette_editor->popup_menu = menu = gtk_menu_new ();

  menu_item = gtk_menu_item_new_with_label (_("New"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
  gtk_widget_show (menu_item);

  g_signal_connect (G_OBJECT (menu_item), "activate", 
		    G_CALLBACK (palette_editor_new_entry_callback),
		    palette_editor);

  menu_item = gtk_menu_item_new_with_label (_("Edit"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
  gtk_widget_show (menu_item);

  g_signal_connect (G_OBJECT (menu_item), "activate", 
		    G_CALLBACK (palette_editor_edit_entry_callback),
		    palette_editor);

  palette_editor->edit_menu_item = menu_item;

  menu_item = gtk_menu_item_new_with_label (_("Delete"));
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
  gtk_widget_show (menu_item);

  g_signal_connect (G_OBJECT (menu_item), "activate", 
		    G_CALLBACK (palette_editor_delete_entry_callback),
		    palette_editor);

  palette_editor->delete_menu_item = menu_item;
}

static void
palette_editor_new_entry_callback (GtkWidget         *widget,
				   GimpPaletteEditor *editor)
{
  GimpPalette *palette;
  GimpContext *user_context;
  GimpRGB      color;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  if (! palette)
    return;

  user_context = gimp_get_user_context (GIMP_DATA_EDITOR (editor)->gimp);

  if (active_color == FOREGROUND)
    gimp_context_get_foreground (user_context, &color);
  else if (active_color == BACKGROUND)
    gimp_context_get_background (user_context, &color);

  editor->color = gimp_palette_add_entry (palette, NULL, &color);
}

static void
palette_editor_edit_entry_callback (GtkWidget         *widget,
				    GimpPaletteEditor *editor)
{
  GimpPalette *palette;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  if (! (palette && editor->color))
    return;

  if (! editor->color_notebook)
    {
      editor->color_notebook =
	color_notebook_new (_("Edit Palette Color"),
			    (const GimpRGB *) &editor->color->color,
			    palette_editor_color_notebook_callback,
			    editor,
			    FALSE,
			    FALSE);
      editor->color_notebook_active = TRUE;
    }
  else
    {
      if (! editor->color_notebook_active)
	{
	  color_notebook_show (editor->color_notebook);
	  editor->color_notebook_active = TRUE;
	}

      color_notebook_set_color (editor->color_notebook,
				&editor->color->color);
    }
}

static void
palette_editor_delete_entry_callback (GtkWidget         *widget,
				      GimpPaletteEditor *editor)
{
  GimpPalette *palette;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  if (! (palette && editor->color))
    return;

  gimp_palette_delete_entry (palette, editor->color);
}

static void
palette_editor_color_notebook_callback (ColorNotebook      *color_notebook,
					const GimpRGB      *color,
					ColorNotebookState  state,
					gpointer            data)
{
  GimpPaletteEditor *editor;
  GimpPalette       *palette;
  GimpContext       *user_context;

  editor = GIMP_PALETTE_EDITOR (data);

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  user_context = gimp_get_user_context (GIMP_DATA_EDITOR (editor)->gimp);

  switch (state)
    {
    case COLOR_NOTEBOOK_UPDATE:
      break;

    case COLOR_NOTEBOOK_OK:
      if (editor->color)
	{
	  editor->color->color = *color;

	  /*  Update either foreground or background colors  */
	  if (active_color == FOREGROUND)
	    gimp_context_set_foreground (user_context, color);
	  else if (active_color == BACKGROUND)
	    gimp_context_set_background (user_context, color);

	  gimp_data_dirty (GIMP_DATA (palette));
	}

      /* Fallthrough */
    case COLOR_NOTEBOOK_CANCEL:
      if (editor->color_notebook_active)
	{
	  color_notebook_hide (editor->color_notebook);
	  editor->color_notebook_active = FALSE;
	}
    }
}

/*  the color area event callbacks  ******************************************/

static gint
palette_editor_eventbox_button_press (GtkWidget         *widget,
				      GdkEventButton    *bevent,
				      GimpPaletteEditor *editor)
{
  if (gtk_get_event_widget ((GdkEvent *) bevent) == editor->color_area)
    return FALSE;

  if (bevent->button == 3)
    {
      if (GTK_WIDGET_SENSITIVE (editor->edit_menu_item))
	{
	  gtk_widget_set_sensitive (editor->edit_menu_item, FALSE);
	  gtk_widget_set_sensitive (editor->delete_menu_item, FALSE);
	}

      gtk_menu_popup (GTK_MENU (editor->popup_menu), NULL, NULL, 
		      NULL, NULL, 3,
		      bevent->time);
    }

  return TRUE;
}

static gint
palette_editor_color_area_events (GtkWidget         *widget,
				  GdkEvent          *event,
				  GimpPaletteEditor *editor)
{
  GimpPalette    *palette;
  GimpContext    *user_context;
  GdkEventButton *bevent;
  GList          *list;
  gint            entry_width;
  gint            entry_height;
  gint            row, col;
  gint            pos;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  user_context = gimp_get_user_context (GIMP_DATA_EDITOR (editor)->gimp);

  switch (event->type)
    {
    case GDK_EXPOSE:
      palette_editor_redraw (editor);
      break;

    case GDK_BUTTON_PRESS:
      bevent = (GdkEventButton *) event;
      entry_width  = editor->col_width + SPACING;
      entry_height = (ENTRY_HEIGHT * editor->zoom_factor) +  SPACING;
      col = (bevent->x - 1) / entry_width;
      row = (bevent->y - 1) / entry_height;
      pos = row * editor->columns + col;

      if (palette)
	list = g_list_nth (palette->colors, pos);
      else
	list = NULL;

      if (list)
	editor->dnd_color = list->data;
      else
	editor->dnd_color = NULL;

      if ((bevent->button == 1 || bevent->button == 3) && palette)
	{
	  if (list)
	    {
	      if (editor->color)
		{
		  editor->freeze_update = TRUE;
 		  palette_editor_draw_entries (editor, -1, -1);
		  editor->freeze_update = FALSE;
		}
	      editor->color = (GimpPaletteEntry *) list->data;

	      if (active_color == FOREGROUND)
		{
		  if (bevent->state & GDK_CONTROL_MASK)
		    gimp_context_set_background (user_context,
						 &editor->color->color);
		  else
		    gimp_context_set_foreground (user_context,
						 &editor->color->color);
		}
	      else if (active_color == BACKGROUND)
		{
		  if (bevent->state & GDK_CONTROL_MASK)
		    gimp_context_set_foreground (user_context,
						 &editor->color->color);
		  else
		    gimp_context_set_background (user_context,
						 &editor->color->color);
		}

	      palette_editor_draw_entries (editor, row, col);

	      /*  Update the active color name  */
	      g_print ("former color name: '%s'\n",
		       editor->color->name);

	      g_signal_handler_block (G_OBJECT (editor->color_name),
				      editor->entry_sig_id);

	      gtk_entry_set_text (GTK_ENTRY (editor->color_name),
				  editor->color->name);

	      g_signal_handler_unblock (G_OBJECT (editor->color_name),
					editor->entry_sig_id);

	      g_print ("new color name: '%s'\n",
		       editor->color->name);

	      gtk_widget_set_sensitive (editor->color_name, TRUE);
	      /* palette_update_current_entry (editor); */

	      if (bevent->button == 3)
		{
		  if (! GTK_WIDGET_SENSITIVE (editor->edit_menu_item))
		    {
		      gtk_widget_set_sensitive (editor->edit_menu_item, TRUE);
		      gtk_widget_set_sensitive (editor->delete_menu_item, TRUE);
		    }

		  gtk_menu_popup (GTK_MENU (editor->popup_menu),
				  NULL, NULL, 
				  NULL, NULL, 3,
				  bevent->time);
		}
	    }
	  else
	    {
	      if (bevent->button == 3)
		{
		  if (GTK_WIDGET_SENSITIVE (editor->edit_menu_item))
		    {
		      gtk_widget_set_sensitive (editor->edit_menu_item, FALSE);
		      gtk_widget_set_sensitive (editor->delete_menu_item, FALSE);
		    }

		  gtk_menu_popup (GTK_MENU (editor->popup_menu),
				  NULL, NULL, 
				  NULL, NULL, 3,
				  bevent->time);
		}
	    }
	}
      break;

    default:
      break;
    }

  return FALSE;
}

/*  functions for drawing & updating the palette dialog color area  **********/

static gint
palette_editor_draw_color_row (guchar            *colors,
			       gint               n_colors,
			       gint               y,
			       gint               column_highlight,
			       guchar            *buffer,
			       GimpPaletteEditor *palette_editor)
{
  guchar    *p;
  guchar     bcolor;
  gint       width, height;
  gint       entry_width;
  gint       entry_height;
  gint       vsize;
  gint       vspacing;
  gint       i, j;
  GtkWidget *preview;

  if (! palette_editor)
    return -1;

  preview = palette_editor->color_area;

  bcolor = 0;

  width        = preview->requisition.width;
  height       = preview->requisition.height;
  entry_width  = palette_editor->col_width;
  entry_height = (ENTRY_HEIGHT * palette_editor->zoom_factor);

  if ((y >= 0) && ((y + SPACING) < height))
    vspacing = SPACING;
  else if (y < 0)
    vspacing = SPACING + y;
  else
    vspacing = height - y;

  if (vspacing > 0)
    {
      if (y < 0)
	y += SPACING - vspacing;

      for (i = SPACING - vspacing; i < SPACING; i++, y++)
	{
	  p = buffer;
	  for (j = 0; j < width; j++)
	    {
	      *p++ = bcolor;
	      *p++ = bcolor;
	      *p++ = bcolor;
	    }
	  
	  if (column_highlight >= 0)
	    {
	      guchar *ph;

	      ph = &buffer[3 * column_highlight * (entry_width + SPACING)];

	      for (j = 0 ; j <= entry_width + SPACING; j++)
		{
		  *ph++ = ~bcolor;
		  *ph++ = ~bcolor;
		  *ph++ = ~bcolor;
		}

 	      gtk_preview_draw_row (GTK_PREVIEW (preview), buffer, 0,
				    y + entry_height + 1, width); 
	    }

	  gtk_preview_draw_row (GTK_PREVIEW (preview), buffer, 0, y, width);
	}

      if (y > SPACING)
	y += SPACING - vspacing;
    }
  else
    y += SPACING;

  vsize = (y >= 0) ? (entry_height) : (entry_height + y);

  if ((y >= 0) && ((y + entry_height) < height))
    vsize = entry_height;
  else if (y < 0)
    vsize = entry_height + y;
  else
    vsize = height - y;

  if (vsize > 0)
    {
      p = buffer;
      for (i = 0; i < n_colors; i++)
	{
	  for (j = 0; j < SPACING; j++)
	    {
	      *p++ = bcolor;
	      *p++ = bcolor;
	      *p++ = bcolor;
	    }

	  for (j = 0; j < entry_width; j++)
	    {
	      *p++ = colors[i * 3];
	      *p++ = colors[i * 3 + 1];
	      *p++ = colors[i * 3 + 2];
	    }
	}

      for (i = 0; i < (palette_editor->columns - n_colors); i++)
	{
	  for (j = 0; j < (SPACING + entry_width); j++)
	    {
	      *p++ = 0;
	      *p++ = 0;
	      *p++ = 0;
	    }
	}

      for (j = 0; j < SPACING; j++)
	{
	  if (n_colors == column_highlight)
	    {
	      *p++ = ~bcolor;
	      *p++ = ~bcolor;
	      *p++ = ~bcolor;
	    }
	  else
	    {
	      *p++ = bcolor;
	      *p++ = bcolor;
	      *p++ = bcolor;
	    }
	}

      if (y < 0)
	y += entry_height - vsize;
      for (i = 0; i < vsize; i++, y++)
	{
	  if (column_highlight >= 0)
	    {
	      guchar *ph;

	      ph = &buffer[3 * column_highlight * (entry_width + SPACING)];

	      *ph++ = ~bcolor;
	      *ph++ = ~bcolor;
	      *ph++ = ~bcolor;
	      ph += 3 * (entry_width);
	      *ph++ = ~bcolor;
	      *ph++ = ~bcolor;
	      *ph++ = ~bcolor;
	    }

	  gtk_preview_draw_row (GTK_PREVIEW (preview), buffer, 0, y, width);
	}
      if (y > entry_height)
	y += entry_height - vsize;
    }
  else
    y += entry_height;

  return y;
}

static void
palette_editor_draw_entries (GimpPaletteEditor *editor,
			     gint               row_start,
			     gint               column_highlight)
{
  GimpPalette      *palette;
  GimpPaletteEntry *entry;
  guchar           *buffer;
  guchar           *colors;
  GList            *list;
  gint              width, height;
  gint              entry_width;
  gint              entry_height;
  gint              index, y;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  if (! palette)
    return;

  width  = editor->color_area->requisition.width;
  height = editor->color_area->requisition.height;

  entry_width  = editor->col_width;
  entry_height = (ENTRY_HEIGHT * editor->zoom_factor);

  if (entry_width <= 0)
    return;

  colors = g_new (guchar, editor->columns * 3);
  buffer = g_new (guchar, width * 3);

  if (row_start < 0)
    {
      y = 0;
      list = palette->colors;
      column_highlight = -1;
    }
  else
    {
      y = (entry_height + SPACING) * row_start;
      list = g_list_nth (palette->colors,
			 row_start * editor->columns);
    }

  index = 0;

  for (; list; list = g_list_next (list))
    {
      entry = (GimpPaletteEntry *) list->data;

      gimp_rgb_get_uchar (&entry->color,
			  &colors[index * 3],
			  &colors[index * 3 + 1],
			  &colors[index * 3 + 2]);
      index++;

      if (index == editor->columns)
	{
	  index = 0;
	  y = palette_editor_draw_color_row (colors,
                                             editor->columns, y,
					     column_highlight, buffer,
                                             editor);

	  if (y >= height || row_start >= 0)
	    {
	      /* This row only */
	      gtk_widget_queue_draw (editor->color_area);
	      g_free (buffer);
	      g_free (colors);
	      return;
	    }
	}
    }

  while (y < height)
    {
      y = palette_editor_draw_color_row (colors, index, y, column_highlight,
					 buffer, editor);
      index = 0;
      if (row_start >= 0)
	break;
    }

  g_free (buffer);
  g_free (colors);

  if (! editor->freeze_update)
    gtk_widget_queue_draw (editor->color_area);
}

static void
palette_editor_scroll_top_left (GimpPaletteEditor *palette_editor)
{
  GtkAdjustment *hadj;
  GtkAdjustment *vadj;

  if (! (palette_editor && palette_editor->scrolled_window))
    return;

  hadj = gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (palette_editor->scrolled_window));
  vadj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (palette_editor->scrolled_window));

  if (hadj)
    gtk_adjustment_set_value (hadj, 0.0);
  if (vadj)
    gtk_adjustment_set_value (vadj, 0.0);
}

static void
palette_editor_redraw (GimpPaletteEditor *editor)
{
  GimpPalette *palette;
  gint         vsize;
  gint         nrows;
  gint         n_entries;
  gint         preview_width;
  guint        width;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  if (! palette)
    return;

  width = editor->color_area->parent->parent->parent->allocation.width;

  if ((editor->columns_valid) && editor->last_width == width)
    return;

  editor->last_width = width;
  editor->col_width  = width / (editor->columns + 1) - SPACING;

  if (editor->col_width < 0)
    editor->col_width = 0;

  editor->columns_valid = TRUE;

  n_entries = palette->n_colors;

  nrows = n_entries / editor->columns;
  if (n_entries % editor->columns)
    nrows += 1;

  vsize = nrows * (SPACING + (gint) (ENTRY_HEIGHT * editor->zoom_factor)) + SPACING;

  preview_width = (editor->col_width + SPACING) * editor->columns + SPACING;

  gtk_preview_size (GTK_PREVIEW (editor->color_area), preview_width, vsize);
  gtk_widget_queue_resize (editor->color_area);

  palette_editor_draw_entries (editor, -1, -1);

  if (editor->color)
    {
      palette_editor_draw_entries (editor,
				   editor->color->position / editor->columns,
				   editor->color->position % editor->columns);
    }
}


/*  the color name entry callback  *******************************************/

static void
palette_editor_color_name_changed (GtkWidget         *widget,
				   GimpPaletteEditor *editor)
{
  GimpPalette *palette;

  palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

  if (! palette)
    return;

  if (editor->color->name)
    g_free (editor->color->name);

  editor->color->name =
    g_strdup (gtk_entry_get_text (GTK_ENTRY (editor->color_name)));

  gimp_data_dirty (GIMP_DATA (palette));
}

/*  palette zoom functions & callbacks  **************************************/

static void
palette_editor_zoom_in_callback (GtkWidget         *widget,
                                 GimpPaletteEditor *editor)
{
  editor->zoom_factor += 0.1;

  palette_editor_redraw_zoom (editor);
}

static void
palette_editor_zoom_out_callback (GtkWidget         *widget,
                                  GimpPaletteEditor *editor)
{
  editor->zoom_factor -= 0.1;

  palette_editor_redraw_zoom (editor);
}

static void
palette_editor_zoom_all_callback (GtkWidget         *widget,
                                  GimpPaletteEditor *editor)
{
  if (GIMP_DATA_EDITOR (editor)->data)
    {
      GimpPalette *palette;
      gint         window_height;
      gint         rows;

      palette = GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data);

      window_height =
        editor->color_area->parent->parent->parent->allocation.height;

      if (palette->n_columns)
        rows = palette->n_colors / palette->n_columns;
      else
        rows = palette->n_colors / COLUMNS;

      rows = MAX (1, rows);

      editor->zoom_factor =
        (((gdouble) window_height - SPACING) / (gdouble) rows - SPACING) /
        ENTRY_HEIGHT;

      palette_editor_redraw_zoom (editor);
    }
}

static void
palette_editor_redraw_zoom (GimpPaletteEditor *editor)
{
  if (editor->zoom_factor > 4.0)
    {
      editor->zoom_factor = 4.0;
    }
  else if (editor->zoom_factor < 0.1)
    {
      editor->zoom_factor = 0.1;
    }

  editor->columns = COLUMNS;

  editor->columns_valid = FALSE;
  palette_editor_redraw (editor); 

  palette_editor_scroll_top_left (editor);
}


/*  the palette dialog color dnd callbacks  **********************************/

static void
palette_editor_drag_color (GtkWidget *widget,
			   GimpRGB   *color,
			   gpointer   data)
{
  GimpPaletteEditor *editor;

  editor = GIMP_PALETTE_EDITOR (data);

  if (editor && GIMP_DATA_EDITOR (editor)->data && editor->dnd_color)
    {
      *color = editor->dnd_color->color;
    }
  else
    {
      gimp_rgba_set (color, 0.0, 0.0, 0.0, 1.0);
    }
}

static void
palette_editor_drop_color (GtkWidget     *widget,
			   const GimpRGB *color,
			   gpointer       data)
{
  GimpPaletteEditor *editor;

  editor = GIMP_PALETTE_EDITOR (data);

  if (editor && GIMP_DATA_EDITOR (editor)->data)
    {
      editor->color =
	gimp_palette_add_entry (GIMP_PALETTE (GIMP_DATA_EDITOR (editor)->data),
                                NULL,
				(GimpRGB *) color);
    }
}

static void
palette_editor_drop_palette (GtkWidget    *widget,
			     GimpViewable *viewable,
			     gpointer      data)
{
  gimp_data_editor_set_data (GIMP_DATA_EDITOR (data), GIMP_DATA (viewable));
}

static void
palette_editor_invalidate_preview (GimpPalette       *palette,
				   GimpPaletteEditor *editor)
{
  editor->columns_valid = FALSE;

  palette_editor_redraw (editor);
}
