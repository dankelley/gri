#include <string>
#include <math.h>
#include <stdio.h>
#include "gr.hh"
#include "extern.hh"
#include "defaults.hh"
extern char     _grTempString[];

bool            new_pageCmd();
bool            newCmd(void);	// for synonyms and vars

bool
new_pageCmd()
{
	extern void reset_top_of_plot(void); // in set.cc
	gr_showpage();
	_need_x_axis = true;
	_need_y_axis = true;
	reset_top_of_plot();
	return true;
}

bool
newCmd()
{
	if (_nword == 1) {
		err("Need name of thing (synonym or variable) to make new version of");
		demonstrate_command_usage();
		return false;
	}
	for (int i = 1; i < _nword; i++) {
                string w(_word[i]);
		un_double_quote(w);
		de_reference(w);
		if (is_syn(w)) {
			create_synonym(w.c_str(), "");
		} else if (is_var(w)) {
			create_variable(w.c_str(), 0.0);
		} else {
			demonstrate_command_usage();
			err("`new' only works on synonyms and variables, not on an item named `\\", _word[i], "'", "\\");
			return false;
		}
	}
	return true;
}
