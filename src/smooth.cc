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

#include        <string.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"GMatrix.hh"

bool            smooth_grid_dataCmd(void);
bool            smoothCmd(void);
bool            smooth_xCmd(void);
bool            smooth_yCmd(void);

bool
smoothCmd()
{
	if (_nword > 1) {
		if (!strcmp(_word[1], "grid"))
			smooth_grid_dataCmd();
		else if (!strcmp(_word[1], "x"))
			smooth_xCmd();
		else if (!strcmp(_word[1], "y"))
			smooth_yCmd();
		else
			err("`smooth' what?? (valid options x|y|grid)");
	} else {
		err("`smooth' needs more info (valid options x|y|grid)");
		return false;
	}
	return true;
}

bool
smooth_xCmd()
{
	if (!_columns_exist) {
		warning("No x data exist yet.  Try `read columns x y'");
		return false;
	}
	if (_nword < 2 || _nword > 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	unsigned int n = _colX.size() - 1;
	std::vector<double> copy(_colX.size(), 0.0);
	// Copy all, to get endparts
	for (unsigned int i = 0; i < n; i++)
		copy[i] = _colX[i];
	if (_nword == 2) {
		copy[0] = _colX[0];
		copy[_colX.size() - 1] = _colX[_colX.size() - 1];
		for (unsigned int i = 1; i < n; i++) {
			double x0 = _colX[i];
			double xleft = _colX[i - 1];
			double xright = _colX[i + 1];
			if (!gr_missingx(x0) && !gr_missingx(xleft) && !gr_missingx(xright)) {
				copy[i] = 0.25 * (xleft + xright) + 0.5 * x0;
			} else {
				copy[i] = gr_currentmissingvalue();
			}
		}
	} else if (_nword == 3) {
		double          tmp;
		unsigned int halfwidth;
		if (!getdnum(_word[2], &tmp)) {
			err("Can't read .n. in `\\", _word[2], "'.", "\\");
			return false;
		}
		halfwidth = int(0.5 + floor(tmp) / 2);
		if (halfwidth < 1) {
			err("Improper .n.; require >= 3");
			return false;
		}
		if (halfwidth >= n) {
			err("Improper .n.; require < n / 2");
			return false;
		}
		for (unsigned int i = halfwidth; i <= n - halfwidth; i++) {
			int             j, nsum = 0;
			double          sum = 0.0;
			for (j = -int(halfwidth); j <= int(halfwidth); j++) {
				double tmp = _colX[i + j];
				if (0 < (1 + (i + j)) && (i + j) <= n && !gr_missingx(tmp)) {
					sum += tmp;
					nsum++;
				}
			}
			if (nsum > 0) {
				copy[i] = sum / nsum;
			} else {
				copy[i] = gr_currentmissingvalue();
			}
		}
	} else {
		fatal_err("Programming error in `smooth x'");
	}
	for (unsigned int i = 0; i < n; i++)
		_colX[i] = copy[i];
	return true;
}

bool
smooth_yCmd()
{
	if (!_columns_exist) {
		warning("No y data exist yet.  Try `read columns x y'");
		return false;
	}
	if (_nword < 2 || _nword > 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	unsigned int n = _colY.size() - 1;
	// Copy all, to get endparts
	std::vector<double> copy(_colY.size(), 0.0);
	for (unsigned int i = 0; i < n; i++)
		copy[i] = _colY[i];
	if (_nword == 2) {
		copy[0] = _colY[0];
		copy[_colY.size() - 1] = _colY[_colY.size() - 1];
		for (unsigned int i = 1; i < n; i++) {
			double y0 = _colY[i];
			double yleft = _colY[i - 1];
			double yright = _colY[i + 1];
			if (!gr_missingy(y0) && !gr_missingy(yleft) && !gr_missingy(yright)) {
				copy[i] = 0.25 * (yleft + yright) + 0.5 * y0;
			} else {
				copy[i] = gr_currentmissingvalue();
			}
		}
	} else if (_nword == 3) {
		double          tmp;
		unsigned int halfwidth;
		if (!getdnum(_word[2], &tmp)) {
			err("Can't read .n. in `\\", _word[2], "'.", "\\");
			return false;
		}
		halfwidth = int(0.5 + floor(tmp) / 2);
		if (halfwidth < 1) {
			err("Improper .n.; require >= 3");
			return false;
		}
		if (halfwidth >= n) {
			err("Improper .n.; require < n / 2");
			return false;
		}
		for (unsigned int i = halfwidth; i <= n - halfwidth; i++) {
			int j, nsum = 0;
			double sum = 0.0;
			for (j = -halfwidth; j <= int(halfwidth); j++) {
				double tmp = _colY[i + j];
				if (0 < (1 + (i + j)) && (i + j) <= n && !gr_missingy(tmp)) {
					sum += tmp;
					nsum++;
				}
			}
			if (nsum > 0) {
				copy[i] = sum / nsum;
			} else {
				copy[i] = gr_currentmissingvalue();
			}
		}
	} else {
		fatal_err("Programming error in `smooth y'");
	}
	for (unsigned int i = 0; i < n; i++)
		_colY[i] = copy[i];
	return true;
}

// `smooth grid data [.f.|{along x|y}]'
bool
smooth_grid_dataCmd()
{
	if (!word_is(1, "grid")) {	// should not occur
		err("correct syntax 'smooth grid data [.f]'");
		return false;
	}
	// check that data exist
	if (!grid_exists()) {
		err("grid does not exist yet");
		return false;
	}
	int method;
	double f = 1.0;
	switch (_nword) {
	case 3:
		// `smooth grid data'
		f = 1.0;
		method = 0;
		break;
	case 4:
		// `smooth grid data .f.'
		getdnum(_word[3], &f);
		if (f < 0.0) {
			warning("`smooth grid data .f.' clipping .f. to 0");
			f = 0.0;
		} else if (f > 1.0) {
			warning("`smooth grid data .f.' clipping .f. to 1");
			f = 1.0;
		}
		method = 0;
		break;
	case 5:
		// `smooth grid data along x|y'
		if (!word_is(3, "along")) {
			err("Fourth word must be `along'");
			return false;
		}
		if (word_is(4, "x")) {
			method = 1;
		} else if (word_is(4, "y")) {
			method = 2;
		} else {
			err("Last word must be `x' or `y'");
			return false;
		}
		f = 1.0;
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	GriMatrix<bool>    legitS;
	GriMatrix<double> zS;
	zS.set_size(_num_xmatrix_data, _num_ymatrix_data);
	legitS.set_size(_num_xmatrix_data, _num_ymatrix_data);
	// 
	// NOTE: I noticed in 2.068 that I was filling in the holes
	// which is stupid, as Peggy Sullivan from UW pointed out.  So
	// from 2.069 on I don't fill in the holes.
	gr_smootharray(1.0, 1.0, 1.0, _f_xy, zS,
		       _legit_xy, legitS, _num_xmatrix_data, _num_ymatrix_data,
		       method);
	for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
		for (unsigned int j = 0; j < _num_ymatrix_data; j++) {
			_f_xy(i, j) = (1.0 - f) * _f_xy(i, j) + f * zS(i, j);
			_legit_xy(i, j) = legitS(i, j);
		}
	}
	return true;
}
