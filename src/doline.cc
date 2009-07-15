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

#define USE_BACKTIC 0		// keep code just in case
//#define DEBUG_DOLLAR_PAREN 1
#include        <string>
#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#include	"gr.hh"

#include "extern.hh"
#include "command.hh"
#include "superus.hh"

void            display_cmd_being_done_stack(void);

extern char     _grTempString[];
extern char    *_griVersion_name;

#if USE_BACKTIC
static          bool sub_backtic(const char *n, char *out);
#endif
static          bool sub_dollar_paren(const char *n, std::string& out);
static          int dollar_paren(const char *s, std::string& res);
bool            get_cmd(char *buf, int max, FILE * fp);
bool            postscriptCmd(void);
static void     check_usage(const char *s);


bool
tracing()
{				// ?? this could be speeded up a lot ??
	double          trace;
	get_var("..trace..", &trace);
	return (trace ? true : false);
}

// do_command_line() -- get, massage, and perform command line 
// RETURN false if no more command lines.  (Set _done=1 if no
// more command lines and the cmd-file stack empties.)
bool
do_command_line()
{
	//printf("at start of do_command_line() _cmdLine is <%s>\n",_cmdLine);
	// *** Get command line, storing it in the global _cmdLine ***
	if (!get_command_line()) {
		_cmdFILE.pop_back();
		if (_cmdFILE.size() == 0) {
			_done = 1;
			return false;
		}
		return false;
	}
	//printf("DEBUG cmdline '%s'\n",_cmdLine);
	// Remove comments, do math expansions, substitute synonyms, etc.
	massage_command_line(_cmdLine);

	// Handle "return" as special case
	if ((_nword > 0)
	    && (!strcmp(_word[0], "return"))
	    && (!skipping_through_if())) {
		_cmdFILE.pop_back();
		if (_cmdFILE.size() == 0) {
			_done = 1;
			return false;
		} else {
			return false;
		}
	} else {
		// Do what the command instructs.
		perform_command_line(NULL, true);
		return true;
	}
}

// Insert command line as a comment in the PostScript file, after
// removing newpage character to blank, (since latex2e/epsfig breaks 
// on newpage).
// 
// The 'note' is helpful in debugging.
void
insert_cmd_in_ps(const char *cmd, const char *note)
{
	extern bool _private;
	if (!_private) {
		extern bool     _store_cmds_in_ps;	// DEFINED IN startup.c
		if (!_store_cmds_in_ps)
			return;
		unsigned int first_nonwhite = 0;
		while (isspace(*(cmd + first_nonwhite)))
			first_nonwhite++;
#if 0				// removed 2001-feb-22 for SF bug #133135
		if (!strncmp(cmd + first_nonwhite, "insert", 6))
			return;		// don't want 'insert' commands (confusing eh)
#endif
		strcpy(_grTempString, "gri:");
		int ii = 4;			// where to start insert
		int len = strlen(cmd);
		for (int i = 0; i < len; i++) {
			if (cmd[i] == PASTE_CHAR)
				break;
			if (cmd[i] != char(12))	// newpage
				_grTempString[ii++] = cmd[i];
		}
		_grTempString[ii] = '\0';
		if (_grTempString[strlen(_grTempString) - 1] == '\n')
			_grTempString[strlen(_grTempString) - 1] = '\0';
		if (*note != '\0') {
			strcat(_grTempString, " # ");
			strcat(_grTempString, note);
		}
		gr_comment(_grTempString);
	}
}

#if 1				// used only if -s256 set
void
insert_source_indicator(char *cl)
{
// BUG must not let overrun
	unsigned int len = strlen(cl);
	char line[1024];
	sprintf(line, " \"%s:%d\"", _cmdFILE.back().get_name(), _cmdFILE.back().get_line());
	strcat(cl, line);
	if (len > 0 && cl[len - 1] == '\n')
		cl[len - 1] = PASTE_CHAR;
	else
		cl[len] = PASTE_CHAR;
}
#endif

// get_command_line() -- get a command line (skip full-line C comments)
bool
get_command_line(void)
{
/*#ifndef HAVE_LIBREADLINE*/
	write_prompt();
/*#endif*/
	stop_replay_if_error();
	// get a line from a file.
	if (_cmdFILE.back().get_interactive()) {
		//printf("DEBUG. interactive. before, had <%s>\n",_cmdLine);
		// Cmd-file is interactive (from keyboard)
		if (!gr_textget(_cmdLine, LineLength_1)) {
			_done = 1;
			return false;
		}
		//printf("DEBUG. interactive. after, have <%s>\n",_cmdLine);
	} else {
		// Cmd-file is non-interactive.
		if (true == get_cmd(_cmdLine, LineLength_1, _cmdFILE.back().get_fp())) {
			if (strlen(_cmdLine) < 1) {
				if (((unsigned) superuser()) & FLAG_AUT2) printf("%s:%d debug 2\n",__FILE__,__LINE__);
				return false;
			} else {
				warning("Missing newline at end of command file.");
			}
		}
		// Now ready to do something.
		if (!is_create_new_command(_cmdLine)) {
			// Print trace info if desired
			if (tracing()) {
				if (skipping_through_if())
					printf("X  ");
				else
					printf("%s", _margin.c_str());
				printf("%s\n", _cmdLine);
			}
		}
	}
	_cmdFILE.back().increment_line(); // BUG line numbers wrong BUG
	if (_cmdFILE.back().get_save2ps())
		insert_cmd_in_ps(_cmdLine/*, "doline.cc:154"*/);
	if (((unsigned) superuser()) & FLAG_AUT1) {
		insert_source_indicator(_cmdLine);
	}
	if (((unsigned) superuser()) & FLAG_AUT2) printf("%s:%d  get_command_line returning [%s]\n",__FILE__,__LINE__,_cmdLine);
	return true;
}

// Insert PostScript directly into file.
bool
postscriptCmd()
{
	extern FILE    *_grPS;
	fprintf(_grPS, "%s\n", _cmdLine + skip_space(_cmdLine) + strlen("postscript "));
	check_psfile();
	return true;
}

// Remove comments, expand rpn, substitute synonyms, etc.
// Return 1 if not end-of-file, or 0 if end-of-file.
bool
remove_source_indicator(char *cmd)
{
	extern char source_indicator[];
	int len = strlen(cmd);
	source_indicator[0] = '\0';
	for (int i = 0; i < len; i++) {
		if (cmd[i] == PASTE_CHAR) {
			int cut_here = i;
			i++;
			while (isspace(*(cmd + i)) && i < len) // skip whitespace if any
				i++;
			if (cmd[i] == '"')
				strcpy(source_indicator, cmd + i + 1);
			cmd[cut_here] = '\0';
			int len = strlen(source_indicator);
			if (source_indicator[len - 1] == '"')
				source_indicator[len - 1] = '\0';
			return true;
		}
	}
	return false;
}

bool
massage_command_line(char *cmd)
{
	_nword = 0;
	if (((unsigned) superuser()) & FLAG_AUT1) {
		remove_source_indicator(cmd);
	}
	remove_comment(cmd);
	if (!is_system_command(cmd))
		check_usage(cmd);
	strcpy(_cmdLineCOPY, cmd + skip_space(cmd));

	// For system commands, just substitute synonyms.
	if (is_system_command(cmd)) {
		strcpy(cmd, _cmdLineCOPY);
		std::string cmd_sub;
		substitute_synonyms_cmdline(cmd, cmd_sub, false);
		strcpy(_cmdLineCOPY, cmd_sub.c_str());
		strcpy(cmd, _cmdLineCOPY);
		// Chop into words.  Note: chop_into_words() destroys it's string, so
		// use a copy.
		chop_into_words(_cmdLineCOPY, _word, &_nword, MAX_nword);
		return true;
	}

	// Don't massage further if it's a `create new command' or a `postscript'
	// command.
	if (is_create_new_command(cmd)) {
		if (((unsigned) superuser()) & FLAG_NEW) printf("[%s]\n", cmd);
		return true;
	}
	// Expand blanks, by separating words, but only in certain circumstances.
	if (!re_compare(_cmdLine, "\\s*cd\\s*.*")
	    && !re_compare(_cmdLine, "\\s*delete\\s*.*")
	    && !re_compare(_cmdLine, "\\s*ls\\s*.*")
	    && !re_compare(_cmdLine, "\\s*insert\\s*.*")
	    && !re_compare(_cmdLine, "\\s*close\\s*.*")
	    && !re_compare(_cmdLine, "\\s*open\\s*.*") // removed 2.5.5; re-inserted 2.6.0
	    && !re_compare(_cmdLine, "\\s*postscript\\s*.*") ) {
		expand_blanks(cmd);
	}
	remove_trailing_blanks(cmd);
	_error_in_cmd = false;
	if (strlen(cmd) < 1) {
		_nword = 0;
		return true;
	}
	strcpy(_cmdLineCOPY, cmd + skip_space(cmd));
	strcpy(cmd, _cmdLineCOPY);
	if (strlen(cmd) < 1) {
		_nword = 0;
		return true;
	}
	// Copy back into cmd.  This must be done before substituting synonyms,
	// so that code like `defined (\syn)' will work.
	if (!re_compare(_cmdLine, "\\s*postscript\\s*.*")
	    && !re_compare(_cmdLine, "\\s*new\\s*.*")) {
		strcpy(cmd, _cmdLineCOPY);
	}
	// Substitute synonyms; copy back into cmd.
	if (((unsigned) superuser()) & FLAG_SYN) printf("[%s]\n", cmd);
	// Don't substitute synonyms for these commands: `new \syn ...' `delete
	// \syn ...'
	if (re_compare(_cmdLine, "\\s*open\\s*.*")) {
		std::string cmd_sub;
		substitute_synonyms_cmdline(cmd, cmd_sub, false);
		strcpy(cmd, cmd_sub.c_str());
	} else {
		if (!re_compare(_cmdLine, "\\s*new\\s*.*")
		    && !re_compare(_cmdLine, "\\s*delete\\s*.*")
#if 0				// 1999-dec-12
		    && !re_compare(_cmdLine, "\\s*read\\s+line\\s*.*")
#else
//		    && !re_compare(_cmdLine, "\\s*read\\s*.*")
#endif
		    && !re_compare(_cmdLine, "\\s*get\\s+env\\s*.*")
			) {
			std::string cmd_sub;
			substitute_synonyms_cmdline(cmd, cmd_sub, true);
			strcpy(cmd, cmd_sub.c_str());
			remove_trailing_blanks(cmd);
		}
	}
	if (((unsigned) superuser()) & FLAG_SYN) printf("DEBUG %s:%d line is now '%s'\n",__FILE__,__LINE__,cmd);
	// Substitute backtic and dollar-paren expressions
#if USE_BACKTIC
	sub_backtic(cmd, _cmdLineCOPY);
	strcpy(cmd, _cmdLineCOPY);
#endif
	std::string tmp;
	sub_dollar_paren(cmd, tmp);
	strcpy(cmd, tmp.c_str());
	// Substitute rpn expressions one by one, recopying back to cmd after
	// each
	if (((unsigned) superuser()) & FLAG_RPN) printf("[%s]\n", cmd);
	if (!re_compare(_cmdLine, "\\s*postscript\\s*.*")
	    && !re_compare(_cmdLine, "\\s*new\\s*.*")
	    && !re_compare(_cmdLine, "\\s*while\\s*.*") ) {
		if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG %s:%d about to substitute rpn in '%s'  skipping_through_if= %d\n",__FILE__,__LINE__,cmd,skipping_through_if());
		if (!skipping_through_if()) {
			while (substitute_rpn_expressions(cmd, _cmdLineCOPY)) {
				strcpy(cmd, _cmdLineCOPY);
			}
			if (_error_in_cmd) {
				err("Error in RPN expression");
			}
		}
		remove_trailing_blanks(cmd);
		if (strlen(cmd) < 1)
			return true;
		strcpy(_cmdLineCOPY, cmd);
	}
	stop_replay_if_error();
	strcpy(cmd, _cmdLineCOPY);
	if (((unsigned) superuser()) & FLAG_RPN) printf("  --> [%s]\n", cmd);
	// Finally, chop up into words.  The words (_word) and number of words
	// (_nword) are used all over the place in other functions.
	chop_into_words(_cmdLineCOPY, _word, &_nword, MAX_nword);
	return true;
}

// Do what command line instructs.
// RETURN true if OK, NO if error
bool
perform_command_line(FILE *fp, bool is_which)
{
	//if (((unsigned) superuser()) & FLAG_FLOW) printf("\nDEBUG %s:%d begin of perform_command_line\n",__FILE__,__LINE__);
	if (strlen(_cmdLine) < 1)
		return true;		// was ok, just blank
	// If it's definition of a new gri command, do that.
	if (is_create_new_command(_cmdLine)) {
		if (is_which)
			create_new_command(_cmdFILE.back().get_fp(), _cmdLine);
		else
			create_new_command(fp, _cmdLine);
		return true;
	}
	// Handle `return'
	if (_nword > 0 && !strcmp(_word[0], "return") && !skipping_through_if()) {
		_done = 2;
		return true;
	}


	if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG %s:%d perform_command_line() has command '%s'\n", __FILE__, __LINE__, _cmdLine);
	if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG %s:%d skipping_through_if is %d (STEP 1)\n", __FILE__, __LINE__, skipping_through_if());


	// Handle `end' and `else'
	if (handle_if_block()) {
		if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG: %s:%d returning early\n",__FILE__,__LINE__);
		return true;
	}

	if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG %s:%d skipping_through_if is %d (STEP 2)\n", __FILE__, __LINE__, skipping_through_if());

	// Process line if not skipping
	if (!skipping_through_if()) {
		// First, handle de-referenced synonyms as lvalues (to left
		// an assignment operator)
		std::string w0(_word[0]);
                // de_reference(w0); BUG: MAY PUT BACK LATER
		char *cp = strdup(w0.c_str());
		_word[0] = cp;
		// Handle `\name = "value"' command
		if (w0[0] == '\\') {
			if (_nword > 2 && is_assignment_op(_word[1])) {
				assign_synonym();
				free(cp);
				return true;
			}
		}
		// Handle math command, e.g.
		//     .var. = 1
		//     x += 1
		//     grid data += 1
		//     image += 1
		// etc., permitting various assignment operators,
		// including '=', '+=', etc.
		if (word_is(0, "x")
		    || word_is(0, "y")
		    || word_is(0, "z")
		    || word_is(0, "u")
		    || word_is(0, "v")
		    || word_is(0, "image")
		    || word_is(0, "grid")
		    || is_var(w0)) {
			if (_nword == 3) {
				if(word_is(1, "=") 
				   || word_is(1, "-=")
				   || word_is(1, "+=")
				   || word_is(1, "*=")
				   || word_is(1, "/=")
				   || word_is(1, "^=")
				   || word_is(1, "_=")) {
					//printf("DOING mathCmd\n"); for (int ii=0;ii<_nword;ii++) printf("\t<%s>\n",_word[ii]);
					mathCmd();
					free(cp);
					return true;
				}
			} else if (_nword == 4 
				   && word_is(0, "grid") && (word_is(1, "data")
							     || word_is(1, "x")
							     || word_is(1, "y")) ) {
				mathCmd();
				free(cp);
				return true;
			} else if (_nword == 4
				   && word_is(0, "image") && (word_is(1, "grayscale")
							      || word_is(1, "greyscale")
							      || word_is(1, "colorscale")
							      || word_is(1, "colourscale"))) {
				mathCmd();
				free(cp);
				return true;
			} else {
				err("Unknown command.  Were you trying to manipulate `\\",
				    _word[0], "' mathematically?", "\\");
				free(cp);
				return false;
			}
		}
		// Figure out what command, and do it.
		int cmd;
		if (0 != (cmd = match_gri_syntax(_cmdLine, 0))) {
			// Do the command.
			push_cmd_being_done_stack(cmd - 1);
			if (!perform_gri_cmd(cmd - 1)) {
				err("Can't perform/parse following command");
				free(cp);
				return false;
			}
			pop_cmd_being_done_stack();
			free(cp);
			return true;
		} else {
			// No syntax registered for this command.  Check special cases,
			// where it's a stray "break," or "continue"
			if (word_is(0, "break")) {
				err("Cannot have `break' outside loops");
				free(cp);
				return false;
			} else if (word_is(0, "continue")) {
				err("Cannot have `continue' outside loops");
				free(cp);
				return false;
			} else if (string_is_blank(_word[0])) {
				free(cp);
				return true;
			} else {
				err("Unknown command encountered.");
				free(cp);
				return false;
			}
		}
		free(cp);
	}
	return true;
}

// Stop replay if previous command instructed so (because _error_in_cmd set
// to 1 because of error in last command).
bool
stop_replay_if_error()
{
	if (_error_in_cmd && !_cmdFILE.back().get_interactive()) {
		ShowStr(" Bad command:  `");
		ShowStr(_cmdLine);
		ShowStr("'\n");
		display_cmd_being_done_stack();
		fatal_err(NULL);
	}
	return true;			// never actually reached
}

// is_system_command() - return 1 if it's a system command
bool
is_system_command(const char *s)
{
	// First, check if '\s*system ...'
	s += skip_space(s);
	if (!strncmp(s, "system", 6) ? true : false)
		return true;

	// Second, check if '\s*\\name\s*=\s*system ...'
	s += skip_nonspace(s);      // name
	s += skip_space(s);         // space
	s += skip_nonspace(s);      // =
	s += skip_space(s);         // space
	if (!strncmp(s, "system", 6) ? true : false)
		return true;
	return false;
}

// systemCmd() - handle request for system command.  Certain translations are
// done first. (1) \\n is translated into \n so that commands like the
// following will work:
// 
// system gawk 'BEGIN { printf("line1\n") ; printf("line2\n");}'
// 
// The problem is that the previous parsing, PRESUMING that NEWLINE is not the name
// of a user synonym, will replace "\n" by "\\n" so that the system would
// otherwise be given the string
//
// system gawk 'BEGIN { printf("line1\\n") ; printf("line2\\n");}'
// 
// to process.  Furthermore, the shell <<WORD syntax is supported.
//
// Important note: this code is only executed outside newcommands
// and blocks.  If already inside a block, this command is *never* 
// called; see perform_block() in command.cc
bool
systemCmd()
{
	if (skipping_through_if())
		return true;
	// Much of following code duplicated in assign_synonym(), so if any
	// problems crop up, check there too.
	char *s = _cmdLine;
	s += skip_space(s);		// skip any initial space
	s += skip_nonspace(s);	// skip "system"
	s += skip_space(s);
	// s now points to first word after "system"
	if (*s == '\0' || *s == '\n') {
		err("`system' needs a system command to do");
		return false;
	}
	// See if last word starts with "<<"; if so, then the stuff to be done
	// appears on the lines following, ended by whatever word follows the
	// "<<". 
	// ... compare set.cc near line 3288.
	int i = strlen(s) - 2;
	static std::string read_until;
	read_until.assign("");
	bool            using_read_until = false;
	while (--i) {
		if (!strncmp((s + i), "<<", 2)) {
			//printf("- looking for <<\n");
			bool            quoted_end_string = false;
			int             spaces = 0;
			while (isspace(*(s + i + 2 + spaces))) {
				spaces++;
			}
			if (*(s + i + 2 + spaces) == '"') {
				spaces++;
				quoted_end_string = true;
			}
			read_until.assign(s + i + 2 + spaces);
			using_read_until = true;
			// trim junk from end of the 'read until' string
			std::string::size_type cut_at;
			if (quoted_end_string)
				cut_at = read_until.find("\"");
			else
				cut_at = read_until.find(" ");
			//printf("READING UNTIL '%s' ... i.e.\n", read_until.c_str());
			if (cut_at != STRING_NPOS)
				read_until.STRINGERASE(cut_at, read_until.size() - cut_at);
			if (read_until.size() < 1) {
				err("`system ... <<STOP_STRING' found no STOP_STRING");
				return false;
			}
			//printf("reading until '%s'\n",read_until.c_str());
			break;
		}
	}
	static std::string cmd;	// might save time in loops
	cmd.assign(s);
	if (using_read_until) {
		// It is of the <<WORD form
		cmd.append("\n");
		//printf("%s:%d ABOUT TO GOBBLE\n",__FILE__,__LINE__);
		while (get_command_line()) {
			//printf("%s:%d cmd line is [%s]\n",__FILE__,__LINE__,_cmdLine);
			// Trim filename/fileline indicator
			unsigned int l = strlen(_cmdLine);
			for (unsigned int ii = 0; ii < l; ii++) {
				if (_cmdLine[ii] == PASTE_CHAR) {
					_cmdLine[ii] = '\0';
					break;
				}
			}
			if (!strncmp(_cmdLine + skip_space(_cmdLine), read_until.c_str(), read_until.size())) {
				cmd.append(_cmdLine + skip_space(_cmdLine));
				cmd.append("\n");
				break;
			}
			cmd.append(_cmdLine);
			cmd.append("\n");
		}
		static std::string cmd_sub;
		substitute_synonyms_cmdline(cmd.c_str(), cmd_sub, false);
		cmd = cmd_sub;
	} else {
		// No, it is not of the <<WORD form
		std::string::size_type loc = 0;
		//printf("system command BEFORE [%s]\n",cmd.c_str());
		while (STRING_NPOS != (loc = cmd.find("\\\\", loc))) {
			cmd.STRINGERASE(loc, 2);
			cmd.insert(loc, "\\");
		}
		//printf("AFTER [%s]\n",cmd.c_str());
	}
	int status = call_the_OS(cmd.c_str(), __FILE__, __LINE__);
	PUT_VAR("..exit_status..", (double) status);
	sprintf(_grTempString, "%d status\n", status);
	RETURN_VALUE(_grTempString);

	return true;
}

static void
show_startup_msg()
{
	std::string fullfilename(_lib_directory.c_str());
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
	fullfilename += "startup.msg";
	FILE *fp = fopen(fullfilename.c_str(), "r");
	if (fp) {
		GriString inLine(128);
		while (!inLine.line_from_FILE(fp)) {
			ShowStr(inLine.getValue());
		}
		fclose(fp);
	}
}


// write_prompt - write a prompt if keyboard input
void
write_prompt()
{
	if (!_cmdFILE.back().get_interactive())
		return;
	// Write prompt if reading from keyboard
	if (_first == true) {
		extern bool _no_startup_message;
		if (!_no_startup_message)
			show_startup_msg();
		_first = false;
	}
#ifndef HAVE_LIBREADLINE
		gr_textput(_prompt.c_str());
#endif
}



// remove_comment() -- fix command line 1) check that input line read OK 2)
// remove the '\n' from fgets 3) remove trailing comments 4) remove trailing
// blanks or tabs
//
// Return true if there WAS a comment and it was the FIRST non-whitespace thing.
bool
remove_comment(char *s)
{
	//printf("\tremove_comment(%s)\n",s);
	int len = strlen(s);
#if 0
	// Discard trailing newlines (or control-M characters, which
	// are added to newline in PC-ish systems), by converting
	// them to nulls
	while (len > 1 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
		s[--len] = '\0';
#endif
	bool is_continued = false;
	if (len > 1 && s[len - 1] == '\\' && s[len - 2] == '\\') 
		is_continued = true;
	// Discard comment on end
	if (len > 0) {
		bool in_dquote = false;
		bool in_squote = false;
		for (int i = 0; i < len; i++) {
			if (s[i] == '"' && (i == 0 || s[i-1] != '\\')) {
				//printf("got s[%d]=\". with in_dquote=%s  in_squote=%s\n",i,in_dquote?"T":"F",in_squote?"T":"F");
				if (!in_squote)
					in_dquote = !in_dquote;
				continue;
			}
			if (s[i] == '\'' && (i == 0 || s[i-1] != '\\')) {
				if (!in_dquote)
					in_squote = !in_squote;
				continue;
			}
			if (!in_dquote
			    && !in_squote
			    && ((s[i] == '/' && s[i+1] == '/') || s[i] == '#')) {
				//printf("remove_comment [%s] ->\n", s);
				s[i] = '\0';
				//printf("               [%s]\n", s);
				if (is_continued) {
					s[i++] = '\\';
					s[i++] = '\\';
				}
				len = i;
				break;
			}
		}
		// Set a flag if there is now nothing but whitespace.
		for (int ii = 0; ii < len; ii++) {
			//printf("\tEXAMINE [%c]\n", s[ii]);
			if (!isspace(s[ii])) {
				//printf("\t\tRETURNING non-BOTTOM false [%s]\n", s);
				return false;
			}
		}
		//printf("\t\tRETURNING true [%s]\n", s);
		return true;
	}
	//printf("\t\tRETURNING BOTTOM false [%s]\n", s);
	return false;
}

static void
check_usage(const char *s)
{
	if (s[0] == '`')
		return;			// defining new command
#if 0				// 2.5.5 allows e.g \.argv[0].
	register int    i, len;
	int             inquote = 0;
	len = strlen(s);
	for (i = 0; i < len; i++) {
		if (s[i] == '"')
			inquote = !inquote;
		if (!inquote && s[i] == '[' && !(i == 0 || s[i - 1] == '\\')) {
			sprintf(_grTempString, "\
Warning: '[' character found in command.  This character is in the\n\
manual to indicate optional items, and is not allowed in commands\n\
except inside system calls\n\
  Command: '%s'\n", s);
			ShowStr(_grTempString);
			sprintf(_grTempString, "\
           ");
			ShowStr(_grTempString);
			int j;
			for (j = 0; j <= i; j++) {
				_grTempString[j] = ' ';
			}
			_grTempString[j] = '\0';
			strcat(_grTempString, "^ Bad character\n");
			ShowStr(_grTempString);	    
		}
	}
#endif
}

#if USE_BACKTIC
// Substitute backtic style system expressions, as in Bourne and 
// Bourne-again shells.  Assume string 'out' is long enough.
static bool
sub_backtic(const char *in, char *out)
{
#if !defined(HAVE_POPEN)
	err("Cannot substitute backtics because computer lacks popen() C subroutine");
#else
	int len = strlen(in);
	int out_index = 0;
	if (len < 2) {
		strcpy(out, in);
		return true;
	}
	char *cmd = new char[LineLength];
	if (!cmd) OUT_OF_MEMORY;
	for (int i = 0; i < len; i++) {
		if (in[i] == '`') {
			// Search for closing backtic
			for (int j = i + 1; j < len; j++) {
				if (in[j] == '`') {
					strcpy(cmd, in + i + 1);
					cmd[j - i - 1] = '\0';
					if (((unsigned) superuser()) & FLAG_SYS) {
						ShowStr("The `` mechanism is sending this to the OS:\n");
						ShowStr(cmd);
						ShowStr("\n");
					}
					FILE *pipefile = (FILE *) popen(cmd, "r");
					if (pipefile) {
						char *result = new char[LineLength];
						if (!result) OUT_OF_MEMORY;
						char *thisline = new char[LineLength];
						if (!thisline) OUT_OF_MEMORY;
						strcpy(result, "");
						while (NULL != fgets(thisline, LineLength_1, pipefile))
							strcat(result, thisline);
						pclose(pipefile);
						// Skip null and possible final newline
						int len_result = strlen(result);
						if (result[len_result - 1] == '\n')
							len_result--;
						for (int k = 0; k < len_result; k++)
							out[out_index++] = result[k];
						delete [] result;
						delete [] thisline;
					} else {
						err("The `` system call failed.  Cannot access system.");
						delete [] cmd;
						return false;
					}
					i = j;
					break;
				} else {
					if (j == len - 1) {
						// If got to end, then was not a backtic
						// command after all, so copy the input
						strcpy(out, in);
						delete [] cmd;
						return true;
					}
				}
			}
		} else {
			out[out_index++] = in[i];
		}
	} // i;
	out[out_index] = '\0';
	delete [] cmd;
	return true;
#endif
}
#endif

// Substitute $() style system expressions, as shells
static bool
sub_dollar_paren(const char *in, std::string& out)
{
#ifdef DEBUG_DOLLAR_PAREN
	printf("sub_dollar_paren('%s', ...)\n", in);
#endif
	unsigned int len = strlen(in);
	if (len < 3) {
		out = in;
		return true;
	}
	out = "";
	std::string in_copy(in);
	bool inserted_something = false;
#ifdef DEBUG_DOLLAR_PAREN
	printf("LEN=%d\n",len);
#endif
	while (1) {
		std::vector<int> start;	// where \$( sequences start
		std::vector<int> depth;	// depth of these sequences
		int level = 0, max_level = 0;
		unsigned int i;
		for (i = 3; i < len; i++) {
#ifdef DEBUG_DOLLAR_PAREN
			printf("in_copy[%d,...] = '%s'\n", i, in_copy.c_str()+i);
#endif
			if (in_copy[i] == '(' && in_copy[i - 1] == '$' && in_copy[i - 2] == '\\') {
				start.push_back(i - 2);
				depth.push_back(++level);
#ifdef DEBUG_DOLLAR_PAREN
				printf("    got \\$( at i=%d\n",i);
#endif
			} else if (in_copy[i] == ')' && in_copy[i-1] != '\\') {
				level--;
#ifdef DEBUG_DOLLAR_PAREN
				printf("    got ) at i=%d\n",i);
#endif
			}
			if (level > max_level)
				max_level = level;
		}
#ifdef DEBUG_DOLLAR_PAREN
		printf("after loop, start.size() = %d    max_level= %d\n", start.size(), max_level);
#endif
		if (max_level == 0)
			break;
		for (i = 0; i < start.size(); i++) {
#ifdef DEBUG_DOLLAR_PAREN
			printf("%s:%d in loop.  depth[%d] = %d  start[i]= %d  '%s'\n",__FILE__,__LINE__,i,depth[i], start[i], in_copy.substr(start[i]).c_str());
#endif
			if (depth[i] == max_level) {
				inserted_something = true;
				// Process this one, the left-most maximumally nested case
				std::string res;
				int skip = dollar_paren(in_copy.c_str() + start[i], res);
				std::string tmp(in_copy.substr(0, start[i]));
#ifdef DEBUG_DOLLAR_PAREN
				printf("1. tmp='%s'  in_copy='%s'  start[%d]=%d\n", tmp.c_str(),in_copy.c_str(),i,start[i]);
#endif
				tmp = tmp + res;
#ifdef DEBUG_DOLLAR_PAREN
				printf("2. tmp '%s'\n", tmp.c_str());
#endif
				tmp = tmp + (in_copy.c_str() + start[i] + skip);
				in_copy = tmp;
				len = in_copy.size();
				for (i = 0; i < start.size(); i++) {
					start.pop_back();
					depth.pop_back();
				}
				break;
			}
		}
	}
	if (inserted_something == false)
		out = in;
	else
		out = in_copy;
	return true;
}

// Evaluate a single \$(CMD) sequence, *assumed* to not be nested.
//
// On input, 's' points to the starting backslash.
// On output, 'res' holds the result and the return value indicates
// the how many characters to skip in the input string, to 
// get to the matching ) character.
//
// BUG: 'res' is *assumed* to be long enough to hold system output
static int
dollar_paren(const char *s, std::string& res)
{	
#if !defined(HAVE_POPEN)
	err("Cannot do \\$(CMD) because computer lacks popen() C subroutine");
	return 0;
#else
#ifdef DEBUG_DOLLAR_PAREN
	printf("dollar_paren(%s,...)\n",s);
#endif
	// Search for closing paren
	std::string ss(s + 3); // so can insert null-terminate within
	unsigned int i;
#ifdef DEBUG_DOLLAR_PAREN
	printf("%s:%d error: dollar_paren() should count ( and then check ) in case sys command has some [%s]\n",__FILE__,__LINE__,ss.c_str());
#endif
	for (i = 0; i < ss.length(); i++) {
		if (ss[i] == ')') {
			ss.STRINGERASE(i);
			if (((unsigned) superuser()) & FLAG_SYS) {
				ShowStr("The $() mechanism is sending this to the OS:\n");
				ShowStr(ss.c_str());
				ShowStr("\n");
			}
			FILE *pipefile = (FILE *) popen(ss.c_str(), "r");
			if (!pipefile) {
				err("The $() system call failed. Cannot access system.");
				return -1;
			}
			char *thisline = new char[LineLength]; // assume enough space (without checking)
			if (!thisline) OUT_OF_MEMORY;
			while (NULL != fgets(thisline, LineLength_1, pipefile))
				res += thisline;
			pclose(pipefile);
			remove_trailing_blanks(res);
			delete [] thisline;
			break;		// done with this system-cmd
		} else if (i == ss.length() - 1) {
			err("Got to end-of-line with no ')' to match '\\$('");
			return -1;
		}
	}
#ifdef DEBUG_DOLLAR_PAREN
	printf("NOTE: dollar_paren returning %d  [%s]\n",i+4,res.c_str());
#endif
	return int(i + 4);		// 4 chars in embracing sequence

#endif
}

// expand_blanks() -- place 1 blank before each math token
void
expand_blanks(char *s)
{
	int             inquote = 0;
	char *sp;			// points to src 
	char *cp;			// points to modified copy 
	char last = '\0';		// last character 
	sp = s;
	cp = _grTempString;
	while (*sp != '\0') {
		if (*sp == '"') {
			if (inquote)
				inquote--;
			else
				inquote++;
		}
#if 1 // 2.060
		// Handle special case of synonym name with brace, 
		// e.g. \{some name} or \{time:unit} (as in netCDF)
		if (*sp == '{' && last == '\\') {
			do 
				*cp++ = *sp++;
			while (*sp != '}' && *sp && *sp != '\n');
			*cp++ = *sp++;
			last = *sp;
			continue;
		}
#endif
		// Handle e.g. ==, _-, <=, etc
		if (!inquote 
		    && *(sp+1) == '='
		    && (*sp    == '=' 
			|| *sp == '_'
			|| *sp == '^'
			|| *sp == '<'
			|| *sp == '>'
			|| *sp == '!'
			|| *sp == '/' )) {
			*cp++ = ' ';	// insert space before
			*cp++ = *sp++;	// the prefix char
			*cp++ = *sp;	// the '=' char
			*cp++ = ' ';	// insert space after
		} else if (!inquote
			   && *sp == '=' && *(sp + 1) == '"') {
			// e.g. read columns x="lon"
			*cp++ = ' ';
			*cp++ = *sp++;
			*cp++ = ' ';
			*cp++ = *sp;
			inquote++;
		} else if (!inquote
			   && (*sp == '+' || *sp == '-')
			   && (last    != 'e'
			       && last != 'E'
			       && last != 'd'
			       && last != 'D')) {
			*cp++ = ' ';
			*cp++ = *sp;
			if (*(sp + 1) == '=') { // += or -=
				*cp++ = '=';
				sp++;
			} else if (!isdigit(*(sp + 1)) && *(sp + 1) != '.') {
				// No extra space if next is a number
				*cp++ = ' ';	// modified 20 Apr 1995, vsn 2.036
			}
		} else if (!inquote
			   && (  *sp == '{'
				 || *sp == '}'
				 || *sp == '!'
				 || *sp == ',')) {
			*cp++ = ' ';
			*cp++ = *sp;
			*cp++ = ' ';
		} else {
			*cp++ = *sp;
		}
		last = *sp;
		sp++;
	}
	*cp = '\0';
	strcpy(s, _grTempString);
}

bool
superuserCmd()
{
	PUT_VAR("..superuser..", 1.0);
	return true;
}

bool
unsuperuserCmd()
{
	PUT_VAR("..superuser..", 0.0);
	return true;
}

// Get command line from file, storing result in null-terminated string (even
// if EOF).  The line is considered to end at a NEWLINE.
// Return true if got EOF
bool
get_cmd(char *buf, int max, FILE *fp)
{
	int             i = 0;
	int             thisc, lastc = 0;
	bool            in_quote = false;
	// Scan characters one by one 
	do {
		thisc = fgetc(fp);
		if (thisc == '"' && lastc != '\\')
			in_quote = in_quote ? false : true;
		// Check for EOF 
		if (thisc == EOF) {
			*(buf + i) = '\0';
			if (((unsigned) superuser()) & FLAG_AUT2) printf("%s:%d get_cmd hit EOF, now returning TRUE with [%s]\n",__FILE__,__LINE__,buf);
			return true;
		}
		// See if NEWLINE  (even if quoted) 
		if (thisc == '\n') {
			// Is newline to be ignored, for continuation? 
			if (lastc == '\\') {
				i -= 2;
				_cmdFILE.back().increment_line();
				continue;
			} else {
				// 2006-11-15 (thanks to SL for the patch)
#if defined(IS_OPENBSD)
				*(buf + i) = '\0';
#else
				if (*(buf + i - 1) == '\r') { // fix DOS
					*(buf + i - 1) = '\0';
				} else {
					*(buf + i) = '\0';
				}
#endif
				if (((unsigned) superuser()) & FLAG_AUT2) printf("%s:%d get_cmd IN MIDDLE returning FALSE with [%s]\n",__FILE__,__LINE__,buf);
				return false;
			}
		}
		// Assign ordinary character 
		*(buf + i) = thisc;
		lastc = thisc;
	} while (i++ < max);
	*(buf + i - 1) = '\0';	// didn't get to end - bad 
	printf("%s:%d get_cmd AT END returning FALSE with [%s]\n",__FILE__,__LINE__,buf);
	return false;
}
