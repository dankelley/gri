#include	<string>
#include	<stdio.h>
#include	<math.h>
#include	<ctype.h>
#include	"gr.hh"
#include	"debug.hh"
#include	"extern.hh"

int
debugCmd()
{
	int             debug;
	switch (_nword) {
	case 2:
		/* debug .n.|off */
		if (!strcmp(_word[1], "off")) {
			debug = 0;
			PUT_VAR("..debug..", debug);
			_debugFlag = 0;
			return 1;
		}
		getinum(_word[1], &debug);
		break;
	case 1:
		/* `debug' -- equivalent to `debug 1' */
		debug = 1;
		_debugFlag = 1;
		break;
	case 6:
		/* `debug clipped values in draw commands' */
		if (!strcmp(_word[1], "clipped")
		    && !strcmp(_word[2], "values")
		    && !strcmp(_word[3], "in")
		    && !strcmp(_word[4], "draw")
		    && !strcmp(_word[5], "commands")) {
			_debugFlag |= DEBUG_CLIPPED;
			return 1;
		} else {
			demonstrate_command_usage();
			err("Can't understand command.");
			return 0;
		}
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return 0;
	}
	PUT_VAR("..debug..", debug);
	return 1;
}
