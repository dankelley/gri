#include <stdio.h>
#include <string.h>
#include "extern.hh"
#include "private.hh"
#include "DataFile.hh"

bool
closeCmd()
{
	if ((_dataFILE.back()).get_type() == DataFile::from_cmdfile) {
		err("`close' ignored: no data file open");
		return false;
	}
	int file = _dataFILE.size() - 1;
	string fname;
	switch (_nword) {
	case 1:
		break;
	case 2:
		fname.assign(_word[1]);
		if (fname[0] == '"')			fname.STRINGERASE(0, 1);
		if (fname[fname.size()-1] == '"')	fname.STRINGERASE(fname.size()-1,1);
		file = data_file_index(fname.c_str());
		if (file < 0) {
			extern char     _grTempString[];
			sprintf(_grTempString, "`close' cannot close `%s' since it is not open", _word[1]);
			err(_grTempString);
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	pop_data_file(file);
	clear_eof_flag_on_data_file();
	return true;
}
