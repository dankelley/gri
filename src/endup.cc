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

#include <string>
#include "gr.hh"
#include "extern.hh"
#include "private.hh"
#include "superus.hh"


void
end_up()
{
#if 0				// inaccurate anyhow!
	if (_chatty > 0) {
		display_unused_var();
		display_unused_syn();
	}
#endif
	gr_set_clip_ps_off();
	close_data_files();
	gr_end("!");
}
