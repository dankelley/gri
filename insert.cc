#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"private.hh"
#include        "superus.hh"


bool            insertCmd(void);
extern char     _grTempString[];


bool
insertCmd()
{
    int unsigned i = 0;
    char *filename;
    char *longfilename;
    FILE *fp;
    GET_STORAGE(filename, char, LineLength);
    GET_STORAGE(longfilename, char, LineLength);
    switch (_nword) {
    case 2:
	while (*(_cmdLine + i) != ' ' && *(_cmdLine + i) != '\0')
	    i++;
	while (*(_cmdLine + i) == ' ' || *(_cmdLine + i) == '\t')
	    i++;
	filename = tilde_expand(_cmdLine + i);
	break;
    default:
	demonstrate_command_usage();
	NUMBER_WORDS_ERROR;
	break;
    }
    // Search directory only if not specified as local/fullpath
    if (filename[0] != '/' && filename[0] != '.')
	longfilename = file_in_list(filename, false, false);
    else
	strcpy(longfilename, filename);
    if (NULL == (fp = fopen(longfilename, "r"))) {
	sprintf(_grTempString, "Sorry, I can't open file `%s'", longfilename);
	err(_grTempString);
	free(filename);
	return false;
    }

#if 1
    if (((unsigned) superuser()) & FLAG_AUT1) {
	CmdFile cf;			// as in file.cc
	cf.set(longfilename, fp, false, 0);
	_cmdFILE.push_back(cf);
    }
#endif

    /*
     * Scan through the file, doing lines.
     */
    while (!feof(fp)) {
	/*
	 * See if hit EOF on a line with no text.
	 */
	if (NULL == fgets(_cmdLine, LineLength, fp))
	    break;
	if (feof(fp)) {
	    warning("Missing newline at end of inserted file `\\",
		    filename,
		    "'",
		    "\\");
	    strcat(_cmdLine, "\n");
	}
	_cmdFILE.back().increment_line(); // BUG line numbers wrong BUG
	if (((unsigned) superuser()) & FLAG_AUT1) {
	    void insert_source_indicator(char *cl); // in doline.cc
	    insert_source_indicator(_cmdLine);
	}
	massage_command_line(_cmdLine);
	if (_nword > 0 && !strcmp(_word[0], "return") && !skipping_through_if())
	    break;
	if (!perform_command_line(fp, false)) {
	    return false;
	}
    }
    _done = 0;
    free(filename);
    fclose(fp);
    if (((unsigned) superuser()) & FLAG_AUT1) {
	_cmdFILE.pop_back();
    }
    return true;
}

