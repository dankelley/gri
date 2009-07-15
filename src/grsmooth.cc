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

#include        <math.h>
#include        <stdio.h>
#include	"gr.hh"
#include	"GMatrix.hh"


// gr_smootharray() -- smooth z(x,y) data
// 
// DESCRIPTION Smooths data z[i][j] defined on a rectangular grid x[i] and y[j]
// (where 0<=i<nx and 0<=j<ny). The resultant smoothed field is put into
// zS[][]. The array legit[i][j] is 1 if z[i][j] is a good data point; 0
// otherwise. When computing the smoothed value at a point, neighboring
// points having legit[][]=0 are ignored. So are points outside the region
// 0<=i<nx and 0<=j<ny. The distance between grid points in the x direction
// is dx; in the y direction it's dy. dt indicates the strength of smoothing,
// as described below.  Holes in the grid (i.e., missing values) will be
// smoothed over if fill_in is false.
// 
// Returns true if all was OK; false if unknown method supplied.
// 
// METHOD: 'method' specifies the smoothing algorithm as follows:
// 
// method = 0
// 
// Smoothing is by equal weights over points in a + shaped window about the
// datum: zS[i][j] =   z[i][j]/2 + (z[i-1][j]+z[i+1][j])/8 +
// (z[i][j-1]+z[i][j+1])/8 The parameters dx, dy, and dt are ignored.
// 
// method = 1
// 
// Smoothing is across x: zS[i][j] = z[i][j]/2 + (z[i-1][j] + z[i+1][j]) / 4;
// 
// method = 2
// 
// Smoothing is across y: zS[i][j] = z[i][j]/2 + (z[i][j-1] + z[i][j+1]) / 4;
// 
// BUGS: None known.
bool
gr_smootharray(double dx, 
               double dy,
               double dt,
	       GriMatrix<double> &z,
	       GriMatrix<double> &zS,
	       GriMatrix<bool> &legit, 
	       GriMatrix<bool> &legitS,
	       int nx,
	       int ny,
	       int method)
{
	register int    i, j;
	double          sum;
	int             good;	// number data used per gridpoint
	int             nx_1, ny_1;
	// Test for errors
	if (nx <= 0 || ny <= 0)
		return false;
	nx_1 = nx - 1;
	ny_1 = ny - 1;
	dx = dy = dt = 0.0;		// Kill warning on non-use
	switch (method) {
	case 0:
		for (i = 0; i < nx; i++) {
			for (j = 0; j < ny; j++) {
				good = 0;
				sum = 0.0;
				// Datum at centre
				if (legit(i, j) == true) {
					sum += 0.5 * z(i, j);
					good++;
				}
				// Datum to left
				if (i == 0) {
					// Left edge: interpolate
					if (legit(i + 1, j) == true
					    && legit(i, j) == true) {
						sum += 0.125 * (2.0 * z(i, j) - z(i + 1, j));
						good++;
					}
				} else {
					// Interior
					if (legit(i - 1, j) == true) {
						sum += 0.125 * z(i - 1, j);
						good++;
					}
				}
				// Datum to right
				if (i == nx_1) {
					// Right edge: interpolate
					if (legit(i - 1, j) == true
					    && legit(i, j) == true) {
						sum += 0.125 * (2.0 * z(i, j) - z(i - 1, j));
						good++;
					}
				} else {
					// Interior
					if (legit(i + 1, j) == true) {
						sum += 0.125 * z(i + 1, j);
						good++;
					}
				}
				// Datum below
				if (j == 0) {
					// Bottom: interpolate
					if (legit(i, j + 1) == true
					    && legit(i, j) == true) {
						sum += 0.125 * (2.0 * z(i, j) - z(i, j + 1));
						good++;
					}
				} else {
					if (legit(i, j - 1) == true) {
						sum += 0.125 * z(i, j - 1);
						good++;
					}
				}
				// Datum above
				if (j == ny_1) {
					// Top: interpolate
					if (legit(i, j - 1) == true
					    && legit(i, j) == true) {
						sum += 0.125 * (2.0 * z(i, j) - z(i, j - 1));
						good++;
					}
				} else {
					if (legit(i, j + 1) == true) {
						sum += 0.125 * z(i, j + 1);
						good++;
					}
				}
				if (good == 5) {
					zS(i, j) = sum;
					legitS(i, j) = true;
				} else {
					zS(i, j) = z(i, j);	// won't be used anyway
					legitS(i, j) = false;
				}
			}
		}
		break;
	case 1:			// smooth across x
		for (i = 0; i < nx; i++) {
			for (j = 0; j < ny; j++) {
				sum = 0.0;
				good = 0;
				// Datum at centre
				if (legit(i, j) == true) {
					sum += 0.5 * z(i, j);
					good++;
				}
				// Datum to left
				if (i == 0) {
					// Left edge: interpolate
					if (legit(i + 1, j) == true
					    && legit(i, j) == true) {
						sum += 0.25 * (2.0 * z(i, j) - z(i + 1, j));
						good++;
					}
				} else {
					// Interior
					if (legit(i - 1, j) == true) {
						sum += 0.25 * z(i - 1, j);
						good++;
					}
				}
				// Datum to right
				if (i == nx_1) {
					// Right edge: interpolate
					if (legit(i - 1, j) == true
					    && legit(i, j) == true) {
						sum += 0.25 * (2.0 * z(i, j) - z(i - 1, j));
						good++;
					}
				} else {
					// Interior
					if (legit(i + 1, j) == true) {
						sum += 0.25 * z(i + 1, j);
						good++;
					}
				}
				if (good == 5) {
					zS(i, j) = sum;
					legitS(i, j) = true;
				} else {
					zS(i, j) = z(i, j);	// won't be used anyway
					legitS(i, j) = false;
				}
			}
		}
		break;
	case 2:			// smooth across y
		for (i = 0; i < nx; i++) {
			for (j = 0; j < ny; j++) {
				sum = 0.0;
				good = 0;
				// Datum at centre
				if (legit(i, j) == true) {
					sum += 0.5 * z(i, j);
					good++;
				}
				// Datum below
				if (j == 0) {
					// Bottom: interpolate
					if (legit(i, j + 1) == true
					    && legit(i, j) == true) {
						sum += 0.25 * (2.0 * z(i, j) - z(i, j + 1));
						good++;
					}
				} else {
					if (legit(i, j - 1) == true) {
						sum += 0.25 * z(i, j - 1);
						good++;
					}
				}
				// Datum above
				if (j == ny_1) {
					// Top: interpolate
					if (legit(i, j - 1) == true
					    && legit(i, j) == true) {
						sum += 0.25 * (2.0 * z(i, j) - z(i, j - 1));
						good++;
					}
				} else {
					if (legit(i, j + 1) == true) {
						sum += 0.25 * z(i, j + 1);
						good++;
					}
				}
				if (good == 5) {
					zS(i, j) = sum;
					legitS(i, j) = true;
				} else {
					zS(i, j) = z(i, j);	// won't be used anyway
					legitS(i, j) = false;
				}
			}
		}
		break;
	default:
		return false;		// unknown
	}
	return true;
}
