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

/* declare some extern things, which are defined in image.c */
#if !defined(_image_extern_h_)
#define         _image_extern_h_
#include "gr.hh"
extern double   _image0, _image255;	/* uservalue <-> [0,255] scale */
extern double   _image_missing_color_red;	/* for missing data */
extern double   _image_missing_color_green;	/* for missing data */
extern double   _image_missing_color_blue;	/* for missing data */
extern double   _image_llx, _image_lly, _image_urx, _image_ury;	/* coords */
extern double  *_imageHist;
extern gr_color_model _image_color_model;
extern bool     _imageTransform_exists;
extern bool     _imageHist_exists;
extern IMAGE    _image, _imageMask;
extern unsigned char *_imageTransform;

#endif				/* not _image_extern_h_ */
