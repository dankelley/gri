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

// #define DEBUG_WHILE
#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"private.hh"
#include	"extern.hh"
#include	"gr.hh"
bool            whileCmd(void);
static bool     test_is_true(const std::string& t);


bool
whileCmd(void)
{
	std::string test(6 + strstr(_cmdLine, "while"));
	if (re_compare(test.c_str(), " *")) {
		err("`while .test.|{rpn ...}' missing the test part");
		return false;
	}
	int loop_level = 1;
	int lines = 0;
	//printf("=============== in while cmd.  '%s'       test '%s'\n",_cmdLine,test.c_str());
	test_is_true(test); // to catch syntax errors on this line
	// Store lines until end while into the buffer
	std::string buffer;
	while (1) {
		if (!get_command_line()) {
			err("Missing `end while'");
			return false;
		}
		lines++;
		// Search for matching end while
		if (re_compare(_cmdLine, "\\s*while.*")) {
			loop_level++;
		} else {
			// Search for `end while', but first make a copy
			// without the source_indicator
			char *copy = (char*)malloc((1 + strlen(_cmdLine)) * sizeof(char));
			if (!copy) OUT_OF_MEMORY;
			strcpy(copy, _cmdLine);
			int len = strlen(copy);
			for (int i = 0; i < len; i++) {
				if (copy[i] == PASTE_CHAR) {
					copy[i] = '\0';
					break;
				}
			}
			if (re_compare(copy, "\\s*end\\s+while.*")) {
				loop_level--;
				if (loop_level < 1) {
					break;
				}
			}
			free(copy);
		}
		buffer.append(_cmdLine);
		buffer.append("\n");
	}
	perform_while_block(buffer.c_str(), test.c_str(), lines);
	return true;
}

#define DEBUG 1
const int NOTIFY = 1000;
bool
perform_while_block(const char *buffer, const char *test, int lines)
{
	//printf("^^^^^^^^^^^^ perform_while_block(...,%s,...)\n",test);
	std::string     filename;
	int             fileline;
	int             passes = 0;
	std::string t(test);
	while (test_is_true(t)) {
		// Check to see if test is now false
		if (block_level() > 0) {
			filename.assign(block_source_file());
			fileline = block_source_line() + 2;
#ifdef DEBUG_WHILE
			printf("Register while AT BLOCKLEVEL = %d at %s:%d lines=%d\n",
			       block_level(),
			       block_source_file(),
			       block_source_line() + 2,
			       block_line());
#endif
		} else {
			filename.assign(what_file());
			fileline = what_line() - lines + 1;
#ifdef DEBUG_WHILE
			printf("Register OUT OF BLOCK while at %s:%d\n",
			       what_file(),
			       what_line() - lines + 1);
#endif
		}
		
		if (!perform_block(buffer, filename.c_str(), fileline)) {
			// got break
			break;
		}

		passes++;
		if (_chatty > 0 && !(passes % NOTIFY)) {
			char msg[100];
			sprintf(msg, "`while' performed %d passes\n", passes);
			gr_textput(msg);
		}
	}
#ifdef DEBUG_WHILE
	printf("\nFINISHED WITH LOOP\n");
#endif
	return true;
}

static bool
test_is_true(const std::string& t)
{
	std::string tt;
	substitute_synonyms(t.c_str(), tt, true);
	clean_blanks_quotes(tt);
	// Catch "! SOMETHING" form
	bool negate = false;
	if (tt[0] == '!') {
		negate = true;
		tt.STRINGERASE(0, 1);
		clean_blanks_quotes(tt);
	}
	char res[100]; // BUG: fixed size
	substitute_rpn_expressions(tt.c_str(), res);
	//printf(" + '%s'        ->     '%s'\n", t.c_str(),tt.c_str());
	double value;
	if (is_var(res)) {
		getdnum(res, &value);
	} else {
		sscanf(res, "%lf", &value);
	}
	if (negate)
		return ((value != 0.0) ? false : true);
	else
		return ((value != 0.0) ? true : false);
}
