/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// define some filenames
#if !defined(_files_hh_)

#define _files_hh_

#if !defined(HAVE_TMPNAM)
#define GRI_TMP_FILE                    "TMP.GRI"
#endif

#if defined(VMS)
#define GRIRC_FILE              	"GRI$MACROS:GRI.RC"
#define GRI_COMMANDS_FILE       	"GRI.CMD"
#else
#define GRIRC_FILE              	".grirc"
#define GRI_COMMANDS_FILE       	"gri.cmd"
#endif
#if defined(HAVE_LIBNETCDF)
#include        <netcdf.h>
#endif

#endif				// _files_hh_
