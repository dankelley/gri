/* #define DEBUG_SYNONYMS */
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "Synonym.hh"


std::vector<GriSynonym> synonymStack;

static inline int end_of_synonym(char c, bool inmath, bool need_brace);
//static bool get_starred_synonym(const char* name, bool want_value/*or name*/, string& result);
static int get_num_cmdwords();
static int find_synonym_name(const std::string& s, std::string& name, bool in_math);
bool get_cmdword(unsigned int index, std::string& cmdword);



static int
get_num_cmdwords()
{
	extern int      _num_command_word;
	if (_num_command_word <= 0)
		return 0;
	extern char    *_command_word[MAX_cmd_word];
	extern char    *_command_word_separator;
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
			return count;
		}
	}
	return 0;
}

bool
get_cmdword(unsigned int index, std::string& cmdword)
{
	extern int      _num_command_word;
	extern char    *_command_word[MAX_cmd_word];
	extern char    *_command_word_separator;

	// The synonym \.wordn. is the n-th word in the command.
	if (_num_command_word == 0)
		return false;
	int cmd, level = 0;
	// Trace back through the stack until at next level deep, then
	// move forward to indicated word.
	for (cmd = _num_command_word - 1; cmd > -1; cmd--) {
		//printf("DEBUG %d  <%s>  <%s>\n",cmd,_command_word[cmd],_command_word_separator);
		if (!strcmp(_command_word[cmd], _command_word_separator))
			level++;
		if (level == 1 || cmd <= 0) {
			cmd += index + 1;
			break;
		}
	}
	// If the command is requesting a word beyond the list (e.g.
	// \.word10. in a line with only 3 words) just paste the name of
	// the desired synonym (e.g "\.word10.") onto the line, and let
	// further processing catch the error, if it is in a bit of code
	// that is not being skipped.
	if (cmd >= _num_command_word) {
		return false;
	} else {
                // The word does exist.  If it's a quoted string, remove the
                // quotes ...
		if (*_command_word[cmd] == '"') {
			cmdword.assign(1 + _command_word[cmd]);
			if (cmdword[cmdword.size() - 1] == '"') {
				cmdword.STRINGERASE(cmdword.size() - 1, 1);
			}
		} else {
			// ... otherwise copy it directly
			cmdword.assign(_command_word[cmd]);
		}
	}
	//printf("DEBUG %s:%d level= %d  cmd= %d  res <%s>\n",__FILE__,__LINE__,level,cmd,_command_word[cmd]);
	return true;		// .word#.
}

#if 0
static bool
get_starred_synonym(const char* name, bool want_value/*or name*/, std::string& result)
{
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d in get_starred_synonym(%s,%c)\n",__FILE__,__LINE__,name,want_value?'T':'F');
	std::string coded_reference;
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
				sprintf(buffer, "%g", variableStack[which].get_value());
				result.assign(buffer);
			} else {
				result.assign(variableStack[which].get_name());
			}
		} else {
			result.assign(coded_reference);
		}
	} else if (!strncmp(coded_reference.c_str(), "\\#s", 3)) {
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d A SYN in <%s>\n",__FILE__,__LINE__,coded_reference.c_str());
		int syn_index;
		sscanf(coded_reference.c_str(), "\\#s%d#", &syn_index);
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d syn_index %d\n",__FILE__,__LINE__,syn_index);
		if (syn_index < 0) {
			err("Internal error with synonym stack");
			return false;
		}
		if (which > -1) {
			if (want_value) {
				result.assign(synonymStack[which].get_value());
			} else {
				result.assign(synonymStack[which].get_name());
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
#endif

// Get index of synonym
// RETURN non-negative integer if 'name' is an existing synonym, or -1 if not.
int
index_of_synonym(const char *name, int mark)
{
	if (!is_syn(name))
		return -1;
	unsigned int stackLen = synonymStack.size();
	if (mark == -1) {
		for (int i = stackLen - 1; i >= 0; i--)
			if (!strcmp(name, synonymStack[i].get_name()))
				return i;
		return -1;
	} else {
		int mark_above = mark + 1;
		unsigned int index;
		int this_mark = 0;
		for (index = 0; index < stackLen; index++) {
			const char *n = synonymStack[index].get_name();
			if (*n == '\0')
				if (++this_mark == mark_above)
					break;
		}
		if (this_mark != mark_above) {
			//printf("DEBUG %s:%d no match for <%s>\n",__FILE__,__LINE__,name);
			return -1;
		}
		//printf("DEBUG %s:%d index %d\n",__FILE__,__LINE__,index);
		for (int i = index - 1; i >= 0; i--) {
			//printf("check <%s> to see if <%s>\n",synonymStack[i].get_name(),name);
			if (!strcmp(synonymStack[i].get_name(), name)) {
				return i;
			}
		}
		return -1;
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
	unsigned int stackLen = synonymStack.size();
	for (unsigned int i = 0; i < stackLen; i++) {
		const char *n = synonymStack[i].get_name();
		if (*n == '\0') {
			printf("    ------------------------------------------------\n");
		} else {
			extern char _grTempString[];
			sprintf(_grTempString, "    %-25s = \"%s\"\n", n, synonymStack[i].get_value());
			ShowStr(_grTempString);
			have_some = true;
		}
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
				name = strdup(synonymStack[i].get_name());
				if (strlen(name) > 0 && *(name + 1) != '.') {
					std::string tmp;
					unbackslash(name, tmp);
					sprintf(_grTempString, "Warning: synonym `%s' defined but not used\n", tmp.c_str());
					ShowStr(_grTempString);
				}
				free(name);
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
is_syn(const std::string& name)
{
	return ((name[0] == '\\') ? true : false);
}

void
show_syn_stack()
{
	unsigned int stackLen = synonymStack.size();
	if (stackLen > 0) {
		printf("Synonym stack [\n");
		for (unsigned int i = 0; i < stackLen; i++) {
			const char *n = synonymStack[i].get_name();
			if (*n == '\0')
				printf("    ------------------------------------------------\n");
			else
				printf("%3d  %s = \"%s\"\n", i, n, synonymStack[i].get_value());
		}
		printf("]\n");
	}
}

// delete_syn() - delete synonym
bool
delete_syn(const std::string& name)
{
	unsigned stackLen = synonymStack.size();
	for (int i = stackLen - 1; i >= 0; i--) {
		if (name == synonymStack[i].get_name()) {
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d DELETING syn %d named <%s>\n",__FILE__,__LINE__,i,name.c_str());
			for (unsigned j = i; j < stackLen - 1; j++)
				synonymStack[j] = synonymStack[j + 1];
			synonymStack.pop_back();
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d after handling 'delete syn', the list is...\n",__FILE__,__LINE__);
			return true;
		}
	}
	return false;
}

// get_syn() - get value of synonym Sufficient storage for (char *value) must
// have been set aside by the calling routine.
// RETURN true if synonym is defined and has a value
bool
get_syn(const char *name, std::string& value, bool do_decoding)
{
        unsigned int name_len = strlen(name);
	//printf("DEBUG %s:%d get_syn(%s,)\n",__FILE__,__LINE__,name);
	if (!is_syn(name))
		return false;
	int word_index;
	if (!strncmp(name, "\\.proper_usage.", 15)) {
		// Take care of synonym \.proper_usage., used to demonstrate the
		// proper usage of a command.
		unbackslash(_command[cmd_being_done()].syntax, value);
		return true;
	} else if (!strcmp(name, "\\.words.")) {
		int nc = get_num_cmdwords();
		if (nc <= 0) {
			value.assign("\\.words.");
			return false;
		} else {
			char value_buffer[100];
			sprintf(value_buffer, "%d", nc);
			value.assign(value_buffer);
			return true;
		}
	} else if (1 == sscanf(name, "\\.word%d.", &word_index)) {
		if (!get_cmdword(word_index, value)) {
			value.assign(name);
			return false;
		}
		if (do_decoding) {
			std::string coded_name;
			int coded_level = -1;
			if (is_coded_string(value, coded_name, &coded_level)) {
				//printf("DEBUG %s:%d cmdword[%d]='%s' was encoded `%s' at level %d\n",__FILE__,__LINE__,word_index, value.c_str(), coded_name.c_str(), coded_level);
				get_coded_value(coded_name, coded_level, value);
			}
		}
		return true;		// .word#.
	} else if (name_len > 1 && name[1] == '[') { // word within synonym (e.g. \[0]syn)
		//printf("DEBUG %s:%d word within synonym [%s]\n",__FILE__,__LINE__,name);
		int index_len = -1;
		for (unsigned int i = 2; i < name_len; i++) {
			if (name[i] == ']') {
				index_len = i - 2;
				break;
			}
		}
		if (index_len == -1) { // malformed; no closing ']'
			value.assign(name);
			return false;
		}
		std::string name_unindexed = name;
		name_unindexed.STRINGERASE(1, 2 + index_len);
		//printf("name_unindexed [%s]\n", name_unindexed.c_str());
		
		int the_index = 0;
		if (index_len) {
			std::string the_index_s = name;
			double tmp;
			if (!getdnum(the_index_s.substr(2, index_len).c_str(), &tmp)) {
				value.assign(name);
				return false;
			}
			the_index = int(floor(0.5 + tmp));
			if (the_index < 0) {
				value.assign(name);
				return false;
			}
		}

		// Check for e.g. \[0].word1.
		int which_word;
		if (1 == sscanf(name_unindexed.c_str(), "\\.word%d.", &which_word)) {
			//printf("DEBUG %s:%d which_word %d\n",__FILE__,__LINE__,which_word);
			std::string word_buf;
			if (!get_cmdword(which_word, word_buf)) {
				value.assign(name);
				return false;
			}
			//printf("DEBUG %s:%d word_buf [%s]\n",__FILE__,__LINE__,word_buf.c_str());
			if (word_buf[0] == '"') {
				word_buf.STRINGERASE(0, 1);
				if (word_buf[word_buf.size() - 1] == '"') {
					word_buf.STRINGERASE(word_buf.size() - 1, 1);
				}
			}
			//printf("DEBUG %s:%d word_buf [%s] the_index= %d\n",__FILE__,__LINE__,word_buf.c_str(), the_index);
			if (index_len) {
				if (get_nth_word(word_buf, the_index, value))
					return true;
			} else {
				char num[30];
				sprintf(num, "%d", get_number_of_words(word_buf));
				value.assign(num);
				return true;
			}
		}

		// Now know it's not of the form "\[0].word#." or "\[].word#.", etc.
		// It must be of the form "\[]syn" or "\[0]syn", etc.
		unsigned int stackLen = synonymStack.size();
		for (int ii = stackLen - 1; ii >= 0; ii--) {
			if (!strcmp(name_unindexed.c_str(), synonymStack[ii].get_name())) {
				if (index_len) {
					if (get_nth_word(synonymStack[ii].get_value(), the_index, value)) {
						;
					} else {
						value.assign(name);
						return true;
					}
				} else {
					char num[30];
					sprintf(num, "%d", get_number_of_words(synonymStack[ii].get_value()));
					value.assign(num);
				}
				synonymStack[ii].incrementCount();
				return true;
			}
		}
	} else {
		// It's an ordinary synonym.  Look it up in the stack
		unsigned int stackLen = synonymStack.size();
		if (stackLen > 0) {
			for (int i = stackLen - 1; i >= 0; i--) {
				if (!strcmp(name, synonymStack[i].get_name())) {
					value.assign(synonymStack[i].get_value());
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
unbackslash(const char *s, std::string& res)
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
				if (!strcmp(name, synonymStack[i].get_name())) {
					synonymStack[i].set_value(value);
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
substitute_synonyms_cmdline(const char *s, std::string& sout, bool allow_math)
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

	unsigned int nword;
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
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d caught \\@ with first cmdword being [%s]\n",__FILE__,__LINE__,_Words2[0]);
		std::string tmp("\\");
		tmp.append(2 + _Words2[0]);
		std::string unaliased;
		get_syn(tmp.c_str(), unaliased);
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d unaliased <%s>\n",__FILE__,__LINE__,unaliased.c_str());
		if (unaliased[0] == '\\') {
			// Not sure on the below.  Cropped up 2001-feb-15.
			if (unaliased[1] == '\\') {
				sout.append(1 + unaliased.c_str());
			} else {
				sout.append(unaliased.c_str());
			}
			sout.append(" "); 
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d SYN named [%s]\n",__FILE__,__LINE__,sout.c_str());
			offset = 1 + strlen(_Words2[0]);
		} else if (unaliased[0] == '.') {
			sout.append(unaliased.c_str());
			sout.append(" "); 
			offset = 1 + strlen(_Words2[0]);
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d VAR name [%s]\n",__FILE__,__LINE__,unaliased.c_str());
		} else {
			// Leave in place to worry about later
			sout.append(_Words2[0]);
			sout.append(" ");
			offset = 1 + strlen(_Words2[0]);
		}
	} else if (*_Words2[0] == '\\') {
		// Protect first word of `\name = "value"', but not of `\syn ...'. In
		// other words, protect first word if matches \synonym[ ]*=.*
		if (nword > 1 && is_assignment_op(_Words2[1])) {
			sout = _Words2[0];
			sout.append(" ");
			offset = 1 + strlen(_Words2[0]);
		}
	}

#if 0
	// Catch *\name = something
	if (*_Words2[0] == '*' || *(1 + _Words2[0]) == '\\') {
		if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d NEED CODE HERE!\n",__FILE__,__LINE__);
		if (nword > 1 && !strcmp(_Words2[1], "=")) {
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d item is <%s>\n",__FILE__,__LINE__,_Words2[0]);
			std::string syn_value(1 + _Words2[0]);
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d looking up ref from <%s>\n",__FILE__,__LINE__,syn_value.c_str()); 
			std::string pointed_to_name;
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
#endif

	return substitute_synonyms(s + offset, sout, allow_math);
}

// Walk through string, substituting synonyms if not in math mode.
// RETURN 0 if empty line, 1 otherwise.
bool
substitute_synonyms(const char *s, std::string& sout, bool allow_math)
{
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d substitute_synonyms('%s',...)\n",__FILE__,__LINE__,s);
	bool            inmath = false; // are we within a math string?
	int             slen = strlen(s);
	std::string sname;

	// Keep this buffer forever.  BUG: may not be long enough
	static char* svalue = NULL;
	if (svalue == NULL) {
		svalue = (char*)malloc(_grTempStringLEN * sizeof(char));
		if (!svalue) OUT_OF_MEMORY;
	}

	for (int i = 0; i < slen; i++) {
		//printf("%s:%d  i=%d     s+i = [%s]\n",__FILE__,__LINE__,i,s+i);
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
		
#if 0
		// See if it is the *\syn syntax, and handle if so.
		if (s[i] == '*' && i < slen - 2 && s[i + 1] == '\\' && s[i + 2] != '\\') {
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d got * i=%d <%s> <%s>\n",__FILE__,__LINE__,i,s,s+i-1);
			i += 2;	// skip ahead, looking for synonym name.
			std::string tmp("\\");
			while (i < slen && !end_of_synonym(s[i], false /*inmath*/, false/*need_brace*/)) {
				tmp += s[i++];
			}
			std::string coded_reference;
			get_syn(tmp.c_str(), coded_reference);
			std::string value;
			get_starred_synonym(tmp.c_str(), true, value);
			sout.append(value);
		}
#endif
		
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

		// Translate two backslashes into one backslash
		if (s[i + 1] == '\\') {
			sout += '\\';
			i++;
			continue;
		}

#if 1
		// Catch \& syntax
		if (s[i + 1] == '&') {
			bool want_name = true; // or level, for \&&
			int iS = i + 2;
			if (s[i + 2] == '&') {
				want_name = false;
				iS++;
			}
			//printf("DEBUG %s:%d got & syntax on <%s>\n",__FILE__,__LINE__, s);
			std::string S("\\");
			while (iS < slen && !end_of_synonym(s[iS], false /*inmath*/, false/*need_brace*/)) {
				S += s[iS++];
			}
			int word_index;
			if (1 == sscanf(S.c_str(), "\\.word%d.", &word_index)) {
				//printf("A WORd %d\n", word_index);
				std::string value;
				if (get_cmdword(word_index, value)) {
					//printf("WORD IS [%s]\n", value.c_str());
					std::string coded_name;
					int coded_level;
					if (is_coded_string(value, coded_name, &coded_level)) {
						//printf("CODED. [%s]\n",coded_name.c_str());
						if (want_name) {
							//printf("SHOULD STORE [%s]\n", coded_name.c_str());
							sout.append(coded_name.c_str());
						} else {
							char buf[20]; // BUG: should be big enough
							sprintf(buf, "%d", coded_level);
							sout.append(buf);
						}
					}
				}
			}
			i = iS - 1;
			continue;
		}
#endif

#if 1
		// Catch \@ [alias synonyms]
		if (s[i + 1] == '@') {
			i += 2;	// skip the '\\' and the '@'
			std::string tmp("\\");
			while (i < slen && !end_of_synonym(s[i], false /*inmath*/, false/*need_brace*/)) {
				tmp += s[i++];
			}
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d ALIAS tmp [%s]\n",__FILE__,__LINE__,tmp.c_str());
			std::string alias_name;
			get_syn(tmp.c_str(), alias_name);
			if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d this syn value is [%s]\n",__FILE__,__LINE__,alias_name.c_str());
			std::string alias_value;
			if (alias_name[0] == '\\') {
				if (get_syn(alias_name.c_str(), alias_value)) {
					sout.append(alias_value);
					if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d looked up '%s' (after skipping) to get '%s'\n",__FILE__,__LINE__,alias_name.c_str(),alias_value.c_str());
				} else {
					if (!skipping_through_if()) {
						err("Cannot un-alias synonym in `\\", alias_name.c_str(), "'.", "\\");
						return false;
					}
				}
			} else if (alias_name[0] == '.') {
				double value = 0.0;
				if (get_var(alias_name.c_str(), &value)) {
					if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d isvar=%d\n",__FILE__,__LINE__,is_var(alias_name));
					if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d looked up '%s' to get %f NUM\n",__FILE__,__LINE__,alias_name.c_str(),value);
					char alias_value_buffer[100];
					sprintf(alias_value_buffer, "%f", value);
					sout.append(alias_value_buffer);
				} else {
					if (!skipping_through_if()) {
						err("Cannot un-alias variable in `\\", alias_name.c_str(), "'.", "\\");
						return false;
					}
				}
			} else {
				err("The purported alias `\\", alias_name.c_str(), "' doesn't name a synonym or a variable.", "\\");
				return false;
			}
			i--;	// BUG: not sure on this!
			continue;
		}
#endif

		// Now know that s[i] is backslash, and not inmath.
		// Pass a few escape strings through directly. 
		if (s[i + 1] == '$'
		    || s[i + 1] == '"'
		    || s[i + 1] == '\\') {
			sout += s[i];
			sout += s[++i];
			continue;
		}

		// Now know that it's the start of a synonym.  Isolate it,
		// then find value.
		sname =  "\\";
		sname.append(s + i + 1);
		std::string the_syn_name;
		int syn_name_len = find_synonym_name(sname, the_syn_name, inmath);
		if (syn_name_len != 0) {
			sname = the_syn_name;
			i += syn_name_len - 1;
		}

		if (((unsigned) superuser()) & FLAG_SYN) printf("  %s:%d the sname is '%s'\n",__FILE__, __LINE__, sname.c_str());

#if 0
		// Catch '\ ', which is not a synonym, and which can come in by
		// malformed continuation lines
		if (sname[1] == ' ') {
			warning("Found `\\ ', which is not legal; is this a malformed continuation?");
		}
#endif

		// Substitute known synonym, then skip over the space the synonym
		// name occupied.
		std::string synonym_value;
		if (get_syn(sname.c_str(), synonym_value)) {
			if (synonym_value.size() > _grTempStringLEN - 1) {
				OUT_OF_MEMORY;
			}
			strcpy(svalue, synonym_value.c_str());
			if (((unsigned) superuser()) & FLAG_SYN) printf("Syn value is <%s>\n", svalue);
			sout.append(synonym_value.c_str());
		} else {
			sout.append(sname.c_str()); 
		}
	}
	// Paste on final blank [can't remember why, but what the heck].
	sout.append(" ");
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d finally [%s]\n",__FILE__,__LINE__,sout.c_str());
	return true;
}

static int			// return length
find_synonym_name(const std::string &s, std::string& name, bool inmath)
{
	unsigned int slen = s.size();
	if (s[0] != '\\')
		return 0;
	unsigned int dots_in_name = 0;
	name += '\\';
	unsigned int len = 1;
	if (s[len] == '[') {
		name += '[';
		len++;
		while (len < slen) {
			name += s[len];
			if (s[len++] == ']')
				break;
		}
		// BUG: should check for missing ']'
	} 
	if (s[len] == '.') {
		dots_in_name++;
		name += '.';
		len++;
		while (len < slen && s[len] == '.') {
			dots_in_name++;
			name += '.';
			len++;
		}
	}
	while (len < slen) {
		char c = s[len];
		if (c == ' ')  break;
		if (c == '\t') break;
		if (c == '\n') break;
		if (c == '`')  break;
		if (c == '\'') break;
		if (c == '\\') break;
		if (c == '"')  break;
		if (c == '|')  break;
		if (c == ':')  break;
		if (c == ';')  break;
		if (c == '#')  break;
		if (c == '(')  break;
		if (c == ')')  break;
		if (c == '{')  break;
		if (c == '}')  break;
		if (c == ']')  break; // CAUTION: might break "\[3]syn"
		if (c == '/')  break;
		if (c == '*')  break;
		if (c == '-')  break;
		if (c == '+')  break;
		if (c == '<')  break;
		if (c == '>')  break;
		if (c == '=')  break;
		if (c == '>')  break;
		if (c == '$')  break;
		if (c == ',' && !inmath)
			break;
		if (c == '.') {
			unsigned int trailing_dots = 0;
			while (len < slen && s[len] == '.' && trailing_dots < dots_in_name) {
				name += s[len];
				len++;
			}
			break;
		}
		name += c;
		len++;
	}
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d find_synonym_name() got syn-name [%s]\n",__FILE__,__LINE__,name.c_str());
	return len;
}

static inline int
end_of_synonym(char c, bool inmath, bool need_brace)
{
	if (need_brace) {
		return c == '}';
	}
	switch (c) {
	case ' ':
	case '\t':
	case '\n':
	case '`':
	case '\'':
	case '\\':
	case '\0':
	case '"':
	case '|':
	case ':':
	case ';':
	case '#':
	case '(':
	case ')':
//	case '[':
//	case ']':
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
		return true;
	case ',':
		return (!inmath);
	}
	return false;
}
