#include <string>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
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
			open_file(DataFile::bin_uchar);
		} else if (!strcmp(_word[2], "uchar")) {
			open_file(DataFile::bin_uchar);
		} else if (!strcmp(_word[2], "8bit")) {
			open_file(DataFile::bin_uchar);
		} else if (!strcmp(_word[2], "16bit")) {
			open_file(DataFile::bin_16bit);
		} else if (!strcmp(_word[2], "int")) {
			open_file(DataFile::bin_int);
		} else if (!strcmp(_word[2], "float")) {
			open_file(DataFile::bin_float);
		} else if (!strcmp(_word[2], "double")) {
			open_file(DataFile::bin_double);
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
	case 4:
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
		// Now check for a pipe
		for (int i = len - 2; i > -1; i--) {
			if (*(_word[1] + i) == '|') {
				// It's a pipe
				string pipecmd;
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
				string tmpfile_name(tmp_file_name());
				pipecmd.append(tmpfile_name.c_str());
				if (((unsigned) superuser()) & FLAG_SYS) {
					ShowStr("\n`open' sending the following command to the operating system:\n");
					ShowStr(pipecmd.c_str());
					ShowStr("\n");
				}
				system(pipecmd.c_str());
				if (!push_data_file(tmpfile_name.c_str(), type, "r", true)) {
					err("`open' can't find file `\\", tmpfile_name.c_str(), "'", "\\");
					return false;
				}
				return false;
			} else if (!isspace(*(_word[1] + i))) {
				// Not a pipe.  Remove the quotes from filename and open
				string filename;
				filename.assign(1 + _word[1]);
				filename[len - 2] = '\0';
				if (filename.size() < 1) {
					err("`open' needs a filename; \"\" won't do!");
					return false;
				}
				string completefilename(filename);
				resolve_filename(completefilename, true);
				if (!push_data_file(completefilename.c_str(), type, "r", false)) {
					err("`open' can't find file `\\", completefilename.c_str(), "'", "\\");
					return false;
				}
			}
		}
	} else {
		// Normal filename
		if (!strlen(_word[1])) {
			err("`open' needs a filename");
			return false;
		}
		string completefilename(_word[1]);
		resolve_filename(completefilename, true);
		if (!push_data_file(completefilename.c_str(), type, "r", false)) {
			err("`open' can't find file `\\", completefilename.c_str(), "'", "\\");
			return false;
		}
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
		system(command);
		break;
	case 2:
		sprintf(command, "ls -CF %s/%s", _current_directory.c_str(), _word[1]);
		system(command);
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
#endif
}
