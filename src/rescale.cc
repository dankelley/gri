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
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"

extern char     _grTempString[];

bool
rescaleCmd()
{
	if (_nword == 1) {
		if (_chatty > 2) {
			sprintf(_grTempString, "Rescaling x/y axes\n");
			ShowStr(_grTempString);
		}
		_need_x_axis = true;
		_need_y_axis = true;
		create_x_scale();
		create_y_scale();
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
}
