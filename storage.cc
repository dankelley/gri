//#define DEBUG_STORAGE		// Debug

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
	GET_STORAGE(_imageTransform, unsigned char, 3 * 256);
	GET_STORAGE(_imageHist, double, 256);
	return true;
}

bool
allocate_image_storage(int nx, int ny)
{
	if (nx < 0 || ny < 0)
		return false;
	if (image_exists())
		delete [] _image.image;
	_image.ras_magic = RAS_MAGIC;
	_image.ras_width = nx;
	_image.ras_height = ny;
	_image.ras_depth = 8;
	_image.ras_length = _image.ras_width * _image.ras_height;
	_image.ras_type = RT_STANDARD;
	_image.ras_maptype = RMT_NONE;
	_image.ras_maplength = 0;
	_image.image = new unsigned char[_image.ras_length];
	if (!_image.image) OUT_OF_MEMORY;
	_imageHist_exists = false;
	return true;
}

bool
allocate_imageMask_storage(int nx, int ny)
{
	if (nx < 0 || ny < 0)
		return false;
	if (imageMask_exists())
		delete [] _imageMask.image;
	_imageMask.ras_magic = RAS_MAGIC;
	_imageMask.ras_width = nx;
	_imageMask.ras_height = ny;
	_imageMask.ras_depth = 8;
	_imageMask.ras_length = _imageMask.ras_width * _imageMask.ras_height;
	_imageMask.ras_type = RT_STANDARD;
	_imageMask.ras_maptype = RMT_NONE;
	_imageMask.ras_maplength = 0;
	_imageMask.image = new unsigned char[_imageMask.ras_length];
	if (!_imageMask.image) OUT_OF_MEMORY;
	for (unsigned int i = 0; i < _imageMask.ras_length; i++)
		*(_imageMask.image + i) = 0;
	_imageHist_exists = false;
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
		printf("allocate_xmatrix_storage(%d) deleting at %x\n", cols,_xmatrix);
#endif
		delete [] _xmatrix;
	}
	_num_xmatrix_data = cols;
	_xmatrix = new double [_num_xmatrix_data];
	if (!_xmatrix) OUT_OF_MEMORY;
#if defined(DEBUG_STORAGE)
	printf("allocate_xmatrix_storage(%d) allocating at %x\n", cols,_xmatrix);
#endif
	_xgrid_exists = true;
	return true;
}

bool
allocate_ymatrix_storage(int rows)
{
	if (_ygrid_exists == true) {
#if defined(DEBUG_STORAGE)
		printf("allocate_ymatrix_storage(%d) deleting at %x\n", rows,_ymatrix);
#endif
		delete [] _ymatrix;
	}
	_num_ymatrix_data = rows;
	_ymatrix = new double [_num_ymatrix_data];
	if (!_ymatrix) OUT_OF_MEMORY;
#if defined(DEBUG_STORAGE)
	printf("allocate_ymatrix_storage(%d) allocating at %x\n", rows,_ymatrix);
#endif
	_ygrid_exists = true;
	return true;
}
