//#define DEBUG_IMAGE 1		// for debugging
#include	<stdio.h>
#include	<string.h>
#include	"extern.hh"

/*
 * Image things (shared by other files through image_ex.h)
 */
double          _image0 = 0.0, _image255 = 0.0;	/* uservalue <-> [0,255]
						 * scale */
double          _image_missing_color_red = 1.0;	/* for missing data */
double          _image_missing_color_green = 1.0; /* for missing data */
double          _image_missing_color_blue = 1.0; /* for missing data */
double          _image_llx, _image_lly, _image_urx, _image_ury;	/* coords */
double         *_imageHist;
int             _image_color_model = 0;	/* 0=bw 1=rgb */
bool            _imageTransform_exists = false;
bool            _imageHist_exists = false;
IMAGE           _image, _imageMask;
unsigned char  *_imageTransform;

static bool     x_image_scale_defined = false;
static bool     y_image_scale_defined = false;
void            show_image_transform(void);	/* for debugging */

void 
show_image_transform()
{
	if (_imageTransform_exists) {
		int             i;
		printf("_imageTransform[0-255] is:\n");
		for (i = 0; i < 256; i++) {
			printf("im_tr[%d] = %d\n", i, _imageTransform[i]);
		}
	} else {
		printf("_imageTransform[] NOT DEFINED YET\n");
	}
}

bool
define_image_scales(double llx, double lly, double urx, double ury)
{
	if (llx != urx) {
		_image_llx = llx;
		_image_urx = urx;
		x_image_scale_defined = true;
	}
	if (lly != ury) {
		_image_lly = lly;
		_image_ury = ury;
		y_image_scale_defined = true;
	}
	return true;
}

bool
image_scales_defined()
{
	return ((x_image_scale_defined && y_image_scale_defined) ? true : false);
}

/*
 * Blank out image; make it be bw, not color.
 */
bool
blank_image()
{
	if (_image.storage_exists) {
		if (_image.image != NULL)
			free(_image.image);
		_image.image = NULL;
		_image.storage_exists = false;
	}
	_image.ras_width = _image.ras_height = _image.ras_length = 0;
	_image_llx = _image_lly = 0.0;
	_image_urx = _image_ury = 0.0;
	_image0 = _image255 = 0.0;	/* used by image_range_exists() */
	_image_color_model = 0;
	x_image_scale_defined = false;
	y_image_scale_defined = false;
	_imageHist_exists = false;
	/*
	 * Make transform into an even ramp, for each or R, G, B.  Note that val
	 * will wrap around at 255.
	 */
	unsigned int i;
	unsigned char val;
	for (val = 0, i = 0; i < 768; i++, val++)
		_imageTransform[i] = val;
	_imageTransform_exists = true;
#ifdef DEBUG_IMAGE
	printf("%s:%d blanked image with storage at %X\n",__FILE__,__LINE__,(int)(_image.image));
#endif
	return true;
}

bool
blank_imageMask()
{
	if (_imageMask.storage_exists) {
		free(_imageMask.image);
		_imageMask.image = NULL;
		_imageMask.storage_exists = false;
	}
	_imageMask.ras_width = _imageMask.ras_height = _imageMask.ras_length = 0;
#ifdef DEBUG_IMAGE
	printf("%s:%d blanked imageMask with storage at %X\n",__FILE__,__LINE__,(int)(_imageMask.image));
#endif
	return true;
}
bool
initialize_image()
{
	_image.image = NULL;
	_image.storage_exists = false;
	GET_STORAGE(_imageTransform, unsigned char, 3 * 256);
	return blank_image();
}
bool
initialize_imageMask()
{
	_imageMask.image = NULL;
	_imageMask.storage_exists = false;
	return blank_imageMask();
}

// Tell if the image range exists (created by `set image range').
bool
image_range_exists()
{
	return ((_image0 != 0.0 || _image255 != 0.0) ? true : false);
}

#if 0
//bool
//image_exists()
//{
//	return ((_image.ras_length > 0) ? true : false);
//}
//bool
//imageMask_exists()
//{
//	return ((_imageMask.ras_length > 0) ? true : false);
//}
#endif

/*
 * Calculate histogram of image, normalized to sum to 1 over all image
 * values.  Histogram is defined in _imageHist[]; for example, _imageHist[0]
 * is the normalized fraction of pixels with value 0.
 * 
 * NOTE this code assumes 8bit images; to change that, change the symbol NUM,
 * and change the unsigned char references to whatever makes sense.
 */
#define NUM 256			/* change if not 8 bit images */
bool
calculate_image_histogram()
{
	long            good = 0, mhis[NUM];
	int             i;
	if (!_image.storage_exists) {
		err("no image exists");
		return false;
	}
	if (_imageHist_exists)
		return true;
	for (i = 0; i < NUM; i++)
		mhis[i] = 0;
	if (_imageMask.storage_exists) {
		long            max = _image.ras_width * _image.ras_height;
		unsigned char  *im = _image.image;
		unsigned char  *imMask = _imageMask.image;
		for (i = 0; i < max; i++) {
			if (!*(imMask)) {
				mhis[*im]++;
				good++;
			}
			im++;
			imMask++;
		}
	} else {
		long            max = _image.ras_width * _image.ras_height;
		unsigned char  *im = _image.image;
		for (i = 0; i < max; i++) {
			mhis[*im]++;
			good++;
			im++;
		}
	}
	for (i = 0; i < NUM; i++)
		_imageHist[i] = (double) mhis[i] / (double) good;
	_imageHist_exists = true;
	return true;
}

#undef NUM

double
image_to_value(int c)
{
	return (double) (_image0 + c * (_image255 - _image0) / 255.0);
}

/* value_to_image() -- conv double to uchar image value */
/*
 * XREF convert_grid_to_imageCmd()
 */
unsigned char
value_to_image(double v)
{
	int             I;
	I = (int) floor(0.5 + (255.0 * (v - _image0) / (_image255 - _image0)));
	if (I < 0)
		return ((unsigned char) 0);
	else if (I > 255)
		return ((unsigned char) 255);
	else
		return ((unsigned char) I);
}
