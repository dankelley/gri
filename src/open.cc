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
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include "extern.hh"
#include "files.hh"
#include "superus.hh"

extern char    *egetenv(const char *item);
bool            lsCmd(void);
bool            open_file(DataFile::type type);
bool            openCmd(void);

bool
openCmd()
{
	if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d opening file named '%s'.  Before doing that, datafile stack_len= %d\n",__FILE__,__LINE__, _word[1], int(_dataFILE.size()));
	switch (_nword) {
	case 2:
		open_file(DataFile::ascii);
		break;
	case 3:
		if (!strcmp(_word[2], "netCDF") || !strcmp(_word[2], "netcdf")) {
#if defined(HAVE_LIBNETCDF)
			open_file(DataFile::bin_netcdf);
#else
			err("`open ... netCDF' impossible since Gri not compiled with netCDF library");
			return false;
#endif
		} else if (!strcmp(_word[2], "binary")) {
			open_file(DataFile::bin_unknown);
		} else {
			err("\
`open filename' can't understand final word `\\",
			    _word[_nword - 1],
			    "'.",
			    "\\"
				);
			return false;
		}
		break;
	case 4:			// open filename binary type
		if (strcmp(_word[2], "binary")) {
			err("\
Third word of `open filename' must be \"binary\", not `\\",
			    _word[2],
			    "' as given.",
			    "\\"
				);
			return false;
		}
		if (!strcmp(_word[3], "uchar"))
			open_file(DataFile::bin_uchar);
		else if (!strcmp(_word[3], "16bit"))
			open_file(DataFile::bin_16bit);
		else if (!strcmp(_word[3], "int"))
			open_file(DataFile::bin_int);
		else if (!strcmp(_word[3], "float"))
			open_file(DataFile::bin_float);
		else if (!strcmp(_word[3], "double"))
			open_file(DataFile::bin_double);
		else {
			err("\
`open filename' can't understand final word `\\",
			    _word[_nword - 1],
			    "'.",
			    "\\"
				);
			return false;
		}
		break;
	default:
		err("`open' needs a file name");
		return false;
	}
	clear_eof_flag_on_data_file();
	return true;
}

bool
open_file(DataFile::type type)
{
	put_syn("\\.return_value.", "", true);
	// Must decode filename, which may have "/" in it which got expanded to "
	// / " by expand_blanks().
	// If filename is quoted, either read from a pipe (if last nonblank
	// character is '|') or just ignore the quotes
	if (*_word[1] == '"') {
		// Quoted filename.  First check to see endquote exists.
		remove_esc_quotes(_word[1]);
		int len = strlen(_word[1]);
		if (len < 2) {
			err("`open' needs a proper filename");
			return false;
		}
		if (*(_word[1] + len - 1) != '"') {
			err("Missing quote on end of file/pipe name.");
			return false;
		}
		// Check for URL
		if (!strncmp(_word[1], "\"http://", 8)) {
			std::string cmd("wget ");
			cmd.append(_word[1] + 1);
			cmd.STRINGERASE(cmd.size() - 1);
			cmd.append(" --output-document=");
			std::string tmpfile_name(tmp_file_name());
			cmd.append(tmpfile_name);
			cmd.append(" --output-file=/dev/null");
			//printf("WILL RUN <%s>\n", cmd.c_str());
			call_the_OS(cmd.c_str(), __FILE__, __LINE__);
			if (!push_data_file(tmpfile_name.c_str(), type, "r", true)) {
				err("`open' can't find file `\\", tmpfile_name.c_str(), "'", "\\");
				return false;
			}
			return true;
		}
		// Now check for a pipe
		for (int i = len - 2; i > -1; i--) {
			if (*(_word[1] + i) == '|') {
				// It's a pipe
				std::string pipecmd;
				if (type == DataFile::bin_netcdf) {
					err("`open' cannot have a pipe that is netCDF type");
					return false;
				}
				// Copy string for system command, and then
				// erase regular expression \s*|\s*"$
				pipecmd.assign(_word[1] + 1);// skip the initial quote
				if (pipecmd[pipecmd.size() - 1] == '"')
					pipecmd.STRINGERASE(pipecmd.size() - 1);
				while (isspace(pipecmd[pipecmd.size() - 1]))
					pipecmd.STRINGERASE(pipecmd.size() - 1);
				if (pipecmd[pipecmd.size() - 1] == '|')
					pipecmd.STRINGERASE(pipecmd.size() - 1);
				while (isspace(pipecmd[pipecmd.size() - 1]))
					pipecmd.STRINGERASE(pipecmd.size() - 1);
				pipecmd.append(" > ");
				std::string tmpfile_name(tmp_file_name());
				pipecmd.append(tmpfile_name.c_str());
				call_the_OS(pipecmd.c_str(), __FILE__, __LINE__);
				if (!push_data_file(tmpfile_name.c_str(), type, "r", true)) {
					err("`open' can't find file `\\", tmpfile_name.c_str(), "'", "\\");
					return false;
				}
				break;
			} else if (!isspace(*(_word[1] + i))) {
				// Presume quoted string ... but check to be sure!
				std::string filename;
				if (*_word[1] == '"' && *_word[1] != '\0')
					filename.assign(1 + _word[1]);
				else
					filename.assign(_word[1]);
				if (filename.size() < 1) {
					err("`open' needs a filename; \"\" won't do!");
					return false;
				}
				if (filename[filename.size() - 1] == '"')
					filename.STRINGERASE(filename.size() - 1, filename.size());
				// Determine actual filename (substituting for ~ etc).
				std::string completefilename(filename);
				resolve_filename(completefilename, true, 'd');
				if (!push_data_file(completefilename.c_str(), type, "r", false)) {
					err("`open' can't find file `\\", completefilename.c_str(), "'", "\\");
					return false;
				}
				break;
			}
		}
	} else {
		// Normal filename
		if (!strlen(_word[1])) {
			err("`open' needs a filename");
			return false;
		}
		std::string completefilename(_word[1]);
		resolve_filename(completefilename, true, 'd');
		if (!push_data_file(completefilename.c_str(), type, "r", false)) {
			err("`open' can't find (or successfully open) file `\\", completefilename.c_str(), "' due to system error `", strerror(errno), "'.", "\\");
			return false;
		}
		put_syn("\\.return_value.", completefilename.c_str(), true);
	}
	return true;
}

bool
cdCmd()
{
#if defined(HAVE_GETENV)
	char *            home = egetenv("HOME");
	switch (_nword) {
	case 1:
		// `cd' means move to home directory
		_current_directory.assign(home);
		break;
	case 2:
		if (!strcmp(_word[1], ".")) {
			// "."
			_current_directory.assign(".");
		} else if (!strncmp(_word[1], "..", 2)) {
			// "../LOCATION"
			_current_directory.append("/");
			_current_directory.append(_word[1]);
		} else if (!strcmp(_word[1], "~")) {
			// "~"
			_current_directory.assign(home);
		} else if (!strcmp(_word[1], "$HOME")) {
			// "$HOME"
			_current_directory.assign(home);
		} else if (*_word[1] == '~') {
			// "~/LOCATION"
			char tmp[1000];
			sprintf(tmp, "%s%s", home, _word[1] + 1);
			_current_directory.assign(tmp);
		} else if (!strncmp(_word[1], "$HOME", 5)) {
			// "$HOME/LOCATION"
			char tmp[1000];
			sprintf(tmp, "%s%s", home, _word[1] + 5);
			_current_directory.assign(tmp);
		} else if (!strncmp(_word[1], "/", 1)) { 
			// "/LOCATION"
			_current_directory.assign(_word[1]);
		} else {
			// "LOCATION"
			_current_directory.append("/");
			_current_directory.append(_word[1]);
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
#else
	err("`cd' requires \"environment variables\", which this computer lacks.");
	return false;
#endif
}

bool
pwdCmd()
{
	// `cd' means move to home directory
	gr_textput(_current_directory.c_str());
	gr_textput("\n");
	return true;
}

bool
lsCmd()
{
#if defined(VMS)
	warning("`ls' not implemented in VMS version.");
	return true;
#else
	// `cd' means move to home directory
	char            command[100];
	switch (_nword) {
	case 1:
		sprintf(command, "ls -CF %s/", _current_directory.c_str());
		call_the_OS(command, __FILE__, __LINE__);
		break;
	case 2:
		sprintf(command, "ls -CF %s/%s", _current_directory.c_str(), _word[1]);
		call_the_OS(command, __FILE__, __LINE__);
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
#endif
}
