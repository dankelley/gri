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

#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"extern.hh"
#include	"gr.hh"

static bool     heal_columnsCmd(void);
static bool     heal_gridCmd(void);
void            heal_col(double *c, unsigned int n);


// `heal columns|{grid along x|y}'
bool
healCmd()
{
	if (word_is(1, "columns")) {
		return heal_columnsCmd();
	} else if (word_is(1, "grid")) {
		if (word_is(2, "along")) {
			return heal_gridCmd();
		} else {
			err("third word must be `along'");
			return false;
		}
	} else {
		err("second word must be `columns' or `grid'");
		return false;
	}
}

// `heal columns'
bool
heal_columnsCmd()
{
	if (_nword != 2) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	if (_colX.size() > 0)
		heal_col(_colX.begin(), _colX.size());
	if (_colY.size() > 0)
		heal_col(_colY.begin(), _colY.size());
	if (_colU.size() > 0)
		heal_col(_colU.begin(), _colU.size());
	if (_colV.size() > 0)
		heal_col(_colV.begin(), _colV.size());
	if (_colZ.size() > 0)
		heal_col(_colZ.begin(), _colZ.size());
	if (_colWEIGHT.size() > 0)
		heal_col(_colWEIGHT.begin(), _colWEIGHT.size());
	return true;
}

void
heal_col(double *c, unsigned int n)
{
	unsigned int first_good;
	unsigned int last_good_index = 0;
	bool last_was_missing = false;
	// Find first good data point
	for (first_good = 0; first_good < n; first_good++)
		if (!gr_missing((double) *(c + first_good)))
			break;
	// Heal gaps, using last_good_value
	for (unsigned int i = first_good; i < n; i++) {
		if (!gr_missing((double) *(c + i))) {
			// This point is good.  If last was bad, have a hole to fill
			if (last_was_missing) {
				double range = *(c + i) - *(c + last_good_index);
				for (unsigned int ii = last_good_index + 1; ii < i; ii++) {
					*(c + ii) = *(c + last_good_index)
						+ (ii - last_good_index)
						* range
						/ (i - last_good_index);
				}
			}
			last_was_missing = false;
			last_good_index = i;
		} else {
			last_was_missing = true;
		}
	}
}

// `heal columns|{grid along x|y}'
bool
heal_gridCmd()
{
	bool            doing_x;
	// Check for obvious errors.
	if (_nword != 4) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	if (!strcmp(_word[3], "x")) {
		if (!_xgrid_exists) {
			err("First `read grid x' or `set x grid'");
			return false;
		}
		if (!grid_exists()) {
			err("No grid exists yet\n");
			return false;
		}
		doing_x = true;
	} else if (!strcmp(_word[3], "y")) {
		if (!_ygrid_exists) {
			err("First `read grid y' or `set y grid'");
			return false;
		}
		if (!grid_exists()) {
			err("No grid exists yet\n");
			return false;
		}
		doing_x = false;
	} else {
		err("Expecting `x' or `y', not `\\", _word[3], "'.", "\\");
		demonstrate_command_usage();
		return false;
	}
	if (doing_x) {
		// Heal across x.
		unsigned int i, j;
		for (j = 0; j < _num_ymatrix_data; j++) {
			for (i = 0; i < _num_xmatrix_data; i++) {
				if (_legit_xy(i, j) == false) {
					int             last_good = i - 1;
					for (; i < _num_xmatrix_data; i++) {
						if (_legit_xy(i, j) == true) {
							// Got to other side of hole. Fill in if it is
							// interior.
							if (i < _num_xmatrix_data - 1 && last_good > -1) {
								unsigned int ii;
								double          a, b;	// y = a + bx
								a = _f_xy(last_good, j);
								b = (_f_xy(i, j) - _f_xy(last_good, j)) /
									(_xmatrix[i] - _xmatrix[last_good]);
								for (ii = last_good + 1; ii < i; ii++) {
									_f_xy(ii, j) = a + b
										* (_xmatrix[ii] - _xmatrix[last_good]);
									_legit_xy(ii, j) = true;
								}
							}
							break;
						}
					}
				}
			}
		}
	} else {
		// Heal across y.
		unsigned int i, j;
		for (i = 0; i < _num_xmatrix_data; i++) {
			for (j = 0; j < _num_ymatrix_data; j++) {
				if (_legit_xy(i, j) == false) {
					int             last_good = j - 1;
					for (; j < _num_ymatrix_data; j++) {
						if (_legit_xy(i, j) == true) {
							// Got to other side of hole. Fill in if it is
							// interior.
							if (j < _num_ymatrix_data - 1 && last_good > -1) {
								unsigned int jj;
								double          a, b;	// y = a + bx
								a = _f_xy(i, last_good);
								b = (_f_xy(i, j) - _f_xy(i, last_good)) /
									(_ymatrix[j] - _ymatrix[last_good]);
								for (jj = last_good + 1; jj < j; jj++) {
									_f_xy(i, jj) = a + b
										* (_ymatrix[jj] - _ymatrix[last_good]);
									_legit_xy(i, jj) = true;
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	return true;
}
