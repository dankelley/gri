/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include	<string>
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#define BEGIN_MATH "{"
#define END_MATH   "}"
extern int      rpn(unsigned int nw, char **w, char ** result); // in rpncalc.cc

bool
rpnfunctionCmd()
{
	if (_nword < 3) {
		demonstrate_command_usage();
		err("Need some action for this rpn function");
		return false;
	}
	rpn_create_function(_word[1], &_word[2], _nword - 2);
	return true;
}

/* copy below from rpncalc.c */
/* Return codes: */
#define NO_ERROR 0		/* everything is OK */
#define BAD_WORD 1		/* not operator, operand, column, function */
#define STACK_UNDERFLOW 2	/* missing operators */
#define STACK_OVERFLOW 3	/* extra stuff */
#define DIV_BY_ZERO 4		/* cannot divide by zero */
#define OUT_OF_STORAGE 5	/* computer limitation */
#define ILLEGAL_TYPE 6		/* cannot do with given operand(s) */
#define NEED_GE_0 7		/* need operand >= 0 */
#define NEED_GT_0 8		/* need operand > 0 */
#define RANGE_1 9		/* need operand in range -1 to 1 */
#define NEED_GT_1 10		/* need operand > 1  */
#define COMPUTER_LIMITATION 11	/* can't do on this machine */
#define GENERAL_ERROR  12	/* some other error */

/*
 * Substitute an rpn expression.  Return TRUE if we found an expression and it
 * parsed properly.  This is called repeatedly by other functions, until it
 *  returns FALSE, at which time all the rpn expressions will have been 
 * converted.
 * 
 * NOTE: nesting is not permitted.
 */
bool
substitute_rpn_expressions(const char *s, char *sout)
{
	void erase_rpn_stack();
	erase_rpn_stack();
	if (((unsigned) superuser()) & FLAG_RPN) printf("DEBUG %s:%d  substitute_rpn_expressions(%s,...)\n", __FILE__,__LINE__,s);
	// To speed action, maintain a buffer in which 's' will be copied,
	// prior to chopping into words.  BUG: this buffer is only cleaned
	// up at exit() time, since I never free() it.
	unsigned int space_needed = 1 + strlen(s);
	static char* copy = NULL;
	static unsigned int copy_len = 0;
	if (copy_len == 0) {
		copy_len = space_needed;
		copy = (char*)malloc(copy_len * sizeof(char));
		//printf(" RPN started with space of %d\n", copy_len);
		if (!copy) {
			gr_Error("Out of memory in `rpn'");
		}
		//printf("RPN initially got space for %d new bytes of memory, to copy `%s'\n", space_needed, s);
	} else {
		if (copy_len < space_needed) {
			copy_len = space_needed;
			copy = (char*)realloc(copy, copy_len * sizeof(char));
			//printf(" RPN increased space to %d\n", copy_len);
			if (!copy) {
				gr_Error("Out of memory in `rpn'");
			}
			//printf("RPN got space for %d new bytes of memory, to copy `%s'\n", space_needed, s);
		}
	}
	strcpy(copy, s);
	//printf("substitute_rpn_expressions(%s,...)\n",s);
	bool            found = false;
	int             rpn_start, rpn_end;
	// Copy s to a buffer, to avoid destroying  it when chopping into words
	unsigned int nword;
	chop_into_words(copy, _Words2, &nword, MAX_nword);
	if (nword < 2) {
		strcpy(sout, _Words2[0]);
		return false;
	}
	strcpy(sout, "");		// initialize
	/* Search for `BEGIN_MATH rpn ... END_MATH' */
	for (unsigned int i = 0; i < nword; i++) {
		char           *result;
		unsigned int   ii;
		rpn_start = -1;
		rpn_end = -1;
		// If at beginning of expression, parse it and put result into next
		// word of sout.
		if (!strcmp(_Words2[i], BEGIN_MATH)) {
			int             error;
			rpn_start = i;
			/* Check for { "string" == "string" } or {\syn == "string"} etc */
			if ((i + 4) <= nword && !strcmp(_Words2[i + 2], "==")) {
				char           *p1, *p2;
				int             len;
				p1 = _Words2[i + 1];
				p2 = _Words2[i + 3];
				/*
				 * Give warning msg if detect statement of the form `if {\syn
				 * == "string"}'
				 */
				if (*p1 == '\\' || *p2 == '\\') {
					err("Synonyms must be contained in quotes to compare them to strings.");
					return false;
				}
				if (*p1 == '"')
					p1++;
				if (*(p1 + (len = strlen(p1)) - 1) == '"')
					*(p1 + len - 1) = '\0';
				if (*p2 == '"')
					p2++;
				if (*(p2 + (len = strlen(p2)) - 1) == '"')
					*(p2 + len - 1) = '\0';
				if (!strcmp(p1, p2))
					strcat(sout, "1");
				else
					strcat(sout, "0");
				for (i += 5; i < nword; i++) {	/* bug: change when
								 * generalized */
					strcat(sout, _Words2[i]);
					strcat(sout, " ");
				}
				return true;
			} else if ((i + 3 > nword) || strcmp(_Words2[i + 1], "rpn")) {
				/* It's not an rpn expression. */
				strcpy(sout, s);
				return false;
			}
			/* Find the matching END_MATH bug: can't nest () in math */
			for (ii = rpn_start + 2; ii < nword; ii++) {
				if (!strcmp(_Words2[ii], END_MATH)) {
					rpn_end = ii;
					break;
				}
			}
			/* Assure that END_MATH existed. */
			if (rpn_end < 0) {
				err("Missing close-brace on RPN expression.  Proper: { rpn ... }");
				_error_in_cmd = true;
				return false;
			}
			//printf("about to call rpn(%d,...)\n",rpn_end-rpn_start-2);
			error = rpn(rpn_end - rpn_start - 2, &_Words2[rpn_start + 2], &result);
			if (((unsigned) superuser()) & FLAG_RPN) printf("%s:%d error= %d\n",__FILE__,__LINE__,error);
			switch (error) {
			case BAD_WORD:
				err("unknown item in rpn expression");
				_error_in_cmd = true;
				return false;
			case STACK_UNDERFLOW:
				err("\
final rpn stack has no operands.  Did you give an extra operator?");
				_error_in_cmd = true;
				return false;
			case STACK_OVERFLOW:
				err("\
final rpn stack has more than 1 operand.  Did you forget an operator?");
				print_rpn_stack();
				_error_in_cmd = true;
				return false;
			case DIV_BY_ZERO:
				err("rpn cannot divide by 0.");
				_error_in_cmd = true;
				return false;
			case OUT_OF_STORAGE:
				err("rpn calculation ran out of storage.  Too many words in rpn expression.");
				_error_in_cmd = true;
				return false;
			case ILLEGAL_TYPE:
				err("rpn inappropriate operator for operand[s] on the stack");
				_error_in_cmd = true;
				return false;
			case NEED_GE_0:
				err("rpn operator requires operand >= 0");
				_error_in_cmd = true;
				return false;
			case NEED_GT_0:
				err("rpn operator requires operand > 0");
				_error_in_cmd = true;
				return false;
			case RANGE_1:
				err("rpn operator requires operand in range -1 to 1 inclusive");
				_error_in_cmd = true;
				return false;
			case NEED_GT_1:
				err("rpn operator requires operand >= 1");
				_error_in_cmd = true;
				return false;
			case COMPUTER_LIMITATION:
				err("rpn: acosh(),asinh(),atanh() not available on this machine");
				_error_in_cmd = true;
				return false;
			case GENERAL_ERROR:
				err("rpn: general error");
				_error_in_cmd = true;
				return false;
			}
			// Everything is OK. Compute value, change it to, a character
			// string, and put it as next word of sout. Then increment it to
			// point to next word after the `END_MATH'.
			strcat(sout, result);
			if (result != (char*)NULL)
				delete [] result;	// was allocated in rpn function
			strcat(sout, " ");
			i = rpn_end;
			found = true;
		} else {
			strcat(sout, _Words2[i]);
			strcat(sout, " ");
		}
	}
	if (((unsigned) superuser()) & FLAG_RPN) printf("substitute_expressions returning %s\n", found ? "TRUE" : "FALSE");
	return found;
}
