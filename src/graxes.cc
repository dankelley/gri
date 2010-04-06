/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2010 Daniel Kelley

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

//#define DEBUG_LABELLING 1
//#define DEBUG_LABELS 1
#include <string>
#include <string.h>
#if !defined(IS_MINGW32)
#include <strings.h>
#else
#define index strrchr
#endif

#include <math.h>
#include <stdlib.h>
#include "gr.hh"
#include "GriPath.hh"
#include "extern.hh"

#if defined(__DECCXX) || defined(OS_IS_BEOS)
extern "C" char *index(const char *s, int c);
#endif

#define TIC_RATIO	0.5	/* (Length small tic) / (large tic) */
extern char     _grXAxisLabel[];
extern char     _grYAxisLabel[];
extern char     _grNumFormat_x[];
extern char     _grNumFormat_y[];
extern char     _grTempString[];
extern int      _grAxisStyle_x;
extern int      _grAxisStyle_y;
extern gr_axis_properties _grTransform_x;
extern gr_axis_properties _grTransform_y;
extern int      _grNeedBegin;
extern int      _grNumSubDiv_x;
extern int      _grNumSubDiv_y;
extern double   _grCmPerUser_x;
extern double   _grCmPerUser_y;
extern double   _grTicSize_cm;

#define	round_down_to_10(x) 	(pow (10.0, floor (log10 ( (x) ))))
#define	round_to_1(x)		(floor (0.5 + (x) ))

/* Some twiddles */
#define AXIS_TWIDDLE            0.005	/* .05mm overhang on axis */
#define SMALLNUM                (1.0e-3)	/* twiddle axis range */
#define SMALLERNUM              (1.0e-4)	/* twiddle axis range */
#define SMALLFONTSIZE           (1.0e-3)	/* too small to draw */
/*
 * local functions
 */
static bool     next_tic(double *next, double labelling, bool gave_labelling, double present, double final, double inc, gr_axis_properties axis_type, bool increasing);

#if 0
static int      num_decimals(char *s);
static int      create_labels(double y, double yb, double yinc, double yt, double smallinc, char *label[], int *num_label);
#endif


// Calculate next tic location on axis.  Note that if the axis starts off
// ragged, this will assign to *next the first multiple of "inc". Returns
// true if more axis to do
static bool
next_tic(double *next, double labelling, bool gave_labelling, double present, double final, double inc, gr_axis_properties axis_type, bool increasing)
{
#if defined(DEBUG_LABELLING)
        printf("%s:%d  next_tic(...,labelling=%f  gave_labelling=%s  present=%f  final=%f  inc=%f  increasing=%s  _x_gave_labelling=%s  _y_gave_labelling=%s\n", __FILE__,__LINE__,labelling,gave_labelling?"true":"false",present,final,inc,increasing?"true":"false", _x_gave_labelling?"true":"false", _y_gave_labelling?"true":"false");
#endif
	double          order_of_mag, mantissa;
	// Check to see if already ran out of axis.
	if (present >= final && increasing == true)
		return false;
        if (present <= final && increasing == false)
		return false;
        // Determine next tic to draw to, treating linear/log separately.
	switch (axis_type) {
	case gr_axis_LINEAR:
		//*next = inc * (1.0 + floor((SMALLERNUM * inc + (present - labelling)) / inc));
                if (gave_labelling) {
                        *next = labelling + inc * (1 + floor(SMALLERNUM + (present - labelling) / inc));
#if defined(DEBUG_LABELLING)
                        printf("%s:%d   next=%f  present=%f  inc=%f\n  %f  %f  %f %f\n",
                               __FILE__, __LINE__, *next, present, inc,
                               inc * (1.001 + floor((present - labelling) / inc)),
                                (1 + floor((present - labelling) / inc)),
                                (floor((present - labelling) / inc)),
                                (present - labelling) / inc);
#endif
                } else {
                        *next = inc * (1.0 + floor((SMALLERNUM * inc + present) / inc));
                }
		break;
	case gr_axis_LOG:
		if (present <= 0.0) {
			err("zero or negative on log axis (internal error).");
                        return false;
                }
		order_of_mag = round_down_to_10(present);
		mantissa = present / order_of_mag;
		if (increasing)
			mantissa = ceil(mantissa + SMALLNUM);
		else {
			if (mantissa <= 1.0)
				mantissa = 0.9;
			else
				mantissa = floor(mantissa - SMALLNUM);
		}
		*next = order_of_mag * mantissa;
		break;
	default:
		gr_Error("unknown axis type (internal error)");
	}
	// Set flag if this will overrun axis.
	if (increasing == true)
		return (*next <= final) ? true : false;
	else
		return (*next >= final) ? true : false;
}

/*
 * gr_drawxyaxes DESCRIPTION: Draws an x-y axis frame
 */
void
gr_drawxyaxes(double xl, double xinc, double xr, double yb, double yinc, double yt)
{
	double          old_fontsize_pt = gr_currentfontsize_pt();
	gr_drawxaxis(yb, xl, xinc, xr, xl, gr_axis_BOTTOM);
	gr_drawyaxis(xl, yb, yinc, yt, yb, gr_axis_LEFT);
	gr_setfontsize_pt(0.0);
	gr_drawxaxis(yt, xl, xinc, xr, xl, gr_axis_TOP);
	gr_drawyaxis(xr, yb, yinc, yt, yb, gr_axis_RIGHT);
	gr_setfontsize_pt(old_fontsize_pt);
}

// DESCRIPTION: The axis extends from 'xl' to 'xr',with numbers placed at
// intervals of 'xinc'.
// If 'side'==BOTTOM/TOP this is an axis designed to appear at the
// bottom/top of a plotting region (ie, the numbers are below/above).
void
gr_drawxaxis(double y, double xl, double xinc, double xr, double xlabelling, gr_axis_properties side)
{
        //printf("gr_drawxaxis(..., xl=%f xlabelling=%f same=%d\n", xl, xlabelling, xl==xlabelling);
	bool user_gave_labels = (_x_labels.size() != 0);
#ifdef DEBUG_LABELS
	if (user_gave_labels) {
		printf("DEBUG: %s:%d x axis should have labels [", __FILE__,__LINE__);
		for (unsigned int i = 0; i < _x_labels.size(); i++)
			printf("'%s' ", _x_labels[i].c_str());
		printf("] at positions [");
		for (unsigned int i = 0; i < _x_label_positions.size(); i++)
			printf("%f ", _x_label_positions[i]);
		printf("]\n");
	}
#endif	
	GriString label;
	std::string slabel;
	extern char     _xtype_map;
	double          CapHeight = gr_currentCapHeight_cm();
	double          angle = 0.0;	// angle of axis tics, labels, etc
#ifdef DEBUG_LABELS
	printf("DEBUG: %s:%d at top of gr_drawxaxis(), angle is %f\n", __FILE__, __LINE__, angle);
#endif
	bool            increasing = ((xr > xl) ? true : false);
	double          tic, tic_sml;	// length of tic marks
	double          xcm, ycm;	// tmp
	double          offset;	// for numbers
	double          present, next, final = xr, smallinc = 0.0;
	int             decade_between_labels;	// for log axes
	double          tmp1, tmp2;
	GriPath         axis_path;
	// XREF -- axis transform
	// Calculate size of large and small tic marks.
	extern bool     _grTicsPointIn;
	tic = ((side == gr_axis_LEFT && _grTicsPointIn == false)
	       || (side == gr_axis_RIGHT && _grTicsPointIn == true))
		? -_grTicSize_cm : _grTicSize_cm;
	if (_grTransform_x == gr_axis_LOG && _xsubdiv < 0)
		tic_sml = 0.0;
	else
		tic_sml = TIC_RATIO * tic;
	// Calculate offset = space for numbers.
	offset = 0.0;
	int old_linecap = _griState.line_cap();
	int old_linejoin = _griState.line_join();
	_griState.set_line_cap(0);
	_griState.set_line_join(0);
	switch (_grTransform_x) {
	case gr_axis_LINEAR:
	case gr_axis_LOG:
		if (side == gr_axis_BOTTOM) {
			if (strlen(_grNumFormat_x))
				offset -= 1.75 * CapHeight;
			offset -= ((_grTicsPointIn == false) ? _grTicSize_cm : 0.0);
		} else {
			if (strlen(_grNumFormat_x))
				offset += 0.75 * CapHeight;
			offset += ((_grTicsPointIn == false) ? _grTicSize_cm : 0.0);
		}
		break;
	default:
		gr_Error("unknown axis type (internal error)");
	}
	// Draw axis, moving from tic to tic.  Tics are advanced by smallinc for
	// linear axes and by something appropriate for log axes. Whenever the
	// current location should be a big tic, such a tic is drawn along with a
	// label.
	double xl_cm, y_cm;
        int this_pass=0, pass_max=5000;
	switch (_grTransform_x) {
	case gr_axis_LINEAR:
		smallinc = xinc / _grNumSubDiv_x;
		// Twiddle axes to extend a bit beyond the requested
		// region, to prevent rounding problems.
		present = xl - xinc / 1.0e3;
		final   = xr + xinc / 1.0e3;
		// Draw x-axis, moving from tic to tic.  Tics are advanced by
		// smallinc for linear axes and by something appropriate for log
		// axes. Whenever the current location should be a big tic, such a
		// tic is drawn along with a label.
		gr_usertocm(xl, y, &xl_cm, &y_cm);
		axis_path.push_back(xl_cm, y_cm, 'm');
#if defined(DEBUG_LABELLING)
                printf("%s:%d  _x_gave_labelling=%d xlabelling=%f   present=%f\n", __FILE__, __LINE__, _x_gave_labelling, xlabelling, present);
#endif
		while (next_tic(&next, xlabelling, _x_gave_labelling, present, final, smallinc, _grTransform_x, increasing)) {
                        if (this_pass++ > pass_max) {
                                extern bool _x_gave_labelling;
                                if (_x_gave_labelling) {
                                        gr_Error("cannot draw x axis (internal error: problem dealing with 'labelling' keyword)");
                                } else {
                                        gr_Error("cannot draw x axis (internal error)");
                                }
                                return;
                        }
			// Determine angle of x-axis tics, for non-rectangular axes
			switch (_grTransform_x) {
			case gr_axis_LINEAR:
			case gr_axis_LOG:
				angle = atan2(1.0, 0.0); // angle for tics
				break;
			default:
				gr_Error("unknown axis type (internal error)");
				break;
			}
			gr_usertocm(next, y, &xcm, &ycm);
			axis_path.push_back(xcm, ycm, 'l');
			// Detect large tics on x axis
			if ((_x_gave_labelling && gr_multiple(next - xlabelling, xinc, 0.01 * smallinc))
                            || (!_x_gave_labelling && gr_multiple(next, xinc, 0.01 * smallinc))) {
#if defined(DEBUG_LABELLING)
                                printf("%s:%d   next=%f\n", __FILE__, __LINE__, next);
#endif
				// draw large tic
				axis_path.push_back(xcm + tic * cos(angle), ycm + tic * sin(angle), 'l');
				if (gr_currentfontsize_pt() > SMALLFONTSIZE) {
					if (_xtype_map != ' ') {
						// It's a map, so figure the deg/min/seconds;
						// over-ride any existing format
						int             hour, min, sec;
						if (gr_multiple(next, 1.0, 1.0e-6)) {
							hour = (int)floor(1.0e-4 + fabs(next));
							if (next >= 0.0) 
								sprintf(_grTempString,"%d$\\circ$%c",hour,_xtype_map);
							else
								sprintf(_grTempString,"-%d$\\circ$%c",hour,_xtype_map);
						} else if (gr_multiple(next, 1.0 / 60.0, 1.0e-7)) {
							hour = (int)floor(1.0e-4 + fabs(next));
							min = (int)floor(1e-5 + 60.0 * (fabs(next) - hour));
							if (next >= 0.0)
								sprintf(_grTempString,"%d$\\circ$%d'%c",hour,min,_xtype_map);
							else 
								sprintf(_grTempString,"-%d$\\circ$%d'%c",hour,min,_xtype_map);
						} else if (gr_multiple(next, 1.0 / 3600.0, 1.0e-8)) {
							hour = (int)floor(1.0e-4 + fabs(next));
							min = (int)floor(1e-5 + 60.0 * (fabs(next) - hour));
							sec = (int)floor(1e-5 + 3600.0 * (fabs(next) - hour - min / 60.0));
							if (next >= 0.0)
								sprintf(_grTempString, "%d$\\circ$%d'%d\"%c",hour,min,sec,_xtype_map);
							else 
								sprintf(_grTempString, "-%d$\\circ$%d'%d\"%c",hour,min,sec,_xtype_map);
						} else {
							sprintf(_grTempString,"%f$\\circ$%c",next,_xtype_map);
						}
					} else if (strlen(_grNumFormat_x)) {
						sprintf(_grTempString, _grNumFormat_x, next);
						if (get_flag("emulate_gre")) {
							char *e = index(_grTempString, int('E'));
							if (e != NULL) {
								std::string gs(_grTempString);
								size_t chop;
								if (STRING_NPOS != (chop = gs.find("E+0"))) {
									gs.replace(chop, 3, "$\\times10^{");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E-0"))) {
									gs.replace(chop, 3, "$\\times10^{-");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E+"))) {
									gs.replace(chop, 2, "$\\times10^{");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E-"))) {
									gs.replace(chop, 2, "$\\times10^{-");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E"))) {
									gs.replace(chop, 1, "$\\times10^{");
									gs.append("}$");
								}
								strcpy(_grTempString, gs.c_str());
							}
						}
					} else {
						*_grTempString = '\0';
					}
					angle = 0;
#ifdef DEBUG_LABELS
					printf("DEBUG: %s:%d after the loop, angle is %f\n", __FILE__, __LINE__, angle);
#endif
					if (!user_gave_labels) {
						slabel.assign(_grTempString);
						fix_negative_zero(slabel);
						label.fromSTR(slabel.c_str());
						label.draw(xcm - offset * sin(angle),
							   ycm + offset * cos(angle),
							   TEXT_CENTERED,
							   DEG_PER_RAD * angle);
					}
				}
			} else {
				// Small tic
				axis_path.push_back(xcm + tic_sml * cos(angle), ycm + tic_sml * sin(angle), 'l');
			}
			axis_path.push_back(gr_usertocm_x(next, y), gr_usertocm_y(next, y), 'l');
			present = next;
#if defined(DEBUG_LABELLING)
                        printf("%s:%d   bottom of loop; present=%f\n", __FILE__, __LINE__, present);
#endif
		}
		if (user_gave_labels) {
			angle = 0;
			for (unsigned int i = 0; i < _x_labels.size(); i++) {
				if (BETWEEN(xl, xr, _x_label_positions[i])) {
					label.fromSTR(_x_labels[i].c_str()); // BUG: should interpolate into this string
					gr_usertocm(_x_label_positions[i], y, &xcm, &ycm);
#ifdef DEBUG_LABELS
					printf("DEBUG: %s:%d drawing %d-th label '%s' at x=%f angle=%f\n",__FILE__,__LINE__,i,_x_labels[i].c_str(),_x_label_positions[i],angle);
#endif
					label.draw(xcm - offset * sin(angle),
						   ycm + offset * cos(angle),
						   TEXT_CENTERED,
						   DEG_PER_RAD * angle);
				} else {
#ifdef DEBUG_LABELS
					printf("DEBUG: %s:%d SKIPPING %d-th label '%s' since x=%f\n",__FILE__,__LINE__,i,_x_labels[i].c_str(),_x_label_positions[i]);
#endif
				}
			}
		}
		// Finish by drawing to end of axis (in case there was no tic there).
		axis_path.push_back(gr_usertocm_x(final, y), gr_usertocm_y(final, y), 'l');
		axis_path.stroke(units_cm, _griState.linewidth_axis());
		break;
	case gr_axis_LOG:
		decade_between_labels = (int) floor(0.5 + xinc);
		gr_usertocm(xl, y, &xcm, &ycm);
		gr_cmtouser(xcm - AXIS_TWIDDLE, ycm, &tmp1, &tmp2);
		present = tmp1;
		axis_path.push_back(present, y, 'm');
#if defined(DEBUG_LABELLING)
                printf("%s:%d  _x_gave_labelling=%d xlabelling=%f   present=%f\n", __FILE__, __LINE__, _x_gave_labelling, xlabelling, present);
#endif
		while (next_tic(&next, xl, _x_gave_labelling, present, final, smallinc, _grTransform_x, increasing)) {
			double          tmp, next_log;
			double xuser, yuser;
			axis_path.push_back(next, y, 'l');
			next_log = log10(next);
			tmp = next_log - floor(next_log);
			gr_usertocm(next, y, &xcm, &ycm);
			if (-0.01 < tmp && tmp < 0.01) {
				// large tic & number
				gr_cmtouser(xcm, ycm+tic, &xuser, &yuser);
				axis_path.push_back(xuser, yuser, 'l');
				gr_cmtouser(xcm, ycm+offset, &xuser, &yuser);
				tmp = next_log - decade_between_labels * floor(next_log / decade_between_labels);
				if (!user_gave_labels 
                                    && gr_currentfontsize_pt() > SMALLFONTSIZE 
                                    && -0.01 < tmp / xinc 
                                    && tmp / xinc < 0.01 
                                    && strlen(_grNumFormat_x)) {
					// Draw "1" as a special case
					if (0.99 < next && next < 1.01)
						sprintf(_grTempString, "1");
					else
						sprintf(_grTempString, "$10^{%.0f}$", log10(next));
					slabel.assign(_grTempString);
					fix_negative_zero(slabel);
					label.fromSTR(slabel.c_str());
					label.draw(xcm, ycm + offset, TEXT_CENTERED, 0.0);
				}
			} else {
				// small tic
				gr_cmtouser(xcm, ycm+tic_sml,&xuser, &yuser);
				axis_path.push_back(xuser, yuser, 'l');
			}
			axis_path.push_back(next, y, 'm');
			present = next;
		}
		if (user_gave_labels) {
			angle = 0;
			for (unsigned int i = 0; i < _x_labels.size(); i++) {
				if (BETWEEN(xl, xr, _x_label_positions[i])) {
					label.fromSTR(_x_labels[i].c_str()); // BUG: should interpolate into this string
					gr_usertocm(_x_label_positions[i], y, &xcm, &ycm);
#ifdef DEBUG_LABELS
					printf("DEBUG: %s:%d drawing %d-th label '%s' at x=%f angle=%f\n",__FILE__,__LINE__,i,_x_labels[i].c_str(),_x_label_positions[i],angle);
#endif
					label.draw(xcm - offset * sin(angle), ycm + offset * cos(angle), TEXT_CENTERED, DEG_PER_RAD * angle);
				} else {
#ifdef DEBUG_LABELS
					//printf("DEBUG: %s:%d SKIPPING %d-th label '%s' since x=%f\n",__FILE__,__LINE__,i,_x_labels[i].c_str(),_x_label_positions[i]);
#endif
				}
			}
		}
		// Finish by drawing to end of axis (in case there was no tic there).
		axis_path.push_back(final, y, 'l');
		axis_path.stroke(units_user, _griState.linewidth_axis());
		break;
	default:
		gr_Error("unknown axis type (internal error)");
	}
	// Draw axis title.
	if (gr_currentfontsize_pt() > SMALLFONTSIZE) {
		label.fromSTR(_grXAxisLabel);
		switch (_grTransform_x) {
		case gr_axis_LINEAR:
			if (side == gr_axis_TOP) {
				double          xcm, ycm;
				gr_usertocm(0.5 * (xl + final), y, &xcm, &ycm);
				label.draw(xcm,
					   ycm + offset + 1.75 * CapHeight,
					   TEXT_CENTERED,
					   0.0);
			} else {
				double          xcm, ycm;
				gr_usertocm(0.5 * (xl + final), y, &xcm, &ycm);
				label.draw(xcm,
					   ycm + offset - 1.75 * CapHeight,
					   TEXT_CENTERED,
					   0.0);
			}
			break;
		case gr_axis_LOG:
			if (side == gr_axis_TOP) {
				double          xcm, ycm;
				gr_usertocm(sqrt(xl * final), y, &xcm, &ycm);
				label.draw(xcm,
					   ycm + offset + (1.75 + 0.75) * CapHeight,
					   TEXT_CENTERED,
					   0.0);
			} else {
				double          xcm, ycm;
				gr_usertocm(sqrt(xl * final), y, &xcm, &ycm);
				label.draw(xcm,
					   ycm + offset - 1.75 * CapHeight,
					   TEXT_CENTERED,
					   0.0);
			}
			break;
		default:
			gr_Error("unknown axis type (internal error)");
		}
	}
	_griState.set_line_cap(old_linecap);
	_griState.set_line_join(old_linejoin);
}

#define FACTOR 1.35		// Kludge to scale fonts up
// Draw a y axis
void
gr_drawyaxis(double x, double yb, double yinc, double yt, double ylabelling, gr_axis_properties side)
{
#if 1				// 2.9.x
	bool user_gave_labels = (_y_labels.size() != 0);
#endif // 2.9.x
	GriString label;
	std::string slabel;
	extern char     _ytype_map;
	double          CapHeight = gr_currentCapHeight_cm();
	double          angle = 0.0; // angle of axis tics, labels, etc
	bool            increasing = ((yt > yb) ? true : false);
	double          tic, tic_sml; // length of tic marks
	double          xcm, ycm; // used to step along axis
	double          xcm2, ycm2; // tmp, allowed to mess with
	double          labelx_cm, labely_cm; // where tic label will go
	double          offset;	// for numbers
	double          present, next, final = yt, smallinc = 0.0;
	int             decade_between_labels; // for log axes
	double          max_num_width_cm = 0.0;	// use for positioning label
	double          tmp0, tmp1, tmp2;
	GriPath         axis_path;
	// Calculate size of large and small tic marks.
	extern bool     _grTicsPointIn;
	tic = ((side == gr_axis_LEFT && _grTicsPointIn == true)
	       || (side == gr_axis_RIGHT && _grTicsPointIn == false))
		? _grTicSize_cm : -_grTicSize_cm;
	if (_grTransform_y == gr_axis_LOG && _ysubdiv < 0)
		tic_sml = 0.0;
	else
		tic_sml = TIC_RATIO * tic;
	// Calculate offset = space for numbers.
	if (side == gr_axis_LEFT) {
		if (_grTicsPointIn == true) {
			offset = -0.5 * FACTOR * CapHeight;
		} else {
			offset = -0.5 * FACTOR * CapHeight - _grTicSize_cm;
		}
	} else {
		if (_grTicsPointIn == true) {
			offset = 0.5 * FACTOR * CapHeight;
		} else {
			offset = 0.5 * FACTOR * CapHeight + _grTicSize_cm;
		}
	}
	int old_linecap = _griState.line_cap();
	int old_linejoin = _griState.line_join();
	_griState.set_line_cap(0);
	_griState.set_line_join(0);
	// Draw y-axis, moving from tic to tic.  Tics are advanced by smallinc
	// for linear axes and by something appropriate for log axes. Whenever
	// the current location should be a big tic, such a tic is drawn along
	// with a label.
        int this_pass=0, pass_max=5000;
	switch (_grTransform_y) {
	case gr_axis_LINEAR:
		smallinc = yinc / _grNumSubDiv_y;
		present = yb - yinc / 1.0e3;
		final   = yt + yinc / 1.0e3;
		axis_path.push_back(gr_usertocm_x(x, yb), gr_usertocm_y(x, yb), 'm');
#if defined(DEBUG_LABELLING)
                printf("%s:%d  _y_gave_labelling=%d ylabelling=%f   present=%f\n", __FILE__, __LINE__, _y_gave_labelling, ylabelling, present);
#endif
		while (next_tic(&next, ylabelling, _y_gave_labelling, present, final, smallinc, _grTransform_y, increasing)) {
			axis_path.push_back(gr_usertocm_x(x, next), gr_usertocm_y(x, next), 'l');
			gr_usertocm(x, next, &xcm, &ycm);
			angle = 0.0;
			// Detect large tics on y axis
			if ((_y_gave_labelling && gr_multiple(next - ylabelling, yinc, 0.01 * smallinc))
                            || (!_y_gave_labelling && gr_multiple(next, yinc, 0.01 * smallinc))) {
				double tmpx, tmpy;
				gr_cmtouser(xcm + tic * cos(angle), ycm + tic * sin(angle), &tmpx, &tmpy);
				axis_path.push_back(xcm + tic * cos(angle), ycm + tic * sin(angle), 'l');
				labelx_cm = xcm + offset * cos(angle);
				labely_cm = ycm + offset * sin(angle) - 0.5 * CapHeight;
				if (gr_currentfontsize_pt() > SMALLFONTSIZE) {
					if (_ytype_map != ' ') {
						// It's a map, so figure the deg/min/seconds;
						// over-ride any existing format
						int             hour, min, sec;
						if (gr_multiple(next, 1.0, 1.0e-6)) {
							hour = (int)floor(1.0e-4 + fabs(next));
							if (next >= 0.0)
								sprintf(_grTempString,"%d$\\circ$%c",hour,_ytype_map);
							else
								sprintf(_grTempString,"-%d$\\circ$%c",hour,_ytype_map);
						} else if (gr_multiple(next, 1.0 / 60.0, 1.0e-7)) {
							hour = (int)floor(1.0e-4 + fabs(next));
							min = (int)floor(1e-5 + 60.0 * (fabs(next) - hour));
							if (next >= 0.0) 
								sprintf(_grTempString,"%d$\\circ$%d'%c",hour,min,_ytype_map);
							else 
								sprintf(_grTempString,"-%d$\\circ$%d'%c",hour,min,_ytype_map);
						} else if (gr_multiple(next, 1.0 / 3600.0, 1.0e-8)) {
							hour = (int)floor(1.0e-4 + fabs(next));
							min = (int)floor(1e-5 + 60.0 * (fabs(next) - hour));
							sec = (int)floor(1e-5 + 3600.0 * (fabs(next) - hour - min / 60.0));
							if (next >= 0.0)
								sprintf(_grTempString, "%d$\\circ$%d'%d\"%c",hour,min,sec,_ytype_map);
							else 
								sprintf(_grTempString, "-%d$\\circ$%d'%d\"%c",hour,min,sec,_ytype_map);
						} else {
							sprintf(_grTempString,"%f$\\circ$%c",next,_ytype_map);
						}
					} else if (strlen(_grNumFormat_y)) {
						if (get_flag("emulate_gre")) {
							sprintf(_grTempString, _grNumFormat_y, next);
							char *e = index(_grTempString, int('E'));
							if (e != NULL) {
								std::string gs(_grTempString);
								size_t chop;
								if (STRING_NPOS != (chop = gs.find("E+0"))) {
									gs.replace(chop, 3, "$\\times10^{");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E-0"))) {
									gs.replace(chop, 3, "$\\times10^{-");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E+"))) {
									gs.replace(chop, 2, "$\\times10^{");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E-"))) {
									gs.replace(chop, 2, "$\\times10^{-");
									gs.append("}$");
								} else if (STRING_NPOS != (chop = gs.find("E"))) {
									gs.replace(chop, 1, "$\\times10^{");
									gs.append("}$");
								}
								strcpy(_grTempString, gs.c_str());
							}
						} else {
							sprintf(_grTempString, _grNumFormat_y, next);
						}
					} else {
						*_grTempString = '\0';
					}
					if (!user_gave_labels) { // 2.9.x
						slabel.assign(_grTempString);
						fix_negative_zero(slabel);
						label.fromSTR(slabel.c_str());
						if (side == gr_axis_LEFT)
							label.draw(labelx_cm, labely_cm, TEXT_RJUST, angle * DEG_PER_RAD);
						else
							label.draw(labelx_cm, labely_cm, TEXT_LJUST, angle * DEG_PER_RAD);
					}
					// Keep track of maximum width of axis numbers, so that
					// axis label can be offset right amount.
					gr_stringwidth(_grTempString, &tmp0, &tmp1, &tmp2);
					if (tmp0 > max_num_width_cm)
						max_num_width_cm = tmp0;
				}
			} else {
				// Small tic
				axis_path.push_back(xcm + tic_sml * cos(angle), ycm + tic_sml * sin(angle), 'l');
			}
			axis_path.push_back(gr_usertocm_x(x, next), gr_usertocm_y(x, next), 'l');
			present = next;
		}
#if 1				// 2.9.x
		if (user_gave_labels) {
		  //printf("labels...\n");
			for (unsigned int i = 0; i < _y_labels.size(); i++) {
				label.fromSTR(_y_labels[i].c_str()); // BUG: should interpolate into this string
				gr_usertocm(x, _y_label_positions[i], &xcm, &ycm);
				labelx_cm = xcm + offset * cos(angle);
				labely_cm = ycm + offset * sin(angle) - 0.5 * CapHeight;
				//printf("%f %f %f %f\n", ycm, offset*sin(angle),CapHeight, labely_cm);
				if (side == gr_axis_LEFT)
					label.draw(labelx_cm, labely_cm, TEXT_RJUST, DEG_PER_RAD * angle);
				else
					label.draw(labelx_cm, labely_cm, TEXT_LJUST, DEG_PER_RAD * angle);
				gr_stringwidth(_y_labels[i].c_str(), &tmp0, &tmp1, &tmp2);
				if (tmp0 > max_num_width_cm)
					max_num_width_cm = tmp0;
			}
		}
#endif
		// Finish by drawing to end of axis (in case there was no tic there).
		axis_path.push_back(gr_usertocm_x(x, yt), gr_usertocm_y(x, yt), 'l');
		axis_path.stroke(units_cm, _griState.linewidth_axis());
		break;
	case gr_axis_LOG:
		decade_between_labels = (int) floor(0.5 + yinc);
		gr_usertocm(x, yb, &xcm, &ycm);
		gr_cmtouser(xcm, ycm - AXIS_TWIDDLE, &tmp1, &tmp2);
		present = tmp2;
		axis_path.push_back(x, present, 'm');
                if (_y_gave_labelling)  {
                        err("cannot use 'labelling' parameter for logarithmic axis");
                        return;
                }
#if defined(DEBUG_LABELLING)
                printf("%s:%d  _y_gave_labelling=%d ylabelling=%f   present=%f\n", __FILE__, __LINE__, _y_gave_labelling, ylabelling, present);
#endif
		while (next_tic(&next, yb, _y_gave_labelling, present, final, smallinc, _grTransform_y, increasing)) {
                        if (this_pass++ > pass_max) {
                                extern bool _y_gave_labelling;
                                if (_y_gave_labelling) {
                                        gr_Error("cannot draw y axis (internal error: cannot use 'labelling' keyword)");
                                } else {
                                        gr_Error("cannot draw y axis (internal error)");
                                }
                                return;
                        }
			double          tmp, next_log;
			axis_path.push_back(x, next, 'l');
			next_log = log10(next);
			tmp = next_log - floor(next_log);
			gr_usertocm(x, next, &xcm2, &ycm2);	// NOTE: not using (xcm,ycm)
			if (-0.01 < tmp && tmp < 0.01) {
				// large tic & number
				double xuser, yuser;
				gr_cmtouser(xcm2 + tic, ycm2, &xuser, &yuser);
				axis_path.push_back(xuser, yuser, 'l');
				gr_cmtouser(xcm2 + tic, ycm2 - 0.5 * FACTOR * CapHeight, &xuser, &yuser);
				tmp = next_log - decade_between_labels * floor(next_log / decade_between_labels);
				if (!user_gave_labels 
                                    && gr_currentfontsize_pt() > SMALLFONTSIZE
                                    && -0.01 < tmp / yinc
                                    && tmp / yinc< 0.01
				    && strlen(_grNumFormat_y)) {
					// Draw "1" as a special case
					if (0.99 < next && next < 1.01)
						sprintf(_grTempString, "1");
					else
						sprintf(_grTempString, "$10^{%.0f}$", log10(next));
					slabel.assign(_grTempString);
					fix_negative_zero(slabel);
					label.fromSTR(slabel.c_str());
					if (side == gr_axis_LEFT)
						label.draw(xcm2 + offset, ycm2 - 0.5 * CapHeight, TEXT_RJUST, 0.0);
					else
						label.draw(xcm2 + offset, ycm2 - 0.5 * CapHeight, TEXT_LJUST, 0.0);
					// Keep track of maximum width of axis numbers, so that
					// axis label can be offset right amount.
					gr_stringwidth(_grTempString, &tmp0, &tmp1, &tmp2);
					if (tmp0 > max_num_width_cm)
						max_num_width_cm = tmp0;
				}
			} else {
				// small tic
				double xuser, yuser;
				gr_cmtouser(xcm2 + tic_sml, ycm2, &xuser, &yuser);
				axis_path.push_back(xuser, yuser, 'l');
			}
			axis_path.push_back(x, next, 'l');
			present = next;
		}
		if (user_gave_labels) {
			angle = 0;
			for (unsigned int i = 0; i < _y_labels.size(); i++) {
				if (BETWEEN(yb, yt, _y_label_positions[i])) {
					label.fromSTR(_y_labels[i].c_str()); // BUG: should interpolate into this string
					gr_usertocm(x, _y_label_positions[i], &xcm, &ycm);
					labelx_cm = xcm + offset * cos(angle);
					labely_cm = ycm + offset * sin(angle) - 0.5 * CapHeight;

#ifdef DEBUG_LABELS
					printf("DEBUG: %s:%d drawing %d-th label '%s' at y=%f angle=%f\n",__FILE__,__LINE__,i,_y_labels[i].c_str(),_y_label_positions[i],angle);
#endif
					if (side == gr_axis_LEFT)
						label.draw(labelx_cm, labely_cm, TEXT_RJUST, DEG_PER_RAD * angle);
					else
						label.draw(labelx_cm, labely_cm, TEXT_LJUST, DEG_PER_RAD * angle);
				} else {
#ifdef DEBUG_LABELS
					//printf("DEBUG: %s:%d SKIPPING %d-th label '%s' since x=%f\n",__FILE__,__LINE__,i,_y_labels[i].c_str(),_y_label_positions[i]);
#endif
				}
			}
		}
		// Finish by drawing to end of axis (in case there was no tic there).
		axis_path.push_back(x, final, 'l');
		axis_path.stroke(units_user, _griState.linewidth_axis());
		break;
	default:
		gr_Error("unknown axis type (internal error)");
	}
	// write label, rotated if necessary
	if (gr_currentfontsize_pt() > SMALLFONTSIZE) {
		// Start to calculate what x to put label at; this makes xcm be on
		// axis, so will have to shift depending on orientation of label.
		// Note: will now re-use 'angle' to mean angle of y axis

		if (_grTransform_y == gr_axis_LOG) {
			double          x_cm, xx_cm, y_cm, yy_cm;
			gr_usertocm(x, sqrt(yb * yt), &x_cm, &y_cm);
			gr_usertocm(x, 0.001 + sqrt(yb * yt), &xx_cm, &yy_cm);
			angle = fabs(DEG_PER_RAD * atan2(yy_cm - y_cm, xx_cm - x_cm)); // abs() ensures from bottom to top
		} else {
			double          x_cm, xx_cm, y_cm, yy_cm;
			gr_usertocm(x, 0.5 * (yb + yt), &x_cm, &y_cm);
			gr_usertocm(x, 0.01 * yinc + 0.5 * (yb + yt), &xx_cm, &yy_cm);
			angle = DEG_PER_RAD * atan2(yy_cm - y_cm, xx_cm - x_cm);
		}
		xcm = 0.5 * (gr_usertocm_x(x, yb) + gr_usertocm_x(x, yt));
		ycm = 0.5 * (gr_usertocm_y(x, yb) + gr_usertocm_y(x, yt));
		// Need  at least max_num_width_cm, i.e., widest numeric label, plus
		// a little space (check against above).
		max_num_width_cm += FACTOR * CapHeight;
		// Need space for tics too
		max_num_width_cm += (_grTicsPointIn == true ? 0.0 : _grTicSize_cm);
		// Do by cases -- inelegant but flexible to change
		label.fromSTR(_grYAxisLabel);
		switch (_grAxisStyle_y) {
		default:
		case 0:		// label parallel to axis
			if (side == gr_axis_LEFT) {
				label.draw(xcm - max_num_width_cm,
					   ycm,
					   TEXT_CENTERED, angle);
			} else {
				label.draw(xcm + max_num_width_cm,
					   ycm,
					   TEXT_CENTERED, angle - 180);
			}
			break;
		case 1:			// horizontal label
			if (side == gr_axis_LEFT) {
				label.draw(xcm - max_num_width_cm,
					   ycm - 0.5 * CapHeight,
					   TEXT_RJUST, 90.0 - angle);
			} else {
				label.draw(xcm + max_num_width_cm,
					   ycm - 0.5 * CapHeight,
					   TEXT_LJUST, 90.0 - angle);
			}
			break;
		}
	}
	_griState.set_line_cap(old_linecap);
	_griState.set_line_join(old_linejoin);
}

#if 0
/* UNUSED -- Delete later BUG ?? */
#define NUM_LABEL 100
/*
 * create_labels - make clean labels for linear axes (clean by making same
 * number of decimals)
 */
static int
create_labels(double y, double yb, double yinc, double yt, double smallinc,
	      char *label[NUM_LABEL],
	      int *num_label)
{
	int             i, max_decimals = 0;
	double          zero = fabs(yt - yb) / 1.0e5;	/* store a number
							 * effectively 0 */
	*num_label = 0;
	do {
		if (gr_multiple(y, yinc, 0.01 * smallinc)) {
			if (fabs(y) < zero)
				sprintf(_grTempString, _grNumFormat_y, 0.0);
			else
				sprintf(_grTempString, _grNumFormat_y, y);
			/* reserve 10 extra characters for 0s which might be appended */
			GET_STORAGE(label[*num_label],
				    char,
				    10 + strlen(_grTempString));
			strcpy(label[*num_label], _grTempString);
			(*num_label)++;
		}
		y += smallinc;
	} while ((*num_label < NUM_LABEL)
		 && ((yinc > 0.0 && y <= yt)
		     || (yinc < 0.0 && y >= yt)));
	if (*num_label >= NUM_LABEL) {
		fprintf(stderr, "Internal error (graxes.c): %d > NUM_LABEL labels\n",
			*num_label);
		gri_exit(1);
	}
	for (i = 0; i < *num_label; i++) {
		int             j;
		if (max_decimals < (j = num_decimals(label[i])))
			max_decimals = j;
	}
	for (i = 0; i < *num_label; i++) {
		int             n = num_decimals(label[i]);
		int             l = strlen(label[i]);
		if (!n)			/* don't meddle if e/E/d/D present */
			continue;
		if (max_decimals > 0) {
			if (n < max_decimals) {
				int             j;
				if (n == 0) {
					*(label[i] + l++) = '.';
					n = 1;
				}
				for (j = max_decimals - n; j < max_decimals; j++)
					*(label[i] + l++) = '0';
			}
			*(label[i] + l) = '\0';
		}
	}
	return 1;
}
#endif

#if 0
/*
 * num_decimals(s) -- return number of places to right of decimal, but return
 * 0 if there is an e/E/d/D in the number (in which case don't meddle
 */
static int
num_decimals(char *s)
{
	char *            cp;
	int             j = 0, jMax = strlen(s);
	/*
	 * First search for e/E/d/D
	 */
	cp = s;
	do
		if (*cp == 'd' || *cp == 'D' || *cp == 'e' || *cp == 'E')
			return 0;
	while (*++cp != '\0');
	/*
	 * It doesn't have e/E/d/D
	 */
	cp = s + strlen(s) - 1;
	do
		if (*cp == '.')
			break;
	while (cp--, ++j < jMax);
	if (j >= jMax)
		j = 0;
	return j;
}
#endif
