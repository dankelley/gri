/* #define DEBUG_SYNONYMS */
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "Synonym.hh"


vector<GriSynonym> synonymStack;
vector<int> synonymPointer;	// used for e.g. \sp = &\\s

static inline int end_of_synonym(char c, bool inmath, bool need_brace);
static bool get_starred_synonym(const char* name, bool want_value/*or name*/, string& result);


static bool
get_starred_synonym(const char* name, bool want_value/*or name*/, string& result)
{
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d in get_starred_synonym(%s,%c)\n",__FILE__,__LINE__,name,want_value?'T':'F');
	string coded_reference;
	get_syn(name, coded_reference);
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d coded_reference <%s>\n",__FILE__,__LINE__,coded_reference.c_str());
	if (!strncmp(coded_reference.c_str(), "\\#v", 3)) {
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d A VAR in <%s>\n",__FILE__,__LINE__,coded_reference.c_str());
		int var_index;
		sscanf(coded_reference.c_str(), "\\#v%d#", &var_index);
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d var_index %d\n",__FILE__,__LINE__,var_index);
		if (var_index < 0 || var_index > int(variablePointer.size())) {
			err("Internal error with variable stack");
			return false;
		}
		int which = variablePointer[var_index];
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d var ptr is %d\n",__FILE__,__LINE__,which);
		if (which > -1) {
			if (want_value) {
				char buffer[100];
				sprintf(buffer, "%g", variableStack[which].getValue());
				result.assign(buffer);
			} else {
				result.assign(variableStack[which].getName());
			}
		} else {
			result.assign(coded_reference);
		}
	} else if (!strncmp(coded_reference.c_str(), "\\#s", 3)) {
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d A SYN in <%s>\n",__FILE__,__LINE__,coded_reference.c_str());
		int syn_index;
		sscanf(coded_reference.c_str(), "\\#s%d#", &syn_index);
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d syn_index %d\n",__FILE__,__LINE__,syn_index);
		if (syn_index < 0 || syn_index > int(synonymPointer.size())) {
			err("Internal error with synonym stack");
			return false;
		}
		int which = synonymPointer[syn_index];
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d syn ptr is %d\n",__FILE__,__LINE__,which);
		if (which > -1) {
			if (want_value) {
				result.assign(synonymStack[which].getValue());
			} else {
				result.assign(synonymStack[which].getName());
			}
		} else {
			result.assign(coded_reference);
		}
	} else {
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d HUH?? no idea what <%s> is\n",__FILE__,__LINE__,coded_reference.c_str());
		result.assign(coded_reference);
	}
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d get_starred_synonym returning <%s>\n",__FILE__,__LINE__,result.c_str());
	return true;
}


// Get index of synonym
// RETURN non-negative integer if 'name' is an existing synonym, or -1 if not.
int
index_of_synonym(const char *name)
{
	if (!is_syn(name))
		return -1;
	unsigned int stackLen = synonymStack.size();
	if (stackLen > 0) {
		for (int i = stackLen - 1; i >= 0; i--) {
			//printf("debug: check [%s] vs %d-th [%s]\n", name, i, synonymStack[i].getName());
			if (!strcmp(name, synonymStack[i].getName())) {
				//printf("DEBUG: returning index %d\n", i);
				return i;
			}
		}
	}
	return -1;
}

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
	int n = synonymStack.size();
	for (int i = 0; i < n; i++) {
		extern char _grTempString[];
		sprintf(_grTempString, "%3d:    %-25s = \"%s\"\n", i, synonymStack[i].getName(), synonymStack[i].getValue());
		ShowStr(_grTempString);
		have_some = true;
	}
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
bool
is_syn(const string& name)
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
delete_syn(const string& name)
{
	unsigned stackLen = synonymStack.size();
	for (int i = stackLen - 1; i >= 0; i--) {
		if (name == synonymStack[i].getName()) {
			int Plen = synonymPointer.size();
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d DELETING syn %d named <%s>\n",__FILE__,__LINE__,i,name.c_str());
			if (((unsigned) superuser()) & FLAG_SYN) for (int ip = 0; ip < Plen; ip++) printf("DEBUG: BEFORE %d <%s>\n", synonymPointer[ip], synonymStack[synonymPointer[ip]].getName());
			for (unsigned j = i; j < stackLen - 1; j++)
				synonymStack[j] = synonymStack[j + 1];
			synonymStack.pop_back();
			for (int ip = 0; ip < Plen; ip++) {
				if (synonymPointer[ip] > i) {
					synonymPointer[ip]--;
				} else if (synonymPointer[ip] == i) {
					synonymPointer[ip] = -1; // missing
				}
			}
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d after handling 'delete syn', the list is...\n",__FILE__,__LINE__);
			if (((unsigned) superuser()) & FLAG_SYN) for (int ip = 0; ip < Plen; ip++) printf("DEBUG: AFTER %d <%s>\n", synonymPointer[ip], synonymStack[synonymPointer[ip]].getName());
			return true;
		}
	}
	return false;
}

// get_syn() - get value of synonym Sufficient storage for (char *value) must
// have been set aside by the calling routine.
// RETURN true if synonym is defined and has a value
bool
get_syn(const char *name, string& value)
{
	// printf("DEBUG get_syn(%s,)\n",name);
	if (!is_syn(name))
		return false;
	if (!strncmp(name, "\\.proper_usage.", 15)) {
		// Take care of synonym \.proper_usage., used to demonstrate the
		// proper usage of a command.
		unbackslash(_command[cmd_being_done()].syntax, value);
		return true;
	} else if (!strncmp(name, "\\.word", 6) && strlen(name) > 6) {
		// Take care of the synonym list for the command being processed.
		// This list has synonyms \.words. (number of words in last level
		// command) and \.word0., \.word1., etc (the words in last level
		// command).
		// 
		// This list is not stored in the normal synonym stack, but is created
		// here using the word stack is maintained in command.c.
		// 
		// Here is what the command stack will look like, for the command `New
		// Draw TS Line \rho' within a new gri command named `Draw
		// Isopycnals' which was called with the line `New Draw TS Line
		// 26.00':
		// 
		// -----
		// 
		// Draw Isopycnals
		// 
		// -----
		// 
		// New Draw TS Line 26.00
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
						char value_buffer[100];
						sprintf(value_buffer, "%d", count);
						value.assign(value_buffer);
						return true;
					}
				}
			} else {
				value.assign("\\.words.");
			}
			return true;
		} else if (1 == sscanf(name + 6, "%d", &i)) {
			// The synonym \.wordn. is the n-th word in the command.
			if (_num_command_word == 0) {
				value.assign(name);
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
			// If the command is requesting a word beyond the list (e.g.
			// \.word10. in a line with only 3 words) just paste the name of
			// the desired synonym (e.g "\.word10.") onto the line, and let
			// further processing catch the error, if it is in a bit of code
			// that is not being skipped.
			if (cmd >= _num_command_word) {
				value.assign(name);
			} else {
				// The word does exist.  If it's a quoted string, remove the
				// quotes ...
				if (*_command_word[cmd] == '"') {
					value.assign(1 + _command_word[cmd]);
					if (value[value.size() - 1] == '"') {
						value.STRINGERASE(value.size() - 1, 1);
					}
				} else {
					// ... otherwise copy it directly
					value.assign(_command_word[cmd]);
				}
			}
			return true;		// .word#.
		} else {
			value.assign(name);
			return true;
		}
	} else {
		// It's an ordinary synonym.  Look it up in the stack
		int i;
		unsigned int stackLen = synonymStack.size();
		if (stackLen > 0) {
			for (i = stackLen - 1; i >= 0; i--) {
				if (!strcmp(name, synonymStack[i].getName())) {
					value.assign(synonymStack[i].getValue());
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
	// In the case where instructed by the value of 
	// replace_existing, simply replace the value of
	// the variable (if it already exists).
	if (replace_existing) {
		unsigned stackLen = synonymStack.size();
		if (stackLen) {
			for (int i = stackLen - 1; i >= 0; i--) {
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

	// Catch e.g. \@.word1 = ...
	if (!strncmp(_Words2[0], "\\@", 2) && nword > 1 && is_assignment_op(_Words2[1])) {
		string tmp("\\");
		tmp.append(2 + _Words2[0]);
		string unaliased;
		//printf("tmp <%s>\n",tmp.c_str());
		get_syn(tmp.c_str(), unaliased);
		//printf("unaliased <%s>\n", unaliased.c_str());
		if (unaliased[0] == '\\') {
			//printf("SYN.\n");
			sout.append(unaliased.c_str());
			sout.append(" "); 
			offset = 1 + strlen(_Words2[0]);
		} else if (unaliased[0] == '.') {
			//printf("VAR.\n");
			sout.append(unaliased.c_str());
			sout.append(" "); 
			offset = 1 + strlen(_Words2[0]);
		} else {
			// Leave in place to worry about later
			sout.append(_Words2[0]);
			sout.append(" ");
			offset = 1 + strlen(_Words2[0]);
		}
	} else if (*_Words2[0] == '\\') {
		// Protect first word of `\name = "value"', but not of `\syn ...'. In
		// other words, protect first word if matches \synonym[ ]*=.*
		if (nword > 1 && !strcmp(_Words2[1], "=")) {
			sout = _Words2[0];
			sout.append(" ");
			offset = 1 + strlen(_Words2[0]);
		}
	}

	// Catch *\name = something
	if (*_Words2[0] == '*' || *(1 + _Words2[0]) == '\\') {
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d NEED CODE HERE!\n",__FILE__,__LINE__);
		if (nword > 1 && !strcmp(_Words2[1], "=")) {
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d item is <%s>\n",__FILE__,__LINE__,_Words2[0]);
			string syn_value(1 + _Words2[0]);
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d looking up ref from <%s>\n",__FILE__,__LINE__,syn_value.c_str()); 
			string pointed_to_name;
			bool res = get_starred_synonym(syn_value.c_str(), false, pointed_to_name);
			if (res) {
				if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d pointed_to_name <%s>\n",__FILE__,__LINE__,pointed_to_name.c_str());
				sout.append(pointed_to_name);
				sout.append(" ");
			} else {
				sout.append(_Words2[0]);
				sout.append(" ");
			}
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
		//printf("%s:%d  i=%d     s+i = [%s]\n",__FILE__,__LINE__,i,s+i);
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
			//printf("    DONE appending 1\n");
			continue;
		}
		
		// If it is the &\ syntax, pass it through directly
		if (s[i] == '&' && i < slen - 2 && s[i + 1] == '\\' && s[i + 2] != '\\') {
			sout.append("&\\");
			i++;
			continue;
		}
		
		// See if it is the *\syn syntax, and handle if so.
		if (s[i] == '*' && i < slen - 2 && s[i + 1] == '\\' && s[i + 2] != '\\') {
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d got * i=%d <%s> <%s>\n",__FILE__,__LINE__,i,s,s+i-1);
			i += 2;	// skip ahead, looking for synonym name.
			string tmp("\\");
			while (i < slen && !end_of_synonym(s[i], false /*inmath*/, false/*need_brace*/)) {
				tmp += s[i++];
			}
			string coded_reference;
			get_syn(tmp.c_str(), coded_reference);
			string value;
			get_starred_synonym(tmp.c_str(), true, value);
			sout.append(value);
		}
		
		// If not start of synonym, just paste character onto end of string
		// and continue. (This also applies to apparent synonyms, if they are
		// during math mode.)
		if (s[i] != '\\' || inmath) {
			sout += s[i];
			continue;
		}

		// We now know that s[i] is a '\\' and now must 
		// investigate further.  There are several possibilities
		// depending on what the previous character was
		// and what the next character is.


		// Catch \@ [alias synonyms]
		if (s[i + 1] == '@') {
			i += 2;	// skip the '\\' and the '@'
			//printf("ALIAS start with <%s>\n", s);
			string tmp("\\");
			while (i < slen && !end_of_synonym(s[i], false /*inmath*/, false/*need_brace*/)) {
				tmp += s[i++];
			}
			//printf("ALIAS tmp [%s]\n", tmp.c_str());
			string alias_name;
			get_syn(tmp.c_str(), alias_name);
			//printf("this syn value is [%s]\n", alias_name);
			string alias_value;
			if (alias_name[0] == '\\') {
				if (get_syn(alias_name.substr(1, alias_name.size()).c_str(), alias_value)) {
					sout.append(alias_value);
					//printf("DEBUG %s:%d looked up '%s' (after skipping) to get '%s'\n",__FILE__,__LINE__,alias_name,alias_value);
				} else {
					err("Cannot un-alias `\\", alias_name.c_str(), "'.", "\\");
					return false;
				}
			} else if (alias_name[0] == '.') {
				double value = 0.0;
				if (get_var(alias_name.c_str(), &value)) {
					//printf("OK %d   isvar=%d\n",ok,is_var(alias_name));
					//printf("looked up '%s' to get %f NUM\n",alias_name,value);
					char alias_value_buffer[100];
					sprintf(alias_value_buffer, "%f", value);
					sout.append(alias_value_buffer);
				} else {
					err("Cannot un-alias `\\", alias_name.c_str(), "'.", "\\");
					return false;
				}
			} else {
				err("The purported alias `\\", alias_name.c_str(), "' doesn't name a synonym or a variable.", "\\");
				return false;
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
			for(int ii = i + 2; ii < slen; ii++) {
				if (s[ii] == '.')
					dots_in_name++;
				else
					break;
			}
			//printf("DEBUG dots_in_name %d      <%s>\n",dots_in_name,s+i);
		} else if (s[i + 1] == '#') {
			printf("DEBUG %s:%d got a # 1 character into <%s>\n",__FILE__,__LINE__,s+i);
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
		string synonym_value;
		if (get_syn(sname.c_str(), synonym_value)) {
			if (synonym_value.size() > _grTempStringLEN - 1) {
				OUT_OF_MEMORY;
			}
			strcpy(svalue, synonym_value.c_str());
			if (((unsigned) superuser()) & FLAG_SYN) printf("Syn value is <%s>\n", svalue);
			if (report_num_words) {
				char *w[MAX_nword];
				int nw;
				chop_into_words(svalue, w, &nw, MAX_nword);
				char tmp[30];
				sprintf(tmp, "%d", nw);
				sout.append(tmp);
			} else if (report_a_word) {
				char *w[MAX_nword]; // BUG: wasteful
				int nw;
				chop_into_words(svalue, w, &nw, MAX_nword);
				if (word_to_report < 0) {
					; // nothing to do here
				} else if (word_to_report < nw) {
					sout.append(w[word_to_report]);
				} else {
					; // nothing to do here
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
	//if (((unsigned) superuser()) & FLAG_SYN) printf("\tend_of_synonym (%c,%d)\n",c, need_brace);
	if (need_brace) {
		//if (((unsigned) superuser()) & FLAG_SYN) printf("\t\t returning %d  (since need_brace)\n", c == '}');
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
	//if (((unsigned) superuser()) & FLAG_SYN) printf("\t\t returning FALSE at place 4\n");
	return false;
}
