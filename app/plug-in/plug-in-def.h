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

#ifndef __PLUG_IN_DEF_H__
#define __PLUG_IN_DEF_H__


struct _PlugInDef
{
  gchar    *prog;
  GSList   *proc_defs;
  gchar    *locale_domain;
  gchar    *locale_path;
  gchar    *help_path;
  time_t    mtime;
  gboolean  query;
  gboolean  has_init;     /* Does the plug-in need to be initialized? */
};


PlugInDef * plug_in_def_new                    (const gchar   *prog);
void        plug_in_def_free                   (PlugInDef     *plug_in_def, 
                                                gboolean       free_proc_defs);

void        plug_in_def_add_proc_def           (PlugInDef     *plug_in_def,
                                                PlugInProcDef *proc_def);

void        plug_in_def_set_mtime              (PlugInDef     *plug_in_def,
                                                time_t         mtime);
void        plug_in_def_set_locale_domain_name (PlugInDef     *plug_in_def,
                                                const gchar   *domain_name);
void        plug_in_def_set_locale_domain_path (PlugInDef     *plug_in_def,
                                                const gchar   *domain_path);
void        plug_in_def_set_help_path          (PlugInDef     *plug_in_def,
                                                const gchar   *help_path);


#endif /* __PLUG_IN_DEF_H__ */
