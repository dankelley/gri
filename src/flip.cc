#include	<string>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"image_ex.hh"

bool            flipCmd();
bool            flip_gridCmd(void);
bool            flip_imageCmd(void);

bool
flipCmd()
{
	if (_nword != 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!strcmp(_word[1], "grid"))
		return flip_gridCmd();
	else if (!strcmp(_word[1], "image"))
		return flip_imageCmd();
	else
		return false;
}

bool
flip_gridCmd()
{
	register int    i, j;
	int             width, width_half, height, height_half;
	double          swap;
	bool            do_x = true;
	if (_nword != 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!strcmp(_word[2], "x"))
		do_x = true;
	else if (!strcmp(_word[2], "y"))
		do_x = false;
	else {
		demonstrate_command_usage();
		err("Can only flip with respect to `x' or `y'");
		return false;
	}
	if (!_grid_exists) {
		err("No grid exists yet\n");
		return false;
	}
	// Flip the grid.
	width = _num_xmatrix_data;
	width_half = width / 2;
	height = _num_ymatrix_data;
	height_half = height / 2;
	if (do_x) {
		for (i = 0; i < width_half; i++)
			for (j = 0; j < height; j++) {
				int             i_swap = width - i - 1;
				swap = _f_xy(i, j);
				_f_xy(i, j) = _f_xy(i_swap, j);
				_f_xy(i_swap, j) = swap;
				bool swap_legit = _legit_xy(i, j);
				_legit_xy(i, j) = _legit_xy(i_swap, j);
				_legit_xy(i_swap, j) = swap_legit;
			}
	} else {
		for (i = 0; i < width; i++)
			for (j = 0; j < height_half; j++) {
				int             j_swap = height - j - 1;
				swap = _f_xy(i, j);
				_f_xy(i, j) = _f_xy(i, j_swap);
				_f_xy(i, j_swap) = swap;
				bool swap_legit = _legit_xy(i, j);
				_legit_xy(i, j) = _legit_xy(i, j_swap);
				_legit_xy(i, j_swap) = swap_legit;
			}
	}
	return true;
}

bool
flip_imageCmd()
{
	register int    i, j, width, width_half, height, height_half;
	register unsigned char swap;
	bool            do_x = true;
	if (_nword != 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!strcmp(_word[2], "x"))
		do_x = true;
	else if (!strcmp(_word[2], "y"))
		do_x = false;
	else {
		demonstrate_command_usage();
		err("Can only flip with respect to `x' or `y'");
		return false;
	}
	if (!_image.storage_exists) {
		err("No image exists yet\n");
		return false;
	}
	/*
	 * Flip the image.
	 */
	width = _image.ras_width;
	width_half = width / 2;
	height = _image.ras_height;
	height_half = height / 2;
	if (do_x) {
		//printf("flipping in x an image %d wide and %d tall\n",width,height);
		for (i = 0; i < width_half; i++)
			for (j = 0; j < height; j++) {
				int             ii = width - i - 1;
				swap = *(_image.image + i * height + j);
				*(_image.image + i * height + j) = *(_image.image + ii * height + j);
				*(_image.image + ii * height + j) = swap;
			}
	} else {
		//printf("flipping in y an image %d wide and %d tall\n",width,height);
		for (i = 0; i < width; i++)
			for (j = 0; j < height_half; j++) {
				int             jj = height - j - 1;
				swap = *(_image.image + i * height + j);
				*(_image.image + i * height + j) = *(_image.image + i * height + jj);
				*(_image.image + i* height + jj) = swap;
			}
	}
	return true;
}
