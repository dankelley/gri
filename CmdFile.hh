// Object for command files
#if !defined(_CmdFile_hh_)
#define _CmdFile_hh_
#include <string.h>
#include <stdio.h>
#include "types.hh"
#include "macro.hh"

class CmdFile
{
public:
    CmdFile() {
	name = new char[1];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, "");
	fp = NULL;		// for now anyway
	interactive = true;
	line = 0;
    }
    CmdFile(const CmdFile& c) {
	name = new char[1 + strlen(c.get_name())];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, c.get_name());
	fp = c.get_fp();
	interactive = c.get_interactive();
	line = c.get_line();
    }
    ~CmdFile() {delete [] name;}
    CmdFile& operator=(const CmdFile& c) {
	COPY_STRING(name, c.get_name());
	fp = c.get_fp();
	interactive = c.get_interactive();
	line = c.get_line();
	return *this;
    }
    void set(const char *n, FILE *f, bool i, int l) {
	COPY_STRING(name, n);
	fp = f;
	interactive = i;
	line = l;
    }
    void increment_line()               {line++;}

    const char *get_name() const        {return name;}
    char *get_name()                    {return name;}
    FILE *get_fp()         const        {return fp;}
    bool get_interactive() const        {return interactive;}
    int  get_line()        const        {return line;}
private:
    char           *name;
    FILE           *fp;
    bool            interactive;
    int             line;
};
#endif
