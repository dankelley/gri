#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"

extern char     _grTempString[];

bool
rescaleCmd()
{
	if (_nword == 1) {
		if (_chatty > 2) {
			sprintf(_grTempString, "Rescaling x/y axes\n");
			ShowStr(_grTempString);
		}
		_need_x_axis = true;
		_need_y_axis = true;
		create_x_scale();
		create_y_scale();
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
}
