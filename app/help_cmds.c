/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2000 Spencer Kimball and Peter Mattis
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

#include "procedural_db.h"

#include "gimphelp.h"
#include "plug_in.h"

static ProcRecord help_proc;

void
register_help_procs (void)
{
  procedural_db_register (&help_proc);
}

static Argument *
help_invoker (Argument *args)
{
  gboolean success = TRUE;
  gchar *prog_name;
  gchar *help_page;

  prog_name = (gchar *) args[0].value.pdb_pointer;
  if (prog_name == NULL)
    success = FALSE;

  help_page = (gchar *) args[1].value.pdb_pointer;
  if (help_page == NULL)
    success = FALSE;

  if (success)
    gimp_help (plug_in_help_path (prog_name), help_page);

  return procedural_db_return_args (&help_proc, success);
}

static ProcArg help_inargs[] =
{
  {
    PDB_STRING,
    "prog_name",
    "The plug-in's executable name or an empty string"
  },
  {
    PDB_STRING,
    "help_page",
    "The location of the help page"
  }
};

static ProcRecord help_proc =
{
  "gimp_help",
  "Load a help page.",
  "This procedure loads the specified help page into the helpbrowser or what ever is configured as help viewer. The location of the help page is given relative to the help rootdir. The help rootdir is determined from the prog_name: if prog_name is NULL, we use the help rootdir of the main GIMP installation, if the plug-in's full executable name is passed as prog_name, the GIMP will use this information to look up the help path the plug-in has registered before with gimp-plugin-help-register.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2000",
  PDB_INTERNAL,
  2,
  help_inargs,
  0,
  NULL,
  { { help_invoker } }
};
