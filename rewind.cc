#include <stdio.h>
#include "extern.hh"
#include "DataFile.hh"

bool
rewindCmd()
{
	if (_nword != 1) {
		err("`rewind' has no parameters");
		return false;
	}
	if (_dataFILE.back().get_type() != DataFile::ascii) {
		err("`rewind' ignored: no data file open (and can't rewind keyboard)");
		return false;
	}
	rewind(_dataFILE.back().get_fp());
	clearerr(_dataFILE.back().get_fp());
	_dataFILE.back().set_line(1);
	clear_eof_flag_on_data_file();
	return true;
}
