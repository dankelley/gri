//#define DEBUG_RE

#include	<string>
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include        <stdarg.h>

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
static void     show_pattern(char *target, int tlen, int star, int plus);
#endif
extern double   strtod(const char *, char **);

// Keep this in static area, so as not to waste time with
// multiple allocation/deallocation for scratch strings
static string tmp_string;

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
	bool do_y     = _colY.size()     == length;
	bool do_z     = _colZ.size()     == length;
	bool do_U     = _colU.size()     == length;
	bool do_V     = _colV.size()     == length;
	bool do_R     = _colR.size()     == length;
	bool do_THETA = _colTHETA.size() == length;
	unsigned int missing = 0;
	for (unsigned int i = 0; i < length; i++) {
		if (            gr_missing(_colX[i]))     { missing++; continue; }
		if (do_y     && gr_missing(_colY[i]))	  { missing++; continue; }
		if (do_z     && gr_missing(_colZ[i]))	  { missing++; continue; }
		if (do_U     && gr_missing(_colU[i]))	  { missing++; continue; }
		if (do_V     && gr_missing(_colV[i]))	  { missing++; continue; }
		if (do_R     && gr_missing(_colR[i]))	  { missing++; continue; }
		if (do_THETA && gr_missing(_colTHETA[i])) { missing++; continue; }
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

// Is the i-th word of _word[] equal to given word? (return 0 if too few
// words)
bool
word_is(int i, const char *word)
{
	return ((-1 < i && i < _nword && !strcmp(word, _word[i])) ? true : false);
}

void
check_psfile()
{
	extern FILE    *_grPS;
	if (ferror(_grPS)) {
		/* never returns */
		fatal_err(" IO error on PostScript output file\n");
	}
}

bool
delete_file(const char *filename)
{
	char            sys_cmd[200];
#if defined(VMS)
	sprintf(sys_cmd, "DEL %s;*", filename);
	call_the_OS(sys_cmd, __FILE__, __LINE__);
	return true;
#elif defined(MSDOS)
	sprintf(sys_cmd, "DEL %s", filename);
	call_the_OS(sys_cmd, __FILE__, __LINE__);
	return true;
#else
	sprintf(sys_cmd, "rm %s", filename);
	call_the_OS(sys_cmd, __FILE__, __LINE__);
	return true;
#endif
}

// Convert full filename to tilde name
char *
cshell_name(char *s)
{
	char *            new_name;
#if defined(HAVE_GETENV)
#if defined(MSDOS)		// for MSDOS, return input
	new_name = new char [1 + strlen(s)];
	if (!new_name) OUT_OF_MEMORY;
	strcpy(new_name, s);
	return new_name;
#else				// not MSDOS
	char *            user = (char *) getenv("USER");
	char *            home = (char *) getenv("HOME");
	if (user && home) {
		// The environment variables were recovered ok.
		if (!strncmp(s, home, strlen(home))) {
			// Below asks for 1 extra space; what the heck.
			new_name = new char [3 + strlen(user) + strlen(s) - strlen(home)];
			if (!new_name) OUT_OF_MEMORY;
			strcpy(new_name, "~");
			strcat(new_name, user);
			strcat(new_name, s + strlen(home));
		} else {
			new_name = new char[1 + strlen(s)];
			if (!new_name) OUT_OF_MEMORY;
			strcpy(new_name, s);
		}
	} else {
		// Could not recover environment variables
		new_name = new char[1 + strlen(s)];
		if (!new_name) OUT_OF_MEMORY;
		strcpy(new_name, s);
	}
	return new_name;
#endif				// not MSDOS
#else				// No environment variables
	new_name = new char[1 + strlen(s)];
	if (!new_name) OUT_OF_MEMORY;
	strcpy(new_name, s);
	return new_name;
#endif
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

// Store result of 'printenv _word[3]' into _word[2] (If no such environment
// variable, store "" into _word[2].)
bool
get_envCmd()
{
	if (_nword == 4) {
		char *            result;
		if (!is_syn(_word[2])) {
			err("No synonym name given");
			demonstrate_command_usage();
			return false;
		}
		result = egetenv((const char*)_word[3]);
		if (result == NULL) {
			if (!put_syn(_word[2], "", true)) {
				gr_Error("Ran out of storage");
				return false;
			}
			warning("No environment variable called `\\", _word[3], "' exits.", "\\");
		} else {
			if (!put_syn(_word[2], result, true)) {
				gr_Error("Ran out of storage");
				return false;
			}
		}
	} else {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	return true;			/* never reached */
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
resolve_filename(string& f, bool trace_path)
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
		f.STRINGERASE(0, 1);
		f.insert(0, egetenv("HOME"));
		return true;
	}

	// BUG: probably should substitute any env-vars here, e.g. $HOME

	// Done, unless we have to trace the path ...
	if (!trace_path)
		return true;

	// ... but can even skip that, if the pathname is complete already ...
	if (f[0] == '.' || f[0] == '/')
		return true;

	// ... ok, now we know we should trace!
	string path(egetenv("GRIINPUTS"));
	if (path.size() < 1)
		path.assign(GRIINPUTS); // in defaults.hh ".:/usr/local/lib/gri"
	string::size_type start = 0;
	string::size_type colon;
	do {
		colon = path.find(":", start);
		string test_file = path.substr(start, colon);
		test_file.append("/");
		test_file.append(f);
		//printf("resolve_filename trying file named '%s'\n", test_file.c_str());
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
		return "";
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

char *
egetenv(const char *s)
{
	char *rval = "";
	if (!strcmp(s, "PWD")) {
		return (char *) pwd();
	} else if (!strcmp(s, "USER")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return "";
		else
			return rval;
#else
		return "unknown";
#endif
	} else if (!strcmp(s, "SYSTEM")) {
#if defined(VMS)
		return "vax";
#elif defined(MSDOS)
		return "msdos";
#else
		return "unix";
#endif
	} else if (!strcmp(s, "HOST")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return "";
		else
			return rval;
#else
		return "unknown";
#endif
	} else if (!strcmp(s, "HOME")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return "";
		else
			return rval;
#else
		return "unknown";
#endif
	} else if (!strcmp(s, "PAGER")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return "";
		else
			return rval;
#else
		return "unknown";
#endif
	} else if (!strcmp(s, "GRIINPUTS")) {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return "";
		else
			return rval;
#else
		return "";
#endif
	} else {
#if defined(HAVE_GETENV)
		rval = (char *)getenv(s);
		if (rval == NULL)
			return "";
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
	int             i;
	char *            fmt;
	char            msg[1024];
	double          x[N];
	if (_nword < 4) {
		err("`sprintf \\synonym \"format\"' requires variable list to process");
		return false;
	}
	*(_word[2] + strlen(_word[2]) - 1) = '\0';
	fmt = _word[2] + 1;
	for (i = 3; i < _nword; i++) {
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
	if (!is_syn(_word[1])) {
		demonstrate_command_usage();
		err("Second word of command must begin with `\\'");
		return false;
	}
	if (!put_syn(_word[1], msg, true)) {
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
	char *            ptr = NULL;
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
		err("Cannot interpret `\\",
		    s,
		    "' as a number.",
		    "\\");
	}
	return false;
}

// getdnum -- return 0 if can't get number
bool
getdnum(const char *s, double *d)
{
	if (*s == '\0')
		return 0;
	extern double   _grMissingValue;
	char *            ptr = NULL;
	*d = strtod(s, &ptr);
	if (*ptr == '\0') {
		// Normal number; check for infinity/not-a-number
#if defined(HAVE_ISNAN) && defined(HAVE_ISINF)
#if !defined(__MACHTEN__)
		if (isinf(*d) || isnan(*d))
			*d = (double) _grMissingValue;
#endif
#endif
		return true;
	}
	// Cannot read as a normal number.  Check to see if it's a variable or
	// NaN/Inf.
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
	} else if (!strcmp(s, "-NaN") || !strcmp(s, "NaN") || !strcmp(s, "-Inf") || !strcmp(s, "Inf")) {
		*d = _grMissingValue;
		return true;
	} else {
		// Maybe it's a fortran 'D' exponential
		tmp_string.assign(s);
		int loc;
		loc=tmp_string.find('d'); if (-1!=loc) tmp_string.replace(loc,1,"e");
		loc=tmp_string.find('D'); if (-1!=loc) tmp_string.replace(loc,1,"e");
		*d = (double)strtod(tmp_string.c_str(), &ptr);
		if (*ptr == '\0')
			return true;
	}
	// It's a mystery.  report error, fatal or nonfatal depending on
	// whether the user is interested in errors.
	if (!_ignore_error) {
		err("Cannot interpret `\\",
		    s,
		    "' as a number.",
		    "\\");
	}
	return false;
}

/*
 * fatal_err () -- print error message.  If the first (string) argument ends
 * in '\\', then several strings follow, ended by a string consisting only of
 * "\\".
 */
void
fatal_err(const char *string,...)
{
	char            msg[1024];
	bool            several = false;
	int             len;
	char *            p;
	va_list         ap;
	if (!_error_in_cmd && _beep)
		gr_textput("\007");

	if (string != NULL) {
		va_start(ap, string);
		strcpy(msg, string);
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
		sprintf(msg, " Error detected at %s\n", source_indicator);
	} else {
		if (block_level() > 0) {
			if (block_source_file() != NULL) {
				if (superuser() & FLAG_AUT1)printf("DEBUG utility.cc:fatal_err(): block_source_line()=%d\n",block_source_line());
				sprintf(msg,
					" Error detected at %s:%d\n",
					block_source_file(),
					block_source_line() + block_offset_line() - 1);
				gr_textput(msg);
			}
		} else {
			if (what_file() != NULL) {
				sprintf(msg,
					" Error detected at %s:%d\n",
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
err(const char *string,...)
{
	char            msg[1024];
	bool            several = false;
	int             len;
	char *            p;
	va_list         ap;
	if (!_error_in_cmd && _beep)
		gr_textput("\007");
	if (string != NULL) {
		va_start(ap, string);
		strcpy(msg, string);
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
	static string   msg_last;	// must be static
	static string msg;		// static only for efficiency if called a lot
	bool            several = false;
	int             len;
	char           *p = NULL;
	va_list         ap;
	if (!_error_in_cmd && _beep)
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
 */
int
ExtractQuote(const char *s, string& sout)
{
	//printf("DEBUG %s:%d input string is '%s'\n",__FILE__,__LINE__,s);
	int             i = 0;
	// Skip along to first quote ...
	while (s[i] != '"') {
		if (s[i] == '\0')
			return 0;
		i++;
	}
	i++;			// skip the quote
	// ... then copy along until find first un-escaped quote ...
	while (s[i] != '\0') {
		if (s[i] == '"') {
			if (i > 0 && s[i - 1] == '\\') {
				sout += '"';
			} else {
				//printf("DEBUG %s:%d returning '%s' with pos %d\n",__FILE__,__LINE__,sout.c_str(), i+1);
				return i + 1;
			}
		} else {
			sout += s[i];
		}
		i++;
	}
	// ... and return an index so the parser can do more
	// work on 's' past the quote
	//printf("DEBUG %s:%d extract quote put sout '%s' from '%s'\n", __FILE__,__LINE__,sout.c_str(),s);
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

void
beep_terminal()
{
	if (_beep)
		fprintf(stderr, "\007");
}

#define BETWEEN(l,r,m)  ( (l)<(r) ? (m)<=(r)&&(l)<=(m) : (r)<=(m)&&(m)<=(l) )
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
	bool find_target(const char *pattern, int *pindex, int plen, string& target, int *star, int *plus);
	int             slen = strlen(s);
	int             plen = strlen(pattern); 
	int             sindex = 0;
	int             pindex = 0;
	int             star;	// is subpattern followed by '*'?
	int             plus;	// is subpattern followed by '+'?
	bool            need_new_target = true;
	int             matches = 0;
	// Search through pattern
	string target("");
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
find_target(const char *pattern, int *pindex, int plen, string& target, int *star, int *plus)
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
show_pattern(char *target, int tlen, int star, int plus)
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
	char *rval;
#if defined(HAVE_TEMPNAM)
	rval = tempnam("/usr/tmp", "gri");
	if (return_value == NULL) 
		return NULL;
	return rval;
#else
#if defined(HAVE_TMPNAM)
	rval = tmpnam(NULL);
	if (rval == NULL)
		return NULL;
	return rval;
#else
	return GRE_TMP_FILE;
#endif
#endif
}

int
call_the_OS(const char* cmd, const char* calling_filename, int calling_line) 
{
	string c(cmd);
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
clean_blanks_quotes(string& c)
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
