// Store variable name/value
#if !defined(_grivariable_h_)
#define  _grivariable_h_
#include "GCounter.hh"
#if 1
class GriVariable : public GriCounter
{
public:
	GriVariable() {
		value = 0.0;
	}
	GriVariable(const char *the_name, double the_value) {
		name.assign(the_name);
		value = the_value;
	};
	GriVariable(const GriVariable& c) {
		name.assign(c.get_name());
		value = c.get_value_quietly();
	};
	~GriVariable() { 
#if 0				// BUG 2001-feb-17 -- not sure on next 2 lines
		name.string::~string(); // not executed
#endif
	};
	void set_name_value(const char *the_name, double the_value) {
		name.assign(the_name);
		value = the_value;
	};
	void   set_value(double the_value) {value = the_value;};
	const char *get_name(void) const        {return name.c_str();};
	double get_value(void)            {incrementCount(); return value;};
	double get_value_quietly(void) const {return value;};
	GriVariable& operator=(const GriVariable& n) {
		name.assign(n.get_name());
		value = n.get_value_quietly();
		return *this;
	}
private:
	std::string  name;
	double  value;
};



#else

class GriVariable : public GriCounter
{
public:
	GriVariable() {
		name = new char [1];
		if (!name) OUT_OF_MEMORY;
		name[0] = '\0';
		value = 0.0;
	}
	GriVariable(const char *the_name, double the_value) {
		name = new char [1 + strlen(the_name)];
		if (!name) OUT_OF_MEMORY;
		strcpy(name, the_name);
		value = the_value;
	};
	GriVariable(const GriVariable& c) {
		name = new char [1 + strlen(c.get_name())];
		if (!name) OUT_OF_MEMORY;
		strcpy(name, c.get_name());
		value = c.get_value_quietly();
	};
	~GriVariable() {delete [] name;};
	void set_name_value(const char *the_name, double the_value) {
		if (strlen(the_name) > strlen(name)) {
			delete [] name;
			name = new char [1 + strlen(the_name)];
			if (!name) OUT_OF_MEMORY;
		}
		strcpy(name, the_name);
		value = the_value;
	};
	void   set_value(double the_value) {value = the_value;};
	char  *get_name(void) const        {return name;};
	double get_value(void)            {incrementCount(); return value;};
	double get_value_quietly(void) const {return value;};
	GriVariable& operator=(const GriVariable& n) {
		char *cp = n.get_name();
		if (strlen(cp) > strlen(name)) {
			delete [] name;
			name = new char [1 + strlen(cp)];
			if (!name) OUT_OF_MEMORY;
		}
		strcpy(name, cp);
		value = n.get_value_quietly();
		return *this;
	}
private:
	char   *name;
	double  value;
};
#endif

#endif
