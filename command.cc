//#define DEBUG_COMMAND
#include        <string>
#include        <string.h>
#include	<ctype.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "private.hh"
#include        "files.hh"
#include        "tags.hh"
#include        "command.hh"
#include        "superus.hh"

vector<BlockSource> bsStack;

static inline bool     testible(const char *s);
static inline int      white_space(const char *sp);
static inline int      word_length(const char *s);
static bool            extract_help(FILE * fp, char *line);
static bool            extract_procedure(FILE * fp, char *line);
static bool            extract_syntax(char *line);

// Store info about execution blocks.
bool             get_line_in_block(const char *block, unsigned int *offset);
char           *_command_word_separator = "-----";
int             _num_command_word = 0;
char           *_command_word[MAX_cmd_word];
static bool     have_command_word_buffer = false;
void            display_command_word_buffer(char *);
void            display_cmd_being_done_stack(void);
void            register_source(void);

bool
listCmd()
{
	if (_nword == 1) {
		err("`list' what?");
		return false;
	}
	// Try to use tempnam(), or tmpnam(), before using hardwired name 
	FILE *fp;
	string tmpname_file(tmp_file_name());
	if (!(fp = fopen(tmpname_file.c_str(), "w"))) {
		err("Error opening buffer-file for `list' command");
		return false;
	}
	// Figure out what command, and give help for it. 
	int i = 0;
	while (!isspace(*(_cmdLine + i)))
		i++;
	while (isspace(*(_cmdLine + i)))
		i++;
	strcat(_cmdLine, " *");
	bool found = false;
	for (int cmd = 0; cmd < _num_command; cmd++) {
		if (same_syntax(_cmdLine + i, _command[cmd].syntax, 1)) {
			found = true;
			fprintf(fp, "%s\n{\n", _command[cmd].help);
			fprintf(fp, "%s}\n", _command[cmd].procedure);
		}
	}
	if (found) {
		fclose(fp);
		more_file_to_terminal(tmpname_file.c_str());
		delete_file(tmpname_file.c_str());
	} else {
		fclose(fp);
		delete_file(tmpname_file.c_str());
		err("Help ?WHAT?");
	}
	return true;
}

// BUG -- not used 1.065?? 
void
dup_cmd_being_done_stack()
{
	if (_cmd_being_done < cmd_being_done_LEN) {
		_cmd_being_done_code[_cmd_being_done] = _cmd_being_done_code[_cmd_being_done - 1];
	}
	_cmd_being_done++;
}

void
push_cmd_being_done_stack(int cmd)
{
	if (_cmd_being_done < cmd_being_done_LEN) {
		_cmd_being_done_code[_cmd_being_done] = cmd;
	}
	_cmd_being_done++;
}

void
pop_cmd_being_done_stack()
{
	if (_cmd_being_done > 0) {
		_cmd_being_done--;
	}
}

int
cmd_being_done()
{
	if (_cmd_being_done > 0)
		return _cmd_being_done_code[_cmd_being_done - 1];
	else
		return -1;
}

void
display_cmd_being_done_stack()
{
	int             i;
	for (i = _cmd_being_done - 1; i > -1; i--) {
		if (i == _cmd_being_done - 1)
			gr_textput("  called by:           `");
		else
			gr_textput("  which was called by: `");
		gr_textput(_command[_cmd_being_done_code[i]].syntax);
		ShowStr("'\n");
	}
}

bool
nullCmd()
{
	return true;
}

// Return 1 if first character is '`', which indicates defn of new gri cmd 
bool
is_create_new_command(const char *s)
{
	register char   i = 0;
	while (isspace(*(s + i)))
		i++;
	return ((*(s + i) == '`') ? true : false);
}

// Parse the command line.  If it's a call to a C function, search a list
// (defined in tags.h) for a C function of that name.  Some restrictions
// apply here; the syntax must be exactly
// 
// extern "C" bool NAME(void)
//
// where NAME is the name of the C function as listed in the list stored in the
// tags.h file.
#define C_call "extern \"C\""
#define C_declaration " bool "
int
parse_C_commandCmd(const char *s)
{
	int             i = 0, start = 0;
	while (isspace(*(s + start)))	// skip white-space 
		start++;
	if (0 == strncmp(s + start, C_call, strlen(C_call))) {
		unsigned int length = 0;
		// It's a C command.  Use tags.h to find a function to call
		start += strlen(C_call);
		start += strlen(C_declaration);
		while (*(s + start + length)
		       && *(s + start + length) != '\n'
		       && *(s + start + length) != '(') {
			length++;
		}
		// Scan to find command name.
		i = 0;
#if 0				// test name lengths
		while (c_fcn[i].fcn) {
			if (strlen(c_fcn[i].name)!= c_fcn[i].name_len) {
				printf("code wants %d for \"%s\" but actually is %d\n", c_fcn[i].name_len,c_fcn[i].name,strlen(c_fcn[i].name));
			}
			i++;
		}
		exit(0);
#endif

		while (c_fcn[i].fcn) {
			if (length == strlen(c_fcn[i].name)
			    && !strncmp(s + start, c_fcn[i].name, length)) {
				c_fcn[i].fcn();
				return 1;
			}
			i++;
		}
		extern char _grTempString[];
		sprintf(_grTempString, "\
Gri cannot execute the command\n\
\t %s\
  since it is not defined in the tags.hh file.\n\
  Please report this internal error to the author.", s);
		gr_Error(_grTempString);
		return 0;		// will not be done
	} else {
		// It's not a C command.  It must be a gri command line, or series of
		// lines.
		GET_STORAGE(_cmd_being_done_IP[_cmd_being_done],
			    char, 1 + strlen(s + start));
		strcpy(_cmd_being_done_IP[_cmd_being_done], s + start);
		push_command_word_buffer();
		//printf("DEBUG %s:%d about to perform block.\n",__FILE__,__LINE__);
		perform_block(_cmd_being_done_IP[_cmd_being_done],
			      _command[_cmd_being_done_code[_cmd_being_done - 1]].filename,
			      _command[_cmd_being_done_code[_cmd_being_done - 1]].fileline
			);
		pop_command_word_buffer();
	}
	return 1;			// BUG: no check on newcommands! 
}				// parse_C_commandCmd() 
#undef C_call
#undef C_declaration

bool
create_commands(const char *filename)
{
	string fullfilename(_lib_directory.c_str());
	// Must check for '/' as file separator, on some machines.
#if !defined(VMS)
#if defined(MSDOS)
	// Insert a '\' if required 
	if (fullfilename[fullfilename.length() - 1] != '\\') {
		fullfilename += "\\";
	}
#else
	// Insert a '/' if required 
	if (fullfilename[fullfilename.length() - 1] != '/') {
		fullfilename += "/";
	}
#endif
#endif
	fullfilename += filename;
	if (push_cmd_file(fullfilename.c_str(), false, false, "r")) {
		/*
		  First, see if the version number in gri.cmd matches hard-wired one.
		  The gri.cmd is of the form:
		  //         gri - scientific graphic program (version 2.1.6)
		  and is contained on first line of file.
		*/
		get_command_line();
		char *s = _cmdLine;
		while (*s != '(')
			s++;
		int major_version, minor_version, minor_minor_version;
		if (3 != sscanf(s, "(version %d.%d.%d)", 
				&major_version, &minor_version, &minor_minor_version)) {
			warning("Cannot find version info in gri.cmd");
		} else {
			int major2 = int(floor(1e-10+_version));
			int minor2 = int(floor(100*(_version - major2)));
			int minor_minor2 = int(floor(1e-10+10000*(_version - major2 - 0.01*minor2)));
			if (major2 != major_version
			    || minor2 != minor_version
			    || minor_minor2 != minor_minor_version) {
				char msg[200];
				sprintf(msg, "Gri version number (%d.%d.%d) doesn't match version (%d.%d.%d) in library file gri.cmd", major2, minor2, minor_minor2, major_version, minor_version, minor_minor_version);
				warning(msg);
			}
		}
		// Finally, ok to process the startup file
		while (do_command_line()) {
			;			// EMPTY 
		}
	} else {
		// Couldn't find that file.
		string msg("ERROR: Gri can't locate the startup file `");
		msg += fullfilename;
		msg += "'.\n\
  You need to tell Gri the directory containing this file.\n\
  There are several ways to do this:\n\
    (1) Name the directory storing `gri.cmd' when you invoke Gri, e.g.\n\
            gri -directory /usr/share/gri/lib\n\
        where you may change the directory name to whatever directory\n\
        contains the correct `gri.cmd' file.\n\
    (2) Recompile Gri so it will know where to look by default (see\n\
        the INSTALL file in the source directory for instructions).\n";
		gr_textput(msg.c_str());
		gri_exit(1);
	}
	return true;
}

bool
create_new_command(FILE * fp, char *line)
{
	int             existing;
	if (_num_command >= COMMAND_STACK_SIZE)
		fatal_err("Too many commands defined");
	if (!extract_syntax(line))
		fatal_err("Can't extract syntax for new command `\\",
			  line,
			  "'",
			  "\\");
	if (!extract_help(fp, line))
		fatal_err("Can't extract help for new command `\\",
			  _command[_num_command].syntax,
			  "'",
			  "\\");
	register_source();		// Note file/line where defined 
	if (!extract_procedure(fp, line))
		fatal_err("Can't extract procedure for new command `\\",
			  _command[_num_command].syntax,
			  "'",
			  "\\");
	// Warn if matches existing syntax, but has different procedure.  If the
	// procedure is the same, no warning.  This is to allow for the re-use of
	// code with different help messages and different "looking" syntax; e.g.
	// the following have the same procedure, and are syntactically
	// identical, but are separated for ease of documenting in the help
	// files: `draw symbol .code.|\name at .x. .y. [cm]' `draw symbol
	// [.code.|\name]'
	existing = match_gri_syntax(_command[_num_command].syntax, 0);
	if (existing
	    && strcmp(_command[_num_command].procedure,
		      _command[existing - 1].procedure)
	    && *(_command[_num_command].syntax) != '?') {
		warning("Your new command\n    `\\",
			_command[_num_command].syntax,
			"'\n\
  matches a pre-existing command, but has a different procedure.\n\
  The *older* definition will be used.",
			"\\");
		extern char source_indicator[];
		printf("    NB: the proposed new cmd is `%s' at %s\n",
		       _command[_num_command].syntax, source_indicator);
		printf("    NB: the existing new cmd is `%s' at %s\n", 
		       _command[existing-1].syntax, source_indicator);
	}
	_num_command++;
	return true;
}

void
register_source()
{
	GET_STORAGE(_command[_num_command].filename, char,
		    1 + strlen(_cmdFILE.back().get_name()));
	strcpy(_command[_num_command].filename, _cmdFILE.back().get_name());
	_command[_num_command].fileline = _cmdFILE.back().get_line() + 1;
}

// require closing ' to be on same line as opening ` 
static bool
extract_syntax(char *line)
{
	char  *cp;
	// Register file/line where defined (used later to determine line numbers
	// if error
	GET_STORAGE(_command[_num_command].filename, char,
		    1 + strlen(_cmdFILE.back().get_name()));
	strcpy(_command[_num_command].filename, _cmdFILE.back().get_name());
	_command[_num_command].fileline = _cmdFILE.back().get_line();
	GET_STORAGE(_command[_num_command].syntax, char, 1 + strlen(line));
	cp = _command[_num_command].syntax;
	while (*line == ' ' || *line == '\t')	// skip initial white space 
		line++;
	while (*++line != '\'') {
		if (*line == '\n' || *line == '\0') {
			err("Missing final ' on new command definition line");
			return false;
		}
		*cp++ = *line;
	}
	*cp = '\0';
	_cmdFILE.back().increment_line();
	return true;
}

// extract_help() - extract help lines for new command, getting
// new lines from file 'fp', and starting with string 'line',
// which contains the syntax, as the first line of the help.
bool extract_help(FILE * fp, char *line)
{
	unsigned size = 2 + strlen(line); // total length of help 
	unsigned offset = 0;	// where to put next char 
	size = strlen(line) + 1 + 2; // for string catted below 
	char *cp = NULL;
	GET_STORAGE(cp, char, size);
	strcpy(cp, line);
	strcat(cp, "\n\n");
	offset = size - 1;
	while (!feof(fp)) {
		unsigned     len;	//  length of a given line 
		unsigned             i;
		char lastc = '\0';
		fgets(line, LineLength, fp);
		_command[_num_command].fileline++;
		_cmdFILE.back().increment_line();
		insert_cmd_in_ps(line/*, "command.cc:387"*/);
		len = strlen(line);
		size += len + 2;	// chars for NEWLINE and NULL (needed?) 
		if (NULL == (cp = (char *) realloc(cp, size))) {
			gr_Error("Can't reallocate space for help for new command");
			free(cp); // BUG: need this?
			return false;
		}
		// An unprotected '{' designates end of help text
		for (i = 0; i < len; i++) {
			if (*(line + i) == '{') {
				if (lastc == '\\') {
					// Brace was protected, so overwrite the backslash 
					*(cp + offset - 1) = '{';
					lastc = '{';
				} else {
					// Unprotected -- check rest is whitespace 
					unsigned int ii;
					for (ii = i + 1; ii < len; ii++) {
						if (*(line + ii) == '\n' || *(line + ii) == '\0') {
							break;
						} else if (!isspace(*(line + ii))) {
							err("Require `{' to be on line by itself.");
							free(cp);
							return false;
						}
					}
					*(cp + offset++) = '\0';
					_command[_num_command].help = cp;
					return true;
				}
			} else {
				// Normal character 
				*(cp + offset++) = lastc = *(line + i);
			}
		}
	}
	cp[0] = '\0';
	return false;
}

// extract_procedure() - extract procedure portion of new command BUG: stops
// when sees "}" as the first character of a line; should really tally the {}
// pairs instead. BUG: dies if "`" is seen at the start of a line; this is
// taken as an indication that the procedure was not terminated properly.
// This might be a useless test.
bool
extract_procedure(FILE * fp, char *line)
{
	int size = 0;
	// Redundant error test. 
	if (!re_compare(line, "\\s*{\\s*")) {
		err("Couldn't find { in the following line `\\", line, "'", "\\");
		return false;
	}
	GET_STORAGE(_command[_num_command].procedure, char, 2);
	strcpy(_command[_num_command].procedure, "");
	while (!feof(fp)) {
		fgets(line, LineLength, fp);
#if 0				// messing up
		if (((unsigned) superuser()) & FLAG_AUT1) {
			extern void insert_source_indicator(char *cl);
			insert_source_indicator(line);
		}
#endif
		if (feof(fp))
			break;
		insert_cmd_in_ps(line /*, "command.cc:454"*/);
		//printf("\n[%s] ", line);
		remove_comment(line);
		//printf("-> [%s]\n",line);
		
		// Test whether starting to define a newcommand within this one
		if (*(line + skip_space(line)) == '`') {
			err("Missing `}' in procedure body.");
			return false;
		}
		if (*line == '}') {
			if (((unsigned) superuser()) & FLAG_AUT2) printf("%s:%d got procedure: <%s>\n",__FILE__,__LINE__,_command[_num_command].procedure);
			return true;
		}
		_cmdFILE.back().increment_line();
		unsigned len = 0;
		while (line[len] != '\0' ) {
			len++;
		}
		if (len == 0)
			continue;
		if (len > 1 && *(line + len - 2) == '\\')
			*(line + len - 2) = '\0';
		else if (*(line + len - 1) != '\n')
			*(line + len - 1) = '\n';
		size += len;
		_command[_num_command].procedure = (char *) realloc(_command[_num_command].procedure, 1 + size);
		if (!_command[_num_command].procedure) {
			err("Insufficient storage space for new command.");
			return false;
		}
		strcat(_command[_num_command].procedure, line);
	}
	err("Missing `}' in procedure body ... EOF encountered.");
	return false;
}

// Does syntax match a gri command? RETURN (1 + cmd)  or 0 if not matching
// any known command
int
match_gri_syntax(const char *cmdline, int flag)
{
	int             cmd;
	for (cmd = 0; cmd < _num_command; cmd++)
		if (same_syntax(cmdline, _command[cmd].syntax, flag))
			return (1 + cmd);
	return 0;
}

// return 1 if syntax is the same.  If flag = 1 then being called by help,
// and "*" in cmdline matches any word or words following in syntax
bool
same_syntax(const char *cmdline, const char *syntax, int flag)
{
	if (cmdline == NULL)
		return false;
	int             ci = 0, si = 0;
	while (flag || testible(syntax + si)) {
		if (cmdline[ci] == '\0' || cmdline[ci] == '\n')
			return isspace(syntax[si]) ? true : false;
		if (syntax[si] == '\0' || syntax[si] == '\n')
			return true;	// at end-of-line
		if (flag && cmdline[ci] == '*')
			return true;	// being used for help, not cmd parsing
		if (!same_word(cmdline + ci, syntax + si))
			return false;	// words differ, cannot be same syntax
		ci += word_length(cmdline + ci);
		ci += white_space(cmdline + ci);
		si += word_length(syntax + si);
		si += white_space(syntax + si);
	}
	return true;
}

inline bool
testible(const char *s)
{
	if (*s == '.')  return false;
	if (*s == '\\') return false;
	if (*s == '[')  return false;
	if (*s == '"')  return false;
	if (*s == '{')  return false;
	if (*s == '&')  return false;
	if (*s == '*')  return false;
	if (*s == '\n') return false;
	if (*s == '\0') return false;
	// May still be non-testible, if the next character after the next blank
	// is |
	while (!isspace(*s)) {
		if (*s == '|')
			return false;
		if (*s == '\0' || *s == '\n')
			return true;
		s++;
	}
	while (isspace(*s))
		s++;
	if (*s == '|')
		return false;
	return true;
}

static inline int
word_length(const char *s)
{
	register int    i = 0;
	while (*(s + i) != ' ' &&
	       *(s + i) != '\t' &&
	       *(s + i) != '\n' &&
	       *(s + i) != '\0')
		i++;
	return i;
}

// Return number of white characters after point 
static inline int
white_space(const char *sp)
{
	if (*sp == '\0')
		return 0;
	else {
		int    i = 0;
		while (*(sp + i) == ' ' || *(sp + i) == '\t')
			i++;
		return i;
	}
}

// same_word () - are the words cp (command) and sp (syntax) the same?
bool
same_word(const char *cp, const char *sp)
{
	int             cplen = skip_nonspace(cp);
	int             splen = skip_nonspace(sp);
	if (cplen != splen)
		return false;
	else {
		return (!strncmp(cp, sp, cplen) ? true : false);
	}
}

// Register, then perform, indicated command. 
int
perform_gri_cmd(int cmd)
{
	return parse_C_commandCmd(_command[cmd].procedure);
}

// Maintain linear pushdown buffer of command-words for \.word0. etc
void
push_command_word_buffer()
{
	char *cp = NULL;		// assignment prevents warning
	GET_STORAGE(cp, char, 1 + strlen(_command_word_separator));
	strcpy(cp, _command_word_separator);
	_command_word[_num_command_word] = cp;
	_num_command_word++;
	if (_num_command_word >= MAX_cmd_word) {
		gr_Error("ran out of storage (must increase MAX_cmd_word in private.hh");
	}
	for (unsigned int i = 0; i < _nword; i++) {
		//printf("DEBUG %s:%d push_command_word_buffer <%s>\n",__FILE__,__LINE__,_word[i]);
		if (*_word[i] == '&') {	// 2001-feb-10 trying new syntax
			const char *name = 1 + _word[i];
			char coded_pointer[20];	// BUG: should be big enough.  Jeeze!
			if (is_var(name)) {
				sprintf(coded_pointer, "\\#v%d#", variablePointer.size());
				int the_index = index_of_variable(name);
				variablePointer.push_back(the_index);
				GET_STORAGE(cp, char, 1 + strlen(coded_pointer));
				strcpy(cp, coded_pointer);
				//printf("DEBUG %s:%d WAS VAR.  made <%s>\n",__FILE__,__LINE__,cp);
			} else if (is_syn(name)) {
				sprintf(coded_pointer, "\\#s%d#", synonymPointer.size());
				int the_index = index_of_synonym(name);
				synonymPointer.push_back(the_index);
				GET_STORAGE(cp, char, 1 + strlen(coded_pointer));
				strcpy(cp, coded_pointer);
				//printf("DEBUG %s:%d WAS SYN.  made <%s>\n",__FILE__,__LINE__,cp);
			}
		} else if (is_var(_word[i])) { // 2000-dec-18 SF bug 122893
			double v;
			bool ok = get_var(_word[i], &v);
			if (ok) {
				char value[100];
				sprintf(value, "%g", v);
				GET_STORAGE(cp, char, 1 + strlen(value));
				strcpy(cp, value);
			} else {
				GET_STORAGE(cp, char, 2);
				strcpy(cp, "0");
			}
		} else {
			GET_STORAGE(cp, char, 1 + strlen(_word[i]));
			strcpy(cp, _word[i]);
		}
		_command_word[_num_command_word] = cp;
		//printf("DEBUG %s:%d pushed command word %d as '%s'\n", __FILE__, __LINE__, _num_command_word, _command_word[_num_command_word]);
		_num_command_word++;
		if (_num_command_word >= MAX_cmd_word) {
			gr_Error("ran out of storage (must increase MAX_cmd_word in private.hh");
		}
	}
}

void
pop_command_word_buffer()
{
	// Trash last end-point, then find new start-point by looking backwards
	// for most recently pushed separator
	if (_num_command_word > 1) {
		for (int i = _num_command_word - 1; i > -1; i--) {
			if (!strcmp(_command_word[i], _command_word_separator)) {
				_num_command_word = i;
				free((char *) _command_word[i]);
				break;
			} else {
				free((char *) _command_word[i]);
			}
		}
	}
}

void
display_command_word_buffer(const char *s)
{
	int             i;
	if (_num_command_word > 0) {
		for (i = 0; i < _num_command_word; i++) {
			printf("%s\t%d/%d `%s'\n", s, i, _num_command_word, _command_word[i]);
		}
	} else {
		printf("%s    ...empty...\n", s);
	}
}

void
set_up_command_word_buffer()
{
	if (!have_command_word_buffer) {
		_num_command_word = 0;
		have_command_word_buffer = true;
	}
}

// Return true if got to end without executing a `break' or `quit'
// NOTE: catch 'while' and 'system' commands and run them here, 
// since they are special cases.
bool
perform_block(const char *block, const char *source_file, int source_line)
{
	//printf("DEBUG %s:%d in perform_block()\n",__FILE__,__LINE__);

	unsigned int lines = 0, offset = 0;
	BlockSource bs(block, source_file, source_line);
	bsStack.push_back(bs);
	// Scan through block line by line.  If a 'while' is found, then scan the
	// loop and give it to perform_while_block.

	extern int chars_read; // defined in read.cc

	while (get_line_in_block(block, &offset)) {
		lines++;
		massage_command_line(_cmdLine);
		if (_nword == 1 
		    && (word_is(0, "break") 
			|| word_is(0, "return") 
			|| word_is(0, "quit"))
		    && !skipping_through_if()) {
			if (word_is(0, "quit"))
				quitCmd();
			bsStack.pop_back();
			return false;
		}
		if (word_is(0, "read") && !skipping_through_if()) {
			extern unsigned int offset_for_read;
			offset_for_read = offset;
		}
		if (word_is(0, "while") && !skipping_through_if()) {
			// Capture the loop (look for matching 'end while')
			string          test;
			unsigned        buffer_offset = offset;
			int             loop_level = 1;
			test.assign((char*)(6 + (char*)strstr(_cmdLine, "while")));
			if (re_compare(test.c_str(), " *")) {
				err("`while .test.|{rpn ...}' missing the test part");
				bsStack.pop_back();
				return false;
			}
			string buffer;
			while (get_line_in_block(block, &buffer_offset)) {
				// Search for matching `end while'
				if (re_compare(_cmdLine, "\\s*while.*")) {
					loop_level++;
				} else if (re_compare(_cmdLine, "\\s*end\\s+while\\s*.*")) {
					loop_level--;
					if (loop_level < 1) {
						break;
					}
				}
				buffer.append(_cmdLine);
				buffer.append("\n");
			}
			if (loop_level != 0) {
				err("Missing `end while'.");
				bsStack.pop_back();
				return false;
			}
			perform_while_block(buffer.c_str(), test.c_str(), lines);
			// Adjust 'offset' to skip this interior loop, then skip the `end
			// while' line.  Therefore will next capture line after the loop.
			offset += buffer.size() + 1;	// point to after this loop 
			get_line_in_block(block, &offset);	// skip `end while' 
			//printf("AFTER THE WHILE, \n~~~%s~~~\n",block+offset);
		} else if (word_is(0, "system")) {
			// Intercept system commands, since if they are of the
			// <<EOF form, it will be neccessary to slurp the whole command
			// right here.  Even if skipping through if, need
			// to slurp until EOF
			int status = 0;
			char *s = _cmdLine;
			char *ptr;
			s += skip_space(s);		// skip any initial space 
			s += skip_nonspace(s);	// skip "system" 
			s += skip_space(s);
			// s now points to first word after "system" 
			if (*s == '\0' || *s == '\n') {
				err("`system' needs a system command to do");
				bsStack.pop_back();
				return false;
			}
			ptr = (char*)strstr(s, "<<");
			if (ptr) {
				// It's of the form <<EOF or <<"EOF", so first find the
				// stop word, named "read_until" here.
				string read_until;
				while(*ptr) {
					if (*ptr != '<' && *ptr != '"' && !isspace(*ptr)) {
						read_until.assign(ptr);
						break;
					}
					ptr++;
				}
				// Chop trailing blanks, or quote mark 
				int len = read_until.size();
				for(int i = 0; i < len; i++) {
					if (read_until[i] == '"' || isspace(read_until[i])) {
						read_until.STRINGERASE(i, read_until.size() - i);
						break;
					}
				}
				static string cmd; // might save time in loops
				cmd.assign(s);
				while(get_line_in_block(block, &offset)) {
					cmd.append("\n");
					cmd.append(_cmdLine);
					if (len && !strncmp(_cmdLine + skip_space(_cmdLine), read_until.c_str(), len)) {
						break;
					}
				}
				if (!skipping_through_if()) {
					string cmd_new;
					substitute_synonyms_cmdline(cmd.c_str(), cmd_new, false);
					status = call_the_OS(cmd_new.c_str(), __FILE__, __LINE__);
					PUT_VAR("..exit_status..", (double) status);
				}
			} else {
				// It's just a simple system command. 
				if (!skipping_through_if()) {
					status = call_the_OS(s, __FILE__, __LINE__);
					PUT_VAR("..exit_status..", (double) status);
				}
			}
		} else {	// It's not a system command
			chars_read = 0;
			perform_command_line(NULL, true);
			offset += chars_read;
			//printf("DEBUG %s:%d AFTER PERFORMING CMD LINE chars read= %d. next is what's left:\n{%s}\n",__FILE__,__LINE__,chars_read,block+offset);
		}
#if 0
		// Increment offset 
		bsStack[bsStack.size() - 1].increment_offset();
#endif
		// See if an error, or if `quit' executed 
		stop_replay_if_error();
		if (_done) {
			bsStack.pop_back();
			return false;
		}
	}				// while (get_line_in_block(block, &offset)) 
	bsStack.pop_back();
	return true;
}				// perform_block

int
block_level()
{
	return bsStack.size();
}

// Do not call this if block_level()<=0 
const char*
block_source_file()
{
	if (bsStack.size() < 1) {
		gr_Error("Underflow of block-stack (internal error)");
	}
	return bsStack[bsStack.size() - 1].get_filename();
}

// Do not call this if block_level()<=0 
unsigned int
block_source_line()
{
	if (bsStack.size() < 1) {
		gr_Error("Underflow of block-stack (internal error)");
	}
	return bsStack[bsStack.size() - 1].get_line();
}

// Do not call this if block_level()<=0 
unsigned int
block_offset_line()
{
	if (bsStack.size() < 1) {
		gr_Error("Underflow of block-stack (internal error)");
	}
	return bsStack[bsStack.size() - 1].get_offset();
}

// Return true if got a line 
bool
get_line_in_block(const char *block, unsigned int *offset)
{
	//printf("DEBUG %s:%d in get_line_in_block ... <%s>\n",__FILE__,__LINE__,block+*offset);

	if (*(block + *offset) == '\0')
		return false;
	unsigned int i = 0;
	while (1) {
		if (*(block + *offset) == '\n') {
			*(_cmdLine + i) = '\0';
			(*offset)++;
			bsStack.back().increment_line(strlen(_cmdLine));
			return true;
		} else if (*(block + *offset) == '\0') {
			*(_cmdLine + i) = '\0';
			bsStack.back().increment_line(strlen(_cmdLine));
			// Note -- next time will still point to '\0', and will catch 
			return true;
		}
		*(_cmdLine + i++) = *(block + (*offset)++);
	}
}

