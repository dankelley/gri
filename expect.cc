#include        <string>

#if !defined(IS_MINGW32)
#include        <strings.h>
#else
#define index strrch
#endif

#include        <stdio.h>
#include        "gr.hh"
#include        "extern.hh"

#if defined(__DECCXX) || defined(OS_IS_BEOS)
extern "C" char *index(const char *s, int c);
#endif

extern char     _grTempString[];

bool
expectingCmd()
{
	double          version_expected, this_version;
	if (_nword != 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	char *dot1 = index(_word[2], int('.'));
	if (dot1 == NULL) {
		err("Version number must have a decimal point");
		return false;
	}
	char *dot2 = index(dot1 + 1, int('.'));
	bool only_one_dot = false;
	if (dot2 == NULL) {
		only_one_dot = true;
		if (!getdnum(_word[2], &version_expected)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".version.");
			return false;
		}
		version_expected = atof(_word[2]);
	} else {
		char *dot3 = index(dot2 + 1, int('.'));
		if (dot3 != NULL) {
			err("Version number must not have more than two decimal points");
			return false;
		}
		int major, minor, minorminor;
		if (3 != sscanf(_word[2], "%d.%d.%d", &major, &minor, &minorminor)) {
			err("Cannot decode version number");
			return false;
		}
		version_expected = major + float(minor) / 100.0 + float(minorminor) / 10000.0;
	}
	_version_expected = version_expected;
	this_version = _version;
	if (!only_one_dot && this_version < version_expected) {
		sprintf(_grTempString, "\
WARNING: You are expecting a gri version (%.4f) more recent than\n\
the present version (%.4f); this might cause problems.\n\n",
			double(version_expected), double(this_version));
		ShowStr(_grTempString);
	}
	if (only_one_dot && version_expected < 1.064) {
		ShowStr("\
\nFollowing is a list of things changed in Gri since the version you\n\
are expecting.  Note that this list includes *all* commands, even if you\n\
are not using them in your Gri program.\n\n");
		ShowStr("\
\nIncompatibilities introduced in version 1.064:\n\
* `{rpn x column_min}' RENAMED `{rpn x min}'; same for\n\
   the column_max and column_mean operators.\n\
* Disallow multiple statements on one line, separated by semicolons\n\n");
	}
	if (only_one_dot && version_expected < 1.037) {
		ShowStr("\
\nIncompatibilities introduced in version 1.037:\n\
* `draw image grayscale' RENAMED `draw image palette'\n\n");
	}
	if (only_one_dot && version_expected < 1.036) {
		ShowStr("\
\nIncompatibilities introduced in version 1.036:\n\
* ALL image commands now require `set image range' to have been first.\n\
  In a related change, `convert grid to image' now does not permit the\n\
  `white' and `black' options, and `read image' now does not permit the\n\
  `scale' option.  Although these changes are obnoxious, they should\n\
  make images much more reliable and easy to use.\n\n");
	}
	if (only_one_dot && version_expected < 1.034) {
		ShowStr("\
\nIncompatibilities introduced in version 1.034:\n\
* Within math - mode, normal letters appear in italics, as in TeX.\n\n");
	}
	if (only_one_dot && only_one_dot && version_expected < 1.033) {
		ShowStr("\
\nIncompatibilities introduced in version 1.033:\n\
* The commandline option ` -extract ' is renamed `-creator'.\n\
* `convert columns to grid neighborhood ' is removed since it was\n\
  pretty useless.  (It' s easy enough to do in awk.\n\
* `convert columns to grid planar ' is removed; use the `boxcar'\n\
  method instead.\n\
* The commandline option ` -quiet ' is removed; use `-chatty' instead.\n\n");
	}
	if (only_one_dot && version_expected < 1.050) {
		ShowStr("\
\nIncompatibilities introduced in version 1.050:\n\
* The `function' command has been removed, and all builtin functions\n\
  replaced with RPN functions\n\n");
	}
	return true;
}
