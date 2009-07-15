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

// Object for command files
#if !defined(_CmdFile_hh_)
#define _CmdFile_hh_
#include <cstdlib>
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
			printf("ERROR at %s:%d -- try to CmdFile copy null  '%s'  from %lx    this %lx\n",
			       __FILE__,__LINE__,name.c_str(),(long unsigned int)(&c),(long unsigned int)this);
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
			printf("%s:%d try to CmdFile operator= null file this %lx\n",__FILE__,__LINE__,(long unsigned int)this);
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
			printf("ERROR at %s:%d -- try to CmdFile set null file this %lx\n",__FILE__,__LINE__,(long unsigned int)this);
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
