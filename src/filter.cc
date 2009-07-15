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

#include <string>
#include <vector>
#include <algorithm>	// for reverse

#include <stdio.h>
#include <stddef.h>

#include "gr.hh"
#include "extern.hh"
#include "image_ex.hh"

static bool filter_butterworth(double *x,
			       double *xout,
			       int nx,
			       const std::vector<double>& a,
			       const std::vector<double>& b);

static bool     filter_image(int horl);
void            highpass_image();
void            lowpass_image();
#define ODD(n)	(2 * ((n) / 2) != (n))

// filter grid rows|columns recursively a[0] a[1] ... b[0] b[1] ...
// 0      1    2            3           4
bool
filter_gridCmd()
{
	if (_nword <= 3) {
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!word_is(3, "recursively")) {
		err("Fourth word must be `recursively'");
		return false;
	}
	bool            do_rows = true;
	if (word_is(2, "rows")) {
		do_rows = true;
	} else if (word_is(2, "columns")) {
		do_rows = false;
	} else {
		err("Third word must be `rows' or `columns'");
		return false;
	}
	// Get the coefficients
	int nab = _nword - 4; // number (a,b) coefficients
	if (nab < 1) {
		err("No filter coefficients given.");
		return false;
	}
	if (ODD(nab)) {
		err("Must give even number of filter coefficients.");
		return false;
	}
	nab = nab / 2;
	std::vector<double> a((size_t)nab, 0.0);
	std::vector<double> b((size_t)nab, 0.0);
	unsigned row, col;
	for (unsigned int i = 0; i < (unsigned int) nab; i++) {
		a[i] = atof(_word[4 + i]);
		b[i] = atof(_word[4 + i + nab]);
	}
	// Do the filtering
	double *orig = NULL, *copy = NULL;
	if (do_rows) {
		GET_STORAGE(orig, double, (size_t)_num_ymatrix_data);
		GET_STORAGE(copy, double, (size_t)_num_ymatrix_data);
		for (col = 0; col < _num_xmatrix_data; col++) {
			for (row = 0; row < _num_ymatrix_data; row++)
				orig[row] = _f_xy(col, row);
			filter_butterworth(orig, copy, _num_ymatrix_data, a, b);
			for (row = 0; row < _num_ymatrix_data; row++)
				_f_xy(col, row) = copy[row];
		}
	} else {
		GET_STORAGE(orig, double, (size_t)_num_xmatrix_data);
		GET_STORAGE(copy, double, (size_t)_num_xmatrix_data);
		for (row = 0; row < _num_ymatrix_data; row++) {
			for (col = 0; col < _num_xmatrix_data; col++)
				orig[col] = _f_xy(col, row);
			filter_butterworth(orig, copy, _num_xmatrix_data, a, b);
			for (col = 0; col < _num_xmatrix_data; col++)
				_f_xy(col, row) = copy[col];
		}
	}
	if (orig != NULL)
		free(orig);
	if (copy != NULL)
		free(copy);
	return true;
}

// filter column x|y|z|u|v|weight recursively a[0] a[1] ... b[0] b[1] ...
bool
filter_columnCmd()
{
	unsigned int nab;	// number of a, b coefficients
	unsigned int num = 0;	// length of column
	Require(_nword > 3,
		NUMBER_WORDS_ERROR);
	Require(word_is(3, "recursively"),
		err("Fourth word must be `recursively'"));
	double *orig;
	if (word_is(2, "x")) {
		num = _colX.size();
		orig = _colX.begin();
	} else if (word_is(2, "y")) {
		num = _colY.size();
		orig = _colY.begin();
	} else if (word_is(2, "z")) {
		num = _colZ.size();
		orig = _colZ.begin();
	} else if (word_is(2, "u")) {
		num = _colU.size();
		orig = _colU.begin();
	} else if (word_is(2, "v")) {
		num = _colV.size();
		orig = _colV.begin();
	} else if (word_is(2, "weight")) {
		num = _colWEIGHT.size();
		orig = _colWEIGHT.begin();
	} else {
		orig = 0;		// prevent compiler warning
		err("Unknown item.");
	}
	Require (num > 0,
		 err("No data in column named '", _word[2], "'", "\\"));
	nab = _nword - 4;
	Require(nab > 0,
		err("No filter coefficients given."));
	Require(!ODD(nab),
		err("Must give even number of filter coefficients."));
	nab = nab / 2;
	std::vector<double> a((size_t)nab, 0.0);
	std::vector<double> b((size_t)nab, 0.0);
	for (unsigned int i = 0; i < nab; i++) {
		a[i] = atof(_word[4 + i]);
		b[i] = atof(_word[4 + i + nab]);
	}
	double *copy = (double*)NULL;
	GET_STORAGE(copy, double, (size_t)num);
	filter_butterworth(orig, copy, num, a, b);
	for (unsigned int i = 0; i < num; i++)
		orig[i] = copy[i];
	free(copy);
	return true;
}

// filter_butterworth() -- do butterworth filtering, forward+back
// Input is x[] (unaltered), output is xout[]; recursive-style coefficients are
// a[]; moving average-style coefficients are b[].
static bool
filter_butterworth(double* x,
		   double* xout,
		   int nx,
		   const std::vector<double>& a,
		   const std::vector<double>& b)
{
	int nab = a.size();
	if (nab >= nx)
		return false;
	double *z = (double*)NULL;
	GET_STORAGE(z, double, (size_t)nx);
	register int    ix, iab;
	// pass 1 -- forward
	for (ix = 0; ix < nab; ix++) {
		// beginning part -- just copy
		z[ix] = x[ix];
	}
	for (ix = nab; ix < nx; ix++) {
		z[ix] = b[0] * x[ix];
		for (iab = 1; iab < nab; iab++)
			z[ix] += b[iab] * x[ix - iab] - a[iab] * z[ix - iab];
	}
	// pass 2 -- backward
	std::reverse(z, z + nx);
	for (ix = 0; ix < nab; ix++)
		xout[ix] = z[ix];
	for (ix = nab; ix < nx; ix++) {
		xout[ix] = b[0] * z[ix];
		for (iab = 1; iab < nab; iab++)
			xout[ix] += b[iab] * z[ix - iab] - a[iab] * xout[ix - iab];
	}
	std::reverse(xout, xout + nx);
	free(z);
	return true;
}

bool
filter_imageCmd()
{
	if (_nword != 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
	} else if (!strcmp(_word[2], "highpass"))
		filter_image(1);
	else if (!strcmp(_word[2], "lowpass"))
		filter_image(0);
	else {
		demonstrate_command_usage();
		err("Can only do `highpass' or `lowpass' filter");
		return false;
	}
	return true;
}

static          bool
filter_image(int horl)
{
	unsigned char  *imPtr;
	register int    i, j;
	int             nx, ny, nx1, ny1, newval, change;
	Require(_image.storage_exists, err("no image exists"));
	nx = _image.ras_width;
	nx1 = nx - 1;
	ny = _image.ras_height;
	ny1 = ny - 1;
	std::vector<unsigned char> imagenew((size_t)(nx * ny), (unsigned char)0);
	for (j = ny - 1; j > -1; j--) {
		imagenew[j] = _image.image[j];
		imagenew[nx1 * ny + j] = _image.image[nx1 * ny + j];
	}
	for (i = 0; i < nx; i++) {
		imagenew[i * ny] = _image.image[i * ny];
		imagenew[i * ny + ny1] = _image.image[i * ny + ny1];
	}
	for (j = 1; j < ny1; j++) {
		for (i = 1; i < nx1; i++) {
			imPtr = _image.image + i * ny + j;
			change = *(imPtr + 1);	// i,j+1
			change += *(imPtr - 1);	// i,j-1
			change += *(imPtr - ny);	// i-1,j
			change += *(imPtr + ny);	// i-1,j
			change -= *imPtr * 4;
			change = (int) (0.125 * (double) change);
			switch (horl) {
			case 0:		// low pass
				newval = *imPtr + change;
				break;
			case 1:		// high pass
			default:
				newval = *imPtr - change;
				break;
			}
			if (newval < 0)
				imagenew[i * ny + j] = (unsigned char) 0;
			else if (newval > 255)
				imagenew[i * ny + j] = (unsigned char) 255;
			else
				imagenew[i * ny + j] = (unsigned char) newval;
		}
	}
	nx *= ny;
	for (i = 0; i < nx; i++)
		_image.image[i] = imagenew[i];
	return true;
}

void
highpass_image()
{
	filter_image(1);
}

void
lowpass_image()
{
	filter_image(0);
}
