/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpplugin_pdb.c
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

/* NOTE: This file is auto-generated by pdbgen.pl */

#include "config.h"

#include "gimp.h"

/**
 * gimp_plugin_domain_register:
 * @domain_name: The name of the textdomain (must be unique).
 * @domain_path: The absolute path to the compiled message catalog (may be NULL).
 *
 * Registers a textdomain for localisation.
 *
 * This procedure adds a textdomain to the list of domains Gimp
 * searches for strings when translating its menu entries. There is no
 * need to call this function for plug-ins that have their strings
 * included in the gimp-std-plugins domain as that is used by default.
 * If the compiled message catalog is not in the standard location, you
 * may specify an absolute path to another location. This procedure can
 * only be called in the query function of a plug-in and it has to be
 * called before any procedure is installed.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_plugin_domain_register (const gchar *domain_name,
                             const gchar *domain_path)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-plugin-domain-register",
                                    &nreturn_vals,
                                    GIMP_PDB_STRING, domain_name,
                                    GIMP_PDB_STRING, domain_path,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_plugin_help_register:
 * @domain_name: The XML namespace of the plug-in's help pages.
 * @domain_uri: The root URI of the plug-in's help pages.
 *
 * Register a help path for a plug-in.
 *
 * This procedure changes the help rootdir for the plug-in which calls
 * it. All subsequent calls of gimp_help() from this plug-in will be
 * interpreted relative to this rootdir.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_plugin_help_register (const gchar *domain_name,
                           const gchar *domain_uri)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-plugin-help-register",
                                    &nreturn_vals,
                                    GIMP_PDB_STRING, domain_name,
                                    GIMP_PDB_STRING, domain_uri,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_plugin_menu_register:
 * @procedure_name: The procedure for which to install the menu path.
 * @menu_path: The procedure's additional menu path.
 *
 * Register an additional menu path for a plug-in procedure.
 *
 * This procedure installs an additional menu entry for the given
 * procedure.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.2
 */
gboolean
gimp_plugin_menu_register (const gchar *procedure_name,
                           const gchar *menu_path)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-plugin-menu-register",
                                    &nreturn_vals,
                                    GIMP_PDB_STRING, procedure_name,
                                    GIMP_PDB_STRING, menu_path,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_plugin_menu_branch_register:
 * @menu_path: The sub-menu's menu path.
 * @menu_name: The name of the sub-menu.
 *
 * Register a sub-menu.
 *
 * This procedure installs a sub-menu which does not belong to any
 * procedure. The menu-name should be the untranslated menu label. GIMP
 * will look up the translation in the textdomain registered for the
 * plug-in.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_plugin_menu_branch_register (const gchar *menu_path,
                                  const gchar *menu_name)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-plugin-menu-branch-register",
                                    &nreturn_vals,
                                    GIMP_PDB_STRING, menu_path,
                                    GIMP_PDB_STRING, menu_name,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * _gimp_plugin_icon_register:
 * @procedure_name: The procedure for which to install the icon.
 * @icon_type: The type of the icon.
 * @icon_data_length: The length of 'icon-data'.
 * @icon_data: The procedure's icon. The format depends on the 'icon_type' parameter.
 *
 * Register an icon for a plug-in procedure.
 *
 * This procedure installs an icon for the given procedure.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.2
 */
gboolean
_gimp_plugin_icon_register (const gchar  *procedure_name,
                            GimpIconType  icon_type,
                            gint          icon_data_length,
                            const guint8 *icon_data)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-plugin-icon-register",
                                    &nreturn_vals,
                                    GIMP_PDB_STRING, procedure_name,
                                    GIMP_PDB_INT32, icon_type,
                                    GIMP_PDB_INT32, icon_data_length,
                                    GIMP_PDB_INT8ARRAY, icon_data,
                                    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}
