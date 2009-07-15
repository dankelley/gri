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
#include	<math.h>
#include	<stdio.h>
#include        <stdlib.h>

#include	"gr.hh"
#include	"extern.hh"

/* Defined here */
bool            reorder_columnsCmd(void);
static bool     reorder_cols_randomly();
static bool     reorder_cols_order(bool ascending, int which, int n);

// Following is unsafe, but I know indices are OK
inline  void swap(GriColumn *A, int a, int b)
{
	if (A->size() > 0) {
		double *ap = A->begin();
		double tmp = *(ap + (a));
		*(ap + (a)) = *(ap + (b));
		*(ap + (b)) = tmp;
	}
}

/*
 * `reorder columns randomly|{ascending in x|y|z}|{descending in x|y|z}'
 */
bool
reorder_columnsCmd()
{
	unsigned int n;
	bool ascending;
	switch (_nword) {
	case 3:
		if (word_is(2, "randomly")) {
			return reorder_cols_randomly();
		} else {
			err("Third word must be `randomly', if only 3 words");
			demonstrate_command_usage();
			return false;
		}
	case 5:
		if (word_is(2, "ascending")) {
			ascending = true;
		} else if (word_is(2, "descending")) {
			ascending = false;
		} else {
			err("Third word must be `ascending' or `descending'");
			demonstrate_command_usage();
			return false;
		}
		if (!word_is(3, "in")) {
			err("Fourth word must be `in'");
			demonstrate_command_usage();
			return false;
		}
		n = _colX.size();		/* presume no x means no cols */
		if (word_is(4, "x")) {
			reorder_cols_order(ascending, 0, n);
		} else if (word_is(4, "y")) {
			reorder_cols_order(ascending, 1, n);
		} else if (word_is(4, "z")) {
			reorder_cols_order(ascending, 2, n);
		} else {
			err("Fifth word must be `x', `y', or `z'");
			demonstrate_command_usage();
			return false;
		}
		if (n < 1) {
			warning("No x column exists, so ignoring `reorder' command");
			return true;
		}
		break;			/* not reached */
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	return false;			/* not reached */
}

static          bool
reorder_cols_randomly()
{
	int             i, inew, n;
	n = _colX.size();		/* presume no x means no cols */
	if (n < 1) {
		warning("No x column exists, so ignoring `reorder' command");
		return true;
	}
	// Seed, then do a few random numbers
#if defined(HAVE_DRAND48)
	srand48(getpid());
#else
	srand(getpid());
#endif
	for (i = 0; i < n; i++) {
#if defined(HAVE_DRAND48)	// range is 0 to 1, but do modulus in case
		inew = int(drand48() * n) % n;
#else
		inew = int(rand() % n);
#endif
		swap(&_colX, i, inew);
		swap(&_colY, i, inew);
		swap(&_colZ, i, inew);
		swap(&_colU, i, inew);
		swap(&_colV, i, inew);
		swap(&_colWEIGHT, i, inew);
	}
	return true;
}

/* `reorder columns ascending|descending in x|y|z' */
/* Use bubble-sort (slow, but easy for me to code) */
/* Also, this code could be made a lot more terse */
static          bool
reorder_cols_order(bool ascending, int which, int n)
{
	int             i, inew;
	switch (which) {
	case 0:			/* by x */
		if (_colX.size() < 1) {
			warning("No x column exists, so cannot `reorder' by it");
			return true;
		}
		if (ascending) {
			for (i = 0; i < n; i++) {
				for (inew = i; inew < n; inew++) {
					if (_colX[i] > _colX[inew]) {
						swap(&_colX, i, inew);
						swap(&_colY, i, inew);
						swap(&_colZ, i, inew);
						swap(&_colU, i, inew);
						swap(&_colV, i, inew);
						swap(&_colWEIGHT, i, inew);
					}
				}
			}
		} else {
			/* descending */
			for (i = n - 1; i >= 0; i--) {
				for (inew = i; inew >= 0; inew--) {
					if (_colX[i] > _colX[inew]) {
						swap(&_colX, i, inew);
						swap(&_colY, i, inew);
						swap(&_colZ, i, inew);
						swap(&_colU, i, inew);
						swap(&_colV, i, inew);
						swap(&_colWEIGHT, i, inew);
					}
				}
			}
		}
		break;
	case 1:			/* by y */
		if (_colY.size() < 1) {
			warning("No y column exists, so cannot `reorder' by it");
			return true;
		}
		if (ascending) {
			for (i = 0; i < n; i++) {
				for (inew = i; inew < n; inew++) {
					if (_colY[i] > _colY[inew]) {
						swap(&_colX, i, inew);
						swap(&_colY, i, inew);
						swap(&_colZ, i, inew);
						swap(&_colU, i, inew);
						swap(&_colV, i, inew);
						swap(&_colWEIGHT, i, inew);
					}
				}
			}
		} else {
			/* descending */
			for (i = n - 1; i >= 0; i--) {
				for (inew = i; inew >= 0; inew--) {
					if (_colY[i] > _colY[inew]) {
						swap(&_colX, i, inew);
						swap(&_colY, i, inew);
						swap(&_colZ, i, inew);
						swap(&_colU, i, inew);
						swap(&_colV, i, inew);
						swap(&_colWEIGHT, i, inew);
					}
				}
			}
		}
		break;
	case 2:			/* by z */
		if (_colZ.size() < 1) {
			warning("No z column exists, so cannot `reorder' by it");
			return true;
		}
		if (ascending) {
			for (i = 0; i < n; i++) {
				for (inew = i; inew < n; inew++) {
					if (_colZ[i] > _colZ[inew]) {
						swap(&_colX, i, inew);
						swap(&_colY, i, inew);
						swap(&_colZ, i, inew);
						swap(&_colU, i, inew);
						swap(&_colV, i, inew);
						swap(&_colWEIGHT, i, inew);
					}
				}
			}
		} else {
			/* descending */
			for (i = n - 1; i >= 0; i--) {
				for (inew = i; inew >= 0; inew--) {
					if (_colZ[i] > _colZ[inew]) {
						swap(&_colX, i, inew);
						swap(&_colY, i, inew);
						swap(&_colZ, i, inew);
						swap(&_colU, i, inew);
						swap(&_colV, i, inew);
						swap(&_colWEIGHT, i, inew);
					}
				}
			}
		}
		break;
	default:
		return false;		/* not reached */
	}
	return true;
}
