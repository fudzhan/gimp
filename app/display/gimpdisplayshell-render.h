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

#ifndef __IMAGE_RENDER_H__
#define __IMAGE_RENDER_H__


/*  Functions  */
void   render_setup (GimpCheckType  check_type,
		     GimpCheckSize  check_size);
void   render_free  (void);
void   render_image (GDisplay *gdisp,
		     gint      x,
		     gint      y,
		     gint      w,
		     gint      h);

/*
 *  Extern variables
 */
extern guchar *render_check_buf;
extern guchar *render_empty_buf;
extern guchar *render_temp_buf;
extern guchar *render_blend_dark_check;
extern guchar *render_blend_light_check;


#endif  /*  __IMAGE_RENDER_H__  */
