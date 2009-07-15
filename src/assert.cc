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

#include        <string>
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "defaults.hh"
#include        "files.hh"
#include        "superus.hh"

bool            assertCmd(void);

bool assertCmd()
{
	if (_nword > 3) {
		demonstrate_command_usage();
		err("Too many words in `assert'");
		return false;
	}
	if (_nword < 2) {
		demonstrate_command_usage();
		err("Too few words in `assert'");
		return false;
	}
	double cond;
	if (!getdnum(_word[1], &cond)) {
		err("`assert' cannot decode the condition");
		return false;
	}
	if (cond) {
		return true;
	} else {
		std::string msg;
		if (_nword == 3) {
			msg = _word[2];
			clean_blanks_quotes(msg);
		}
		if (msg.empty()) {
			printf("Failed assertion at ?file? line ?line?.\n");
		} else {
			if (msg.size() > 2
			    && msg[msg.size() - 1] == 'n'
			    && msg[msg.size() - 2] == '\\') {
				ShowStr(msg.c_str());
			} else {
				ShowStr(msg.c_str());
				printf(" at ?file? line ?line?.\n");
			}
		}
		gri_exit(1);
		return false;	// never executed
	}
}
