/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpvectoroptions.c
 * Copyright (C) 1999 Sven Neumann <sven@gimp.org>
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

#include "libgimpconfig/gimpconfig.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"

#include "core/gimptoolinfo.h"

#include "widgets/gimphelp-ids.h"
#include "widgets/gimpwidgets-utils.h"

#include "gimpvectoroptions.h"
#include "gimptooloptions-gui.h"

#include "gimp-intl.h"


enum
{
  PROP_0,
  PROP_VECTORS_EDIT_MODE,
  PROP_VECTORS_POLYGONAL
};


static void   gimp_vector_options_set_property (GObject      *object,
                                                guint         property_id,
                                                const GValue *value,
                                                GParamSpec   *pspec);
static void   gimp_vector_options_get_property (GObject      *object,
                                                guint         property_id,
                                                GValue       *value,
                                                GParamSpec   *pspec);


G_DEFINE_TYPE (GimpVectorOptions, gimp_vector_options, GIMP_TYPE_TOOL_OPTIONS)


static void
gimp_vector_options_class_init (GimpVectorOptionsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = gimp_vector_options_set_property;
  object_class->get_property = gimp_vector_options_get_property;

  GIMP_CONFIG_INSTALL_PROP_ENUM (object_class, PROP_VECTORS_EDIT_MODE,
                                 "vectors-edit-mode", NULL,
                                 GIMP_TYPE_VECTOR_MODE,
                                 GIMP_VECTOR_MODE_DESIGN,
                                 GIMP_PARAM_STATIC_STRINGS);

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_VECTORS_POLYGONAL,
                                    "vectors-polygonal",
                                    N_("Restrict editing to polygons"),
                                    FALSE,
                                    GIMP_PARAM_STATIC_STRINGS);
}

static void
gimp_vector_options_init (GimpVectorOptions *options)
{
}

static void
gimp_vector_options_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GimpVectorOptions *options = GIMP_VECTOR_OPTIONS (object);

  switch (property_id)
    {
    case PROP_VECTORS_EDIT_MODE:
      options->edit_mode = g_value_get_enum (value);
      break;
    case PROP_VECTORS_POLYGONAL:
      options->polygonal = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}


static void
gimp_vector_options_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GimpVectorOptions *options = GIMP_VECTOR_OPTIONS (object);

  switch (property_id)
    {
    case PROP_VECTORS_EDIT_MODE:
      g_value_set_enum (value, options->edit_mode);
      break;
    case PROP_VECTORS_POLYGONAL:
      g_value_set_boolean (value, options->polygonal);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}


GtkWidget *
gimp_vector_options_gui (GimpToolOptions *tool_options)
{
  GObject           *config  = G_OBJECT (tool_options);
  GimpVectorOptions *options = GIMP_VECTOR_OPTIONS (tool_options);
  GtkWidget         *vbox    = gimp_tool_options_gui (tool_options);
  GtkWidget         *frame;
  GtkWidget         *button;
  gchar             *str;

  /*  tool toggle  */
  frame = gimp_prop_enum_radio_frame_new (config, "vectors-edit-mode",
                                          _("Edit Mode"), 0, 0);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  button = gimp_prop_check_button_new (config, "vectors-polygonal",
                                       _("Polygonal"));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  str = g_strdup_printf (_("Path to Selection\n"
                           "%s  Add\n"
                           "%s  Subtract\n"
                           "%s  Intersect"),
                         gimp_get_mod_string (GDK_SHIFT_MASK),
                         gimp_get_mod_string (GDK_CONTROL_MASK),
                         gimp_get_mod_string (GDK_SHIFT_MASK |
                                              GDK_CONTROL_MASK));

  button = gimp_button_new ();
  gtk_button_set_label (GTK_BUTTON (button), _("Create Selection from Path"));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (button, FALSE);
  gimp_help_set_help_data (button, str, GIMP_HELP_PATH_SELECTION_REPLACE);
  gtk_widget_show (button);

  g_free (str);

  options->to_selection_button = button;

  button = gtk_button_new_with_label (_("Stroke Path"));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (button, FALSE);
  gimp_help_set_help_data (button, NULL, GIMP_HELP_PATH_STROKE);
  gtk_widget_show (button);

  options->stroke_button = button;

  return vbox;
}
