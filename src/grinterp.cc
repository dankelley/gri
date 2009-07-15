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
#include        "private.hh"
#include        "gr.hh"
extern double   _grMissingValue;/* defined in gr.c */
int number_good_xyz(const std::vector<double>& x,
		    const std::vector<double>& y,
		    const std::vector<double>& f,
		    int n);
#define root_of_2  1.414213562

int
number_good_xyz(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& f, int n)
{
	int good = 0;
	for (int i = 0; i < n; i++)
		if (!gr_missingx(x[i]) && !gr_missingy(y[i]) && !gr_missing(f[i]))
			good++;
	return good;
}

/*
 * gr_grid1: objective analysis of 2d field
 * 
 * DESCRIPTION: Given f[i] defined at points (x[i],y[i]), where 0<=i<n, this
 * routine uses objective analysis to get a predicted value fOut. The points
 * f[i] used to calculate fOut come from a specified search rectangle xr wide
 * by yr high.
 *
 * Also does boxcar gridding if 'method' is 0.
 * 
 * Third method exists to .... better clean this doc up!
 *
 * If there are `neighbors' or more points in a rectangular neighborhood 2*xr
 * wide and 2*yr high, then only the data inside that region are used, with x
 * and y distances weighted by xr and yr. If there are fewer than `neighbors'
 * points in the neighborhood, then the search rectangle is enlarged in area
 * [xr and yr are multiplied by sqrt(2)] and the process starts over. This
 * enlargement may be repeated up to 'enlargements' times, until either
 * `neighbors' data are located in the search region, or all the data are
 * used.  If `enlargements' is set to a negative value, the enlargement
 * process will be continued until all the data are used, if necessary.
 * 
 * When all relevant points are located, the output is calculated using a
 * weighting formula for the data within the rectangle:
 *
 * fOut = sum(f[i]*w[i])/sum(w[i]
 *
 * Various weightings 'w' are provided, each as a function of
 * the scaled distance 'r = sqrt([(x1-x2)/xr]^2 + [(y1-y2)/yr]^2)'.
 *
 * method = 0: w = 1 (boxcar smoothing)
 *
 * method = 1: w = (2 - r^2) / (2 + r^2) (Levy + Brown method "1")
 *
 * method = 2: w = [(2 - r^2) / (2 + r^2)]^2 (Levy + Brown method "2")
 *
 * method = 3: w = [(2 - r^2) / (2 + r^2)]^4 (Levy + Brown method "3")
 *
 * REFERENCE: Levy & Brown [1986 Journal of
 * Geophysical Research 91C4: 5153-5158].  Note that the present method 
 * extends for unequal weightings in the (x,y) direction, so that nonisotropic
 * data can be handled.
 * 
 * RETURN VALUE: The number of points finally used in the sum, or 0 if there was
 * an error (if n<=0).
 */
int
gr_grid1(const std::vector<double> &x,
	 const std::vector<double> &y,
	 const std::vector<double> &f,
	 double x0, double y0,
	 double xr, double yr,
	 int method,
	 unsigned int neighbors,
	 int enlargements,
	 double *fOut)
{
	unsigned int n = x.size();
	double          dx, dy;
	int             enlarge = 0;
	unsigned int    i;
	unsigned int    nn, num_in_rect;
	double          d2;	/* squared distance */
	double          sumw;	/* sum of weights */
	double          sumfw;	/* sum of weighted values */
	double          w;		/* weight of f[i] */
	/* Check for obvious errors */
	if (n <= 0 || neighbors == 0) {
		*fOut = _grMissingValue;
		return 0;
	}
	nn = number_good_xyz(x, y, f, n);
	if (neighbors > nn)
		neighbors = nn;
	/*
	 * Search the rectangle, increasing its size if necessary.
	 */
	do {
		num_in_rect = 0;
		sumw = sumfw = 0.0;
		switch (method) {
		case 0:
			for (i = 0; i < n; i++) {
				dx = x0 - x[i];
				if (-xr <= dx && dx <= xr) {
					dy = y0 - y[i];
					if (-yr <= dy && dy <= yr) {
						sumw += 1.0;
						sumfw += f[i];
						num_in_rect++;
					}
				}
			}
			break;
		case 1:
			for (i = 0; i < n; i++) {
				dx = GRI_ABS(x0 - x[i]);
				if (dx <= xr) {
					dy = GRI_ABS(y0 - y[i]);
					if (dy <= yr) {
						dx /= xr;
						dy /= yr;
						d2 = dx * dx + dy * dy;	/* note 0<=d2<=2 */
						w = (2.0 - d2) / (2.0 + d2);
						sumw += w;
						sumfw += f[i] * w;
						num_in_rect++;
					}
				}
			}
			break;
		case 2:
			for (i = 0; i < n; i++) {
				dx = GRI_ABS(x0 - x[i]);
				if (dx <= xr) {
					dy = GRI_ABS(y0 - y[i]);
					if (dy <= yr) {
						dx /= xr;
						dy /= yr;
						d2 = dx * dx + dy * dy;	/* note 0<=d2<=2 */
						w = (2.0 - d2) / (2.0 + d2);
						w *= w;
						sumw += w;
						sumfw += f[i] * w;
						num_in_rect++;
					}
				}
			}
			break;
		case 3:
			for (i = 0; i < n; i++) {
				dx = GRI_ABS(x0 - x[i]);
				if (dx <= xr) {
					dy = GRI_ABS(y0 - y[i]);
					if (dy <= yr) {
						dx /= xr;
						dy /= yr;
						d2 = dx * dx + dy * dy;	/* note 0<=d2<=2 */
						w = (2.0 - d2) / (2.0 + d2);
						w *= w;
						w *= w;
						sumw += w;
						sumfw += f[i] * w;
						num_in_rect++;
					}
				}
			}
			break;
		default:
			return 0;		/* unknown method! */
		}
		xr *= root_of_2;
		yr *= root_of_2;
	} while ((++enlarge <= enlargements || enlargements < 0)
		 && num_in_rect < neighbors);
	if (num_in_rect > 0) {
		*fOut = sumfw / sumw;
		return num_in_rect;
	} else {
		*fOut = _grMissingValue;
		return 0;
	}
}
