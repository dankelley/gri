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

#include	"gr.hh"
#include	"extern.hh"
#include	"GMatrix.hh"

#if defined(IS_MINGW32)
#include        <windows.h>
#endif


bool
sleepCmd()
{
	if (_nword != 2) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	double sec_f;
	if (!getdnum(_word[1], &sec_f)) {
		err("`sleep' can't read .sec. in `\\", _word[1], "'.", "\\");
		return false;
	}
	if (sec_f <= 0.0)
		return true;
	int sec = int(floor(0.5 + sec_f));
#if !defined(IS_MINGW32)
	sleep(sec); 
#else
	Sleep(sec*1000);
#endif
	return true;
}
