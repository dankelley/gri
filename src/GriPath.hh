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

// GriPath -- store PostScript path
#if !defined(_GriPath_hh_)
#define _GriPath_hh_
#include "types.hh"
#include "gr_coll.hh"
class GriPath
{
public:
	enum type {moveto, lineto};
	GriPath();
	GriPath(unsigned capacity);
	~GriPath();
	void clear();		// Allows reuse without realloc
	void expand();		// Get more space
	void push_back(double xx, double yy, char aa); // Append at end
	unsigned size();		// Return length of path
	void trim();		// remove extraneous moveto commands
	void stroke(units u, double width = -1, bool closepath = false); // Stroke the path
	void stroke_or_fill(char s_or_f, units u, double width = -1, bool closepath = false); // Stroke or fill
	void fill(units u, bool closepath = false);		// Fill path in
	void print();		// Mostly for debugging
	void translate(double dx, double dy); // Only makes sense if units_cm
	void scale(double enlargement); // Only makes sense if units_cm
	void rotate(double degrees); // Rotate anticlockwise
	double get_x(unsigned offset) {return x[offset];} // bug: no checking
	double get_y(unsigned offset) {return y[offset];} // no checking
	rectangle bounding_box(units u);
private:
	unsigned int depth;		// Length of path
	unsigned int capacity;	// Max elements in path; see expand\(\)
	double *x;			// Data
	double *y;			// Data
	type *action;		// 'm' or 'l'
};
#endif
