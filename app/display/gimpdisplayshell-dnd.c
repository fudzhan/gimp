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

#include <gtk/gtk.h>

#include "display-types.h"

#include "core/gimp.h"
#include "core/gimp-edit.h"
#include "core/gimpbuffer.h"
#include "core/gimpcontainer.h"
#include "core/gimpcontext.h"
#include "core/gimpdrawable.h"
#include "core/gimpdrawable-bucket-fill.h"
#include "core/gimpimage.h"
#include "core/gimpimage-merge.h"
#include "core/gimpimage-undo.h"
#include "core/gimplayer.h"
#include "core/gimppattern.h"

#include "file/file-open.h"
#include "file/file-utils.h"

#include "text/gimptext.h"
#include "text/gimptextlayer.h"

#include "vectors/gimpvectors.h"
#include "vectors/gimpvectors-import.h"

#include "gimpdisplay.h"
#include "gimpdisplayshell.h"
#include "gimpdisplayshell-dnd.h"
#include "gimpdisplayshell-transform.h"

#include "gimp-intl.h"

/* #define DEBUG_DND */

#ifdef DEBUG_DND
#define D(stmnt) stmnt
#else
#define D(stmnt)
#endif


void
gimp_display_shell_drop_drawable (GtkWidget    *widget,
                                  GimpViewable *viewable,
                                  gpointer      data)
{
  GimpDisplayShell *shell  = GIMP_DISPLAY_SHELL (data);
  GimpImage        *gimage = shell->gdisp->gimage;
  GType             new_type;
  GimpItem         *new_item;

  if (gimage->gimp->busy)
    return;

  if (GIMP_IS_LAYER (viewable))
    new_type = G_TYPE_FROM_INSTANCE (viewable);
  else
    new_type = GIMP_TYPE_LAYER;

  new_item = gimp_item_convert (GIMP_ITEM (viewable), gimage, new_type, TRUE);

  if (new_item)
    {
      GimpLayer *new_layer;
      gint       x, y, width, height;
      gint       off_x, off_y;

      new_layer = GIMP_LAYER (new_item);

      gimp_image_undo_group_start (gimage, GIMP_UNDO_GROUP_EDIT_PASTE,
                                   _("Drop New Layer"));

      gimp_display_shell_untransform_viewport (shell, &x, &y, &width, &height);

      gimp_item_offsets (new_item, &off_x, &off_y);

      off_x = x + (width  - gimp_item_width  (new_item)) / 2 - off_x;
      off_y = y + (height - gimp_item_height (new_item)) / 2 - off_y;

      gimp_item_translate (new_item, off_x, off_y, FALSE);

      gimp_image_add_layer (gimage, new_layer, -1);

      gimp_image_undo_group_end (gimage);

      gimp_image_flush (gimage);

      gimp_context_set_display (gimp_get_user_context (gimage->gimp),
                                shell->gdisp);
    }
}

void
gimp_display_shell_drop_vectors (GtkWidget    *widget,
                                 GimpViewable *viewable,
                                 gpointer      data)
{
  GimpDisplayShell *shell  = GIMP_DISPLAY_SHELL (data);
  GimpImage        *gimage = shell->gdisp->gimage;
  GimpItem         *new_item;

  if (gimage->gimp->busy)
    return;

  new_item = gimp_item_convert (GIMP_ITEM (viewable), gimage,
                                G_TYPE_FROM_INSTANCE (viewable), TRUE);

  if (new_item)
    {
      GimpVectors *new_vectors = GIMP_VECTORS (new_item);

      gimp_image_undo_group_start (gimage, GIMP_UNDO_GROUP_EDIT_PASTE,
                                   _("Drop New Path"));

      gimp_image_add_vectors (gimage, new_vectors, -1);

      gimp_image_undo_group_end (gimage);

      gimp_image_flush (gimage);

      gimp_context_set_display (gimp_get_user_context (gimage->gimp),
                                shell->gdisp);
    }
}

void
gimp_display_shell_drop_svg (GtkWidget     *widget,
                             const gchar   *svg_data,
                             gint           svg_data_len,
                             gpointer       data)
{
  GimpDisplayShell *shell  = GIMP_DISPLAY_SHELL (data);
  GimpImage        *gimage = shell->gdisp->gimage;
  GError           *error  = NULL;

  if (gimage->gimp->busy)
    return;

  D (g_print ("drop SVG on canvas\n"));

  if (! gimp_vectors_import_buffer (gimage, svg_data, svg_data_len,
                                    TRUE, TRUE, &error))
    {
      g_message (error->message);
      g_clear_error (&error);
    }
  else
    {
      gimp_image_flush (gimage);

      gimp_context_set_display (gimp_get_user_context (gimage->gimp),
                                shell->gdisp);
    }
}

static void
gimp_display_shell_bucket_fill (GimpDisplayShell   *shell,
                                GimpBucketFillMode  fill_mode,
                                const GimpRGB      *color,
                                GimpPattern        *pattern)
{
  GimpImage    *gimage = shell->gdisp->gimage;
  GimpDrawable *drawable;

  if (gimage->gimp->busy)
    return;

  drawable = gimp_image_active_drawable (gimage);

  if (! drawable)
    return;

  /* FIXME: there should be a virtual method for this that the
   *        GimpTextLayer can override.
   */
  if (color && gimp_drawable_is_text_layer (drawable))
    {
      gimp_text_layer_set (GIMP_TEXT_LAYER (drawable), NULL,
                           "color", color,
                           NULL);
    }
  else
    {
      gimp_drawable_bucket_fill_full (drawable,
                                      fill_mode,
                                      GIMP_NORMAL_MODE, GIMP_OPACITY_OPAQUE,
                                      FALSE,             /* no seed fill */
                                      FALSE, 0.0, FALSE, /* fill params  */
                                      0.0, 0.0,          /* ignored      */
                                      color, pattern);
    }

  gimp_image_flush (gimage);

  gimp_context_set_display (gimp_get_user_context (gimage->gimp),
                            shell->gdisp);
}

void
gimp_display_shell_drop_pattern (GtkWidget    *widget,
                                 GimpViewable *viewable,
                                 gpointer      data)
{
  if (GIMP_IS_PATTERN (viewable))
    gimp_display_shell_bucket_fill (GIMP_DISPLAY_SHELL (data),
                                    GIMP_PATTERN_BUCKET_FILL,
                                    NULL, GIMP_PATTERN (viewable));
}

void
gimp_display_shell_drop_color (GtkWidget     *widget,
                               const GimpRGB *color,
                               gpointer       data)
{
  gimp_display_shell_bucket_fill (GIMP_DISPLAY_SHELL (data),
                                  GIMP_FG_BUCKET_FILL,
                                  color, NULL);
}

void
gimp_display_shell_drop_buffer (GtkWidget    *widget,
                                GimpViewable *viewable,
                                gpointer      data)
{
  GimpDisplayShell *shell  = GIMP_DISPLAY_SHELL (data);
  GimpImage        *gimage = shell->gdisp->gimage;
  GimpBuffer       *buffer;
  gint              x, y, width, height;

  if (gimage->gimp->busy)
    return;

  buffer = GIMP_BUFFER (viewable);

  gimp_display_shell_untransform_viewport (shell, &x, &y, &width, &height);

  /* FIXME: popup a menu for selecting "Paste Into" */

  gimp_edit_paste (gimage, gimp_image_active_drawable (gimage),
		   buffer, FALSE,
                   x, y, width, height);

  gimp_image_flush (gimage);

  gimp_context_set_display (gimp_get_user_context (gimage->gimp),
                            shell->gdisp);
}

void
gimp_display_shell_drop_files (GtkWidget *widget,
                               GList     *files,
                               gpointer   data)
{
  GimpDisplayShell *shell  = GIMP_DISPLAY_SHELL (data);
  GimpImage        *gimage = shell->gdisp->gimage;;
  GimpContext      *context;
  GList            *list;

  context = gimp_get_user_context (gimage->gimp);

  for (list = files; list; list = g_list_next (list))
    {
      const gchar       *uri   = list->data;
      GimpImage         *new_image;
      GimpPDBStatusType  status;
      GError            *error = NULL;

      new_image = file_open_image (gimage->gimp, context, uri, uri,
                                   NULL, GIMP_RUN_NONINTERACTIVE,
                                   &status, NULL, &error);

      if (new_image)
        {
          GimpLayer *layer;

          gimp_image_undo_disable (new_image);

          if (gimp_container_num_children (new_image->layers) > 1)
            {
              layer = gimp_image_merge_visible_layers (new_image, context,
                                                       GIMP_CLIP_TO_IMAGE);
            }
          else
            {
              layer = (GimpLayer *)
                gimp_container_get_child_by_index (new_image->layers, 0);
            }

          if (layer)
            {
              GimpItem *new_item;

              new_item = gimp_item_convert (GIMP_ITEM (layer), gimage,
                                            G_TYPE_FROM_INSTANCE (layer),
                                            TRUE);

              if (new_item)
                {
                  GimpLayer *new_layer;
                  gint       x, y, width, height;
                  gint       off_x, off_y;
                  gchar     *basename;

                  new_layer = GIMP_LAYER (new_item);

                  basename = file_utils_uri_to_utf8_basename (uri);
                  gimp_object_set_name (GIMP_OBJECT (new_layer), basename);
                  g_free (basename);

                  gimp_display_shell_untransform_viewport (shell, &x, &y,
                                                           &width, &height);

                  gimp_item_offsets (new_item, &off_x, &off_y);

                  off_x = x + (width  - gimp_item_width  (new_item)) / 2 - off_x;
                  off_y = y + (height - gimp_item_height (new_item)) / 2 - off_y;

                  gimp_item_translate (new_item, off_x, off_y, FALSE);

                  gimp_image_add_layer (gimage, new_layer, -1);
                }
            }

          g_object_unref (new_image);
        }
      else if (status != GIMP_PDB_CANCEL)
        {
          gchar *filename = file_utils_uri_to_utf8_filename (uri);

          g_message (_("Opening '%s' failed:\n\n%s"),
                     filename, error->message);

          g_clear_error (&error);
          g_free (filename);
        }
    }

  gimp_image_flush (gimage);

  gimp_context_set_display (context, shell->gdisp);
}
