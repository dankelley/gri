#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"private.hh"
#include        "superus.hh"


bool
sourceCmd()
{
	if (_nword != 2) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	std::string fname(_word[1]);
	if (!resolve_filename(fname, false, 'c')) {
		err("`source' cannot handle file named `\\", _word[1], "'", "\\");
		return false;
	}
	//printf("Sourcing '%s'\n", fname.c_str());
	FILE *fp = fopen(fname.c_str(), "r");
	if (NULL == fp) {
		extern char _grTempString[];
		sprintf(_grTempString, "`source' cannot open file `%s' (translated to `%s')", _word[1], fname.c_str());
		err(_grTempString);
		return false;
	}

#if 1
	if (((unsigned) superuser()) & FLAG_AUT1) {
		CmdFile cf;			// as in file.cc
		cf.set(fname.c_str(), fp, false, 0);
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
				fname.c_str(),
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
		//printf("[%s]\n",_cmdLine);
		if (_nword > 0 && !strcmp(_word[0], "return") && !skipping_through_if())
			break;
		if (!perform_command_line(fp, false)) {
			return false;
		}
	}
	_done = 0;
	fclose(fp);
	if (((unsigned) superuser()) & FLAG_AUT1) {
		_cmdFILE.pop_back();
	}
	return true;
}

