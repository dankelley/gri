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
    int             i;
    if (_nword == 1) {
	err("Need name of thing (synonym or variable) to make new version of");
	demonstrate_command_usage();
	return false;
    }
    for (i = 1; i < _nword; i++) {
	if (is_syn(_word[i])) {
	    /* Make new synonym */
	    create_synonym(_word[i], "");
	} else if (is_var(_word[i])) {
	    create_variable(_word[i], 0.0);
	} else {
	    demonstrate_command_usage();
	    err("`new' only works on synonyms and variables, not on an item named `\\", _word[i], "'", "\\");
	    return false;
	}
    }
    return true;
}
