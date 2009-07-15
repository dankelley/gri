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

// Store synonym name/value
#if !defined(_grisynonym_h_)
#define  _grisynonym_h_
#include "GCounter.hh"
#if 1
class GriSynonym : public GriCounter
{
public:
	GriSynonym() {
		;
	}
	GriSynonym(const char *the_name, const char *the_value) {
		name.assign(the_name);
		value.assign(the_value);
	}
	GriSynonym(const GriSynonym& c) {
		name.assign(c.get_name());
		value.assign(c.get_value_quietly());
	}
	~GriSynonym() {
#if 0				// BUG 2001-feb-17 -- not sure on next 2 lines
		name.string::~string();	// not executed
		value.string::~string(); // not executed
#endif
	}
	void setNameValue(const char *the_name, const char *the_value) {
		name.assign(the_name);
		value.assign(the_value);
	}
	void set_value(const char *the_value)		{value.assign(the_value); }
	const char *get_name(void) const		{return name.c_str();};
	const char *get_value(void)			{incrementCount(); return value.c_str();};
	const char *get_value_quietly(void) const	{return value.c_str();};
	GriSynonym& operator=(const GriSynonym& n) {
		name.assign(n.get_name());
		value.assign(n.get_value_quietly());
		return *this;
	}
private:
	std::string name;
	std::string value;
};



#else

class GriSynonym : public GriCounter
{
public:
	GriSynonym() {
		name = new char [1];	if (!name)  OUT_OF_MEMORY;
		name[0] = '\0';
		value = new char [1];	if (!value) OUT_OF_MEMORY;
		value[0] = '\0';
	}
	GriSynonym(const char *the_name, const char *the_value) {
		name = new char [1 + strlen(the_name)];	  if (!name)  OUT_OF_MEMORY;
		strcpy(name, the_name);
		value = new char [1 + strlen(the_value)]; if (!value) OUT_OF_MEMORY;
		strcpy(value, the_value);
	}
	GriSynonym(const GriSynonym& c) {
		name = new char [1 + strlen(c.getName())];
		if (!name) OUT_OF_MEMORY;
		strcpy(name, c.getName());
		value = new char [1 + strlen(c.get_value_quietly())];
		if (!value) OUT_OF_MEMORY;
		strcpy(value, c.get_value_quietly());
	}
	~GriSynonym() {
#if 0
		delete [] name;
		delete [] value;
#endif
	}
	void setNameValue(const char *the_name, const char *the_value) {
		if (strlen(the_name) > strlen(name)) {
			delete [] name;
			name = new char [1 + strlen(the_name)];
			if (!name) OUT_OF_MEMORY;
		}
		strcpy(name, the_name);
		if (strlen(the_value) > strlen(value)) {
			delete [] value;
			value = new char [1 + strlen(the_value)];
			if (!value) OUT_OF_MEMORY;
		}
		strcpy(value, the_value);
	}
	void set_value(const char *the_value) {
		if (strlen(the_value) > strlen(value)) {
			delete [] value;
			value = new char[1 + strlen(the_value)];
			if (!value) OUT_OF_MEMORY;
		}
		strcpy(value, the_value);
	}
	char *getName(void) const		{return name;};
	char *getValue(void)  	      {incrementCount(); return value;};
	char *get_value_quietly(void) const {return value;};
	GriSynonym& operator=(const GriSynonym& n) {
		char *cp = n.getName();
		if (strlen(cp) > strlen(name)) {
			delete [] name;
			name = new char [1 + strlen(cp)];
			if (!name) OUT_OF_MEMORY;
		}
		strcpy(name, cp);
		cp = n.get_value_quietly();
		if (strlen(cp) > strlen(value)) {
			delete [] value;
			value = new char [1 + strlen(cp)];
			if (!value) OUT_OF_MEMORY;
		}
		strcpy(value, cp);
		return *this;
	}
private:
	char *name;
	char *value;
};
#endif

#endif
