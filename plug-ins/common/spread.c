/* Spread --- image filter plug-in for The Gimp image manipulation program
 * Copyright (C) 1997 Brian Degenhardt and Federico Mena Quintero
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
 *
 * Please direct all comments, questions, bug reports  etc to Brian Degenhardt
 * bdegenha@ucsd.edu
 *
 * You can contact Federico Mena Quintero at quartic@polloux.fciencias.unam.mx
 * You can contact the original The Gimp authors at gimp@xcf.berkeley.edu
 */
#include "config.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

/* Some useful macros */
#define SCALE_WIDTH 200
#define TILE_CACHE_SIZE 16
#define ENTRY_WIDTH 50

typedef struct {
    gdouble spread_amount_x;
    gdouble spread_amount_y;
} SpreadValues;

typedef struct {
  gint run;
} SpreadInterface;


/* Declare local functions.
 */
static void      query  (void);
static void      run    (gchar    *name,
			 gint      nparams,
			 GParam   *param,
			 gint     *nreturn_vals,
			 GParam  **return_vals);
static void      spread  (GDrawable * drawable);

static gint      spread_dialog (void);
static GTile *   spread_pixel  (GDrawable * drawable,
			       GTile *     tile,
			       gint        x1,
			       gint        y1,
			       gint        x2,
			       gint        y2,
			       gint        x,
			       gint        y,
			       gint *      row,
			       gint *      col,
			       guchar *    pixel);

static void      spread_ok_callback     (GtkWidget *widget,
					gpointer   data);
static void      spread_fentry_callback   (GtkWidget     *widget,
					     gpointer       data);

static void      spread_fscale_callback   (GtkAdjustment *adjustment,
					     gpointer       data);
/***** Local vars *****/

GPlugInInfo PLUG_IN_INFO =
{
  NULL,    /* init_proc */
  NULL,    /* quit_proc */
  query,   /* query_proc */
  run,     /* run_proc */
};

static SpreadValues spvals =
{
  5,   /*  horizontal spread amount  */
  5   /*  vertical spread amount */
};

static SpreadInterface pint =
{
  FALSE   /*  run  */
};

/***** Functions *****/

MAIN ()

static void
query ()
{
  static GParamDef args[] =
  {
    { PARAM_INT32, "run_mode", "Interactive, non-interactive" },
    { PARAM_IMAGE, "image", "Input image (unused)" },
    { PARAM_DRAWABLE, "drawable", "Input drawable" },
    { PARAM_FLOAT, "spread_amount_x", "Horizontal spread amount (0 <= spread_amount_x <= 200)" },
    { PARAM_FLOAT, "spread_amount_y", "Vertical spread amount (0 <= spread_amount_y <= 200)" }
  };
  static GParamDef *return_vals = NULL;
  static gint nargs = sizeof (args) / sizeof (args[0]);
  static gint nreturn_vals = 0;

  INIT_I18N();

  gimp_install_procedure ("plug_in_spread",
			  _("Spread the contents of the specified drawable"),
			  _("Spreads the pixels of the specified drawable.  Pixels are randomly moved to another location whose distance varies from the original by the horizontal and vertical spread amounts "),
			  "Spencer Kimball and Peter Mattis, ported by Brian Degenhardt and Federico Mena Quintero",
			  "Federico Mena Quintero and Brian Degenhardt",
			  "1997",
              N_("<Image>/Filters/Noise/Spread..."),
			  "RGB*, GRAY*",
			  PROC_PLUG_IN,
			  nargs, nreturn_vals,
			  args, return_vals);
}

static void
run (gchar  *name,
     gint    nparams,
     GParam  *param,
     gint   *nreturn_vals,
     GParam **return_vals)
{
  static GParam values[1];
  GDrawable *drawable;
  GRunModeType run_mode;
  GStatusType status = STATUS_SUCCESS;

  run_mode = param[0].data.d_int32;

  /*  Get the specified drawable  */
  drawable = gimp_drawable_get (param[2].data.d_drawable);

  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = PARAM_STATUS;
  values[0].data.d_status = status;

  switch (run_mode)
    {
    case RUN_INTERACTIVE:
      INIT_I18N_UI();
      /*  Possibly retrieve data  */
      gimp_get_data ("plug_in_spread", &spvals);

      /*  First acquire information with a dialog  */
      if (! spread_dialog ())
	return;
      break;

    case RUN_NONINTERACTIVE:
      INIT_I18N();
      /*  Make sure all the arguments are there!  */
      if (nparams != 5)
	status = STATUS_CALLING_ERROR;
      if (status == STATUS_SUCCESS)
	{
	  spvals.spread_amount_x= param[3].data.d_float;
          spvals.spread_amount_y = param[4].data.d_float;
        }
      if ((status == STATUS_SUCCESS) &&
	  (spvals.spread_amount_x < 0 || spvals.spread_amount_x > 200) &&
          (spvals.spread_amount_y < 0 || spvals.spread_amount_y > 200))
	status = STATUS_CALLING_ERROR;
      break;

    case RUN_WITH_LAST_VALS:
      INIT_I18N();
      /*  Possibly retrieve data  */
      gimp_get_data ("plug_in_spread", &spvals);
      break;

    default:
      break;
    }

  if (status == STATUS_SUCCESS)
    {
      /*  Make sure that the drawable is gray or RGB color  */
      if (gimp_drawable_is_rgb (drawable->id) || gimp_drawable_is_gray (drawable->id))
	{
	  gimp_progress_init ( _("Spreading..."));

	  /*  set the tile cache size  */
	  gimp_tile_cache_ntiles (TILE_CACHE_SIZE);

	  /*  run the spread effect  */
	  spread (drawable);

	  if (run_mode != RUN_NONINTERACTIVE)
	    gimp_displays_flush ();

	  /*  Store data  */
	  if (run_mode == RUN_INTERACTIVE)
	    gimp_set_data ("plug_in_spread", &spvals, sizeof (SpreadValues));
	}
      else
	{
	  /* gimp_message ("spread: cannot operate on indexed color images"); */
	  status = STATUS_EXECUTION_ERROR;
	}
    }

  values[0].data.d_status = status;

  gimp_drawable_detach (drawable);
}

/*****/

static void
spread (GDrawable *drawable)
{
  GPixelRgn dest_rgn;
  GTile   * tile = NULL;
  gint      row = -1;
  gint      col = -1;
  gpointer  pr;

  gint    width, height;
  gint    bytes;
  guchar *destrow;
  guchar *dest;
  guchar  pixel[4][4];
  gint    x1, y1, x2, y2;
  gint    x, y;
  gint    progress, max_progress;

  gdouble x_amount, y_amount;
  gdouble angle;

  gint xdist, ydist;
  gint xi, yi;

  gint k;
  gint x_mod_value, x_sub_value;
  gint y_mod_value, y_sub_value;
  gint angle_mod_value, angle_sub_value;

  /* Get selection area */

  gimp_drawable_mask_bounds (drawable->id, &x1, &y1, &x2, &y2);

  width  = drawable->width;
  height = drawable->height;
  bytes  = drawable->bpp;

  progress     = 0;
  max_progress = (x2 - x1) * (y2 - y1);

  x_amount = spvals.spread_amount_x;
  y_amount = spvals.spread_amount_y;

  /* Initialize random stuff */
  srand (time (NULL));
  angle_mod_value = G_PI*2;
  angle_sub_value = angle_mod_value / 2;
  x_mod_value = x_amount + 1;
  x_sub_value = x_mod_value / 2;
  y_mod_value = y_amount + 1;
  y_sub_value = y_mod_value / 2;

  /* Spread the image.  This is done by going through every pixel
     in the source image and swapping it with some other random
     pixel.  The random pixel is located within an ellipse that is
     as high as the spread_amount_y parameter and as wide as the
     spread_amount_x parameter.  This is done by randomly selecting
     an angle and then multiplying the sine of the angle to a random
     number whose range is between -spread_amount_x/2 and spread_amount_x/2.
     The y coordinate is found by multiplying the cosine of the angle
     to the random value generated from spread_amount_y.  The reason
     that the spread is done this way is to make the end product more
     random looking.  To see a result of this, compare spreading a
     square with gimp 0.54 to spreading a square with this filter.
     The corners are less sharp with this algorithm.
  */



  /* Spread the image! */


  gimp_pixel_rgn_init (&dest_rgn, drawable, x1, y1, (x2 - x1), (y2 - y1), TRUE, TRUE);
  for (pr = gimp_pixel_rgns_register (1, &dest_rgn); pr != NULL; pr = gimp_pixel_rgns_process (pr))
    {
      destrow = dest_rgn.data;

      for (y = dest_rgn.y; y < (dest_rgn.y + dest_rgn.h); y++)
	{
	  dest = destrow;

	  for (x = dest_rgn.x; x < (dest_rgn.x + dest_rgn.w); x++)
	    {
              /* get random angle, x distance, and y distance */
              xdist = (rand () % x_mod_value) - x_sub_value;
              ydist = (rand () % y_mod_value) - y_sub_value;
              angle = (rand () % angle_mod_value) - angle_sub_value;

              xi = x + floor(sin(angle)*xdist);
              yi = y + floor(cos(angle)*ydist);

              /* Only displace the pixel if it's within the bounds of the image. */
              if ((xi >= 0) && (xi < width) && (yi >= 0) && (yi < height))
                  tile = spread_pixel (drawable, tile, x1, y1, x2, y2, xi, yi, &row, &col, pixel[0]);
	      else
              {
              /* Else just copy it */
                  tile = spread_pixel (drawable, tile, x1, y1, x2, y2, x, y, &row, &col, pixel[0]);
              }

              for (k = 0; k < bytes; k++)
                  *dest++ = pixel[0][k];
            } /* for */

	  destrow += dest_rgn.rowstride;;
	} /* for */

      progress += dest_rgn.w * dest_rgn.h;
      gimp_progress_update ((double) progress / (double) max_progress);
    }  /* for  */

  if (tile)
    gimp_tile_unref (tile, FALSE);

  /*  update the region  */
  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->id, TRUE);
  gimp_drawable_update (drawable->id, x1, y1, (x2 - x1), (y2 - y1));
} /* spread */


static gint
spread_dialog ()
{
  GtkWidget *dlg;
  GtkWidget *label;
  GtkWidget *scale;
  GtkWidget *frame;
  GtkWidget *hbox;
  GtkWidget *entry;
  GtkWidget *table;
  GtkObject *x_scale_data;
  GtkObject *y_scale_data;
  gchar **argv;
  gchar   buffer[32];
  gint    argc;

  argc    = 1;
  argv    = g_new (gchar *, 1);
  argv[0] = g_strdup ("spread");

  gtk_init (&argc, &argv);
  gtk_rc_parse (gimp_gtkrc ());

  dlg = gimp_dialog_new (_("Spread"), "spread",
			 gimp_plugin_help_func, "filters/spread.html",
			 GTK_WIN_POS_MOUSE,
			 FALSE, TRUE, FALSE,

			 _("OK"), spread_ok_callback,
			 NULL, NULL, NULL, TRUE, FALSE,
			 _("Cancel"), gtk_widget_destroy,
			 NULL, 1, NULL, FALSE, TRUE,

			 NULL);

  gtk_signal_connect (GTK_OBJECT (dlg), "destroy",
		      GTK_SIGNAL_FUNC (gtk_main_quit),
		      NULL);

  /*  parameter settings  */
  frame = gtk_frame_new ( _("Parameter Settings"));
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 6);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dlg)->vbox), frame, TRUE, TRUE, 0);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_table_set_row_spacings (GTK_TABLE (table), 2);
  gtk_container_set_border_width (GTK_CONTAINER (table), 4);
  gtk_container_add (GTK_CONTAINER (frame), table);

  /* Horizontal Amount */
  label = gtk_label_new (_("Horizontal Spread Amount:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
		    GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 0, 1,
		    GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  x_scale_data = gtk_adjustment_new (spvals.spread_amount_x, 0, 200, 1, 1, 0.0);
  gtk_signal_connect (GTK_OBJECT (x_scale_data), "value_changed",
		      (GtkSignalFunc) spread_fscale_callback,
		      &spvals.spread_amount_x);

  scale = gtk_hscale_new (GTK_ADJUSTMENT (x_scale_data));
  gtk_widget_set_usize (scale, SCALE_WIDTH, 0);
  gtk_scale_set_draw_value (GTK_SCALE (scale), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), scale, TRUE, TRUE, 0);
  gtk_widget_show (scale);

  entry = gtk_entry_new ();
  gtk_object_set_user_data (GTK_OBJECT (entry), x_scale_data);
  gtk_object_set_user_data (x_scale_data, entry);
  gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
  gtk_widget_set_usize (entry, ENTRY_WIDTH, 0);
  g_snprintf (buffer, sizeof (buffer), "%0.2f", spvals.spread_amount_x);
  gtk_entry_set_text (GTK_ENTRY (entry), buffer);
  gtk_signal_connect (GTK_OBJECT (entry), "changed",
		      (GtkSignalFunc) spread_fentry_callback,
		      &spvals.spread_amount_x);
  gtk_widget_show (entry);

  /* Vertical Amount */
  label = gtk_label_new (_("Vertical Spread Amount:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
		    GTK_FILL | GTK_EXPAND, GTK_FILL, 10, 5);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 1, 2,
		    GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  y_scale_data = gtk_adjustment_new (spvals.spread_amount_y, 0, 200, 1, 1, 0.0);
  gtk_signal_connect (GTK_OBJECT (y_scale_data), "value_changed",
		      (GtkSignalFunc) spread_fscale_callback,
		      &spvals.spread_amount_y);

  scale = gtk_hscale_new (GTK_ADJUSTMENT (y_scale_data));
  gtk_widget_set_usize (scale, SCALE_WIDTH, 0);
  gtk_scale_set_digits (GTK_SCALE (scale), 2);
  gtk_scale_set_draw_value (GTK_SCALE (scale), FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), scale, TRUE, TRUE, 0);
  gtk_widget_show (scale);

  entry = gtk_entry_new ();
  gtk_object_set_user_data (GTK_OBJECT (entry), y_scale_data);
  gtk_object_set_user_data (y_scale_data, entry);
  gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
  gtk_widget_set_usize (entry, ENTRY_WIDTH, 0);
  g_snprintf (buffer, sizeof (buffer), "%0.2f", spvals.spread_amount_y);
  gtk_entry_set_text (GTK_ENTRY (entry), buffer);
  gtk_signal_connect (GTK_OBJECT (entry), "changed",
		      (GtkSignalFunc) spread_fentry_callback,
		      &spvals.spread_amount_y);
  gtk_widget_show (entry);

  gtk_widget_show (frame);
  gtk_widget_show (table);
  gtk_widget_show (dlg);

  gtk_main ();
  gdk_flush ();

  return pint.run;
}

/*****/

static GTile *
spread_pixel (GDrawable * drawable,
	     GTile *     tile,
	     gint        x1,
	     gint        y1,
	     gint        x2,
	     gint        y2,
	     gint        x,
	     gint        y,
	     gint *      row,
	     gint *      col,
	     guchar *    pixel)
{
  static guchar empty_pixel[4] = {0, 0, 0, 0};
  guchar *data;
  gint b;

  if (x >= x1 && y >= y1 && x < x2 && y < y2)
    {
      if ((x >> 6 != *col) || (y >> 6 != *row))
	{
	  *col = x / 64;
	  *row = y / 64;
	  if (tile)
	    gimp_tile_unref (tile, FALSE);
	  tile = gimp_drawable_get_tile (drawable, FALSE, *row, *col);
	  gimp_tile_ref (tile);
	}

      data = tile->data + tile->bpp * (tile->ewidth * (y % 64) + (x % 64));
    }
  else
    data = empty_pixel;

  for (b = 0; b < drawable->bpp; b++)
    pixel[b] = data[b];

  return tile;
}




/*  Spread interface functions  */

static void
spread_ok_callback (GtkWidget *widget,
		   gpointer   data)
{
  pint.run = TRUE;
  gtk_widget_destroy (GTK_WIDGET (data));
}

static void
spread_fscale_callback (GtkAdjustment *adjustment,
			gpointer       data)
{
  GtkWidget *entry;
  gchar buffer[32];
  double *val;

  val = data;
  if (*val != adjustment->value)
    {
      *val = adjustment->value;
      entry = gtk_object_get_user_data (GTK_OBJECT (adjustment));
      g_snprintf (buffer, sizeof (buffer), "%0.2f", adjustment->value);

      gtk_signal_handler_block_by_data (GTK_OBJECT (entry), data);
      gtk_entry_set_text (GTK_ENTRY (entry), buffer);
      gtk_signal_handler_unblock_by_data (GTK_OBJECT (entry), data);
    }
}

static void
spread_fentry_callback (GtkWidget *widget,
			gpointer   data)
{
  GtkAdjustment *adjustment;
  double new_val;
  double *val;

  val = data;
  new_val = atof (gtk_entry_get_text (GTK_ENTRY (widget)));

  if (*val != new_val)
    {
      adjustment = gtk_object_get_user_data (GTK_OBJECT (widget));

      if ((new_val >= adjustment->lower) &&
	  (new_val <= adjustment->upper))
	{
	  *val = new_val;
	  adjustment->value = new_val;
	  gtk_signal_emit_by_name (GTK_OBJECT (adjustment), "value_changed");
	}
    }
}
