#include "gr.hh"

// DESCRIPTION:  Get words (stored in strings w[0], w[1], ...)
// from string `s', finding at most 'max' words.  Then
// set 'nw' to number words read.
// 
// IMPORTANT: input string `s' is destroyed in the process!
//
// RETURN VALUE:
//  0 if everything is OK
//  1 if a double-quoted item lacked a final double-quote
//  2 if string has too many words for the w[] buffer.
//
// 1995-Feb-11: not using 'isspace' speeds by factor of 1.9, which
//     yields a 9% speedup in 'read grid data' with 55,000 elements.
// 1999-Dec-15: permit TAB type separator
// 2001-Feb-02: change to return error values
int
chop_into_words(char *s, char **w, unsigned int *nw, unsigned int max)
{
	register char   c, *cp;
	max--;
	cp = s;
	*nw = 0;
	if (*cp == '\0')
		return 0;	// ok
	// Traverse s, getting pointers to words and terminating ends
	while (*nw < max) {
		// Skip space and tabs; break if done.
		while (*cp == ' ' || *cp == '\t' || *cp == '\n' || *cp == '\r')
			cp++;
		if (!*cp)
			break;
		// Now point to non-blank.  Different actions depending on whether
		// it is a double-quoted string
		if (*cp == '"') {
			char last = *cp;
			w[*nw] = cp;
			// Collect until matching double-quote or end-of-string
			while (*++cp && !(*cp == '"' && last != '\\'))
				last = *cp;
			if (!*cp) {
				(*nw)++;
				return 1;
			}
			cp++;
			c = *cp;
			*cp++ = '\0';
			(*nw)++;		// increment number-of-words
		} else {
				// It's a word not beginning with `"'
			w[(*nw)++] = cp;
			while (*++cp 
			       && !(*cp == ' ' 
				    || *cp == '\t' 
				    || *cp == '\n'
				    || *cp == '\r')) {
				;		// EMPTY
			}
			c = *cp;
			*cp++ = '\0';
		}
		// Break if done
		if (!c)
			break;
	}
#if 0
	printf("chop_into_words set nw %d\n",*nw);
	for (int ii = 0; ii < *nw; ii++)
		printf("\tw[%d] is <%s>\n",ii,w[ii]);
#endif
	if (*nw == max)
		return 2;	// too many words for buffer
	return 0;		// ok
}

// As above, but obey the separator from 'set input data separator'
bool
chop_into_data_words(char *s, char **w, unsigned int *nw, unsigned int max)
{
	extern char _input_data_separator;
	if (_input_data_separator == ' ') {
		max--;
		char c;
		register char *cp = s;
		*nw = 0;
		if (*cp == '\0')
			return true;
		// Traverse s, getting pointers to words and terminating ends
		while (*nw < max) {
			// Skip space and tabs; break if done.
			while (*cp == ' ' || *cp == '\t' || *cp == '\n' || *cp == '\r')
				cp++;
			if (!*cp)
				break;
			// Now point to non-blank.  Different actions depending on whether
			// it is a double-quoted string
			if (*cp == '"') {
				// It's a word beginning with `"'. Set word to point to first
				// character after the `"', and break the word at the last
				// character before the final `"'.  Intermediate quotes may be
				// protected with a backslash; these are left in the word as \",
				// which must be removed later if needed.
				register char last = *cp;
				w[*nw] = cp;
				while (*++cp && !(*cp == '"' && last != '\\'))
					last = *cp;
				cp++;		// save the quote
				c = *cp;
				*cp++ = '\0';
				(*nw)++;		// increment number-of-words
			} else {
				// It's a word not beginning with `"'
				w[(*nw)++] = cp;
				while (*++cp 
				       && !(*cp == ' ' 
					    || *cp == '\t' 
					    || *cp == '\n'
					    || *cp == '\r')) {
					;		// EMPTY
				}
				c = *cp;
				*cp++ = '\0';
			}
			// Break if done
			if (!c)
				break;
		}
		return true;
	} else if (_input_data_separator == '\t') {
		max--;		// save <= below (is this faster or slower?)
		register char *cp = s;
		*nw = 0;
		if (*cp == '\0')
			return true;
		while (*nw < max) {
			w[(*nw)++] = cp;
			if (*cp == '\t') {
				*cp++ = '\0';
				continue;
			}
			while (*++cp != '\0' && *cp != '\t') {
				;		// EMPTY
			}
			char c = *cp;
			*cp++ = '\0';
			if (!c)
				break;
		}
		return true;
	} else {
		*nw = 0;
		return false;	// huh??
	}

}

