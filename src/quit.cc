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
#include	"gr.hh"
#include	"extern.hh"

bool
quitCmd()
{
	double tmp;
	switch (_nword) {
	case 1:
		_done = 1;
		return true;
	case 2:
		if (!getdnum(_word[1], &tmp)) {
			fatal_err("Cannot interpret `quit' exit value `\\", _word[1], "'", "\\");
		}
		_exit_value = (int) floor(0.5 + tmp);
		_done = 1;
		return true;
	default:
		demonstrate_command_usage();
		fatal_err("Extra word in command");
		return false;
	}
}
