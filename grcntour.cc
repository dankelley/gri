#define DEBUG_CONTOUR
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "gr.hh"
#include "private.hh"
#include "errors.hh"
#include "GMatrix.hh"
#include "GriPath.hh"

// globals to this file
static double   Contour_space_first;
static double   Contour_space_later;
static double   Contour_minlength;
static bool     Center_labels;

static bool     Whiteunder_labels = true; // always set anyway
static bool     Label_contours = true;	 // always set anyway
static bool     Rotate_labels = false;	 // always set anyway
static int      nx_1, ny_1, iGT, jGT, iLE, jLE;

static void     free_space_for_curve();
static void     get_space_for_curve();
static void     draw_the_contour(FILE * out_file,
				 const GriColor &line_color,
				 const GriColor &label_color,
				 /*const*/ GriString &label);

static bool     trace_contour(double z0,
			      const double x[],
			      const double y[],
                              /*const*/ GriMatrix<double> &z,
			      /*const*/ GriMatrix<bool> &legit,
			      FILE *out_file,
			      const GriColor &line_color,
			      const GriColor &label_color,
			      /*const*/ GriString &label);
static int      FLAG(int ni, int nj, int ind);
static int      append_segment(double xr, double yr, double zr, bool OKr,
			       double xs, double ys, double zs, bool OKs,
			       double z0);

// Space for curve, shared by several routines
static double  *xcurve, *ycurve;
static bool *legitcurve;
#define INITIAL_CURVE_SIZE 100
static int      num_in_curve, max_in_curve;
static bool     curve_storage_exists = false;


void
free_space_for_curve()
{
	if (curve_storage_exists) {
		delete [] xcurve;
		delete [] ycurve;
		delete [] legitcurve;
		curve_storage_exists = false;
	}
	num_in_curve = 0;
}

void
get_space_for_curve()
{
	max_in_curve = INITIAL_CURVE_SIZE;
	if(curve_storage_exists) {
		gr_Error("storage is messed up (internal error)");
		return;			// will not execute
	}
	xcurve = new double [max_in_curve];
	ycurve = new double [max_in_curve];
	legitcurve = new bool [max_in_curve];
	curve_storage_exists = true;
	num_in_curve = 0;
}



//  gr_contour() -- draw contour line for gridded data
//  
//  DESCRIPTION: Draws a contour for the value z0, through data z[i][j] defined
//  on the rectangular grid x[i] and y[j] (where 0<=i<nx and 0<=j<ny).  That
//  the grid is rectangular but needn't be square or regular. Contours are
//  drawn only in triangular regions surrounded by 3 good points (ie, 3 points
//  with legit[i][j]==true).
//  
//  The contour is labelled, with the string// lab, at intervals of
//  contour_space_later centimeters, starting with a space of
//  contour_space_first from the beginning of the trace.
//  
//  If 'out_file' is not NULL, then do not actually draw anything; instead, write
//  the contour to that file, in lines with (x,y) in user coordinates, (ended
//  by blank line), following a header containing 3 numbers:
//  
//  CONTOUR_VALUE MISSING_VALUE
void
gr_contour(const double x[],
	   const double y[],
	   /*const*/ GriMatrix<double> &z,
	   /*const*/ GriMatrix<bool> &legit,
	   int nx,
	   int ny, 
	   double z0,
	   const char *lab,
	   bool rotate_labels,
	   bool whiteunder_labels,
	   bool center_labels,
	   const GriColor &line_color,
	   const GriColor &label_color,
	   double contour_minlength,
	   double contour_space_first, 
	   double contour_space_later,
	   FILE *out_file)
{
	register int    i, j;
	// Test for errors
	if (nx <= 0) gr_Error("nx<=0 (internal error)");
	if (ny <= 0) gr_Error("ny<=0 (internal error)");
	// Header for output file
	if (out_file != NULL) 
		fprintf(out_file, "%f %g\n", z0, gr_currentmissingvalue());
	// Save some globals
	nx_1 = nx - 1;
	ny_1 = ny - 1;
	Contour_space_first = contour_space_first;
	Contour_space_later = contour_space_later;
	Contour_minlength = contour_minlength;
	Rotate_labels = rotate_labels;
	Whiteunder_labels = whiteunder_labels;
	GriString label;
	if (lab)
		label.fromSTR(lab);
	else
		label.fromSTR("");
	// Save some flags
	Center_labels = center_labels;
	Label_contours = (contour_space_later > 0.1) ? true : false;
	// Clear  all switches.
	FLAG(nx, ny, -1);
	// Get space for the curve.
	get_space_for_curve();
    
	// Search for a contour intersecting various places on the grid. Whenever
	// a contour is found to be between two grid points, call trace_contour()
	// after defining the global variables iLE,jLE,iGT,jGT so that
	// z[iLE]jLE] <= z0 < z[iGT][jGT], where legit[iLE][jLE]==true 
	// and legit[iGT][jGT]==true.
	//
	// NOTE: always start a contour running upwards (to greater j), between
	// two sideways neighboring points (same j).  Later, in trace_contour(),
	// test 'locate' for value 5.  If it's 5, it means that the same geometry
	// obtains, so set a flag and check whether already set.  If already
	// set, it means we've traced this contour before, so trace_contour()
	// knows to stop then.

	// Search bottom
#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2"))
		printf("Search bottom...\n");
#endif

	for (i = 1; i < nx; i++) {
		j = 0;
		while (j < ny_1) {
			// move north to first legit point
			while (j < ny_1 
			       && !(legit(i, j) == true && legit(i - 1, j) == true)
				) {
				j++;
			}
			// trace a contour if it hits here
			if (j < ny_1 && z(i, j) > z0 && z(i - 1, j) <= z0) {
				iLE = i - 1;
				jLE = j;
				iGT = i;
				jGT = j;
				trace_contour(z0, x, y, z, legit, out_file, line_color, label_color, label);
			}
			// Space through legit points, that is, skipping through good
			// data looking for another island of bad data which will
			// thus be a new 'bottom edge'.
			while (j < ny_1 && legit(i, j) == true && legit(i - 1, j) == true)
				j++;
		}
	}

#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2"))
		printf("Search right edge ...\n");
#endif
	// search right edge
	for (j = 1; j < ny; j++) {
		i = nx_1;
		while (i > 0) {
			// move west to first legit point
			while (i > 0 && !(legit(i, j) == true && legit(i, j - 1) == true))
				i--;
			// trace a contour if it hits here
			if (i > 0 && z(i, j) > z0 && z(i, j - 1) <= z0) {
				iLE = i;
				jLE = j - 1;
				iGT = i;
				jGT = j;
				trace_contour(z0, x, y, z, legit, out_file, line_color, label_color, label);
			}
			// space through legit points
			while (i > 0 && legit(i, j) == true && legit(i, j - 1) == true)
				i--;
		}
	}

#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2"))
		printf("Search top edge ...\n");
#endif
	// search top edge
	for (i = nx_1 - 1; i > -1; i--) {
		j = ny_1;
		while (j > 0) {
			while (j > 0 && !(legit(i, j) == true && legit(i + 1, j) == true))
				j--;
			// trace a contour if it hits here
			if (j > 0 && z(i, j) > z0 && z(i + 1, j) <= z0) {
				iLE = i + 1;
				jLE = j;
				iGT = i;
				jGT = j;
				trace_contour(z0, x, y, z, legit, out_file, line_color, label_color, label);
			}
			// space through legit points
			while (j > 0 && legit(i, j) == true && legit(i + 1, j) == true)
				j--;
		}
	}

#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2"))
		printf("Search left edge ...\n");
#endif
	// search left edge
	for (j = ny_1 - 1; j > -1; j--) {
		i = 0;
		while (i < nx_1) {
			while (i < nx_1 && !(legit(i, j) == true && legit(i, j + 1) == true))
				i++;
			// trace a contour if it hits here
			if (i < nx_1 && z(i, j) > z0 && z(i, j + 1) <= z0) {
				iLE = i;
				jLE = j + 1;
				iGT = i;
				jGT = j;
				trace_contour(z0, x, y, z, legit, out_file, line_color, label_color, label);
			}
			// space through legit points
			while (i < nx_1 && legit(i, j) == true && legit(i, j + 1) == true)
				i++;
		}
	}

#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2"))
		printf("Search interior ...\n");
#endif
	// Search interior. Pass up from bottom (starting at left), through all
	// interior points. Look for contours which enter, with high to right,
	// between iLE on left and iGT on right.
	for (j = 1; j < ny_1; j++) {
		int             flag_is_set;
		for (i = 1; i < nx; i++) {
			// trace a contour if it hits here
			flag_is_set = FLAG(i, j, 0);
			if (flag_is_set < 0)
				gr_Error("ran out of storage (internal error)");
			if (!flag_is_set
			    && legit(i, j) == true
			    && z(i, j) > z0
			    && legit(i - 1, j) == true
			    && z(i - 1, j) <= z0) {
				iLE = i - 1;
				jLE = j;
				iGT = i;
				jGT = j;
				trace_contour(z0, x, y, z, legit, out_file, line_color, label_color, label);
			}
		}
	}
	// Free up space.
	free_space_for_curve();
	FLAG(nx, ny, 2);
	// Trailer for output file
	if (out_file != NULL)
		fprintf(out_file, "\n");
}

// trace_contour() -- trace_contour a contour line with high values of z to
// it's right.  Stores points in (*xcurve, *ycurve) and the legit flag is
// stored in *legitcurve; initially these must be empty; you must also free
// them after this call, so that the next call will work OK.
static          bool
trace_contour(double z0,
	      const double x[],
	      const double y[],
	      /*const*/ GriMatrix<double> &z,
	      /*const*/ GriMatrix<bool> &legit,
	      FILE *out_file,
	      const GriColor &line_color,
	      const GriColor &label_color,
	      /*const*/ GriString &label)
{
	int             i, ii, j, jj;
	double          zp, vx, vy, zcentre;
	int             locate;
	// locate tells where delta-grid point is.  It codes as follows to
	// i_test[] and j_test[] 6 7 8 3 4 5 0 1 2
	static int      i_test[9] = {
		0, 1, 1,		// 6 7 8
		0, 9, 0,		// 3 4 5
		-1, -1, 0		// 0 1 2
	};
	static int      j_test[9] =
	{
		-1, 0, 0,		// 6 7 8
		-1, 9, 1,		// 3 4 5
		0, 0, 1			// 0 1 2
	};
	static int      dtest[9] =
	{
		0, 1, 0,		// 6 7 8
		1, 0, 1,		// 3 4 5
		0, 1, 0			// 0 1 2
	};

#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2")) {
		printf("trace_contour()  iLE=%d jLE=%d iGT=%d jGT=%d\n",iLE,jLE,iGT,jGT);
		printf("\tLE (%.2f  %.2f  %.2f) %d\n", x[iLE],y[jLE],z(iLE,jLE),legit(iLE,jLE));
		printf("\tGT (%.2f  %.2f  %.2f) %d\n", x[iGT],y[jGT],z(iGT,jGT),legit(iGT,jGT));
	}
#endif

	// Trace the curve, storing results with append_segment() into *xcurve,
	// *ycurve, *legitcurve.  When done, call draw_the_contour(), which draws
	// the contour stored in these arrays and draws labels.
	while (true) {
	
		append_segment(x[iLE], y[jLE], z(iLE, jLE), legit(iLE, jLE),
			       x[iGT], y[jGT], z(iGT, jGT), legit(iGT, jGT),
			       z0);
		// Find the next point to check through a table lookup.
		locate = 3 * (jGT - jLE) + (iGT - iLE) + 4;
		i = iLE + i_test[locate];
		j = jLE + j_test[locate];

#ifdef DEBUG_CONTOUR
		if (get_flag("kelley2"))
			printf("iLE=%d jLE=%d   iGT=%d jGT=%d  locate=%d -> i=%d j=%d\n",iLE,jLE,iGT,jGT,locate,i,j); 
#endif

	
		// Did it hit an edge?
		if (i > nx_1 || i < 0 || j > ny_1 || j < 0) {
			draw_the_contour(out_file, line_color, label_color, label);
#ifdef DEBUG_CONTOUR
			if (get_flag("kelley2"))
				printf("Hit edge\n");
#endif
			return true;		// all done
		}

		// Test if retracing an existing contour.  See explanation
		// above, in grcntour(), just before search starts. 
		if (locate == 5) {
#ifdef DEBUG_CONTOUR
			if (get_flag("kelley2"))
				printf("locate==5 ... ");
#endif
			int             already_set = FLAG(iGT, jGT, 1);
			if (already_set < 0) {
				gr_Error("ran out of storage (internal error)");
				return false;
			}
			if (already_set) {
#ifdef DEBUG_CONTOUR
				if (get_flag("kelley2"))
					printf("so draw it\n");
#endif
				draw_the_contour(out_file, line_color, label_color, label);
				return true;	// all done
			}
#ifdef DEBUG_CONTOUR
			if (get_flag("kelley2"))
				printf("didn't draw it\n");
#endif
		}

#ifdef DEBUG_CONTOUR
		if (get_flag("kelley2")) {
			printf("\tnew i=%d j=%d\n", i, j);
			printf("\t     (legit there is %d)\n", legit(i,j));
		}
#endif
		// Following new for 2.1.13
		if (!legit(i,j)) {
#ifdef DEBUG_CONTOUR
			if (get_flag("kelley2"))
				printf("not legit at i=%d j=%d, so drawing\n",i,j);
#endif
			draw_the_contour(out_file, line_color, label_color, label);
			return true;		// all done
		} 

		if (!dtest[locate]) {
			zp = z(i, j);
			if (zp > z0)
				iGT = i, jGT = j;
			else
				iLE = i, jLE = j;
			continue;
		}
		vx = (x[iGT] + x[i]) * 0.5;
		vy = (y[jGT] + y[j]) * 0.5;
		locate = 3 * (jGT - j) + iGT - i + 4;
		// Fourth point in rectangular boundary
		ii = i + i_test[locate];
		jj = j + j_test[locate];
		bool legit_diag = 
			(legit(iLE, jLE) == true
			 && legit(iGT, jGT) == true 
			 && legit(i, j) == true
			 && legit(ii, jj) == true) ? true : false;
		zcentre = 0.25 * (z(iLE, jLE) + z(iGT, jGT) + z(i, j) + z(ii, jj));
#ifdef DEBUG_CONTOUR
		if (get_flag("kelley2")) {
			printf("ii=%d jj=%d.  legit_diag=%d  (zcenter=%f)\n",ii,jj,legit_diag,zcentre);
			if (!legit_diag) printf("-- will [for kelley_contour1] consider this an 'edge', probably\n");
		}
#endif
		if (get_flag("kelley1") && !legit_diag) {
			printf("\n*** flag_kelley1: hit missing-value region, so stopping contour trace\n");
			draw_the_contour(out_file, line_color, label_color, label);
			return true;		// all done
		}

		if (zcentre <= z0) {
			append_segment(x[iGT], y[jGT], z(iGT, jGT), legit(iGT, jGT),
				       vx, vy, zcentre, legit_diag,
				       z0);
			if (z(ii, jj) <= z0) {
				iLE = ii, jLE = jj;
				continue;
			}
			append_segment(x[ii], y[jj], z(ii, jj), legit(ii, jj),
				       vx, vy, zcentre, legit_diag,
				       z0);
			if (z(i, j) <= z0) {
				iGT = ii, jGT = jj;
				iLE = i, jLE = j;
				continue;
			}
			append_segment(x[i], y[j], z(i, j), legit(i, j),
				       vx, vy, zcentre, legit_diag,
				       z0);
			iGT = i, jGT = j;
			continue;
		}
		append_segment(vx, vy, zcentre, legit_diag,
			       x[iLE], y[jLE], z(iLE, jLE), legit(iLE, jLE),
			       z0);
		if (z(i, j) > z0) {
			iGT = i, jGT = j;
			continue;
		}
		append_segment(vx, vy, zcentre, legit_diag,
			       x[i], y[j], z(i, j), legit(i, j),
			       z0);
		if (z(ii, jj) <= z0) {
			append_segment(vx, vy, zcentre, legit_diag,
				       x[ii], y[jj], z(ii, jj), legit(ii, jj),
				       z0);
			iLE = ii;
			jLE = jj;
			continue;
		}
		iLE = i;
		jLE = j;
		iGT = ii;
		jGT = jj;
	}
}

// append_segment() -- append a line segment on the contour
static double   xplot_last, yplot_last;
static int
append_segment(double xr, double yr, double zr, bool OKr,
	       double xs, double ys, double zs, bool OKs,
	       double z0)
{
	if (zr == zs) gr_Error("Contouring problem: zr = zs, which is illegal");
	double frac = (zr - z0) / (zr - zs);
	if (frac < 0.0) gr_Error("Contouring problem: frac < 0");
	if (frac > 1.0) gr_Error("Contouring problem: frac > 1");
	double xplot = xr - frac * (xr - xs);
	double yplot = yr - frac * (yr - ys);
	// Avoid replot, which I suppose must be possible, given this code
	if (num_in_curve > 0 && xplot == xplot_last && yplot == yplot_last)
		return 1;
	if (num_in_curve > max_in_curve - 1) {
		// Get new storage if running on empty.  Better to
		// do this with an STL vector class
		max_in_curve *= 2;
		int i;
		double *tmp = new double [num_in_curve]; if (!tmp) OUT_OF_MEMORY;
		for (i = 0; i < num_in_curve; i++) tmp[i] = xcurve[i];
		delete [] xcurve; xcurve = new double [max_in_curve]; if (!xcurve) OUT_OF_MEMORY;
		for (i = 0; i < num_in_curve; i++) xcurve[i] = tmp[i];
		for (i = 0; i < num_in_curve; i++) tmp[i] = ycurve[i];
		delete [] ycurve; ycurve = new double [max_in_curve]; if (!ycurve) OUT_OF_MEMORY;
		for (i = 0; i < num_in_curve; i++) ycurve[i] = tmp[i];
		delete [] tmp;
		bool *tmpl = new bool [num_in_curve]; if (!tmpl) OUT_OF_MEMORY;
		for (i = 0; i < num_in_curve; i++)	tmpl[i] = legitcurve[i];
		delete [] legitcurve; legitcurve = new bool [max_in_curve]; if (!legitcurve) OUT_OF_MEMORY;
		for (i = 0; i < num_in_curve; i++)	legitcurve[i] = tmpl[i];
		delete [] tmpl;
	}
	// A segment is appended only if both the present point and the last
	// point came by interpolating between OK points.
	xcurve[num_in_curve] = xplot;
	ycurve[num_in_curve] = yplot;
	if (!inside_box(xcurve[num_in_curve], ycurve[num_in_curve])) {
		legitcurve[num_in_curve] = false;
	} else {
		if (OKr == true && OKs == true)
			legitcurve[num_in_curve] = true;
		else
			legitcurve[num_in_curve] = false;
	}
	num_in_curve++;
	xplot_last = xplot;
	yplot_last = yplot;
#ifdef DEBUG_CONTOUR
	if (get_flag("kelley2")) {
		printf("\nin append_segment... curve is now\n");
		for (int i = 0; i < num_in_curve; i++)
			printf("\t%.2f %.2f legit=%d\n", xcurve[i], ycurve[i], legitcurve[i]);
	}
#endif
	return 1;
}

// Draw contour stored in (xcurve[],ycurve[],legitcurve[]), possibly with
// labels (depending on global Label_contours).
//
// Trick: if out_file is not NULL, then don't actually draw the contour; rather,
// print out the pairs of numbers (ended by blank line), after a header line
// containing three numbers:
// 
// CONTOUR_VALUE MISSING_VALUE
#define FACTOR 3.0 // contour must be FACTOR*len long to be labelled
static void
draw_the_contour(FILE * out_file,
		 const GriColor &line_color, 
		 const GriColor &label_color,
		 /*const*/ GriString &label)
{
	if (num_in_curve == 1) {
		num_in_curve = 0;
		return;
	}
	register int    i;
	bool            first_label, first_point;
	double          contour_length = 0.0;
	double          xcmlast = 0.0, ycmlast = 0.0; // values over-ridden
	double          x, xcm, y, ycm;
	double          cumdist;
	double          half_height = (gr_currentCapHeight_cm() + gr_thinspace_cm()) / 2.0;
	double          angle = 0.0;
	double          width_cm, ascent_cm, descent_cm;
	// See if being tricked into just printing the contour (x,y) locations.
	if (NULL != out_file) {
		double          miss = gr_currentmissingvalue();
		for (i = 0; i < num_in_curve; i++) {
			if (legitcurve[i] == true) {
				fprintf(out_file, "%f %f\n", xcurve[i], ycurve[i]);
			} else {
				fprintf(out_file, "%g %g\n", miss, miss);
			}
		}
		fprintf(out_file, "%g %g\n", miss, miss); // end off
		num_in_curve = 0;
		return;
	}
	GriPath p(num_in_curve);
	p.push_back(xcurve[0], ycurve[0], 'm');
	for (i = 1; i < num_in_curve; i++) {
		p.push_back(xcurve[i], ycurve[i], legitcurve[i] ? 'l' : 'm');
	}
	p.trim();
	p.stroke(units_user);
	// Figure total length of contour line
	first_point = true;
	for (i = 0; i < num_in_curve; i++) {
		if (legitcurve[i] == false)
			continue;
		gr_usertocm(xcurve[i], ycurve[i], &xcm, &ycm);
		if (first_point) {
			xcmlast = xcm;
			ycmlast = ycm;
			first_point = false;
		}
		contour_length += sqrt((xcm - xcmlast) * (xcm - xcmlast)
				       + (ycm - ycmlast) * (ycm - ycmlast));
		xcmlast = xcm;
		ycmlast = ycm;
	}
	// Return right away, if contour too short
	if (contour_length < 0.0) {
		num_in_curve = 0;
		return;
	}
	gr_stringwidth(label.getValue(), &width_cm, &ascent_cm, &descent_cm);
	if (contour_length < FACTOR * width_cm || contour_length < Contour_minlength) {
		num_in_curve = 0;
		return;
	}
	if (Label_contours) {
		if (Center_labels) {
			// Centered labels
			double          half_length = contour_length / 2.0;
			cumdist = 0.0;
			first_point = true;
			for (i = 0; i < num_in_curve; i++) {
				if (legitcurve[i] == false)
					continue;
				gr_usertocm(xcurve[i], ycurve[i], &xcm, &ycm);
				if (first_point) {
					xcmlast = xcm;
					ycmlast = ycm;
					first_point = false;
				}
				cumdist += sqrt((xcm - xcmlast) * (xcm - xcmlast)
						+ (ycm - ycmlast) * (ycm - ycmlast));
				if (cumdist > half_length) {
					if (Rotate_labels) {
						// Rotate the angle, making sure that text reads from
						// left-to-right.
						angle = DEG_PER_RAD * atan2(ycm - ycmlast, xcm - xcmlast);
						if (angle > 90.0 || angle < -90.0)
							angle += 180.0;
					}
					gr_cmtouser(xcm, ycm - half_height, &x, &y);
					if (Whiteunder_labels) {
						GriColor        white;
						white.setRGB(1.0, 1.0, 1.0);
						gr_show_in_box(label, label_color, white, xcm, ycm - half_height, angle);
					} else {
						double          xxcm, yycm;
						gr_usertocm(x, y, &xxcm, &yycm);
						label.draw(xxcm, yycm, TEXT_CENTERED, angle);
					}
					break;	// done now
				}
				xcmlast = xcm;
				ycmlast = ycm;
			}
		} else {
			// Not centered.
			cumdist = 0.0;
			first_point = true;
			first_label = true;
			for (i = 0; i < num_in_curve; i++) {
				if (legitcurve[i] == false)
					continue;
				gr_usertocm(xcurve[i], ycurve[i], &xcm, &ycm);
				if (first_point) {
					xcmlast = xcm;
					ycmlast = ycm;
					first_point = false;
				}
				cumdist += sqrt((xcm - xcmlast) * (xcm - xcmlast)
						+ (ycm - ycmlast) * (ycm - ycmlast));
				if ((first_label && (cumdist > Contour_space_first))
				    || (!first_label && (cumdist > Contour_space_later))) {
					if (Rotate_labels) {
						// Rotate the angle, making sure that text reads from
						// left-to-right.
						angle = DEG_PER_RAD * atan2(ycm - ycmlast, xcm - xcmlast);
						if (angle > 90.0 || angle < -90.0)
							angle += 180.0;
					}
					gr_cmtouser(xcm, ycm - half_height, &x, &y);
					if (Whiteunder_labels) {
						GriColor        white;
						white.setRGB(1.0, 1.0, 1.0);
						gr_show_in_box(label, label_color, white, xcm, ycm - half_height, angle);
					} else {
						double          xxcm, yycm;
						gr_usertocm(x, y, &xxcm, &yycm);
						label.draw(xxcm, yycm, TEXT_CENTERED, angle);
					}
					cumdist = 0.0;
					first_label = false;
				}
				xcmlast = xcm;
				ycmlast = ycm;
			}
		}
	}
	// Invalidate the data, so that next curve will use same storage.
	num_in_curve = 0;
}

// FLAG() -- check flag for gr_contour() and trace_contour()
// ni = row (or, if ind==-1, number of rows)
// nj = col (or, if ind==-1, number of cols)
// if (ind == -1), get flag storage space; initialize flags to 0
// if (ind == 1), check flag and then set it
// if (ind == 2), clear the flag storage space
// if (ind == 0), check flag, return value
// RETURN VALUE: Normally, the flag value (0 or 1).  If the storage is
// exhausted, return a number <0.
#define	NBITS		32
static int
FLAG(int ni, int nj, int ind)
{
	static bool     flag_storage_exists = false;
	static unsigned long *flag, mask[NBITS];
	static int      size;
	static int      ni_max;	// x-dimension is saved
	int             i, ipos, iword, ibit, return_value;
	switch (ind) {
	case -1:
		// Allocate storage for flag array
		if (flag_storage_exists)
			gr_Error("storage is messed up (internal error)");
		size = 1 + ni * nj / NBITS;	// total storage array length
		flag = new unsigned long [size];
		if (!flag) OUT_OF_MEMORY;
		// Create mask
		mask[0] = 1;
		for (i = 1; i < NBITS; i++)
			mask[i] = 2 * mask[i - 1];
		for (i = 0; i < size; i++)	// Zero out flag
			flag[i] = 0;
		ni_max = ni;		// Save for later
		flag_storage_exists = true;
		return 0;
	case 2:
		if (!flag_storage_exists)
			gr_Error("No flag storage exists");
		delete [] flag;
		flag_storage_exists = false;
		return 0;
	default:
		if (!flag_storage_exists)
			gr_Error("No flag storage exists");
		break;
	}
	// ind was not -1 or 2
	// Find location of bit.
	ipos = nj * ni_max + ni;
	iword = ipos / NBITS;
	ibit = ipos - iword * NBITS;
	// Check for something being broken here, causing to run out of space.
	// This should never happen, but may as well check.
	if (iword >= size)
		return (-99);		// no space
	// Get flag.
	return_value = (0 != (*(flag + iword) & mask[ibit]));
	// If ind=1 and flag wasn't set, set the flag
	if (ind == 1 && !return_value)
		flag[iword] |= mask[ibit];
	// Return the flag value
	return return_value;
}
#undef NBITS

