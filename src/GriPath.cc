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

// Classes for Gri.  See gr_coll.hh for docs
//#define DEBUG_GR_COLL 1		// uncomment to debug

#include <string>
#include <stdio.h>
#include <string.h>
#include "GriPath.hh"
#include "superus.hh"
#include "gr.hh"
#include "extern.hh"
#include "gr_coll.hh"
#include "defaults.hh"

extern FILE *_grPS;
extern FILE *_grSVG;
static const int CAPACITY_DEFAULT = 32;
double missing_value = -999.0;	// in case not gri
static void ps_begin_path(double width, char s_or_f);
static int straighten_curve(double *x, double *y, GriPath::type *a, unsigned int length, double allow);



GriPath::GriPath()
{
	capacity = CAPACITY_DEFAULT;	
	x = new double[capacity];		if (!x) OUT_OF_MEMORY;
	y = new double[capacity];		if (!y) OUT_OF_MEMORY;		
	action = new GriPath::type[capacity];	if (!action) OUT_OF_MEMORY;
	depth = 0;
}
GriPath::GriPath(unsigned c)
{
	capacity = c;
	x = new double[capacity];		if (!x) OUT_OF_MEMORY;
	y = new double[capacity];		if (!y) OUT_OF_MEMORY;
	action = new GriPath::type[capacity];	if (!action) OUT_OF_MEMORY;
	depth = 0;
}
GriPath::~GriPath()
{
	delete [] x;
	delete [] y;
	delete [] action;
}
void GriPath::clear()
{
	depth = 0;
}
void GriPath::expand()
{
	if (!capacity)
		capacity = CAPACITY_DEFAULT;
	capacity *= 2;
	double *tmp;
	// Enlarge x
	tmp = new double[capacity];
	if (!tmp) OUT_OF_MEMORY;
	unsigned int i;
	for (i = 0; i < depth; i++)
		tmp[i] = x[i];
	delete [] x;
	x = tmp;
	// Enlarge y
	tmp = new double[capacity];
	if (!tmp) OUT_OF_MEMORY;
	for (i = 0; i < depth; i++)
		tmp[i] = y[i];
	delete [] y;
	y = tmp;
	// Enlarge action
	GriPath::type* tmp_c = new GriPath::type[capacity];
	if (!tmp_c) OUT_OF_MEMORY;
	for (i = 0; i < depth; i++)
		tmp_c[i] = action[i];
	delete [] action;
	action = tmp_c;
}
void GriPath::push_back(double xx, double yy, char a)
{
	while (depth >= capacity - 1)
		expand();
	x[depth] 		= xx;
	y[depth] 		= yy;
	switch (a) {
	case 'm':
		action[depth] = moveto;
		break;
	case 'l':
		action[depth] = lineto;
		break;
	default:
		fprintf(stderr, "INTERNAL error at GriPath.cc line %d (a path action is neither 'moveto' nor 'lineto')\n", __LINE__);
		exit(99);
	}
	depth++;
}
unsigned GriPath::size()
{
	return depth;
}

static void
ps_begin_path(double width, char s_or_f='s') // Q: what's with this width=-1 condition??
{
	switch (_output_file_type) {
	case postscript:
	case gif:
		set_ps_color('p');
		if (width != -1) {
			fprintf(_grPS, "1.0 i %d J %d j %.3f w 10.0 M [",
				_griState.line_cap(),
				_griState.line_join(),
				width);
		} else {
			fprintf(_grPS, "1.0 i %d J %d j %.3f w 10.0 M [",
				_griState.line_cap(),
				_griState.line_join(),
				_griState.linewidth_line());
		}
		for (unsigned int i = 0; i < _dash.size(); i++)
			fprintf(_grPS, "%.3f ", _dash[i] * PT_PER_CM);
		fprintf(_grPS, "] %d d\n", int(_dash.size()));
		break;
	case svg:
		fprintf(_grSVG, "<g>\n<path style=\"stroke:%s; stroke-width:%.3f; fill:%s;stroke-opacity:%f;stroke-linejoin:%s;stroke-linecap:%s\"\nd=\"\n",
			_griState.color_line().get_hexcolor().c_str(),
			(width == -1) ? _griState.linewidth_line() : width,
                        (s_or_f == 's') ? "none" : _griState.color_line().get_hexcolor().c_str(),
			1.0 - _griState.color_line().getT(),
			_griState.line_join() == 0 ? "sharp" : (_griState.line_join() == 1 ? "round" : "bevel"),
			_griState.line_cap()  == 0 ? "butt"  : (_griState.line_cap()  == 1 ? "round" : "square"));
		if (_dash.size() > 0)
			fprintf(stderr, "%s:%d:gr_begin_path() is ignoring dash type for SVG output\n", __FILE__,__LINE__);			
		break;
	}
}

void GriPath::stroke(units the_units, double width, bool closepath)
{
	stroke_or_fill('s', the_units, width, closepath);
	bounding_box_update(bounding_box(the_units));
}

void GriPath::fill(units the_units, bool closepath)
{
	stroke_or_fill('f', the_units, -1.0, closepath);
	bounding_box_update(bounding_box(the_units));
}

void GriPath::stroke_or_fill(char s_or_f, units the_units, double width, bool closepath)
{
	double page_height_pt = gr_page_height_pt();
	if (depth < 1)
		return;
	const unsigned int max_length = GR_POINTS_IN_PS_PATH - 1;
	// If no 'lineto' in the path, ignore it completely ...
	unsigned int i;
	for (i = 0; i < depth; i++)
		if (action[i] == GriPath::lineto)
			break;
	if (i == depth)
		return;
//	if (_output_file_type != postscript) {
//		fprintf(stderr, "%s:%d:stroke_or_file() is broken on non-postscript file output\n", __FILE__,__LINE__);
//	}
	// must be some data.  Process island by island
	double        *xc = new double[depth];	if (!xc) OUT_OF_MEMORY;
	double        *yc = new double[depth];	if (!yc) OUT_OF_MEMORY;
	GriPath::type *ac = new GriPath::type[depth];	if (!ac) OUT_OF_MEMORY;
	unsigned int start = 0, stop;
	do {
		// Gobble to first 'm' not at beginning
		xc[0] = x[start];
		yc[0] = y[start];
		ac[0] = action[start];
		stop = depth;
		for (i = start + 1; i < depth; i++) {
			if (action[i] == 'm') {
				stop = i;
				break;
			} else {
				xc[i - start] = x[i];
				yc[i - start] = y[i];
				ac[i - start] = action[i];
			}
		}
		unsigned int length = stop - start;
		// If too many points, chop some.
		double allow = 0.005;	// initial allowed distance off curve, cm
		unsigned int iteration = 0, max_iteration = 20;
		char msg[1000];
		while (0&&iteration < max_iteration && length > max_length) {
			if (iteration == 0) {
				if (s_or_f == 'f')
					sprintf(msg, "`draw curve filled' can't have > %d points in a", max_length);
				else
					sprintf(msg, "`draw curve' can't have > %d points in a", max_length);
				warning(msg);
				sprintf(msg, "\
  curve, owing to a limitation of PostScript.  FYI, the\n\
  curve starts with the coordinate pairs\n\
    (%f, %f), (%f, %f), ...\n\
  and has %d points.",
					x[start], y[start], 
					x[start + 1], y[start + 1],
					length);
				ShowStr(msg);
				ShowStr("  Gri will now remove nearly co-linear\n");
				ShowStr("  points, in an iterative triplet-wise fashion.\n");
			}
			length = straighten_curve(xc, yc, ac, length, allow);
			sprintf(msg, "    Iteration %2d: removed points %.2f mm from curve, shortening to %d.\n", iteration, 10.0 * allow, length);
			ShowStr(msg);
			allow *= 1.414213562;
			iteration++;
		}
		if (iteration >= max_iteration) {
			sprintf(msg, "\
`draw curve filled' didn't get a small enough curve even after %d\n\
  iterations.  Your PostScript interpreter may fail\n", iteration - 1);
			warning(msg);
		}
		if (length > 1) {
			ps_begin_path(width, 'f');
			if (the_units == units_user) { // convert user -> cm
				for (i = 0; i < length; i++) {
					double xcm, ycm;
					gr_usertocm(xc[i], yc[i], &xcm, &ycm);
					xc[i] = xcm;
					yc[i] = ycm;
				}
			} else if (the_units == units_pt) { // convert pt -> cm
				for (i = 0; i < length; i++) {
					xc[i] /= PT_PER_CM;
					yc[i] /= PT_PER_CM;
				}
			}
			ac[0] = GriPath::moveto;
			for (i = 0; i < length; i++) {
				if (_warn_offpage 
				    && ( xc[i] < OFFPAGE_LEFT 
					 || xc[i] > OFFPAGE_RIGHT
					 || yc[i] < OFFPAGE_BOTTOM
					 || yc[i] > OFFPAGE_TOP)) {
					warning("Gri drew a point that is offpage.");
				}
				switch (ac[i]) {
				case GriPath::moveto:		// moveto (skip multiple)
#if 0
					if (i < length - 1 && ac[i + 1] == GriPath::moveto)
						continue;	
#endif
					switch (_output_file_type) {
					case postscript:
						fprintf(_grPS, "%.2f %.2f m\n", xc[i] * PT_PER_CM, yc[i] * PT_PER_CM);
						break;
					case svg:
						fprintf(_grSVG, "M%.2f %.2f\n", xc[i] * PT_PER_CM, page_height_pt - yc[i] * PT_PER_CM);
						break;
					case gif:
						break;
					}
					break;
				case GriPath::lineto:		// lineto (skip identical)
#if 0
					if (i < length - 1 && ac[i + 1] == GriPath::lineto
					    && xc[i] == xc[i + 1] && yc[i] == yc[i + 1])
						continue;	
#endif
					switch (_output_file_type) {
					case postscript:
						fprintf(_grPS, "%.2f %.2f l\n", xc[i] * PT_PER_CM, yc[i] * PT_PER_CM);
						break;
					case svg:
						fprintf(_grSVG, "L%.2f %.2f\n", xc[i] * PT_PER_CM, gr_page_height_pt() - yc[i] * PT_PER_CM);
						break;
					case gif:
						break;
					}
					break;
				}
			}
			switch(_output_file_type) {
			case postscript:
				if (s_or_f == 'f') {
					fprintf(_grPS, "h\n");
					fprintf(_grPS, "F\n");
				} else {
					if (closepath)
						fprintf(_grPS, "h\n");
					fprintf(_grPS, "S\n");
				}
				fprintf(_grPS, "%% END GriPath stroke/fill\n");
				break;
			case svg:
				if (s_or_f == 'f') {
					; //fprintf(stderr, "%s:%d ERROR - cannot fill paths in SVG mode\n", __FILE__,__LINE__);
				} else {
					if (closepath)
						fprintf(stderr, "%s:%d ERROR - cannot close paths in SVG mode\n", __FILE__,__LINE__);
				}
				fprintf(_grSVG, "\"/>\n</g>\n");
				break;
			case gif:
				break;
			}
		}
		start = stop /*+ 1*/;	// point at last, which is 'm'
	} while (stop < depth);
	delete [] xc;
	delete [] yc;
	delete [] ac;
	_drawingstarted = true;
}

// Remove points in curve which lie within 'allow' centimeters
// a line connecting the points before and after.
// BUG: I'm not sure of missing-value behaviour.
int
straighten_curve(double *x, double *y, GriPath::type *a, unsigned int length, double allow)
{
	if (length < 3)
		return length;
	double a1, a2, b1, b2, c1, c2; // in points
	allow *= PT_PER_CM;
	std::vector<bool> remove;
	unsigned int i;
	for (i = 0; i < length; i++)
		remove.push_back(false);
	double A, cos_theta;
	double ab, ac;		// distances a<->b and a<->c
	remove[0] = false;		// Keep endpoints
	remove[length - 1] = false;
	for (i = 1; i < length - 1; i++) {
		if (remove[i - 1]) {	// avoid huge holes
			remove[i] = false;
			continue;
		}
		gr_usertopt(x[i - 1], y[i - 1], &a1, &a2); // last
		gr_usertopt(x[i],     y[i],     &b1, &b2); // this
		gr_usertopt(x[i + 1], y[i + 1], &c1, &c2); // next
		ab = sqrt((a1 - b1) * (a1 - b1) + (a2 - b2) * (a2 - b2));
		if (ab == 0.0) {
			remove[i] = true;
			continue;
		}
		ac = sqrt((a1 - c1) * (a1 - c1) + (a2 - c2) * (a2 - c2));
		if (ac == 0.0) {
			remove[i] = false;
			continue;
		}
		cos_theta = 
			((a1 - b1) * (a1 - c1) + (a2 - b2) * (a2 - c2))
			/ ab / ac;
		if (cos_theta < 1.0)	// roundoff protection
			A = ab * sqrt(1.0 - cos_theta * cos_theta);
		else
			A = 0.0;
		if (A <= allow) {
			remove[i] = true;
		} else {
			remove[i] = false;
		}
	}
	// Very SLOW
	int new_length = length;
	for (i = length - 1; i + 1 > 0; i--) {
		if (remove[i]) {
			new_length--;
			int ir;
			for (ir = i; ir < new_length; ir++) {
				x[ir] = x[ir + 1];
				y[ir] = y[ir + 1];
				a[ir] = a[ir + 1];
			}
		}
	}
	return new_length;
}

void GriPath::print()
{
	printf("Path @ %x is:\n", unsigned(long(this)));
	for (unsigned int i = 0; i < depth; i++)
		printf(" %f %f %c\n", x[i], y[i], action[i]);
}

void GriPath::translate(double dx, double dy)
{
	for (unsigned int i = 0; i < depth; i++) {
		x[i] += dx;
		y[i] += dy;
	}
}
void GriPath::scale(double enlargement)
{
	for (unsigned int i = 0; i < depth; i++) {
		x[i] *= enlargement;
		y[i] *= enlargement;
	}
}
// Rotate anticlockwise by indicated angle
void GriPath::rotate(double degrees)
{
	double c = cos(degrees / DEG_PER_RAD);
	double s = sin(degrees / DEG_PER_RAD);
	double oldx;
	for (unsigned int i = 0; i < depth; i++) {
		oldx = x[i];
		x[i] = c * oldx - s * y[i];
		y[i] = s * oldx + c * y[i];
	}
}

void
GriPath::trim()			// remove junk
{
	double *xx = new double[depth];		if (!xx) OUT_OF_MEMORY;
	double *yy = new double[depth];		if (!yy) OUT_OF_MEMORY;
	GriPath::type *aa = new GriPath::type[depth];	if (!aa) OUT_OF_MEMORY;
	unsigned newlen = 0;
	unsigned int i = 0;
    
	// Trim any junk at start
	while (action[i] == GriPath::moveto)
		i++;
	if (i > 0)                  // keep one 'moveto' though!
		i--;

	// Now trim interior.  Keep first and last of any series though
	for (; i < depth; i++) {
		if ((i > 0 && i < depth - 1)
		    && action[i - 1] == GriPath::moveto
		    && action[i    ] == GriPath::moveto 
		    && action[i + 1] == GriPath::moveto)
			continue;
		xx[newlen] = x[i];
		yy[newlen] = y[i];
		aa[newlen] = action[i];
		newlen++;
	}
    
	// Finally, trim from end
	while(newlen && aa[newlen - 1] == GriPath::moveto)
		newlen--;

	//printf("trim started with :\n"); for (i = 0; i < depth; i++) printf("%.2f %.2f %s\n",x[i],y[i],action[i]==moveto?"moveto":"lineto");

	for (i = 0; i < newlen; i++) {
		x[i] = xx[i];
		y[i] = yy[i];
		action[i] = aa[i];
	}
	depth = newlen;

	//printf("trim created:\n"); for (i = 0; i < depth; i++) printf("%.2f %.2f %s\n",x[i],y[i],action[i]==moveto?"moveto":"lineto");
	delete [] xx;
	delete [] yy;
	delete [] aa;

}

// Return bounding box for this path (in cm on page).
// Assume already did trim()
rectangle
GriPath::bounding_box(units u)
{
	rectangle *r = new rectangle;
	bool first = true;
	for (unsigned int i = 0; i < depth; i++) {
		if (gr_missingx(x[i]) || gr_missingy(y[i]))
			continue;
		double xx, yy;
		xy_to_cm(x[i], y[i], u, &xx, &yy);
		if (first) {
			r->set(xx, yy, xx, yy);
			first = false;
		} else {
			if (xx < r->llx())
				r->set_llx(xx);
			if (r->urx() < xx)
				r->set_urx(xx);
			if (yy < r->lly())
				r->set_lly(yy);
			if (r->ury() < yy )
				r->set_ury(yy);
		}
	}
	if (_clipping_postscript 
	    && _clip_ps_xleft != _clip_ps_xright
	    && _clip_ps_ybottom != _clip_ps_ytop) {
		//printf("BEFORE BBOX ll=(%f %f)  ur=(%f %f)\n",r->llx(), r->lly(), r->urx(), r->ury());
		if (r->llx() < (_clip_ps_xleft / PT_PER_CM))
			r->set_llx(_clip_ps_xleft / PT_PER_CM);
		if (r->urx() > (_clip_ps_xright / PT_PER_CM))
			r->set_urx(_clip_ps_xright / PT_PER_CM);
		if (r->lly() < (_clip_ps_ybottom / PT_PER_CM))
			r->set_lly(_clip_ps_ybottom / PT_PER_CM);
		if (r->ury() > (_clip_ps_ytop / PT_PER_CM))
			r->set_ury(_clip_ps_ytop / PT_PER_CM);
	}
	//printf("test BBOX ll=(%f %f)  ur=(%f %f)\n",r->llx(), r->lly(), r->urx(), r->ury());
	return *r;
}
