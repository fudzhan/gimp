/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1999 Spencer Kimball and Peter Mattis
 *
 * gimpdevicestatus.c
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

#include "config.h"

#include <gtk/gtk.h>

#include "libgimpcolor/gimpcolor.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "widgets-types.h"

#include "core/gimp.h"
#include "core/gimpbrush.h"
#include "core/gimpdatafactory.h"
#include "core/gimpgradient.h"
#include "core/gimplist.h"
#include "core/gimppattern.h"
#include "core/gimptoolinfo.h"

#include "gimpdnd.h"
#include "gimpdeviceinfo.h"
#include "gimpdevices.h"
#include "gimpdevicestatus.h"
#include "gimpdialogfactory.h"
#include "gimppropwidgets.h"
#include "gimpview.h"

#include "gimp-intl.h"


#define CELL_SIZE 20 /* The size of the view cells */


enum
{
  PROP_0,
  PROP_GIMP
};


struct _GimpDeviceStatusEntry
{
  GimpDeviceInfo *device_info;

  GtkWidget      *table;
  GtkWidget      *label;
  GtkWidget      *arrow;
  GtkWidget      *tool;
  GtkWidget      *foreground;
  GtkWidget      *background;
  GtkWidget      *brush;
  GtkWidget      *pattern;
  GtkWidget      *gradient;
};


static GObject *gimp_device_status_constructor (GType                  type,
                                                guint                  n_params,
                                                GObjectConstructParam *params);
static void gimp_device_status_set_property    (GObject               *object,
                                                guint                  property_id,
                                                const GValue          *value,
                                                GParamSpec            *pspec);

static void gimp_device_status_destroy         (GtkObject             *object);

static void gimp_device_status_device_add      (GimpContainer         *devices,
                                                GimpDeviceInfo        *device_info,
                                                GimpDeviceStatus      *status);
static void gimp_device_status_device_remove   (GimpContainer         *devices,
                                                GimpDeviceInfo        *device_info,
                                                GimpDeviceStatus      *status);

static void gimp_device_status_update_entry    (GimpDeviceInfo        *device_info,
                                                GimpDeviceStatusEntry *entry);
static void gimp_device_status_save_clicked    (GtkWidget             *button,
                                                GimpDeviceStatus      *status);
static void gimp_device_status_preview_clicked (GtkWidget             *widget,
                                                GdkModifierType        state,
                                                const gchar           *identifier);


G_DEFINE_TYPE (GimpDeviceStatus, gimp_device_status, GIMP_TYPE_EDITOR);

#define parent_class gimp_device_status_parent_class


static void
gimp_device_status_class_init (GimpDeviceStatusClass *klass)
{
  GObjectClass   *object_class     = G_OBJECT_CLASS (klass);
  GtkObjectClass *gtk_object_class = GTK_OBJECT_CLASS (klass);

  object_class->constructor  = gimp_device_status_constructor;
  object_class->set_property = gimp_device_status_set_property;

  gtk_object_class->destroy  = gimp_device_status_destroy;

  g_object_class_install_property (object_class, PROP_GIMP,
                                   g_param_spec_object ("gimp", NULL, NULL,
                                                        GIMP_TYPE_GIMP,
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY));
}

static void
gimp_device_status_init (GimpDeviceStatus *status)
{
  status->gimp           = NULL;
  status->current_device = NULL;

  status->vbox = gtk_vbox_new (FALSE, 12);
  gtk_container_set_border_width (GTK_CONTAINER (status->vbox), 6);
  gtk_container_add (GTK_CONTAINER (status), status->vbox);
  gtk_widget_show (status->vbox);

  status->save_button =
    gimp_editor_add_button (GIMP_EDITOR (status), GTK_STOCK_SAVE,
                            _("Save device status"), NULL,
                            G_CALLBACK (gimp_device_status_save_clicked),
                            NULL,
                            status);
}

static GObject *
gimp_device_status_constructor (GType                  type,
                                guint                  n_params,
                                GObjectConstructParam *params)
{
  GObject          *object;
  GimpDeviceStatus *status;
  GimpContainer    *devices;
  GList            *list;

  object = G_OBJECT_CLASS (parent_class)->constructor (type, n_params, params);

  status = GIMP_DEVICE_STATUS (object);

  g_assert (GIMP_IS_GIMP (status->gimp));

  devices = gimp_devices_get_list (status->gimp);

  for (list = GIMP_LIST (devices)->list; list; list = list->next)
    gimp_device_status_device_add (devices, list->data, status);

  g_signal_connect_object (devices, "add",
                           G_CALLBACK (gimp_device_status_device_add),
                           status, 0);
  g_signal_connect_object (devices, "remove",
                           G_CALLBACK (gimp_device_status_device_remove),
                           status, 0);

  gimp_device_status_update (status);

  return object;
}

static void
gimp_device_status_set_property (GObject      *object,
                                 guint         property_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  GimpDeviceStatus *status = GIMP_DEVICE_STATUS (object);

  switch (property_id)
    {
    case PROP_GIMP:
      status->gimp = GIMP (g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_device_status_destroy (GtkObject *object)
{
  GimpDeviceStatus *status = GIMP_DEVICE_STATUS (object);

  if (status->devices)
    {
      GList *list;

      for (list = status->devices; list; list = list->next)
        {
          GimpDeviceStatusEntry *entry = list->data;

          g_signal_handlers_disconnect_by_func (entry->device_info,
                                                gimp_device_status_update_entry,
                                                entry);
          g_free (entry);
        }

      g_list_free (status->devices);
      status->devices = NULL;
    }

  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gimp_device_status_device_add (GimpContainer    *devices,
                               GimpDeviceInfo   *device_info,
                               GimpDeviceStatus *status)
{
  GimpContext           *context = GIMP_CONTEXT (device_info);
  GimpDeviceStatusEntry *entry;
  GtkWidget             *hbox;
  GClosure              *closure;
  gchar                 *name;

  /*  only list present devices  */
  if (! device_info->device)
    return;

  entry = g_new0 (GimpDeviceStatusEntry, 1);

  status->devices = g_list_prepend (status->devices, entry);

  entry->device_info = device_info;

  closure = g_cclosure_new (G_CALLBACK (gimp_device_status_update_entry),
                            entry, NULL);
  g_object_watch_closure (G_OBJECT (status), closure);
  g_signal_connect_closure (device_info, "changed", closure, FALSE);

  entry->table = gtk_table_new (2, 7, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (entry->table), 6);
  gtk_box_pack_start (GTK_BOX (status->vbox), entry->table,
                      FALSE, FALSE, 0);
  gtk_widget_show (entry->table);

  /*  the device name  */

  if (device_info->display == gdk_display_get_default ())
    name = g_strdup (gimp_object_get_name (GIMP_OBJECT (device_info)));
  else
    name = g_strdup_printf ("%s (%s)",
                            gimp_object_get_name (GIMP_OBJECT (device_info)),
                            gdk_display_get_name (device_info->display));

  entry->label = gtk_label_new (name);
  g_free (name);

  gimp_label_set_attributes (GTK_LABEL (entry->label),
                             PANGO_ATTR_WEIGHT, PANGO_WEIGHT_BOLD,
                             -1);
  gtk_widget_set_size_request (entry->label, -1, CELL_SIZE);
  gtk_misc_set_alignment (GTK_MISC (entry->label), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (entry->table), entry->label,
                    1, 7, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (entry->label);

  /*  the arrow  */

  entry->arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_OUT);
  gtk_widget_set_size_request (entry->arrow, CELL_SIZE, CELL_SIZE);
  gtk_table_attach (GTK_TABLE (entry->table), entry->arrow,
                    0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_size_request (hbox, CELL_SIZE, CELL_SIZE);
  gtk_table_attach (GTK_TABLE (entry->table), hbox,
                    0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  /*  the tool  */

  entry->tool = gimp_prop_preview_new (G_OBJECT (context),
                                       "tool", CELL_SIZE);
  GIMP_VIEW (entry->tool)->clickable = TRUE;
  gtk_table_attach (GTK_TABLE (entry->table), entry->tool,
                    1, 2, 1, 2, 0, 0, 0, 0);
  gtk_widget_show (entry->tool);

  g_signal_connect (entry->tool, "clicked",
                    G_CALLBACK (gimp_device_status_preview_clicked),
                    "gimp-tool-list|gimp-tool-grid");

  /*  the foreground color  */

  entry->foreground = gimp_prop_color_area_new (G_OBJECT (context),
                                                "foreground",
                                                CELL_SIZE, CELL_SIZE,
                                                GIMP_COLOR_AREA_FLAT);
  gtk_widget_add_events (entry->foreground,
                         GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
  gtk_table_attach (GTK_TABLE (entry->table), entry->foreground,
                    2, 3, 1, 2, 0, 0, 0, 0);
  gtk_widget_show (entry->foreground);

  /*  the background color  */

  entry->background = gimp_prop_color_area_new (G_OBJECT (context),
                                                "background",
                                                CELL_SIZE, CELL_SIZE,
                                                GIMP_COLOR_AREA_FLAT);
  gtk_widget_add_events (entry->background,
                         GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
  gtk_table_attach (GTK_TABLE (entry->table), entry->background,
                    3, 4, 1, 2, 0, 0, 0, 0);
  gtk_widget_show (entry->background);

  /*  the brush  */

  entry->brush = gimp_prop_preview_new (G_OBJECT (context),
                                        "brush", CELL_SIZE);
  GIMP_VIEW (entry->brush)->clickable  = TRUE;
  GIMP_VIEW (entry->brush)->show_popup = TRUE;
  gtk_table_attach (GTK_TABLE (entry->table), entry->brush,
                    4, 5, 1, 2, 0, 0, 0, 0);
  gtk_widget_show (entry->brush);

  g_signal_connect (entry->brush, "clicked",
                    G_CALLBACK (gimp_device_status_preview_clicked),
                    "gimp-brush-grid|gimp-brush-list");

  /*  the pattern  */

  entry->pattern = gimp_prop_preview_new (G_OBJECT (context),
                                          "pattern", CELL_SIZE);
  GIMP_VIEW (entry->pattern)->clickable  = TRUE;
  GIMP_VIEW (entry->pattern)->show_popup = TRUE;
  gtk_table_attach (GTK_TABLE (entry->table), entry->pattern,
                    5, 6, 1, 2, 0, 0, 0, 0);
  gtk_widget_show (entry->pattern);

  g_signal_connect (entry->pattern, "clicked",
                    G_CALLBACK (gimp_device_status_preview_clicked),
                    "gimp-pattern-grid|gimp-pattern-list");

  /*  the gradient  */

  entry->gradient = gimp_prop_preview_new (G_OBJECT (context),
                                           "gradient", 2 * CELL_SIZE);
  GIMP_VIEW (entry->gradient)->clickable  = TRUE;
  GIMP_VIEW (entry->gradient)->show_popup = TRUE;
  gtk_table_attach (GTK_TABLE (entry->table), entry->gradient,
                    6, 7, 1, 2, 0, 0, 0, 0);
  gtk_widget_show (entry->gradient);

  g_signal_connect (entry->gradient, "clicked",
                    G_CALLBACK (gimp_device_status_preview_clicked),
                    "gimp-gradient-list|gimp-gradient-grid");

  gimp_device_status_update_entry (device_info, entry);
}

static void
gimp_device_status_device_remove (GimpContainer    *devices,
                                  GimpDeviceInfo   *device_info,
                                  GimpDeviceStatus *status)
{
  GList *list;

  for (list = status->devices; list; list = list->next)
    {
      GimpDeviceStatusEntry *entry = list->data;

      if (entry->device_info == device_info)
        {
          status->devices = g_list_remove (status->devices, entry);

          g_signal_handlers_disconnect_by_func (entry->device_info,
                                                gimp_device_status_update_entry,
                                                entry);
          g_free (entry);

          return;
        }
    }
}

GtkWidget *
gimp_device_status_new (Gimp *gimp)
{
  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  return g_object_new (GIMP_TYPE_DEVICE_STATUS,
                       "gimp", gimp,
                       NULL);
}

void
gimp_device_status_update (GimpDeviceStatus *status)
{
  GList *list;

  g_return_if_fail (GIMP_IS_DEVICE_STATUS (status));

  status->current_device = gimp_devices_get_current (status->gimp);

  for (list = status->devices; list; list = list->next)
    {
      GimpDeviceStatusEntry *entry = list->data;

      if (entry->device_info->device &&
          entry->device_info->device == status->current_device)
        gtk_widget_show (entry->arrow);
      else
        gtk_widget_hide (entry->arrow);
    }
}


/*  private functions  */

static void
gimp_device_status_update_entry (GimpDeviceInfo        *device_info,
                                 GimpDeviceStatusEntry *entry)
{
  if (! device_info->device || device_info->device->mode == GDK_MODE_DISABLED)
    {
      gtk_widget_hide (entry->table);
    }
  else
    {
      GimpContext *context = GIMP_CONTEXT (device_info);
      GimpRGB      color;
      guchar       r, g, b;
      gchar        buf[64];

      gimp_context_get_foreground (context, &color);
      gimp_rgb_get_uchar (&color, &r, &g, &b);
      g_snprintf (buf, sizeof (buf), _("Foreground: %d, %d, %d"), r, g, b);
      gimp_help_set_help_data (entry->foreground, buf, NULL);

      gimp_context_get_background (context, &color);
      gimp_rgb_get_uchar (&color, &r, &g, &b);
      g_snprintf (buf, sizeof (buf), _("Background: %d, %d, %d"), r, g, b);
      gimp_help_set_help_data (entry->background, buf, NULL);

      gtk_widget_show (entry->table);
    }
}

static void
gimp_device_status_save_clicked (GtkWidget        *button,
                                 GimpDeviceStatus *status)
{
  gimp_devices_save (status->gimp, TRUE);
}

static void
gimp_device_status_preview_clicked (GtkWidget       *widget,
                                    GdkModifierType  state,
                                    const gchar     *identifier)
{
  GimpDialogFactory *dialog_factory;

  dialog_factory = gimp_dialog_factory_from_name ("dock");

  gimp_dialog_factory_dialog_raise (dialog_factory,
                                    gtk_widget_get_screen (widget),
                                    identifier, -1);
}
