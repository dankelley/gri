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

// NB: _gri_number will be checked against line 1 of gri.cmd
#ifdef OSX_BUNDLE
char _gri_number[] = PACKAGE_VERSION;
#else
// Don't ask me why, but I used to do
//        #define stringify(x) # x
//        char _gri_number[] = stringify(VERSION);
// here.  I think I was messing with the OSX bundle release,
// but whatever I was doing, it sems not to be needed.  I'll
// just leave it here for a while, though.
// DEK 2005-12-18
//
char _gri_number[] = PACKAGE_VERSION;
#endif
char _gri_release_time[] = "2007-07-20";
char _gri_date[] = "2007";
