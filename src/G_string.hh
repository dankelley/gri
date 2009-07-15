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

#if 0
#if !defined(_GString_hh_)
#define _GString_hh_

#include <string>
#include "types.hh"


class G_string : public string
{
public:
	bool line_from_FILE(FILE *fp); // Get Line from file, true if EOF
#if 0
	bool word_from_FILE(FILE *fp); // Get a word from file, true if EOF
#endif
#if 0
	void draw(double xcm, double ycm, gr_textStyle s, double angle) const;
#endif
#if 0
	void sed(const char *cmd);	  // Modify by (limited) sed command
#endif
};
#endif
#endif
