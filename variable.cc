#include	<string.h>
#include	<stdio.h>
#include        "gr.hh"
#include        "extern.hh"
#include        "private.hh"
#include        "Variable.hh"


static vector<GriVariable> variableStack;


// Make new variable
bool
create_variable(const char *name, double value)
{
	GriVariable newVariable(name, value);
	variableStack.push_back(newVariable);
	return true;
}

bool
show_variablesCmd()
{
	const int variables_per_line = 1; // how many columns?
	int             line_break = 0;
	bool            have_some = false;
	ShowStr("Variables...\n");
	vector<GriVariable>::iterator i;
	for (i = variableStack.begin(); i < variableStack.end(); i++) {
		extern char     _grTempString[];
		sprintf(_grTempString, "    %-25s = %g", i->getName(), i->getValue());
		ShowStr(_grTempString);
		if (!(++line_break % variables_per_line)) {
			ShowStr("\n");
		} else {
			ShowStr("  ");
		}
		have_some = true;
	}
	ShowStr("\n");
	if (!have_some)
		ShowStr(" ... none exist\n");
	return true;
}

// display unused user variables
void
display_unused_var()
{
	int i;
	unsigned stackLen = variableStack.size();
	extern char     _grTempString[];
	if (stackLen > 0) {
		char *name;
		for (i = stackLen - 1; i >= 0; i--) {
			if (0 == variableStack[i].getCount()) {
				name = variableStack[i].getName();
				if (*(name + 1) != '.') { // avoid builtins
					sprintf(_grTempString, "\
Warning: variable `%s' defined but not used\n", name);
					ShowStr(_grTempString);
				}
			}
		}
	}
}

// is_var - return 0 if not a variable, or 1 if is
bool
is_var(const char *w)
{
	int             len = strlen(w);
	return (len > 2 && w[0] == '.' && w[-1 + len] == '.' ? true : false);
}
// is_var - return 0 if not a variable, or 1 if is
bool
is_var(const string& w)
{
	int len = w.size();
	return (len > 2 && w[0] == '.' && w[-1 + len] == '.' ? true : false);
}

// for internal debugging
void
show_var_stack()
{
	int i;
	unsigned stackLen = variableStack.size();
	if (stackLen > 0) {
		printf("Variable stack [\n");
		for (i = stackLen - 1; i >= 0; i--) {
			printf("  %s = %f\n", 
			       variableStack[i].getName(),
			       variableStack[i].getValue());
		}
		printf("]\n");
	}
}

// Delete variable, searching from end of stack
bool
delete_var(const string& name)
{
	unsigned stackLen = variableStack.size();
	for (int i = stackLen - 1; i >= 0; i--) {
		if (name == variableStack[i].getName()) {
			for (unsigned int j = i; j < stackLen - 1; j++)
				variableStack[j] = variableStack[j + 1];
			variableStack.pop_back();
			return true;
		}
	}
	return false;
}

// get_var() - get value of variable (incrementing the 'uses' flag)
// 
// RETURN true if variable is defined and has a value
// RETURN false otherwise
bool
get_var(const char *name, double *value)
{
	*value = 0.0;		// store something in case not found
	if (!is_var(name))
		return false;
	// Following are special cases.  They are not stored in the stack because
	// it would take far too much time.
	//
	// If more variables are added to this list, be sure to make changes to
	// gri.cc and extern.h, under comments which appear as follows:
	// 
	// The following globals have symbolic names associated with them, and
	// MUST be updated whenever these names are assigned to.  See the note in
	// put_var() in variable.c.  The reason for the parallel C storage is
	// that the following are accessed for every data point plotted. Certain
	// other symbolic variables (like ..publication.. for example) are not
	// accessed frequently, and hence have no parallel C storage as the
	// following do.  Thus they are safe against breakage.
	if (!strcmp(name, "..trace..")) {
		*value = (int) _griState.trace();
		return true;
	} else if (!strcmp(name, "..use_default_for_query..")) {
		*value = (int) _use_default_for_query;
		return true;
	} else if (!strcmp(name, "..linewidth..")) {
		*value = _griState.linewidth_line();
		return true;
	} else if (!strcmp(name, "..linewidthaxis..")) {
		*value = _griState.linewidth_axis();
		return true;
	} else if (!strcmp(name, "..linewidthsymbol..")) {
		*value = _griState.linewidth_symbol();
		return true;
	} else if (!strcmp(name, "..superuser..")) {
		*value = double(_griState.superuser());
		return true;
	} else {
		// Look it up in stack
		int             i;
		unsigned        stackLen = variableStack.size();
		if (stackLen > 0) {
			for (i = stackLen - 1; i >= 0; i--) {
#ifdef DEBUG_VARIABLE
				printf("debug: check [%s] vs %d-th [%s]\n", name, i, variableStack[i].getName());
#endif
				if (!strcmp(name, variableStack[i].getName())) {
					*value = variableStack[i].getValue();
					variableStack[i].incrementCount(); // record the usage
					return true;
				}
			}
		}
	}
	return false;
}

// put_var() -- assign value to name, creating new variable if necessary.
// If replace_existing=0 replace existing value.
// If replace_existing=1 create a new variable with the old name.
// RETURN NULL if can't do it.
bool
put_var(const char *name, double value, bool replace_existing)
{
	void            reset_top_of_plot(void);
	int             i;
	unsigned        stackLen = variableStack.size();
	// put_var (): Certain special cases are stored in C variables too,
	// for speed. (An example is ..xleft.., which is stored as _xleft and
	// used all over the code.)  Capture these and store the values. Also,
	// some operations should be intercepted for general use later: e.g.
	// changing ..ymargin.. requires calling reset_top_of_plot(). Note
	// that this code fragment is easy to break, because addition of new
	// internals means addition of code here.  See also the note in gri.cc,
	// where the internals are segmented.
	if (!strcmp(name, "..trace..")) {
		_griState.set_trace((floor(0.5 + value)) ? true : false);
	} else if (!strcmp(name, "..use_default_for_query..")) {
		_use_default_for_query = int(floor(0.5 + value)) ? true : false;
	} else if (!strcmp(name, "..linewidth..")) {
		_griState.set_linewidth_line(value);
	} else if (!strcmp(name, "..linewidthaxis..")) {
		_griState.set_linewidth_axis(value);
	} else if (!strcmp(name, "..linewidthsymbol..")) {
		_griState.set_linewidth_symbol(value);
	} else if (!strcmp(name, "..superuser.."))
		_griState.set_superuser((unsigned int)(floor(0.5 + value)));
	else if (!strcmp(name, "..xleft.."))
		_xleft = value;
	else if (!strcmp(name, "..xright.."))
		_xright = value;
	else if (!strcmp(name, "..ybottom.."))
		_ybottom = value;
	else if (!strcmp(name, "..ytop.."))
		_ytop = value;
	// Replace if on stack already.
	if (replace_existing) {
		if (stackLen) {
			for (i = stackLen - 1; i >= 0; i--) {
				if (!strcmp(name, variableStack[i].getName())) {
					variableStack[i].setValue(value);
					return true;
				}
			}
		}
	}
	// Store on end of stack.
	GriVariable     newVariable(name, value);
	variableStack.push_back(newVariable);
	if (!strcmp(name, "..ymargin..") || !strcmp(name, "..ysize.."))
		reset_top_of_plot();
	return true;
}
