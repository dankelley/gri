#include	"gr.hh"
#include	"extern.hh"
#include	"GMatrix.hh"

#if defined(IS_MINGW32)
#include        <windows.h>
#endif


bool
sleepCmd()
{
	if (_nword != 2) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	double sec_f;
	if (!getdnum(_word[1], &sec_f)) {
		err("`sleep' can't read .sec. in `\\", _word[1], "'.", "\\");
		return false;
	}
	if (sec_f <= 0.0)
		return true;
	int sec = int(floor(0.5 + sec_f));
#if !defined(IS_MINGW32)
	sleep(sec); 
#else
	Sleep(sec*1000);
#endif
	return true;
}
