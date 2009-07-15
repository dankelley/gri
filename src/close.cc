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

#include <stdio.h>
#include <string.h>
#include "extern.hh"
#include "private.hh"
#include "DataFile.hh"

bool
closeCmd()
{
	if (((unsigned) superuser()) & FLAG_AUT1)printf("\nDEBUG: %s:%d closing a datafile.   Before doing that, datafile stack_len= %d\n",__FILE__,__LINE__,int(_dataFILE.size()));


	if ((_dataFILE.back()).get_type() == DataFile::from_cmdfile) {
		err("`close' ignored: no data file open");
		return false;
	}
	int file = _dataFILE.size() - 1;
	std::string fname;
	switch (_nword) {
	case 1:
		break;
	case 2:
		fname.assign(_word[1]);
		un_double_quote(fname);
		file = data_file_index(fname.c_str());
		if (file < 0) {
			extern char     _grTempString[];
			sprintf(_grTempString, "`close' cannot close `%s' since it is not open", _word[1]);
			err(_grTempString);
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	pop_data_file(file);
	clear_eof_flag_on_data_file();
	return true;
}
