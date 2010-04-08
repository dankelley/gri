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

//#define DEBUG_RESOLVE_PATH
//#define DEBUG_RE
//#define DEBUG_UNDERSCORE 

#include <string>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#if !defined(IS_MINGW32)
#include <pwd.h>
#include <paths.h>
#endif
#include <sys/types.h>

#if defined(MSDOS)		// need all these?
#include <stdlib.h>
#include <conio.h>
#include <dir.h>
#include <io.h>
#include <process.h>
#include <signal.h>
#endif

#include "extern.hh"
#include "debug.hh"
#include "defaults.hh"
#include "gr.hh"
#include "superus.hh"
#include "GriTimer.hh"

#ifdef DEBUG_RE
static void     show_pattern(const char *target, int tlen, int star, int plus);
#endif
extern double   strtod(const char *, char **);

bool
get_normal_number(const char *s, double *d)
{
	char *ptr = NULL;
	*d = strtod(s, &ptr);
	if (*ptr == '\0') {
		// Normal number; check for infinity/not-a-number
#if defined(HAVE_ISNAN) && defined(HAVE_ISINF)
#if !defined(__MACHTEN__)
		extern double _grMissingValue;
		if (isinf(*d) || isnan(*d)) {
		        extern bool _grMissingValueUsed;
		        if (!_grMissingValueUsed) // turn on missing-value, if it's off
			        gr_set_missing_value(1.0e22); 
			*d = (double) _grMissingValue;
		}
#endif
#endif
		return true;
	} else {
		return false;
	}
}

// This warning business is too hard.  Just ignore it for now.
#if 1
bool
get_number_with_underscores(const char *s, double *value)
{
	const char *si = s;
	static std::string ss;	// keep in storage 
	ss.assign("");
	unsigned int slen = strlen(s);
	if (!isdigit(*si) && *si != '.' && *si != '+' && *si != '-')
		return false;
	if (*si == '.' && !isdigit(*(si + 1)))
		return false;
	for (unsigned int i = 0; i < slen; i++) {
		if (isdigit(*si) || *si == '.' || *si == '-' || *si == '+'
		    || *si == 'e' || *si == 'E' || *si == 'd' || *si == 'D') {
			ss += *si;
		} else if (*si == '_') {
			;
		} else {
			return false;
		}
		si++;
	}
	if (get_normal_number(ss.c_str(), value))
		return true;
	else
		return false;
}
#else
bool
get_number_with_underscores(const char *s, double *value)
{
	const char *si = s;
	static std::string ss;	// keep in storage 
	ss.assign("");
	unsigned int slen = strlen(s);
	unsigned int last_underline = 0;
	bool have_underline = false;
	bool have_decimal = false;
	if (!isdigit(*si) && *si != '.' && *si != '+' && *si != '-')
		return false;
	if (*si == '.' && !isdigit(*(si + 1)))
		return false;
	for (unsigned int i = 0; i < slen; i++) {
		if (isdigit(*si) || *si == '-' || *si == '+') {
			ss += *si;
		} else if (*si == '.') {
			ss += *si;
			if (have_underline && ((i - 1 - last_underline) != 3))
				warning("misplaced _ before decimal place in `\\", s, "'", "\\");
			last_underline = i; // pretend
			have_underline = false;
			have_decimal = true;
		} else if (*si == 'e' || *si == 'E' || *si == 'd' || *si == 'D') {
			ss += *si;
			if (have_underline && ((i - 1 - last_underline) != 3))
				warning("misplaced _ before exponent indicator in `\\", s, "'", "\\");
			last_underline = i;
			have_underline = false;	// reset
		} else if (*si == '_') {
			if (have_underline && ((i -1 - last_underline) != 3))
				warning("misplaced _ in numerical constant `\\", s, "'", "\\");
			last_underline = i;
			have_underline = true;
		} else {
#ifdef DEBUG_UNDERSCORE
			printf("%s:%d NOT A NUMBER '%s'\n", __FILE__,__LINE__,s);
#endif
			return false;
		}
		si++;
	}
#ifdef DEBUG_UNDERSCORE
	printf("%s:%d last_underscore %d    end at %d\n", __FILE__,__LINE__,last_underline, slen);
#endif
	if (have_underline && (slen - 1 - last_underline != 3)) {
		warning("misplaced _ in constant `\\", s, "'", "\\");
	}
#ifdef DEBUG_UNDERSCORE
	printf("%s:%d OK, translated '%s' -> '%s'\n", __FILE__,__LINE__,s,ss.c_str());
#endif
	if (get_normal_number(ss.c_str(), value)) {
#ifdef DEBUG_UNDERSCORE
		printf("%s:%d Right this is a number '%s' -> '%s' -> %e\n", __FILE__,__LINE__,s,ss.c_str(), *value);
#endif
		return true;
	}
#ifdef DEBUG_UNDERSCORE
	printf("%s:%d Not a number '%s' -> '%s'\n", __FILE__,__LINE__,s,ss.c_str());
#endif
	return false;
}
#endif

bool
get_coded_value(const std::string& name, int level, std::string& result)
{
	//printf("DEBUG %s:%d get_coded_value(<%s>,%d)\n",__FILE__,__LINE__,name.c_str(),level);
	int mark_above = level + 1;
	if (name[0] == '.') {
		int index = index_of_variable(name.c_str(), level);
		//printf("DEBUG %s:%d index %d\n",__FILE__,__LINE__,index);
		if (index < 0) {
			// No such variable known
			return false;
		}
		char buf[100]; // BUG: could be too short
		sprintf(buf, "%g", variableStack[index].get_value());
		result.assign(buf);
		return true;
	} else if (name[0] == '\\'){
		int mark = 0;
		int index;
		for (index = 0; index < int(synonymStack.size()); index++) {
			const char *n = synonymStack[index].get_name();
			if (*n == '\0')
				if (++mark == mark_above)
					break;
		}
		if (mark != mark_above) {
			//printf("DEBUG %s:%d no match for <%s>\n",__FILE__,__LINE__,name.c_str());
			return false;
		}
		//printf("DEBUG %s:%d index %d\n",__FILE__,__LINE__,index);
		for (int i = index - 1; i >= 0; i--) {
			if (synonymStack[i].get_name() == name) {
				//printf("DEBUG %s:%d match at i= %d\n",__FILE__,__LINE__,i);
				result.assign(synonymStack[i].get_value());
				return true;
			}
		}
	} else {
		err("Internal error in synonyms.cc; cannot decode `\\", name.c_str(), "'", "\\");
		return false;
	}
        return true;
}

bool 
is_coded_string(const std::string&s, std::string& name, int* mark_level)
{
	//printf("DEBUG %s:%d is_coded_string <%s>\n",__FILE__,__LINE__,s.c_str());

	//for (unsigned int i = 0; i < s.size(); i++) printf("\ts[%d] = '%c'\n",i,s[i]);

	if (s.size() < 33) { // this long even if var and level each have only 1 character
		return false;
	}
	char cname[100];	// BUG: may be too short
	int ml = 0;
	if (2 != sscanf(s.c_str(), AMPERSAND_CODING, cname, &ml)) {
		//printf("DEBUG: %s:%d problem ... cname [%s]  level %d\n",cname, ml);
		return false;
	}
	name.assign(cname);
	*mark_level = ml;
	//printf("DEBUG %s:%d decoded name <%s> level %d\n",__FILE__,__LINE__,name.c_str(),*mark_level);
	return true;
}


bool
marker_draw() // put a marker on top
{
	GriVariable markVar("", 0.0);
	variableStack.push_back(markVar);
	GriSynonym markSyn("", "");
	synonymStack.push_back(markSyn);
	return true;
}
int
marker_count() // -1 if error
{
	int nv = 0;
	int vlen = variableStack.size();
	for (int i = vlen - 1; i >= 0; i--)
		if (*variableStack[i].get_name() == '\0')
			nv++;

	int ns = 0;
	int slen = synonymStack.size();
	for (int ii = slen - 1; ii >= 0; ii--)
		if (*synonymStack[ii].get_name() == '\0')
			ns++;
	if (ns == nv)
		return ns;
	return -1;		// error
}
bool
marker_erase() // erase top marker; return false if there is none
{
	bool ok = false;
	int vlen = variableStack.size();
	for (int i = vlen - 1; i >= 0; i--) {
		if (*variableStack[i].get_name() == '\0') {
			variableStack.erase(variableStack.begin() + i);
			ok = true;
			break;
		}
	}
	if (ok) {
		ok = false;
		int slen = synonymStack.size();
		for (int i = slen - 1; i >= 0; i--) {
			if (*synonymStack[i].get_name() == '\0') {
				synonymStack.erase(synonymStack.begin() + i);
				ok = true;
				break;
			}
		}
	}
	return ok;
}

// Keep this in static area, so as not to waste time with
// multiple allocation/deallocation for scratch strings
static std::string tmp_string;

bool
get_nth_word(const std::string& s, unsigned int which, std::string& result)
{
	char *w[MAX_nword];	// BUG: wasteful
	unsigned int nw;
	char *cpy = strdup(s.c_str());
	chop_into_words(cpy, w, &nw, MAX_nword);
	if (nw <= which)
		return false;
	result.assign(w[which]);
	free(cpy);
	return true;
}

unsigned int
get_number_of_words(const std::string& s)
{
	if (s.size() == 0)
		return 0;
	char *w[MAX_nword];	// BUG: wasteful
	unsigned int nw;
	char *cpy = strdup(s.c_str());
	chop_into_words(cpy, w, &nw, MAX_nword);
	free(cpy);
	return nw;

}


bool
is_assignment_op(const char *s)
{
	if (!strcmp(s, "="))
		return true;
	if (!strcmp(s, "+="))
		return true;
	if (!strcmp(s, "*="))
		return true;
	if (!strcmp(s, "-="))
		return true;
	if (!strcmp(s, "/="))
		return true;
	if (!strcmp(s, "^="))
		return true;
	if (!strcmp(s, "_="))
		return true;
	return false;
}


// Return true if string is all space/tab chars
bool
string_is_blank(const char *s)
{
	int len = strlen(s);
	for (int i = 0; i < len; i++)
		if (!isspace(s[i]))
			return false;
	return true;
}

// Returns number column data that are missing in *any* variable
unsigned int
number_missing_cols()
{
	// Only examine other columns if they have same length
	unsigned int length = _colX.size();
	bool do_y      = _colY.size()      == length;
	bool do_z      = _colZ.size()      == length;
	bool do_U      = _colU.size()      == length;
	bool do_V      = _colV.size()      == length;
	bool do_WEIGHT = _colWEIGHT.size() == length;
	unsigned int missing = 0;
	for (unsigned int i = 0; i < length; i++) {
		if (             gr_missing(_colX[i]))      { missing++; continue; }
		if (do_y      && gr_missing(_colY[i]))	    { missing++; continue; }
		if (do_z      && gr_missing(_colZ[i]))	    { missing++; continue; }
		if (do_U      && gr_missing(_colU[i]))	    { missing++; continue; }
		if (do_V      && gr_missing(_colV[i]))	    { missing++; continue; }
		if (do_WEIGHT && gr_missing(_colWEIGHT[i])) { missing++; continue; }
	}
	return missing;
}

/* Return true if string starts and ends with character `"' */
bool
quoted(const char *s)
{
	int len = strlen(s);
	if (len < 2)
		return false;
	return (*s == '"' && *(s + len - 1) == '"') ? true : false;
}

/* Return transformed -- see code that calls it. */
double
quantize(double x, int levels, double dx)
{
	if (dx < 0.0)
		dx = -dx;
	if (dx)
		return (dx * levels / (1.0 + levels) * (floor(1.0 + x / dx)));
	else
		return x;
}

/* Change escaped quotes in word to quotes.  Strlen changed! */
void
remove_esc_quotes(char *w)
{
	int             i, max = strlen(w);
	char            last;
	last = ' ';
	for (i = 0; i < max; i++) {
		if (w[i] == '\\' && w[i + 1] == '"' && last != '\\') {
			/* gobble rest up */
			int             j;
			for (j = i; j < max; j++) {
				w[j] = w[j + 1];
			}
		} else {
			last = w[i];
		}
	}
}

void
show_words()
{
	if (_nword > 0) {
		printf("DEBUG: ");
		for (unsigned int i = 0; i < _nword; i++) {
			printf("%d\"%s\" ", i, _word[i]);
		}
		printf("\n");
	} else {
		printf("DEBUG: commandline is blank\n");
	}

}

// Is the i-th word of _word[] equal to given word? (return 0 if too few
// words)
bool
word_is(int i, const char *word)
{
	return ((-1 < i && i < int(_nword) && !strcmp(word, _word[i])) ? true : false);
}

void
check_psfile()
{
	extern output_file_type _output_file_type;
	if (_output_file_type == postscript) {
		extern FILE *_grPS;
		if (ferror(_grPS)) {
			/* never returns */
			fatal_err(" IO error on PostScript output file\n");
		}
	}
}

bool
delete_file(const char *filename)
{
#if defined(HAVE_UNISTD_H)
	return !(unlink(filename));
#else  // will have to delete it 'manually'
	char            sys_cmd[200];
#if defined(VMS)
	sprintf(sys_cmd, "DEL %s;*", filename);
	call_the_OS(sys_cmd, __FILE__, __LINE__);
#elif defined(MSDOS)
	sprintf(sys_cmd, "DEL %s", filename);
	call_the_OS(sys_cmd, __FILE__, __LINE__);
#else
	sprintf(sys_cmd, "rm %s", filename);
	call_the_OS(sys_cmd, __FILE__, __LINE__);
#endif
#endif // whether have unlink
	return true;
}

/*
 * skip_space () -- return number of spaces at start of string. (A space may
 * be SPC, TAB, etc.)
 */
int
skip_space(const char *s)
{
	int             i = 0;
	while (isspace(*(s + i)) && *(s + i) != '\0')
		i++;
	return i;
}

// skip_nonspace () -- return number of non-spaces at start of string. (A
// space may be SPC, TAB, etc.)
int
skip_nonspace(const char *s)
{
	int             i = 0;
	while (!isspace(*(s + i)) && *(s + i) != '\0')
		i++;
	return i;
}

// 'get env \a SHELL'
bool
get_envCmd()
{
	if (_nword != 4) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	std::string the_syn(_word[2]);
	un_double_quote(the_syn);
	un_double_slash(the_syn);
	de_reference(the_syn);
	if (!is_syn(the_syn)) {
		err("No synonym name given");
		demonstrate_command_usage();
		return false;
	}
	std::string the_env_var(_word[3]);
	un_double_quote(the_env_var);
	de_reference(the_env_var);
	char *result = egetenv(the_env_var.c_str());
	if (result == NULL) {
		if (!put_syn(the_syn.c_str(), "", true)) {
			gr_Error("Ran out of storage");
			return false;
		}
		warning("No environment variable called `\\", the_env_var.c_str(), "' exits.", "\\");
	} else {
		if (!put_syn(the_syn.c_str(), result, true)) {
			gr_Error("Ran out of storage");
			return false;
		}
	}
	return true;
}


/* Paste a single character onto the end of a string. */
void
strcat_c(char *s, int c)
{
	int             slen = strlen(s);
	s[slen] = c;
	s[++slen] = '\0';
}

bool
is_punctuation(int c)
{
	return ((c == ' ' || c == '\t' ||
		 c == '!' ||
		 c == '@' ||
		 c == '#' ||
		 c == '$' ||
		 c == '%' ||
		 c == '^' ||
		 c == '&' ||
		 c == '*' ||
		 c == '(' || c == ')' ||
		 c == '-' ||
		 c == '+' || c == '=' ||
		 c == '~' || c == '`' ||
		 c == '{' || c == '[' ||
		 c == '}' || c == ']' ||
		 c == '\\' ||
		 c == ':' || c == ';' ||
		 c == ',' || c == '<' ||
		 c == '"' || c == '<' ||
		 c == '.' || c == '>' ||
		 c == '/' || c == '?'
		) ? true : false);
}

bool
full_path_name(std::string& f)
{
	if (f[0] == '~') {
		if (f[1] == '/') {
			f.STRINGERASE(0, 1);
			f.insert(0, egetenv("HOME"));
			return true;
		} else {
#if !defined(IS_MINGW32)
			size_t name_end = f.find("/");
			if (name_end == STRING_NPOS)
				name_end = f.size();
			std::string username = f.substr(1, name_end - 1);
			struct passwd *pw_entry;
			pw_entry = getpwnam(username.c_str());
			f.STRINGERASE(0, username.size() + 1);
			f.insert(0, pw_entry->pw_dir);
			return true;
#else
			return false;
#endif
		}
	} else if (f[0] == '.') {
		char wd[1024], *ptr = wd; // BUG: may not be long enough
		ptr = getcwd(ptr, 1023);
		if (ptr) {
			f.STRINGERASE(0, 1);
			f.insert(0, wd);
			//printf("GOT DIR %s\n", wd); 
		} else {
			;
			//printf("CANNOT get cwd\n");
		}
	}
	return true;
}

bool
resolve_filename(std::string& f, bool trace_path, char c_or_d)
{
	unsigned int len;
	if (f[0] == '"') {
		f.STRINGERASE(0,1);
		len = f.size();
		if (len < 2)
			return false;
		if (f[len - 1] != '"')
			return false;
		f.STRINGERASE(len-1, 1);
	}
	// Change any escaped quotes to quotes (not sure why
	// anybody would do this, but what the heck).
	unsigned int i;
	len = f.size();
	for (i = 1; i < len; i++) {
		if (f[i] == '"' && f[i-1] == '\\') {
			f.STRINGERASE(i - 1, 1);
			if (--len == 0)
				return false;
		}
	}
	if (f[0] == '~') {
		if (f[1] == '/') {
			f.STRINGERASE(0, 1);
			f.insert(0, egetenv("HOME"));
			return true;
		} else {
#if !defined(IS_MINGW32)
			size_t name_end = f.find("/");
			if (name_end == STRING_NPOS)
				name_end = f.size();
			std::string username = f.substr(1, name_end - 1);
			struct passwd *pw_entry;
			pw_entry = getpwnam(username.c_str());
			f.STRINGERASE(0, username.size() + 1);
			f.insert(0, pw_entry->pw_dir);
			return true;
#else
			return false;
#endif
		}
	}

	// BUG: probably should substitute any env-vars here, e.g. $HOME

	// Done, unless we have to trace the path ...
	if (!trace_path)
		return true;

	// ... but can even skip that, if the pathname is complete already ...
	if (f[0] == '.' || f[0] == '/')
		return true;

	// ... ok, now we know we should trace!
	std::string path;
	if (c_or_d == 'c') {
		if (!get_syn("\\.path_commands.", path)) {
			err("Internal error in utility.cc:resolve_filename() -- cannot determine value of \\.path_commands\n");
			return false;
		}
	} else if (c_or_d == 'd') {
		if (!get_syn("\\.path_data.", path)) {
			err("Internal error in utility.cc:resolve_filename() -- cannot determine value of \\.path_data\n");
			return false;
		}
	} else {
		err("Internal error in utility.cc:resolve_filename() -- c_or_d has unacceptable value\n");
		return false;
	}
	if (path.size() < 1)
		return true;	// BUG: is this what I want for empty path?
	// HAD: path.assign(GRIINPUTS); // in defaults.hh ".:/usr/local/lib/gri"
	std::string::size_type start = 0;
	std::string::size_type colon;
#ifdef DEBUG_RESOLVE_PATH
	printf("DEBUG(%s:%d) resolve_filename has path '%s'\n",__FILE__,__LINE__,path.c_str());
#endif
	do {
		colon = path.find(":", start);
#ifdef DEBUG_RESOLVE_PATH
		printf("DEBUG(%s:%d) resolve_filename top of loop colon=%d  start=%d string='%s'\n",__FILE__,__LINE__,int(colon),int(start),path.c_str()+start);
#endif
		std::string test_file = path.substr(start, colon - start);
#ifdef DEBUG_RESOLVE_PATH
		printf("DEBUG(%s:%d) resolve_filename isolated    colon=%d  start=%d string='%s'\n",__FILE__,__LINE__,int(colon),int(start),test_file.c_str());
#endif
		test_file.append("/");
		test_file.append(f);
#ifdef DEBUG_RESOLVE_PATH
		printf("DEBUG(%s:%d) resolve_filename trying file named '%s'\n", __FILE__,__LINE__,test_file.c_str());
#endif
		FILE *fp = fopen(test_file.c_str(), "r");
		if (fp != NULL) {
			fclose(fp);
			f = test_file;
			return true;
		}
		start = colon + 1; // skip the ':'
	} while (colon != STRING_NPOS);

	// Well, we just can't find this file.  Too bad.
	return false;
}



char *
pwd()
{
#if HAVE_GETCWD
	static char msg[1024];
	if (NULL == getcwd(msg, 1024))
		return (char *)"";
	else
		return msg;
#elif defined(VMS)		/* vms version braindead */
	return "";
#elif defined(MSDOS)		/* msdos version braindead */
	return "";
#elif !defined(HAVE_POPEN)
	//err("Cannot do `pwd' because computer lacks popen() subroutine");
	return "";
#else
	char            msg[1024];
	FILE           *pipefile = (FILE *) popen("pwd", "r");
	if (pipefile) {
		char *result;
		if (1 == fscanf(pipefile, "%s", msg)) {
			pclose(pipefile);
			result = new char[1 + strlen(msg)];
			if (!result) OUT_OF_MEMORY;
			strcpy(result, msg);
			return result;
		} else {
			pclose(pipefile);
			warning("Can't determine name of working directory; using `.' instead.");
			result = new char[2];
			if (!result) OUT_OF_MEMORY;
			strcpy(result, ".");
			return result;
		}
	} else {
		return ".";
	}
#endif
}

char*
egetenv(const char *s)
{
#if defined(HAVE_GETENV)
	char *rval = (char *)"";
#endif
	if (!strcmp(s, "PWD")) {
		return (char *) pwd();
	} else if (!strcmp(s, "USER")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return (char *)"";
		else
			return rval;
#else
		return (char *)"unknown";
#endif
	} else if (!strcmp(s, "SYSTEM")) {
#if defined(VMS)
		return (char *)"vax";
#elif defined(MSDOS)
		return (char *)"msdos";
#else
		return (char *)"unix";
#endif
	} else if (!strcmp(s, "HOST")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return (char *)"";
		else
			return rval;
#else
		return (char *)"unknown";
#endif
	} else if (!strcmp(s, "HOME")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return (char *)"";
		else
			return rval;
#else
		return (char *)"unknown";
#endif
	} else if (!strcmp(s, "PAGER")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return (char *)"";
		else
			return rval;
#else
		return (char *)"unknown";
#endif
	} else if (!strcmp(s, "GRIINPUTS")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return (char *)"";
		else
			return rval;
#else
		return "";
#endif
	} else {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return (char*)"";
		else
			return rval;
#else
		return "unknown";
#endif
	}
}

/* sprintfCmd() -- print things into a string */
#define N 20
bool
sprintfCmd()
{
	char           *fmt;
	char            msg[1024];
	double          x[N];
	if (_nword < 4) {
		err("`sprintf \\synonym \"format\"' requires variable list to process");
		return false;
	}
	*(_word[2] + strlen(_word[2]) - 1) = '\0';
	fmt = _word[2] + 1;
	for (unsigned int i = 3; i < _nword; i++) {
		getdnum(_word[i], &x[i - 3]);
	}
	switch (_nword) {
	case 4:
		sprintf(msg, fmt,
			x[0]);
		break;
	case 5:
		sprintf(msg, fmt,
			x[0], x[1]);
		break;
	case 6:
		sprintf(msg, fmt,
			x[0], x[1], x[2]);
		break;
	case 7:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3]);
		break;
	case 8:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4]);
		break;
	case 9:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5]);
		break;
	case 10:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6]);
		break;
	case 11:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7]);
		break;
	case 12:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8]);
		break;
	case 13:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9]);
		break;
	case 14:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10]);
		break;
	case 15:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11]);
		break;
	case 16:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12]);
		break;
	case 17:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13]);
		break;
	case 18:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13], x[14]);
		break;
	case 19:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13], x[14],
			x[15]);
		break;
	case 20:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13], x[14],
			x[15], x[16]);
		break;
	case 21:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13], x[14],
			x[15], x[16], x[17]);
		break;
	case 22:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13], x[14],
			x[15], x[16], x[17], x[18]);
		break;
	case 23:
		sprintf(msg, fmt,
			x[0], x[1], x[2], x[3], x[4],
			x[5], x[6], x[7], x[8], x[9],
			x[10], x[11], x[12], x[13], x[14],
			x[15], x[16], x[17], x[18], x[19]);
		break;
	default:
		err("Can only do `sprintf' for 1-20 variables");
		return false;
	}
	std::string w1(_word[1]); 
	de_reference(w1);
	if (!is_syn(w1)) {
		demonstrate_command_usage();
		err("Second word of command must be a synonym-name'");
		return false;
	}
	if (!put_syn(w1.c_str(), msg, true)) {
		gr_Error("Ran out of storage");
		return false;
	}
	return true;
}

#undef N

/*
 * get (double) values from a list of words (typically the words in the cmd
 * line).  Typically this is used in getting optional parameters from
 * commands, and _dstack[] is used for objects. RETURN number of objects
 * actually found
 */
int
get_cmd_values(char ** w, int nw, const char *key, int nobjects, double *objects)
{
	int             istart = -1, i, found = 0, iobject;
	if (nobjects > _num_dstackMAX)
		return 0;
	for (i = 0; i < nw; i++)
		if (!strcmp(w[i], key)) {
			istart = i + 1;
			break;
		}
	if (istart == -1)
		return 0;		/* 'key' not present */
	if (istart + nobjects > nw)
		return -1;		/* too few */
	for (i = istart, iobject = 0; iobject < nobjects; i++, iobject++) {
		if (true != getdnum(w[i], &objects[iobject]))
			return found;
		found++;
	}
	return found;
}

char
last_character(const char *s)
{
	return s[strlen(s) - 1];
}

// getinum -- return 0 if can't get number
bool
getinum(const char *s, int *i)
{
	if (*s == '\0')
		return 0;
	extern double   _grMissingValue;
	double          d;
	char           *ptr = NULL;
	d = _grMissingValue;
	*i = (int) strtod(s, &ptr);
	if (*ptr == '\0') {
		// It was a normal number with no problems in reading.
		return true;
	}
	// Cannot read as a normal number.  Check to see if it's a variable or
	// NaN/Inf.
	if (is_var(s)) {
		if (get_var(s, &d)) {
			*i = (int) d;
			return true;
		} else {
			if (!skipping_through_if()) {
				err("variable `\\", s, "' is not defined yet", "\\");
				return false;
			} else {
				*i = (int) _grMissingValue;
				return true;
			}
		}
	} else if (is_syn(s)) {
		std::string syn_value;
		bool exists = get_syn(s, syn_value);
		//printf("DEBUG %s:%d '%s' exists= %d  value [%s]\n",__FILE__,__LINE__,s,exists,syn_value.c_str());
		if (exists) {
			const char* vptr = syn_value.c_str();
			ptr = NULL;	// reset this
			*i = int(strtod(vptr, &ptr));
			if (*ptr == '\0') {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else if (!strcmp(s, "-NaN") 
		   || !strcmp(s, "NaN") 
		   || !strcmp(s, "Inf")
		   || !strcmp(s, "Infinity")
		   || !strcmp(s, "-Inf") 
		   || !strcmp(s, "-Infinity") 
		) {
		*i = (int) (_grMissingValue);
		return true;
	} else {
		// Maybe it's a fortran 'D' exponential
		tmp_string.assign(s);
		int loc;
		loc=tmp_string.find('d'); if (-1!=loc) tmp_string.replace(loc,1,"e");
		loc=tmp_string.find('D'); if (-1!=loc) tmp_string.replace(loc,1,"e");
		*i = (int)strtod(tmp_string.c_str(), &ptr);
		if (*ptr == '\0')
			return true;
	}
	// It's a mystery.  report error, fatal or nonfatal depending on
	// whether the user is interested in errors.
	*i = (int) _grMissingValue;
	if (!_ignore_error) {
		if (isalpha(s[1])) {
			err("Cannot interpret `\\",
			    s,
			    "' as a number.\n       Did you forget the final \".\" in a variable name?",
			    "\\");
		} else {
			err("Cannot interpret `\\",
			    s,
			    "' as a number.",
			    "\\");
		}
	}
	return false;
}

// getdnum -- return 0 if can't get number
bool
getdnum(const char *s, double *d)
{
	if (*s == '\0')
		return false;
	if (get_normal_number(s, d)) {
#ifdef DEBUG_UNDERSCORE
		printf("%s:%d normal number '%s' = %e\n", __FILE__,__LINE__,s, *d);
#endif
		return true;
	}
	if (get_number_with_underscores(s, d)) {
#ifdef DEBUG_UNDERSCORE
		printf("%s:%d decoded underline '%s' to be %e\n", __FILE__,__LINE__,s, *d);
#endif
		return true;
	}
#ifdef DEBUG_UNDERSCORE
	printf("%s:%d OK, not any kinda number '%s'\n", __FILE__,__LINE__,s);
#endif
	// Cannot read as a normal number.  Check to see if it's a variable or
	// NaN/Inf.
	extern double _grMissingValue;
	if (is_var(s)) {
		if (get_var(s, d)) {
			return true;
		} else {
			if (!skipping_through_if()) {
				err("variable `\\", s, "' is not defined yet", "\\");
				return false;
			} else {
				*d = _grMissingValue;
				return true;
			}
		}
#if 1				// vsn 2.6.0 [2001-feb-18]
	} else if (is_syn(s)) {
		std::string syn_value;
		bool exists = get_syn(s, syn_value);
		//printf("DEBUG %s:%d '%s' exists= %d  value [%s]\n",__FILE__,__LINE__,s,exists,syn_value.c_str());
		if (exists) {
			const char* vptr = syn_value.c_str();
			char *ptr;
			ptr = NULL;	// reset this
			*d = strtod(vptr, &ptr);
			if (*ptr == '\0') {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
#endif
	} else if (!strcmp(s, "-NaN") || !strcmp(s, "NaN") || !strcmp(s, "-Inf") || !strcmp(s, "Inf")) {
		*d = _grMissingValue;
		return true;
	} else {
		// Maybe it's a fortran 'D' exponential
		tmp_string.assign(s);
		int loc;
		loc=tmp_string.find('d'); if (-1!=loc) tmp_string.replace(loc,1,"e");
		loc=tmp_string.find('D'); if (-1!=loc) tmp_string.replace(loc,1,"e");
		char *ptr;
		*d = (double)strtod(tmp_string.c_str(), &ptr);
		if (*ptr == '\0')
			return true;
	}
	// It's a mystery.  report error, fatal or nonfatal depending on
	// whether the user is interested in errors.
	*d = (double) _grMissingValue;
	if (!_ignore_error) {
		if (isalpha(s[1])) {
			err("Cannot interpret `\\",
			    s,
			    "' as a number.\n       Did you forget the final \".\" in a variable name?",
			    "\\");
		} else {
			err("Cannot interpret `\\",
			    s,
			    "' as a number.",
			    "\\");
		}
	}
	return false;
}

/*
 * fatal_err () -- print error message.  If the first (string) argument ends
 * in '\\', then several strings follow, ended by a string consisting only of
 * "\\".
 */
void
fatal_err(const char *str,...)
{
	char            msg[1024];
	bool            several = false;
	int             len;
	char *            p;
	va_list         ap;
	if (!_error_in_cmd && _gri_beep)
		gr_textput("\007");

	if (str != NULL) {
		va_start(ap, str);
		strcpy(msg, str);
		len = strlen(msg);
		if (msg[len - 1] == '\\') {
			msg[len - 1] = '\0';
			several = true;
		}
		gr_textput("FATAL ERROR: ");
		do {
			gr_textput(msg);
			if (several) {
				p = va_arg(ap, char *);
				strcpy(msg, p);
			}
		}
		while (several && strcmp(msg, "\\"));
		gr_textput("\n");
		_error_in_cmd = true;
		va_end(ap);
	}
	//
	// Print file:line so emacs mode can locate error
	//
	if (superuser() & FLAG_AUT1) {
		// Remove quotes around source indicator
		extern char source_indicator[];
		sprintf(msg, " Error at %s\n", source_indicator);
	} else {
		if (block_level() > 0) {
			if (block_source_file() != NULL) {
				if (superuser() & FLAG_AUT1)printf("DEBUG utility.cc:fatal_err(): block_source_line()=%d\n",block_source_line());
				sprintf(msg,
					" Error at %s:%d\n",
					block_source_file(),
					block_source_line() - 1);
				gr_textput(msg);
			}
		} else {
			if (what_file() != NULL) {
				sprintf(msg,
					" Error at %s:%d\n",
					what_file(),
					what_line());
				gr_textput(msg);
			}
		}
	}
	if (_error_action == 1)
		gri_abort();
	else
		gri_exit(1);
}

const char *
what_file()
{
	if (_cmdFILE.size() == 0)
		return NULL;
	if (block_level() > 0) {
		if (_cmd_being_done < 1) {
			return NULL;
		} else {
			return _command[_cmd_being_done_code[_cmd_being_done - 1]].filename;
		}
	} else {
		return _cmdFILE.back().get_name();
	}
}

int
what_line()
{
	if (block_level() > 0) {
		if (_cmd_being_done < 1) {
			return -1;		/* note: what_file() will give NULL */
		} else {
			return
				_command[_cmd_being_done_code[_cmd_being_done - 1]].fileline
				+ block_offset_line() - 1;
		}
	} else {
		return _cmdFILE.back().get_line();
	}
}

/*
 * err () -- print error message.  If the first (string) argument ends in
 * '\\', then several strings follow, ended by a string consisting only of
 * "\\".
 */
void
err(const char *str,...)
{
	char            msg[1024];
	bool            several = false;
	int             len;
	char *            p;
	va_list         ap;
	if (!_error_in_cmd && _gri_beep)
		gr_textput("\007");
	if (str != NULL) {
		va_start(ap, str);
		strcpy(msg, str);
		len = strlen(msg);
		if (msg[len - 1] == '\\') {
			msg[len - 1] = '\0';
			several = true;
		}
		gr_textput("ERROR: ");
		do {
			gr_textput(msg);
			if (several) {
				p = va_arg(ap, char *);
				strcpy(msg, p);
			}
		}
		while (several && strcmp(msg, "\\"));
		gr_textput("\n");
	}
	_error_in_cmd = true;
	va_end(ap);
}

// warning () -- print warning message.  All arguments must be char*.
// If the first argument ends in '\\', then several strings follow,
// ended by a string consisting only of "\\".  If only string is '\\',
// then this is a request to notify of number of repeats of last msg.
void
warning(const char *s,...)
{
	static          unsigned int msg_last_copies = 0;
	static std::string   msg_last;	// must be static
	static std::string msg;		// static only for efficiency if called a lot
	bool            several = false;
	int             len;
	char           *p = NULL;
	va_list         ap;
	if (!_error_in_cmd && _gri_beep)
		gr_textput("\007");
	// Check for final wrap-up command
	if (!strcmp(s, "\\\\")) {
		if (msg_last_copies > 0) {
			char buffer[100];
			sprintf(buffer,
				"         ... this warning was repeated %d times.\n",
				++msg_last_copies);
			gr_textput(buffer);
		}
		return;
	}
	if (s != NULL) {
		va_start(ap, s);
		len = strlen(s);
		msg = s;
		if (msg[len - 1] == '\\') {
			msg.STRINGERASE(len - 1, 1);
			several = true;
		}
		do {
			if (several) {
				p = va_arg(ap, char *);
				if (strcmp(p, "\\"))
					msg += p;
			}
		} while (several && strcmp(p, "\\"));
		if (msg == msg_last) {
			// Same as last msg.  Collect till a new one.
			msg_last_copies++;
		} else {
			// Not same as last msg.
			if (msg_last_copies > 0) {
				// This msg repeated
				char buffer[100];
				sprintf(buffer,
					"Warning: (The last warning was repeated %d times.)\n",
					msg_last_copies + 1);
				gr_textput(buffer);
				// Now show current msg
				gr_textput("Warning: ");
				gr_textput(msg.c_str());
				gr_textput("\n");
			} else {
				// First time for this msg
				gr_textput("Warning: ");
				gr_textput(msg.c_str());
				gr_textput("\n");
			}
			msg_last_copies = 0;
		}
		msg_last = msg;
	} va_end(ap);
}

/*
 * ExtractQuote() -- extract quote `sout' from string `s'. NOTE: You must
 * ensure sout is as long as s. RETURN VALUE: character position at end of
 * extracted quote, if all was OK.  Otherwise 0.
 *
 * Accepts both quoted strings '... "hi" ...' and unquoted strings '... \"hi\" ...'
 * but not combinations.
 */
// RETURN VALUE:
//      0  if no quoted thing found
//     -1  if missing final-quote
//     >0  OK; value 'i' is such that s[i] is just after the final quote
int
ExtractQuote(const char *s, std::string& sout)
{
	//printf("DEBUG %s:%d input string is '%s'\n",__FILE__,__LINE__,s);
	int i = 0;
	bool slash_quoted = false;
	// Skip along to first quote ...
	while (s[i] != '"') {
		if (s[i] == '\0')
			return 0; // never found any quoted items
		i++;
	}
	if (i > 0 && s[i - 1] == '\\')
		slash_quoted = true;

	i++;			// skip the quote
	// ... then copy along until find first un-escaped quote ...
	while (s[i] != '\0') {
		if (s[i] == '"') {
			if (i > 0 && s[i - 1] == '\\') {
				if (!slash_quoted) {
					sout += '"';
				} else {
					sout.STRINGERASE(sout.size() - 1, 1); // trim it
					//printf("DEBUG %s:%d RETURNING-A %d.  Extracted quote length %d as \n<%s>\n", __FILE__,__LINE__,i+1,sout.size(),sout.c_str());
					return i + 1;
				}
			} else {
				//printf("DEBUG %s:%d RETURNING-B %d\n<%s>\n\n",__FILE__,__LINE__,i+1,sout.c_str());
				return i + 1;
			}
		} else {
			sout += s[i];
		}
		i++;
	}
	if (s[i] == '\0') {
		//printf("DEBUG %s:%d RETURNING-C -1.  Extracted quote length %d as \n<%s>\n", __FILE__,__LINE__,sout.size(),sout.c_str());
		return -1;
	}

	// ... and return an index so the parser can do more
	// work on 's' past the quote
	//printf("DEBUG %s:%d RETURNING %d.  Extracted quote length %d as \n<%s>\n", __FILE__,__LINE__,i,sout.size(),sout.c_str());
	return i;
}

// Make all trailing blanks, tabs, etc, into null chars
void
remove_trailing_blanks(char *s)
{
	int             i = strlen(s);
	while (--i > -1)
		if (isspace(s[i]))
			s[i] = '\0';
		else
			break;
}
// Make all trailing blanks, tabs, etc, into null chars
void
remove_trailing_blanks(std::string& s)
{
	int i = s.length();
	while (--i > -1)
		if (!isspace(s[i]))
			break;
	//printf("BEFORE removing trailing blanks '%s' ... ", s.c_str());
	s.STRINGERASE(i + 1);
	//printf("AFTER '%s' ... ", s.c_str());
}

void
beep_terminal()
{
	if (_gri_beep)
		fprintf(stderr, "\007");
}

/* matrix_limits() - find min/max of matrix */
void
matrix_limits(double *min, double *max)
{
	bool            first = true;
	double          f;
	*min = *max = 0.0;
	for (unsigned int c = 0; c < _num_xmatrix_data; c++) {
		for (unsigned int r = 0; r < _num_ymatrix_data; r++) {
			if (_legit_xy(c, r) == true) {
				f = _f_xy(c, r);
				if (first == true) {
					*min = *max = f;
					first = false;
				}
				if (f < *min)
					*min = f;
				if (*max < f)
					*max = f;
			}
		}
	}
	if (first == true) {
		*min = gr_currentmissingvalue();
		*max = gr_currentmissingvalue();
	}
}

/*
 * inside_box - see if data point inside clip box DESCRIPTION Returns 1 if
 * either: (1) clipping turned off or	(2) clipping on and inside axes or
 * (3) clipping on and inside (xl,xr)(yb,yt) box
 */
bool
inside_box(double x, double y)
{
	extern char     _grTempString[];
	extern gr_axis_properties _grTransform_x, _grTransform_y;
	/* If logarithmic, ensure that positive value */
	if (_grTransform_x == gr_axis_LOG && x <= 0.0)
		return false;
	if (_grTransform_y == gr_axis_LOG && y <= 0.0)
		return false;
	if (_clipData == 0) {	/* user did `set clip off' */
		return true;
	} else if (_clipData == 1) {/* user did `set clip on xl xr yb yt' */
		if (!BETWEEN(_clipxleft, _clipxright, x)) {
			if (_debugFlag & DEBUG_CLIPPED) {
				sprintf(_grTempString, "Clipping (%g, %g)\n", x, y);
				gr_textput(_grTempString);
			}
			return false;
		}
		if (!BETWEEN(_clipybottom, _clipytop, y)) {
			if (_debugFlag & 0x01) {
				sprintf(_grTempString, "Clipping (%g, %g)\n", x, y);
				gr_textput(_grTempString);
			}
			return false;
		}
		return true;
	} else {			/* user did `set clip on' */
		if (!BETWEEN(_xleft, _xright, x)) {
			if (_debugFlag & 0x01) {
				sprintf(_grTempString, "Clipping (%g, %g)\n", x, y);
				gr_textput(_grTempString);
			}
			return false;
		}
		if (!BETWEEN(_ybottom, _ytop, y)) {
			if (_debugFlag & 0x01) {
				sprintf(_grTempString, "Clipping (%g, %g)\n", x, y);
				gr_textput(_grTempString);
			}
			return false;
		}
		return true;
	}
}

bool
grid_exists()
{
	Require(_xgrid_exists,
		err("First `set x grid' or `read grid x'"));
	Require(_ygrid_exists,
		err("First `set y grid' or `read grid y'"));
	Require(_grid_exists,
		err("First `read grid data' or `convert columns to grid'"));
	return true;
}

bool
scales_defined()
{
	return ((_xscale_exists && _yscale_exists) ? true : false);
}

// Set environment for line drawing.
bool
set_environment()
{
	// Update some things possibly ruined by other routines. BUG: I don't
	// think most of this is required; Macintosh remants
	gr_fontID       old_font = gr_currentfont();
	double          fontsize = FONTSIZE_PT_DEFAULT;
	double          linewidth = LINEWIDTH_DEFAULT;
	double          symbolsize = SYMBOLSIZE_DEFAULT;
	double          tic_direction = 0.0; // out
	double          tic_size = 0.2;
	double          xmargin = XMARGIN_DEFAULT;
	double          ymargin = YMARGIN_DEFAULT;
	double          xsize = XSIZE_DEFAULT;
	double          ysize = YSIZE_DEFAULT;
	if (!get_var("..fontsize..", &fontsize))
		warning("(set_environment), ..fontsize.. undefined so using 12");
	gr_setfontsize_pt(fontsize);
	gr_setfont(old_font);	// weird, since already set
	if (!get_var("..linewidth..", &linewidth))
		warning("(set_environment), ..linewidth.. undefined so using default");
	_griState.set_linewidth_line(linewidth);

	if (!get_var("..linewidthsymbol..", &linewidth))
		warning("(set_environment), ..linewidthsymbol.. undefined so using default");
	_griState.set_linewidth_symbol(linewidth);

	if (!get_var("..symbolsize..", &symbolsize))
		warning("(set_environment) ..symbolsize.. undefined so using 5.0");
	gr_setsymbolsize_cm(symbolsize);
	if (!get_var("..tic_direction..", &tic_direction))
		warning("(set_environment) ..tic_direction.. undefined so using OUT");
	gr_setticdirection(int(floor(0.5 + tic_direction)) ? true : false);
	if (!get_var("..tic_size..", &tic_size)) {
		warning("(set_environment) ..tic_size.. undefined so using default (0.2cm)");
		gr_setticsize_cm((double) TICSIZE_DEFAULT);
	} else
		gr_setticsize_cm(tic_size);
	if (!get_var("..xmargin..", &xmargin))
		warning("(set_environment) ..xmargin.. undefined so using default");
	if (!get_var("..ymargin..", &ymargin))
		warning("(set_environment) ..ymargin.. undefined so using default");
	if (!get_var("..xsize..", &xsize))
		warning("(set_environment) ..xsize.. undefined so using default");
	if (!get_var("..ysize..", &ysize))
		warning("(set_environment) ..ysize.. undefined so using default");
	gr_setxtransform(_xtype);
	gr_setxscale(xmargin, xmargin + xsize, _xleft, _xright);
	gr_setytransform(_ytype);
	gr_setyscale(ymargin, ymargin + ysize, _ybottom, _ytop);
	gr_record_scale();
	return true;
}

bool
draw_axes_if_needed()
{
	if (_need_x_axis && _need_y_axis)
		draw_axes(_axesStyle, 0.0, (gr_axis_properties) gr_axis_LEFT, true);
	return true;
}

bool
batch()
{
	double          batch;
	get_var("..batch..", &batch);
	if (batch)
		return true;
	else
		return false;
}

unsigned int
superuser()
{
	return _griState.superuser();
}

// display file on terminal
void
more_file_to_terminal(const char *filename)
{
	char            sys_cmd[100];
#if defined(VMS)		// vax-vms machine
	sprintf(sys_cmd, "TYPE %s/PAGE", filename);
#elif defined(MSDOS)		// ibm-style msdos machine
	GriString fn(filename);
	fn.convert_slash_to_MSDOS();
	sprintf(sys_cmd, "COMMAND.COM MORE < %s", fn.getValue());
#else				// a neutral machine
	char *egetenv(const char *);
	char *pager = egetenv("PAGER");
	if (*pager != '\0')
		sprintf(sys_cmd, "%s %s", pager, filename);
	else
		sprintf(sys_cmd, "more %s", filename);
#endif
	if (((unsigned) superuser()) & FLAG_SYS) {
		ShowStr("\nSending the following command to the operating system:\n");
		ShowStr(sys_cmd);
		ShowStr("\n");
	}
	call_the_OS(sys_cmd, __FILE__, __LINE__);
}

bool
demonstrate_command_usage()
{
	ShowStr("PROPER USAGE: `");
	if (cmd_being_done() > -1) {
		ShowStr(_command[cmd_being_done()].syntax);
		ShowStr("'\n");
	} else {
		ShowStr(" unknown.\n");
	}
	return true;
}

// Regular expression search. This is limited; presently can match:

// (1) characters; (2) alternative characters given in square brackets; (3)
// multiple characters followed by '*' or by '+'; (4) the '\s' metacharacter
// (for whitespace, either SPACE, TAB or NEWLINE).

// Notes: (1) notation is standard for regular expressions, in the 'perl' style;
// (2) '^' is not supported (implied anyway); (3) '+' not supported, but easy
// if I ever need it; (4) () not supported.
bool
re_compare(const char *s, const char *pattern)
{
	bool find_target(const char *pattern, int *pindex, int plen, std::string& target, int *star, int *plus);
	int             slen = strlen(s);
	int             plen = strlen(pattern); 
	int             sindex = 0;
	int             pindex = 0;
	int             star;	// is subpattern followed by '*'?
	int             plus;	// is subpattern followed by '+'?
	bool            need_new_target = true;
	int             matches = 0;
	// Search through pattern
	std::string target("");
	while (sindex < slen) {
		bool            this_matches = false;
		if (need_new_target) {
			if (!find_target(pattern, &pindex, plen, target, &star, &plus)) {
#ifdef DEBUG_RE
				printf("ran out of pattern\n");
#endif
				return false;
			}
			matches = 0;
		}
		// Now see if source string matches
#ifdef DEBUG_RE
		printf("source[%d] = `%c'  ", sindex, s[sindex]);
		show_pattern(target.c_str(), target.size(), star, plus);
#endif
		for (unsigned int tindex = 0; tindex < target.size(); tindex++) {
			if (s[sindex] == target[tindex] || target[tindex] == '.') {
				this_matches = true;
				matches++;
				break;
			}
		}
		// See if match
		if (this_matches) {
			if (star || plus) {
				need_new_target = false;
			}
		} else {
			// Not match.  See if it was a repeat pattern ('*' or '+')
			if (star) {
				need_new_target = true;
				sindex--;	// try again on this one
			} else if (plus) {
				if (matches < 1) {
					return false;
				}
				need_new_target = true;
				sindex--;	// try again on this one
			} else {
				return false;	// Failed match
			}
		}
		sindex++;
	}				// while (sindex < slen)
	// Have matched all the way through the source string.  So we have a
	// complete match if and only if the pattern string is now exhausted.
	if (pindex == plen) {
		return true;
	}
	// Some pattern left.  See if all remaining targets are '*' types; then
	// have a match.
	while (find_target(pattern, &pindex, plen, target, &star, &plus)) {
		if (!star) {
#ifdef DEBUG_RE
			printf(" ... still some non-* target left: `%s'\n", target.c_str());
#endif
			return false;
		}
	}
	// All remaining patterns (if any) were * type, so don't need to match
	return true;
}

bool
find_target(const char *pattern, int *pindex, int plen, std::string& target, int *star, int *plus)
{
	target = "";
	/*
	 * Determine present target, leaving *pindex pointing at next part of
	 * pattern.
	 */
	*star = *plus = 0;
	if (*pindex >= plen)
		return false;
	switch (pattern[*pindex]) {
	case '\\':
		/*
		 * Check against list of known escapes
		 */
		switch (pattern[*pindex + 1]) {
		case '\\':
			target += '\\';
			(*pindex)++;	/* the backslash */
			(*pindex)++;	/* the '\\' */
			break;
		case 's':		/* whitespace as in perl */
			target += ' ';
			target += '\t';
			//target += '\r';
			target += '\n';
			(*pindex)++;	/* the backslash */
			(*pindex)++;	/* the 's' */
			break;
		default:
			printf("unknown escape pattern in search string");
			return false;
		}
		break;
	case '[':
		/*
		 * List of alternatives.
		 */
		(*pindex)++;		/* skip the '[' */
		while (pattern[*pindex] != ']' && pattern[*pindex] != '\0')
			target += pattern[(*pindex)++];
		(*pindex)++;		/* skip the ']' */
		break;
	default:
		/*
		 * A single character.
		 */
		target += pattern[(*pindex)++];
	}
	if (pattern[*pindex] == '*') {
		*star = 1;
		(*pindex)++;		/* skip the '*' */
	} else if (pattern[*pindex] == '+') {
		*plus = 1;
		(*pindex)++;		/* skip the '+' */
	}
	return true;
}

#ifdef DEBUG_RE
static void
show_pattern(const char *target, int tlen, int star, int plus)
{
	int             i;
	printf("target: ");
	for (i = 0; i < tlen; i++) {
		printf(" `%c'", target[i]);
	}
	if (star)
		printf("*");
	else if (plus)
		printf("+");
	printf("\n");
}
#endif

void
swap(double& a, double& b)
{
	double tmp = a;
	a = b;
	b = tmp;
}

// Indicate if this command will take a long time. Usage example:
// GriTimer t;
// for (i = 0; i < n; i++) {
//     if (!warned)
//         double frac = double(i) / double(n - 1);
//         warned = warn_if_slow(&t, frac, "draw something");
//     ... possibly slow code
// }
bool
warn_if_slow(GriTimer *t, double fraction_done, const char *cmd)
{
	const double calibrate = 10.0; // wait this long to calibrate speed
	double dt = t->elapsed_time();
	extern char _grTempString[];
	if (_chatty < 1)
		return true;		// user does not want this
	if (fraction_done == 0.0)
		return false;
	if (dt > calibrate) {
		dt = dt / fraction_done;
		if (dt > 3600.0) {
			sprintf(_grTempString, "\n`%s':\n  This operation will take %.1g h; it is now %s",
				cmd,
				dt / 3600.0, 
				t->now_ascii());
			ShowStr(_grTempString);
		} else if (dt > 60.0) {
			sprintf(_grTempString, "\n`%s':\n  This operation will take %.1g min; it is now %s",
				cmd,
				dt / 60.0,
				t->now_ascii());
			ShowStr(_grTempString);
		} else if (dt > 15.0) {
			sprintf(_grTempString, "\n`%s':\n  This operation will take %.0g sec; it is now %s",
				cmd,
				dt,
				t->now_ascii());
			ShowStr(_grTempString);
		}
		return true;
	}
	return false;
}

// Returns portion of filename after last '/' character, if 
// there is one, otherwise returns full filename.
const char 
*filename_sans_dir(const char *fullfilename)
{
	int i, len = strlen(fullfilename);
	for (i = len - 1; i > -1; i--) {
		if (fullfilename[i] == '/') {
			return fullfilename + i + 1;
		}
	}
	return fullfilename;
}

void
bounding_box_display(const char *msg)
{
	printf("%s\n", msg);
	printf("bbox: (%f %f) (%f %f) cm\n",
	       _bounding_box.llx(),_bounding_box.lly(),
	       _bounding_box.urx(),_bounding_box.ury());
}

// Update bounding box (stored in cm on page)
void
bounding_box_update(const rectangle& box)
{
	// Only process if supplied bbox is nonzero in size
#if 0
        printf("updating bounding box %f < x < %f     %f < y < %f\n", box.llx(), box.urx(), box.lly(), box.ury());
#endif
	if (box.llx()    != box.urx()
	    || box.lly() != box.ury()) {
		// If have existing bbox, see if this lies outside ...
		if (_bounding_box.llx()    != _bounding_box.urx()
		    || _bounding_box.lly() != _bounding_box.ury()) {
			if (box.llx() < _bounding_box.llx())
				_bounding_box.set_llx(box.llx());
			if (box.lly() < _bounding_box.lly()) 
				_bounding_box.set_lly(box.lly());
			if (_bounding_box.urx() < box.urx())
				_bounding_box.set_urx(box.urx());
			if (_bounding_box.ury() < box.ury()) 
				_bounding_box.set_ury(box.ury());
		} else {		// ... else just copy it
			_bounding_box.set_llx(box.llx());
			_bounding_box.set_lly(box.lly());
			_bounding_box.set_urx(box.urx());
			_bounding_box.set_ury(box.ury());
		}
	}
}

double
vector_min(double *v, unsigned n)
{
	double return_value = v[0];
	for (unsigned i = 1; i < n; i++)
		if (v[i] < return_value)
			return_value = v[i];
	return return_value;
}

double
vector_max(double *v, unsigned n)
{
	double return_value = v[0];
	for (unsigned i = 1; i < n; i++)
		if (return_value < v[i])
			return_value = v[i];
	return return_value;
}

void
set_ps_color(char what)		// what='p' for path or 't' for text
{
	extern output_file_type _output_file_type;
	if (_output_file_type == postscript) {
		extern FILE *_grPS;
		extern bool _grWritePS;
		if (!_grWritePS)
			return;
		double r, g, b;
		if (what == 'p')
			_griState.color_line().getRGB(&r, &g, &b);
		else
			_griState.color_text().getRGB(&r, &g, &b);
		if (r == g && g == b) {
			fprintf(_grPS, "%.3g g\n", r);
			fprintf(_grPS, "%.3g G\n", r);
		} else {
			fprintf(_grPS, "%.3g %.3g %.3g rg\n", r, g, b);
			fprintf(_grPS, "%.3g %.3g %.3g RG\n", r, g, b);
		}
	}
}

void
gri_abort()
{
	close_data_files();
	abort();
}

void
gri_exit(int code)
{
	close_data_files();
	exit(code);
}

// Determine a 1-2-5 scaling for interval xl<x<xr, trying
// to get n increments each of which is a multiple of 1, 2, or 5.
// The results are xlr, ylr = the new range, which includes 
// the old range as a subset, and nr = the number of increments.
void
gr_scale125(double xl, double xr, int n, double *xlr, double *xrr, int *nr)
{
	const int max_tries = 5;
	int n_orig = n;
	int tries = 0;
	do {
		double delta = fabs((xr - xl) / n);
		if (delta == 0.0) {
			*xlr = xl - 1.0;
			*xrr = xr + 1.0;
			*nr = 1;
			return;
		}
		double order_of_magnitude = pow(10.0, floor(log10(delta)));
		double delta125 = delta / order_of_magnitude;
		if (delta125 < 2.0)
			delta125 = 1.0;
		else if (delta125 < 5.0)
			delta125 = 2.0;
		else
			delta125 = 5.0;
		delta125 *= order_of_magnitude;
		if (xr > xl) {
			*xlr = delta125 * (floor(xl / delta125));
			*xrr = delta125 * (ceil(xr / delta125));
			*nr = (int) floor(0.5 + (*xrr - *xlr) / delta125);
		} else {
			*xlr = delta125 * (ceil(xl / delta125));
			*xrr = delta125 * (floor(xr / delta125));
			*nr = (int) floor(0.5 - (*xrr - *xlr) / delta125);
		}
		n = int(0.8 * n);
		if (n < 1) 
			n = 1;		// need at least 1 segment
	} while ((*nr > int(1.75 * n_orig)) && ++tries < max_tries);
}

double
rho(double S /* PSU */, double T /* in-situ degC */, double p /* dbar */)
{
	double          rho_w, Kw, Aw, Bw, p1, S12, ro, xkst;
	rho_w = 999.842594 +
		T * (6.793952e-2 +
		     T * (-9.095290e-3 +
			  T * (1.001685e-4 +
			       T * (-1.120083e-6 + T * 6.536332e-9))));
	Kw = 19652.21
		+ T * (148.4206 +
		       T * (-2.327105 +
			    T * (1.360477e-2 - T * 5.155288e-5)));
	Aw = 3.239908 +
		T * (1.43713e-3 +
		     T * (1.16092e-4 -
			  T * 5.77905e-7));
	Bw = 8.50935e-5 +
		T * (-6.12293e-6 +
		     T * 5.2787e-8);
	p1 = 0.1 * p;
	S12 = sqrt(S);
	ro = rho_w +
		S * (8.24493e-1 +
		     T * (-4.0899e-3 +
			  T * (7.6438e-5 +
			       T * (-8.2467e-7 + T * 5.3875e-9))) +
		     S12 * (-5.72466e-3 +
			    T * (1.0227e-4 -
				 T * 1.6546e-6) +
			    S12 * 4.8314e-4));
	xkst = Kw +
		S * (54.6746 +
		     T * (-0.603459 +
			  T * (1.09987e-2 -
			       T * 6.1670e-5)) +
		     S12 * (7.944e-2 +
			    T * (1.6483e-2 +
				 T * (-5.3009e-4)))) +
		p1 * (Aw +
		      S * (2.2838e-3 +
			   T * (-1.0981e-5 +
				T * (-1.6078e-6)) +
			   S12 * (1.91075e-4)) +
		      p1 * (Bw +
			    S * (-9.9348e-7 +
				 T * (2.0816e-8 +
				      T * (9.1697e-10)))));
	return (ro / (1.0 - p1 / xkst));
}

double
pot_temp(double S, double t, double p, double pref)
{
	double dp, sq2;
	double dt1, t1, q1;
	double dt2, t2, q2;
	double dt3, t3, q3;
	double dt4, t4;

	dp = pref - p;
	sq2 = sqrt(2.);
    
	dt1 = dp * lapse_rate(S, t, p);
	q1 = dt1;
	t1 = t + 0.5 * dt1;

	dt2 = dp * lapse_rate(S, t1, p + 0.5 * dp);
	q2 = (2. - sq2) * dt2 + (-2. + 3. / sq2) * q1;
	t2 = t1 + (1. - 1. / sq2) * (dt2 - q1);

	dt3 = dp * lapse_rate(S, t2, p + 0.5*dp);
	q3 = (2 + sq2) * dt3 + (-2. - 3. / sq2) * q2;
	t3 = t2 + (1. + 1. / sq2) * (dt3 - q2);

	dt4 = dp * lapse_rate(S, t3, p + dp);
	t4 = t3 + 1. / 6. * (dt4 - 2. * q3);

	return t4;
}

/* From Unesco technical papers in marine science, number 44 (1983).
 * 
 * SYNTAX        double lapse_rate(double S, double t, double p)
 * 
 * UNITS         S in psu; t in degC; p in dbar;
 * 
 * RETURN VALUE  adiabatic lapse rate in degC/dbar
 *
 * Note: used to compute potential temperature.
 */
double
lapse_rate(double S, double t, double p)
{
	const double a0 = 3.5803e-5;
	const double a1 = 8.5258e-6;
	const double a2 = -6.8360e-8;
	const double a3 = 6.6228e-10;
	const double b0 = 1.8932e-6;
	const double b1 = -4.2393e-8;
	const double c0 = 1.8741e-8;
	const double c1 = -6.7795e-10;
	const double c2 = 8.7330e-12;
	const double c3 = -5.4481e-14;
	const double d0 = -1.1351e-10;
	const double d1 = 2.7759e-12;
	const double e0 = -4.6206e-13;
	const double e1 = 1.8676e-14;
	const double e2 = -2.1687e-16;
    
	double Gamma;

	Gamma = a0 + t * (a1 + t * (a2 + t * a3))
		+ (S - 35.) * (b0 + t * b1)
		+ p * (c0 + t * (c1 + t * (c2 + t * c3))
		       + (S - 35.) * (d0 + t * d1)
		       + p * (e0 + t * (e1 + t * e2)));
    
	return Gamma;
}

// Get name for temporary file (hide details of libraries here)
char*
tmp_file_name()
{
#if defined(HAVE_MKSTEMP)
	static char rval[PATH_MAX];
	int fd;
	/*
	 * Create the file safely and let caller scribble on it. Not
	 * perfect but better than the alternative. (One could also
	 * change the function to return a fd or FILE * instead of a
	 * path, but that's quite invasive.)
	 */
	strcpy(rval, _PATH_TMP "griXXXXXX");
	fd = mkstemp(rval);
	if (fd < 0) {
		return NULL;
	}
	close(fd);
	return rval;
#else
#if defined(HAVE_TEMPNAM)
	//	rval = tempnam("/usr/tmp", "gri");
	char *rval = tempnam(NULL, "gri");
	if (rval == NULL)
		return NULL;
	return rval;
#else
#if defined(HAVE_TMPNAM)
	char *rval = tmpnam(NULL);
	if (rval == NULL)
		return NULL;
	return rval;
#else
	return GRI_TMP_FILE;
#endif
#endif
#endif
}

int
call_the_OS(const char* cmd, const char* calling_filename, int calling_line) 
{
	std::string c(cmd);
	clean_blanks_quotes(c);
	c.append("\n");
	if (((unsigned) superuser()) & FLAG_SYS) {
		printf("Sending the following command to the operating system [ref: %s:%d]:\n%s\n", 
		       calling_filename, calling_line, c.c_str());
	}
	int status = system(c.c_str());
	PUT_VAR("..exit_status..", double(status));
	return status;
}

void
clean_blanks_quotes(std::string& c)
{
	// Trim any blanks at the start and end ...
	while (isspace(c[0]))
		c.STRINGERASE(0,1);
	while (c.size() > 0 && isspace(c[-1 + c.size()]))
		c.STRINGERASE(-1 + c.size(), 1);

	// ... and, if the first nonblank symbol is a quote,
	// then remove both it and a matching trailing quote.
	if (c[0] == '"') {
		c.STRINGERASE(0,1);
		if (c.size() > 0 && c[-1 + c.size()] == '"')
			c.STRINGERASE(-1 + c.size(), 1);
	}
}

bool
is_even_integer(double v)
{
	int iv = int(v);
	if (double(iv) != v)
		return false;	// not even an integer
	int iiv = 2 * (iv / 2);
	if (iv == iiv)
		return true;
	return false;
}

bool
is_odd_integer(double v)
{
	int iv = int(v);
	if (double(iv) != v)
		return false;	// not even an integer
	int iiv = 2 * (iv / 2);
	if (iv != iiv)
		return true;
	return false;
}

void
de_reference(std::string& syn)
{
	//printf("%s:%d 1. de_reference (%s)...\n",__FILE__,__LINE__,syn.c_str());
	if (syn[0] == '\\' && syn[1] == '@') {
		std::string deref("\\");
		deref.append(syn.substr(2, syn.size()));
		//printf("2. deref= <%s>\n", deref.c_str());
		std::string buf;
		if (get_syn(deref.c_str(), buf)) {
			syn.assign(buf);
			//printf("3. syn= <%s>\n", syn.c_str());
			if (syn[0] == '\\' && syn[1] == '\\')
				syn.STRINGERASE(0, 1);
			//printf("4. syn= <%s>\n", syn.c_str());
		}
	}
	//printf("%s:%d de_reference returning MODIFIED TO <%s>\n",__FILE__,__LINE__,syn.c_str());
}

void
un_double_slash(std::string& word)	// change leading double-backslash to single-backslash
{
	if (word[0] == '\\' && word[1] == '\\')
		word.STRINGERASE(0, 1);
}

void

un_double_quote(std::string& word)
{
	if (word[0] == '"') 
		if (word[word.size() - 1] == '"') {
			word.STRINGERASE(word.size() - 1, 1);
			word.STRINGERASE(0, 1);
		}
}

void
fix_negative_zero(std::string& number) // change e.g. "-0" to "0", for axes
{
        //#define DEBUG_FIX_NEGATIVE_ZERO
#ifdef DEBUG_FIX_NEGATIVE_ZERO
        number = "-" + number;
        printf("called fix_negative_zero(%s) [%s]\n", number.c_str(), number.c_str());
#endif
	unsigned int i;
        unsigned size = number.size();
        // find first non-blank character
        unsigned start = 0;
	for (i = 0; i < size; i++) {
		if (number[i] != ' ') {
                        start = i;
			break;
                }
        }
	if (i == size || number[i] != '-')
		return;
#ifdef DEBUG_FIX_NEGATIVE_ZERO
        printf("    first character is a minus.  start=%d\n    ", start);
#endif
        // find last digit
        unsigned end = size - 1;
        for (i = end; i > start; i--) {
#ifdef DEBUG_FIX_NEGATIVE_ZERO
                printf("c[%2d]='%c' ", i, number[i]);
#endif
                if (isdigit(number[i]) || number[i] == '.') {
                        end = i;
#ifdef DEBUG_FIX_NEGATIVE_ZERO
                        printf("\n");
#endif
                        break;
                }
        }
#ifdef DEBUG_FIX_NEGATIVE_ZERO
                        printf("    end=%d\n", end);
#endif
        std::string portion = number.substr(start + 1, end - start); // just digits or decimals
#ifdef DEBUG_FIX_NEGATIVE_ZERO
        printf("    portion='%s'\n", portion.c_str());
#endif
        // The 'portion' is now a middle portion consisting of digits and decimals
	bool is_zero = true;
        for (i = 0; i < portion.size(); i++) {
                if (!(portion[i] == '0' || portion[i] == '.')) {
                        is_zero = false;
                        break;
                }
        }
        printf("    is_zero=%d\n", is_zero);
	if (is_zero) {
#ifdef DEBUG_FIX_NEGATIVE_ZERO
		printf("    ERASING at start=%d\n", start);
#endif
                number[start] = ' ';
        }
#ifdef DEBUG_FIX_NEGATIVE_ZERO
        printf("    returning '%s'\n", number.c_str());
#endif
#undef DEBUG_FIX_NEGATIVE_ZERO
}

bool
get_optionsCmd()
{
	DEBUG_FUNCTION_ENTRY;
	DEBUG_MESSAGE("This command does NOTHING yet.\n");
	DEBUG_MESSAGE("This command is NOT documented yet.\n");
	if (_nword < 3 || _nword > 5) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	bool keep = false;	// keep the unused options?
	if (_nword == 4) {
		if (word_is(3, "keep")) {
			keep = true;
		} else {
			demonstrate_command_usage();
			err("Cannot understand word `\\", _word[3], "'.  Expecting `keep' here, if anything", "\\");
			DEBUG_FUNCTION_EXIT;
			return false;
		}
	}
	if (keep) 
		DEBUG_MESSAGE("Will keep unused options.\n") 
	else 
		DEBUG_MESSAGE("Will NOT keep unused options.\n") 

	std::string options(_word[2]);
	// Remove containing quotes, if present
	if (options[0] == '"')
		options.STRINGERASE(0, 1);
	if (options[options.size() - 1] == '"')
		options.STRINGERASE(options.size() - 1, 1);
	
	DEBUG_MESSAGE("Option-specification string was"); printf(" \"%s\"\n", options.c_str());
	DEBUG_FUNCTION_EXIT;

	return true;
}


// Byte swapping, from /usr/include/bits/byteswap.h on a linux box
#define gri_bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
unsigned int 
endian_swap_uint(unsigned int v)
{
	return gri_bswap_constant_32(v);
}
#undef gri_bswap_constant_32


bool
gri_version_exceeds(unsigned int n1, unsigned int n2, unsigned int n3)
{
	double v = n1 + n2 / 100.0 + n3 / 100000.0;
	return _version > v;
}

bool
xy_to_pt(double xin, double yin, units u, double *xout, double *yout)
{
	if (u == units_user) {
		gr_usertopt(xin, yin, xout, yout);
	} else if (u == units_cm) {
		*xout = xin * PT_PER_CM;
		*yout = yin * PT_PER_CM;
	} else if (u == units_pt) {
		*xout = xin;
		*yout = yin;
	} else {		// impossible????
		*xout = xin;
		*yout = yin;
	}
	return true;
}

bool
xy_to_cm(double xin, double yin, units u, double *xout, double *yout)
{
	if (u == units_user) {
		gr_usertocm(xin, yin, xout, yout);
	} else if (u == units_pt) {
		*xout = xin / PT_PER_CM;
		*yout = yin / PT_PER_CM;
	} else if (u == units_cm) {
		*xout = xin;
		*yout = yin;
	} else {		// impossible??
		*xout = xin;
		*yout = yin;
	}
	return true;
}

void
fix_line_ending(char *line)
{
	unsigned int len = strlen(line);
	if (len > 2 && line[len - 2] == '\r') {
		line[len - 2] = '\n';
		line[len - 1] = '\0';
	}
}

bool
is_column_name(const char* n)
{
	//printf("is_column_name(%s)\n", n);
	if (strEQ(n, "u")
	    || strEQ(n, "v")
	    || strEQ(n, "weight")
	    || strEQ(n, "x") 
	    || strEQ(n, "y")
	    || strEQ(n, "z")
	    || strEQ(n, "z")
		)
		return true;
	else
		return false;
}

#define ASSIGN_TO_COLUMN(i,v,c) \
{ \
	if ((i) >= int((c).size())) { \
		for (int ii = int((c).size()); ii <= (i); ii++) {\
			(c).push_back(0.0); \
                } \
	        PUT_VAR("..num_col_data..", double(i));\
        } \
	(c)[(i)] = (v); \
	_columns_exist = true;\
}

bool
assign_to_column(int index, double value, const char* c)
{
	if (index < 0)
		return false;
	//printf("assigning %f to  %s[%d]\n", value, c, index);
	if (strEQ(c, "x"))
		ASSIGN_TO_COLUMN(index, value, _colX);
	if (strEQ(c, "y"))
		ASSIGN_TO_COLUMN(index, value, _colY);
	if (strEQ(c, "z"))
		ASSIGN_TO_COLUMN(index, value, _colZ);
	if (strEQ(c, "u"))
		ASSIGN_TO_COLUMN(index, value, _colU);
	if (strEQ(c, "v"))
		ASSIGN_TO_COLUMN(index, value, _colV);
	return true;
	if (strEQ(c, "weight"))
		ASSIGN_TO_COLUMN(index, value, _colWEIGHT);
	return true;
}

double
gr_page_height_pt()
{
	extern rectangle _page_size;
	return (_page_size.ury() * PT_PER_CM);
}
