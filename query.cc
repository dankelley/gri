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
    int             def_words = 0;
    int             i;
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
    chop_into_words(def, def_word, &def_words, NCHAR);
    // Strip off double-quotes form list of defaults
    for (i = 0; i < def_words; i++) {
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
		int             i;
		bool            acceptable = false;
		while (!acceptable) {
		    for (i = 0; i < def_words; i++) {
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
    int             i;
    int             len = strlen(_cmdLine);
    char            lastc = '\0';
    hint[0] = '\0';
    def[0] = '\0';
    // Find and extract hint, as first quoted string.
    for (i = 0; i < len; i++) {
	if (*(_cmdLine + i) == '\"') {
	    bool            valid = false;
	    int             ii;
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
    // Extract default, as last string enclosed in parentheses
    int def_start = 0, def_end = 0;
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
    for (i = def_start; i <= def_end; i++)
	def[i - def_start] = _cmdLine[i];
    def[1 + def_end - def_start] = '\0';
    return true;
}
