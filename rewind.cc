#include <stdio.h>
#include "extern.hh"
#include "DataFile.hh"

bool
rewindCmd()
{
	if (_nword == 1) {
		if (_dataFILE.back().get_type() != DataFile::ascii) {
			err("`rewind' ignored: no data file open (and can't rewind keyboard)");
			return false;
		}
		rewind(_dataFILE.back().get_fp());
		clearerr(_dataFILE.back().get_fp());
		_dataFILE.back().set_line(1);
		clear_eof_flag_on_data_file();
	} else if (_nword == 2) {
		std::string fname(_word[1]);
		un_double_quote(fname);
		int i = data_file_index(fname.c_str());
		if (i >= 0) {
			rewind(_dataFILE[i].get_fp());
			clearerr(_dataFILE[i].get_fp());
			_dataFILE[i].set_line(1);
			clear_eof_flag_on_data_file();
		} else {
			err("cannot `rewind' file named `\\", _word[1], "' since it is not open.", "\\");
			return false;
		}
	} else {
		err("`rewind [\filename]' takes no additional parameters");
		return false;
	}
	return true;
}
