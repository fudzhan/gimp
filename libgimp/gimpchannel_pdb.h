/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimpchannel_pdb.h
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

/* NOTE: This file is autogenerated by pdbgen.pl */

#ifndef __GIMP_CHANNEL_PDB_H__
#define __GIMP_CHANNEL_PDB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* For information look into the C source or the html documentation */


gint32   _gimp_channel_new            (gint32    image_ID,
				       gint      width,
				       gint      height,
				       gchar    *name,
				       gdouble   opacity,
				       guchar    red,
				       guchar    green,
				       guchar    blue);
gint32   gimp_channel_copy            (gint32    channel_ID);
void     gimp_channel_delete          (gint32    channel_ID);
gchar*   gimp_channel_get_name        (gint32    channel_ID);
void     gimp_channel_set_name        (gint32    channel_ID,
				       gchar    *name);
gboolean gimp_channel_get_visible     (gint32    channel_ID);
void     gimp_channel_set_visible     (gint32    channel_ID,
				       gboolean  visible);
gboolean gimp_channel_get_show_masked (gint32    channel_ID);
void     gimp_channel_set_show_masked (gint32    channel_ID,
				       gboolean  show_masked);
gdouble  gimp_channel_get_opacity     (gint32    channel_ID);
void     gimp_channel_set_opacity     (gint32    channel_ID,
				       gdouble   opacity);
void     gimp_channel_get_color       (gint32    channel_ID,
				       guchar   *red,
				       guchar   *green,
				       guchar   *blue);
void     gimp_channel_set_color       (gint32    channel_ID,
				       guchar    red,
				       guchar    green,
				       guchar    blue);
gint     gimp_channel_get_tattoo      (gint32    channel_ID);
void     gimp_channel_set_tattoo      (gint32    channel_ID,
				       gint      tattoo);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GIMP_CHANNEL_PDB_H__ */
