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
#include "gr.hh"
#include "extern.hh"
#include "image_ex.hh"

bool            maskCmd(void);

/* `mask image [to {uservalue .u.}|{imagevalue .i.}]' */
bool
maskCmd()
{
	if (!_image.storage_exists) {
		demonstrate_command_usage();
		err("First `read image' or `convert grid to image'");
		return false;
	}
	if (_imageMask.ras_width < 1) {
		demonstrate_command_usage();
		err("No image mask exists yet");
		return false;
	}
	if (!_imageTransform_exists) {
		demonstrate_command_usage();
		err("First `set image grayscale'");
		return false;
	}
	unsigned int    i, j;
	unsigned int    width, height;
	unsigned char   replacement;
	int             imagevalue;
	double          uservalue;
	/* `mask image [to {uservalue .u.}|{imagevalue .i.}]' */
	switch (_nword) {
	case 2:
		/* `mask image */
		replacement = 0;
		break;
	case 5:
		/* `mask image [to {uservalue .u.}|{imagevalue .i.}]' */
		if (!word_is(2, "to")) {
			demonstrate_command_usage();
			err("Third word must be `to'");
			return false;
		}
		if (word_is(3, "uservalue")) {
			if (!getdnum(_word[4], &uservalue)) {
				demonstrate_command_usage();
				err("Can't read .uservalue.");
				return false;
			}
			replacement = value_to_image(uservalue);
		} else if (word_is(3, "imagevalue")) {
			if (!getinum(_word[4], &imagevalue)) {
				demonstrate_command_usage();
				err("Can't read .imagevalue.");
				return false;
			}
			if (imagevalue < 0 || imagevalue > 255) {
				demonstrate_command_usage();
				err("Require .imagevalue. to be in range 0-255 inclusive");
				return false;
			}
			replacement = imagevalue;
		} else {
			demonstrate_command_usage();
			err("Fourth word must be `uservalue' or `imagevalue'");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	/* replace pixel by pixel */
	height = _image.ras_height;
	width = _image.ras_width;
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (*(_imageMask.image + i * height + j) == 1) {
				*(_image.image + i * height + j) = replacement;
			}
		}
	}
	return true;
}
