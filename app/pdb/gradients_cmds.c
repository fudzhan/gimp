/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"

#include <string.h>

#include <glib-object.h>

#include "libgimpbase/gimpbasetypes.h"

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/gimp.h"
#include "core/gimpcontainer-filter.h"
#include "core/gimpcontainer.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimpgradient.h"
#include "core/gimplist.h"

static ProcRecord gradients_refresh_proc;
static ProcRecord gradients_get_list_proc;
static ProcRecord gradients_get_gradient_proc;
static ProcRecord gradients_set_gradient_proc;
static ProcRecord gradients_sample_uniform_proc;
static ProcRecord gradients_sample_custom_proc;
static ProcRecord gradients_get_gradient_data_proc;
static ProcRecord gradients_new_proc;
static ProcRecord gradients_duplicate_proc;
static ProcRecord gradients_delete_proc;
static ProcRecord gradients_rename_proc;

void
register_gradients_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &gradients_refresh_proc);
  procedural_db_register (gimp, &gradients_get_list_proc);
  procedural_db_register (gimp, &gradients_get_gradient_proc);
  procedural_db_register (gimp, &gradients_set_gradient_proc);
  procedural_db_register (gimp, &gradients_sample_uniform_proc);
  procedural_db_register (gimp, &gradients_sample_custom_proc);
  procedural_db_register (gimp, &gradients_get_gradient_data_proc);
  procedural_db_register (gimp, &gradients_new_proc);
  procedural_db_register (gimp, &gradients_duplicate_proc);
  procedural_db_register (gimp, &gradients_delete_proc);
  procedural_db_register (gimp, &gradients_rename_proc);
}

static Argument *
gradients_refresh_invoker (Gimp        *gimp,
                           GimpContext *context,
                           Argument    *args)
{
  gimp_data_factory_data_save (gimp->gradient_factory);
  gimp_data_factory_data_init (gimp->gradient_factory, FALSE);
  return procedural_db_return_args (&gradients_refresh_proc, TRUE);
}

static ProcRecord gradients_refresh_proc =
{
  "gimp_gradients_refresh",
  "Refresh current gradients. This function always succeeds.",
  "This procedure retrieves all gradients currently in the user's gradient path and updates the gradient dialogs accordingly.",
  "Michael Natterer",
  "Michael Natterer",
  "2002",
  GIMP_INTERNAL,
  0,
  NULL,
  0,
  NULL,
  { { gradients_refresh_invoker } }
};

static Argument *
gradients_get_list_invoker (Gimp        *gimp,
                            GimpContext *context,
                            Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *filter;
  gint32 num_gradients;
  gchar **gradient_list = NULL;

  filter = (gchar *) args[0].value.pdb_pointer;
  if (filter && !g_utf8_validate (filter, -1, NULL))
    success = FALSE;

  if (success)
    gradient_list = gimp_container_get_filtered_name_array (gimp->gradient_factory->container, filter, &num_gradients);

  return_args = procedural_db_return_args (&gradients_get_list_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = num_gradients;
      return_args[2].value.pdb_pointer = gradient_list;
    }

  return return_args;
}

static ProcArg gradients_get_list_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "filter",
    "An optional regular expression used to filter the list"
  }
};

static ProcArg gradients_get_list_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "num_gradients",
    "The number of loaded gradients"
  },
  {
    GIMP_PDB_STRINGARRAY,
    "gradient_list",
    "The list of gradient names"
  }
};

static ProcRecord gradients_get_list_proc =
{
  "gimp_gradients_get_list",
  "Retrieve the list of loaded gradients.",
  "This procedure returns a list of the gradients that are currently loaded in the gradient editor. You can later use the gimp_gradients_set_active function to set the active gradient.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  GIMP_INTERNAL,
  1,
  gradients_get_list_inargs,
  2,
  gradients_get_list_outargs,
  { { gradients_get_list_invoker } }
};

static Argument *
gradients_get_gradient_invoker (Gimp        *gimp,
                                GimpContext *context,
                                Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;

  success = gimp_context_get_gradient (context) != NULL;

  return_args = procedural_db_return_args (&gradients_get_gradient_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (gimp_context_get_gradient (context))->name);

  return return_args;
}

static ProcArg gradients_get_gradient_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the active gradient"
  }
};

static ProcRecord gradients_get_gradient_proc =
{
  "gimp_gradients_get_gradient",
  "Retrieve the name of the active gradient.",
  "This procedure returns the name of the active gradient in the gradient editor.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  GIMP_INTERNAL,
  0,
  NULL,
  1,
  gradients_get_gradient_outargs,
  { { gradients_get_gradient_invoker } }
};

static Argument *
gradients_set_gradient_invoker (Gimp        *gimp,
                                GimpContext *context,
                                Argument    *args)
{
  gboolean success = TRUE;
  gchar *name;
  GimpGradient *gradient;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      gradient = (GimpGradient *)
        gimp_container_get_child_by_name (gimp->gradient_factory->container, name);

      if (gradient)
        gimp_context_set_gradient (context, gradient);
      else
        success = FALSE;
    }

  return procedural_db_return_args (&gradients_set_gradient_proc, success);
}

static ProcArg gradients_set_gradient_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the gradient to set"
  }
};

static ProcRecord gradients_set_gradient_proc =
{
  "gimp_gradients_set_gradient",
  "Sets the specified gradient as the active gradient.",
  "This procedure lets you set the specified gradient as the active or \"current\" one. The name is simply a string which corresponds to one of the loaded gradients in the gradient editor. If no matching gradient is found, this procedure will return an error. Otherwise, the specified gradient will become active and will be used for subsequent custom gradient operations.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  GIMP_INTERNAL,
  1,
  gradients_set_gradient_inargs,
  0,
  NULL,
  { { gradients_set_gradient_invoker } }
};

static Argument *
gradients_sample_uniform_invoker (Gimp        *gimp,
                                  GimpContext *context,
                                  Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gint32 i;
  gboolean reverse;
  gint32 array_length = 0;
  gdouble *color_samples = NULL;
  GimpGradient *gradient;
  gdouble pos, delta;
  GimpRGB color;
  gdouble *pv;

  i = args[0].value.pdb_int;
  if (i < 2)
    success = FALSE;

  reverse = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      pos = 0.0;
      delta = 1.0 / (i - 1);

      array_length = i * 4;

      pv = color_samples = g_new (gdouble, array_length);

      gradient = gimp_context_get_gradient (context);

      while (i--)
        {
          gimp_gradient_get_color_at (gradient, pos, reverse, &color);

          *pv++ = color.r;
          *pv++ = color.g;
          *pv++ = color.b;
          *pv++ = color.a;

          pos += delta;
        }
    }

  return_args = procedural_db_return_args (&gradients_sample_uniform_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = array_length;
      return_args[2].value.pdb_pointer = color_samples;
    }

  return return_args;
}

static ProcArg gradients_sample_uniform_inargs[] =
{
  {
    GIMP_PDB_INT32,
    "num_samples",
    "The number of samples to take"
  },
  {
    GIMP_PDB_INT32,
    "reverse",
    "Use the reverse gradient (TRUE or FALSE)"
  }
};

static ProcArg gradients_sample_uniform_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "array_length",
    "Length of the color_samples array (4 * num_samples)"
  },
  {
    GIMP_PDB_FLOATARRAY,
    "color_samples",
    "Color samples: { R1, G1, B1, A1, ..., Rn, Gn, Bn, An }"
  }
};

static ProcRecord gradients_sample_uniform_proc =
{
  "gimp_gradients_sample_uniform",
  "Sample the active gradient in uniform parts.",
  "This procedure samples the active gradient from the gradient editor in the specified number of uniform parts. It returns a list of floating-point values which correspond to the RGBA values for each sample. The minimum number of samples to take is 2, in which case the returned colors will correspond to the { 0.0, 1.0 } positions in the gradient. For example, if the number of samples is 3, the procedure will return the colors at positions { 0.0, 0.5, 1.0 }.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  GIMP_INTERNAL,
  2,
  gradients_sample_uniform_inargs,
  2,
  gradients_sample_uniform_outargs,
  { { gradients_sample_uniform_invoker } }
};

static Argument *
gradients_sample_custom_invoker (Gimp        *gimp,
                                 GimpContext *context,
                                 Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gint32 i;
  gdouble *pos;
  gboolean reverse;
  gint32 array_length = 0;
  gdouble *color_samples = NULL;
  GimpGradient *gradient;
  GimpRGB color;
  gdouble *pv;

  i = args[0].value.pdb_int;
  if (i <= 0)
    success = FALSE;

  pos = (gdouble *) args[1].value.pdb_pointer;

  reverse = args[2].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      array_length = i * 4;

      pv = color_samples = g_new (gdouble, array_length);

      gradient = gimp_context_get_gradient (context);

      while (i--)
        {
          gimp_gradient_get_color_at (gradient, *pos, reverse, &color);

          *pv++ = color.r;
          *pv++ = color.g;
          *pv++ = color.b;
          *pv++ = color.a;

          pos++;
        }
    }

  return_args = procedural_db_return_args (&gradients_sample_custom_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = array_length;
      return_args[2].value.pdb_pointer = color_samples;
    }

  return return_args;
}

static ProcArg gradients_sample_custom_inargs[] =
{
  {
    GIMP_PDB_INT32,
    "num_samples",
    "The number of samples to take"
  },
  {
    GIMP_PDB_FLOATARRAY,
    "positions",
    "The list of positions to sample along the gradient"
  },
  {
    GIMP_PDB_INT32,
    "reverse",
    "Use the reverse gradient (TRUE or FALSE)"
  }
};

static ProcArg gradients_sample_custom_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "array_length",
    "Length of the color_samples array (4 * num_samples)"
  },
  {
    GIMP_PDB_FLOATARRAY,
    "color_samples",
    "Color samples: { R1, G1, B1, A1, ..., Rn, Gn, Bn, An }"
  }
};

static ProcRecord gradients_sample_custom_proc =
{
  "gimp_gradients_sample_custom",
  "Sample the active gradient in custom positions.",
  "This procedure samples the active gradient from the gradient editor in the specified number of points. The procedure will sample the gradient in the specified positions from the list. The left endpoint of the gradient corresponds to position 0.0, and the right endpoint corresponds to 1.0. The procedure returns a list of floating-point values which correspond to the RGBA values for each sample.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  GIMP_INTERNAL,
  3,
  gradients_sample_custom_inargs,
  2,
  gradients_sample_custom_outargs,
  { { gradients_sample_custom_invoker } }
};

static Argument *
gradients_get_gradient_data_invoker (Gimp        *gimp,
                                     GimpContext *context,
                                     Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gint32 sample_size;
  gboolean reverse;
  gdouble *values = NULL;
  GimpGradient *gradient = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  sample_size = args[1].value.pdb_int;
  if (sample_size <= 0 || sample_size > 10000)
    sample_size = GIMP_GRADIENT_DEFAULT_SAMPLE_SIZE;

  reverse = args[2].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      if (strlen (name))
        {
          gradient = (GimpGradient *)
            gimp_container_get_child_by_name (gimp->gradient_factory->container,
                                              name);
        }
      else
        {
          gradient = gimp_context_get_gradient (context);
        }

      if (gradient)
        {
          gdouble *pv;
          gdouble  pos, delta;
          GimpRGB  color;
          gint     i;

          i     = sample_size;
          pos   = 0.0;
          delta = 1.0 / (i - 1);

          pv = values = g_new (gdouble, i * 4);

          while (i--)
            {
              gimp_gradient_get_color_at (gradient, pos, reverse, &color);

              *pv++ = color.r;
              *pv++ = color.g;
              *pv++ = color.b;
              *pv++ = color.a;

              pos += delta;
            }
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&gradients_get_gradient_data_proc, success);

  if (success)
    {
      return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (gradient)->name);
      return_args[2].value.pdb_int = sample_size * 4;
      return_args[3].value.pdb_pointer = values;
    }

  return return_args;
}

static ProcArg gradients_get_gradient_data_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The gradient name (\"\" means current active gradient)"
  },
  {
    GIMP_PDB_INT32,
    "sample_size",
    "Size of the sample to return when the gradient is changed (0 < sample_size <= 10000)"
  },
  {
    GIMP_PDB_INT32,
    "reverse",
    "Use the reverse gradient (TRUE or FALSE)"
  }
};

static ProcArg gradients_get_gradient_data_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The gradient name"
  },
  {
    GIMP_PDB_INT32,
    "width",
    "The gradient sample width (r,g,b,a)"
  },
  {
    GIMP_PDB_FLOATARRAY,
    "grad_data",
    "The gradient sample data"
  }
};

static ProcRecord gradients_get_gradient_data_proc =
{
  "gimp_gradients_get_gradient_data",
  "Retrieve information about the specified gradient (including data).",
  "This procedure retrieves information about the gradient. This includes the gradient name, and the sample data for the gradient.",
  "Federico Mena Quintero",
  "Federico Mena Quintero",
  "1997",
  GIMP_INTERNAL,
  3,
  gradients_get_gradient_data_inargs,
  3,
  gradients_get_gradient_data_outargs,
  { { gradients_get_gradient_data_invoker } }
};

static Argument *
gradients_new_invoker (Gimp        *gimp,
                       GimpContext *context,
                       Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  GimpGradient * gradient = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      gradient = (GimpGradient *)
        gimp_data_factory_data_new (gimp->gradient_factory, name);
    }

  return_args = procedural_db_return_args (&gradients_new_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (gradient)->name);

  return return_args;
}

static ProcArg gradients_new_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The requested name of the new gradient"
  }
};

static ProcArg gradients_new_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The actual new gradient name"
  }
};

static ProcRecord gradients_new_proc =
{
  "gimp_gradients_new",
  "Creates a new gradient",
  "This procedure creates a new, uninitialized gradient",
  "Shlomi Fish",
  "Shlomi Fish",
  "2004",
  GIMP_INTERNAL,
  1,
  gradients_new_inargs,
  1,
  gradients_new_outargs,
  { { gradients_new_invoker } }
};

static Argument *
gradients_duplicate_invoker (Gimp        *gimp,
                             GimpContext *context,
                             Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  GimpGradient *gradient = NULL;
  GimpGradient *gradient_copy = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (strlen (name))
      {
        gradient = (GimpGradient *)
          gimp_container_get_child_by_name (gimp->gradient_factory->container,
                                            name);
      }
    else
      {
        gradient = gimp_context_get_gradient (context);
      }

      if (gradient)
        {
          gradient_copy = (GimpGradient *)
            gimp_data_factory_data_duplicate (gimp->gradient_factory,
                                              GIMP_DATA (gradient));

          success = (gradient_copy != NULL);
        }
       else
        {
          success = FALSE;
        }
    }

  return_args = procedural_db_return_args (&gradients_duplicate_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (gradient_copy)->name);

  return return_args;
}

static ProcArg gradients_duplicate_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the gradient to duplicate"
  }
};

static ProcArg gradients_duplicate_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the gradient's copy"
  }
};

static ProcRecord gradients_duplicate_proc =
{
  "gimp_gradients_duplicate",
  "Duplicates a gradient",
  "This procedure creates an identical gradient by a different name",
  "Shlomi Fish",
  "Shlomi Fish",
  "2004",
  GIMP_INTERNAL,
  1,
  gradients_duplicate_inargs,
  1,
  gradients_duplicate_outargs,
  { { gradients_duplicate_invoker } }
};

static Argument *
gradients_delete_invoker (Gimp        *gimp,
                          GimpContext *context,
                          Argument    *args)
{
  gboolean success = TRUE;
  gchar *name;
  GimpGradient *gradient = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (strlen (name))
      {
        gradient = (GimpGradient *)
          gimp_container_get_child_by_name (gimp->gradient_factory->container,
                                            name);
      }
    else
      {
        gradient = gimp_context_get_gradient (context);
      }

      success = (gradient && GIMP_DATA (gradient)->deletable);

      if (success)
        {
          GError *error = NULL;

          success = gimp_data_factory_data_delete (gimp->gradient_factory,
                                                   GIMP_DATA (gradient),
                                                   TRUE, &error);

          if (! success)
            {
              g_message (error->message);
              g_clear_error (&error);
            }
        }
    }

  return procedural_db_return_args (&gradients_delete_proc, success);
}

static ProcArg gradients_delete_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the gradient to delete"
  }
};

static ProcRecord gradients_delete_proc =
{
  "gimp_gradients_delete",
  "Deletes a gradient",
  "This procedure deletes a gradient",
  "Shlomi Fish",
  "Shlomi Fish",
  "2004",
  GIMP_INTERNAL,
  1,
  gradients_delete_inargs,
  0,
  NULL,
  { { gradients_delete_invoker } }
};

static Argument *
gradients_rename_invoker (Gimp        *gimp,
                          GimpContext *context,
                          Argument    *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gchar *new_name;
  GimpGradient *gradient = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  new_name = (gchar *) args[1].value.pdb_pointer;
  if (new_name == NULL || !g_utf8_validate (new_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (strlen (name))
      {
        gradient = (GimpGradient *)
          gimp_container_get_child_by_name (gimp->gradient_factory->container,
                                            name);
      }
    else
      {
        gradient = gimp_context_get_gradient (context);
      }

      success = (gradient && GIMP_DATA (gradient)->writable);

      if (success)
        gimp_object_set_name (GIMP_OBJECT (gradient), new_name);
    }

  return_args = procedural_db_return_args (&gradients_rename_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (gradient)->name);

  return return_args;
}

static ProcArg gradients_rename_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the gradient to rename"
  },
  {
    GIMP_PDB_STRING,
    "new_name",
    "The new name of the gradient"
  }
};

static ProcArg gradients_rename_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The actual new name of the gradient"
  }
};

static ProcRecord gradients_rename_proc =
{
  "gimp_gradients_rename",
  "Rename a gradient",
  "This procedure renames a gradient",
  "Shlomi Fish",
  "Shlomi Fish",
  "2004",
  GIMP_INTERNAL,
  2,
  gradients_rename_inargs,
  1,
  gradients_rename_outargs,
  { { gradients_rename_invoker } }
};
