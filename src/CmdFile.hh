// Object for command files
#if !defined(_CmdFile_hh_)
#define _CmdFile_hh_
#include <string>
#include <stdio.h>
#include "types.hh"
#include "macro.hh"
#include "superus.hh"
unsigned int superuser(void);

class CmdFile
{
public:
	CmdFile() {
		name.assign("");
		fp = (FILE*)NULL;		// for now anyway
		interactive = true;
		save2ps = false;
		line = 0;
	}
	CmdFile(const CmdFile& c) {
		name.assign(c.get_name());
		fp = c.get_fp();
		if (fp == (FILE*)NULL) {
			printf("ERROR at %s:%d -- try to CmdFile copy null  '%s'  from %x    this %x\n",__FILE__,__LINE__,name.c_str(),int(&c),int(this));
			exit(1);
		}
		interactive = c.get_interactive();
		save2ps = c.get_save2ps();
		line = c.get_line();
	}
	~CmdFile() {
#if 0				// BUG 2001-feb-17 -- not sure on next 2 lines
		name.string::~string();	// not executed
#endif
	}
	CmdFile& operator=(const CmdFile& c) {
		name.assign(c.get_name());
		fp = c.get_fp();
		if (fp == (FILE*)NULL) {
			printf("%s:%d try to CmdFile operator= null file this %x\n",__FILE__,__LINE__,int(this));
			exit(1);
		}
		interactive = c.get_interactive();
		save2ps = c.get_save2ps();
		line = c.get_line();
		return *this;
	}
	void set(const char *n, FILE *f, bool i, int l, bool save_in_ps = true) {
		name.assign(n);
		fp = f;
		if (fp == (FILE*)NULL) {
			printf("ERROR at %s:%d -- try to CmdFile set null file this %x\n",__FILE__,__LINE__,int(this));
			exit(1);
		}
		interactive = i;
		save2ps = save_in_ps;
		line = l;
	}
	void increment_line()        { line++;		        }
	const char *get_name() const { return name.c_str();	}
	FILE *get_fp()         const { return fp;		}
	bool get_interactive() const { return interactive;	}
	int  get_line()        const { return line;		}
	bool get_save2ps()     const { return save2ps;          }
private:
	FILE           *fp;
	std::string     name;
	bool            interactive;
	bool            save2ps;
	int             line;
};
#endif
