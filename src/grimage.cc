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
	extern output_file_type _output_file_type;
	if (_output_file_type == postscript) {
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
	} else {
		err("INTERNAL ERROR: gr_drawBWmaskedimage() only works for postscript files.");
	}
}

void gr_drawimage_svg( // Draw image, possibly color, in rectangle given in cm coords.
                      unsigned char *im,
                      unsigned char *imTransform,
                      gr_color_model color_model,
                      unsigned char *mask,
                      double mask_r,
                      double mask_g,
                      double mask_b,
                      int imax,		// image size
                      int jmax,		// image size
                      double xl,        // image lower-left-x, in cm
                      double yb,        // image lower-left-y, in cm
                      double xr,        // image upper-right-x, in cm
                      double yt,        // image upper-right-y, in cm
                      bool insert_placer)
{
	extern FILE *_grSVG;
	unsigned char   cmask_r, cmask_g, cmask_b;
	bool            have_mask;
	unsigned char   value;
	register int    i, j;
	double x, y, page_height_pt = gr_page_height_pt();
	char hex[8]; // #000000 for example
	if (!_grWritePS)
		return;
	// Figure out about mask
	have_mask = (mask == NULL) ? false : true;
	xl *= PT_PER_CM;
	xr *= PT_PER_CM;
	yb *= PT_PER_CM;
	yt *= PT_PER_CM;
	double dx = (xr - xl) / imax; // FIXME: check to see the sign always works
	double dy = (yt - yb) / jmax;
	double xl_c = xl, xr_c = xr, yb_c = yb, yt_c = yt;
	int ilow = 0, ihigh = imax, jlow = 0, jhigh = jmax;
	if (_clipping_postscript && _clipping_is_postscript_rect) {
		ilow =  int(floor(0.5 + (_clip_ps_xleft   - xl)*imax/((xr-xl))));
		ihigh = int(floor(0.5 + (_clip_ps_xright  - xl)*imax/((xr-xl))));
		jlow =  int(floor(0.5 + (_clip_ps_ybottom - yb)*jmax/((yt-yb))));
		jhigh = int(floor(0.5 + (_clip_ps_ytop    - yb)*jmax/((yt-yb))));
		if (ihigh < ilow) {
			int tmp = ihigh;
			ihigh = ilow;
			ilow = tmp;
		}
		if (jhigh < jlow) {
			int tmp = jhigh;
			jhigh = jlow;
			jlow = tmp;
		}
		ilow = LARGER_ONE(ilow, 0);
		ihigh = SMALLER_ONE(ihigh, imax);
		jlow = LARGER_ONE(jlow, 0);
		jhigh = SMALLER_ONE(jhigh, jmax);
		if (ilow > 0)     xl_c = xl + ilow * (xr - xl) / imax;
		if (ihigh < imax) xr_c = xl + ihigh * (xr - xl) / imax;
		if (jlow > 0)     yb_c = yb + jlow * (yt - yb) / jmax;
		if (jhigh < jmax) yt_c = yb + jhigh * (yt - yb) / jmax;
                if (_chatty > 2) {
                        printf("image clipping debugging...\n");
                        printf("image xrange (%f %f) pt\n",xl,xr);
                        printf("image yrange (%f %f) pt\n",yb,yt);
                        printf("clip xrange (%f %f) pt\n",_clip_ps_xleft,_clip_ps_xright);
                        printf("clip yrange (%f %f) pt\n",_clip_ps_ybottom,_clip_ps_ytop);
                        printf("making i run from %d to %d instead of 0 to %d\n",ilow,ihigh,imax);
                        printf("making j run from %d to %d instead of 0 to %d\n",jlow,jhigh,jmax);
                        printf("clipped image xrange (%f %f) pt\n",xl_c,xr_c);
                        printf("clipped image yrange (%f %f) pt\n",yb_c,yt_c);
                }
	}
	rectangle box(xl_c/PT_PER_CM, yb_c/PT_PER_CM, xr_c/PT_PER_CM, yt_c/PT_PER_CM); // CHECK: is it only updating if it's within clip region?
	bounding_box_update(box);
	// Make image overhang the region.
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

	// Optimize a bit by using style sheet to define fixed quantities.  (I had
	// hoped to set width= and height= here, but that does not work.)
	fprintf(_grSVG, "<defs>\n  <style type=\"text/css\"><![CDATA[\n    rect {\n      stroke-width: %.2f;\n      fill-opacity:%.2f\n    } ]]>\n  </style>\n</defs>\n", 
		0.2, 1.0);	// FIXME: remove opacity, if add image transparency on a pixel-by-pixel basis.
        // Handle BW and color differently, since PostScript handles differently.
	switch (color_model) {
	default:			// taken as BW
	case bw_model:
		fprintf(_grSVG, "<g> <!-- BW image -->\n");
		err("Sorry, svg output of black/white images is not working yet");
#if 0
		check_psfile();
                // Write map to PostScript, creating a linear one if none exists
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
			fprintf(_grPS, "%%BEGIN_IMAGE\n"); // for grepping in ps file
                // Now write image.
		fprintf(_grPS, "%f %f %f %f %d %d im\n", xl_c, yb_c, xr_c, yt_c, (jhigh-jlow), (ihigh-ilow)); // BUG or +1?
		if (have_mask == true) {
			int             diff, min_diff = 256;
			unsigned char   index = 0; // assign to calm compiler ????
			unsigned char mask_value = (unsigned char)(255.0 * mask_r);
			// If there is a mapping, must (arduously) look up which image
                        // value corresponds to this color.
			if (imTransform != NULL) {
				for (i = 0; i < 256; i++) {
					diff = (int) fabs(double(imTransform[i] - mask_value));
					if (diff < min_diff) {
						min_diff = diff;
						index = i;
					}
				}
				mask_value = index;
			}
		}
		for (j = jhigh - 1; j >= jlow; j--) {
			for (i = ilow; i < ihigh; i++) {
				value = *(im + i * jmax + j);
				if (have_mask == true && *(mask + i * jmax + j) == 2) {
					fprintf(_grSVG, "<rect %02x/>\n", mask_value);
				} else {
					fprintf(_grSVG, "<rect %02X/>\n", value);
				}
			}
		}
		fprintf(_grSVG, "</g> <!-- end of BW image -->\n");
#endif
		break;
	case rgb_model:
		group_start("RGB image");
		// printf("DEBUG: ilow, ihigh = %d %d      jlow, jhigh = %d %d\n",ilow,ihigh,jlow,jhigh);
		cmask_r = (unsigned char)pin0_255(mask_r * 255.0);
		cmask_g = (unsigned char)pin0_255(mask_g * 255.0);
		cmask_b = (unsigned char)pin0_255(mask_b * 255.0);
		double adx = fabs(dx), ady = fabs(dy);
		char label[100];
		if (imTransform == NULL) {
			err("cannot handle SVG images that lack an image-transform.");
                        for (j = jhigh - 1; j >= jlow; j--) {
				y = page_height_pt - (yb + (jhigh - j) * dy); // offset for page_height_pt ??
				for (i = ilow; i < ihigh; i++) {
					x = xl + i * dx;
					value = *(im + i * jmax + j);
					if (have_mask == true && *(mask + i * jmax + j) == 2)
						sprintf(hex, "#%02X%02X%02X", cmask_r, cmask_g, cmask_b);
					else
						sprintf(hex, "#%02X%02X%02X", value, value, value);
					fprintf(_grSVG, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" style=\"fill:%s;stroke:%s;\"/>\n", x, y, adx, ady, hex, hex);
				}
			}
		} else {
                        for (j = jhigh - 1; j >= jlow; j--) {
				//y = page_height_pt - (yb + (jhigh - j) * dy);
				y = page_height_pt - (yb + j * dy);
				sprintf(label, "j=%d", j);
				group_start(label);
				for (i = ilow; i < ihigh; i++) {
					x = xl + i * dx;
					value = *(im + i * jmax + j);
					if (have_mask == true && *(mask + i * jmax + j) == 2) {
						sprintf(hex, "#%02X%02X%02X", cmask_r, cmask_g, cmask_b);
						fprintf(_grSVG, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" style=\"fill:%s;stroke:%s;\"/>\n", x, y, adx, ady, hex, hex);
					} else {
						sprintf(hex, "#%02X%02X%02X", 
							imTransform[value], imTransform[value + 256], imTransform[value + 512]);
						fprintf(_grSVG, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" style=\"fill:%s;stroke:%s;\"/>\n", x, y, adx, ady, hex, hex);
					}
				}
				group_end();
			}
		}
		group_end();
	}				// switch(color_model)
}


void gr_drawimage( // Draw image, possibly color, in rectangle given in cm coords.
                  unsigned char *im,
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
	//printf("DEBUG [gr_drawimage() %s:%d] xl=%lf  xr=%lf  yb=%lf  yt=%lf\n",__FILE__,__LINE__,xl,xr,yb,yt);
	// Figure out about mask
	have_mask = (mask == NULL) ? false : true;
	xl *= PT_PER_CM;
	xr *= PT_PER_CM;
	yb *= PT_PER_CM;
	yt *= PT_PER_CM;
	double xl_c = xl, xr_c = xr, yb_c = yb, yt_c = yt;
	int ilow = 0, ihigh = imax, jlow = 0, jhigh = jmax;
	if (_clipping_postscript && _clipping_is_postscript_rect) {
		ilow =  int(floor(0.5 + (_clip_ps_xleft   - xl)*imax/((xr-xl))));
		ihigh = int(floor(0.5 + (_clip_ps_xright  - xl)*imax/((xr-xl))));
		jlow =  int(floor(0.5 + (_clip_ps_ybottom - yb)*jmax/((yt-yb))));
		jhigh = int(floor(0.5 + (_clip_ps_ytop    - yb)*jmax/((yt-yb))));
                if (jhigh > jmax) jhigh = jmax;
                if (_chatty > 1)
                        printf("clipping postscript to rect %f < x < %f and %f < y < %f\n",
                               _clip_ps_xleft, _clip_ps_xright,
                               _clip_ps_ybottom, _clip_ps_ytop);

		if (ihigh < ilow) {
                        if (_chatty > 2) printf("Interchanging ihigh (orig. %d) and ilow (orig %d)\n", ihigh, ilow);
			int tmp = ihigh;
			ihigh = ilow;
			ilow = tmp;
		}
		if (jhigh < jlow) {
                        if (_chatty > 2) printf("Interchanging jhigh (orig. %d) and jlow (orig %d)\n", jhigh, jlow);
			int tmp = jhigh;
			jhigh = jlow;
			jlow = tmp;
		}
		ilow = LARGER_ONE(ilow, 0);
		ihigh = SMALLER_ONE(ihigh, imax);
		jlow = LARGER_ONE(jlow, 0);
		jhigh = SMALLER_ONE(jhigh, jmax);
		if (ilow > 0)     xl_c = xl + ilow * (xr - xl) / imax;
		if (ihigh < imax) xr_c = xl + ihigh * (xr - xl) / imax;
		if (jlow > 0)     yb_c = yb + jlow * (yt - yb) / jmax;
		if (jhigh < jmax) yt_c = yb + jhigh * (yt - yb) / jmax;
                if (_chatty > 2) {
                        printf("image clipping debugging...\n");
                        printf("image xrange (%f %f) pt\n",xl,xr);
                        printf("image yrange (%f %f) pt\n",yb,yt);
                        printf("clip xrange (%f %f) pt\n",_clip_ps_xleft,_clip_ps_xright);
                        printf("clip yrange (%f %f) pt\n",_clip_ps_ybottom,_clip_ps_ytop);
                        printf("making i run from %d to %d instead of 0 to %d\n",ilow,ihigh,imax);
                        printf("making j run from %d to %d instead of 0 to %d\n",jlow,jhigh,jmax);
                        printf("clipped image xrange (%f %f) pt\n",xl_c,xr_c);
                        printf("clipped image yrange (%f %f) pt\n",yb_c,yt_c);
                }
	}
	rectangle box(xl_c/PT_PER_CM, yb_c/PT_PER_CM, xr_c/PT_PER_CM, yt_c/PT_PER_CM); // CHECK: is it only updating if it's within clip region?
	bounding_box_update(box);
	// Make image overhang the region.
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
        // Handle BW and color differently, since PostScript handles differently.
	switch (color_model) {
	default:                // taken as BW
	case bw_model:
		perlineMAX = 39; // use only 78 columns so more readible
		if (imax < perlineMAX)
			perlineMAX = imax;
		check_psfile();
                // Write map to PostScript, creating a linear one if none exists
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
			fprintf(_grPS, "%%BEGIN_IMAGE\n"); // for grepping in ps file
                // Now write image.
		//printf("CASE 1a\n");
		fprintf(_grPS, "%f %f %f %f %d %d im\n", xl_c, yb_c, xr_c, yt_c, (jhigh-jlow), (ihigh-ilow)); // BUG or +1?
		if (have_mask == true) {
			int             diff, min_diff = 256;
			unsigned char   index = 0; // assign to calm compiler ????
			mask_value = (unsigned char)(255.0 * mask_r);
                        // If there is a mapping, must (arduously) look up which image
                        // value corresponds to this color.
			if (imTransform != NULL) {
				for (i = 0; i < 256; i++) {
					diff = (int) fabs(double(imTransform[i] - mask_value));
					if (diff < min_diff) {
						min_diff = diff;
						index = i;
					}
				}
				mask_value = index;
			}
		}
		for (j = jhigh - 1; j >= jlow; j--) {
			for (i = ilow; i < ihigh; i++) {
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
			fprintf(_grPS, "%%END_IMAGE\n"); // for grepping in ps file
		break;
	case rgb_model:
		perlineMAX = 13;	// use only 78 columns so more readible
		if (imax < perlineMAX)
			perlineMAX = imax;
		if (insert_placer)
			fprintf(_grPS, "%%BEGIN_IMAGE\n");
		fprintf(_grPS, "%f %f %f %f %d %d cim\n", xl_c, yb_c, xr_c, yt_c, (jhigh-jlow), (ihigh-ilow)); // BUG
                // printf("DEBUG: ilow, ihigh = %d %d      jlow, jhigh = %d %d\n",ilow,ihigh,jlow,jhigh);
                
		check_psfile();
		cmask_r = (unsigned char)pin0_255(mask_r * 255.0);
		cmask_g = (unsigned char)pin0_255(mask_g * 255.0);
		cmask_b = (unsigned char)pin0_255(mask_b * 255.0);
		if (imTransform == NULL) {
                        for (j = jhigh - 1; j >= jlow; j--) {
                                for (i = ilow; i < ihigh; i++) {
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
                        for (j = jhigh - 1; j >= jlow; j--) { // BUG: should these run on max, or on 'ihigh' etc???
				for (i = ilow; i < ihigh; i++) {
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
			fprintf(_grPS, "%%END_IMAGE\n"); // for grepping in ps file
		check_psfile();
                        } // switch(color_model)
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
