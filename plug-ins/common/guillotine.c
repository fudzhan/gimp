/*
 *  Guillotine plug-in v0.9 by Adam D. Moss, adam@foxbox.org.  1998/09/01
 */

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

/*
 * HISTORY:
 *     0.9 : 1998/09/01
 *           Initial release.
 */

#include "config.h"

#include <stdlib.h>

#include <libgimp/gimp.h>

#include "libgimp/stdplugins-intl.h"


/* Declare local functions.
 */
static void   query      (void);
static void   run        (const gchar      *name,
                          gint              nparams,
                          const GimpParam  *param,
                          gint             *nreturn_vals,
                          GimpParam       **return_vals);

static void   guillotine (gint32            image_ID);


GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};


MAIN ()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    { GIMP_PDB_INT32,    "run_mode", "Interactive, non-interactive" },
    { GIMP_PDB_IMAGE,    "image",    "Input image"                  },
    { GIMP_PDB_DRAWABLE, "drawable", "Input drawable (unused)"      }
  };

  gimp_install_procedure ("plug_in_guillotine",
                          "Slice up the image into subimages, cutting along "
                          "the image's Guides.  Fooey to you and your "
                          "broccoli, Pokey.",
                          "This function takes an image and blah blah.  Hooray!",
                          "Adam D. Moss (adam@foxbox.org)",
                          "Adam D. Moss (adam@foxbox.org)",
                          "1998",
                          N_("<Image>/Image/Transform/_Guillotine"),
                          "RGB*, INDEXED*, GRAY*",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (args), 0,
                          args, NULL);
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[1];
  gint32            image_ID;
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;

  *nreturn_vals = 1;
  *return_vals  = values;

  values[0].type          = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  INIT_I18N();

  image_ID = param[1].data.d_image;

  if (status == GIMP_PDB_SUCCESS)
    {
      gimp_progress_init (_("Guillotine..."));
      guillotine (image_ID);
      gimp_displays_flush ();
    }

  values[0].data.d_status = status;
}


static gint
guide_sort_func (gconstpointer a,
		 gconstpointer b)
{
  return GPOINTER_TO_INT (a) - GPOINTER_TO_INT (b);
}

static void
guillotine (gint32 image_ID)
{
  gint      guide;
  gint      image_width;
  gint      image_height;
  gboolean  guides_found = FALSE;
  GList    *hguides, *hg;
  GList    *vguides, *vg;

  image_width  = gimp_image_width (image_ID);
  image_height = gimp_image_height (image_ID);

  hguides = g_list_append (NULL,    GINT_TO_POINTER (0));
  hguides = g_list_append (hguides, GINT_TO_POINTER (image_height));

  vguides = g_list_append (NULL,    GINT_TO_POINTER (0));
  vguides = g_list_append (vguides, GINT_TO_POINTER (image_width));

  for (guide = gimp_image_find_next_guide (image_ID, 0);
       guide > 0;
       guide = gimp_image_find_next_guide (image_ID, guide))
    {
      gint position = gimp_image_get_guide_position (image_ID, guide);

      switch (gimp_image_get_guide_orientation (image_ID, guide))
        {
        case GIMP_ORIENTATION_HORIZONTAL:
          if (! g_list_find (hguides, GINT_TO_POINTER (position)))
            {
              hguides = g_list_insert_sorted (hguides,
                                              GINT_TO_POINTER (position),
                                              guide_sort_func);
              guides_found = TRUE;
            }
          break;

        case GIMP_ORIENTATION_VERTICAL:
          if (! g_list_find (vguides, GINT_TO_POINTER (position)))
            {
              vguides = g_list_insert_sorted (vguides,
                                              GINT_TO_POINTER (position),
                                              guide_sort_func);
              guides_found = TRUE;
            }
          break;

        case GIMP_ORIENTATION_UNKNOWN:
          g_assert_not_reached ();
          break;
        }
    }

  if (guides_found)
    {
      gchar *filename;
      gint   x, y;

      filename = gimp_image_get_filename (image_ID);
      if (!filename)
        filename = g_strdup (_("Untitled"));

      /*  Do the actual dup'ing and cropping... this isn't a too naive a
       *  way to do this since we got copy-on-write tiles, either.
       */
      for (y = 0, hg = hguides; hg && hg->next; y++, hg = hg->next)
        {
          for (x = 0, vg = vguides; vg && vg->next; x++, vg = vg->next)
            {
              gint32  new_image = gimp_image_duplicate (image_ID);
              gchar  *new_filename;

              if (new_image == -1)
                {
                  g_warning ("Couldn't create new image.");
                  return;
                }

              gimp_image_undo_disable (new_image);

              gimp_image_crop (new_image,
                               GPOINTER_TO_INT (vg->next->data) -
                               GPOINTER_TO_INT (vg->data),
                               GPOINTER_TO_INT (hg->next->data) -
                               GPOINTER_TO_INT (hg->data),
                               GPOINTER_TO_INT (vg->data),
                               GPOINTER_TO_INT (hg->data));

              /* show the rough coordinates of the image in the title */
              new_filename = g_strdup_printf ("%s-(%i,%i)", filename, x, y);
              gimp_image_set_filename (new_image, new_filename);
              g_free (new_filename);

              while ((guide = gimp_image_find_next_guide (new_image, 0)))
                gimp_image_delete_guide (new_image, guide);

              gimp_image_undo_enable (new_image);

              gimp_display_new (new_image);
            }
        }

      g_free (filename);
    }

  g_list_free (hguides);
  g_list_free (vguides);
}
