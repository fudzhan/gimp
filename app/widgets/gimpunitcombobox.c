/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpunitcombobox.c
 * Copyright (C) 2004  Sven Neumann <sven@gimp.org>
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

#include "widgets-types.h"

#include "gimpunitcombobox.h"
#include "gimpunitstore.h"


static void  gimp_unit_combo_box_init (GimpUnitComboBox *combo);


GType
gimp_unit_combo_box_get_type (void)
{
  static GType combo_box_type = 0;

  if (!combo_box_type)
    {
      static const GTypeInfo combo_box_info =
      {
        sizeof (GimpUnitComboBoxClass),
        NULL,           /* base_init      */
        NULL,           /* base_finalize  */
        NULL,           /* class_init     */
        NULL,           /* class_finalize */
        NULL,           /* class_data     */
        sizeof (GimpUnitComboBox),
        0,              /* n_preallocs    */
        (GInstanceInitFunc) gimp_unit_combo_box_init
      };

      combo_box_type = g_type_register_static (GTK_TYPE_COMBO_BOX,
                                               "GimpUnitComboBox",
                                               &combo_box_info, 0);
    }

  return combo_box_type;
}

static void
gimp_unit_combo_box_init (GimpUnitComboBox *combo)
{
  GtkCellLayout   *layout = GTK_CELL_LAYOUT (combo);
  GtkCellRenderer *cell;

  cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (layout, cell, TRUE);
  gtk_cell_layout_set_attributes (layout, cell,
                                  "text", GIMP_UNIT_STORE_UNIT_PLURAL,
                                  NULL);

  /*  hackedehack ...  */
  layout = GTK_CELL_LAYOUT (GTK_BIN (combo)->child);
  gtk_cell_layout_clear (layout);

  cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (layout, cell, TRUE);
  gtk_cell_layout_set_attributes (layout, cell,
                                  "text", GIMP_UNIT_STORE_UNIT_ABBREVIATION,
                                  NULL);
}

/**
 * gimp_unit_combo_box_new:
 *
 * Return value: a new #GimpUnitComboBox.
 **/
GtkWidget *
gimp_unit_combo_box_new (void)
{
  GtkWidget     *combo_box;
  GimpUnitStore *store;

  store = gimp_unit_store_new (0);

  combo_box = g_object_new (GIMP_TYPE_UNIT_COMBO_BOX,
                            "model", store,
                            NULL);

  g_object_unref (store);

  return combo_box;
}

/**
 * gimp_unit_combo_box_new_with_model:
 * @model: a GimpUnitStore
 *
 * Return value: a new #GimpUnitComboBox.
 **/
GtkWidget *
gimp_unit_combo_box_new_with_model (GimpUnitStore *model)
{
  return g_object_new (GIMP_TYPE_UNIT_COMBO_BOX,
                       "model", model,
                       NULL);
}
