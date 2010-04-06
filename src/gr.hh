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

// gr.h  -- header file for gr and gri (Copyright 1993 Dan Kelley) You must
// #include "gr.h" at start of any C program which uses the gr library.

#ifndef _grh_
#define         _grh_

#include <string>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
// For getpid etc.
#if HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

// Possibly include debugging malloc header file.
#ifdef USE_DBMALLOC
#ifdef __linux__
#include        <malloc.h>
#else
#include        <dbmalloc.h>
#endif
#endif

// Standard libraries, malloc, etc (this confuses me)
#if STDC_HEADERS
#include        <stdlib.h>
#else
#if !defined(MSDOS) && !defined(IS_FREEBSD)
#include        <malloc.h>
#endif
#endif				// Doesn't have stdlib

// Should not really need these, but on gcc 2.5.8 on my sparc
// machine, they are not prototyped.  I only prototyping them 
// for GNU cc because e.g. alpha compiler chokes on a 
// disagreement with /usr/include/stdlib.h
#if 0                           // removed 1999-mar-07 to port to AIX
#if defined(__GNUC__)
extern "C" {
	int  pclose(FILE *stream);
}
#endif
#endif


#include "types.hh"
#include "gr_coll.hh"
#include "GriColor.hh"
#include "GMatrix.hh"

// Useful things to know.
#define	_grTempStringLEN	32768 // = 2^15  (was 4096 until 2001-feb-17)
#define PASTE_CHAR    char(255)

// output file type
enum output_file_type {
	postscript = 0,
	svg = 1,
	gif = 2
};
// Geometrical things.
#define PI_VALUE 3.14159265358979323846
// Postscript things.
#define GR_POINTS_IN_PS_PATH 1499 // really 1500, but be safe
#define PT_PER_IN 72.27		// points per inch
#define PT_PER_CM 28.45		// points per centimetre
#define CM_PER_IN 2.54		// BUG: more digits?
#define DEG_PER_RAD 57.29577951	// degrees per radian

// An io buffer that resorts to file i/o only if a buffer is exhausted. Used
// by gr_textget() and gr_textsave().
typedef struct {
	FILE           *fp;	// file to read if buffer exhausted
	char           *buf;	// the buffer
	int             buf_capacity; // chars in buffer
	int             buf_position; // next char to read in buffer
}               FBUFFER;



// Axis properties
typedef enum {
	gr_axis_BOTTOM = 1,
	gr_axis_TOP = 0,
	gr_axis_LEFT = 1,
	gr_axis_RIGHT = 0,
	gr_axis_LOG = 0,
	gr_axis_LINEAR = 1
} gr_axis_properties;

typedef struct {
	double width_pt;	// width of pen
} gr_pen;

// Font numbers.  To add new fonts, use 'extract_font_widths' perlscript to
// get size info, then incorporate into grstring.c, in a list before the
// gr_charwidth_cm() function.
// DO NOT change the =0 below, or you'll mess everything up in grstring (esp in
// the gr_drawstring() function).
typedef enum {
	gr_font_Courier = 0,	// Courier
	gr_font_CourierOblique,	// Courier-Oblique
	gr_font_CourierBold,	// Courier-Bold
	gr_font_CourierBoldOblique, // Courier-BoldOblique
	gr_font_Helvetica,	// Helvetica
	gr_font_HelveticaOblique, // Helvetica-Oblique
	gr_font_HelveticaBold,	// Helvetica-Bold
	gr_font_HelveticaBoldOblique,	// Helvetica-BoldOblique
	gr_font_PalatinoRoman,	// Palatino-Roman
	gr_font_PalatinoItalic,	// Palatino-Italic
	gr_font_PalatinoBold,	// Palatino-Bold
	gr_font_PalatinoBoldItalic, // Palatino-BoldItalic
	gr_font_Symbol,		// Symbol
	gr_font_Century,    // called NewCenturySchoolbook sometimes (I guess)
	gr_font_TimesRoman,	// Times-Roman
	gr_font_TimesItalic,	// Times-Italic
	gr_font_TimesBold,	// Times-Bold
	gr_font_TimesBoldItalic, // Times-BoldItalic
	gr_font_end_of_list
} gr_fontID;


typedef struct {
	gr_fontID       id;
	char           *name;
} gr_font_info;	// used grstring.c:63 for font_list
enum gr_font_encoding { font_encoding_standard, font_encoding_isolatin1};
typedef struct {
	gr_fontID        id;
	gr_font_encoding encoding;
	double           size_pt;
} gr_font;

// Symbol numbers
enum gr_symbol_type {
	gr_unknown_symbol = -1,
	gr_plus_symbol,
	gr_times_symbol,
	gr_box_symbol,
	gr_circ_symbol,
	gr_diamond_symbol,
	gr_triangleup_symbol,
	gr_triangleright_symbol,
	gr_triangledown_symbol,
	gr_triangleleft_symbol,
	gr_asterisk_symbol,
	gr_star_symbol,
	gr_filledbox_symbol,
	gr_bullet_symbol,
	gr_filleddiamond_symbol,
	gr_filledtriangleup_symbol,
	gr_filledtriangleright_symbol,
	gr_filledtriangledown_symbol,
	gr_filledtriangleleft_symbol,
	gr_filledhalfmoonup_symbol,
	gr_filledhalfmoondown_symbol
};

// Color.  Is this used??
typedef enum {
	bw_model, rgb_model, hsb_model
}               gr_color_model;

typedef struct {
	double          red;
	double          green;
	double          blue;
	double          hue;
	double          saturation;
	double          brightness;
	gr_color_model  color_model;
}               gr_color;


// Function headers.
void            abort_gri(void);
bool            delete_ps_file(void);
void            gr_begin(int specifications);
bool            gr_buffgets(char *s, unsigned int most, FBUFFER * fbuf);
void            gr_cmtouser(double x_cm, double y_cm, double *x, double *y);
void            gr_comment(const char *message);
void            gr_contour(const double x[],
			   const double y[],
			   /*const*/ GriMatrix<double> &z,
			   /*const*/ GriMatrix<bool> &legit,
			   int nx,
			   int ny,
			   double z0,
			   const char *lab,
			   bool rotate_labels,
			   bool whiteunder_labels,
			   bool center_labels,
			   const GriColor& line_color,
			   const GriColor& text_color,
			   double contour_minlength,
			   double contour_space_first,
			   double contour_space_later,
			   FILE * out_file);
gr_fontID       gr_currentfont(void);
gr_font_encoding gr_current_font_encoding();
double          gr_currentCapHeight_cm(void);	// From font metric info
double          gr_currentXHeight_cm(void);	// From font metric info
double          gr_currentAscender_cm(void);	// From font metric info
double          gr_currentDesscender_cm(void);	// From font metric info
double          gr_currentfontsize_pt(void);
#define gr_currentfontsize_cm() (gr_currentfontsize_pt() / PT_PER_CM)
double          gr_currentmissingvalue(void);
char           *gr_currentPSfilename(void);
FILE           *gr_currentPSFILEpointer(void);
bool            gr_current_ps_landscape(void);
double          gr_currentsymbolsize_pt(void);
#define gr_currentsymbolsize_cm() (gr_currentsymbolsize_pt() / PT_PER_CM)
double          gr_currentticsize_cm(void);
void            gr_drawimage_svg(unsigned char *im, unsigned char *imTransform, gr_color_model color_model, unsigned char *mask, double mask_r, double maskg, double mask_b, int imax, int jmax, double llx_cm, double lly_cm, double urx_cm, double ury_cm, bool insert_placer);
void            gr_drawimage(unsigned char *im, unsigned char *imTransform, gr_color_model color_model, unsigned char *mask, double mask_r, double maskg, double mask_b, int imax, int jmax, double llx_cm, double lly_cm, double urx_cm, double ury_cm, bool insert_placer);
void            gr_draw_arc_cm(bool filled, double xc, double yc, double r, double angle1, double angle2);
void            gr_drawarrow_cm(double x, double y, double xend, double yend, double halfwidth);
void            gr_drawarrow2_cm(double x, double y, double xend, double yend, double halfwidth);
void            gr_drawarrow3_cm(double x, double y, double xend, double yend, double halfwidth);
void            gr_drawerrorbars(double x, double xmin, double xmax, double y, double ymin, double ymax, int type);
void            gr_drawsymbol(double xcm, double ycm, gr_symbol_type symbol_name);
void            gr_drawxaxis(double y, double xl, double xinc, double xr, double xlabeling, gr_axis_properties side);
void            gr_drawxyaxes(double xl, double xinc, double xr, double yb, double yinc, double yt); // FIXME need the starts here
void            gr_drawyaxis(double x, double yb, double yinc, double yt, double ylabelling, gr_axis_properties side);
void            gr_end(const char *filename);
void            gr_error(const char *lab);
//
// Gridding routines.
int             gr_grid1(const std::vector<double> &x,
			 const std::vector<double> &y,
			 const std::vector<double> &f,
			 double x0,
			 double y0,
			 double xRadius,
			 double yRadius,
			 int method,
			 unsigned int neighbors,
			 int enlargements,
			 double *fOut);


void            gr_hsv2rgb(double h, double s, double br, double *r, double *g, double *b);
void            gr_lastxy(double *x, double *y);
bool            gr_missing(double x);
bool            gr_missingx(double x);
bool            gr_missingy(double y);
void            gr_moveto_cm(double x_cm, double y_cm);
bool            gr_multiple(double x, double d, double precision);
bool            gr_onxpage_cm(double x_cm);
bool            gr_onypage_cm(double y_cm);
double          gr_quad_cm();	// width of "M"
void            gr_record_handle(double x_cm, double y_cm);
void            gr_record_scale(void);
bool            gr_reopen_postscript(const char *new_name);
void            gr_rgb2cmyk(double R[], double G[], double B[], unsigned int n,	double c[], double m[], double y[], double k[]);
void            gr_rgb2hsb(double r, double g, double b, double *h, double *s, double *br);
void            gr_rmoveto_cm(double rx_cm, double ry_cm);
void            gr_rmoveto_pt(double rx_pt, double ry_pt);
void            gr_rotate_xy(double x, double y, double angle, double *xx, double *yy);
void            gr_save_postscript(const char *PS_name, int normal_scale);
void            gr_scale125(double xl, double xr, int n, double *xlr, double *xrr, int *nr);
// Routines to set various things
void            gr_set_clip_ps_curve(const double *xc, const double *yc, unsigned int len);
void            gr_set_clip_ps_rect(double ll_x_pt, double ll_y_pt, double ur_x_pt, double ur_y_pt);
void            gr_set_clip_ps_off();
bool            gr_set_dash(std::vector<double> dash);
void            gr_setfont(gr_fontID newID, bool force = false);
#define gr_setfontsize_cm(f) (gr_setfontsize_pt(f * PT_PER_CM))
void            gr_setfontsize_pt(double new_fontsize_pt);
void            gr_set_font_encoding(gr_font_encoding encoding);
bool            gr_using_missing_value();
void            gr_set_missing_value(double missingvalue);
void            gr_set_missing_value_none();
void            gr_setscale(double xfactor, double yfactor);
#define gr_setsymbolsize_cm(s) (gr_setsymbolsize_pt(s * PT_PER_CM))
void            gr_setsymbolsize_pt(double size_pt);
void            gr_setticdirection(bool tics_point_in);
void            gr_setticsize_cm(double newsize_cm);
void            gr_settranslate(double xcm, double ycm);
void            gr_setup_creatorname(const char *s);
void            gr_setup_ps_filename(const char *new_name);
void            gr_setup_ps_landscape(void);
void            gr_setup_ps_portrait(void);
void            gr_setup_ps_scale(double xfactor, double yfactor);
void            gr_setxaxisstyle(int xstyle);
void            gr_setxlabel(const char *xlab);
void            gr_setxnumberformat(const char *xformat);
void            gr_setxpagesize_cm(double x_cm);
void            gr_setxscale(double xl_cm, double xr_cm, double xl, double xr);
void            gr_setxsubdivisions(int num);
void            gr_setxtransform(gr_axis_properties xstyle);
void            gr_setyaxisstyle(int ystyle);
void            gr_setylabel(const char *ylab);
void            gr_setynumberformat(const char *yformat);
void            gr_setypagesize_cm(double y_cm);
void            gr_setyscale(double yb_cm, double yt_cm, double yb, double yt);
void            gr_setysubdivisions(int num);
void            gr_setytransform(gr_axis_properties ystyle);
void            gr_show_at(/*const*/ char *lab, double xcm, double ycm, gr_textStyle style, double angle);
void            gr_show_in_box(/*const */GriString &s, const GriColor& text_color, const GriColor& box_color, double x, double y, double angle_deg);
void            gr_showpage(void);
bool            gr_smootharray(double dx, double dy, double dt, GriMatrix<double> &z, GriMatrix<double> &zS, GriMatrix<bool> &legit, GriMatrix<bool> &legitS, int nx, int ny, int method);
void            gr_stringwidth(const char *s, double *x_cm, double *ascent_cm, double *descent_cm);
bool            gr_textget(char *s, int max);
bool            gr_textsave(const char *s);
void            gr_textput(const char *s);
double          gr_thinspace_cm();	// 1/6 width of "M"
void            gr_usertocm(double x, double y, double *x_cm, double *y_cm);
double          gr_usertocm_x(double x, double y);
double          gr_usertocm_y(double x, double y);
void            gr_usertopt(double x, double y, double *x_pt, double *y_pt);

// Equation of state
double rho(double S, double T, double p);
double pot_temp(double S, double t, double p, double pref);
double lapse_rate(double S, double t, double p);


// Macros

// Pin number to a range
#if !defined(pin0_1)
#define pin0_1(x)  	((x) < 0 ? 0 : ((x) <   1 ? (x) :   1))
#endif
#if !defined(pin0_255)
#define pin0_255(x)	((x) < 0 ? 0 : ((x) < 255 ? (x) : 255))
#endif

// Allocate storage, printing file/line if cannot
#if !defined(GET_STORAGE)
#define GET_STORAGE(var, type, num)					\
{									\
    if ((num) > 0) {							\
	if (! ((var) = ( type *) malloc( (num) * sizeof(type)))) {	\
	    gr_Error("Out of memory");					\
	}								\
    } else {								\
	gr_Error("Cannot allocate zero or fewer bytes of storage");	\
    }									\
}
#endif

#define interpolate_linear(x,  x0,  y0,  x1,  y1)                       \
    ((y0) + ((x) - (x0)) * ((y1) - (y0)) / ((x1) - (x0)))


// Take care of the fact that the standard c++ library CHANGED the name
// of the string::remove to string::erase, as evidenced by the change
// in g++ from versions 2.7.x to 2.8.x (early 1998).

#if defined(HAVE_OLD_STRING)
#define STRINGERASE remove
#define STRING_NPOS NPOS
#else
#define STRINGERASE erase
#define STRING_NPOS std::string::npos
#endif

// Time type (time_t, int, or long) varies per machine (ug).
#if defined(VMS) || defined(MSDOS) || defined(IS_DEC_ALPHA) || defined(AIX) || defined(IS_FREEBSD) || defined(IS_NETBSD) || defined(IS_OPENBSD)
#define SECOND_TYPE time_t
#else
#if defined(__DECCXX)
#define SECOND_TYPE int
#else
#define SECOND_TYPE long
#endif
#endif


#endif				// not _gr_
