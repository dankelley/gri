#include	<string>
#include	"gr.hh"
#include	"extern.hh"

bool
quitCmd()
{
	double tmp;
	switch (_nword) {
	case 1:
		_done = 1;
		return true;
	case 2:
		if (!getdnum(_word[1], &tmp)) {
			fatal_err("Cannot interpret `quit' exit value `\\", _word[1], "'", "\\");
		}
		_exit_value = (int) floor(0.5 + tmp);
		_done = 1;
		return true;
	default:
		demonstrate_command_usage();
		fatal_err("Extra word in command");
		return false;
	}
}
