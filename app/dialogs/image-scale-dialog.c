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

#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "dialogs-types.h"

#include "config/gimpguiconfig.h"

#include "core/gimp.h"
#include "core/gimpcontext.h"
#include "core/gimpimage.h"
#include "core/gimpimage-scale.h"

#include "widgets/gimphelp-ids.h"
#include "widgets/gimpmessagebox.h"
#include "widgets/gimpmessagedialog.h"

#include "display/gimpdisplay.h"
#include "display/gimpdisplayshell.h"

#include "scale-dialog.h"

#include "image-scale-dialog.h"

#include "gimp-intl.h"


static void  image_scale_callback (GtkWidget             *widget,
                                   GimpViewable          *viewable,
                                   gint                   width,
                                   gint                   height,
                                   GimpUnit               unit,
                                   GimpInterpolationType  interpolation,
                                   gdouble                xresolution,
                                   gdouble                yresolution,
                                   GimpUnit               resolution_unit,
                                   gpointer               data);

static GtkWidget * image_scale_confirm_dialog   (ImageScaleDialog *dialog);
static void        image_scale_confirm_large    (ImageScaleDialog *dialog,
                                                 gint64            new_memsize,
                                                 gint64            max_memsize);
static void        image_scale_confirm_small    (ImageScaleDialog *dialog);
static void        image_scale_confirm_response (GtkWidget        *widget,
                                                 gint              response_id,
                                                 ImageScaleDialog *dialog);

/*  public functions  */

ImageScaleDialog *
image_scale_dialog_new (GimpImage                *image,
                        GimpDisplay              *display,
                        GimpContext              *context,
                        GtkWidget                *parent,
                        ImageScaleDialogCallback  callback)
{
  ImageScaleDialog *dialog;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_DISPLAY (display), NULL);
  g_return_val_if_fail (GIMP_IS_CONTEXT (context), NULL);
  g_return_val_if_fail (callback != NULL, NULL);

  dialog = g_new0 (ImageScaleDialog, 1);

  dialog->gimage  = image;
  dialog->gdisp   = display;
  dialog->context = context;
  dialog->dialog  = scale_dialog_new (GIMP_VIEWABLE (display->gimage),
                                      _("Scale Image"), "gimp-image-scale",
                                      parent,
                                      gimp_standard_help_func,
                                      GIMP_HELP_IMAGE_SCALE,
                                      GIMP_DISPLAY_SHELL (display->shell)->unit,
                                      image->gimp->config->interpolation_type,
                                      image_scale_callback,
                                      dialog);

  g_object_weak_ref (G_OBJECT (dialog->dialog),
                     (GWeakNotify) g_free, dialog);

  dialog->callback = callback;

  return dialog;
}

static void
image_scale_callback (GtkWidget             *widget,
                      GimpViewable          *viewable,
                      gint                   width,
                      gint                   height,
                      GimpUnit               unit,
                      GimpInterpolationType  interpolation,
                      gdouble                xresolution,
                      gdouble                yresolution,
                      GimpUnit               resolution_unit,
                      gpointer               data)
{
  ImageScaleDialog        *dialog = data;
  GimpImage               *image  = GIMP_IMAGE (viewable);
  GimpImageScaleCheckType  scale_check;
  gint64                   max_memsize;
  gint64                   new_memsize;

  dialog->width           = width;
  dialog->height          = height;
  dialog->unit            = unit;
  dialog->interpolation   = interpolation;
  dialog->xresolution     = xresolution;
  dialog->yresolution     = yresolution;
  dialog->resolution_unit = resolution_unit;

  gtk_widget_set_sensitive (widget, FALSE);

  max_memsize = GIMP_GUI_CONFIG (image->gimp->config)->max_new_image_size;

  scale_check = gimp_image_scale_check (image,
                                        width, height, max_memsize,
                                        &new_memsize);
  switch (scale_check)
    {
    case GIMP_IMAGE_SCALE_TOO_BIG:
      image_scale_confirm_large (dialog, new_memsize, max_memsize);
      break;

    case GIMP_IMAGE_SCALE_TOO_SMALL:
      image_scale_confirm_small (dialog);
      break;

    case GIMP_IMAGE_SCALE_OK:
      /* If all is well, return directly after scaling image. */
      dialog->callback (dialog);
      gtk_widget_destroy (widget);
      break;
    }
}

static GtkWidget *
image_scale_confirm_dialog (ImageScaleDialog *dialog)
{
  GtkWidget *widget;

  widget = gimp_message_dialog_new (_("Confirm Scaling"),
                                    GIMP_STOCK_WARNING,
                                    dialog->dialog,
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    gimp_standard_help_func,
                                    GIMP_HELP_IMAGE_SCALE_WARNING,

                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                    GIMP_STOCK_SCALE, GTK_RESPONSE_OK,

                                    NULL);

  g_signal_connect (widget, "response",
                    G_CALLBACK (image_scale_confirm_response),
                    dialog);

  return widget;
}

static void
image_scale_confirm_large (ImageScaleDialog *dialog,
                           gint64            new_memsize,
                           gint64            max_memsize)
{
  GtkWidget *widget = image_scale_confirm_dialog (dialog);
  gchar     *size;

  size = gimp_memsize_to_string (new_memsize);
  gimp_message_box_set_primary_text (GIMP_MESSAGE_DIALOG (widget)->box,
                                     _("You are trying to create an image "
                                       "with a size of %s."), size);
  g_free (size);

  size = gimp_memsize_to_string (max_memsize);
  gimp_message_box_set_text (GIMP_MESSAGE_DIALOG (widget)->box,
                             _("Scaling the image to the choosen size will "
                               "make it use more memory than what is "
                               "configured as \"Maximum Image Size\" in the "
                               "Preferences dialog (currently %s)."), size);
  g_free (size);

  gtk_widget_show (widget);
}

static void
image_scale_confirm_small (ImageScaleDialog *dialog)
{
  GtkWidget *widget = image_scale_confirm_dialog (dialog);

  gimp_message_box_set_primary_text (GIMP_MESSAGE_DIALOG (widget)->box,
                                     _("Scaling the image to the choosen size "
                                       "will shrink some layers completely "
                                       "away."));
  gimp_message_box_set_text (GIMP_MESSAGE_DIALOG (widget)->box,
                             _("Is this what you want to do?"));

  gtk_widget_show (widget);
}

static void
image_scale_confirm_response (GtkWidget        *widget,
                              gint              response_id,
                              ImageScaleDialog *dialog)
{
  gtk_widget_destroy (widget);

  if (response_id == GTK_RESPONSE_OK)
    {
      dialog->callback (dialog);
      gtk_widget_destroy (dialog->dialog);
    }
  else
    {
      gtk_widget_set_sensitive (dialog->dialog, TRUE);
    }
}
