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
		sprintf(_grTempString, "    \\\\%-25s = \"%s\"\n", 
			i->getName(), i->getValue());
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
					string tmp;
					unbackslash(name, tmp);
					sprintf(_grTempString, "Warning: synonym `%s' defined but not used\n", tmp.c_str());
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
	// printf("DEBUG get_syn(%s,)\n",name);
	if (!is_syn(name))
		return false;
	if (!strncmp(name, "\\.proper_usage.", 15)) {
		/*
		 * Take care of synonym \.proper_usage., used to demonstrate the
		 * proper usage of a command.
		 */
		string tmp;
		unbackslash(_command[cmd_being_done()].syntax, tmp);
		strcpy(value, tmp.c_str());
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

// Convert single backslashes into double.
void
unbackslash(const char *s, string& res)
{
	res = "";
	char lastc = '\0';
	while (*s != '\0') {
		if (*s == '\\' && lastc != '\\')
			res += '\\';
		res += *s;
		lastc = *s++;
	}
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
	// To speed action, maintain a buffer in which 's' will be copied,
	// prior to chopping into words.  BUG: this buffer is only cleaned
	// up at exit() time, since I never free() it.
	unsigned int space_needed = 1 + strlen(s);
	static char* copy = NULL;
	static unsigned int copy_len = 0;
	if (copy_len == 0) {
		copy_len = space_needed;
		copy = (char*)malloc(copy_len * sizeof(char));
		if (!copy) {
			gr_Error("Out of memory in `substitute_synonyms_cmdline'");
		}
	} else {
		if (copy_len < space_needed) {
			copy_len = space_needed;
			copy = (char*)realloc(copy, copy_len * sizeof(char));
			if (!copy) {
				gr_Error("Out of memory in `substitute_synonyms_cmdline'");
			}
		}
	}
	strcpy(copy, s);

	int             nword;
	chop_into_words(copy, _Words2, &nword, MAX_nword);
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
		return true;
	}
	// Pass `delete \synonym' through directly.
	if (nword == 2 && !strcmp(_Words2[0], "delete") && *_Words2[1] == '\\') {
		return true;
	}
	if (nword < 1) {
		return true;
	}
	// Pass `read \syn ...', `read .var. ...' and `read line \syn' through
	if (strEQ(_Words2[0], "read")) {
		if (nword >= 2
		    && (strEQ(_Words2[1], "*")
			|| is_syn(_Words2[1]) 
			|| is_var(_Words2[1]))) {
			sout = s;
			return true;
		} else if (nword == 3
			   && strEQ(_Words2[1], "line")
			   && is_syn(_Words2[2])) {
			sout = s;
			return true;
		}
	}
	// Pass `while ...' through
	if (!strcmp(_Words2[0], "while")) {
		sout = s;
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
	return substitute_synonyms(s + offset, sout, allow_math);
}

// Walk through string, substituting synonyms if not in math mode.
// RETURN 0 if empty line, 1 otherwise.
bool
substitute_synonyms(const char *s, string& sout, bool allow_math)
{
	if (((unsigned) superuser()) & FLAG_SYN) printf("\n\nsubstitute_synonyms('%s',...)\n",s);
	bool            inmath = false; // are we within a math string?
	int             trailing_dots_in_name = 0;
	int             dots_in_name = 0;
	int             slen = strlen(s);
	string sname;

	// Keep this buffer forever.  BUG: may not be long enough
	static char* svalue = NULL;
	if (svalue == NULL) {
		svalue = (char*)malloc(_grTempStringLEN * sizeof(char));
		if (!svalue) OUT_OF_MEMORY;
	}

	for (int i = 0; i < slen; i++) {
		//printf("DEBUG1: i %d  (%s)\n",i,s+i);
		int             found = 0;
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
		// Catch de-referenced synonyms
		if (s[i + 1] == '@') {
			i += 2;	// skip the '\\' and the '@'
			//printf("DEREF start with <%s>\n", s);
			string tmp("\\");
			while (i < slen && !end_of_synonym(s[i], false /*inmath*/, false/*need_brace*/)) {
				tmp += s[i++];
			}
			//printf("DEREF tmp [%s]\n", tmp.c_str());
			char deref_name[100];
			get_syn(tmp.c_str(), deref_name);
			//printf("this syn value is [%s]\n", deref_name);
			char deref_value[100]; // BUG: size won't be sufficient
			if (deref_name[0] == '\\') {
				if (get_syn(deref_name + 1, deref_value)) {
					sout.append(deref_value);
					//printf("looked up '%s' (after skipping) to get '%s'\n",deref_name,deref_value);
				} else {
					err("Cannot de-reference \\`", deref_name, "'.", "\\");
					return false;
				}
			} else {
				double value = 0.0;
				if (get_var(deref_name, &value)) {
					//printf("OK %d   isvar=%d\n",ok,is_var(deref_name));
					//printf("looked up '%s' to get %f NUM\n",deref_name,value);
					sprintf(deref_value, "%f", value);
					sout.append(deref_value);
				} else {
					err("Cannot de-reference \\`", deref_name, "'.", "\\");
					return false;
				}
			}
			i--;	// BUG: not sure on this!
			continue;
		}
		// Now know that s[i] is backslash, and not inmath.
		// Pass a few escape strings through directly. 
		if (s[i + 1] == '$'
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
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG.  At start of synonym i= %d  s+i= '%s'\n",i,s+i);
		if (s[i + 1] == '.') {
			dots_in_name = 1;
			for(int ii = i + 1; ii < slen; ii++) {
				if (s[ii] == '.')
					dots_in_name++;
				else
					break;
			}
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
				dots_in_name = 1;
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
		// To find length, scan the string, checking characters against
		// stopper characters.
		unsigned int syn_len = 1; // We will want to focus on just the synonym, of course ...
		sname =  "\\";	          // ... and so far we have one character in our list. 
		sname.append(s + i + 1);
		syn_len += dots_in_name; // perhaps some dots
		if (((unsigned) superuser()) & FLAG_SYN) printf("%s:%d about to try to find syn name in '%s'   syn_len= %d  dots_in_name= %d\n", __FILE__, __LINE__, sname.c_str(), syn_len, dots_in_name);
		while (syn_len < sname.size() && !end_of_synonym(sname[syn_len], inmath, need_brace)) {
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
		if (need_brace) {
			syn_len++;
		}
		sname.STRINGERASE(syn_len, sname.size() - syn_len);

		if (((unsigned) superuser()) & FLAG_SYN) printf("  %s:%d the sname is '%s'\n",__FILE__, __LINE__, sname.c_str());

		// Catch '\ ', which is not a synonym, and which can come in by
		// malformed continuation lines
		if (sname[1] == ' ') {
			warning("Found `\\ ', which is not legal; is this a malformed continuation?");
		}
		
		// Substitute known synonym, then skip over the space the synonym
		// name occupied.
		if (get_syn(sname.c_str(), svalue)) {
			if (((unsigned) superuser()) & FLAG_SYN) printf("Syn value is <%s>\n", svalue);
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
#if 0				        // annoying since reports in 'false' parts of if.
					char buf[100];
					sprintf(buf, " Cannot get word %d of a synonym; using first word instead", word_to_report);
					warning(buf);
#endif
					sout.append(w[0]);
				} else if (word_to_report < nw) {
					sout.append(w[word_to_report]);
				} else {
#if 0				        // annoying since reports in 'false' parts of if.
					char buf[100];
					sprintf(buf, "Cannot get word %d of synonym `%s'; using last word ([%d]) instead", word_to_report, sname.c_str(), nw - 1);
					warning(buf);
#endif
					sout.append(w[nw - 1]);
				}
			} else {
				if (((unsigned) superuser()) & FLAG_SYN) printf("    %s:%d the sname value is '%s'\n",__FILE__, __LINE__, svalue);
				sout.append(svalue);
			}
			i += syn_len - 1;
			report_a_word = false;	// reset
			report_num_words = false;
		} else {
			// leave unknown synonym in place.
			if (!found) {
				//printf("\n'%s' WAS NOT FOUND\n", s+i);
				sout += '\\'; // don't forget that!
				while (!end_of_synonym(s[++i], inmath, need_brace))
					sout += s[i];
				i--; // otherwise we'll miss the next character
			}
		}
		dots_in_name = 0; // reset
	}
	// Paste on final blank [can't remember why, but what the heck].
	sout.append(" ");
	if (((unsigned) superuser()) & FLAG_SYN) printf("Finally [%s]\n",sout.c_str());
	return true;
}

static inline int
end_of_synonym(char c, bool inmath, bool need_brace)
{
	if (((unsigned) superuser()) & FLAG_SYN) printf("\tend_of_synonym (%c,%d)\n",c, need_brace);
	if (need_brace) {
		if (((unsigned) superuser()) & FLAG_SYN) printf("\t\t returning %d  (since need_brace)\n", c == '}');
		return c == '}';
	}
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
	case '#':
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
	case '/':
	case '*':
	case '@':
	case '-':
	case '+':
	case '<':
	case '>':
	case '=':
	case '$':
		if (((unsigned) superuser()) & FLAG_SYN) printf("\t\t returning TRUE place 2\n");
		return true;
	case ',':
		if (((unsigned) superuser()) & FLAG_SYN) printf("\t\t returning %d at place 3\n", !inmath);
		return (!inmath);
	}
	if (((unsigned) superuser()) & FLAG_SYN) printf("\t\t returning FALSE at place 4\n");
	return false;
}
