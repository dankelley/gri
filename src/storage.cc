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

//#define DEBUG_STORAGE 1		// Debug

#include        <stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"image_ex.hh"
#include	"private.hh"
extern char     _grTempString[];

/*
 * Allocate storage.
 */
#define CHARLEN	50
bool
create_arrays()
{
	GET_STORAGE(_dstack, double, _num_dstackMAX);
	GET_STORAGE(_errorMsg, char, LineLength);
	GET_STORAGE(_cmdLine, char, LineLength);
	GET_STORAGE(_cmdLineCOPY, char, LineLength);
	GET_STORAGE(_imageHist, double, 256);
	return true;
}

bool
allocate_image_storage(int nx, int ny)
{
#ifdef DEBUG_STORAGE
	printf("%s:%d ENTERING allocate_image_storage(%d, %d)\n",__FILE__,__LINE__,nx,ny);
#endif
	if (nx < 0 || ny < 0)
		return false;
	_image.ras_magic = RAS_MAGIC;
	_image.ras_width = nx;
	_image.ras_height = ny;
	_image.ras_depth = 8;
	_image.ras_length = _image.ras_width * _image.ras_height;
	_image.ras_type = RT_STANDARD;
	_image.ras_maptype = RMT_NONE;
	_image.ras_maplength = 0;
	if (_image.storage_exists)
		free(_image.image);
	GET_STORAGE(_image.image, unsigned char, _image.ras_length);
	if (!_image.image) OUT_OF_MEMORY;
	_image.storage_exists = true;
	_imageHist_exists = false;
#ifdef DEBUG_STORAGE
	printf("%s:%d allocate_image_storage() got storage for image %d wide and %d tall\n",__FILE__,__LINE__,_image.ras_width,_image.ras_height);
#endif
	return true;
}

bool
allocate_imageMask_storage(int nx, int ny)
{
#ifdef DEBUG_STORAGE
	printf("%s:%d ENTERING allocate_imageMask_storage(%d, %d)\n",__FILE__,__LINE__,nx,ny);
#endif
	if (nx < 0 || ny < 0)
		return false;
	_imageMask.ras_magic = RAS_MAGIC;
	_imageMask.ras_width = nx;
	_imageMask.ras_height = ny;
	_imageMask.ras_depth = 8;
	_imageMask.ras_length = _imageMask.ras_width * _imageMask.ras_height;
	_imageMask.ras_type = RT_STANDARD;
	_imageMask.ras_maptype = RMT_NONE;
	_imageMask.ras_maplength = 0;
	if (_imageMask.storage_exists) {
#ifdef DEBUG_STORAGE
		printf("%s:%d allocate_imageMask_storage() freeing up storage\n",__FILE__,__LINE__);
#endif
		free(_imageMask.image);
	}
	GET_STORAGE(_imageMask.image, unsigned char, _imageMask.ras_length);
	if (!_imageMask.image) OUT_OF_MEMORY;
	_imageMask.storage_exists = true;
	for (unsigned int i = 0; i < _imageMask.ras_length; i++)
		*(_imageMask.image + i) = 0;
	_imageHist_exists = false;
#ifdef DEBUG_STORAGE
	printf("%s:%d allocate_imageMask_storage() got storage for imageMask %d wide and %d tall\n",__FILE__,__LINE__,_imageMask.ras_width,_imageMask.ras_height);
#endif
	return true;
}

bool
allocate_grid_storage(int nx, int ny)
{
	if (nx < 0 || ny < 0)
		return false;
	_num_xmatrix_data = nx;
	_num_ymatrix_data = ny;
	_f_xy.set_size(_num_xmatrix_data, _num_ymatrix_data);
	_f_xy.set_value(0.0);
	_legit_xy.set_size(_num_xmatrix_data, _num_ymatrix_data);
	_legit_xy.set_value(true);
	_grid_exists = true;
	return true;
}

bool
allocate_xmatrix_storage(int cols)
{
	if (_xgrid_exists == true) {
#if defined(DEBUG_STORAGE)
		printf("allocate_xmatrix_storage(%d) deleting storage\n", cols);
#endif
		delete [] _xmatrix;
	}
	_num_xmatrix_data = cols;
	_xmatrix = new double [_num_xmatrix_data];
	if (!_xmatrix) OUT_OF_MEMORY;
#if defined(DEBUG_STORAGE)
	printf("allocate_xmatrix_storage(%d) allocating\n", cols);
#endif
	_xgrid_exists = true;
	return true;
}

bool
allocate_ymatrix_storage(int rows)
{
	if (_ygrid_exists == true) {
#if defined(DEBUG_STORAGE)
		printf("allocate_ymatrix_storage(%d) deleting\n", rows);
#endif
		delete [] _ymatrix;
	}
	_num_ymatrix_data = rows;
	_ymatrix = new double [_num_ymatrix_data];
	if (!_ymatrix) OUT_OF_MEMORY;
#if defined(DEBUG_STORAGE)
	printf("allocate_ymatrix_storage(%d) allocating\n", rows);
#endif
	_ygrid_exists = true;
	return true;
}
