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

// #define DEBUG
#include	<stdio.h>
#include	<string.h>
#include	"extern.hh"
#include	"private.hh"
#include        "gr.hh"
extern char     _grTempString[];

static bool     find_hint_and_def(char *hint, char *def);
static void     show_valid_choices(char *hint, char *def_word[], int def_words);

// `query \synonym|.variable ["\prompt" [(DEFAULT[s])]'
#define NCHAR 256
bool
queryCmd()
{
	char            hint[NCHAR];
	char            answer[NCHAR];
	char            def[NCHAR], *def_word[NCHAR];
	unsigned int    def_words = 0;
	bool            user_replied = true;
	if (_nword < 2) {
		demonstrate_command_usage();
		err("`query' what?");
		return false;
	}
	// Check that target synonym or variable is well-formed.
	if (!is_syn(_word[1]) && !is_var(_word[1])) {
		demonstrate_command_usage();
		err("Can't interpret `\\",
		    _word[1],
		    "' as a synonym or variable name.",
		    "\\");
		return false;
	}
	answer[0] = '\0';		// prevent problems later
	if (!find_hint_and_def(hint, def)) {
		return false;
	}
	def_word[0] = '\0';
	chop_into_words(def, def_word, &def_words, NCHAR);
	// Strip off double-quotes form list of defaults
	for (unsigned int i = 0; i < def_words; i++) {
		if (*def_word[i] == '"') {
			def_word[i] = 1 + def_word[i];
			if (*(def_word[i] + strlen(def_word[i]) - 1) == '"') {
				*(def_word[i] + strlen(def_word[i]) - 1) = '\0';
			}
		}
	}
	if (_use_default_for_query) {
		strcpy(answer, def_word[0]);
	} else {
		// In non-batch mode, print prompt and default
		if (!batch()) {
			if (hint[0] != '\0') {
				switch (def_words) {
				case 0:
					sprintf(_grTempString, "%s : ", hint);
					gr_textput(_grTempString);
					break;
				default:
					show_valid_choices(hint, def_word, def_words);
					break;
				}
			} else {
				sprintf(_grTempString, "Please give value of %s ", _word[1]);
				gr_textput(_grTempString);
			}
			beep_terminal();
		}
		// Get the answer
		gr_textget(answer, NCHAR);
		answer[strlen(answer) - 1] = '\0';
		// If no string supplied, use default if have one
		if (strlen(answer) < 1) {
			if (def[0] != '\0') {
				strcpy(answer, def_word[0]);
			} else {
				err("No response and no default, so I won't define the synonym");
				return false;
			}
			user_replied = false;
		} else {
			// They gave a string.  If there was a list of acceptable
			// choices, check that string was in list.
			if (def_words > 1) {
				bool            acceptable = false;
				while (!acceptable) {
					for (unsigned int i = 0; i < def_words; i++) {
						if (!strcmp(def_word[i], answer)) {
							acceptable = true;
							break;
						}
					}
					if (acceptable)
						break;
					gr_textput(" Sorry, this is an invalid answer.  Try again.\n");
					show_valid_choices(hint, def_word, def_words);
					gr_textget(answer, LineLength_1);
					answer[strlen(answer) - 1] = '\0';
					if (strlen(answer) < 1) {
						strcpy(answer, def_word[0]);
						break;
					}
				}
			}
		}
	}
	// Store result into the target variable or synonym
	if (is_syn(_word[1])) {
		if (!put_syn(_word[1], answer, true)) {
			sprintf(_grTempString,
				"Sorry, synonym stack exhausted; no space for `%s'",
				_word[1]);
			fatal_err(_grTempString);
			return false;
		}
	} else if (is_var(_word[1])) {
		double          value;
		bool            old = _ignore_error;
		_ignore_error = true;
		if (getdnum(answer, &value)) {
			PUT_VAR(_word[1], value);
		} else {
			if (user_replied) {
				if (!_cmdFILE.back().get_interactive()) {
					fatal_err("`query' cannot understand number in reply `\\",
						  answer,
						  "'.",
						  "\\");
				} else {
					err("`query' cannot understand number in reply `\\",
					    answer,
					    "'.",
					    "\\");
				}
			} else {
				if (!_cmdFILE.back().get_interactive()) {
					fatal_err("`query' cannot understand number in default `\\",
						  answer,
						  "'.",
						  "\\");
				} else {
					err("`query' cannot understand number in default `\\",
					    answer,
					    "'.",
					    "\\");
				}
			}
			_ignore_error = old;
		}
	} else {
		// Actually, this code is a repeat of code above, and this line can
		// never be reached in present version.  But I'll keep it here for
		// safety upon changes.
		demonstrate_command_usage();
		err("Can't interpret `\\",
		    _word[1],
		    "' as a synonym or variable name.",
		    "\\");
		return false;
	}
	return true;
}

static void
show_valid_choices(char *hint, char *def_word[], int def_words)
{
	if (def_words == 1) {
		sprintf(_grTempString, "%s (default = `%s'): ", hint, def_word[0]);
		gr_textput(_grTempString);
	} else {
		int             i;
		sprintf(_grTempString, "%s (default = `%s'; choices are", hint, def_word[0]);
		gr_textput(_grTempString);
		for (i = 0; i < def_words; i++) {
			gr_textput(" `");
			gr_textput(def_word[i]);
			gr_textput("'");
		}
		gr_textput(") : ");
	}
}

// find_hint_and_def -- find hint and default. (make def="" if no default)
// return true if ok, NO if deformed commandline
static          bool
find_hint_and_def(char *hint, char *def)
{
	hint[0] = '\0';
	def[0] = '\0';
	// Find and extract hint, as first quoted string.
#ifdef DEBUG
	printf("CMDLINE [%s]\n",_cmdLine);
#endif
	int i;
	int len = strlen(_cmdLine);
	char lastc = '\0';
	int ii = 0;
	for (i = 0; i < len; i++) {
		if (*(_cmdLine + i) == '\"') {
			bool valid = false;
			i++;
			for (ii = 0; ii < len - i; ii++) {
				if (_cmdLine[i + ii] == '"' && lastc != '\\') {
					hint[ii] = '\0';
					valid = true;
					break;
				} else {
					hint[ii] = _cmdLine[i + ii];
				}
				lastc = _cmdLine[i + ii];
			}
			if (valid) {
				break;
			}
		}
	}

	// Return now if no default was given ...
	if (i + ii == len - 1)
		return true;
	// ... or extract it, if it was given.
	int def_start = -1, def_end = -1;
	int level = 0;
	for (i = len - 1; i > 0; i--) {
		if (_cmdLine[i] == ')') {
			if (level++ == 0)
				def_end = i - 1;
		} else if (_cmdLine[i] == '(') {
			if (--level == 0) {
				def_start = i + 1;
				break;
			}
		}
	}
	if (def_start && !def_end) {
		err("Default is malformed; have `(' but no `)'");
		return false;
	}
	if (def_end && !def_start) {
		err("Default is malformed; have `)' but no `('");
		return false;
	}
	if (def_start > def_end) {
		err("No default value found inside parentheses");
		return false;
	}
	if (def_start > -1 && def_end > -1) {
		for (i = def_start; i <= def_end; i++)
			def[i - def_start] = _cmdLine[i];
		def[1 + def_end - def_start] = '\0';
	}
#ifdef DEBUG
	printf("HINT [%s] DEFAULT [%s]\n", hint, def);
#endif

	// If the default is a variable, parse it into the hint
	// that will be presented to the user.
	if (is_var(def)) {
#ifdef DEBUG
		printf("it is a var [%s]\n",def);
#endif
		double def_value;
		bool ok = get_var(def, &def_value);
		if (ok) {
			sprintf(def, "%f", def_value); // BUG: assume will fit
#ifdef DEBUG
			printf("WROTE [%s]\n",def);
#endif
		}
#ifdef DEBUG
		printf("def is now [%s]\n",def);
#endif
	}
	return true;
}
