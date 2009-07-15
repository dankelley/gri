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

#include	<string>
#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#include	"gr.hh"
#include	"debug.hh"
#include	"extern.hh"

bool
debugCmd()
{
	int             debug;
	switch (_nword) {
	case 2:
		/* debug .n.|off */
		if (!strcmp(_word[1], "off")) {
			debug = 0;
			PUT_VAR("..debug..", debug);
			_debugFlag = 0;
			return 1;
		}
		getinum(_word[1], &debug);
		break;
	case 1:
		/* `debug' -- equivalent to `debug 1' */
		debug = 1;
		_debugFlag = 1;
		break;
	case 6:
		/* `debug clipped values in draw commands' */
		if (!strcmp(_word[1], "clipped")
		    && !strcmp(_word[2], "values")
		    && !strcmp(_word[3], "in")
		    && !strcmp(_word[4], "draw")
		    && !strcmp(_word[5], "commands")) {
			_debugFlag |= DEBUG_CLIPPED;
			return 1;
		} else {
			demonstrate_command_usage();
			err("Can't understand command.");
			return 0;
		}
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return 0;
	}
	PUT_VAR("..debug..", debug);
	return 1;
}
