#include <string>
#include	<math.h>
#include	<stdio.h>
#include	"gr.hh"
#include	"extern.hh"

extern bool     _grWritePS;
extern FILE    *_grPS;

/*
 * gr_drawBWmaskedimage() -- draw a image of an unsigned char matrix
 * 
 * DESCRIPTION:  Draws an image using 'im', a matrix of rows*cols unsigned char
 * elements. You should assign elements to im as:
 * 
 * for(i=0;i<cols;i++) for(j=0;j<cols;j++) (im + i * jmax + j) = value_at_i_j;
 * 
 * where i=j=0 is the lower-left corner, and i=1,j=0 is just to the right of it.
 * 
 * GRAYSCALE: Normally, white paper is left for points with im==255, and black
 * ink is used for im==0. To get this mapping, do this: #include <stdio.h>
 * gr_drawBWimage(im,NULL,...); If you would like to supply an alternate
 * mapping of values of im to grayscale, you can store it in imTransform.
 * Then gr_drawBWimage() uses not 'im', but rather imTransform[im]. Here's an
 * example which reverses the mapping, so that im==0 becomes white and
 * im==255 becomes black:
 * 
 * unsigned char imTransform[256]; for (i = 0; i < 256; i++) imTransform[i] =
 * 255 - i; gr_drawBWmaskedimage_pt(...);
 * 
 * 
 * NOTE: To use the default mapping, imTransform must be supplied as NULL; to
 * arrange an alternate mapping, it must be an unsigned character vector of
 * length 256 (anything  else will produce a spurious map).
 * 
 * GEOMETRY: The image is drawn in the rectangle defined by (xl,yb) as
 * lower-left and (xr,yt) as upper-right, in user units.
 * 
 * MISSING VALUES: if the mask image is equal to 2, the supplied "missing" value
 * is used.
 * 
 */
void
gr_drawBWmaskedimage_pt(unsigned char missing,
			unsigned char *mask,
			unsigned char *im,
			unsigned char *imTransform,
			int imax, int jmax,
			double xlpt, double ybpt, double xrpt, double ytpt)
{
	register int    i, j;
	extern FILE    *_grPS;
	int             perline = 0;
	int             perlineMAX = 39;
	if (imax < perlineMAX)
		perlineMAX = imax;
	/* write postscript */
	if (_grWritePS) {
		fprintf(_grPS, "%f %f %f %f %d %d im\n",
			xlpt, ybpt, xrpt, ytpt, jmax, imax);
		check_psfile();
		if (imTransform == NULL) {
			for (j = jmax - 1; j > -1; j--) {
				for (i = 0; i < imax; i++) {
					if (mask != NULL && *(mask + i * jmax + j) == 2)
						fprintf(_grPS, "%02X", missing);
					else
						fprintf(_grPS, "%02X", *(im + i * jmax + j));
					if ((++perline) == perlineMAX) {
						fprintf(_grPS, "\n");
						perline = 0;
					}
				}
			}
			check_psfile();
		} else {
			/* scale contained in imTransform[] */
			for (j = jmax - 1; j > -1; j--) {
				for (i = 0; i < imax; i++) {
					if (mask != NULL && *(mask + i * jmax + j) == 2)
						fprintf(_grPS, "%02X", missing);
					else
						fprintf(_grPS, "%02X", imTransform[*(im + i * jmax + j)]);
					if ((++perline) == perlineMAX) {
						fprintf(_grPS, "\n");
						perline = 0;
					}
				}
			}
		}
		if (perline != 0)
			fprintf(_grPS, "\n");
		check_psfile();
	}
}

/*
 * Draw image, possibly color, in rectangle given in cm coords.
 */
void
gr_drawimage(unsigned char *im,
	     unsigned char *imTransform,
	     gr_color_model color_model,
	     unsigned char *mask,
	     double mask_r,
	     double mask_g,
	     double mask_b,
	     int imax,		// image size
	     int jmax,		// image size
	     double xl,		// image lower-left-x, in cm
	     double yb,		// image lower-left-y, in cm
	     double xr,		// image upper-right-x, in cm
	     double yt,		// image upper-right-y, in cm
	     bool insert_placer)
{
	unsigned char   cmask_r, cmask_g, cmask_b;
	bool            have_mask;
	unsigned char   value, mask_value = 0; // assign to calm compiler
	register int    i, j;
	int             perline = 0;
	int             perlineMAX;
	if (!_grWritePS)
		return;
	/* Figure out about mask */
	have_mask = (mask == NULL) ? false : true;
	// Convert cm to pt
	xl *= PT_PER_CM;
	xr *= PT_PER_CM;
	yb *= PT_PER_CM;
	yt *= PT_PER_CM;
	double xl_c = xl, xr_c = xr, yb_c = yb, yt_c = yt;
	int ilow = 0, ihigh = imax, jlow = 0, jhigh = jmax;
	if (_clipping_postscript && _clipping_is_postscript_rect) {
#ifdef __GNUC__
		gri_debug_printf(1,"image xrange (%f %f) pt\n",xl,xr);
		gri_debug_printf(1,"image yrange (%f %f) pt\n",yb,yt);
		gri_debug_printf(1,"clip xrange (%f %f) pt\n",_clip_ps_xleft,_clip_ps_xright);
		gri_debug_printf(1,"clip yrange (%f %f) pt\n",_clip_ps_ybottom,_clip_ps_ytop);
#endif
		ilow =  int(floor(0.5 + (_clip_ps_xleft   - xl)*imax/((xr-xl))));
		ihigh = int(floor(0.5 + (_clip_ps_xright  - xl)*imax/((xr-xl))));
		jlow =  int(floor(0.5 + (_clip_ps_ybottom - yb)*jmax/((yt-yb))));
		jhigh = int(floor(0.5 + (_clip_ps_ytop    - yb)*jmax/((yt-yb))));
#ifdef __GNUC__
		gri_debug_printf(1,"switching i to run from %d to %d instead of 0 to %d\n",ilow,ihigh,imax);
		gri_debug_printf(1,"switching j to run from %d to %d instead of 0 to %d\n",jlow,jhigh,jmax);
#endif
		if (ilow > 0)     xl_c = xl + ilow * (xr - xl) / imax;
		if (ihigh < imax) xr_c = xl + ihigh * (xr - xl) / imax;
		if (jlow > 0)     yb_c = yb + jlow * (yt - yb) / jmax;
		if (jhigh < jmax) yt_c = yb + jhigh * (yt - yb) / jmax;
#ifdef __GNUC__
		gri_debug_printf(1,"clipped image xrange (%f %f) pt\n",xl_c,xr_c);
		gri_debug_printf(1,"clipped image yrange (%f %f) pt\n",yb_c,yt_c);
#endif
	}
	rectangle box(xl_c/PT_PER_CM, yb_c/PT_PER_CM, xr_c/PT_PER_CM, yt_c/PT_PER_CM); // CHECK: is it only updating if it's within clip region?
	bounding_box_update(box);

	// Make image overhang the region.  This change, vsn 2.005, *finally*
	// solves a confusion I've had for a long time about how to do
	// images.
	if (imax > 1) {
		double dx = (xr_c - xl_c) / ((ihigh-ilow) - 1); // pixel width
		xl_c -= dx / 2.0;
		xr_c += dx / 2.0;
	}
	if (jmax > 1) {
		double dy = (yt_c - yb_c) / ((jhigh-jlow) - 1); // pixel height
		yb_c -= dy / 2.0;
		yt_c += dy / 2.0;
	}
#if 0				// BUG: this contradicts the one above.
	rectangle r(xl_c / PT_PER_CM,
		    yb_c / PT_PER_CM,
		    xr_c / PT_PER_CM,
		    yt_c / PT_PER_CM);
	bounding_box_update(r);
#endif
	/*
	 * Handle BW and color differently, since PostScript handles differently.
	 */
	switch (color_model) {
	default:			/* ? taken as BW */
	case bw_model:
		perlineMAX = 39;	/* use only 78 columns so more readible */
		if (imax < perlineMAX)
			perlineMAX = imax;
		check_psfile();
		/*
		 * Write map to PostScript, creating a linear one if none exists
		 */
		fprintf(_grPS, "%% Push map onto stack, then image stuff.\n");
		fprintf(_grPS, "[\n");
		if (imTransform == NULL) {
			for (i = 0; i < 256; i++) {
				fprintf(_grPS, "%.4f ", i / 255.0);
				if (!((i + 1) % 10))
					fprintf(_grPS, "\n");
			}
		} else {
			for (i = 0; i < 256; i++) {
				fprintf(_grPS, "%.4f ", imTransform[i] / 255.0);
				if (!((i + 1) % 10))
					fprintf(_grPS, "\n");
			}
		}
		fprintf(_grPS, "\n]\n");
		if (insert_placer)
			fprintf(_grPS, "%%BEGIN_IMAGE\n");	/* for grepping in ps file */
		/*
		 * Now write image.
		 */
#if 0
		fprintf(_grPS, "%f %f %f %f %d %d im\n", xl, yb, xr, yt, jmax, imax);
#else
		//printf("CASE 1a\n");
		fprintf(_grPS, "%f %f %f %f %d %d im\n", xl_c, yb_c, xr_c, yt_c, (jhigh-jlow), (ihigh-ilow)); // BUG or +1?
#endif
		if (have_mask == true) {
			int             diff, min_diff = 256;
			unsigned char   index = 0; // assign to calm compiler ????
			mask_value = (unsigned char)(255.0 * mask_r);
			/*
			 * If there is a mapping, must (arduously) look up which image
			 * value corresponds to this color.
			 */
			if (imTransform != NULL) {
				for (i = 0; i < 256; i++) {
					diff = (int) fabs(imTransform[i] - mask_value);
					if (diff < min_diff) {
						min_diff = diff;
						index = i;
					}
				}
				mask_value = index;
			}
		}
#if 0
		for (j = jmax - 1; j > -1; j--) {
			for (i = 0; i < imax; i++) {
#else
                // printf("CASE 1\n"); // HEREHEREHERE
		for (j = jhigh - 1; j >= jlow; j--) {
			for (i = ilow; i < ihigh; i++) {
#endif
				value = *(im + i * jmax + j);
				if (have_mask == true && *(mask + i * jmax + j) == 2) {
					fprintf(_grPS, "%02X", mask_value);
				} else {
					fprintf(_grPS, "%02X", value);
				}
				if ((++perline) == perlineMAX) {
					fprintf(_grPS, "\n");
					perline = 0;
				}
			}
		}
		check_psfile();
		if (perline != 0)
			fprintf(_grPS, "\n");
		check_psfile();
		if (insert_placer)
			fprintf(_grPS, "%%END_IMAGE\n");	/* for grepping in ps file */
		break;
	case rgb_model:
		perlineMAX = 13;	/* use only 78 columns so more readible */
		if (imax < perlineMAX)
			perlineMAX = imax;
		if (insert_placer)
			fprintf(_grPS, "%%BEGIN_IMAGE\n");
		fprintf(_grPS, "%f %f %f %f %d %d cim\n", xl_c, yb_c, xr_c, yt_c, (jhigh-jlow), (ihigh-ilow)); // BUG
		check_psfile();
#if 0
		if (have_mask == true) {
			warning("BUG in grimage.c - masking of color images not working yet");
		}
#endif
		cmask_r = (unsigned char)pin0_255(mask_r * 255.0);
		cmask_g = (unsigned char)pin0_255(mask_g * 255.0);
		cmask_b = (unsigned char)pin0_255(mask_b * 255.0);
		if (imTransform == NULL) {
#if 0
			for (j = jmax - 1; j > -1; j--) {
				for (i = 0; i < imax; i++) {
#else
                        for (j = jhigh - 1; j >= jlow; j--) {
				for (i = ilow; i < ihigh; i++) {
#endif
					value = *(im + i * jmax + j);
					if (have_mask == true && *(mask + i * jmax + j) == 2) {
						fprintf(_grPS, "%02X%02X%02X", cmask_r, cmask_g, cmask_b);
					} else {
						fprintf(_grPS, "%02X%02X%02X", value, value, value);
					}
					if ((++perline) == perlineMAX) {
						fprintf(_grPS, "\n");
						perline = 0;
					}
				}
			}
			check_psfile();
		} else {
#if 0
			for (j = jmax - 1; j > -1; j--) {
				for (i = 0; i < imax; i++) {
#else
                        for (j = jhigh - 1; j >= jlow; j--) {
				for (i = ilow; i < ihigh; i++) {
#endif
					value = *(im + i * jmax + j);
					if (have_mask == true && *(mask + i * jmax + j) == 2) {
						fprintf(_grPS, "%02X%02X%02X", cmask_r, cmask_g, cmask_b);
					} else {
						fprintf(_grPS, "%02X%02X%02X", imTransform[value], imTransform[value + 256], imTransform[value + 512]);
					}
					if ((++perline) == perlineMAX) {
						fprintf(_grPS, "\n");
						perline = 0;
					}
				}
			}
		}
		if (perline != 0)
			fprintf(_grPS, "\n");
		if (insert_placer)
			fprintf(_grPS, "%%END_IMAGE\n");	/* for grepping in ps file */
		check_psfile();
	}				/* switch(color_model) */
}

/*
 * gr_drawBWimage_pt() -- draw a image of an unsigned char matrix
 * 
 * SYNOPSIS void  gr_drawBWimage_pt(unsigned char *im, unsigned char
 * imTransform, imax, jmax, xl, yb, xr, yt);
 * 
 * DESCRIPTION:  As gr_drawBWimagept() except that xl, yb, xr and yt are
 * measured in points.
 * 
 */
void
gr_drawBWimage_pt(unsigned char *im,
		  unsigned char *imTransform,
		  int imax, int jmax,
		  double xlpt, double ybpt, double xrpt, double ytpt)
{
	register int    i, j;
	int             perline = 0;
	int             perlineMAX = 39;
	if (imax < perlineMAX)
		perlineMAX = imax;
	/* write postscript */
	if (_grWritePS) {
		fprintf(_grPS, "%f %f %f %f %d %d im\n",
			xlpt, ybpt, xrpt, ytpt, jmax, imax);
		check_psfile();
		if (imTransform == NULL) {
			for (j = jmax - 1; j > -1; j--) {
				for (i = 0; i < imax; i++) {
					fprintf(_grPS, "%02X", *(im + i * jmax + j));
					if ((++perline) == perlineMAX) {
						fprintf(_grPS, "\n");
						perline = 0;
					}
				}
			}
			check_psfile();
		} else {
			/* scale contained in imTransform[] */
			for (j = jmax - 1; j > -1; j--) {
				for (i = 0; i < imax; i++) {
					fprintf(_grPS, "%02X", imTransform[*(im + i * jmax + j)]);
					if ((++perline) == perlineMAX) {
						fprintf(_grPS, "\n");
						perline = 0;
					}
				}
			}
		}
		if (perline != 0)
			fprintf(_grPS, "\n");
		check_psfile();
	}
}
