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
	gr_setfont(gr_currentfont(), true);
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
	//printf("DEBUG.  Should now start a new ps file named '%s'\n",_word[3]);
	gr_end("!");
	std::string unquoted(_word[3]);
	un_double_quote(unquoted);
	gr_setup_ps_filename(unquoted.c_str());
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
	for (unsigned int i = 1; i < _nword; i++) {
                std::string w(_word[i]);
		un_double_quote(w);
		//printf("DEBUG %s:%d <%s>\n",__FILE__,__LINE__,w.c_str());
		std::string value;
		if (get_syn(w.c_str(), value, false)) {
			std::string coded_name;
			int coded_level = -1;
			//printf("DEBUG <%s>\n", value.c_str());
			if (is_coded_string(value.c_str(), coded_name, &coded_level)) {
				//printf("DEBUG %s:%d is <%s> <%s> level %d\n",__FILE__,__LINE__,value.c_str(),coded_name.c_str(),coded_level);
				if (coded_name[0] == '.') {
					int index = index_of_variable(coded_name.c_str(), coded_level);
					GriVariable newVariable(coded_name.c_str(), 0.0);
					variableStack.insert(variableStack.begin() + index + 1, newVariable);
					//printf("VAR index is %d\n", index);
				} else if (coded_name[0] == '\\'){
					int index = index_of_synonym(coded_name.c_str(), coded_level);
					//printf("SYN index is %d, now holds <%s>\n", index, synonymStack[index].get_value());
					GriSynonym newSynonym(coded_name.c_str(), "");
					synonymStack.insert(synonymStack.begin() + index + 1, newSynonym);
				} else {
					err("new cannot decode item `\\", _word[i], "'.", "\\");
					return false;
				}
				return true;
			}
		}

		// de_reference(w);

		if (is_syn(w)) {
			//printf("DEBUG 4-a SYN <%s>\n", w.c_str());
			if (w[1] == '@') {
				std::string clean("\\");
				clean.append(w.substr(2, w.size()));
				std::string named;
				get_syn(clean.c_str(), named, false);
				//printf("NEW IS AN ALIAS SYN %s:%d <%s>  [%s]\n",__FILE__,__LINE__,clean.c_str(),named.c_str());
				if (is_var(named.c_str())) {
					//printf("NEW VAR [%s]\n",named.c_str());
					create_variable(named.c_str(), 0.0);
				} else if (is_syn(named.c_str())) {
					//printf("NEW SYN [%s]\n",named.c_str());
					create_synonym(named.c_str(), "");
				} else {
					err("`new' cannot decode `\\", w.c_str(), "'", "\\");
					return false;
				}
			} else {
				create_synonym(w.c_str(), "");
			}
		} else if (is_var(w)) {
			//printf("DEBUG 4-b VAR <%s>\n", w.c_str());
			create_variable(w.c_str(), 0.0);
		} else {
			demonstrate_command_usage();
			err("`new' only works on synonyms and variables, not on an item named `\\", _word[i], "'", "\\");
			return false;
		}
	}
	return true;
}
