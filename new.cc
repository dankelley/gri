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
// new postscript file "name"
bool
new_postscript_fileCmd()
{
	if (_nword < 4) {
		err("`new postscript file' needs a filename.");
		demonstrate_command_usage();
		return false;
	} else if (_nword > 4) {
		err("`new postscript file' takes just 1 argument, a filename");
		demonstrate_command_usage();
		return false;
	}
	printf("DEBUG.  Should now start a new ps file named '%s'\n",_word[3]);
	gr_end("!");
	gr_setup_ps_filename(_word[3]);
#if 0
	// BUG: don't know argc/argv from here!
	insert_creator_name_in_PS(argc, argv, psname);
#endif
	gr_begin(2);
	// Trick it into resetting to present font 
	gr_fontID present_font = gr_currentfont();
	gr_setfont(gr_font_Courier);
	gr_setfont(present_font);
	return true;
}

// new [.var.|\syn [.var.|\syn [...]]
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
