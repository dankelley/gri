#include        <stdio.h>
#include        <string.h>
#include        <math.h>
#include	"extern.hh"
#define	TOP	20000		/* max number nested if-s */
static int      top = 0;	/* nesting level minus 1 */
static bool     got_an_else[TOP];	/* keeps track of duplicate `else' */
static bool     skipping[TOP];	/* flag telling that skipping code */
static bool     skip_to_end[TOP];	/* set to 1 if part of block done */
static bool     inside_if_statement[TOP];
static bool     need_to_initialize = true;
void            initialize_if_necessary(void);

void
initialize_if_necessary()
{
	if (need_to_initialize) {
		for (int i = 0; i < TOP; i++) {
			got_an_else[i] = false;
			skipping[i] = false;
			skip_to_end[i] = false;
			inside_if_statement[i] = false;
		}
		need_to_initialize = false;
	}
}

void
push_if(int flag)
{
	initialize_if_necessary();
	if (++top >= TOP)
		fatal_err("Fatal error: too many nested if-statements (max is 20000).");
	got_an_else[top] = false;
	skipping[top] = ((!flag) || skipping[top - 1]) ? true : false;
	skip_to_end[top] = flag != 0 ? true : false;
	inside_if_statement[top] = true;
}

void
pop_if()
{
	initialize_if_necessary();
	if (--top < 0) {
		warning("Warning - ignoring extra `end if'");
		top = 0;
	}
}

bool
skipping_through_if()
{
	initialize_if_necessary();
	return skipping[top];
}

/*
 * handle_if_block - handle `else if' and `else' by setting skip-flags.
 * RETURN VALUE 1 if the commands `if', `else if' or `else' were detected.
 */
bool
handle_if_block()
{
	if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG: %s:%d handle_if_block()\n",__FILE__,__LINE__);
	initialize_if_necessary();
	if (!_nword)
		return false;
	/*
	 * Handle `end if'
	 */
	if (_nword == 2 && !strcmp(_word[0], "end") && !strcmp(_word[1], "if")) {
		pop_if();
		return true;
	}
	/*
	 * Handle `if'
	 */
	if (!strcmp(_word[0], "if")) {
		if (_nword < 2) {
			err("if what?");
			return true;
		}
		double flag;

		if (((unsigned) superuser()) & FLAG_FLOW) printf("DEBUG %s:%d DEBUG '%s' ... inside_if= %d    skipping= %d\n",__FILE__,__LINE__,_word[1], inside_if_statement[top], skipping[top]);
		if (skipping[top]) {
			push_if(false);	// does the value matter??????
		} else {
			if (!strcmp(_word[1], "!")) {
				if (!getdnum(_word[2], &flag)) {
					err("if what?");
					return true;
				}
				flag = !flag;
			} else if (!getdnum(_word[1], &flag)) {
				err("`if' what?");
				return true;
			}
			push_if(floor(0.5 + flag) ? true : false);
		}
		return true;
	}
	/*
	 * Handle `else' and `else if ...'
	 */
	if (!strcmp(_word[0], "else")) {
		if (!inside_if_statement[top]) {
			err("This `else' doesn't match an `if'");
			return true;
		}
		/* `else [...]' */
		if (_nword == 1) {
			/* `else' */
			if (got_an_else[top] && !skipping[top]) {
				err("Ignoring duplicate else");
				return true;
			}
			got_an_else[top] = true;
			skipping[top] = (skip_to_end[top] || skipping[top - 1]) ? true : false;
			return true;
		} else if (_nword > 2 && !strcmp(_word[1], "if")) {
			if (!inside_if_statement[top]) {
				err("This `else if' doesn't match an `if'");
				return true;
			}
			//printf("NOTE: In else if.  skipping[%d]=%d\n",top,skipping[top]);

			// Only consider the value if presently skipping
			if (skipping[top] == 0) {
				skipping[top] = 1;
				//printf("ELSE IF Set skipping[%d] to 1\n",top);
				return true;
			}

			// Must figure out rpn expressions (if any) since they
			// are ignored for the 'false' part of if blocks, and
			// that's what we are in at the moment.
			while (substitute_rpn_expressions(_cmdLine, _cmdLineCOPY))
				strcpy(_cmdLine, _cmdLineCOPY);
			chop_into_words(_cmdLine, _word, &_nword, MAX_nword);
			/* `else if #' */
			double           tmp;
			if (!getdnum(_word[2], &tmp)) {
				err("else if ?what?");
				return true;
			}
			if (tmp) {
				if (skip_to_end[top]) {
					err("skipping second TRUE part of IF");
					skipping[top] = true;
					return true;
				}
				skipping[top] = skipping[top - 1];
				skip_to_end[top] = true;
			} else {
				skipping[top] = true;
			}
			return true;
		} else {
			err("else ?what?");
			return true;
		}
	}
	/* cmd = something to do */
	return false;
}
