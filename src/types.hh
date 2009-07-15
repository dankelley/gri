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

// Define some useful types
#ifndef _gri_types_hh_
#include <stdio.h>
#include <math.h>
#define         _gri_types_hh_

enum gr_textStyle {		// Text writing options
	TEXT_LJUST,
	TEXT_RJUST,
	TEXT_CENTERED
};

enum units {			// Units for GriPath
	units_cm,
	units_pt,
	units_user
};


static inline double min4(double a, double b, double c, double d) {
	double rval = a;
	if (b < a) rval = b;
	if (c < a) rval = c;
	if (d < a) rval = d;
	return rval;
}
static inline double max4(double a, double b, double c, double d) {
	double rval = a;
	if (b > a) rval = b;
	if (c > a) rval = c;
	if (d > a) rval = d;
	return rval;
}

class rectangle { // a rectangle oriented parallel to axis
public:
	rectangle(double llx, double lly, double urx, double ury) {
		ll_x = llx;
		ll_y = lly;
		ur_x = urx;
		ur_y = ury;
	}
	rectangle(void) { ll_x = ll_y = ur_x = ur_y = 0.0; }
	~rectangle() {}
	void set(double llx, double lly, double urx, double ury) {
		ll_x = llx;
		ll_y = lly;
		ur_x = urx;
		ur_y = ury;
	}
	void scale(double f) { ll_x *= f; ll_y *= f; ur_x *= f; ur_y *= f;}
	double llx(void) const { return ll_x; }
	double lly(void) const { return ll_y; }
	double urx(void) const { return ur_x; }
	double ury(void) const { return ur_y; }
	void set_llx(double llx) { ll_x = llx;} 
	void set_lly(double lly) { ll_y = lly;} 
	void set_urx(double urx) { ur_x = urx;} 
	void set_ury(double ury) { ur_y = ury;} 
	void shift_x(double dx)  { ll_x += dx; ur_x += dx;}
	void shift_y(double dy)  { ll_y += dy; ur_y += dy;}
	void rotate(double degrees_ccw) {
		double rad_ccw = degrees_ccw / 57.29577951;
		double c = cos(rad_ccw);
		double s = sin(rad_ccw);
		double x1 = c * ll_x - s * ll_y;
		double y1 = s * ll_x + c * ll_y;
		double x2 = c * ur_x - s * ll_y;
		double y2 = s * ur_x + c * ll_y;
		double x3 = c * ur_x - s * ur_y;
		double y3 = s * ur_x + c * ur_y;
		double x4 = c * ll_x - s * ur_y;
		double y4 = s * ll_x + c * ur_y;
		ll_x = min4(x1, x2, x3, x4);
		ur_x = max4(x1, x2, x3, x4);
		ll_y = min4(y1, y2, y3, y4);
		ur_y = max4(y1, y2, y3, y4);
	}
	double get_llx() { return ll_x; }
	double get_lly() { return ll_y; }
	double get_urx() { return ur_x; }
	double get_ury() { return ur_y; }
private:
	double ll_x;		// lower-left
	double ll_y;
	double ur_x;		// upper-right
	double ur_y;
};

#endif
