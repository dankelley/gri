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
