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


#include <glib-object.h>

#include "libgimpbase/gimpbasetypes.h"

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/gimp.h"
#include "core/gimpcontainer.h"
#include "text/gimpfonts.h"

static ProcRecord fonts_refresh_proc;
static ProcRecord fonts_get_list_proc;

void
register_fonts_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &fonts_refresh_proc);
  procedural_db_register (gimp, &fonts_get_list_proc);
}

static Argument *
fonts_refresh_invoker (Gimp     *gimp,
                       Argument *args)
{
  gimp_fonts_load (gimp);
  return procedural_db_return_args (&fonts_refresh_proc, TRUE);
}

static ProcRecord fonts_refresh_proc =
{
  "gimp_fonts_refresh",
  "Refresh current fonts.",
  "This procedure retrieves all fonts currently in the user's font path and updates the font dialogs accordingly.",
  "Sven Neumann",
  "Sven Neumann",
  "2003",
  GIMP_INTERNAL,
  0,
  NULL,
  0,
  NULL,
  { { fonts_refresh_invoker } }
};

static Argument *
fonts_get_list_invoker (Gimp     *gimp,
                        Argument *args)
{
  Argument *return_args;
  gint32 num_fonts;
  gchar **font_list;

  font_list = gimp_container_get_name_array (gimp->fonts, &num_fonts);

  return_args = procedural_db_return_args (&fonts_get_list_proc, TRUE);

  return_args[1].value.pdb_int = num_fonts;
  return_args[2].value.pdb_pointer = font_list;

  return return_args;
}

static ProcArg fonts_get_list_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "num_fonts",
    "The number of available fonts"
  },
  {
    GIMP_PDB_STRINGARRAY,
    "font_list",
    "The list of font names"
  }
};

static ProcRecord fonts_get_list_proc =
{
  "gimp_fonts_get_list",
  "Retrieve the list of loaded fonts.",
  "This procedure returns a list of the fonts that are currently available.",
  "Sven Neumann",
  "Sven Neumann",
  "2003",
  GIMP_INTERNAL,
  0,
  NULL,
  2,
  fonts_get_list_outargs,
  { { fonts_get_list_invoker } }
};
