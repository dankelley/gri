// #define DEBUG_WHILE
#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"private.hh"
#include	"extern.hh"
#include	"gr.hh"
bool            whileCmd(void);
static bool     test_is_true(const char *s);


bool
whileCmd(void)
{
	int             loop_level = 1;
	int             lines = 0;
	string test(6 + strstr(_cmdLine, "while"));
	if (re_compare(test.c_str(), " *")) {
		err("`while .test.|{rpn ...}' missing the test part");
		return false;
	}
	//printf("=============== in while cmd.  '%s'       test '%s'\n",_cmdLine,test.c_str());
	test_is_true(test.c_str()); // to catch syntax errors on this line
	// Store lines until end while into the buffer
	string buffer;
	while (1) {
		if (!get_command_line()) {
			err("Missing `end while'");
			return false;
		}
		lines++;
		// Search for matching end while
		if (re_compare(_cmdLine, "\\s*while.*")) {
			loop_level++;
		} else {
			// Search for `end while', but first make a copy
			// without the source_indicator
			char *copy = new char [1 + strlen(_cmdLine)];
			if (!copy) OUT_OF_MEMORY;
			strcpy(copy, _cmdLine);
			int len = strlen(copy);
			for (int i = 0; i < len; i++) {
				if (copy[i] == PASTE_CHAR) {
					copy[i] = '\0';
					break;
				}
			}
			if (re_compare(copy, "\\s*end\\s+while\\s*")) {
				loop_level--;
				if (loop_level < 1) {
					break;
				}
			}
			delete [] copy;
		}
		buffer.append(_cmdLine);
		buffer.append("\n");
	}
	perform_while_block(buffer.c_str(), test.c_str(), lines);
	return true;
}

const int NOTIFY = 1000;
bool
perform_while_block(const char *buffer, const char *test, int lines)
{
	//printf("^^^^^^^^^^^^ perform_while_block(...,%s,...)\n",test);
	string          filename;
	int             fileline;
	int             passes = 0;
	while (test_is_true(test)) {
		//printf(">>> STILL IN LOOP test '%s'\n",test);
		// Check to see if test is now false
		if (block_level() > 0) {
			filename.assign(block_source_file());
			fileline = block_source_line() + 2;
#ifdef DEBUG_WHILE
			printf("Register while AT BLOCKLEVEL = %d at %s:%d lines=%d\n",
			       block_level(),
			       block_source_file(),
			       block_source_line() + 2,
			       block_line());
#endif
		} else {
			filename.assign(what_file());
			fileline = what_line() - lines + 1;
#ifdef DEBUG_WHILE
			printf("Register OUT OF BLOCK while at %s:%d\n",
			       what_file(),
			       what_line() - lines + 1);
#endif
		}
		if (!perform_block(buffer, filename.c_str(), fileline)) {
			// got break
			break;
		}
		passes++;
		if (_chatty > 0 && !(passes % NOTIFY)) {
			char msg[100];
			sprintf(msg, "`while' performed %d passes\n", passes);
			gr_textput(msg);
		}
	}
#ifdef DEBUG_WHILE
	printf("\nFINISHED WITH LOOP\n");
#endif
	return true;
}

static bool
test_is_true(const char *s)
{
	char            res[100];
	double          value;
#if 1
	string ss;
	bool substitute_synonyms_new(const char *s, string& sout, bool allow_math);
	substitute_synonyms(s, ss, true);
	substitute_rpn_expressions(ss.c_str(), res);
	//printf(" + '%s'        ->     '%s'\n", s,ss.c_str());
#else
	substitute_rpn_expressions(s, res);
#endif
	if (is_var(res)) {
		getdnum(res, &value);
	} else {
		sscanf(res, "%lf", &value);
	}
	return ((value != 0.0) ? true : false);
}
