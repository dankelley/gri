#include <math.h>
#include <stdio.h>
#include <string.h>
#include "extern.hh"
#include "private.hh"
#include "gr.hh"
#include "types.hh"
#include "files.hh"
#include "DataFile.hh"


// Push a new cmd file onto the stack.  Return NO if cannot load the file.
bool
push_cmd_file(const char * fname, bool interactive, bool allow_warning, const char * status)
{
    if (!fname)
	gr_Error("empty command-file name");
    CmdFile cf;
    FILE *the_fp;

    // Open file, unless it's "stdin", which is already open
    if (!strcmp(fname, "stdin")) {
	the_fp = stdin;
    } else {
	FILE           *thefile = fopen(fname, status);
	if (thefile) {		// Can open file
	    the_fp = thefile;
	} else {		// Cannot open file
	    if (allow_warning) {
		warning("Can't open command-file `\\", fname, "'", "\\");
	    }
	    return false;
	}
    }
    cf.set(fname, the_fp, interactive, 0);
    _cmdFILE.push_back(cf);
    return true;
}

// Push a new data file onto the stack.
bool
push_data_file(const char * name, DataFile::type the_type, const char * status, bool delete_when_close)
{
    if (the_type == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
	int file_id;
	ncopts = NC_VERBOSE;	// Set external flag to live if error
	file_id = ncopen(name, NC_NOWRITE);
	if (file_id == -1)
	    return false;	// failed
	DataFile df;
	df.set_name(name);
	df.set_netCDF_id(file_id);
	df.set_type(the_type);
	df.set_delete_when_close(delete_when_close);
	_dataFILE.push_back(df);
#else
	// This may be redundant; see openCmd
	err("`open ... netCDF' impossible since Gri not compiled with netCDF library");
	return false;
#endif
    } else {
	FILE *tmp;
#if defined(MSDOS)
	// For MSDOS binary files, status must be of the form "rb"
	char status2[10];
	strcpy(status2, status);
	if (the_type != DataFile::ascii) {
	    // binary file
	    strcat(status2, "b");
	}
	tmp = fopen(name, status2);
#else
	tmp = fopen(name, status);
#endif
	if (NULL == tmp)
	    return false;
	DataFile df;
	df.set_name(name);
	df.set_fp(tmp);
	df.set_type(the_type);
	df.set_delete_when_close(delete_when_close);
	_dataFILE.push_back(df);
    }
    update_readfrom_file_name();
    return true;
}

int
data_file_index(const char * name)
{
    int             i, len_request;
    char *             fullname;
    fullname = complete_filename(name);
    len_request = strlen(fullname);
    int n = _dataFILE.size();
    for (i = 0; i < n; i++) {
	int             len = strlen(_dataFILE[i].get_name());
	if (len < len_request)
	    continue;
	if (!strcmp(_dataFILE[i].get_name() + len - len_request, fullname))
	    return i;
    }
    return -1;
}

// Reorder data-file stack so named file is ready for reading
bool
push_data_file_to_top(const char * filename)
{
    int             i;
    if (-1 == (i = data_file_index(filename)))
	return false;
    DataFile n;
    n = _dataFILE.back();
    _dataFILE.back() = _dataFILE[i];
    _dataFILE[i] = n;
    return true;
}

// Pop a given file out of the data-file stack.
bool
pop_data_file(int file)
{
    extern char     _grTempString[];
    unsigned int    i;
    if (file < 0) {
	err("No such data file exists");
	return false;
    }
    if (_dataFILE[file].get_type() == DataFile::from_cmdfile) {
	err("Internal gri error [file.c][pop_data_file]: No data file open");
	return false;
    }
    if (file > int(_dataFILE.size())) {
	err("Internal gri error [file.cc/pop_data_file()]: Data stack overflow");
	return false;
    }
    if (_dataFILE[file].get_type() == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
	int success = ncclose(_dataFILE[file].get_netCDF_id());
	if (success == -1) {
	    err("Internal gri error: cannot close netCDF file `\\",
		_dataFILE[file].get_name(), "'", "\\");
	    return false;
	}
#else
	err("Gri internal error: attempting to use nonexistent netCDF library");
	return false;
#endif
    } else {
	fclose(_dataFILE[file].get_fp());
    }
    if (_dataFILE[file].get_delete_when_close()) {
	char            sys_cmd[200];
	if (_chatty > 1) {
	    sprintf(_grTempString, "Deleting temporary file named %s\n",
		    _dataFILE[file].get_name());
	    ShowStr(_grTempString);
	}
#if defined(VMS)
	sprintf(sys_cmd, "DEL %s;*", _dataFILE[file].get_name());
	system(sys_cmd);
#else
	sprintf(sys_cmd, "rm -f %s", _dataFILE[file].get_name());
	system(sys_cmd);
#endif
    }
    for (i = file; i < _dataFILE.size() - 1; i++)
	_dataFILE[file] = _dataFILE[file + 1];
    _dataFILE.pop_back();
    return true;
}

void
display_data_stack(const char * s)
{
    printf("%s", s);
    unsigned int i, n = _dataFILE.size();
    if (n == 0)
	printf("Data file stack is empty\n");
    else {
	printf("Data file stack is as follows:\n");
	for (i = 0; i < n; i++)
	    printf("   file [%s] type=%d delete_when_close=%d\n", _dataFILE[i].get_name(), int(_dataFILE[i].get_type()),int(_dataFILE[i].get_delete_when_close()));
    }
}

void
display_cmd_stack(const char * s)
{
    unsigned int i, n = _cmdFILE.size();
    printf("%s Command file stack is as follows:\n", s);
    for (i = 0; i < n; i++)
	printf("%s   file [%s]\n", s, _cmdFILE[i].get_name());
}
void
close_data_files()
{
    int i, n = _dataFILE.size();
    for (i = n - 1; i >= 0; i--)
	if (_dataFILE[i].get_type() != DataFile::from_cmdfile)
	    pop_data_file(i);
}
