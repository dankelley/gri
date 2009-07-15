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

#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"private.hh"
extern char     _grTempString[];

/* `input \ps_filename [.xcm. .ycm. [.xmag. .ymag. [.rot_deg.]]]' */
bool
inputCmd()
{
	double          xcm = 0.0, ycm = 0.0, xmag = 1.0, ymag = 1.0, angle = 0.0;
	std::string fname(_word[1]);
	un_double_quote(fname);
	std::string completefilename(fname);
	resolve_filename(completefilename, true, 'c'); // BUG do I want 'c' or 'd' here??
	switch (_nword) {
	case 7:
		getdnum(_word[6], &angle);
		getdnum(_word[5], &ymag);
		getdnum(_word[4], &xmag);
		getdnum(_word[3], &ycm);
		getdnum(_word[2], &xcm);
		break;
	case 6:
		getdnum(_word[5], &ymag);
		getdnum(_word[4], &xmag);
		getdnum(_word[3], &ycm);
		getdnum(_word[2], &xcm);
		break;
	case 4:
		getdnum(_word[3], &ycm);
		getdnum(_word[2], &xcm);
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		break;
	}
	/* Search directory only if not specified as local/fullpath */
	std::string longfilename(completefilename);

//OLD	if (completefilename[0] != '/' && completefilename[0] != '.')
//OLD		longfilename.assign(file_in_list(completefilename.c_str(), false, false));

	FILE *fp;
	if (NULL == (fp = fopen(longfilename.c_str(), "r"))) {
		fatal_err("Cannot open `input' file named `\\",
			  longfilename.c_str(),
			  "'",
			  "\\");
		return false;
	}
	/*
	 * Scan through the file, doing lines.
	 */
	extern FILE *_grPS;
	fprintf(_grPS, "q %% Beginning of `input' file named `%s'\n", longfilename.c_str());
	fprintf(_grPS, "%f %f translate %f %f scale %f rotate\n",
		xcm * PT_PER_CM, ycm * PT_PER_CM, xmag, ymag, angle);
	while (!feof(fp)) {
		/*
		 * See if hit EOF on a line with no text.
		 */
		if (NULL == fgets(_grTempString, LineLength, fp))
			break;
		fprintf(_grPS, "%s", _grTempString);
	}
	fprintf(_grPS, "Q %% End of `input' file named `%s'\n", longfilename.c_str());
	fclose(fp);
	return true;
}
