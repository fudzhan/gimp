/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimpcontrollerinfo.c
 * Copyright (C) 2004-2005 Michael Natterer <mitch@gimp.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h>

#include <gtk/gtk.h>

#include "libgimpconfig/gimpconfig.h"
#include "libgimpwidgets/gimpwidgets.h"

#define GIMP_ENABLE_CONTROLLER_UNDER_CONSTRUCTION
#include "libgimpwidgets/gimpcontroller.h"

#include "widgets-types.h"

#include "core/gimp-utils.h"
#include "core/gimpmarshal.h"

#include "gimpcontrollerinfo.h"

#include "gimp-intl.h"


enum
{
  PROP_0,
  PROP_ENABLED,
  PROP_DEBUG_EVENTS,
  PROP_CONTROLLER,
  PROP_MAPPING
};

enum
{
  EVENT_MAPPED,
  LAST_SIGNAL
};


static void     gimp_controller_info_config_iface_init (GimpConfigInterface *iface);

static void     gimp_controller_info_finalize     (GObject          *object);
static void     gimp_controller_info_set_property (GObject          *object,
                                                   guint             property_id,
                                                   const GValue     *value,
                                                   GParamSpec       *pspec);
static void     gimp_controller_info_get_property (GObject          *object,
                                                   guint             property_id,
                                                   GValue           *value,
                                                   GParamSpec       *pspec);

static gboolean gimp_controller_info_serialize_property   (GimpConfig       *config,
                                                           guint             property_id,
                                                           const GValue     *value,
                                                           GParamSpec       *pspec,
                                                           GimpConfigWriter *writer);
static gboolean gimp_controller_info_deserialize_property (GimpConfig       *config,
                                                           guint             property_id,
                                                           GValue           *value,
                                                           GParamSpec       *pspec,
                                                           GScanner         *scanner,
                                                           GTokenType       *expected);

static gboolean gimp_controller_info_event (GimpController            *controller,
                                            const GimpControllerEvent *event,
                                            GimpControllerInfo        *info);


G_DEFINE_TYPE_WITH_CODE (GimpControllerInfo, gimp_controller_info,
                         GIMP_TYPE_VIEWABLE,
                         G_IMPLEMENT_INTERFACE (GIMP_TYPE_CONFIG,
                                                gimp_controller_info_config_iface_init));

#define parent_class gimp_controller_info_parent_class

static guint info_signals[LAST_SIGNAL] = { 0 };


static void
gimp_controller_info_class_init (GimpControllerInfoClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize     = gimp_controller_info_finalize;
  object_class->set_property = gimp_controller_info_set_property;
  object_class->get_property = gimp_controller_info_get_property;

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_ENABLED,
                                    "enabled", NULL,
                                    TRUE,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_DEBUG_EVENTS,
                                    "debug-events", NULL,
                                    FALSE,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_OBJECT (object_class, PROP_CONTROLLER,
                                   "controller", NULL,
                                   GIMP_TYPE_CONTROLLER,
                                   0);
  GIMP_CONFIG_INSTALL_PROP_POINTER (object_class, PROP_MAPPING,
                                    "mapping", NULL,
                                    0);

  info_signals[EVENT_MAPPED] =
    g_signal_new ("event-mapped",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GimpControllerInfoClass, event_mapped),
                  gimp_boolean_handled_accum, NULL,
                  gimp_marshal_BOOLEAN__OBJECT_POINTER_STRING,
                  G_TYPE_BOOLEAN, 3,
                  G_TYPE_OBJECT,
                  G_TYPE_POINTER,
                  G_TYPE_STRING);
}

static void
gimp_controller_info_init (GimpControllerInfo *info)
{
  info->controller = NULL;
  info->mapping    = g_hash_table_new_full (g_str_hash,
                                            g_str_equal,
                                            (GDestroyNotify) g_free,
                                            (GDestroyNotify) g_free);
}

static void
gimp_controller_info_config_iface_init (GimpConfigInterface *iface)
{
  iface->serialize_property   = gimp_controller_info_serialize_property;
  iface->deserialize_property = gimp_controller_info_deserialize_property;
}

static void
gimp_controller_info_finalize (GObject *object)
{
  GimpControllerInfo *info = GIMP_CONTROLLER_INFO (object);

  if (info->controller)
    g_object_unref (info->controller);

  if (info->mapping)
    g_hash_table_destroy (info->mapping);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gimp_controller_info_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GimpControllerInfo *info = GIMP_CONTROLLER_INFO (object);

  switch (property_id)
    {
    case PROP_ENABLED:
      info->enabled = g_value_get_boolean (value);
      break;
    case PROP_DEBUG_EVENTS:
      info->debug_events = g_value_get_boolean (value);
      break;
    case PROP_CONTROLLER:
      if (info->controller)
        {
          g_signal_handlers_disconnect_by_func (info->controller,
                                                gimp_controller_info_event,
                                                info);
          g_object_unref (info->controller);
        }

      info->controller = (GimpController *) g_value_dup_object (value);

      if (info->controller)
        {
          g_signal_connect_object (info->controller, "event",
                                   G_CALLBACK (gimp_controller_info_event),
                                   G_OBJECT (info),
                                   0);
        }
      break;
    case PROP_MAPPING:
      if (info->mapping)
        g_hash_table_destroy (info->mapping);
      info->mapping = g_value_get_pointer (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_controller_info_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GimpControllerInfo *info = GIMP_CONTROLLER_INFO (object);

  switch (property_id)
    {
    case PROP_ENABLED:
      g_value_set_boolean (value, info->enabled);
      break;
    case PROP_DEBUG_EVENTS:
      g_value_set_boolean (value, info->debug_events);
      break;
    case PROP_CONTROLLER:
      g_value_set_object (value, info->controller);
      break;
    case PROP_MAPPING:
      g_value_set_pointer (value, info->mapping);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_controller_info_serialize_mapping (gpointer key,
                                        gpointer value,
                                        gpointer data)
{
  const gchar      *event_name  = key;
  const gchar      *action_name = value;
  GimpConfigWriter *writer      = data;

  gimp_config_writer_open (writer, "map");
  gimp_config_writer_string (writer, event_name);
  gimp_config_writer_string (writer, action_name);
  gimp_config_writer_close (writer);
}

static gboolean
gimp_controller_info_serialize_property (GimpConfig       *config,
                                         guint             property_id,
                                         const GValue     *value,
                                         GParamSpec       *pspec,
                                         GimpConfigWriter *writer)
{
  GHashTable *mapping;

  if (property_id != PROP_MAPPING)
    return FALSE;

  mapping = g_value_get_pointer (value);

  if (mapping)
    {
      gimp_config_writer_open (writer, pspec->name);

      g_hash_table_foreach (mapping,
                            (GHFunc) gimp_controller_info_serialize_mapping,
                            writer);

      gimp_config_writer_close (writer);
    }

  return TRUE;
}

static gboolean
gimp_controller_info_deserialize_property (GimpConfig *config,
                                           guint       property_id,
                                           GValue     *value,
                                           GParamSpec *pspec,
                                           GScanner   *scanner,
                                           GTokenType *expected)
{
  GHashTable *mapping;
  GTokenType  token;

  if (property_id != PROP_MAPPING)
    return FALSE;

  mapping = g_hash_table_new_full (g_str_hash,
                                   g_str_equal,
                                   (GDestroyNotify) g_free,
                                   (GDestroyNotify) g_free);

  token = G_TOKEN_LEFT_PAREN;

  while (g_scanner_peek_next_token (scanner) == token)
    {
      token = g_scanner_get_next_token (scanner);

      switch (token)
        {
        case G_TOKEN_LEFT_PAREN:
          token = G_TOKEN_IDENTIFIER;
          break;

        case G_TOKEN_IDENTIFIER:
          if (! strcmp (scanner->value.v_identifier, "map"))
            {
              gchar *event_name;
              gchar *action_name;

              token = G_TOKEN_STRING;
              if (! gimp_scanner_parse_string (scanner, &event_name))
                goto error;

              token = G_TOKEN_STRING;
              if (! gimp_scanner_parse_string (scanner, &action_name))
                goto error;

              g_hash_table_insert (mapping, event_name, action_name);
            }
          token = G_TOKEN_RIGHT_PAREN;
          break;

        case G_TOKEN_RIGHT_PAREN:
          token = G_TOKEN_LEFT_PAREN;
          break;

        default:
          break;
        }
    }

  if (token == G_TOKEN_LEFT_PAREN)
    {
      token = G_TOKEN_RIGHT_PAREN;

      if (g_scanner_peek_next_token (scanner) == token)
        {
          g_value_set_pointer (value, mapping);
        }
      else
        {
          goto error;
        }
    }
  else
    {
    error:
      g_hash_table_destroy (mapping);

      *expected = token;
    }

  return TRUE;
}


/*  public functions  */

GimpControllerInfo *
gimp_controller_info_new (GType type)
{
  GimpControllerClass *controller_class;
  GimpController      *controller;
  GimpControllerInfo  *info;

  g_return_val_if_fail (g_type_is_a (type, GIMP_TYPE_CONTROLLER), NULL);

  controller_class = g_type_class_ref (type);

  controller = gimp_controller_new (type);
  info = g_object_new (GIMP_TYPE_CONTROLLER_INFO,
                       "name",       controller_class->name,
                       "controller", controller,
                       NULL);
  g_object_unref (controller);

  g_type_class_unref (controller_class);

  return info;
}

void
gimp_controller_info_set_enabled (GimpControllerInfo *info,
                                  gboolean            enabled)
{
  g_return_if_fail (GIMP_IS_CONTROLLER_INFO (info));

  if (enabled != info->enabled)
    g_object_set (info, "enabled", enabled, NULL);
}

gboolean
gimp_controller_info_get_enabled (GimpControllerInfo *info)
{
  g_return_val_if_fail (GIMP_IS_CONTROLLER_INFO (info), FALSE);

  return info->enabled;
}

void
gimp_controller_info_set_event_snooper (GimpControllerInfo         *info,
                                        GimpControllerEventSnooper  snooper,
                                        gpointer                    snooper_data)
{
  g_return_if_fail (GIMP_IS_CONTROLLER_INFO (info));

  info->snooper      = snooper;
  info->snooper_data = snooper_data;
}


/*  private functions  */

static gboolean
gimp_controller_info_event (GimpController            *controller,
                            const GimpControllerEvent *event,
                            GimpControllerInfo        *info)
{
  const gchar *event_name;
  const gchar *event_blurb;
  const gchar *action_name = NULL;

  event_name = gimp_controller_get_event_name (controller, event->any.event_id);
  event_blurb = gimp_controller_get_event_blurb (controller, event->any.event_id);

  if (info->debug_events)
    {
      g_print ("Received '%s' (class '%s')\n"
               "    controller event '%s (%s)'\n",
               controller->name, GIMP_CONTROLLER_GET_CLASS (controller)->name,
               event_name, event_blurb);

      switch (event->any.type)
        {
        case GIMP_CONTROLLER_EVENT_TRIGGER:
          g_print ("    (trigger event)\n");
          break;

        case GIMP_CONTROLLER_EVENT_VALUE:
          if (G_VALUE_HOLDS_DOUBLE (&event->value.value))
            g_print ("    (value event, value = %f)\n",
                     g_value_get_double (&event->value.value));
          else
            g_print ("    (value event, unhandled type '%s')\n",
                     g_type_name (event->value.value.g_type));
          break;
        }
    }

  if (info->snooper)
    {
      if (info->snooper (info, controller, event, info->snooper_data))
        {
          if (info->debug_events)
            g_print ("    intercepted by event snooper\n\n");

          return TRUE;
        }
    }

  if (! info->enabled)
    {
      if (info->debug_events)
        g_print ("    ignoring because controller is disabled\n\n");

      return FALSE;
    }

  if (info->mapping)
    action_name = g_hash_table_lookup (info->mapping, event_name);

  if (action_name)
    {
      gboolean retval = FALSE;

      if (info->debug_events)
        g_print ("    maps to action '%s'\n", action_name);

      g_signal_emit (info, info_signals[EVENT_MAPPED], 0,
                     controller, event, action_name, &retval);

      if (info->debug_events)
        {
          if (retval)
            g_print ("    action was found\n\n");
          else
            g_print ("    action NOT found\n\n");
        }

      return retval;
    }
  else
    {
      if (info->debug_events)
        g_print ("    doesn't map to action\n\n");
    }

  return FALSE;
}
