/* #define DEBUG_SYNONYMS */
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "Synonym.hh"

static vector<GriSynonym> synonymStack;

static inline int end_of_synonym(char c, bool inmath, bool need_brace);

bool
create_synonym(const char *name, const char *value)
{
	GriSynonym newSynonym(name, value);
	synonymStack.push_back(newSynonym);
	return true;
}

bool
show_synonymsCmd()
{
	ShowStr("Synonyms...\n");
	bool have_some = false;
	vector<GriSynonym>::iterator i;
	for (i = synonymStack.begin(); i < synonymStack.end(); i++) {
		extern char     _grTempString[];
		sprintf(_grTempString, "%s=\"%s\"\n", 
			unbackslash(i->getName()), i->getValue());
		ShowStr(_grTempString);
		have_some = true;
	}
	ShowStr("\n");
	if (!have_some) {
		ShowStr(" ... none exist\n");
	}
	return true;
}

// display unused user synonyms
void
display_unused_syn()
{
	int i;
	unsigned stackLen = synonymStack.size();
	extern char     _grTempString[];
	if (stackLen > 0) {
		char *name;
		for (i = stackLen - 1; i >= 0; i--) {
			if (0 == synonymStack[i].getCount()) {
				name = synonymStack[i].getName();
				if (strlen(name) > 0 && *(name + 1) != '.') {
					sprintf(_grTempString, "\
Warning: synonym `%s' defined but not used\n", unbackslash(name));
					ShowStr(_grTempString);
				}
			}
		}
	}
}

bool
update_readfrom_file_name()
{
	// .readfrom_file.
	if (!put_syn("\\.readfrom_file.", _dataFILE.back().get_name(), true)) {
		err("Sorry, can't update `\\.readfrom_file.'");
		return false;
	}
	return true;
}

// is_syn - return 0 if not a synonym, or 1 if is
bool
is_syn(const char *name)
{
	return ((name[0] == '\\') ? true : false);
}

void
show_syn_stack()
{
	int i;
	unsigned stackLen = synonymStack.size();
	if (stackLen > 0) {
		printf("Synonym stack [\n");
		for (i = stackLen - 1; i >= 0; i--) {
			printf("  %s = %s\n", synonymStack[i].getName(), synonymStack[i].getValue());
		}
		printf("]\n");
	}
}

// delete_syn() - delete synonym
bool
delete_syn(const char *name)
{
	int i;
	unsigned stackLen = synonymStack.size();
	for (i = stackLen - 1; i >= 0; i--) {
		if (!strcmp(name, synonymStack[i].getName())) {
			for (unsigned j = i; j < stackLen - 1; j++)
				synonymStack[j] = synonymStack[j + 1];
			synonymStack.pop_back();
			return true;
		}
	}
	return false;
}

// get_syn() - get value of synonym Sufficient storage for (char *value) must
// have been set aside by the calling routine.
// RETURN true if synonym is defined and has a value
bool
get_syn(const char *name, char *value)
{
	if (!is_syn(name))
		return false;
	if (!strncmp(name, "\\.proper_usage.", 15)) {
		/*
		 * Take care of synonym \.proper_usage., used to demonstrate the
		 * proper usage of a command.
		 */
		strcpy(value, unbackslash(_command[cmd_being_done()].syntax));
		return true;
	} else if (!strncmp(name, "\\.word", 6) && strlen(name) > 6) {
		/*
		 * Take care of the synonym list for the command being processed.
		 * This list has synonyms \.words. (number of words in last level
		 * command) and \.word0., \.word1., etc (the words in last level
		 * command).
		 * 
		 * This list is not stored in the normal synonym stack, but is created
		 * here using the word stack is maintained in command.c.
		 * 
		 * Here is what the command stack will look like, for the command `New
		 * Draw TS Line \rho' within a new gri command named `Draw
		 * Isopycnals' which was called with the line `New Draw TS Line
		 * 26.00':
		 * 
		 * -----
		 * 
		 * Draw Isopycnals
		 * 
		 * -----
		 * 
		 * New Draw TS Line 26.00
		 */
		int             i;
		extern int      _num_command_word;
		extern char    *_command_word[MAX_cmd_word];
		extern char    *_command_word_separator;
		if (*(name + 6) == 's' && *(name + 7) == '.' && *(name + 8) == '\0') {
			// The synonym \.words. is the number of words in the command.
			if (_num_command_word > 0) {
				int             cmd, level = 0, count = 0;
				for (cmd = _num_command_word - 1; cmd > -1; cmd--) {
					if (!strcmp(_command_word[cmd], _command_word_separator))
						level++;
					if (level == 1 || cmd <= 0) {
						for (cmd++; cmd < _num_command_word; cmd++) {
							if (!strcmp(_command_word[cmd], _command_word_separator))
								break;
							else
								count++;
						}
						sprintf(value, "%d", count);
						return true;
					}
				}
			} else {
				strcpy(value, "\\.words.");
			}
			return true;
		} else if (1 == sscanf(name + 6, "%d", &i)) {
			// The synonym \.wordn. is the n-th word in the command.
			if (_num_command_word == 0) {
				strcpy(value, name);
				return true;
			}
			int             cmd, level = 0;
			// Trace back through the stack until at next level deep, then
			// move forward to indicated word.
			for (cmd = _num_command_word - 1; cmd > -1; cmd--) {
				if (!strcmp(_command_word[cmd], _command_word_separator))
					level++;
				if (level == 1 || cmd <= 0) {
					cmd += i + 1;
					break;
				}
			}
			/*
			 * If the command is requesting a word beyond the list (e.g.
			 * \.word10. in a line with only 3 words) just paste the name of
			 * the desired synonym (e.g "\.word10.") onto the line, and let
			 * further processing catch the error, if it is in a bit of code
			 * that is not being skipped.
			 */
			if (cmd >= _num_command_word) {
				strcpy(value, name);
			} else {
				/*
				 * The word does exist.  If it's a quoted string, remove the
				 * quotes ...
				 */
				if (*_command_word[cmd] == '"') {
					strcpy(value, 1 + _command_word[cmd]);
					if (*(value + strlen(value) - 1) == '"') {
						value[strlen(value) - 1] = '\0';
					}
				} else {
					/* ... otherwise copy it directly. */
					strcpy(value, _command_word[cmd]);
				}
			}
			return true;		/* .word#. */
		} else {
			strcpy(value, name);
			return true;
		}
	} else {
		/*
		 * It's an ordinary synonym.  Look it up in the stack
		 */
		int             i;
		unsigned        stackLen = synonymStack.size();
		if (stackLen > 0) {
			for (i = stackLen - 1; i >= 0; i--) {
				if (!strcmp(name, synonymStack[i].getName())) {
					strcpy(value, synonymStack[i].getValue());
					synonymStack[i].incrementCount();
					return true;
				}
			}
		}
	}
	return false;
}

// Convert single backslashes into double.  Allocates new storage.
char           *
unbackslash(const char *s)
{
	char            lastc = 0;
	int             i, ii, len;
	unsigned int    hits = 0;
	len = strlen(s);
	// count unprotected backslashes
	for (i = 0; i < len; i++) {
		if (s[i] == '\\' && lastc != '\\')
			hits++;
		lastc = s[i];
	}
	char *ret = new char [1 + hits + len];
	if (!ret) OUT_OF_MEMORY;
	lastc = 0;
	for (i = ii = 0; i < len; i++) {
		if (s[i] == '\\' && lastc != '\\') {
			ret[ii++] = '\\';
			ret[ii++] = '\\';
		} else {
			ret[ii++] = s[i];
		}
		lastc = s[i];
	}
	ret[ii] = '\0';
	return ret;
}

// put_syn() -- assign value to name, creating new synonm if necessary.
// RETURN true if successfull
bool
put_syn(const char *name, const char *value, bool replace_existing)
{
	int             i;
	unsigned        stackLen = synonymStack.size();
	// In the case where instructed by the value of replace_existing, simply
	// replace the value of the variable if it already exists.
	if (replace_existing) {
		if (stackLen) {
			for (i = stackLen - 1; i >= 0; i--) {
				if (!strcmp(name, synonymStack[i].getName())) {
					synonymStack[i].setValue(value);
					return true;
				}
			}
		}
	}
	// Store on end of stack.
	GriSynonym newSynonym(name, value);
	synonymStack.push_back(newSynonym);
	return true;
}

#if 0
// Scan through input string s[], substituting synonyms.  The result is put
// into string sout[].
//
// If inmath=1, special math strings such as \alpha are inserted as different
// fonts.
//
// RETURN true if line not empty
bool
substitute_synonyms_cmdline(const char *s, char *sout, bool allow_math)
{
	if (strlen(s) < 1) {
		strcpy(sout, s);
		return false;
	}
	*sout = '\0';
	char *sCopy = strdup(s);
	int             nword;
	chop_into_words(sCopy, _Words2, &nword, MAX_nword);

	int offset = 0;
#if 1 // 9apr95, vsn2.035
	// Pass `sprintf \synonym ...' through directly
	if (nword > 1 && !strcmp(_Words2[0], "sprintf")) {
		strcat(sout, _Words2[0]);
		strcat(sout, " ");
		strcat(sout, _Words2[1]); // the synonym name
		strcat(sout, " ");
		offset = skip_space(s);	// initial blanks
		offset += skip_nonspace(s + offset); // SPRINTF
		offset += skip_space(s + offset);    // blanks
		offset += skip_nonspace(s + offset); // the synonym name
		offset += skip_space(s + offset);    // blanks
	}
#endif
	// Pass `show defined ... (\synonym)' through directly
	if (nword > 2 && !strcmp(_Words2[0], "show") && !strcmp(_Words2[1], "defined")) {
		strcpy(sout, s);
		free(sCopy);
		return true;
	}
	// Pass `delete \synonym' through directly.
	if (nword == 2 && !strcmp(_Words2[0], "delete") && *_Words2[1] == '\\') {
		free(sCopy);
		return true;
	}
	if (nword < 1) {
		free(sCopy);
		return true;
	}
	// Pass `read \syn ...' and `read .var. ...' through without substituting
	if (!strcmp(_Words2[0], "read")
	    && nword > 1 
	    && (is_syn(_Words2[1]) || is_var(_Words2[1]))) {
		strcpy(sout, s);
		free(sCopy);
		return true;
	}
#if 1
	// Pass `while ...' through
	if (!strcmp(_Words2[0], "while")) {
		strcpy(sout, s);
		free(sCopy);
		return true;
	}
#endif
	// Some special cases are passed through without the syn name being
	// substituted.  This is done by supplying an offset to the string.
	if (!strcmp(_Words2[0], "query")) {
		strcpy(sout, _Words2[0]);
		strcat(sout, " ");
		strcat(sout, _Words2[1]);
		strcat(sout, " ");
		offset = skip_space(s);	// initial blanks
		offset += skip_nonspace(s + offset); // query
		offset += skip_space(s + offset);    // blanks
		offset += skip_nonspace(s + offset); // item
		offset += skip_space(s + offset);    // blanks
	}
	// Protect first word of `\name = "value"', but not of `\syn ...'. In
	// other words, protect first word if matches \synonym[ ]*=.*
	if (*_Words2[0] == '\\') {
		if (nword > 1 && !strcmp(_Words2[1], "=")) {
			strcpy(sout, _Words2[0]);
			strcat(sout, " ");
			offset = 1 + strlen(_Words2[0]);
		}
	}
	free(sCopy);
	return substitute_synonyms(s + offset, sout, allow_math);
}
#else

// Scan through input string s[], substituting synonyms.  The result is put
// into string sout[].
//
// If inmath=1, special math strings such as \alpha are inserted as different
// fonts.
//
// RETURN true if line not empty
bool
substitute_synonyms_cmdline(const char *s, string& sout, bool allow_math)
{
	sout = "";
	if (strlen(s) < 1) {
		return false;
	}
	char *sCopy = strdup(s);
	int             nword;
	chop_into_words(sCopy, _Words2, &nword, MAX_nword);
	int offset = 0;
	// Pass `sprintf \synonym ...' through directly
	if (nword > 1 && !strcmp(_Words2[0], "sprintf")) {
		sout.append(_Words2[0]);
		sout.append(" ");
		sout.append(_Words2[1]); // the synonym name
		sout.append(" ");
		offset = skip_space(s);	// initial blanks
		offset += skip_nonspace(s + offset); // SPRINTF
		offset += skip_space(s + offset);    // blanks
		offset += skip_nonspace(s + offset); // the synonym name
		offset += skip_space(s + offset);    // blanks
	}
	// Pass `show defined ... (\synonym)' through directly
	if (nword > 2 && !strcmp(_Words2[0], "show") && !strcmp(_Words2[1], "defined")) {
		sout = s;
		free(sCopy);
		return true;
	}
	// Pass `delete \synonym' through directly.
	if (nword == 2 && !strcmp(_Words2[0], "delete") && *_Words2[1] == '\\') {
		free(sCopy);
		return true;
	}
	if (nword < 1) {
		free(sCopy);
		return true;
	}
	// Pass `read \syn ...', `read .var. ...' and `read line \syn' through
	if (strEQ(_Words2[0], "read")) {
		if (nword == 2
		    && (is_syn(_Words2[1]) || is_var(_Words2[1]))) {
			sout = s;
			free(sCopy);
			return true;
		} else if (nword == 3
			   && strEQ(_Words2[1], "line")
			   && is_syn(_Words2[2])) {
			sout = s;
			free(sCopy);
			return true;
		}
	}
	// Pass `while ...' through
	if (!strcmp(_Words2[0], "while")) {
		sout = s;
		free(sCopy);
		return true;
	}
	// Some special cases are passed through without the syn name being
	// substituted.  This is done by supplying an offset to the string.
	if (!strcmp(_Words2[0], "query")) {
		sout.append(_Words2[0]);
		sout.append(" ");
		sout.append(_Words2[1]);
		sout.append(" ");
		offset = skip_space(s);	// initial blanks
		offset += skip_nonspace(s + offset); // query
		offset += skip_space(s + offset);    // blanks
		offset += skip_nonspace(s + offset); // item
		offset += skip_space(s + offset);    // blanks
	}
	// Protect first word of `\name = "value"', but not of `\syn ...'. In
	// other words, protect first word if matches \synonym[ ]*=.*
	if (*_Words2[0] == '\\') {
		if (nword > 1 && !strcmp(_Words2[1], "=")) {
			sout = _Words2[0];
			sout.append(" ");
			offset = 1 + strlen(_Words2[0]);
		}
	}
	free(sCopy);
	return substitute_synonyms(s + offset, sout, allow_math);
}
#endif

#if 0 // before 2.6.0, using 'char*' as the output buffer, but that's dangerous
// Walk through string, substituting synonyms if not in math mode.
// RETURN 0 if empty line, 1 otherwise.
bool
substitute_synonyms(const char *s, char *sout, bool allow_math)
{
	//printf("--- substitute_synonyms('%s',...)\n",s);
	bool            inmath = false; // are we within a math string?
	int             trailing_dots_in_name = 0;
	int             dots_in_name = 0;
	int             slen = strlen(s);
	char           *sname = new char [1 + slen]; // certainly long enough
	char           *svalue = new char [_grTempStringLEN]; // very long also
	for (int i = 0; i < slen; i++) {
		//printf("DEBUG1: i %d  (%s)\n",i,s+i);
		int             found = 0;
		int             syn_len = 0;
		// If entering or leaving math mode, just paste $ onto the end and
		// skip to the next character.
		//
		if (s[i] == '$' && allow_math) {
			if (i == 0) {
				inmath = true;
			} else if (s[i - 1] != '\\') {	// \$ is escape to pass $
				inmath = inmath ? false : true;
			}
			strcat_c(sout, '$');
			continue;
		}
		// If not start of synonym, just paste character onto end of string
		// and continue. (This also applies to apparent synonyms, if they are
		// during math mode.)
		//printf("DEBUG a\n");
		if (s[i] != '\\' || inmath) {
			//printf("DEBUG b\n");
			strcat_c(sout, s[i]);
			continue;
		}
		//printf("DEBUG ** found backslash\n");

		// Now know that s[i] is backslash, and not inmath.
		// Pass a few escape strings through directly. 
		if (s[i + 1] == '$'
#if 0				// 2.2.3
		    || s[i + 1] == '['
		    || s[i + 1] == ']'
#endif
		    || s[i + 1] == '"'
		    || s[i + 1] == '\\') {
			strcat_c(sout, s[i]);
			strcat_c(sout, s[++i]);
			continue;
		}
		// Now know that it's the start of a synonym.  Isolate it, then find
		// value.   But first, take note of whether name has a period at the
		// start (since this determines whether a period can be used to end
		// it).
		bool            report_num_words = false;
		bool            report_a_word = false;
		int             word_to_report = -1;
		bool            need_brace = (s[i + 1] == '{');
		//printf("DEBUG1: i %d  (%s)\n",i,s+i);
#if 0
		if (need_brace)
			i++;		// skip the brace itself
#endif
		if (s[i + 1] == '.') {
			int ii;
			for(ii = i + 1; ii < slen; ii++) {
				if (s[ii] == '.')
					dots_in_name++;
				else
					break;
			}
#if 0			// 2.5.5. not sure if I want to do this but keep in case
			// Check for e.g. \.argv[0]. (starting in version 2.5.5).
			// BUG: only permits integer \.argv[]. indices
			printf("dotty: '%s'\n",s+i);
			if (dots_in_name == 1) {
				printf("one dot in name '%s'.  checking '%s'\n",s+i,s+i+1);
				if (!strncmp(s + i + 1, ".argv[", 6)) {
					int ends_at = -1;
					for (int ii = i; ii < int(strlen(s)) - 1; ii++) {
						if (s[ii] == ']') {
							if (s[ii + 1] == '.')
								ends_at = ii + 1;
							else
								fatal_err("Cannot parse `\\", s + i, "'", "\\");
							break;
						}
					}
					printf("'%s' it ends at %d, trailer '%s'\n",s+i,ends_at,s+ends_at);
					int the_index;
					if (1 != sscanf(s + i + 1, ".argv[%d].", &the_index)) {
						fatal_err("ERROR: cannot figure out argv[] index at `", s + 1 + i, "'", "\\");
					}
					//printf("YUP.  matches index=%d\n", the_index);
					extern vector<char*> _argv;
					if (the_index < _argv.size()) {
						strcat(sout, _argv[the_index]);
					}
					//strcat(sout, "TEST_OF_ARGV");
					//fprintf(stderr, "argc %d\n",_argv.size());
					i = ends_at + 1;
				}
			}
#endif
		} else if (s[i + 1] == '[') {
			// Indexing a word within synonym
			//printf("Indexing within synonym.\n");
			int index_length = -1;
			int j;
			for (j = i + 2; j < slen; j++) {
				if (s[j] == ']') {
					index_length = j - i - 2;
					break;
				}
			}
			if (index_length == 0) {
				report_num_words = true;
			} else {
				report_a_word = true;
				char *num = new char [index_length + 1];
				for (j = 0; j < index_length; j++) 
					num[j] = s[i + 2 + j];
				num[j] = '\0';
				double tmp;
				getdnum(num, &tmp);
				word_to_report = int(floor(0.5 + tmp));
				printf("'%s' gave %f  ie %d\n",num,tmp,word_to_report);
				delete [] num;
			}
			i += index_length + 2;
			// Check to see if synonym-name has dots in it
			if (s[i + 1] == '.') {
				for(int ii = i + 1; ii < slen; ii++) {
					if (s[ii] == '.')
						dots_in_name++;
					else
						break;
				}
			}
		}
		trailing_dots_in_name = 0;
		if (strlen(s + i) > _grTempStringLEN) {
			fatal_err("Not enough space for string `\\", s + i, "'", "\\");
		}
		strcpy(sname, "\\");
		strcat(sname, s + i + 1);
		// To find length, scan the string, checking characters against
		// stopper characters.
		//
		syn_len = 1; // the backslash
		syn_len += dots_in_name; // perhaps some dots
		while (!end_of_synonym(sname[syn_len], inmath, need_brace)) {
			// Also end synonym if its an unmatched dot
			if (sname[syn_len] == '.') {
				trailing_dots_in_name++;
				if (trailing_dots_in_name > dots_in_name) {
					break;
				} else if (trailing_dots_in_name == dots_in_name) {
					syn_len++;
					break;
				}
			}
			syn_len++;
		}
		if (need_brace)
			syn_len++;
		sname[syn_len] = '\0';
		// Catch '\ ', which is not a synonym, and which can come in by
		// malformed continuation lines
		if (sname[1] == ' ') {
			warning("Found `\\ ', which is not legal; is this a malformed continuation?");
		}
		if (get_syn(sname, svalue)) {
				// Substitute known synonym, then skip over the space the synonym
				// name occupied.
			if (report_num_words) {
				char *w[MAX_nword];
				int nw;
				chop_into_words(svalue, w, &nw, MAX_nword);
				char tmp[30];
				sprintf(tmp, "%d", nw);
				strcat(sout, tmp);
			} else if (report_a_word) {
				char *w[MAX_nword];
				int nw;
				chop_into_words(svalue, w, &nw, MAX_nword);
				
				if (word_to_report < 0) {
					char buf[100];
					sprintf(buf, "\
Cannot get word %d of a synonym; using first word instead",
						word_to_report);
					warning(buf);
					strcat(sout, w[0]);
				} else if (word_to_report < nw) {
					printf("*** Pasting word[%d], i.e. '%s', onto output\n",word_to_report,w[word_to_report]);
					strcat(sout, w[word_to_report]);
				} else {
					char buf[100];
					sprintf(buf, "\
Cannot get word %d of synonym `%s'; using last word ([%d]) instead",
						word_to_report, sname, nw - 1);
					warning(buf);
					strcat(sout, w[nw - 1]);
				}
			} else {
				strcat(sout, svalue);
			}
			i += syn_len - 1;
			report_a_word = false;	// reset
			report_num_words = false;
		} else {
				// Leave unknown synonym in place.
			if (!found) {
				strcat_c(sout, '\\');
				while (!end_of_synonym(s[++i], inmath, need_brace))
					strcat_c(sout, s[i]);
				if (s[i] == '\0')
					strcat_c(sout, '\0');
				i--;		// otherwise miss next char
			}
		}
	}
	// Paste on final blank [can't remember why, but what the heck].
	strcat(sout, " ");
	delete [] sname;
	delete [] svalue;
	return true;
}
#else
// Walk through string, substituting synonyms if not in math mode.
// RETURN 0 if empty line, 1 otherwise.
bool
substitute_synonyms(const char *s, string& sout, bool allow_math)
{
	//printf("--- substitute_synonyms('%s',...)\n",s);
	bool            inmath = false; // are we within a math string?
	int             trailing_dots_in_name = 0;
	int             dots_in_name = 0;
	int             slen = strlen(s);
	char           *sname = new char [1 + slen]; // certainly long enough
	char           *svalue = new char [_grTempStringLEN]; // very long also
	for (int i = 0; i < slen; i++) {
		//printf("DEBUG1: i %d  (%s)\n",i,s+i);
		int             found = 0;
		int             syn_len = 0;
		// If entering or leaving math mode, just paste $ onto the end and
		// skip to the next character.
		//
		if (s[i] == '$' && allow_math) {
			if (i == 0) {
				inmath = true;
			} else if (s[i - 1] != '\\') {	// \$ is escape to pass $
				inmath = inmath ? false : true;
			}
			sout.append("$");
			continue;
		}
		// If not start of synonym, just paste character onto end of string
		// and continue. (This also applies to apparent synonyms, if they are
		// during math mode.)
		if (s[i] != '\\' || inmath) {
			sout += s[i];
			continue;
		}
		//printf("DEBUG ** found backslash.  sout '%s'\n", sout.c_str());

		// Now know that s[i] is backslash, and not inmath.
		// Pass a few escape strings through directly. 
		if (s[i + 1] == '$'
#if 0				// 2.2.3
		    || s[i + 1] == '['
		    || s[i + 1] == ']'
#endif
		    || s[i + 1] == '"'
		    || s[i + 1] == '\\') {
			sout += s[i];
			sout += s[++i];
			continue;
		}
		// Now know that it's the start of a synonym.  Isolate it, then find
		// value.   But first, take note of whether name has a period at the
		// start (since this determines whether a period can be used to end
		// it).
		bool            report_num_words = false;
		bool            report_a_word = false;
		int             word_to_report = -1;
		bool            need_brace = (s[i + 1] == '{');
		//printf("DEBUG1: i %d  (%s)\n",i,s+i);
#if 0
		if (need_brace)
			i++;		// skip the brace itself
#endif
		if (s[i + 1] == '.') {
			int ii;
			for(ii = i + 1; ii < slen; ii++) {
				if (s[ii] == '.')
					dots_in_name++;
				else
					break;
			}
#if 0			// 2.5.5. not sure if I want to do this but keep in case
			// Check for e.g. \.argv[0]. (starting in version 2.5.5).
			// BUG: only permits integer \.argv[]. indices
			printf("dotty: '%s'\n",s+i);
			if (dots_in_name == 1) {
				printf("one dot in name '%s'.  checking '%s'\n",s+i,s+i+1);
				if (!strncmp(s + i + 1, ".argv[", 6)) {
					int ends_at = -1;
					for (int ii = i; ii < int(strlen(s)) - 1; ii++) {
						if (s[ii] == ']') {
							if (s[ii + 1] == '.')
								ends_at = ii + 1;
							else
								fatal_err("Cannot parse `\\", s + i, "'", "\\");
							break;
						}
					}
					printf("'%s' it ends at %d, trailer '%s'\n",s+i,ends_at,s+ends_at);
					int the_index;
					if (1 != sscanf(s + i + 1, ".argv[%d].", &the_index)) {
						fatal_err("ERROR: cannot figure out argv[] index at `", s + 1 + i, "'", "\\");
					}
					//printf("YUP.  matches index=%d\n", the_index);
					extern vector<char*> _argv;
					if (the_index < _argv.size()) {
						sout.append(_argv[the_index]);
					}
					//strcat(sout, "TEST_OF_ARGV");
					//fprintf(stderr, "argc %d\n",_argv.size());
					i = ends_at + 1;
				}
			}
#endif
		} else if (s[i + 1] == '[') {
			// Indexing a word within synonym
			int index_length = -1;
			int j;
			for (j = i + 2; j < slen; j++) {
				if (s[j] == ']') {
					index_length = j - i - 2;
					break;
				}
			}
			if (index_length == 0) {
				report_num_words = true;
			} else {
				report_a_word = true;
				char *num = new char [index_length + 1];
				for (j = 0; j < index_length; j++) 
					num[j] = s[i + 2 + j];
				num[j] = '\0';
				double tmp;
				getdnum(num, &tmp);
				word_to_report = int(floor(0.5 + tmp));
				delete [] num;
			}
			i += index_length + 2;
			// Check to see if synonym-name has dots in it
			if (s[i + 1] == '.') {
				for(int ii = i + 1; ii < slen; ii++) {
					if (s[ii] == '.')
						dots_in_name++;
					else
						break;
				}
			}
		}
		trailing_dots_in_name = 0;
		if (strlen(s + i) > _grTempStringLEN) {
			fatal_err("Not enough space for string `\\", s + i, "'", "\\");
		}
		strcpy(sname, "\\");
		strcat(sname, s + i + 1);
		// To find length, scan the string, checking characters against
		// stopper characters.
		syn_len = 1; // the backslash
		syn_len += dots_in_name; // perhaps some dots
		while (!end_of_synonym(sname[syn_len], inmath, need_brace)) {
			// Also end synonym if its an unmatched dot
			if (sname[syn_len] == '.') {
				trailing_dots_in_name++;
				if (trailing_dots_in_name > dots_in_name) {
					break;
				} else if (trailing_dots_in_name == dots_in_name) {
					syn_len++;
					break;
				}
			}
			syn_len++;
		}
		if (need_brace)
			syn_len++;
		sname[syn_len] = '\0';
		// Catch '\ ', which is not a synonym, and which can come in by
		// malformed continuation lines
		if (sname[1] == ' ') {
			warning("Found `\\ ', which is not legal; is this a malformed continuation?");
		}
		
		if (get_syn(sname, svalue)) {
				// Substitute known synonym, then skip over the space the synonym
				// name occupied.
			if (report_num_words) {
				char *w[MAX_nword];
				int nw;
				chop_into_words(svalue, w, &nw, MAX_nword);
				char tmp[30];
				sprintf(tmp, "%d", nw);
				sout.append(tmp);
			} else if (report_a_word) {
				char *w[MAX_nword];
				int nw;
				chop_into_words(svalue, w, &nw, MAX_nword);
				
				if (word_to_report < 0) {
					char buf[100];
					sprintf(buf, "\
Cannot get word %d of a synonym; using first word instead",
						word_to_report);
					warning(buf);
					sout.append(w[0]);
				} else if (word_to_report < nw) {
					sout.append(w[word_to_report]);
				} else {
					char buf[100];
					sprintf(buf, "\
Cannot get word %d of synonym `%s'; using last word ([%d]) instead",
						word_to_report, sname, nw - 1);
					warning(buf);
					sout.append(w[nw - 1]);
				}
			} else {
				sout.append(svalue);
			}
			i += syn_len - 1;
			report_a_word = false;	// reset
			report_num_words = false;
		} else {
				// Leave unknown synonym in place.
			if (!found) {
				while (!end_of_synonym(s[++i], inmath, need_brace))
					sout += s[i];
				i--;		// otherwise miss next char
			}
		}
	}
	// Paste on final blank [can't remember why, but what the heck].
	sout.append(" ");
	delete [] sname;
	delete [] svalue;
	return true;
}
#endif

static inline int
end_of_synonym(char c, bool inmath, bool need_brace)
{
	if (need_brace)
		return c == '}';
	switch (c) {
	case ' ':
	case '\t':
	case '\n':
	case '`':
	case '\\':
	case '\0':
	case '"':
	case '|':
	case '\'':
	case ':':
	case ';':
	case '@':
	case '#':
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
	case '/':
	case '*':
	case '-':
	case '+':
	case '<':
	case '>':
	case '$':
		return true;
	case ',':
		return (!inmath);
	}
	return false;
}
