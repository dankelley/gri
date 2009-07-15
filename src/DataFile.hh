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

// Object for data files
#if !defined(_DATAFILE_HH_)
#define _DATAFILE_HH_
#include <string>
#include <vector>		// part of STL
#include <stdio.h>

#include "types.hh"
#include "macro.hh"
#include "CmdFile.hh"

class DataFile
{
public:
	enum type {from_cmdfile, ascii, bin_unknown, bin_uchar, bin_16bit, bin_int, bin_float, bin_double, bin_netcdf};
	DataFile() {
		extern std::vector<CmdFile>  _cmdFILE;
		name.assign("stdin");
		if (_cmdFILE.size() > 0) {
			fp = _cmdFILE.end()->get_fp();
 			if (superuser() & FLAG_AUT1)printf("    DEBUG: %s:%d construct from cmdFILE  fp= %lx  ",__FILE__,__LINE__,(long unsigned int)(fp));
		} else {
 			if (superuser() & FLAG_AUT1)printf("    DEBUG: %s:%d construct from stdin    fp= %lx  ",__FILE__,__LINE__,(long unsigned int)(stdin));
			fp = stdin;
		}
		if (fp == (FILE*)NULL) {printf("ERROR.  Null fp [blank-constructor phase] %s:%d\n",__FILE__, __LINE__);}
		the_type = from_cmdfile;
		line = 1;
		delete_when_close = false;
		if (superuser() & FLAG_AUT1)printf(" ... %s:%d  name= '%s'\t fp= %lx  this= %lx\n",__FILE__,__LINE__,name.c_str(),long(fp),long(this));
	}
	DataFile(const DataFile& d) {
		name.assign(d.get_name());
		fp = d.get_fp();
		//if (fp == (FILE*)NULL) {printf("ERROR.  Null fp [copy-constructor phase] %s:%d\n",__FILE__, __LINE__);}
		netCDF_id = d.get_netCDF_id();
		the_type = d.get_type();
		line = d.get_line();
		delete_when_close = d.get_delete_when_close();
		if (superuser() & FLAG_AUT1)printf("    DEBUG: %s:%d DataFile(%lx)    name= '%s'\t fp= %lx   this= %lx\n",__FILE__,__LINE__,long(&d),name.c_str(),long(fp),long(this));
	}
	DataFile(FILE* a_fp, const char* a_name, int a_netCDF_id, type a_the_type, bool a_delete_when_close) {
		fp = a_fp;
		name.assign(a_name);
		netCDF_id = a_netCDF_id;
		the_type = a_the_type;
		line = 0;
		delete_when_close = a_delete_when_close;
		if (superuser() & FLAG_AUT1)printf("    DEBUG: %s:%d DataFile(fp= %lx, name= '%s', ...) this= %lx\n",__FILE__,__LINE__,long(a_fp),a_name,long(this));
	}
	~DataFile() {
		if (superuser() & FLAG_AUT1)printf("    DEBUG: %s:%d DataFile::~DataFile() name= '%s'\t fp= %lx   this= %lx\n",__FILE__,__LINE__, name.c_str(), long(fp), long(this));
#if 0				// BUG 2001-feb-17 -- not sure on next 2 lines
		name.string::~string(); // not executed
#endif
	}
	DataFile& operator=(const DataFile& d) {
		name.assign(d.get_name());
		fp = d.get_fp();
		if (fp == (FILE*)NULL) {printf("ERROR.  Null fp [operator= phase] %s:%d\n",__FILE__, __LINE__);}
		netCDF_id = d.get_netCDF_id();
		the_type = d.get_type();
		line = d.get_line();
		delete_when_close = d.get_delete_when_close();
		if (superuser() & FLAG_AUT1)printf("    DEBUG: %s:%d DataFile::operator=   name= '%s'\t fp=  %lx  this= %lx\n",__FILE__,__LINE__, name.c_str(), long(fp), long(this));
		return *this;
	}
	void set_line(int new_line)         {line = new_line > 1 ? new_line : 1;}
	void increment_line()               {line++;			}
	const char *get_name()       const  {return name.c_str();	}
	FILE* get_fp()               const  {return fp;			}
	int  get_netCDF_id()         const  {return netCDF_id;		}
	type get_type()              const  {return the_type;		}
	int get_line()               const  {return line;		}
	bool get_delete_when_close() const  {return delete_when_close;	}
private:
	FILE* fp;		// file pointer
	std::string name;	// name of file, or stdin
	int  netCDF_id;		// only used if FILE_BIN_NETCDF
	type the_type;		// 
	int  line;		// current line number
	bool delete_when_close;	// for open "...|"
};
#endif // _DATAFILE_HH_
