#include        <string>
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "defaults.hh"
#include        "files.hh"
#include        "superus.hh"

bool            assertCmd(void);

bool assertCmd()
{
	if (_nword > 3) {
		demonstrate_command_usage();
		err("Too many words in `assert'");
		return false;
	}
	if (_nword < 2) {
		demonstrate_command_usage();
		err("Too few words in `assert'");
		return false;
	}
	double cond;
	if (!getdnum(_word[1], &cond)) {
		err("`assert' cannot decode the condition");
		return false;
	}
	if (cond) {
		return true;
	} else {
		std::string msg;
		if (_nword == 3) {
			msg = _word[2];
			clean_blanks_quotes(msg);
		}
		if (msg.empty()) {
			printf("Failed assertion at ?file? line ?line?.\n");
		} else {
			if (msg.size() > 2
			    && msg[msg.size() - 1] == 'n'
			    && msg[msg.size() - 2] == '\\') {
				ShowStr(msg.c_str());
			} else {
				ShowStr(msg.c_str());
				printf(" at ?file? line ?line?.\n");
			}
		}
		gri_exit(1);
		return false;	// never executed
	}
}
