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

#include <glib-object.h>

#include "paint-types.h"

#include "config/gimpconfig-params.h"

#include "gimpdodgeburnoptions.h"


#define DODGE_BURN_DEFAULT_TYPE     GIMP_DODGE
#define DODGE_BURN_DEFAULT_MODE     GIMP_MIDTONES
#define DODGE_BURN_DEFAULT_EXPOSURE 50.0


enum
{
  PROP_0,
  PROP_TYPE,
  PROP_MODE,
  PROP_EXPOSURE
};


static void   gimp_dodge_burn_options_class_init   (GimpDodgeBurnOptionsClass *klass);

static void   gimp_dodge_burn_options_set_property (GObject      *object,
                                                    guint         property_id,
                                                    const GValue *value,
                                                    GParamSpec   *pspec);
static void   gimp_dodge_burn_options_get_property (GObject      *object,
                                                    guint         property_id,
                                                    GValue       *value,
                                                    GParamSpec   *pspec);


static GimpPaintOptionsClass *parent_class = NULL;


GType
gimp_dodge_burn_options_get_type (void)
{
  static GType type = 0;

  if (! type)
    {
      static const GTypeInfo info =
      {
        sizeof (GimpDodgeBurnOptionsClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gimp_dodge_burn_options_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data     */
        sizeof (GimpDodgeBurnOptions),
        0,              /* n_preallocs    */
        (GInstanceInitFunc) NULL
      };

      type = g_type_register_static (GIMP_TYPE_PAINT_OPTIONS,
                                     "GimpDodgeBurnOptions",
                                     &info, 0);
    }

  return type;
}

static void
gimp_dodge_burn_options_class_init (GimpDodgeBurnOptionsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->set_property = gimp_dodge_burn_options_set_property;
  object_class->get_property = gimp_dodge_burn_options_get_property;

  GIMP_CONFIG_INSTALL_PROP_ENUM (object_class, PROP_TYPE,
                                 "type", NULL,
                                 GIMP_TYPE_DODGE_BURN_TYPE,
                                 DODGE_BURN_DEFAULT_TYPE,
                                 0);
  GIMP_CONFIG_INSTALL_PROP_ENUM (object_class, PROP_MODE,
                                 "mode", NULL,
                                 GIMP_TYPE_TRANSFER_MODE,
                                 DODGE_BURN_DEFAULT_MODE,
                                 0);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_EXPOSURE,
                                   "exposure", NULL,
                                   0.0, 100.0, DODGE_BURN_DEFAULT_EXPOSURE,
                                   0);
}

static void
gimp_dodge_burn_options_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GimpDodgeBurnOptions *options = GIMP_DODGE_BURN_OPTIONS (object);

  switch (property_id)
    {
    case PROP_TYPE:
      options->type = g_value_get_enum (value);
      break;
    case PROP_MODE:
      options->mode = g_value_get_enum (value);
      break;
    case PROP_EXPOSURE:
      options->exposure = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_dodge_burn_options_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  GimpDodgeBurnOptions *options = GIMP_DODGE_BURN_OPTIONS (object);

  switch (property_id)
    {
    case PROP_TYPE:
      g_value_set_enum (value, options->type);
      break;
    case PROP_MODE:
      g_value_set_enum (value, options->mode);
      break;
    case PROP_EXPOSURE:
      g_value_set_double (value, options->exposure);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}
