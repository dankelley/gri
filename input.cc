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
    char *            filename;
    char *            longfilename;
    extern FILE    *_grPS;
    FILE           *fp;
    double          xcm = 0.0, ycm = 0.0, xmag = 1.0, ymag = 1.0, angle = 0.0;
    GET_STORAGE(longfilename, char, LineLength);
    filename = _word[1];	/* copy pointer */
    if (*_word[1] == '"')
	filename++;
    if (*(filename + strlen(filename) - 1) == '"')
	*(filename + strlen(filename) - 1) = '\0';
    filename = complete_filename(filename);
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
    if (filename[0] != '/' && filename[0] != '.')
	longfilename = file_in_list(filename, false, false);
    else
	strcpy(longfilename, filename);
    if (NULL == (fp = fopen(longfilename, "r"))) {
	fatal_err("Cannot open `input' file named `\\",
		  longfilename,
		  "'",
		  "\\");
	free(filename);
	return false;
    }
    /*
     * Scan through the file, doing lines.
     */
    fprintf(_grPS, "q %% Beginning of `input' file named `%s'\n",
	    longfilename);
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
    fprintf(_grPS, "Q %% End of `input' file named `%s'\n",
	    longfilename);
    free(filename);
    fclose(fp);
    return true;
}
