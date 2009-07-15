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
#include	"gr.hh"
#include	"extern.hh"
#include	"private.hh"

// Return NO if cannot make a scale
bool
create_x_scale()
{
	int nsegs;
	double min, mint, max;
	//printf("DEBUG [create_x_scale() %s:%d] _need_x_axis=%d  _user_set_x_axis=%d\n",__FILE__,__LINE__,_need_x_axis,_user_set_x_axis);
	if (_need_x_axis && !_user_set_x_axis) {
		//extern double _grMissingValue;
		//extern bool _grMissingValueUsed;
		//printf("DEBUG [create_x_scale %s:%d] gr_missing(%le)=%d _grMissingValue=%le  _grMissingValueUsed=%d\n",__FILE__,__LINE__,_colX.min(),gr_missing(_colX.min()),_grMissingValue,_grMissingValueUsed);
		if (_colX.size() > 0 && !gr_missing(mint = _colX.min())) {
			min = mint;
			max = _colX.max();
			//printf("%s:%d min= %lf   max= %lf\n",__FILE__,__LINE__,min,max);
		} else {
			if (_xgrid_exists) {
				min = max = _xmatrix[0];
				for (unsigned int i = 1; i < _num_xmatrix_data; i++) {
					if (_xmatrix[i] < min)
						min = _xmatrix[i];
					if (_xmatrix[i] > max)
						max = _xmatrix[i];
				}
			} else {
				return false;
			}
		}
		if (_xtype == gr_axis_LINEAR) {
			int nsegs_est;
			double xsize;
			if (!get_var("..xsize..", &xsize))
				xsize = 10.0;	/* guess */
			nsegs_est = 1 + (int) (xsize / 2.0); // ??? Should use fontsize
			if (_xincreasing)
				gr_scale125(min, max, nsegs_est, &_xleft, &_xright, &nsegs);
			else
				gr_scale125(min, max, nsegs_est, &_xright, &_xleft, &nsegs);
			_xinc = (_xright - _xleft) / nsegs;
			PUT_VAR("..xleft..", _xleft);
			PUT_VAR("..xright..", _xright);
			PUT_VAR("..xinc..", _xinc);
		} else {
			// LOG axis
			if (min <= 0.0 || max <= 0.0) {
				// log data not OK
				int             nsegs_est;
				double          xsize;
				warning("Have non-positive data -- switching to linear x axis");
				if (!get_var("..xsize..", &xsize))
					xsize = 10.0;	/* guess */
				nsegs_est = 1 + (int) (xsize / 2.0);
				_xtype = gr_axis_LINEAR;
				gr_scale125(min, max, nsegs_est, &_xleft, &_xright, &nsegs);
				_xinc = ((double) _xright - (double) _xleft) / nsegs;
				PUT_VAR("..xleft..", _xleft);
				PUT_VAR("..xright..", _xright);
				PUT_VAR("..xinc..", _xinc);
			} else {
				_xinc = 1.0;
				PUT_VAR("..xleft..", _xleft = pow(10.0, floor(log10((double) min))));
				PUT_VAR("..xright..", _xright = pow(10.0, ceil(log10((double) max))));
				PUT_VAR("..xinc..", _xinc);
			}
		}
		_xscale_exists = true;
	}
	return true;
}

// Return NO if cannot make a scale
bool
create_y_scale()
{
	int nsegs;
	double min, mint, max;
	if (_need_y_axis && !_user_set_y_axis) {
		if (_colY.size() > 0 && !gr_missing(mint = _colY.min())) {
			min = mint;
			max = _colY.max();
		} else {
			if (_ygrid_exists) {
				min = max = _ymatrix[0];
				for (unsigned int i = 1; i < _num_ymatrix_data; i++) {
					if (_ymatrix[i] < min)
						min = _ymatrix[i];
					if (_ymatrix[i] > max)
						max = _ymatrix[i];
				}
			} else {
				return false;
			}
		}
		if (_ytype == gr_axis_LINEAR) {
			int             nsegs_est;
			double          ysize;
			if (!get_var("..ysize..", &ysize))
				ysize = 10.0;	// guess
			nsegs_est = 1 + (int) (ysize / 2.0);
			if (_yincreasing)
				gr_scale125(min, max, nsegs_est, &_ybottom, &_ytop, &nsegs);
			else
				gr_scale125(min, max, nsegs_est, &_ytop, &_ybottom, &nsegs);
			_yinc = (_ytop - _ybottom) / nsegs;
			PUT_VAR("..ybottom..", _ybottom);
			PUT_VAR("..ytop..", _ytop);
			PUT_VAR("..yinc..", _yinc);
		} else {
			// LOG axis
			if (min <= 0.0 || max <= 0.0) {
				// log data not OK
				int             nsegs_est;
				double          ysize;
				warning("Have non-positive data -- switching to linear y axis");
				if (!get_var("..ysize..", &ysize))
					ysize = 10.0;	// guess
				nsegs_est = 1 + (int) (ysize / 2.0);
				_ytype = gr_axis_LINEAR;
				gr_scale125(min, max, nsegs_est, &_ybottom, &_ytop, &nsegs);
				_yinc = (_ytop - _ybottom) / nsegs;
				PUT_VAR("..ybottom..", _ybottom);
				PUT_VAR("..ytop..", _ytop);
				PUT_VAR("..yinc..", _yinc);
			} else {
				_yinc = 1.0;
				PUT_VAR("..ybottom..", _ybottom = pow(10.0, floor(log10(min))));
				PUT_VAR("..ytop..", _ytop = pow(10.0, ceil(log10(max))));
				PUT_VAR("..yinc..", _yinc);
			}
		}
		_yscale_exists = true;
	}
	return true;
}
