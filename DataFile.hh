// Object for data files
#if !defined(_DataFile_hh_)
#define _DataFile_hh_
#include <string>
#include <vector>		// part of STL
#include <stdio.h>

#include "types.hh"
#include "macro.hh"
#include "CmdFile.hh"

class DataFile
{
public:
	enum type {from_cmdfile, ascii, bin_uchar, bin_16bit, bin_int, bin_float, bin_double, bin_netcdf};
	DataFile() {
		extern vector<CmdFile>  _cmdFILE;
		name.assign("stdin");
		if (_cmdFILE.size() > 0) {
			fp = _cmdFILE.end()->get_fp();
 			if (superuser() & FLAG_AUT1)printf("\n\n***DataFile() %s:%d got fp from cmdFILE.  it is %x\n", __FILE__,__LINE__,int(fp));
		} else {
 			if (superuser() & FLAG_AUT1)printf("\n\n***DataFile() %s:%d setting fp to %x since cmdFILE is empty  Hm.************\n",__FILE__,__LINE__,int(stdin));
			fp = stdin;
		}
		if (!fp) {printf("DataFile() %s:%d: INVALID fp=0****\n",__FILE__,__LINE__);exit(1);}
		the_type = from_cmdfile;
		line = 0;
		delete_when_close = false;
		if (superuser() & FLAG_AUT1)printf("DEBUG: %s:%d DataFile::DataFile() named '%s' has fp at %x.  this %x\n",__FILE__,__LINE__, name.c_str(), int(fp),int(this));
	}
	DataFile(const DataFile& d) {
		name.assign(d.get_name());
		fp = d.get_fp();
		if (fp == (FILE*)NULL) {
			if (superuser() & FLAG_AUT1)printf("%s:%d somehow DataFile copy null  '%s'  from %x    this %x\n",__FILE__,__LINE__,name.c_str(),int(&d),int(this));
			exit(1);
		}
		netCDF_id = d.get_netCDF_id();
		the_type = d.get_type();
		line = d.get_line();
		delete_when_close = d.get_delete_when_close();
		if (superuser() & FLAG_AUT1)printf("DEBUG: %s:%d COPYCONSTRUCT DataFile(%x) '%s'   fp %x   this %x\n",__FILE__,__LINE__,int(&d),name.c_str(),int(fp),int(this));
	}
	DataFile(FILE* a_fp, const char* a_name, int a_netCDF_id, type a_the_type, bool a_delete_when_close) {
		fp = a_fp;
		name.assign(a_name);
		netCDF_id = a_netCDF_id;
		the_type = a_the_type;
		line = 0;
		delete_when_close = a_delete_when_close;
	}
	~DataFile() {
		if (superuser() & FLAG_AUT1)printf("DEBUG: %s:%d DataFile::~DataFile()    '%s'    fp %x    this %x\n",__FILE__,__LINE__, name.c_str(), int(fp), int(this));
		name.string::~string();
	}
	DataFile& operator=(const DataFile& d) {
		name.assign(d.get_name());
		fp = d.get_fp();
		if (fp == (FILE*)NULL) {
			printf("ERROR at %s:%d -- try to DataFile operator= null file this %x\n",__FILE__,__LINE__,int(this));
			exit(1);
		}
		netCDF_id = d.get_netCDF_id();
		the_type = d.get_type();
		line = d.get_line();
		delete_when_close = d.get_delete_when_close();
		if (superuser() & FLAG_AUT1)printf("DEBUG: %s:%d DataFile::operator= for file '%s' has fp at %x\n",__FILE__,__LINE__, name.c_str(), int(fp));
		return *this;
	}
	void set_line(int new_line)         {line = new_line > 1 ? new_line : 1;}
	void increment_line()               {line++;}

	const char *get_name()       const  {return name.c_str();}
	FILE* get_fp()               const  { return fp; }
	int  get_netCDF_id()         const  {return netCDF_id;}
	type get_type()              const  {return the_type;}
	int get_line()               const  {return line;}
	bool get_delete_when_close() const  {return delete_when_close;}
private:
	FILE* fp;			// file pointer
	string name;			// name of file, or stdin
	int  netCDF_id;		// only used if FILE_BIN_NETCDF
	type the_type;		// 
	int  line;			// current line number
	bool delete_when_close;	// for open "...|"
};
#endif
