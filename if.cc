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
	initialize_if_necessary();
	if (!_nword)
		return false;
	double           flag;
	if (!strcmp(_word[0], "if")) {
		if (_nword < 2) {
			err("if what?");
			return true;
		}
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
		return true;
	}
	/*
	 * Handle `end if'
	 */
	if (_nword == 2 && !strcmp(_word[0], "end") && !strcmp(_word[1], "if")) {
		pop_if();
		return true;
	}
	/*
	 * Handle `else' and `else if ...'
	 */
	if (!strcmp(_word[0], "else")) {
		if (!inside_if_statement[top]) {
			err("Ignoring `else' preceeding `if'");
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
