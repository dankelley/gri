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
	name = new char [1 + strlen("stdin")];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, "stdin");
	if (_cmdFILE.size() > 0)
	    fp = _cmdFILE.end()->get_fp();
	else 
	    fp = NULL;
	the_type = from_cmdfile;
	line = 0;
	delete_when_close = false;
    }
    DataFile(const DataFile& d) {
	name = new char[1 + strlen(d.get_name())];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, d.get_name());
	fp = d.get_fp();
	netCDF_id = d.get_netCDF_id();
	the_type = d.get_type();
	line = d.get_line();
	delete_when_close = d.get_delete_when_close();
    }
    ~DataFile() {
	delete [] name;
    }
    DataFile& operator=(const DataFile& d) {
	COPY_STRING(name, d.get_name());
	fp = d.get_fp();
	netCDF_id = d.get_netCDF_id();
	the_type = d.get_type();
	line = d.get_line();
	delete_when_close = d.get_delete_when_close();
	return *this;
    }
    void set_name(const char *new_name) {
	COPY_STRING(name, new_name);
    }
    void set_fp(FILE *new_fp)           {fp = new_fp;}
    void set_netCDF_id(int new_id)      {netCDF_id = new_id;}
    void set_type(type new_type)        {the_type = new_type;}
    void set_line(int new_line)         {line = new_line > 1 ? new_line : 1;}
    void set_delete_when_close(bool d)  {delete_when_close = d;}

    void increment_line()               {line++;}

    char *get_name()                    {return name;}
    const char *get_name()       const  {return name;}
    FILE *get_fp()               const  {return fp;}
    int  get_netCDF_id()         const  {return netCDF_id;}
    type get_type()              const  {return the_type;}
    int get_line()               const  {return line;}
    bool get_delete_when_close() const  {return delete_when_close;}
private:
    char *name;			// name of file, or stdin
    FILE *fp;			// file pointer
    int  netCDF_id;		// only used if FILE_BIN_NETCDF
    type the_type;		// 
    int  line;			// current line number
    bool delete_when_close;	// for open "...|"
};
#endif
