#include <math.h>
#include <stdio.h>
#include <string.h>
#include "extern.hh"
#include "private.hh"
#include "gr.hh"
#include "types.hh"
#include "files.hh"
#include "DataFile.hh"
#include "superus.hh"


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
	if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG %s:%d push_data_file(%s,...) ...\n",__FILE__,__LINE__,name);
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
		if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d created a datafile at %x\n",__FILE__,__LINE__,int(&df));
#if 0
		df.set_name(name);
		df.set_netCDF_id(file_id);
		df.set_type(the_type);
		df.set_delete_when_close(delete_when_close);
#endif
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
#if 0
		df.set_fp(fp);
		df.set_name(name);
		df.set_type(the_type);
		df.set_delete_when_close(delete_when_close);
#endif
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
				printf("ERROR: gri-%s cannot open file '%s' (source at %s:%d)\n", VERSION,tmpfile_name.c_str(),__FILE__,__LINE__);
				return false;
			}
			if (((unsigned) superuser()) & FLAG_AUT1)printf("FILE-OPEN: '%s' at fp %x\n", tmpfile_name.c_str(), int(fp));
			DataFile df(fp, tmpfile_name.c_str(), 0, the_type, true);
			if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d created a datafile at %x\n",__FILE__,__LINE__,int(&df));
			_dataFILE.push_back(df);
		} else {
			FILE *fp = fopen(name, status);
			if (NULL != fp) {
				if (((unsigned) superuser()) & FLAG_AUT1)printf("FILE-OPEN: '%s' at fp %x\n", name, int(fp));
				DataFile df(fp, name, 0, the_type, delete_when_close);
				if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG %s:%d push_data_file(%s,...) about to do _dataFILE.push_back(fp=%x)\n",__FILE__,__LINE__,name,int(fp));
				_dataFILE.push_back(df);

			} else {
#if !defined(HAVE_ACCESS)
				return false; // just give up then
#else
				string sname(name);
				sname.append(".gz");
				if (0 != access(sname.c_str(), R_OK)) {
					warning("Cannot locate file `", name, "'",
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
				if (NULL == fp)
					return false;
				DataFile df(fp, tmpfile_name.c_str(), 0, the_type, true);
				if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d created a datafile at %x\n",__FILE__,__LINE__,int(&df));
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
data_file_index(const char * name)
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
push_data_file_to_top(const char * filename)
{
	int i = data_file_index(filename);
	if (i == -1)
		return false;
	DataFile n(_dataFILE.back());
	if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d push_data_file_to_top(%s) created a datafile at %x\n",__FILE__,__LINE__,filename,int(&n));
	_dataFILE.back() = _dataFILE[i];
	_dataFILE[i] = n;
	return true;
}

// Remove a file from the data-file stack.
bool
pop_data_file(int file)
{
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
			if (((unsigned) superuser()) & FLAG_AUT1)printf("********** %s:%d pop_data_file(%d) trying to close stdin\n",__FILE__,__LINE__,file);
			exit(1);
		} else {
			if (((unsigned) superuser()) & FLAG_AUT1)printf("FILE-CLOSE %s:%d about to close dataFILE[%d] which has fp at %x ...\n",__FILE__,__LINE__,file,int(_dataFILE[file].get_fp()));
			fclose(_dataFILE[file].get_fp());
			if (((unsigned) superuser()) & FLAG_AUT1)printf("\t     ... closed it ok.\n");
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
	_dataFILE.erase(_dataFILE.begin() + file);
	if (((unsigned) superuser()) & FLAG_AUT1)printf("pop_data_file(%d) about to do _dataFILE.pop_back() size before is %d ...\n",file,_dataFILE.size());
	if (((unsigned) superuser()) & FLAG_AUT1)printf("        ... did pop_data_file(%d) ok ... size now %d\n",file,_dataFILE.size());
	return true;
}

void
display_data_stack(const char * s)
{
	printf("%s", s);
	unsigned int n = _dataFILE.size();
	if (n == 0)
		printf("Data file stack is empty\n");
	else {
		printf("Data file stack is as follows:\n");
		for (unsigned int i = 0; i < n; i++)
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
	int n = _dataFILE.size();
	if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d in close_data_files().  n=%d\n",__FILE__,__LINE__,n);
	for (int i = n - 1; i >= 0; i--) {
		if (((unsigned) superuser()) & FLAG_AUT1)printf("\tDEBUG: %s:%d in close_data_files().  i=%d\n",__FILE__,__LINE__,i);
		if (_dataFILE[i].get_type() != DataFile::from_cmdfile) {
			if (((unsigned) superuser()) & FLAG_AUT1)printf("\t\tDEBUG: %s:%d in close_data_files().  i=%d (ok to pop this one)\n",__FILE__,__LINE__,i);
			pop_data_file(i);
		} else {
			if (((unsigned) superuser()) & FLAG_AUT1)printf("\t\tDEBUG: %s:%d in close_data_files().  i=%d (IT IS **NOT** ok to pop this one)\n",__FILE__,__LINE__,i);
		}
	}
	if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG-------- leaving close_data_files()\n");
}
