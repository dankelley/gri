#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"private.hh"
#include	"extern.hh"
#include	"image_ex.hh"
#include	"gr.hh"
extern char     _grTempString[];

bool            writeCmd(void);
bool            write_columnsCmd(const char *filename);
bool            write_contourCmd(const char *filename);
bool            write_gridCmd(const char *filename);
bool            write_imageCmd(const char *filename, int image_type);
bool            write_image_maskCmd(const char *filename, int image_type);
bool            write_image_colorscaleCmd(const char *filename);
bool            write_image_grayscaleCmd(const char *filename);
// image types
#define IMAGE_RAW 0		// no header
#define IMAGE_RASTER 1		// sun rasterfile header
#define IMAGE_PGM 2		// pgm 'rawbits' header


// `write contour .value. to \filename'
bool
write_contourCmd(const char *filename)
{
	double          dlevel;
	FILE           *fp;
	// Decode command
	if (_nword == 5) {
		if (!getdnum(_word[2], &dlevel))
			return false;
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	std::string fname(filename);
	un_double_quote(fname);
	if (!strcmp(fname.c_str(), "stdout"))
		fp = stdout;
	else if (!strcmp(fname.c_str(), "stderr"))
		fp = stderr;
	else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
		err("Sorry, `write columns \\file' can't open file named in next line");
		err(fname.c_str());
		return false;
	}
	// Check that data exist
	if (!grid_exists())
		return false;
	if (!scales_defined()) {
		no_scales_error();
		return false;
	}
	// Do contour(s)
	set_environment();
	gr_contour(_xmatrix,
		   _ymatrix,
		   _f_xy,
		   _legit_xy,
		   _num_xmatrix_data,
		   _num_ymatrix_data,
		   dlevel,
		   NULL,
		   false,
		   false,
		   false,
		   _griState.color_line(),
		   _griState.color_text(),
		   0.0,
		   0.0,
		   0.0,
		   fp);
	_drawingstarted = true;
	draw_axes_if_needed();
	fclose(fp);
	return true;
}

bool
write_image_header(IMAGE im, FILE * fp)
{
	fwrite((char *) & im.ras_magic, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_width, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_height, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_depth, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_length, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_type, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_maptype, sizeof(int), 1, fp);
	fwrite((char *) & im.ras_maplength, sizeof(int), 1, fp);
	return true;
}

// ?? BUG -- should check for "to" in words
bool
writeCmd()
{
	int             image_type = IMAGE_RAW;
	char *            filename;
	if (_nword < 4) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	filename = _word[_nword - 1];
	if (word_is(1, "image")) {
		if (word_is(2, "rasterfile"))
			return write_imageCmd(filename, 1);
		else if (word_is(2, "pgm"))
			return write_imageCmd(filename, 2);
		else if (word_is(2, "colorscale"))
			return write_image_colorscaleCmd(filename);
		else if (word_is(2, "colourscale"))
			return write_image_colorscaleCmd(filename);
		else if (word_is(2, "grayscale"))
			return write_image_grayscaleCmd(filename);
		else if (word_is(2, "greyscale"))
			return write_image_grayscaleCmd(filename);
		else if (word_is(2, "mask"))
			return write_image_maskCmd(filename, image_type);
		else
			return write_imageCmd(filename, image_type);
	} else if (!strcmp(_word[1], "columns")) {
		write_columnsCmd(_word[_nword - 1]);
	} else if (!strcmp(_word[1], "contour")) {
		write_contourCmd(_word[_nword - 1]);
	} else if (!strcmp(_word[1], "grid")) {
		write_gridCmd(_word[_nword - 1]);
	} else {
		demonstrate_command_usage();
		err("Can't understand command.");
		return false;
	}
	return true;
}

bool
write_gridCmd(const char *filename)
{
	bool            bycolumns = false;
	FILE           *fp;
	double          missing_value = gr_currentmissingvalue();
	if (!grid_exists()) {
		err("No grid yet");
		return false;
	}
	// Special case
	std::string fname;
	unsigned int i, j;
	switch (_nword) {
	case 4:
		// `write grid to filename'
		fname.assign(filename);
		un_double_quote(fname);
		if (!strcmp(fname.c_str(), "stdout"))
			fp = stdout;
		else if (!strcmp(fname.c_str(), "stderr"))
			fp = stderr;
		else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
			err("Sorry, `write grid to \\file' can't open the file named in next line");
			err(fname.c_str());
			return false;
		}
		break;
	case 5:
		// `write grid to filename bycolumns'
		if (word_is(4, "bycolumns")) {
			fname.assign(_word[_nword - 2]);
			un_double_quote(fname);
			bycolumns = true;
			if (!strcmp(fname.c_str(), "stdout"))
				fp = stdout;
			else if (!strcmp(fname.c_str(), "stderr"))
				fp = stderr;
			else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
				err("Sorry, `write grid to \\file' can't open the file named in next line");
				err(fname.c_str());
				return false;
			}
		} else {
			demonstrate_command_usage();
			err("Fourth word must be `bycolumns'");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (bycolumns == true) {
		for (i = 0; i < _num_xmatrix_data; i++) {
			for (j = 0; j < _num_ymatrix_data; j++) {
				if (_legit_xy(i, j) == true) {
					fprintf(fp, "%f ", _f_xy(i, j));
				} else {
					fprintf(fp, "%f ", missing_value);
				}
			}
			fprintf(fp, "\n");
		}
	} else {
		j = _num_ymatrix_data - 1;
		do {
			for (i = 0; i < _num_xmatrix_data; i++) {
				if (_legit_xy(i, j) == true) {
					fprintf(fp, "%f ", _f_xy(i, j));
				} else {
					fprintf(fp, "%f ", missing_value);
				}
			}
			fprintf(fp, "\n");
		} while (j-- != 0);
	}
	fclose(fp);
	return true;
}

bool
write_columnsCmd(const char *filename)
{

	unsigned int num = _colX.size();
	if (num < 1) {
		err("Can't `write columns \\file' since no columns exist yet\n");
		return false;
	}
	std::string fname(filename);
	un_double_quote(fname);
	FILE *fp;
	if (!strcmp(fname.c_str(), "stdout"))
		fp = stdout;
	else if (!strcmp(fname.c_str(), "stderr"))
		fp = stderr;
	else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
		err("Sorry, `write columns \\file' can't open file named in next line");
		err(fname.c_str());
		return false;
	}
	// print label line
	fprintf(fp, "//");
	if (_colX.size() > 0)
		fprintf(fp, "%20s\t", "x");
	if (_colY.size() > 0)
		fprintf(fp, "%20s\t", "y");
	if (_colU.size() > 0)
		fprintf(fp, "%15s\t", "u");
	if (_colV.size() > 0)
		fprintf(fp, "%15s\t", "v");
	if (_colZ.size() > 0)
		fprintf(fp, "%15s\t", "z");
	fprintf(fp, "\n");
	for (unsigned int i = 0; i < num; i++) {
		fprintf(fp, "%15g\t%15g\t", _colX[i], _colY[i]);
		if (_colU.size() > 0)
			fprintf(fp, "%15g\t", _colU[i]);
		if (_colV.size() > 0)
			fprintf(fp, "%15g\t", _colV[i]);
		if (_colZ.size() > 0)
			fprintf(fp, "%15g\t", _colZ[i]);
		fprintf(fp, "\n");
	}
	fclose(fp);
	return true;
}

// write image [pgm] to filename
bool
write_imageCmd(const char *filename, int image_type)
{
	unsigned char   zero = 0;
	int             ii;
	register int    i, j;
	FILE           *fp;
	if (!_image.storage_exists) {
		err("No image exists yet\n");
		return false;
	}
	std::string fname(filename);
	un_double_quote(fname);
	if (!strcmp(fname.c_str(), "stdout"))
		fp = stdout;
	else if (!strcmp(fname.c_str(), "stderr"))
		fp = stderr;
	else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
		err("Sorry, `write image \\file' can't open file named in next line");
		err(filename);
		return false;
	}
	if (_num_xmatrix_data < 1 || _num_xmatrix_data < 1) {
		err("First create x/y grids, e.g. `read grid x' or `set x grid', ...");
		return false;
	}
	// kludge because sun wants even number of bytes / row
	ii = _image.ras_width;
	if (ii % 2)
		ii++;
	// write header (maybe)
	switch (image_type) {
	case IMAGE_RAW:
		break;
	case IMAGE_RASTER:
		write_image_header(_image, fp);
		break;
	case IMAGE_PGM:
		fprintf(fp, "P5\n%d %d\n255\n", _image.ras_width, _image.ras_height);
		break;
	}
	if (_imageTransform_exists) {
		for (j = int(_image.ras_height - 1); j >= 0; j--) {
			for (i = 0; i < int(_image.ras_width); i++)
				fwrite((char *)
				       (_imageTransform + *(_image.image + _image.ras_height * i + j)),
				       sizeof(unsigned char), 1, fp);
			if (ii != i)
				fwrite((char *) & zero, sizeof(unsigned char), 1, fp);
		}
	} else {
		for (j = int(_image.ras_height - 1); j >= 0; j--) {
			for (i = 0; i < int(_image.ras_width); i++)
				fwrite((char *)
				       (_image.image + _image.ras_height * i + j),
				       sizeof(unsigned char), 1, fp);
			if (ii != i) {
				fwrite((char *) & zero, sizeof(unsigned char), 1, fp);
			}
		}
	}
	fclose(fp);
	return true;
}

bool
write_image_maskCmd(const char *filename, int image_type)
{
	unsigned char   zero = 0;
	int             ii;
	register int    i, j;
	FILE           *fp;
	if (!_imageMask.storage_exists) {
		err("No image mask exists yet\n");
		return false;
	}
	if (_num_xmatrix_data < 1 || _num_xmatrix_data < 1) {
		err("Sorry, `write image mask to \\file' can't figure out nx or ny");
		return false;
	}
	std::string fname(filename);
	un_double_quote(fname);
	if (!strcmp(fname.c_str(), "stdout"))
		fp = stdout;
	else if (!strcmp(fname.c_str(), "stderr"))
		fp = stderr;
	else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
		err("Sorry, `write image mask \\file' can't open file named in next line");
		err(filename);
		return false;
	}
	// kludge because sun wants even number of bytes / row
	ii = _num_xmatrix_data;
	if (ii % 2)
		ii++;

	// write header (maybe)
	switch (image_type) {
	case IMAGE_RAW:
		break;
	case IMAGE_RASTER:
	{
		IMAGE           im;
		im.ras_magic = RAS_MAGIC;
		im.ras_width = _num_xmatrix_data;
		im.ras_height = _num_ymatrix_data;
		im.ras_depth = 8;
		im.ras_length = im.ras_width * im.ras_height;
		im.ras_type = RT_STANDARD;
		im.ras_maptype = RMT_NONE;
		im.ras_maplength = 0;
		write_image_header(im, fp);
	}
	break;
	case IMAGE_PGM:
		fprintf(fp, "P5\n%d %d\n255\n", _image.ras_width, _image.ras_height);
		break;
	}
	for (j = int(_image.ras_height - 1); j > -1; j--) {
		for (i = 0; i < int(_image.ras_width); i++)
			fwrite((char *) (_imageMask.image + _imageMask.ras_height * i + j),
			       sizeof(unsigned char), 1, fp);
		if (ii != i)
			fwrite((char *) & zero, sizeof(unsigned char), 1, fp);
	}
	fclose(fp);
	return true;
}

bool
write_image_colorscaleCmd(const char *filename)
{
	extern unsigned char *_imageTransform;
	int             i;
	FILE           *fp;
	if (!_imageTransform_exists) {
		err("First `set image grayscale'");
		return false;
	}
	std::string fname(filename);
	un_double_quote(fname);
	if (!strcmp(fname.c_str(), "stdout"))
		fp = stdout;
	else if (!strcmp(fname.c_str(), "stderr"))
		fp = stderr;
	else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
		err("Sorry, `write image grayscale to \\file' can't open file named in next line");
		err(filename);
		return false;
	}
	for (i = 0; i < 256; i++)
		fprintf(fp, "%g %g %g\n",
			*(_imageTransform + i) / 255.0,
			*(_imageTransform + i + 256) / 255.0,
			*(_imageTransform + i + 512) / 255.0);
	fclose(fp);
	return true;
}

bool
write_image_grayscaleCmd(const char *filename)
{
	extern unsigned char *_imageTransform;
	int             i;
	FILE           *fp;
	if (!_imageTransform_exists) {
		err("First `set image grayscale'");
		return false;
	}
	std::string fname(filename);
	un_double_quote(fname);
	if (!strcmp(fname.c_str(), "stdout"))
		fp = stdout;
	else if (!strcmp(fname.c_str(), "stderr"))
		fp = stderr;
	else if (NULL == (fp = fopen(fname.c_str(), "a+"))) {
		err("Sorry, `write image grayscale to \\file' can't open file named in next line");
		err(fname.c_str());
		return false;
	}
	for (i = 0; i < 256; i++)
		fprintf(fp, "%g\n", *(_imageTransform + i) / 255.0);
	fclose(fp);
	return true;
}
