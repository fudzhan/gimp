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

#ifndef __GIMP_IMAGE_CONVERT_H__
#define __GIMP_IMAGE_CONVERT_H__


#define MAXNUMCOLORS 256


void   gimp_image_convert          (GimpImage              *gimage,
                                    GimpImageBaseType       new_type,
                                    /* The following params used only for
                                     * new_type == GIMP_INDEXED
                                     */
                                    gint                    num_cols,
                                    GimpConvertDitherType   dither,
                                    gboolean                alpha_dither,
                                    gboolean                remove_dups,
                                    GimpConvertPaletteType  palette_type,
                                    GimpPalette            *custom_palette);

void   gimp_drawable_convert_rgb       (GimpDrawable       *drawable,
                                        TileManager        *new_tiles,
                                        GimpImageBaseType   old_base_type);
void   gimp_drawable_convert_grayscale (GimpDrawable       *drawable,
                                        TileManager        *new_tiles,
                                        GimpImageBaseType   old_base_type);


#endif  /*  __GIMP_IMAGE_CONVERT_H__  */
