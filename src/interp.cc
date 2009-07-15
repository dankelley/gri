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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include	"extern.hh"
#include        "files.hh"
#include        "superus.hh"

// `interpolate x grid to .left. .right. .inc.|{/.cols.}'
// `interpolate y grid to .bottom. .top. .inc.|{/.rows.}'
bool
interpolateCmd()
{
	bool is_x = false;
	if (word_is(1, "x")) 
		is_x = true;
	else if (!word_is(1, "y")) {
		err("Second word must be `x' or `y'");
	}
	int num;
	double start, stop, increment;
	if (is_x) {
		Require(getdnum(_word[4], &start), READ_WORD_ERROR(".left."));
		Require(getdnum(_word[5], &stop),  READ_WORD_ERROR(".right."));
	} else {
		Require(getdnum(_word[4], &start), READ_WORD_ERROR(".bottom."));
		Require(getdnum(_word[5], &stop),  READ_WORD_ERROR(".top."));
	}
	switch (_nword) {
	case 7:
		if (is_x) {
			Require(getdnum(_word[6], &increment), 
				READ_WORD_ERROR(".cols."));
		} else {
			Require(getdnum(_word[6], &increment), 
				READ_WORD_ERROR(".rows."));
		}
		num = int(1 + (stop - start) / increment);
		break;
	case 8:
		Require(word_is(6, "/"), err("6-th word must be `/'"));
		Require(getinum(_word[7], &num), READ_WORD_ERROR(".inc."));
		if (num < 2) {
			err("Increment .inc. must exceed 1");
			return false;
		}
		increment = (stop - start) / (num - 1);
		break;
	}
	if (num < 0) {
		err("Sign of increment disagrees with start and stop values");
		return false;
	}
	//
	// The grid must already exist
	if (!_grid_exists) {
		err("Cannot `convert grid to image' since no grid data exist yet");
		return false;
	}
	if (!_xgrid_exists) {
		err("Cannot `convert grid to image' since x-grid doesn't exist yet");
		return false;
	}
	if (!_ygrid_exists) {
		err("Cannot `convert grid to image' since y-grid doesn't exist yet");
		return false;
	}
	increment *= 0.9999999;	// to avoid overflow
	double znew;
	GriMatrix<double> _f_xy_new;
	if (is_x) {
		std::vector<double> ygrid((size_t)_num_ymatrix_data, 0.0);
		_f_xy_new.set_size(num, _num_ymatrix_data);
		unsigned int i, j;
		j = _num_ymatrix_data - 1;
		do {
			ygrid[j] = _ymatrix[j];
			double xnew = start;
			for (i = 0; i < (unsigned int)num; i++) {
				grid_interp(xnew, _ymatrix[j], &znew);
				_f_xy_new(i, j) = znew;
				xnew += increment;
			}
		} while (j-- != 0);
		// Dump into official storage
		allocate_grid_storage(num, _num_ymatrix_data);
		allocate_xmatrix_storage(num);
		for (i = 0; i < (unsigned int)num; i++) 
			_xmatrix[i] = start + i * increment;
		for (j = 0; j < _num_ymatrix_data; j++)
			_ymatrix[j] = ygrid[j];
		for (j = 0; j < _num_ymatrix_data; j++) {
			for (i = 0; i < (unsigned int)num; i++) {
				_f_xy(i, j) = _f_xy_new(i, j);
				_legit_xy(i, j) = true;
			}
		}
	} else {
		std::vector<double> xgrid((size_t)_num_xmatrix_data, 0.0);
		_f_xy_new.set_size(_num_xmatrix_data, num);
		unsigned int i, j;
		for (i = 0; i < _num_xmatrix_data; i++) {
			xgrid[i] = _xmatrix[i];
			double ynew = start;
			for (j = 0; j < (unsigned int)num; j++) {
				grid_interp(_xmatrix[i], ynew, &znew);
				_f_xy_new(i, j) = znew;
				ynew += increment;
			}
		}
		// Dump into official storage
		allocate_grid_storage(_num_xmatrix_data, num);
		allocate_ymatrix_storage(num);
		for (i = 0; i < _num_xmatrix_data; i++) 
			_xmatrix[i] = xgrid[i];
		for (j = 0; j < (unsigned int) num; j++)
			_ymatrix[j] = start + j * increment;
		for (i = 0; i < _num_xmatrix_data; i++) {
			for (j = 0; j < (unsigned int)num; j++) {
				_f_xy(i, j) = _f_xy_new(i, j);
				_legit_xy(i, j) = true;
			}
		}
	}
	return true;
}
