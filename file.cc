#include <math.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "extern.hh"
#include "private.hh"
#include "gr.hh"
#include "types.hh"
#include "files.hh"
#include "DataFile.hh"
#include "superus.hh"


// Push a new cmd file onto the stack.  Return NO if cannot load the file.
bool
push_cmd_file(const char* fname, bool interactive, bool allow_warning, const char* status)
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

static bool
is_compressed_file(string& fname)
{
	if (fname.size() < 3)
		return false;
	string last_three(fname, fname.size()-3, fname.size()-1);
	if (last_three == ".gz")
		return true;
	return false;
}

// Push a new data file onto the stack.
// The 'delete_when_close' is just a suggestion (true for output
// from system commands).  Even if it is false, if we detect
// here that a system command must be called, we'll still
// remove the file when done.
bool
push_data_file(const char* name, DataFile::type the_type, const char* status, bool delete_when_close)
{
	if (((unsigned) superuser()) & FLAG_AUT1)printf("\n  DEBUG %s:%d push_data_file(%s,...) ...\n",__FILE__,__LINE__,name);
	if (the_type == DataFile::bin_netcdf) {
#if !defined(HAVE_LIBNETCDF)
		// This may be redundant; see openCmd
		err("`open ... netCDF' impossible since Gri not compiled with netCDF library");
		return false;
#else
		ncopts = NC_VERBOSE;	// Set external flag to live if error
		int file_id = ncopen(name, NC_NOWRITE);
		if (file_id == -1)
			return false;
		DataFile df((FILE*)NULL, name, file_id, the_type, delete_when_close);
		_dataFILE.push_back(df);
#endif
	} else {
#if defined(MSDOS)
		// For MSDOS binary files, status must be of the form "rb"
		char status2[10];
		strcpy(status2, status);
		if (the_type != DataFile::ascii)
			strcat(status2, "b");
		FILE *fp = fopen(name, status2);
		if (NULL == fp)
			return false;
		DataFile df(fp, name, 0, the_type, delete_when_close);
		_dataFILE.push_back(df);
#else
		string sname(name);
		if (is_compressed_file(sname)) {
			string pipecmd("zcat ");
			pipecmd.append(sname);
			pipecmd.append(" > ");
			string tmpfile_name(tmp_file_name());
			pipecmd.append(tmpfile_name);
			if (((unsigned) superuser()) & FLAG_SYS) {
				ShowStr("\n`open' sending the following command to the operating system:\n");
				ShowStr(pipecmd.c_str());
				ShowStr("\n");
			}
			system(pipecmd.c_str());
			FILE *fp = fopen(tmpfile_name.c_str(), status);
			if (NULL == fp) {
				err("Cannot open file `\\", tmpfile_name.c_str(), "'.", "\\");
				return false;
			}
			DataFile df(fp, tmpfile_name.c_str(), 0, the_type, true);
			_dataFILE.push_back(df);
		} else {
			FILE *fp = fopen(name, status);
			if (NULL != fp) {
				DataFile df(fp, name, 0, the_type, delete_when_close);
				_dataFILE.push_back(df);
			} else {
				//printf("DEBUG: %s:%d ... error is '%s'\n", __FILE__, __LINE__, strerror(errno));
				if (errno == EMFILE) { // ref: 'man errno'
					err("Cannot open file `\\", name, "' since there are too many open files.", "\\");
					return false;
				}
#if !defined(HAVE_ACCESS)
				return false; // just give up then
#else
				string sname(name);
				sname.append(".gz");
				if (0 != access(sname.c_str(), R_OK)) {
					warning("Cannot locate file `\\", name, "'",
						" or a compressed version `", sname.c_str(), "'",
						"\\");
					
					return false; // failure
				}
				warning("`open' can't find `\\", name, "' so using `",
					sname.c_str(), "' instead.", "\\");
				string pipecmd("zcat ");
				pipecmd.append(sname);
				string tmpfile_name(tmp_file_name());
				pipecmd.append(" > ");
				pipecmd.append(tmpfile_name);
				if (((unsigned) superuser()) & FLAG_SYS) {
					ShowStr("`open' sending the following command to the operating system:\n");
					ShowStr(pipecmd.c_str());
					ShowStr("\n");
				}
				system(pipecmd.c_str());
				fp = fopen(tmpfile_name.c_str(), status);
				if (NULL == fp) {
					//printf("%s:%d 2222  Cannot open.  err is '%s'\n", __FILE__, __LINE__, strerror(errno));
					return false;
				}
				DataFile df(fp, tmpfile_name.c_str(), 0, the_type, true);
				_dataFILE.push_back(df);
#endif
			}
		}
	}
#endif
	update_readfrom_file_name();
	return true;
}

int
data_file_index(const char* name)
{
	string completefilename(name);
	resolve_filename(completefilename, true);
	for (unsigned int i = 0; i < _dataFILE.size(); i++)
		if (_dataFILE[i].get_name() == completefilename)
			return i;
	return -1;
}

// Reorder data-file stack so named file is ready for reading
bool
push_data_file_to_top(const char* filename)
{
	int i = data_file_index(filename);
	if (((unsigned) superuser()) & FLAG_AUT1)printf("\n  DEBUG: %s:%d push_data_file_to_top(%s).  This is the %d file   stack_len= %d\n",__FILE__,__LINE__,filename, i, _dataFILE.size());
	if (i == -1)
		return false;
	DataFile n(_dataFILE.back());
	_dataFILE.back() = _dataFILE[i];
	_dataFILE[i] = n;
	return true;
}

// Remove a file from the data-file stack.
bool
pop_data_file(int file)
{
	if (((unsigned) superuser()) & FLAG_AUT1)printf("\n  DEBUG: %s:%d pop_data_file(file= %3d) fp= %x   stack_len= %d\n",__FILE__,__LINE__,file, int(_dataFILE[file].get_fp()), _dataFILE.size());
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
		if (_dataFILE[file].get_fp() == stdin) {
			fatal_err("pop_data_file() is trying to close stdin");
			exit(1);
		} else {
			if (EOF == fclose(_dataFILE[file].get_fp())) {
				fatal_err("pop_data_file() cannot close a data file reason \"\\", strerror(errno), "\".", "\\");
				exit(1);
			}
		}
	}
	if (_dataFILE[file].get_delete_when_close()) {
		if (_chatty > 1) {
			string msg("Deleting temporary file named `");
			msg.append(_dataFILE[file].get_name());
			msg.append("'");
			ShowStr(msg.c_str());
		}
		string sys_cmd("rm -f ");
		sys_cmd.append(_dataFILE[file].get_name());
		if (((unsigned) superuser()) & FLAG_SYS) {
			ShowStr("\n`system' sending the following command to the operating system:\n");
			ShowStr(sys_cmd.c_str());
			ShowStr("\n");
		}
		system(sys_cmd.c_str());
	}
	//display_data_stack("BEFORE the erasure of a file\n");
	_dataFILE.erase(_dataFILE.begin() + file);
	//display_data_stack("AFTER the erasure\n");
	return true;
}

void
display_data_stack(const char* s)
{
	printf("%s", s);
	unsigned int n = _dataFILE.size();
	if (n == 0)
		printf("Data file stack is empty\n");
	else {
		printf("Data file stack is as follows:\n");
		for (unsigned int i = 0; i < n; i++)
			printf("   file name= '%s' type= %d delete_when_close= %d  fp= %x\n", _dataFILE[i].get_name(), int(_dataFILE[i].get_type()),int(_dataFILE[i].get_delete_when_close()), int(_dataFILE[i].get_fp()));
	}
}

void
display_cmd_stack(const char* s)
{
	unsigned int i, n = _cmdFILE.size();
	printf("%s Command file stack is as follows:\n", s);
	for (i = 0; i < n; i++)
		printf("%s   file [%s]\n", s, _cmdFILE[i].get_name());
}
void
close_data_files()
{
	int n = _dataFILE.size();
	if (((unsigned) superuser()) & FLAG_AUT1)printf("\n  DEBUG: %s:%d in close_data_files() about to dispose %d files\n",__FILE__,__LINE__,n);
	for (int i = n - 1; i >= 0; i--) {
		if (_dataFILE[i].get_type() != DataFile::from_cmdfile) {
			pop_data_file(i);
		}
	}
}
