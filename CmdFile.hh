// Object for command files
#if !defined(_CmdFile_hh_)
#define _CmdFile_hh_
#include <string.h>
#include <stdio.h>
#include "types.hh"
#include "macro.hh"
#include "superus.hh"
int superuser(void);

class CmdFile
{
public:
	CmdFile() {
		name.assign("");
		fp = (FILE*)NULL;		// for now anyway
		interactive = true;
		line = 0;
		if (((unsigned) superuser()) & FLAG_AUT1)printf("%s:%d CONSTRUCT CmdFile fp= %x  this= %x\n",__FILE__,__LINE__,int(fp),int(this));
	}
	CmdFile(const CmdFile& c) {
		name.assign(c.get_name());
		fp = c.get_fp();
		if (fp == (FILE*)NULL) {
			printf("ERROR at %s:%d -- try to CmdFile copy null  '%s'  from %x    this %x\n",__FILE__,__LINE__,name.c_str(),int(&c),int(this));
			exit(1);
		}
		interactive = c.get_interactive();
		line = c.get_line();
		if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG %s:%d COPYCONSTRUCT CmdFile(%x)   '%s'  fp %x  this %x\n",__FILE__,__LINE__,int(&c),name.c_str(),int(fp),int(this));
	}
	~CmdFile() {
		if (((unsigned) superuser()) & FLAG_AUT1)printf("DEBUG: %s:%d  CmdFile::~CmdFile()    '%s'    fp %x    this %d\n",__FILE__,__LINE__,name.c_str(), int(fp), int(this));
		name.string::~string();
	}
	CmdFile& operator=(const CmdFile& c) {
		name.assign(c.get_name());
		fp = c.get_fp();
		if (fp == (FILE*)NULL) {
			printf("%s:%d try to CmdFile operator= null file this %x\n",__FILE__,__LINE__,int(this));
			exit(1);
		}
		interactive = c.get_interactive();
		line = c.get_line();
		if (((unsigned) superuser()) & FLAG_AUT1)printf("%s:%d CmdFile operator=   fp= %x  this= %x\n",__FILE__,__LINE__,int(fp),int(this));
		return *this;
	}
	void set(const char *n, FILE *f, bool i, int l) {
		name.assign(n);
		fp = f;
		if (fp == (FILE*)NULL) {
			printf("ERROR at %s:%d -- try to CmdFile set null file this %x\n",__FILE__,__LINE__,int(this));
			exit(1);
		}
		interactive = i;
		line = l;
		if (((unsigned) superuser()) & FLAG_AUT1)printf("%s:%d CmdFile set  fp= %x  this= %x\n",__FILE__,__LINE__,int(fp),int(this));
	}
	void increment_line()               {line++;}

	const char *get_name() const       {return name.c_str();}
	FILE *get_fp()         const        {
		if (((unsigned) superuser()) & FLAG_AUT1) {
			printf("... broken next\n");
			printf("%s:%d ... broken next\n",__FILE__,__LINE__);
			//printf("%s:%d about to return fp %x\n",__FILE__,__LINE__,int(fp));
			printf("%s:%d about to return this %x\n",__FILE__,__LINE__,int(this));
		}
		return fp;
	}
	bool get_interactive() const        {return interactive;}
	int  get_line()        const        {return line;}
private:
	FILE           *fp;
	string          name;
	bool            interactive;
	int             line;
};
#endif
