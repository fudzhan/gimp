/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpcoordmath.h
 * Copyright (C) 2002 Simon Budig  <simon@gimp.org>
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

void     gimp_bezier_coords_mix         (const gdouble          amul,
                                         const GimpCoords      *a,
                                         const gdouble          bmul,
                                         const GimpCoords      *b,
                                         GimpCoords            *ret_val);
void     gimp_bezier_coords_average     (const GimpCoords      *a,
                                         const GimpCoords      *b,
                                         GimpCoords            *ret_average);
void     gimp_bezier_coords_add         (const GimpCoords      *a,
                                         const GimpCoords      *b,
                                         GimpCoords            *ret_add);
void     gimp_bezier_coords_difference  (const GimpCoords      *a,
                                         const GimpCoords      *b,
                                         GimpCoords            *difference);
void     gimp_bezier_coords_scale       (const gdouble          f,
                                         const GimpCoords      *a,
                                         GimpCoords            *ret_product);

gdouble  gimp_bezier_coords_scalarprod  (const GimpCoords      *a,
                                         const GimpCoords      *b);
gdouble  gimp_bezier_coords_length      (const GimpCoords      *a);
gdouble  gimp_bezier_coords_length2     (const GimpCoords      *a);

