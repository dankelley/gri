// Store variable name/value
#if !defined(_grivariable_h_)
#define  _grivariable_h_
#include "GCounter.hh"
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
	name = new char [1 + strlen(c.getName())];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, c.getName());
	value = c.get_value_quietly();
    };
    ~GriVariable() {delete [] name;};
    void setNameValue(const char *the_name, double the_value) {
	if (strlen(the_name) > strlen(name)) {
	    delete [] name;
	    name = new char [1 + strlen(the_name)];
	    if (!name) OUT_OF_MEMORY;
	}
	strcpy(name, the_name);
	value = the_value;
    };
    void   setValue(double the_value) {value = the_value;};
    char  *getName(void) const        {return name;};
    double getValue(void)            {incrementCount(); return value;};
    double get_value_quietly(void) const {return value;};
    GriVariable& operator=(const GriVariable& n) {
	char *cp = n.getName();
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
