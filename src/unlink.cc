#include <string>
#include "extern.hh"

// 'unlink \filename'
bool
unlinkCmd()
{
	if (_nword != 2) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	std::string filename(_word[1]);
	un_double_quote(filename);
	return delete_file(filename.c_str());
}
