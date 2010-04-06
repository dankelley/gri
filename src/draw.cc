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
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "gr.hh"
#include "extern.hh"
#include "defaults.hh"
#include "types.hh"
#include "image_ex.hh"
#include "GriPath.hh"
#include "GriTimer.hh"

double          _contour_space_first = -1.0;     // <-> set.c
double          _contour_space_later = -1.0;	 // <-> set.c
bool            _contour_space_centered = false; // <-> set.c
double          _contour_minlength = 0.0;	 // <-> lines must exceed this

extern char     _grTempString[];
#define OFFSET_AFTER_TITLE 1.0	// title is this many cm above plot 

gr_symbol_type  determine_symbol_code(const char * s);
bool            draw_symbolCmd(void);
bool            draw_circleCmd(void);
bool            draw_contourCmd(void);
bool            draw_arcCmd(void);
bool            draw_arrow_from_toCmd(void);
bool            draw_arrowsCmd(void);
bool            draw_axesCmd(void);
bool            draw_curveCmd(void);
static bool     draw_curve_filled_to_valueCmd(bool to_y, double value);
bool            draw_image_gridCmd(void);
bool            draw_image_histogramCmd(void);
bool            draw_image_paletteCmd(void);
bool            draw_imageCmd(void);
bool            draw_labelCmd(void);
bool            draw_line_from_toCmd(void);
bool            draw_patchesCmd(void);
bool            draw_polygonCmd(void);
bool            draw_titleCmd(void);
bool            draw_valuesCmd(void);
bool            draw_x_axisCmd(void);
bool            draw_y_axisCmd(void);
bool            draw_x_box_plotCmd(void);
bool            draw_y_box_plotCmd(void);
bool            draw_zero_lineCmd(void);
bool            draw_zeroline_horizontally(void);
bool            draw_zeroline_vertically(void);
bool            set_x_scale(void);

void
set_line_width_symbol()
{
	double linewidthsymbol = LINEWIDTHSYMBOL_DEFAULT;
	if (!get_var("..linewidthsymbol..", &linewidthsymbol))
		warning("Sorry ..linewidthsymbol.. undefined so using default");
	_griState.set_linewidth_symbol(linewidthsymbol);
	if (_output_file_type == postscript) {
		extern FILE *_grPS;
		fprintf(_grPS, "%.3f w\n", _griState.linewidth_symbol());
	}
}

void
set_line_width_curve()
{
	double linewidth = LINEWIDTH_DEFAULT;
	if (!get_var("..linewidth..", &linewidth))
		warning("Sorry ..linewidth.. undefined so using default");
	_griState.set_linewidth_line(linewidth);
	if (_output_file_type == postscript) {
		extern FILE *_grPS;
		fprintf(_grPS, "%.3f w\n", _griState.linewidth_line());
	}
}

void
set_line_width_axis()
{
	double linewidthaxis = LINEWIDTHAXIS_DEFAULT;
	if (!get_var("..linewidthaxis..", &linewidthaxis))
		warning("Sorry ..linewidthaxis.. undefined so using default");
	_griState.set_linewidth_axis(linewidthaxis);
}

void
no_scales_error()
{
	err("Either x or y scale not defined.  Try using `set x axis' or `set y axis'.");
}

// `draw box .xleft. .ybottom. .xright. .ytop. [cm|pt]'
bool
draw_boxCmd()
{
	static GriPath p(5);	// static since might be done often
	p.clear();
	double          llx, lly, urx, ury;
	if (4 == get_cmd_values(_word, _nword, "box", 4, _dstack)) {
		llx = _dstack[0];
		lly = _dstack[1];
		urx = _dstack[2];
		ury = _dstack[3];
	} else {
		demonstrate_command_usage();
		READ_WORD_ERROR("(llx, lly, urx, ury)");
		return false;
	}
	if (_nword == 6) {
		// Coordinates in user units
		set_x_scale();
		set_y_scale();
		set_environment();
		p.push_back(llx, lly, 'm');
		p.push_back(urx, lly, 'l');
		p.push_back(urx, ury, 'l');
		p.push_back(llx, ury, 'l');
		p.push_back(llx, lly, 'l');
		p.stroke(units_user);
		return true;
	} else if (_nword == 7) {
		// Require `cm' or 'pt' keyword to be present
		if (!word_is(6, "cm") && !word_is(6, "pt")) {
			demonstrate_command_usage();
			MISSING_WORD_ERROR("cm or pt");
			return false;
		}
		set_environment();
		p.push_back(llx, lly, 'm');
		p.push_back(urx, lly, 'l');
		p.push_back(urx, ury, 'l');
		p.push_back(llx, ury, 'l');
		p.push_back(llx, lly, 'l');
		if (word_is(6, "pt")) {
			p.stroke(units_pt);			
		} else if (word_is(6, "cm")) {
			p.stroke(units_cm);
		} else {
			demonstrate_command_usage();
			MISSING_WORD_ERROR("cm or pt");
			return false;
		}
		return true;
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
}

// `draw box filled .xleft. .ybottom. .xright. .ytop. [cm|pt]'
bool
draw_box_filledCmd()
{
	static GriPath p(5);	// static since might be done often
	p.clear();
	double          llx, lly, urx, ury;
	if (4 == get_cmd_values(_word, _nword, "filled", 4, _dstack)) {
		llx = _dstack[0];
		lly = _dstack[1];
		urx = _dstack[2];
		ury = _dstack[3];
	} else {
		READ_WORD_ERROR("(llx, lly, urx, ury)");
		demonstrate_command_usage();
		return false;
	}
	if (_nword == 7) {
		// Coordinates in user units
		set_x_scale();
		set_y_scale();
		set_environment();
		p.push_back(llx, lly, 'm');
		p.push_back(urx, lly, 'l');
		p.push_back(urx, ury, 'l');
		p.push_back(llx, ury, 'l');
		p.push_back(llx, lly, 'l');
		p.fill(units_user);
	} else if (_nword == 8) {
		// Require `cm' or 'pt' keyword to be present
		if (!word_is(7, "cm") && !word_is(7, "pt")) {
			demonstrate_command_usage();
			MISSING_WORD_ERROR("cm or pt");
			return false;
		}
		set_environment();
		p.push_back(llx, lly, 'm');
		p.push_back(urx, lly, 'l');
		p.push_back(urx, ury, 'l');
		p.push_back(llx, ury, 'l');
		p.push_back(llx, lly, 'l');
		if (word_is(7, "pt")) {
			p.fill(units_pt);			
		} else if (word_is(7, "cm")) {
			p.fill(units_cm);
		} else {
			demonstrate_command_usage();
			MISSING_WORD_ERROR("cm or pt");
			return false;
		}
	}
	return true;
}

// `draw symbol .code.|\name at .x. .y. [cm|pt]'
// `draw symbol [[.code.|\name] [color hue z|.h. [brightness z|.b.] [saturation
// z|.s.]]]'
// `draw symbol [[.code.|\name] [graylevel z]'
bool
draw_symbolCmd()
{
	unsigned int    num_drawn = 0;
	double          x, y;
	bool            fixedSymbol = true;
	bool            old = _ignore_error;
	bool            uses_color = false;
	bool            hue_in_z = false, saturation_in_z = false, brightness_in_z = false;
	double          hue = 1.0, saturation = 1.0, brightness = 1.0;
	gr_symbol_type  symbolCode = gr_bullet_symbol; // will be changed

	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}

	// Columns must exist, if not the "at" style, which is for a single point
	if (!word_is(3, "at")) {
		if (!_columns_exist) {
			warning("`draw symbol' noticed that no column data exist.");
			return false;
		}
	}
	// Scales must be defined unless symbol location given in cm or pt
	if (!word_is(_nword - 1, "cm") && !word_is(_nword - 1, "pt")) {
		if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
			no_scales_error();
			return false;
#else
			create_x_scale();
			create_y_scale();
#endif
		}
	}
	// Is it brightness in z?
	bool uses_graylevel = false;
	if (word_is(2, "graylevel")) { // draw symbol graylevel z
		if (!word_is(3, "z")) {
			err("Word following 'graylevel' must be 'z'");
			demonstrate_command_usage();
			return false;
		}
		if (_nword != 4) {
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		uses_graylevel = true;
	} else if (word_is(3, "graylevel")) { // draw symbol NAME graylevel z
		if (!word_is(4, "z")) {
			err("Word following 'graylevel' must be 'z'");
			demonstrate_command_usage();
			return false;
		}
		if (_nword != 5) {
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		uses_graylevel = true;
		if (gr_unknown_symbol == (symbolCode = determine_symbol_code(_word[2]))) {
			demonstrate_command_usage();
			err("Can't understand symbol \\`", _word[2], "'", "\\");
			return false;
		}
		fixedSymbol = true;
	}
	// If it's color, extract hue/saturation/brightness
	if (!uses_graylevel) {
		if (word_is(2, "color") || word_is(2, "colour") || word_is(3, "color") || word_is(3, "colour")) {
			bool            OLD = _ignore_error;
			_ignore_error = true;	// can't read "z" in "hue z" as number
			if (1 == get_cmd_values(_word, _nword, "saturation", 1, _dstack))
				saturation = _dstack[0];
			if (1 == get_cmd_values(_word, _nword, "brightness", 1, _dstack))
				brightness = _dstack[0];
			if (1 == get_cmd_values(_word, _nword, "hue", 1, _dstack))
				hue = _dstack[0];
			// Figure if anything coded into z column
			for (unsigned int i = 2; i < _nword; i++) {
				if (word_is(i, "z")) {
					if (word_is(i - 1, "brightness"))
						brightness_in_z = true;
					else if (word_is(i - 1, "saturation"))
						saturation_in_z = true;
					else if (word_is(i - 1, "hue")) {
						hue_in_z = true;
					} else {
						err("Unexpected word preceding word `z'");
						return false;
					}
				}
			}
			_ignore_error = OLD;
			uses_color = true;
			// Get symbol type if given 
			if (word_is(3, "color") || word_is(3, "colour")) {
				if (gr_unknown_symbol == (symbolCode = determine_symbol_code(_word[2]))) {
					demonstrate_command_usage();
					err("Can't understand symbol \\`", _word[2], "'", "\\");
					return false;
				}
				fixedSymbol = true;
			}
		} else {
			// Not color
			switch (_nword) {
			case 2:
				// `draw symbol' 
				if (_colZ.size() <= 0) {
					fixedSymbol = true;
					symbolCode = gr_times_symbol;
				} else
					fixedSymbol = false;
				break;
			case 3:
				// `draw symbol [.code.|\name]' 
				if (gr_unknown_symbol == (symbolCode = determine_symbol_code(_word[2]))) {
					demonstrate_command_usage();
					err("Can't understand symbol \\`", _word[2], "'", "\\");
					return false;
				}
				fixedSymbol = true;
				break;
			case 6:
				// `draw symbol .code.|\name at .x. .y.' 
				if (gr_unknown_symbol == (symbolCode = determine_symbol_code(_word[2]))) {
					demonstrate_command_usage();
					err("Can't understand symbol `\\", _word[2], "'", "\\");
					return false;
				}
				if (strcmp(_word[3], "at")) {
					demonstrate_command_usage();
					MISSING_WORD_ERROR("at");
					return false;
				}
				_ignore_error = true;
				if (!getdnum(_word[4], &x)) {
					_ignore_error = old;
					READ_WORD_ERROR(".x.");
					demonstrate_command_usage();
					return false;
				} else
					_ignore_error = old;
				_ignore_error = true;
				if (!getdnum(_word[5], &y)) {
					_ignore_error = old;
					READ_WORD_ERROR(".y.");
					demonstrate_command_usage();
					return false;
				} else
					_ignore_error = old;
				if (inside_box(x, y)) { // ignore if clipped
					set_environment();
					set_line_width_symbol();
					double xcm, ycm;
					gr_usertocm(x, y, &xcm, &ycm);
					set_ps_color('p');
					set_line_width_symbol();
					gr_drawsymbol(xcm, ycm, symbolCode);
					PUT_VAR("..xlast..", x);
					PUT_VAR("..ylast..", y);
				}
				draw_axes_if_needed();
				return true;
			case 7:
				// `draw symbol .code.|\name at .x. .y. cm' 
				if (gr_unknown_symbol == (symbolCode = determine_symbol_code(_word[2]))) {
					demonstrate_command_usage();
					err("Can't understand symbol `\\", _word[2], "'", "\\");
					return false;
				}
				if (strcmp(_word[3], "at")) {
					demonstrate_command_usage();
					MISSING_WORD_ERROR("at");
					return false;
				}
				if (!word_is(6, "cm") && !word_is(6, "pt")) {
					demonstrate_command_usage();
					MISSING_WORD_ERROR("cm or pt");
					return false;
				}
				_ignore_error = true;
				if (!getdnum(_word[4], &x)) {
					_ignore_error = old;
					READ_WORD_ERROR(".x.");
					demonstrate_command_usage();
					return false;
				} else
					_ignore_error = old;
				_ignore_error = true;
				if (!getdnum(_word[5], &y)) {
					_ignore_error = old;
					READ_WORD_ERROR(".y.");
					demonstrate_command_usage();
					return false;
				} else
					_ignore_error = old;
				double xuser, yuser;
				if (word_is(6, "cm")) {
					gr_cmtouser(x, y, &xuser, &yuser); 
				} else if (word_is(6, "pt")) {
					gr_cmtouser(x / PT_PER_CM, y / PT_PER_CM, &xuser, &yuser); 
				} else { // duplicate check
					demonstrate_command_usage();
					MISSING_WORD_ERROR("cm or pt");
					return false;
				}
				if (inside_box(xuser, yuser)) { // ignore if clipped
					set_environment();
					set_line_width_symbol();
					set_ps_color('p');
					set_line_width_symbol();
					if (word_is(6, "pt")) 
						gr_drawsymbol(x / PT_PER_CM, y / PT_PER_CM, symbolCode);
					else
						gr_drawsymbol(x, y, symbolCode);
					PUT_VAR("..xlast..", x);
					PUT_VAR("..ylast..", y);
				}
				return true;
			default:
				demonstrate_command_usage();
				NUMBER_WORDS_ERROR;
				return false;
			}
		}
	}
	// Note: the following only done for `draw symbol ...'
	double *xp = _colX.begin();
	double *yp = _colY.begin();
	double *zp = _colZ.begin();
	set_environment();
	if (!uses_color)
		set_ps_color('p');
	set_line_width_symbol();
	unsigned int num = _colX.size();
	GriColor c, old_color = _griState.color_line();
	set_line_width_symbol();
	double xlast = gr_currentmissingvalue();
	double ylast = gr_currentmissingvalue();
	for (unsigned int i = 0; i < num; i++) {
		if (!gr_missingx((double) *xp)
		    && !gr_missingy((double) *yp)
		    && inside_box((double) *xp, (double) *yp)) {
			double xcm, ycm;
			gr_usertocm(*xp, *yp, &xcm, &ycm);
			num_drawn++;
			if (fixedSymbol == true) {
				// Fixed symbol, possibly in colour 
				if (uses_color) {
					if (hue_in_z)
						hue = *zp;
					if (saturation_in_z)
						saturation = *zp;
					if (brightness_in_z)
						brightness = *zp;
					c.setHSV(hue, saturation, brightness);
					_griState.set_color_line(c);
					set_ps_color('p');
				} else if (uses_graylevel) {
					brightness = *zp;
					c.setRGB(brightness, brightness, brightness);
					_griState.set_color_line(c);
					set_ps_color('p');
				}
				gr_drawsymbol(xcm, ycm, symbolCode);
				xlast = *xp;
				ylast = *yp;
			} else {
				// Symbol stored in z 
				if (!gr_missing((double) *zp)) {
					gr_drawsymbol(xcm, ycm, gr_symbol_type(int(floor((0.5 + *zp)))));
					xlast = *xp;
					ylast = *yp;
				}
			}
		}
		xp++;
		yp++;
		zp++;
	}
	if (uses_color || uses_graylevel) {
		_griState.set_color_line(old_color);
		set_ps_color('p');
	}
	draw_axes_if_needed();
	if (_chatty > 0) {
		if (num_drawn < _colX.size()) {
			sprintf(_grTempString, "\
`draw symbol' drew %d of the %d data; the remaining %d points\n\
  were either missing or out of clip region.\n",
				num_drawn, 
				(unsigned int)(_colX.size()),
				(unsigned int)(_colX.size()) - num_drawn);
			gr_textput(_grTempString);
		}

	}
	PUT_VAR("..xlast..", xlast);
	PUT_VAR("..ylast..", ylast);
	return true;
}

gr_symbol_type
determine_symbol_code(const char * s)
{
	double          tmp;
	bool            old = _ignore_error;
	_ignore_error = true;
	if (getdnum(s, &tmp)) {
		_ignore_error = old;
		return gr_symbol_type(int(floor(0.5 + (double) tmp)));
	} else if (!strcmp(s, "plus"))
		return gr_plus_symbol;
	else if (!strcmp(s, "times"))
		return gr_times_symbol;
	else if (!strcmp(s, "box"))
		return gr_box_symbol;
	else if (!strcmp(s, "circ"))
		return gr_circ_symbol;
	else if (!strcmp(s, "diamond"))
		return gr_diamond_symbol;
	else if (!strcmp(s, "triangleup"))
		return gr_triangleup_symbol;
	else if (!strcmp(s, "triangleright"))
		return gr_triangleright_symbol;
	else if (!strcmp(s, "triangledown"))
		return gr_triangledown_symbol;
	else if (!strcmp(s, "triangleleft"))
		return gr_triangleleft_symbol;
	else if (!strcmp(s, "asterisk"))
		return gr_asterisk_symbol;
	else if (!strcmp(s, "star"))
		return gr_star_symbol;
	else if (!strcmp(s, "filledbox"))
		return gr_filledbox_symbol;
	else if (!strcmp(s, "bullet"))
		return gr_bullet_symbol;
	else if (!strcmp(s, "filleddiamond"))
		return gr_filleddiamond_symbol;
	else if (!strcmp(s, "filledtriangleup"))
		return gr_filledtriangleup_symbol;
	else if (!strcmp(s, "filledtriangleright"))
		return gr_filledtriangleright_symbol;
	else if (!strcmp(s, "filledtriangledown"))
		return gr_filledtriangledown_symbol;
	else if (!strcmp(s, "filledtriangleleft"))
		return gr_filledtriangleleft_symbol;
	else if (!strcmp(s, "filledhalfmoonup"))
		return gr_filledhalfmoonup_symbol;
	else if (!strcmp(s, "filledhalfmoondown"))
		return gr_filledhalfmoondown_symbol;
	else
		return gr_unknown_symbol;
}

// draw circle with radius .r_cm. at .x_cm. .y_cm. 
bool
draw_circleCmd()
{
	double          r_cm, x_cm, y_cm;
	double old_size = gr_currentsymbolsize_cm();
	switch (_nword) {
	case 8:
		if (!getdnum(_word[4], &r_cm))
			return false;
		if (!getdnum(_word[6], &x_cm))
			return false;
		if (!getdnum(_word[7], &y_cm))
			return false;
		gr_setsymbolsize_cm(2 * r_cm);
		set_ps_color('p');
		set_line_width_symbol();
		gr_drawsymbol(x_cm, y_cm, gr_circ_symbol);
		gr_setsymbolsize_cm(old_size);
		return true;
	default:
		return false;
	}
}

// `draw contour' `draw contour .value. [unlabelled|{labelled "\label"}]'
// 
// `draw contour .min. .max. .inc. [.inc_unlabelled.] [unlabelled]'
bool
draw_contourCmd()
{
	extern bool     _contour_label_rotated;	// <-> startup.c set.c
	extern bool     _contour_label_whiteunder;	// <-> startup.c set.c
	double          min, inc, inc_unlabelled, max;
	double          dlevel, dmin, dinc, dinc_unlabelled = 0.0, dmax;
	double          contour_space_first = _contour_space_first;
	double          contour_space_later = _contour_space_later;
	double          contour_minlength = _contour_minlength;
	double          xsize = XSIZE_DEFAULT;
	double          ysize = YSIZE_DEFAULT;
	std::string     user_label;
	int             nword = _nword;
	bool            user_gave_label = false;	// only for .value. format
	bool            have_unlabelled_and_labelled = false;
	bool            labelled = true;
	int             contour, numcontours = 0;
	// Check that data exist.
	if (!grid_exists())
		return false;
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	// Decode command
	labelled = true;
	if (!strcmp(_word[nword - 1], "unlabelled")) {
		labelled = false;
		nword--;
	}
	if (nword == 2) {
		// `draw contour' -- Gri will pick levels
		int             nlevels;
		min = _f_min;		// matrix limits
		max = _f_max;		// matrix limits
		gr_scale125((double) min, (double) max, 5, &dmin, &dmax, &nlevels);
		dinc = (dmax - dmin) / nlevels;
	} else if (nword == 3) {
		// `draw contour .value.'
		double          level;
		if (!getdnum(_word[2], &level))
			return false;
		dmin = level;
		dmax = level;
		dinc = level;
		numcontours = 1;
	} else if (nword == 5 && !strcmp(_word[3], "labelled")) {
		// `draw contour .value.' labelled "label"
		double          level;
		if (!getdnum(_word[2], &level))
			return false;
		user_gave_label = true;	// label is _word[4]
		user_label.assign(_word[4]);
		un_double_quote(user_label);
		dmin = level;
		dmax = level;
		dinc = level;
		numcontours = 1;
	} else if (nword == 5) {
		// `draw contour .min. .max. .inc.'
		if (!getdnum(_word[2], &min)
		    || !getdnum(_word[3], &max)
		    || !getdnum(_word[4], &inc))
			return false;
		if (inc > 0.0) {
			if ((min + inc) > max) {
				demonstrate_command_usage();
				err("Increment has wrong sign to go between the stated min and max");
				return false;
			}
			dmin = min;
			dmax = max;
			dinc = inc;
		} else if (inc < 0.0) {
			if ((min + inc) < max) {
				demonstrate_command_usage();
				err("Increment has wrong sign to go between the stated min and max");
				return false;
			}
			dmin = max;
			dmax = min;
			dinc = -inc;
		} else {
			err("Cannot use increment of zero.");
			return false;
		}
		if (!gr_multiple(dmax - dmin, dinc, 0.001 * dinc)) {
			demonstrate_command_usage();
			err("Require (.max. - .min.) a multiple of .inc. to within 0.1%");
			return false;
		}
	} else if (nword == 6) {
		// `draw contour .min. .max. .inc. .inc_unlabelled.'
		if (!getdnum(_word[2], &min)
		    || !getdnum(_word[3], &max)
		    || !getdnum(_word[4], &inc)
		    || !getdnum(_word[5], &inc_unlabelled))
			return false;
		if ((min + inc) > max) {
			demonstrate_command_usage();
			err("Require (.min. + .inc.) <= .max.");
			return false;
		}
		// Ensure same sign for unlabelled increment
		if (inc < 0.0)
			inc_unlabelled = -fabs(inc_unlabelled);
		else
			inc_unlabelled = fabs(inc_unlabelled);
		dmin = min;
		dmax = max;
		dinc = inc;
		dinc_unlabelled = inc_unlabelled;
		if (!gr_multiple(dmax - dmin, dinc, 0.001 * dinc)) {
			demonstrate_command_usage();
			err("Require (.max. - .min.) a multiple of .inc. to within 0.1%");
			return false;
		}
		if (!gr_multiple(dmax - dmin, dinc_unlabelled, 0.001 * dinc_unlabelled)) {
			demonstrate_command_usage();
			err("Require (.max. - .min.) a multiple of .inc_unlabelled. to within 0.1%");
			return false;
		}
		have_unlabelled_and_labelled = true;
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (dinc == 0.0 && dmin != 0.0 && dmax != 0.0) {
		demonstrate_command_usage();
		err("`draw contour .min. .max. 0' not allowed");
		return false;
	}
	// Do contour(s)
	if (!get_var("..xsize..", &xsize))
		warning("Don't know value of ..xsize.. so using XSIZE_DEFAULT");
	if (!get_var("..ysize..", &ysize))
		warning("Don't know value of ..ysize.. so using YSIZE_DEFAULT");
	set_environment();
	set_line_width_curve();
	// If numcontours not already set, try to figure it out.
	if (!numcontours) {
		if (have_unlabelled_and_labelled) {
			if (dinc_unlabelled == 0.0 && dmin == 0.0 && dmax == 0.0)
				numcontours = 1;
			else
				numcontours = 1 + (int) fabs(0.5 + (dmax - dmin) / dinc_unlabelled);
		} else {
			if (dinc == dmin && dmax == dmin)
				numcontours = 1;
			else
				numcontours = 1 + (int) fabs(0.5 + (dmax - dmin) / dinc);
		}
	}
	dlevel = dmax;
	bool warned = false;
	GriTimer t;
	GriString label;
	for (contour = 0; contour < numcontours; contour++) {
		if (_chatty > 1) {
			sprintf(_grTempString, "`draw contour' drawing for value =%g\n", dlevel);
			gr_textput(_grTempString);
		}
		if (labelled
		    && (numcontours == 1 || gr_multiple(dmax - dlevel, dinc, 0.001 * dinc))) {
			if (fabs(dlevel) <= 1.0e-6 * fabs(dinc)) {
				label.fromSTR("0");
			} else {
				char tmp[1000];
				sprintf(tmp, _contourFmt.c_str(), dlevel);
				label.fromSTR(tmp);
			}
			// Figure distance spacing for contours.  Use stored values if
			// the space-later is > 0; otherwise do old default
			if (_contour_space_later < 0.0) {
				contour_space_first = 1.0;
				contour_space_later = labelled ? 0.5 * (xsize + ysize) : 0.0;
			} else {
				contour_space_first = _contour_space_first;
				contour_space_later = _contour_space_later;
			}
		} else {
			label.fromSTR("");
			contour_space_first = 0.0;
			contour_space_later = 0.0;
		}
		if (user_gave_label) {
			if (_f_min <= dlevel && dlevel <= _f_max) {
				gr_contour(_xmatrix, _ymatrix, _f_xy, _legit_xy,
					   _num_xmatrix_data, _num_ymatrix_data,
					   dlevel, user_label.c_str(),
					   _contour_label_rotated,
					   _contour_label_whiteunder,
					   _contour_space_centered,
					   _griState.color_line(),
					   _griState.color_text(),
					   contour_minlength,
					   contour_space_first,
					   contour_space_later,
					   NULL);
			}
		} else {
			if (_f_min <= dlevel && dlevel <= _f_max) {
				gr_contour(_xmatrix,
					   _ymatrix,
					   _f_xy,
					   _legit_xy,
					   _num_xmatrix_data,
					   _num_ymatrix_data,
					   dlevel, 
					   label.getValue(),
					   _contour_label_rotated,
					   _contour_label_whiteunder,
					   _contour_space_centered,
					   _griState.color_line(),
					   _griState.color_text(),
					   contour_minlength,
					   contour_space_first,
					   contour_space_later,
					   NULL);
			}
		}
		if (have_unlabelled_and_labelled)
			dlevel -= dinc_unlabelled;
		else
			dlevel -= dinc;
		if (!warned) {
			double frac = (1.0 + contour) / numcontours;
			warned = warn_if_slow(&t, frac, "draw contour");
		}
	}
	_drawingstarted = true;
	draw_axes_if_needed();
	return true;
}

// draw arc [filled] .xc_cm. .yc_cm. .r_cm. .angle_1. .angle_2.
bool
draw_arcCmd(void)
{
	bool filled = false;
	int start_word = 2;
	if (_nword == 8) {
		if (word_is(2, "filled")) {
			filled = true; 
			start_word = 3;
		} else {
			err("`draw arc' expecting `filled' but got `\\", _word[2], "'.", "\\");
			return false;
		}
	} else if (_nword != 7) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	double xc, yc, r, angle1, angle2;
	if (!getdnum(_word[start_word], &xc)) {
		READ_WORD_ERROR(".xc_cm.");
		demonstrate_command_usage(); return false; 
	}
	if (!getdnum(_word[start_word + 1], &yc)) {
		READ_WORD_ERROR(".yc_cm.");
		demonstrate_command_usage();
		return false;
	}
	if (!getdnum(_word[start_word + 2], &r)) {
		READ_WORD_ERROR(".r_cm.");
		demonstrate_command_usage();
		return false; 
	}
	if (!getdnum(_word[start_word + 3], &angle1)) { 
		READ_WORD_ERROR(".angle_1.");
		demonstrate_command_usage();
		return false; 
	}
	if (!getdnum(_word[start_word + 4], &angle2)) { 
		READ_WORD_ERROR(".angle_2.");
		demonstrate_command_usage();
		return false; 
	}
	gr_draw_arc_cm(filled, xc, yc, r, angle1, angle2);
	_drawingstarted = true;
	return true;
}

bool
draw_arrow_from_toCmd()
{
	double          halfwidth = ARROWSIZE_DEFAULT;
	double          x0, y0, x1, y1;
	if (2 == get_cmd_values(_word, _nword, "from", 2, _dstack)) {
		x0 = _dstack[0];
		y0 = _dstack[1];
		if (2 == get_cmd_values(_word, _nword, "to", 2, _dstack)) {
			x1 = _dstack[0];
			y1 = _dstack[1];
		} else {
			READ_WORD_ERROR("(.x1., .y1.)");
			demonstrate_command_usage();
			return false;
		}
	} else {
		READ_WORD_ERROR("(.x0., .y0.)");
		demonstrate_command_usage();
		return false;
	}
	set_environment();
	set_line_width_curve();
	// Convert to cm units if given in user units.
	if (strcmp(_word[_nword - 1], "cm")) {
		double          x_cm, y_cm;
		if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
			no_scales_error();
			return false;
#else
			create_x_scale();
			create_y_scale();
#endif
		}
		gr_usertocm(x0, y0, &x_cm, &y_cm);
		x0 = x_cm;
		y0 = y_cm;
		gr_usertocm(x1, y1, &x_cm, &y_cm);
		x1 = x_cm;
		y1 = y_cm;
	}
	if (!get_var("..arrowsize..", &halfwidth))
		warning("Don't know ..arrowsize.. so using default _ARROWSIZE_PT_DEFAULT");
	switch (_arrow_type) {
	case 0:
		gr_drawarrow_cm(x0, y0, x1, y1, halfwidth);
		break;
	case 1:
		gr_drawarrow2_cm(x0, y0, x1, y1, halfwidth);
		break;
	case 2:
		gr_drawarrow3_cm(x0, y0, x1, y1, halfwidth);
		break;
	}
	_drawingstarted = true;
	return true;
}

bool
draw_arrowsCmd()
{
	if (_nword != 2) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!_columns_exist || _colU.size() <= 0) {
		warning("`draw arrows' noticed that no column data exist.");
		return true;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	if (!_uscale_exists) {
		demonstrate_command_usage();
		err("First `set u scale'");
		return false;
	}
	if (!_vscale_exists) {
		demonstrate_command_usage();
		err("First `set v scale'");
		return false;
	}
	set_environment();
	set_line_width_curve();
	double *xp = _colX.begin();
	double *yp = _colY.begin();
	double *up = _colU.begin();
	double *vp = _colV.begin();
	double          halfwidth = ARROWSIZE_DEFAULT;
	if (!get_var("..arrowsize..", &halfwidth))
		warning("Don't know ..arrowsize.. so using default _ARROWSIZE_PT_DEFAULT");
	unsigned int i_max = _colX.size();
	for (unsigned int i = 0; i < i_max; i++) {
		if (!gr_missingx((double) *xp)
		    && !gr_missingy((double) *yp)
		    && inside_box((double) *xp, (double) *yp)
		    && !gr_missing((double) *up)
		    && !gr_missing((double) *up)
		    && (*up != 0.0 || *vp != 0.0)) {
			double          x0, y0, x1, y1;
			gr_usertocm(*xp, *yp, &x0, &y0);
			x1 = x0 + *up * _cm_per_u;
			y1 = y0 + *vp * _cm_per_v;
			switch (_arrow_type) {
			case 0:
				gr_drawarrow_cm(x0, y0, x1, y1, halfwidth);
				break;
			case 1:
				gr_drawarrow2_cm(x0, y0, x1, y1, halfwidth);
				break;
			case 2:
				gr_drawarrow3_cm(x0, y0, x1, y1, halfwidth);
				break;
			}
		}
		xp++;
		yp++;
		up++;
		vp++;
	}
	_drawingstarted = true;
	draw_axes_if_needed();
	return true;
}

bool
draw_axesCmd()
{
	double          tmp;
	int             type = 0;
	switch (_nword) {
	case 2:
		type = _axesStyle;
		draw_axes(type, 0.0, (gr_axis_properties) 0, true);
		return true;
	case 3:
		if (!strcmp(_word[2], "none")) {
			_drawingstarted = true;
			_need_x_axis = false;
			_need_y_axis = false;
			return true;
		} else if (!strcmp(_word[2], "frame")) {
			draw_axes(2, 0.0, (gr_axis_properties) 0, true);
			return true;
		} else if (!getdnum(_word[2], &tmp)) {
			demonstrate_command_usage();
			err("`draw axes ?what?");
			return false;
		}
		type = (int) fabs(0.5 + (double) tmp);
		draw_axes(type, 0.0, (gr_axis_properties) 0, true);
		return true;
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
}

void
draw_outline_frame()
{
	int old_line_cap = _griState.line_cap();
	_griState.set_line_cap(0);
	GriPath p(5);
	p.push_back(_xleft, _ytop, 'm');
	p.push_back(_xright, _ytop, 'l');
	p.push_back(_xright, _ybottom, 'l');
	p.push_back(_xleft, _ybottom, 'l');
	p.push_back(_xleft, _ytop, 'l');
	p.stroke(units_user, _griState.linewidth_axis());
	_griState.set_line_cap(old_line_cap);
}

bool
draw_axes(int type, double loc, gr_axis_properties side, bool allow_offset)
{
	double          tic_direction = 0; // tics extend out by default
	double          tic_size = TICSIZE_DEFAULT;
	double          ysize = YSIZE_DEFAULT;
	double          ymargin = YMARGIN_DEFAULT;
	double          xsize = XSIZE_DEFAULT;
	double          xmargin = XMARGIN_DEFAULT;
	double          fontsize = FONTSIZE_PT_DEFAULT;
	double          oldFontsize_pt = gr_currentfontsize_pt();
	gr_fontID       old_font = gr_currentfont();
	double          tmpx, tmpx_cm, tmpy, tmpy_cm;
	double          axes_offset;
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	if (allow_offset)
		axes_offset = _axes_offset;
	else
		axes_offset = 0.0;
	group_start("axis_frame");
	gr_setxtransform(_xtype);
	gr_setytransform(_ytype);
	gr_setxlabel(_colX.getName());
	gr_setylabel(_colY.getName());
	gr_setxnumberformat(_xFmt.c_str());
	gr_setynumberformat(_yFmt.c_str());
	gr_setxsubdivisions(_xsubdiv);
	gr_setysubdivisions(_ysubdiv);
	if (!get_var("..tic_direction..", &tic_direction))
		warning("(set_environment) ..tic_direction.. undefined so using OUT");
	gr_setticdirection(int(floor(0.5 + tic_direction)) ? true : false);
	if (!get_var("..tic_size..", &tic_size))
		warning("(set_environment) ..tic_size.. undefined so using default (0.2cm)");
	gr_setticsize_cm(tic_size);
	if (!get_var("..fontsize..", &fontsize))
		warning("(draw_axes) ..fontsize.. undefined so using 12");
	gr_setfontsize_pt(fontsize);
	gr_setfont(old_font);
	if (!get_var("..xmargin..", &xmargin))
		warning("draw_axes: don't know ..xmargin.. so using default.");
	if (!get_var("..xsize..", &xsize))
		warning("draw_axes: don't know ..xsize.. so using default.");
	if (!get_var("..ymargin..", &ymargin))
		warning("draw_axes: don't know ..ymargin.. so using default.");
	if (!get_var("..ysize..", &ysize))
		warning("draw_axes: don't know ..ysize.. so using default.");
	set_x_scale();
	set_y_scale();
	// Set to proper linewidth, and turn dashing off
	set_line_width_axis();
	std::vector<double> old_dash;
	for (unsigned int i = 0; i < _dash.size(); i++)
		old_dash.push_back(_dash[i]);
	std::vector<double> dash;
	_dash.erase(_dash.begin(), _dash.end());
	switch (type) {
	case 0:			// full axes 
		if (_xatbottom) {
			gr_usertocm(_xleft, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm, tmpy_cm - axes_offset, &tmpx, &tmpy);
			gr_drawxaxis(tmpy, _xleft, _xinc, _xright, _x_labelling, gr_axis_BOTTOM);
			gr_setfontsize_pt(0.0);
			gr_usertocm(_xleft, _ytop, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm, tmpy_cm + axes_offset, &tmpx, &tmpy);
			gr_drawxaxis(tmpy, _xleft, _xinc, _xright,  _x_labelling, gr_axis_TOP);
			gr_setfontsize_pt(fontsize);
		} else {
			gr_setfontsize_pt(0.0);
			gr_usertocm(_xleft, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm, tmpy_cm - axes_offset, &tmpx, &tmpy);
			gr_drawxaxis(tmpy, _xleft, _xinc, _xright,  _x_labelling, gr_axis_BOTTOM);
			gr_setfontsize_pt(fontsize);
			gr_usertocm(_xleft, _ytop, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm, tmpy_cm + axes_offset, &tmpx, &tmpy);
			gr_drawxaxis(tmpy, _xleft, _xinc, _xright,  _x_labelling, gr_axis_TOP);
		}
		if (_yatleft == true) {
			gr_usertocm(_xleft, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm - axes_offset, tmpy_cm, &tmpx, &tmpy);
			gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, gr_axis_LEFT);
			gr_setfontsize_pt(0.0);
			gr_usertocm(_xright, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm + axes_offset, tmpy_cm, &tmpx, &tmpy);
			gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, gr_axis_RIGHT);
			gr_setfontsize_pt(fontsize);
		} else {
			gr_setfontsize_pt(0.0);
			gr_usertocm(_xleft, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm - axes_offset, tmpy_cm, &tmpx, &tmpy);
			gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, gr_axis_LEFT);
			gr_setfontsize_pt(fontsize);
			gr_usertocm(_xright, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm + axes_offset, tmpy_cm, &tmpx, &tmpy);
			gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, gr_axis_RIGHT);
		}
		_need_x_axis = false;
		_need_y_axis = false;
		if ((ymargin + ysize) > _top_of_plot)
			_top_of_plot = ymargin + ysize;
		break;
	case 1:			// axes at left and bottom + simple frame 
		gr_usertocm(_xleft, _ybottom, &tmpx_cm, &tmpy_cm);
		gr_cmtouser(tmpx_cm, tmpy_cm - axes_offset, &tmpx, &tmpy);
		gr_drawxaxis(tmpy, _xleft, _xinc, _xright,  _x_labelling, gr_axis_BOTTOM);
		gr_usertocm(_xleft, _ybottom, &tmpx_cm, &tmpy_cm);
		gr_cmtouser(tmpx_cm - axes_offset, tmpy_cm, &tmpx, &tmpy);
		gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, gr_axis_LEFT);
		draw_outline_frame();
		_need_x_axis = false;
		_need_y_axis = false;
		if ((ymargin + ysize) > _top_of_plot)
			_top_of_plot = ymargin + ysize;
		break;
	case 2:			// simple frame, no axes 
		draw_outline_frame();
		_need_x_axis = false;
		_need_y_axis = false;
		if ((ymargin + ysize) > _top_of_plot)
			_top_of_plot = ymargin + ysize;
		break;
	case 3:			// x axis only -- don't do frame if offset 
		if (side == gr_axis_BOTTOM) {
			gr_usertocm(_xleft, loc, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm, tmpy_cm - axes_offset, &tmpx, &tmpy);
			gr_drawxaxis(tmpy, _xleft, _xinc, _xright,  _x_labelling, side);
		} else {
			gr_usertocm(_xleft, loc, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm, tmpy_cm + axes_offset, &tmpx, &tmpy);
			gr_drawxaxis(tmpy, _xleft, _xinc, _xright,  _x_labelling, side);
		}
		_need_x_axis = false;
		{
			// add space for tics (maybe), space, number, space, label, and
			// then some inter-axis space
			double          x_cm, y_cm;
			extern double  _grTicSize_cm;
			extern bool    _grTicsPointIn;
			gr_usertocm(_xleft, loc, &x_cm, &y_cm);
			y_cm += _grTicsPointIn == true ? 0.0 : _grTicSize_cm;
			y_cm += 5.0 * gr_currentCapHeight_cm();
			if (y_cm > _top_of_plot)
				_top_of_plot = y_cm;
		}
		break;
	case 4:			// y axis only -- don't do frame if offset 
		if (side == gr_axis_BOTTOM) {
			gr_usertocm(loc, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm - axes_offset, tmpy_cm, &tmpx, &tmpy);
			gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, side);
		} else {
			gr_usertocm(loc, _ybottom, &tmpx_cm, &tmpy_cm);
			gr_cmtouser(tmpx_cm + axes_offset, tmpy_cm, &tmpx, &tmpy);
			gr_drawyaxis(tmpx, _ybottom, _yinc, _ytop, _y_labelling, side);
		}
		_need_y_axis = false;
		{
			// add space 
			double          x_cm, y_cm;
			gr_usertocm(loc, _ytop, &x_cm, &y_cm);
			if (y_cm > _top_of_plot)
				_top_of_plot = y_cm;
		}
		break;
        default:
		err("unknown axis type");
		group_end();
		return false;
	}
	gr_setfontsize_pt(oldFontsize_pt);
	_drawingstarted = true;
	draw_axes_if_needed();
	for (unsigned int i = 0; i < old_dash.size(); i++)
		_dash.push_back(old_dash[i]);
	group_end();
	return true;
}

bool
draw_curveCmd()
{
        //printf("%s:%d draw_curveCmd()...\n",__FILE__,__LINE__);
	if (!_columns_exist) {
		warning("`draw curve' noticed that no column data exist.");
		return true;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	bool filled = false;
	if (_nword == 6
	    && !strcmp(_word[2], "filled")
	    && !strcmp(_word[3], "to")) {
		double tmp;
		if (!getdnum(_word[4], &tmp)) {
			demonstrate_command_usage();
			err("Can't read value to fill to.");
			return false;
		}
		if (!strcmp(_word[5], "x")) {
			return draw_curve_filled_to_valueCmd(false, tmp);
		} else if (!strcmp(_word[5], "y")) {
			return draw_curve_filled_to_valueCmd(true, tmp);
		} else {
			demonstrate_command_usage();
			err("Last word must be \"x\" or \"y\".");
			return false;
		}
	} else if (_nword == 3 && !strcmp(_word[2], "filled")) {
		filled = true;
	} else if (_nword != 2) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	double          *xp = _colX.begin();
	double          *yp = _colY.begin();
	double          lastx = gr_currentmissingvalue();
	double          lasty = gr_currentmissingvalue();
	bool            first = true, last_OK = true;
	extern FILE *_grSVG;
	unsigned int i_max = _colX.size();
	if (_output_file_type == svg) {
		fprintf(_grSVG, "<g> <!-- %s -->\n", _cmdLine);
	}
	set_environment();
	set_line_width_curve();
	GriPath  path(i_max);
	for (unsigned int i = 0; i < i_max; i++) {
		//printf("i %d   x %f   y %f\n", i, *xp, *yp);
		if (!gr_missingx((double) *xp)
		    && !gr_missingy((double) *yp)
		    && inside_box((double) *xp, (double) *yp)) {
			if (first) {
				path.push_back(*xp, *yp, 'm');
				first = false;
			} else {
				if (last_OK) {
					path.push_back(*xp, *yp, 'l');
					lastx = *xp;
					lasty = *yp;
				} else {
					path.push_back(*xp, *yp, 'm');
				}
			}
			last_OK = true;
		} else {
			// Not ok. 
			last_OK = false;
		}
		xp++;
		yp++;
	}
	if (filled)
		path.fill(units_user);
	else
		path.stroke(units_user);
	PUT_VAR("..xlast..", lastx);
	PUT_VAR("..ylast..", lasty);
	_drawingstarted = true;
	draw_axes_if_needed();
	if (_output_file_type == svg) {
		fprintf(_grSVG, "</g> <!-- %s -->\n", _cmdLine);
	}
	return true;
}

// `draw curve [filled [to {.y. y}|{.x. x}]]' 
bool
draw_curve_filled_to_valueCmd(bool to_y, double value)
{
	double          *xp = _colX.begin();
	double          *yp = _colY.begin();
	double          lastx = 0.0, lasty = 0.0; // last good; assignment calms compiler
	bool            path_exists = false;
	bool            last_OK = false;
	if (!_columns_exist) {
		warning("`draw curve filled to' noticed that no column data exist.");
		return true;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	GriPath p;
	set_environment();
	unsigned int i_max = _colX.size();
	for (unsigned i = 0; i < i_max; i++) {
		if (!gr_missingx((double) *xp)
		    && !gr_missingy((double) *yp)
		    && inside_box((double) *xp, (double) *yp)) {
			// This point is not missing, and it's in the box.
			if (!last_OK) {
				// Last point was not OK (or this is the first point), so
				// start a new path, beginning at y=value so the fill will
				// work correctly.
				path_exists = true;
				if (to_y)
					p.push_back(*xp, value, 'm');
				else
					p.push_back(value, *yp, 'm');
			}
			p.push_back(*xp, *yp, 'l');
			lastx = *xp;
			lasty = *yp;
			last_OK = true;
		} else {
			// This point is either missing or not in the box.
			if (last_OK) {
				// Must have just finished a run of good data.  Continue the
				// path to y=value, then fill it.
				if (to_y)
					p.push_back(lastx, value, 'l');
				else
					p.push_back(value, lasty, 'l');
				p.fill(units_user);
				path_exists = false;
			}
			// Last was not OK.  Nothing to do but wait.
			last_OK = false;
		}
		xp++;
		yp++;
	}
	if (path_exists) {
		if (to_y)
			p.push_back(lastx, value, 'l');
		else
			p.push_back(value, lasty, 'l');
		p.fill(units_user);
	}
	PUT_VAR("..lastx..", *(xp - 1));
	PUT_VAR("..lasty..", *(yp - 1));
	draw_axes_if_needed();
	_drawingstarted = true;
	return true;
}

// CHANGE: before 2.052, put circles at missing points
bool
draw_gridCmd()
{
	if (_nword != 2) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!_grid_exists) {
		err("First create the grid");
		return false;
	}
	if (!_xgrid_exists || _num_xmatrix_data < 1) {
		err("First `read grid x' or `set x grid'");
		return false;
	}
	if (!_ygrid_exists || _num_ymatrix_data < 1) {
		err("First `read grid y' or `set y grid'");
		return false;
	}
	set_environment();
	set_line_width_symbol();
	set_ps_color('p');
	unsigned j = _num_ymatrix_data - 1;
	do {
		double xcm, ycm;
		for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
			if (inside_box(_xmatrix[i], _ymatrix[j])) {
				if (_legit_xy(i,j) == true) {
					gr_usertocm(_xmatrix[i], _ymatrix[j], &xcm, &ycm);
					gr_drawsymbol(xcm, ycm, gr_plus_symbol);
				}
			}
		}
	} while (j-- != 0);
	return true;
}

// `Draw isopycnal [unlabelled] .density. [.P_sigma. [.P_theta.]]'
bool
draw_isopycnalCmd()
{
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	bool labelled = true;
	double density, P_sigma = 0.0, P_theta = 0.0;
	int start = 2;
	if (!strcmp(_word[start], "unlabelled")) {
		labelled = false;
		start++;
	}
	switch (_nword - start) {
	case 1: // draw isopycnal [unlabelled] .density.
		if (!getdnum(_word[start], &density)) {
			READ_WORD_ERROR(".density.");
			demonstrate_command_usage();
			return false;
		}
		break;
	case 2: // draw isopycnal [unlabelled] .density. .P_sigma.
		if (!getdnum(_word[start], &density)) {
			READ_WORD_ERROR(".density.");
			demonstrate_command_usage();
			return false;
		}
		if (!getdnum(_word[1 + start], &P_sigma)) {
			READ_WORD_ERROR(".P_sigma.");
			demonstrate_command_usage();
			return false;
		}
		break;
	case 3: // draw isopycnal [unlabelled] .density. .P_sigma.
		if (!getdnum(_word[start], &density)) {
			READ_WORD_ERROR(".density.");
			demonstrate_command_usage();
			return false;
		}
		if (!getdnum(_word[1 + start], &P_sigma)) {
			READ_WORD_ERROR(".P_sigma.");
			demonstrate_command_usage();
			return false;
		}
		if (!getdnum(_word[2 + start], &P_theta)) {
			READ_WORD_ERROR(".P_theta.");
			demonstrate_command_usage();
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (density > 100.0)
		density -= 1000;	// make in sigma-ish unit
	double Smin = _xleft, Smax = _xright;
	if (Smax <= Smin) {
		err("X-axis must have salinity, increasing to right.");
		demonstrate_command_usage();	
		return false;
	}
	set_environment();		// for scales
	bool T_ito_rho_S_p(double *T, double S1, double S2, double dS,double Terr);
	GriPath iso;
	const double T_tolerance = 0.0001; // stop when this close
	bool hit_top = false;	// (if false, hit RHS)
	bool first = true;
	double Tlast = -999;
	double S, T = -999.0;

	// Next few lines determine how fine a mesh to use for S,
	// since a fine mesh is needed if the lines are nearly
	// vertical, but a coarser mesh is more efficient.
	double d_rho_dS = (rho(_xleft, _ybottom, 0.0) - rho(_xright, _ybottom, 0.0)) / (_xright - _xleft);

	double d_rho_dT = (rho(_xleft, _ytop, 0.0) - rho(_xleft, _ybottom, 0.0)) / (_ytop - _ybottom);

	double slope_on_page = d_rho_dS*(_xright-_xleft)/(d_rho_dT*(_ytop-_ybottom));
	double dS;
	// BUG: the S increments are just a guess that seems ok now.
	if (slope_on_page > 5.0) 
		dS = (Smax - Smin) / 500.0;
	else 
		dS = (Smax - Smin) / 100.0;

	//printf("rho_x %f   rho_y %f   slope %f\n", d_rho_dS, d_rho_dT, d_rho_dS*(_xright-_xleft)/(d_rho_dT*(_ytop-_ybottom)));

	double S_cm, T_cm, S_cm_last = 0, T_cm_last = 0;
	std::vector<double> S_label_cm;	// where to put labels
	std::vector<double> T_label_cm;
	double cum_dist = 0.0;
	extern double _contour_space_first, _contour_space_later;
	double contour_space_first, contour_space_later;
	if (_contour_space_later < 0.0) {
		double xsize = XSIZE_DEFAULT;
		double ysize = YSIZE_DEFAULT;
		if (!get_var("..xsize..", &xsize))
			warning("Don't know value of ..xsize.. so using XSIZE_DEFAULT");
		if (!get_var("..ysize..", &ysize))
			warning("Don't know value of ..ysize.. so using YSIZE_DEFAULT");
		contour_space_first = 1.0;
		contour_space_later = labelled ? 0.5 * (xsize + ysize) : 0.0;
	} else {
		contour_space_first = _contour_space_first;
		contour_space_later = _contour_space_later;
	}

	double contour_spacing = contour_space_first;

	for (S = Smin; S <= Smax + dS / 10.0; S += dS) {
		//printf("should calc for S=%f\n", S);
		T = -999.0;	// the T where isopycnal hits
		double T1 = -2.0, T2 = 40;	// must be between these
		double dev1 = rho(S, pot_temp(S, T1, P_theta, P_sigma), P_sigma)
			- 1000.0 - density;
		double dev2 = rho(S, pot_temp(S, T2, P_theta, P_sigma), P_sigma)
			- 1000.0 - density;
		if (dev1 * dev2 > 0.0)
			continue;		// not bracketted at this salinity
		//printf("\n");
		while (true) {
			//printf("T= %.4f  T1= %.4f T2= %.4f\n", T,T1,T2);
			if (T2 - T1 < T_tolerance)
				break;
			T = (T1 + T2) / 2.0; // midpoint
			double dev = rho(S, pot_temp(S, T, P_theta, P_sigma), P_sigma)
				- 1000.0 - density;
			if (dev * dev1 < 0.0) {
				T2 = T;
				dev2 = dev;
			} else if (dev * dev2 < 0.0) {
				T1 = T;
				dev1 = dev;
			} else {
				break;		// dev=0, so must have hit it exactly
			}
		}
		if (T < _ybottom) {
			Tlast = T;
			continue;
		}
		if (Tlast == -999.0 && _ytop < T) {
			//printf("S=%f T=%f -- no intersection\n",S,T);
			break;		// didn't intersect regions
		}
		// Now know that   _ybottom < T
		if (Tlast < _ybottom) {	// interpolate to bottom side
			//printf("-- S,T %f %f ",S,T);
			if (Tlast != -999.0) {
				S = (S - dS) + dS * (_ybottom - Tlast) / (T - Tlast);
				T = _ybottom;
			}
			//printf("--> %f %f\n",S,T);
			gr_usertocm(S, T, &S_cm, &T_cm);
			if (!first)
				cum_dist += sqrt((S_cm - S_cm_last) * (S_cm - S_cm_last) 
						 + (T_cm - T_cm_last) * (T_cm - T_cm_last)); 
			iso.push_back(S_cm, T_cm, first ? 'm' : 'l');
			S_cm_last = S_cm;
			T_cm_last = T_cm;
			first = false;
		} else if (T > _ytop) {	// interpolate S to intersection
			//printf("-- S,T %f %f ",S,T);
			if (Tlast != -999.0) {
				S = (S - dS) + dS * (_ytop - Tlast) / (T - Tlast);
				T = _ytop;
			}
			//printf("--> %f %f\n",S,T);
			gr_usertocm(S, T, &S_cm, &T_cm);
			if (!first)
				cum_dist += sqrt((S_cm - S_cm_last) * (S_cm - S_cm_last) 
						 + (T_cm - T_cm_last) * (T_cm - T_cm_last)); 
			iso.push_back(S_cm, T_cm, first ? 'm' : 'l');
			S_cm_last = S_cm;
			T_cm_last = T_cm;
			first = false;
			hit_top = true;
			break;
		}
		gr_usertocm(S, T, &S_cm, &T_cm);
		if (!first) {
			cum_dist += sqrt((S_cm - S_cm_last) * (S_cm - S_cm_last) 
					 + (T_cm - T_cm_last) * (T_cm - T_cm_last)); 
			if (cum_dist > contour_spacing) {
				S_label_cm.push_back(S_cm);
				T_label_cm.push_back(T_cm);
				contour_spacing = contour_space_later;
				cum_dist = 0.0;	// start over
			}
		}
		iso.push_back(S_cm, T_cm, first ? 'm' : 'l');
		S_cm_last = S_cm;
		T_cm_last = T_cm;
		first = false;
		//printf(" -- S,T,cumdist = %.4f %.4f %.1f\n", S, T,cum_dist);
		Tlast = T;
	}
	if (iso.size() > 0) {
		set_line_width_curve();
		iso.stroke(units_cm);
		if (labelled) {
			char clabel[20];
			sprintf(clabel, _contourFmt.c_str(), density);
			GriString label(clabel);
#if 0				// old way -- along sides
			double xcm, ycm;
			double tic_direction = 0;
			get_var("..tic_direction..", &tic_direction);
			double tic_size = TICSIZE_DEFAULT;
			get_var("..tic_size..", &tic_size);
			set_environment();
			gr_usertocm(S, T, &xcm, &ycm);
			set_ps_color('t');
			if (hit_top) {
				ycm += 2.0 * tic_size; // put above
				label.draw(xcm, ycm, TEXT_CENTERED, 0.0);
			} else {
				xcm += 2.0 * tic_size; // put to right
				ycm -= gr_currentfontsize_pt() / PT_PER_CM / 2;
				label.draw(xcm, ycm, TEXT_LJUST, 0.0);
			}
#else
			set_ps_color('t');
			GriColor white;
			white.setRGB(1.0, 1.0, 1.0);
			for (unsigned int ii = 0; ii < S_label_cm.size(); ii++) {
				gr_show_in_box(label, _griState.color_text(), white, S_label_cm[ii], T_label_cm[ii], 0.0);
			}
#endif

		}
		_drawingstarted = true;
		draw_axes_if_needed();
	}
	return true;
}

// `draw image histogram [box .xleft_cm. .ybottom_cm. .xright_cm. .ytop_cm.]'
bool
draw_image_histogramCmd()
{
#if 1
	enum y_axis_type {log, percentage};
	y_axis_type the_y_axis_type = log;
#endif

	int             i;
	double          x_ll_cm = XMARGIN_DEFAULT, y_ll_cm, x_ur_cm, y_ur_cm;
	double          dx = XSIZE_DEFAULT, dy;
	double          xval, dxval, yMin = 1.e-4, yMax = 1.0;
	double          left, right;
	int             num;
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	if (!_image.storage_exists) {
		err("First `read image' or `convert grid to image'");
		return false;
	}
	if (!_imageTransform_exists) {
		err("First `set image grayscale'");
		return false;
	}
#if 1
	// The optional last word may indicate the type of y axis.
	int nword = _nword;
	if (strEQ(_word[_nword - 1], "percentage")) {
		the_y_axis_type = percentage;
		nword--;
	} else if (strEQ(_word[_nword - 1], "log")) {
		the_y_axis_type = log;
		nword--;
	}
#endif
	switch (nword) {
	case 3:
		// `draw image histogram' 
		if (!get_var("..xmargin..", &x_ll_cm))
			warning("Don't know value of ..xmargin.. so using XMARGIN_DEFAULT");
		y_ll_cm = _top_of_plot + 1.5;
		if (!get_var("..xsize..", &dx))
			warning("Don't know value of ..xsize.. so using XSIZE_DEFAULT");
		dy = 2.5;
		break;
	case 7:
		err("Sorry, but the old `draw histogram .xleft. .ybottom. .dx. .dy.' is\n\
now `draw image histogram [box .xleft_cm. .ybottom_cm. .xright_cm. .ytop_cm.]'.\n\
Note the extra word `box', and the new meaning of the last 2 parameters.");
		return false;
	case 8:
		// `draw image histogram [box .xleft_cm. .ybottom_cm. .xright_cm. .ytop_cm.]'
		if (4 == get_cmd_values(_word, nword, "box", 4, _dstack)) {
			x_ll_cm = _dstack[0];
			y_ll_cm = _dstack[1];
			x_ur_cm = _dstack[2];
			y_ur_cm = _dstack[3];
		} else {
			err("Cannot read the `box ...' part of command");
			return false;
		}
		dx = x_ur_cm - x_ll_cm;
		dy = y_ur_cm - y_ll_cm;
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	calculate_image_histogram();
	set_environment();
	// Draw the histogram, using 125 scaling of x axis
#if 1
	if (the_y_axis_type == log) {
		yMin = 1.0e-4;
		yMax = 1.0;
		gr_setytransform(gr_axis_LOG);
		gr_setysubdivisions(1);
		gr_setxsubdivisions(1);
		gr_setxlabel("");
		gr_setylabel("");
		gr_scale125((double) _image0, (double) _image255, 4, &left, &right, &num);
		gr_setxscale((double) x_ll_cm, (double) x_ll_cm + dx, left, right);
		gr_setyscale((double) y_ll_cm, (double) y_ll_cm + dy, yMin, yMax);
		gr_drawxaxis(yMin, left, (right - left) / num, right, left, gr_axis_BOTTOM);
		gr_drawyaxis((double) left, yMin, 1., yMax, yMin, gr_axis_LEFT);
		GriPath p;
		p.push_back(left, yMin, 'm');
		p.push_back(left, yMax, 'l');
		p.push_back(right, yMax, 'l');
		p.push_back(right, yMin, 'l');
		xval = _image0;
		dxval = (_image255 - _image0) / 255.0;
		p.push_back(_image0, yMin, 'm');
		gr_setytransform(gr_axis_LOG);
		gr_setysubdivisions(1);
		for (i = 0; i < 256; i++) {
			p.push_back(xval, yMin + _imageHist[i], 'l');
			xval += dxval;
		}
		p.stroke(units_user);
	} else if (the_y_axis_type == percentage) {
		yMin = 0.0;
		yMax = 100.0;
		gr_setytransform(gr_axis_LINEAR);
		gr_setysubdivisions(1);
		gr_setxsubdivisions(1);
		gr_setxlabel("");
		gr_setylabel("");
		extern char _grNumFormat_y[];
		string old_y_fmt;
		old_y_fmt.assign(_grNumFormat_y);
		gr_setynumberformat("%.0lf%%");
		gr_scale125((double) _image0, (double) _image255, 4, &left, &right, &num);
		gr_setxscale((double) x_ll_cm, (double) x_ll_cm + dx, left, right);
		gr_setyscale((double) y_ll_cm, (double) y_ll_cm + dy, yMin, yMax);
		gr_drawxaxis(yMin, left, (right - left) / num, right, left, gr_axis_BOTTOM);
		gr_setysubdivisions(4);
		gr_drawyaxis((double) left, yMin, 25., yMax, yMin, gr_axis_LEFT);
		gr_setynumberformat(old_y_fmt.c_str());
		GriPath p;
		p.push_back(left, yMin, 'm');
		p.push_back(left, yMax, 'l');
		p.push_back(right, yMax, 'l');
		p.push_back(right, yMin, 'l');
		xval = _image0;
		dxval = (_image255 - _image0) / 255.0;
		p.push_back(_image0, yMin, 'm');
		for (i = 0; i < 256; i++) {
			p.push_back(xval, yMin + 100*_imageHist[i], 'l');
			xval += dxval;
		}
		p.stroke(units_user);
	} else {
		err("'draw image histogram' cannot understand type `\\'", _word[_nword - 1], "'", "\\");
		return false;
	}
#endif
	gr_setytransform(gr_axis_LINEAR);
	if ((y_ll_cm + dy) > _top_of_plot)
		_top_of_plot = y_ll_cm + dy;
	return true;
}

bool
draw_image_paletteCmd()
{
	if (_output_file_type == svg)
		warning("svg mode may not handle image palettes properly yet");
	extern FILE    *_grPS;
	const double height = 1.0;	// height of box (cm)
	const double space = 2.0;	// space of box above top of plot
	const int LEN = 512;	// length of tmp image
	int             rotpal=0;
	int             otherside=0;
	int             i;
	double          left_cm, bottom_cm, right_cm, top_cm;
	unsigned int    words_understood = 3;	// 3 in `draw image palette'
	unsigned char   gray[LEN];
	double          left, inc = 0.0, right, grayVal, grayInc;
	int             num;
	double          llx, lly, dx = XSIZE_DEFAULT, urx, ury;
	if (!get_var("..xmargin..", &llx))
		warning("Sorry, don't know value of ..xmargin.. so using XMARGIN_DEFAULT");
	if (!get_var("..xsize..", &dx))
		warning("Sorry, don't know value of ..xsize.. so using XSIZE_DEFAULT");
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	// Check for old usage `draw image grayscale'.  Give warning, but proceed
	// anyway.
	if (word_is(2, "grayscale") || word_is(2, "greyscale")) {
		warning("`draw image grayscale' RENAMED `draw image palette'; please change your cmdfile");
	}
	if (word_is(3, "axistop")) {
		words_understood++;
		rotpal=0;
		otherside=1;
	}
	if (word_is(3, "axisbottom")) {
		words_understood++;
		rotpal=0;
		otherside=0;
	}
	if (word_is(3, "axisleft")) {
		words_understood++;
		rotpal=1;
		otherside=0;
	}
	if (word_is(3, "axisright")) {
		words_understood++;
		rotpal=1;
		otherside=1;
	}
	if (1 == get_cmd_values(_word, _nword, "left", 1, _dstack)) {
		// Getting light/dark levels from command.
		left = _dstack[0];
		if (1 == get_cmd_values(_word, _nword, "right", 1, _dstack)) {
			right = _dstack[0];
			words_understood += 4;
		} else {
			demonstrate_command_usage();
			err("Missing `right' keyword");
			return false;
		}
	} else {
		// Getting light/dark levels from image.
		if (!image_range_exists()) {
			err("Image range unknown.  First `convert grid to image' or `set image range'");
			return false;
		}
		gr_scale125((double) _image0, (double) _image255, 4, &left, &right, &num);
		inc = (right - left) / num;
	}
	if (1 == get_cmd_values(_word, _nword, "increment", 1, _dstack)) {
		inc = _dstack[0];
		words_understood += 2;
	} else {
		if (inc == 0.0) {
			// Make guess, probably an ugly one
			inc = 0.2 * (right - left);
		}
	}
	if (!well_ordered(left, right, inc))
		inc = -inc;
	if (4 == get_cmd_values(_word, _nword, "box", 4, _dstack)) {
		llx = _dstack[0];
		lly = _dstack[1];
		urx = _dstack[2];
		ury = _dstack[3];
		words_understood += 5;
	} else {
		llx = XMARGIN_DEFAULT;
		lly = _top_of_plot + space;
		urx = llx + dx;
		ury = lly + height;
	}
	// Check that command syntax was OK.
	if (words_understood != _nword) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	// Draw image, then box.
	set_environment();
	set_ps_color('p');
	if (rotpal==0) {
		gr_setxlabel("");
		gr_setxnumberformat(_xFmt.c_str());
		gr_setxsubdivisions(1);
		gr_setysubdivisions(1);
		gr_setxscale(llx, urx, left, right);
		gr_setyscale(lly, ury, 0.0, 1.0);
	} else {
		gr_setylabel("");
		gr_setynumberformat(_yFmt.c_str());
		gr_setxsubdivisions(1);
		gr_setysubdivisions(1);
		gr_setxscale(llx, urx, 0.0, 1.0);
		gr_setyscale(lly, ury, left, right);
	}
	if (ury > _top_of_plot)
		_top_of_plot = ury;
	grayInc = (right - left) / (LEN - 1.0);
	grayVal = left;		// - 0.5 * grayInc
	for (i = 0; i < LEN; i++) {
		int             imval;
		imval = (int) floor(0.5 + (255.0 * (grayVal - _image0) / (_image255 - _image0)));
		if (imval < 0)
			imval = 0;
		else if (imval > 255)
			imval = 255;
		gray[i] = imval;
		grayVal += grayInc;
	}
	if (rotpal==0) {
		gr_usertocm(left, 0.0, &left_cm, &bottom_cm);
		gr_usertocm(right, 1.0, &right_cm, &top_cm);
	} else {
		gr_usertocm(0.0, left, &left_cm, &bottom_cm);
		gr_usertocm(1.0, right, &right_cm, &top_cm);
	}
	// Clip to this, because image overhangs
	if (_output_file_type == postscript) {
		fprintf(_grPS, "q n %% turn clipping on for image palette\n");
		fprintf(_grPS, "%f %f moveto\n", left_cm * PT_PER_CM, bottom_cm * PT_PER_CM);
		fprintf(_grPS, "%f %f lineto\n", right_cm * PT_PER_CM, bottom_cm * PT_PER_CM);
		fprintf(_grPS, "%f %f lineto\n", right_cm * PT_PER_CM, top_cm * PT_PER_CM);
		fprintf(_grPS, "%f %f lineto\n", left_cm * PT_PER_CM, top_cm * PT_PER_CM);
		fprintf(_grPS, "%f %f lineto\n", left_cm * PT_PER_CM, bottom_cm * PT_PER_CM);
		fprintf(_grPS, "closepath W\n");
	}
	if (rotpal==0) {
		gr_drawimage(gray,
			     _imageTransform, 
			     _image_color_model,
			     NULL, 0.0, 0.0, 0.0,
			     LEN, 1,
			     left_cm, bottom_cm, right_cm, top_cm,
			     false);
	} else {
		gr_drawimage(gray,
			     _imageTransform, 
			     _image_color_model,
			     NULL, 0.0, 0.0, 0.0,
			     1, LEN,
			     left_cm, bottom_cm, right_cm, top_cm,
			     false);
	}
	if (_output_file_type == postscript)
		fprintf(_grPS, "Q %% turn clipping off for image palette\n");
	double actual_linewidth = _griState.linewidth_line();
	_griState.set_linewidth_line(_griState.linewidth_axis());
	if (rotpal==0) {
		if (otherside==0)
			gr_drawxaxis(0.0, left, inc, right, left, gr_axis_BOTTOM);
		else
			gr_drawxaxis(1.0, left, inc, right, left, gr_axis_TOP);
		if (_output_file_type == postscript)
			fprintf(_grPS, "%.3f w %% test\n", _griState.linewidth_axis());
		GriPath p(4);
		p.push_back(left, 0.0, 'm');
		p.push_back(left, 1.0, 'l');
		p.push_back(right, 1.0, 'l');
		p.push_back(right, 0.0, 'l');
		p.push_back(left, 0.0, 'l');
		p.stroke(units_user);
	} else {
		if (otherside==0)
			gr_drawyaxis(0.0, left, inc, right, left, gr_axis_LEFT);
		else
			gr_drawyaxis(1.0, left, inc, right, left, gr_axis_RIGHT);
		if (_output_file_type == postscript)
			fprintf(_grPS, "%.3f w\n", _griState.linewidth_axis());
		GriPath p(4);
		p.push_back(0.0, left, 'm');
		p.push_back(1.0, left, 'l');
		p.push_back(1.0, right, 'l');
		p.push_back(0.0, right, 'l');
		p.push_back(0.0, left, 'l');
		p.stroke(units_user);
	}
	_griState.set_linewidth_line(actual_linewidth);
	return true;
}

// TODO - handle missingcolor in color images
bool
draw_imageCmd()
{
	double          llx_cm, lly_cm, urx_cm, ury_cm;
	if (!image_scales_defined()) {
		demonstrate_command_usage();
		err("First define box containing image (`set x grid' and `set y grid')");
		return false;
	}
	if (!scales_defined()) {
		create_x_scale();
		create_y_scale();
	}
	if (!_image.storage_exists) {
		demonstrate_command_usage();
		err("First `read image' or `convert grid to image'");
		return false;
	}
	if (!_imageTransform_exists) {
		demonstrate_command_usage();
		err("First `set image grayscale'");
		return false;
	}
	set_environment();
	//printf("DEBUG [draw_imageCmd() %s:%d]  _image_llx=%lf  _image_lly=%lf  _image_urx=%lf   _image_ury=%lf\n",__FILE__,__LINE__,_image_llx,_image_lly,_image_urx,_image_ury);
	gr_usertocm(_image_llx, _image_lly, &llx_cm, &lly_cm);
	gr_usertocm(_image_urx, _image_ury, &urx_cm, &ury_cm);
	if (_imageMask.storage_exists) {
		//printf("DEBUG [draw_imageCmd() %s:%d]  llx_cm=%lf  lly_cm=%lf  urx_cm=%lf   ury_cm=%lf\n",__FILE__,__LINE__,llx_cm,lly_cm,urx_cm,ury_cm);
		extern double   _image_missing_color_red; // in set.c
		extern double   _image_missing_color_green; // in set.c
		extern double   _image_missing_color_blue; // in set.c
		double          mask_r, mask_g, mask_b;
		mask_r = _image_missing_color_red;
		mask_g = _image_missing_color_green;
		mask_b = _image_missing_color_blue;
		//printf("%s:%d image mask at %x\n",__FILE__,__LINE__,(unsigned int)( _imageMask.image));
		if (_output_file_type == svg)
			gr_drawimage_svg(_image.image,
					 _imageTransform, 
					 _image_color_model,
					 _imageMask.image, mask_r, mask_g, mask_b,
					 _image.ras_width, _image.ras_height,
					 llx_cm, lly_cm, urx_cm, ury_cm,
					 true);
		else 
			gr_drawimage(_image.image, 
				     _imageTransform, 
				     _image_color_model,
				     _imageMask.image, mask_r, mask_g, mask_b,
				     _image.ras_width, _image.ras_height,
				     llx_cm, lly_cm, urx_cm, ury_cm,
				     true);
	} else {
		if (_output_file_type == svg)
			gr_drawimage_svg(_image.image,
					 _imageTransform, 
					 _image_color_model,
					 NULL, 0.0, 0.0, 0.0,
					 _image.ras_width, _image.ras_height,
					 llx_cm, lly_cm, urx_cm, ury_cm,
					 true);
		else 
			gr_drawimage(_image.image,
				     _imageTransform, 
				     _image_color_model,
				     NULL, 0.0, 0.0, 0.0,
				     _image.ras_width, _image.ras_height,
				     llx_cm, lly_cm, urx_cm, ury_cm,
				     true);
	}
	_drawingstarted = true;
	draw_axes_if_needed();
	return true;
}

bool
draw_labelCmd()
{
	gr_fontID       old_font = gr_currentfont();
	double          fontsize = FONTSIZE_PT_DEFAULT;
	double          textangle_deg = 0.0;
	double          tmp1, tmp2;
	double          xcm, ycm;
	bool            user_units;
	bool            left_justified = false, centered = false, right_justified = false;
	int             coord_word = 0;
	if (_nword < 5) {
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	// Find quote string
	std::string unquoted;
	int status = ExtractQuote(_cmdLine, unquoted);
	if (status == 0) {
		err("`draw label' needs a double-quoted string");
		return false;
	}
	if (status < 0) {
		err("`draw label' found start of a double-quoted string, but not a closing double-quote");
		return false;
	}
	int QuoteEnd = status;
	GriString label(unquoted.c_str());
	chop_into_words(_cmdLine + QuoteEnd, _word, &_nword, MAX_nword);
	// Parse for text angle, in degrees from horizontal.  If the option
	// [rotated .deg.] exists, interpret and then strip from the commandline.
	if (1 == get_cmd_values(_word, _nword, "rotated", 1, _dstack)) {
		textangle_deg = _dstack[0];
		_nword--;
		_nword--;
	}
	// Parse for coordinates
	if (_nword < 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	// Figure out whether "at" or "centered at" or "rightjustified at"
	if (!strcmp(_word[0], "at")) {
		left_justified = true;
		coord_word++;
	} else if ((!strcmp(_word[0], "centered") || !strcmp(_word[0], "centred"))
		   && !strcmp(_word[1], "at")) {
		centered = true;
		coord_word += 2;
	} else if (!strcmp(_word[0], "rightjustified") && !strcmp(_word[1], "at")) {
		right_justified = true;
		coord_word += 2;
	} else {
		demonstrate_command_usage();
		err("Where do you want this label drawn?  (missing `at' word)");
		return false;
	}
	user_units = !word_is(_nword - 1, "cm") && !word_is(_nword - 1, "pt");
	if (user_units) {
		set_environment();
		if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
			no_scales_error();
			return false;
#else
			create_x_scale();
			create_y_scale();
#endif
		}
		if (!getdnum(_word[coord_word], &tmp1)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".x.");
			return false;
		}
		if (!getdnum(_word[coord_word + 1], &tmp2)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".y.");
			return false;
		}
		gr_usertocm(tmp1, tmp2, &xcm, &ycm);
	} else {
		if (word_is(_nword - 1, "cm")) {
			if (!getdnum(_word[coord_word], &xcm)) {
				demonstrate_command_usage();
				READ_WORD_ERROR(".xcm.");
				return false;
			}
			if (!getdnum(_word[coord_word + 1], &ycm)) {
				demonstrate_command_usage();
				READ_WORD_ERROR(".ycm.");
				return false;
			}
		} else if (word_is(_nword - 1, "pt")) {
			if (!getdnum(_word[coord_word], &xcm)) {
				demonstrate_command_usage();
				READ_WORD_ERROR(".xpt.");
				return false;
			}
			if (!getdnum(_word[coord_word + 1], &ycm)) {
				demonstrate_command_usage();
				READ_WORD_ERROR(".ypt.");
				return false;
			}
			xcm /= PT_PER_CM;
			ycm /= PT_PER_CM;
		} else {
			err("Expecting 'cm' or 'pt', but got \\", _word[_nword - 1], "\\");
			return false;
		}
	}
	if (!get_var("..fontsize..", &fontsize))
		warning("Sorry, don't know value of ..fontsize.. so using _FONTSIZE_PT_DEFAULT");
	gr_setfontsize_pt(fontsize);
	gr_setfont(old_font);
	_drawingstarted = true;
	if (user_units)
		draw_axes_if_needed();
	if (left_justified) {
		label.draw(xcm, ycm, TEXT_LJUST, textangle_deg);
	} else if (right_justified) {
		label.draw(xcm, ycm, TEXT_RJUST, textangle_deg);
	} else if (centered) {
		label.draw(xcm, ycm, TEXT_CENTERED, textangle_deg);
	} else {
		err("Where do you want this label drawn?"); // never reached, I think
		return false;
	}
	return true;
}

bool
draw_line_from_toCmd()
{
	double          x0, y0, x1, y1;
	if (2 == get_cmd_values(_word, _nword, "from", 2, _dstack)) {
		x0 = _dstack[0];
		y0 = _dstack[1];
		if (2 == get_cmd_values(_word, _nword, "to", 2, _dstack)) {
			x1 = _dstack[0];
			y1 = _dstack[1];
		} else {
			READ_WORD_ERROR("(.x1., .y1.)");
			demonstrate_command_usage();
			return false;
		}
	} else {
		READ_WORD_ERROR("(.x0., .y0.)");
		demonstrate_command_usage();
		return false;
	}
	// Check if zero-length line
	if (((x1 - x0)*(x1 - x0) + (y1 - y0) * (y1 - y0)) == 0.0) {
		warning("`draw line from ... to ...' is drawing a zero-length line");
	}
	set_environment();
	set_ps_color('p');
	set_line_width_curve();
	static GriPath p(2);
	p.clear();
	if (!strcmp(_word[_nword - 1], "cm")) {
		p.push_back(x0, y0, 'm');
		p.push_back(x1, y1, 'l');
		p.stroke(units_cm);
		_drawingstarted = true;
		return true;
	} else if (!strcmp(_word[_nword - 1], "pt")) {
		p.push_back(x0, y0, 'm');
		p.push_back(x1, y1, 'l');
		p.stroke(units_pt);
		_drawingstarted = true;
		return true;
	} else {
		// User units -- draw axes if needed
		if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
			no_scales_error();
			return false;
#else
			create_x_scale();
			create_y_scale();
#endif
		}
		if (!gr_missingx(x0) && !gr_missingx(x1) && !gr_missingy(y0) && !gr_missingy(y1)) {
			p.push_back(x0, y0, 'm');
			p.push_back(x1, y1, 'l');
			p.stroke(units_user);
			_drawingstarted = true;
			draw_axes_if_needed();
		}
		return true;
	}
}

// `draw lines {vertically .left. .right. .inc.}|{horizontally .bottom. .top.
// .inc.}'
bool
draw_linesCmd(void)
{
	bool            vert = false;
	double          min, max, inc, tmp;
	static GriPath  p(2);
	p.clear();
	switch (_nword) {
	case 6:
		if (!strcmp(_word[2], "vertically"))
			vert = true;
		else if (!strcmp(_word[2], "horizontally"))
			vert = false;
		else {
			err("`\\", _word[2], "' not understood.", "\\");
			demonstrate_command_usage();
			return false;
		}
		set_environment();
		set_line_width_curve();
		if (!getdnum(_word[3], &min)
		    || !getdnum(_word[4], &max)
		    || !getdnum(_word[5], &inc)) {
			return false;
		}
		if (!well_ordered(min, max, inc))
			inc = -inc;
		for (tmp = min; tmp <= (max + 0.25 * inc); tmp += inc) {
			if (vert == true) {
				p.push_back(tmp, _ybottom, 'm');
				p.push_back(tmp, _ytop, 'l');
			} else {
				p.push_back(_xleft, tmp, 'm');
				p.push_back(_xright, tmp, 'l');
			}
		}
		p.stroke(units_user);
		_drawingstarted = true;
		return true;
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
}

// `draw patches .width. .height. [cm]' 
bool
draw_patchesCmd()
{
	double          *xp = _colX.begin();
	double          *yp = _colY.begin();
	double          *zp = _colZ.begin();
	double          dx, dx2, dy, dy2;
	GriColor old_color = _griState.color_line();
	bool            cmUnits;
	if (_nword < 4 || _nword > 5) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!_columns_exist || _colZ.size() <= 0) {
		warning("`draw patches' noticed that no column data exist.");
		return true;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	if (!_imageTransform_exists) {
		err("Image transform doesn't exist yet.  First `set image grayscale'");
		return false;
	}
	// Figure out units for patch size
	cmUnits = (!strcmp(_word[_nword - 1], "cm")) ? true : false;
	// Figure out patch size
	if (!getdnum(_word[2], &dx) || !getdnum(_word[3], &dy)) {
		demonstrate_command_usage();
		if (cmUnits) {
			READ_WORD_ERROR(".x_cm. and .y_cm.");
		} else {
			READ_WORD_ERROR(".x. and .y.");
		}
		return false;
	}
	if (dx <= 0. || dy <= 0.0) {
		demonstrate_command_usage();
		if (cmUnits)
			err("Can't have .dx_cm. <= 0 or .dy_cm. <= 0");
		else
			err("Can't have .dx. <= 0 or .dy. <= 0");
		return false;
	}
	dx2 = dx / 2.0;
	dy2 = dy / 2.0;
	set_environment();
	unsigned int i_max = _colX.size();
	static GriPath p(5);
	GriColor gray_level;
	for (unsigned int i = 0; i < i_max; i++) {
		p.clear();
		double          xl, yb, xr, yt;
		if (!gr_missingx((double) *xp)
		    && !gr_missingy((double) *yp)
		    && !gr_missing((double) *zp)
		    && inside_box((double) *xp, (double) *yp)) {
			// Set grayscale
			gray_level.setRGB(_imageTransform[value_to_image(*zp)] / 255.0,
					  _imageTransform[value_to_image(*zp)] / 255.0,
					  _imageTransform[value_to_image(*zp)] / 255.0);
			_griState.set_color_line(gray_level);
			if (cmUnits) {
				gr_usertocm((double) *xp, (double) *yp, &xl, &yb);
				xl -= dx2;
				xr = xl + dx;
				yb -= dy2;
				yt = yb + dy;
				p.push_back(xl, yb, 'm');
				p.push_back(xr, yb, 'l');
				p.push_back(xr, yt, 'l');
				p.push_back(xl, yt, 'l');
				p.push_back(xl, yb, 'l');
				p.fill(units_cm);
			} else {
				xl = *xp;
				yb = *yp;
				xl -= dx2;
				xr = xl + dx;
				yb -= dy2;
				yt = yb + dy;
				p.push_back(xl, yb, 'm');
				p.push_back(xr, yb, 'l');
				p.push_back(xr, yt, 'l');
				p.push_back(xl, yt, 'l');
				p.push_back(xl, yb, 'l');
				p.fill(units_user);
			}
		}
		xp++;
		yp++;
		zp++;
	}
	_griState.set_color_line(old_color);
	_drawingstarted = true;
	draw_axes_if_needed();
	return true;
}

// `draw polygon [filled] .x0. .y0. .x1. .y1. .x2. .y2. [...] [cm|pt|user]'
bool
draw_polygonCmd(void)
{
	int             i, start = 0, num;
	bool            filled = false;
	units the_unit = units_user;
	int nword = _nword;
	if (nword < 4) {
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (word_is(nword - 1, "cm")) {
		the_unit = units_cm;
		nword--;	// trim this last word
	} else if (word_is(nword - 1, "pt")) {
		the_unit = units_pt;
		nword--;	// trim this last word
	} else if (word_is(nword - 1, "user")) {
		the_unit = units_user;
		nword--;	// trim this last word
	}
	if (word_is(2, "filled")) {
		start = 3;
		filled = true;
		num = nword - 3;
	} else {
		start = 2;
		num = nword - 2;
	}
	if (2 * (num / 2) != num) {
		err("Need matched (x,y) pairs");
		return false;
	}
	set_x_scale();
	set_y_scale();
	set_environment();
	GriPath p;
	for (i = 0; i < num; i += 2) {
		double          x, y;
		if (!getdnum(_word[i + start], &x)) {
			err("Cannot read x");
			return false;
		}
		if (!getdnum(_word[1 + i + start], &y)) {
			err("Cannot read x");
			return false;
		}
		p.push_back(x, y, i == 0 ? 'm' : 'l');
	}
	if (filled)
		p.fill(the_unit);
	else
		p.stroke(the_unit);
	return true;
}

bool
draw_titleCmd()
{
	if (_nword > 2) {
		// find quote string 
		std::string unquoted;
		int status = ExtractQuote(_cmdLine, unquoted);
		if (status == 0) {
			err("`draw title' found no double-quoted string to use.");
			return false;
		}
		if (status < 0) {
			err("`draw title' found no starting double-quote, but no ending double-quote.");
			return false;
		}
		if (!unquoted.empty()) {
			double xmargin = XMARGIN_DEFAULT;
			double xsize   = XSIZE_DEFAULT;
			//set_environment();
			if (!get_var("..xmargin..", &xmargin))
				warning("Sorry, don't know value of ..xmargin.. so using XMARGIN_DEFAULT");
			if (!get_var("..xsize..", &xsize))
				warning("Sorry, don't know value of ..xsize.. so using XSIZE_DEFAULT");
			GriString label;
			label.fromSTR(unquoted.c_str());
			label.draw(xmargin + 0.5 * xsize,
				   _top_of_plot + OFFSET_AFTER_TITLE,
				   TEXT_CENTERED,
				   0.0);
			_top_of_plot += OFFSET_AFTER_TITLE + gr_currentfontsize_pt() / PT_PER_CM;
		}
	} else {
		err("`draw title' what?");
		return false;
	}
	return true;
}

// draw values [.dx. .dy.] [\format] [separation .dist_cm.]
bool
draw_valuesCmd()
{
	if (!_columns_exist || _colZ.size() <= 0) {
		err("First `read columns'");
		return false;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	// Get separation x and y if they are given
	unsigned int gave_separation = 0;
	double xseparation = 0.0, yseparation = 0.0;
	if (2 == get_cmd_values(_word, _nword, "separation", 2, _dstack)) {
		xseparation = _dstack[0];
		yseparation = _dstack[1];
		if (xseparation < 0.0 || yseparation < 0.0) {
			warning("`draw values ... separation .xcm. .ycm.' ignoring negative values");
			xseparation = 0.0;
			yseparation = 0.0;
		}
		gave_separation = 1;
	}
	// Get format if it is given
	unsigned int    gave_fmt = 0;
	char            fmt[20];
	strcpy(fmt, "%g");
	for (unsigned int word = 2; word < _nword; word++) {
		if ('%' == *_word[word]) {
			strcpy(fmt, _word[word]);
			gave_fmt = 1;
			break;
		}
	}
	set_environment();
	double          dx_cm, dy_cm;
	if (_nword == 4 + gave_fmt + 3 * gave_separation) {
		if (!getdnum(_word[2], &dx_cm) || !getdnum(_word[3], &dy_cm)) {
			err("`draw values' can't read dx_cm and dy_cm");
			return false;
		}
	} else if (_nword == 2 + gave_fmt + 3 * gave_separation) {
		double          width_cm, ascent_cm, descent_cm;
		gr_stringwidth("M", &width_cm, &ascent_cm, &descent_cm);
		dx_cm = 0.5 * width_cm;
		dy_cm = -0.5 * ascent_cm;
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	// Now start drawing
	double *xp = _colX.begin();
	double *yp = _colY.begin();
	double *zp = _colZ.begin();
	unsigned int num = _colX.size();
	double          xcm, ycm;
	double          xcm_last = 0.0, ycm_last = 0.0; // assignment calms compiler
	bool first = true;
	GriString label;
	for (unsigned int i = 0; i < num; i++) {
		if (!gr_missing(*zp) && !gr_missingx(*xp) && !gr_missingy(*yp) && inside_box(*xp, *yp)) {
			gr_usertocm(*xp, *yp, &xcm, &ycm);
			xcm += dx_cm;
			ycm += dy_cm;
			if (first || fabs(xcm - xcm_last) >= xseparation || fabs(ycm - ycm_last) >= yseparation) {
				sprintf(_grTempString, fmt, *zp);
				label.fromSTR(_grTempString);
				label.draw(xcm, ycm, TEXT_LJUST, 0.0);
				xcm_last = xcm;
				ycm_last = ycm;
				first = false;
			}
		}
		xp++;
		yp++;
		zp++;
	}
	if (!first) {
		_drawingstarted = true;
		draw_axes_if_needed();
	}
	return true;
}

bool
draw_x_axisCmd()
{
	double          tmp;
	gr_axis_properties side;
	int             n = _nword;
	bool            position_in_cm = false;
	set_x_scale();
	set_y_scale();
	// see if lower|upper specified 
	if (!strcmp(_word[n - 1], "lower") &&
	    strcmp(_word[n - 2], "at")) {
		side = gr_axis_BOTTOM;
		n--;
	} else if (!strcmp(_word[n - 1], "upper") && strcmp(_word[n - 2], "at")) {
		side = gr_axis_TOP;
		n--;
	} else
		side = gr_axis_BOTTOM;
	if (!strcmp(_word[n - 1], "cm")) {
		position_in_cm = true;
		n--;
	}
	switch (n) {
	case 3:			// `draw x axis' 
		draw_axes(3, _ybottom, gr_axis_BOTTOM, true);
		break;
	case 5:			// `draw x axis at bottom|top|.y.' 
		if (!strcmp(_word[n - 1], "bottom"))
			draw_axes(3, _ybottom, gr_axis_BOTTOM, true);
		else if (!strcmp(_word[n - 1], "top")) {
			draw_axes(3, _ytop, gr_axis_TOP, true);
		} else if (getdnum(_word[n - 1], &tmp)) {
			if (position_in_cm) {
				double          xuser, yuser;
				gr_cmtouser((double) 1.0, (double) tmp, &xuser, &yuser);
				tmp = yuser;
			}
			draw_axes(3, tmp, side, false);
		} else {
			demonstrate_command_usage();
			err("`draw x axis at ?where?'");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		err("`draw x axis ?what?");
		return false;
	}
	return true;
}

bool
draw_y_axisCmd()
{
	double          tmp;
	gr_axis_properties side;
	int             n = _nword;
	bool            position_in_cm = false;
	set_x_scale();
	set_y_scale();
	// see if `left|right' specified 
	if (!strcmp(_word[n - 1], "left") && strcmp(_word[n - 2], "at")) {
		side = gr_axis_LEFT;
		n--;
	} else if (!strcmp(_word[n - 1], "right") && strcmp(_word[n - 2], "at")) {
		side = gr_axis_RIGHT;
		n--;
	} else
		side = gr_axis_LEFT;
	if (!strcmp(_word[n - 1], "cm")) {
		position_in_cm = true;
		n--;
	}
	switch (n) {
	case 3:			// `draw y axis' 
		draw_axes(4, _xleft, gr_axis_LEFT, true);
		break;
	case 5:			// `draw y axis at left|right|.x.' 
		if (!strcmp(_word[n - 1], "left")) {
			draw_axes(4, _xleft, gr_axis_LEFT, true);
		} else if (!strcmp(_word[n - 1], "right")) {
			draw_axes(4, _xright, gr_axis_RIGHT, true);
		} else if (getdnum(_word[n - 1], &tmp)) {
			if (position_in_cm) {
				double          xuser, yuser;
				gr_cmtouser((double) tmp, (double) 1.0, &xuser, &yuser);
				tmp = xuser;
			}
			draw_axes(4, tmp, side, false);
		} else {
			demonstrate_command_usage();
			err("`draw y axis at ?where?'");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		err("`draw y axis ?what?");
		return false;
	}
	return true;
}

static const double FRAC = 0.3;	// size of symbols compared to box 
bool
draw_x_box_plotCmd()
{
	double          q1_cm, q2_cm, q3_cm;	// quartiles in cm 
	double          y_cm, ymin_cm, ymax_cm;	// box boundaries in cm 
	double          old_symbol_size = gr_currentsymbolsize_pt(); // store old 
	double          size_cm = 0.5;		      // box size 
	double          y;				      // location 
	double          q1, q2, q3;			      // quartiles 
	double          upper_adjacent_value;	      // whisker>q3 
	double          lower_adjacent_value;	      // whisker<q1 
	double          iqr;			      // interquartile range 
	double          upper, lower;		      // temporary boundaries 
	double          xcm, ycm;
	// See if data exist
	if (!_columns_exist) {
		warning("`draw x box plot' noticed that no column data exist.");
		return true;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	// Check command syntax
	switch (_nword) {
	case 6:
		// draw x box plot at .y. 
		if (!getdnum(_word[5], &y)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".y.");
			return false;
		}
		break;
	case 8:
		// draw x box plot at .y. size .size_cm. 
		if (!getdnum(_word[5], &y)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".y.");
			return false;
		}
		if (!getdnum(_word[7], &size_cm)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".size_cm.");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	set_environment();
	// figure statistics
	histogram_stats(_colX.begin(), _colX.size(), &q1, &q2, &q3);
	iqr = q3 - q1;
	// draw box
	gr_usertocm(q1, y, &q1_cm, &y_cm);
	gr_usertocm(q2, y, &q2_cm, &y_cm);
	gr_usertocm(q3, y, &q3_cm, &y_cm);
	ymin_cm = y_cm + size_cm / 2.0;
	ymax_cm = y_cm - size_cm / 2.0;
	static GriPath p(7);
	p.clear();
#if 0
	// Clockwise from upper-left
	p.push_back(q1_cm, ymax_cm, 'm');
	p.push_back(q3_cm, ymax_cm, 'l');
	p.push_back(q3_cm, ymin_cm, 'l');
	p.push_back(q1_cm, ymin_cm, 'l');
	p.push_back(q1_cm, ymax_cm, 'l');
	p.push_back(q2_cm, ymax_cm, 'm');
	p.push_back(q2_cm, ymin_cm, 'l');
	set_line_width_symbol();
	p.stroke(units_cm);
#else
	// Clockwise from upper-left
	set_line_width_curve();
	p.push_back(q1_cm, ymax_cm, 'm');
	p.push_back(q3_cm, ymax_cm, 'l');
	p.push_back(q3_cm, ymin_cm, 'l');
	p.push_back(q1_cm, ymin_cm, 'l');
	p.stroke(units_cm, -1.0, true);
	p.clear();
	p.push_back(q2_cm, ymax_cm, 'm');
	p.push_back(q2_cm, ymin_cm, 'l');
	p.stroke(units_cm);
#endif
	// draw whiskers
	upper_adjacent_value = q3 - 1;	// will be max between q3 and upper 
	lower_adjacent_value = q1 + 1;	// will be min between q3 and upper 
	upper = q3 + 1.5 * iqr;	// upper fence 
	lower = q1 - 1.5 * iqr;	// lower fence 
	for (unsigned int i = 0; i < _colX.size(); i++) {
		double x = _colX[i];
		if (!gr_missingx(x)) {
			if (x > q3 && x < upper && x > upper_adjacent_value)
				upper_adjacent_value = x;
			if (x < q1 && x > lower && x < lower_adjacent_value)
				lower_adjacent_value = x;
		 }
	}
	set_line_width_symbol();
	gr_setsymbolsize_cm(FRAC * size_cm);
	if (upper_adjacent_value > q3) {	// draw whisker > q3 
		gr_usertocm(q3, y, &xcm, &ycm);
		p.push_back(xcm, ycm, 'm');
		gr_usertocm(upper_adjacent_value, y, &xcm, &ycm);
		p.push_back(xcm, ycm, 'l');
		p.stroke(units_cm);
		gr_drawsymbol(xcm, ycm, gr_times_symbol);
	}
	if (lower_adjacent_value < q1) {	// draw whisker < q1 
		gr_usertocm(q1, y, &xcm, &ycm);
		p.push_back(xcm, ycm, 'm');
		gr_usertocm(lower_adjacent_value, y, &xcm, &ycm);
		p.push_back(xcm, ycm, 'l');
		p.stroke(units_cm);
		gr_drawsymbol(xcm, ycm, gr_times_symbol);
	}
	// draw outliers > q3
	lower = q3 + 1.5 * iqr;
	upper = q3 + 3.0 * iqr;
	for (unsigned int i = 0; i < _colX.size(); i++) {
		double val = _colX[i];
		if (!gr_missingx((double)val)) {
			if (val > lower) {
				gr_usertocm(val, y, &xcm, &ycm);
				if (val <= upper)	// minor outlier 
					gr_drawsymbol(xcm, ycm, gr_circ_symbol);
				else		// major outlier 
					gr_drawsymbol(xcm, ycm, gr_bullet_symbol);
			}
		}
	}
	// draw outliers < q1
	lower = q1 - 3.0 * iqr;
	upper = q1 - 1.5 * iqr;
	for (unsigned int i = 0; i < _colX.size(); i++) {
		double val = _colX[i];
		if (!gr_missingx((double)val)) {
			if (val < upper) {
				gr_usertocm(val, y, &xcm, &ycm);
				if (val >= lower)	// minor outlier 
					gr_drawsymbol(xcm, ycm, gr_circ_symbol);
				else		// major outlier 
					gr_drawsymbol(xcm, ycm, gr_bullet_symbol);
			}
		}
	}
	_drawingstarted = true;
	draw_axes_if_needed();
	gr_setsymbolsize_pt(old_symbol_size);
	return true;
}

bool
draw_y_box_plotCmd()
{
	double          q1_cm, q2_cm, q3_cm; // quartiles in cm 
	double          x_cm, xmin_cm, xmax_cm; // box boundaries in cm 
	double          old_symbol_size = gr_currentsymbolsize_pt(); // store old 
	double          size_cm = 0.5;                 // box size 
	double          x;				   // location 
	double          q1, q2, q3;			   // quartiles 
	double          upper_adjacent_value;	   // whisker>q3 
	double          lower_adjacent_value;	   // whisker<q1 
	double          iqr;			   // interquartile range 
	double          upper, lower;		   // temporary boundaries 
	double          xcm, ycm;
	// See if data exist
	if (!_columns_exist) {
		warning("`draw y box plot' noticed that no column data exist.");
		return true;
	}
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	// Check command syntax
	switch (_nword) {
	case 6:
		// draw y box plot at .x. 
		if (!getdnum(_word[5], &x)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".x.");
			return false;
		}
		break;
	case 8:
		// draw y box plot at .x. size .size_cm. 
		if (!getdnum(_word[5], &x)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".x.");
			return false;
		}
		if (!getdnum(_word[7], &size_cm)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".size_cm.");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	set_environment();
	// Figure statistics.
	histogram_stats(_colY.begin(), _colY.size(), &q1, &q2, &q3);
	iqr = q3 - q1;
	// Draw box.
	gr_usertocm(x, q1, &x_cm, &q1_cm);
	gr_usertocm(x, q2, &x_cm, &q2_cm);
	gr_usertocm(x, q3, &x_cm, &q3_cm);
	xmin_cm = x_cm + size_cm / 2.0;
	xmax_cm = x_cm - size_cm / 2.0;
	static GriPath p(7);
#if 0
	p.clear();
	set_line_width_curve();
	p.push_back(xmin_cm, q1_cm, 'm');
	p.push_back(xmax_cm, q1_cm, 'l');
	p.push_back(xmax_cm, q3_cm, 'l');
	p.push_back(xmin_cm, q3_cm, 'l');
	p.push_back(xmin_cm, q1_cm, 'l');
	p.push_back(xmin_cm, q2_cm, 'm');
	p.push_back(xmax_cm, q2_cm, 'l');
	p.stroke(units_cm);
#else
	p.clear();
	set_line_width_curve();
	p.push_back(xmin_cm, q1_cm, 'm');
	p.push_back(xmax_cm, q1_cm, 'l');
	p.push_back(xmax_cm, q3_cm, 'l');
	p.push_back(xmin_cm, q3_cm, 'l');
	p.stroke(units_cm, -1.0, true);
	p.clear();
	p.push_back(xmin_cm, q2_cm, 'm');
	p.push_back(xmax_cm, q2_cm, 'l');
	p.stroke(units_cm);
#endif
	// Draw whiskers.
	upper_adjacent_value = q3 - 1; // will be max between q3 and upper 
	lower_adjacent_value = q1 + 1; // will be min between q3 and upper 
	upper = q3 + 1.5 * iqr;	   // upper fence 
	lower = q1 - 1.5 * iqr;	   // lower fence 
	for (unsigned int i = 0; i < _colY.size(); i++) {
		double y = _colY[i];
		if (!gr_missingy(y)) {
			if (y > q3 && y < upper && y > upper_adjacent_value)
				upper_adjacent_value = y;
			if (y < q1 && y > lower && y < lower_adjacent_value)
				lower_adjacent_value = y;
		}
	}
	gr_setsymbolsize_cm(FRAC * size_cm);
	set_line_width_symbol();
	if (upper_adjacent_value > q3) {	// draw whisker > q3 
		gr_usertocm(x, q3, &xcm, &ycm);
		p.push_back(xcm, ycm, 'm');
		gr_usertocm(x, upper_adjacent_value, &xcm, &ycm);
		p.push_back(xcm, ycm, 'l');
		p.stroke(units_cm);
		gr_drawsymbol(xcm, ycm, gr_times_symbol);
	}
	if (lower_adjacent_value < q1) {	// draw whisker < q1 
		gr_usertocm(x, q1, &xcm, &ycm);
		p.push_back(xcm, ycm, 'm');
		gr_usertocm(x, lower_adjacent_value, &xcm, &ycm);
		p.push_back(xcm, ycm, 'l');
		p.stroke(units_cm);
		gr_drawsymbol(xcm, ycm, gr_times_symbol);
	}
	// Draw outliers > q3
	lower = q3 + 1.5 * iqr;
	upper = q3 + 3.0 * iqr;
	gr_setsymbolsize_cm(FRAC * size_cm);
	for (unsigned int i = 0; i < _colY.size(); i++) {
		double val = _colY[i];
		if (!gr_missingy((double)val)) {
			if (val > lower) {
			   gr_usertocm(x, val, &xcm, &ycm);
			   if (val <= upper)	// minor outlier 
				   gr_drawsymbol(xcm, ycm, gr_circ_symbol);
			   else		// major outlier 
				   gr_drawsymbol(xcm, ycm, gr_bullet_symbol);
			}
		}
	}
	// Draw outliers < q1
	lower = q1 - 3.0 * iqr;
	upper = q1 - 1.5 * iqr;
	for (unsigned int i = 0; i < _colY.size(); i++) {
		double val = _colY[i];
		if (!gr_missingy((double)val)) {
			if (val < upper) {
				gr_usertocm(x, val, &xcm, &ycm);
				if (val >= lower)	// minor outlier 
					gr_drawsymbol(xcm, ycm, gr_circ_symbol);
				else		// major outlier 
					gr_drawsymbol(xcm, ycm, gr_bullet_symbol);
			}
		}
	}
	_drawingstarted = true;
	draw_axes_if_needed();
	gr_setsymbolsize_pt(old_symbol_size);
	return true;
}

#undef FRAC

bool
draw_zero_lineCmd()
{
	if (!scales_defined()) {
#if 0				// Fix SF bug #129856 (I hope!)
		no_scales_error();
		return false;
#else
		create_x_scale();
		create_y_scale();
#endif
	}
	switch (_nword) {
	case 3:
		// `draw zero line' 
		draw_zeroline_horizontally();
		break;
	case 4:
		if (!strcmp(_word[3], "horizontally"))
			draw_zeroline_horizontally();
		else if (!strcmp(_word[3], "vertically"))
			draw_zeroline_vertically();
		else {
			err("How should I draw this zero line?");
			demonstrate_command_usage();
			return false;
		}
		break;
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	return true;
}

bool
draw_zeroline_horizontally()
{
	if ((_ybottom * _ytop) < 0.0) {
		set_environment();
		set_line_width_curve();
		GriPath p(2);
		p.push_back(_xleft,  0.0, 'm');
		p.push_back(_xright, 0.0, 'l');
		p.stroke(units_user);
		_drawingstarted = true;
		draw_axes_if_needed();
	}
	return true;
}

bool
draw_zeroline_vertically()
{
	if ((_xleft * _xright) < 0.0) {
		set_environment();
		set_line_width_curve();
		GriPath p(2);
		p.push_back(0.0, _ybottom, 'm');
		p.push_back(0.0, _ytop,    'l');
		p.stroke(units_user);
		_drawingstarted = true;
		draw_axes_if_needed();
	}
	return true;
}

bool
set_x_scale()
{
	double          xsize = XSIZE_DEFAULT;
	double          xmargin = XMARGIN_DEFAULT;
	if (!get_var("..xmargin..", &xmargin))
		warning("(draw_axes) don't know ..xmargin.. so using XMARGIN_DEFAULT");
	if (!get_var("..xsize..", &xsize))
		warning("(draw_axes) don't know ..xsize.. so using XSIZE_DEFAULT");
	gr_setxtransform(_xtype);
	gr_setxscale(xmargin, xmargin + xsize, _xleft, _xright);
	_xscale_exists = true;
	return true;
}

bool
set_y_scale()
{
	double          ysize = YSIZE_DEFAULT;
	double          ymargin = YMARGIN_DEFAULT;
	if (!get_var("..ymargin..", &ymargin))
		warning("(draw_axes) don't know ..ymargin.. so using YMARGIN_DEFAULT");
	if (!get_var("..ysize..", &ysize))
		warning("(draw_axes) don't know ..ysize.. so using YSIZE_DEFAULT");
	gr_setytransform(_ytype);
	gr_setyscale(ymargin, ymargin + ysize, _ybottom, _ytop);
	_yscale_exists = true;
	return true;
}

//`Draw gri logo .x_cm. .y_cm. .height_cm. .style. \fgcolor \bgcolor'
//   0   1    2    3      4        5          6        7        8
bool
draw_gri_logoCmd()
{
	double dx=0.03, dy=0.03; // Q: should these be arguments?
	//
	// Process args
	double x_cm, y_cm, height_cm;
	int style;
	if (_nword != 9) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	Require(getdnum(_word[3], &x_cm),      READ_WORD_ERROR(".x_cm."));
	Require(getdnum(_word[4], &y_cm),      READ_WORD_ERROR(".y_cm."));
	Require(getdnum(_word[5], &height_cm), READ_WORD_ERROR(".height_cm."));
	Require(getinum(_word[6], &style),     READ_WORD_ERROR(".style."));
	double fg_r, fg_g, fg_b;
	double bg_r, bg_g, bg_b;
	Require(look_up_color(_word[7], &fg_r, &fg_g, &fg_b), 
		err("unknown colorname `\\", _word[7], "'", "\\"));
	Require(look_up_color(_word[8], &bg_r, &bg_g, &bg_b), 
		err("unknown colorname `\\", _word[8], "'", "\\"));
	GriColor bg_color;
	bg_color.setRGB(bg_r, bg_g, bg_b);
	GriColor fg_color;
	fg_color.setRGB(fg_r, fg_g, fg_b);
	//
	// Read data
	char fname[256];
	sprintf(fname, "%s%s", _lib_directory.c_str(), "logo.dat");
	FILE *fp = fopen(fname, "r");
	if (!fp) {
		warning("Could not open logo data file `\\", fname,
			"'.\n         Continuing with rest of commandfile.", "\\");
		return true;
	}
	GriPath p;
	double xx, yy;
	//
	// Save entry values
	GriColor old_color = _griState.color_line();

	bool was_using_missing_value = gr_using_missing_value();
	double old_grMissingValue = gr_currentmissingvalue();
	gr_set_missing_value(-999.0);
	bool first = true, last_OK = true;
	while (2 == fscanf(fp, "%lf %lf", &xx, &yy)) {
		if (!gr_missing(xx)) {
			xx = x_cm + xx * height_cm;
			yy = y_cm + yy * height_cm;
			if (first) {
				p.push_back(xx, yy, 'm');
				first = false;
			} else {
				if (last_OK) {
					p.push_back(xx, yy, 'l');
				} else {
					p.push_back(xx, yy, 'm');
				}
			}
			last_OK = true;
		} else {	    
			last_OK = false;
		}
	}
	fclose(fp);
	GriPath bg;
	double xmax = 1.81291;	// of data in logo
	double ymax = 1.0;		// of data in logo
	switch (style) {
	case 0:			// stroke it
		_griState.set_color_line(fg_color);
		p.stroke(units_cm);
		break;
	default:
	case 1:			// fill it with no background
		_griState.set_color_line(fg_color);
		p.fill(units_cm);
		break;
	case 2:			// fill it in tight box
		_griState.set_color_line(bg_color);
		bg.push_back(x_cm - height_cm * 0.05,        y_cm - height_cm * 0.05,        'm');
		bg.push_back(x_cm + xmax * height_cm * 1.05, y_cm - height_cm * 0.05,        'l');
		bg.push_back(x_cm + xmax * height_cm * 1.05, y_cm + ymax * height_cm * 1.05, 'l');
		bg.push_back(x_cm - height_cm * 0.05,        y_cm + ymax * height_cm * 1.05, 'l');
		bg.fill(units_cm);
		_griState.set_color_line(fg_color);
		p.fill(units_cm);
		break;
	case 3:			// fill it in square box
		_griState.set_color_line(bg_color);
		bg.push_back(x_cm - height_cm * 0.05,        y_cm - height_cm * 0.05,        'm');
		bg.push_back(x_cm + xmax * height_cm * 1.05, y_cm - height_cm * 0.05,        'l');
		bg.push_back(x_cm + xmax * height_cm * 1.05, y_cm + xmax * height_cm * 1.05, 'l');
		bg.push_back(x_cm - height_cm * 0.05,        y_cm + xmax * height_cm * 1.05, 'l');
		bg.fill(units_cm);
		_griState.set_color_line(fg_color);
		p.fill(units_cm);
		break;
	case 4:			// draw over offset underlay
		_griState.set_color_line(bg_color);
		p.translate(+height_cm * dx, -height_cm * dy);
		p.fill(units_cm);
		p.translate(-height_cm * dx, +height_cm * dy);
		_griState.set_color_line(fg_color);
		p.fill(units_cm);
		break;
	}
	//
	// Reset to entry values
	_griState.set_color_line(old_color);
	if (was_using_missing_value)
		gr_set_missing_value(old_grMissingValue);
	else
		gr_set_missing_value_none();
	return true;
}
