// Store synonym name/value
#if !defined(_grisynonym_h_)
#define  _grisynonym_h_
#include "GCounter.hh"
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
	void setValue(const char *the_value) {
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
