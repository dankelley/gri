#include        <string>
#include	<ctype.h>
#include	<math.h>
#include	<stdio.h>
#include	<string.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"image_ex.hh"
#include        "defaults.hh"
#include        "files.hh"
#include        "superus.hh"
#include        "gr_coll.hh"
#include        "GriState.hh"

static inline bool between(double x, double lim0, double lim1);
extern char     _grTempString[];
void            reset_top_of_plot(void);
bool            ignore_initial_newline(void);
bool            set_ignore_initial_newlineCmd(void);
bool            set_image_colorscaleCmd(void);
bool            set_image_grayscaleCmd(void);
bool            set_image_grayscale_using_hist(void);
bool            set_image_missingCmd(void);
bool            set_image_rangeCmd(void);
bool            set_grid_missingCmd(void);
bool            set_grid_missing_curve(bool inside);
bool            mask_an_island(double *x, double *y, unsigned int n);
bool            set_y_axis_nameCmd(void);
// following shared with read.c
double          _input_data_window_x_min = 1.0;
double          _input_data_window_x_max = -1.0;
double          _input_data_window_y_min = 1.0;
double          _input_data_window_y_max = -1.0;
bool            _input_data_window_x_exists = false;
bool            _input_data_window_y_exists = false;
static bool     already_landscape = false;
static double   xleft, xright, xinc, ybottom, ytop, yinc;
static double   tmp, tmp2;

static inline bool between(double x, double lim0, double lim1)
{
	if (lim0 <= x && x < lim1)
		return true;
	if (lim1 <= x && x < lim0)
		return true;
	return false;
}

void
reset_top_of_plot()
{
	double          margin, size;
	get_var("..ymargin..", &margin);
	get_var("..ysize..", &size);
	_top_of_plot = margin + size;
}

static bool     ignore_initial_newline_flag = false;
bool
set_ignore_initial_newlineCmd()
{
	switch (_nword) {
	case 4:
		ignore_initial_newline_flag = true;
		break;
	case 5:
		if (!strcmp(_word[4], "off")) {
			ignore_initial_newline_flag = false;
			return true;
		} else {
			demonstrate_command_usage();
			return false;
		}
	default:
		demonstrate_command_usage();
		return false;
	}
	return false;
}
bool
ignore_initial_newline()
{
	return ((ignore_initial_newline_flag) ? true : false);
}

bool
set_axes_styleCmd()
{
	if (_nword < 3) {
		err("Too few words in `set axes'");
		return false;
	}
	if (strcmp(_word[2], "style")) {
		err("Third word of `set axes' must be \"style\".");
		return false;
	}
	if (_nword < 4) {
		err("`set axes style' to what?");
		return false;
	}
	// `set axes style offset [.distance_cm.]'
	if (_nword >= 4 && !strcmp(_word[3], "offset")) {
		double          tmp;
		if (_nword == 5) {
			if (!getdnum(_word[4], &tmp)) {
				READ_WORD_ERROR(".distance_cm.");
				return false;
			}
			_axes_offset = tmp;
		} else if (!get_var("..tic_size..", &tmp)) {
			err("Can't remember the tic size.");
			return false;
		}
		_axes_offset = tmp;
		return true;
	}
	switch (_nword) {
	case 4:
		// `set axes style .style.|rectangular|default|none'
		if (!strcmp(_word[3], "default")) {
			_axesStyle = 0;
			_axes_offset = 0.0;
			return true;
		} else if (!strcmp(_word[3], "none")) {
			_need_x_axis = false;
			_need_y_axis = false;
		} else if (!strcmp(_word[3], "rectangular")) {
			// `set axes style rectangular'
			_axesStyle = 0;
			return true;
		} else {
			// `set axes style .style.'
			if (!getdnum(_word[3], &tmp))
				return false;
			if (tmp < 0.0 || tmp > 2.0) {
				err("Ignoring bad axes type <0 or >2");
				return false;
			}
			_axesStyle = (int) floor(0.5 + tmp);
			return true;
		}
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
}

// Set arrow head halfwidth into ..arrowsize..; a positive value indicates
// size in cm; a negative value
bool
set_arrow_sizeCmd()
{
	if (_nword < 3) {
		err("`set arrow' what?");
		return false;
	}
	if (!strcmp(_word[2], "size")) {
		if (_nword < 4) {
			err("`set arrow size' what?");
			return false;
		}
		switch (_nword) {
		case 4:
			// set arrow size .size.|default
			if (!strcmp(_word[3], "default")) {
				PUT_VAR("..arrowsize..", ARROWSIZE_DEFAULT);
				return true;
			} else {
				if (!getdnum(_word[3], &tmp)) {
					err("Can't read arrow size");
					return false;
				}
				if (tmp < 0.0) {
					err("Ignoring bad (negative) arrow size.");
					return false;
				}
				PUT_VAR("..arrowsize..", tmp);
				return true;
			}
			// NOT REACHED
		case 8:
			// `set arrow size as .num. percent of length'
			if (word_is(5, "percent") && word_is(6, "of") && word_is(7, "length")) {
				Require(getdnum(_word[4], &tmp), err("Can't read percentage"));
				Require(tmp >= 0.0, err("Ignoring bad (negative) percentage arrow size."));
				PUT_VAR("..arrowsize..", -tmp / 100.0);
				return true;
			} else {
				demonstrate_command_usage();
				err("Cannot understand command");
				return false;
			}
			// NOT REACHED
		default:
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
	} else {
		err("`set arrow' what?");
		return false;
	}
	// NOT REACHED
}

bool
set_arrow_typeCmd()
{
	if (_nword != 4) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	double tmp;
	if (!getdnum(_word[3], &tmp)) {
		READ_WORD_ERROR(".which.");
		return false;
	}
	_arrow_type = int(floor(0.5 + tmp));
	if (_arrow_type != 0 && _arrow_type != 1 && _arrow_type != 2) {
		err("Valid arrow types are 0, 1, and 2");
		return false;
	}
	return true;
}

bool
set_beepCmd()
{
	if (_nword == 2 || !strcmp(_word[2], "on"))
		_gri_beep = true;
	else if (!strcmp(_word[2], "off"))
		_gri_beep = false;
	else {
		err("`set beep' what?");
		return false;
	}
	return true;
}

//`set bounding box .xleft. .ybottom. .xright. .ytop. [cm|pt]'
bool
set_bounding_boxCmd()
{
	// Set to _bounding_box_user
	double ll_x, ll_y, ur_x, ur_y; // user-units
	double ll_x_cm, ll_y_cm, ur_x_cm, ur_y_cm; // points
	if (_nword == 7) {		// user-units
		if (!getdnum(_word[3], &ll_x)) {
			demonstrate_command_usage();
			err("Can't read .xleft. in `\\", _word[3], "'.", "\\");
			return false;
		}
		if (!getdnum(_word[4], &ll_y)) {
			demonstrate_command_usage();
			err("Can't read .ybottom. in `\\", _word[4], "'.", "\\");
			return false;
		}
		if (!getdnum(_word[5], &ur_x)) {
			demonstrate_command_usage();
			err("Can't read .xright. in `\\", _word[5], "'.", "\\");
			return false;
		}
		if (!getdnum(_word[6], &ur_y)) {
			demonstrate_command_usage();
			err("Can't read .ytop. in `\\", _word[6], "'.", "\\");
			return false;
		}
		double          xmargin = XMARGIN_DEFAULT;
		double          ymargin = YMARGIN_DEFAULT;
		double          xsize = XSIZE_DEFAULT;
		double          ysize = YSIZE_DEFAULT;

		if (!get_var("..xmargin..", &xmargin))
			warning("(set_environment) ..xmargin.. undefined so using default");
		if (!get_var("..ymargin..", &ymargin))
			warning("(set_environment) ..ymargin.. undefined so using default");
		if (!get_var("..xsize..", &xsize))
			warning("(set_environment) ..xsize.. undefined so using default");
		if (!get_var("..ysize..", &ysize))
			warning("(set_environment) ..ysize.. undefined so using default");
		gr_setxtransform(_xtype);
		gr_setxscale(xmargin, xmargin + xsize, _xleft, _xright);
		gr_setytransform(_ytype);
		gr_setyscale(ymargin, ymargin + ysize, _ybottom, _ytop);

		gr_usertocm(ll_x, ll_y, &ll_x_cm, &ll_y_cm);
		gr_usertocm(ur_x, ur_y, &ur_x_cm, &ur_y_cm);

	} else if (_nword == 8) {

		bool in_pt = false;
		if (word_is(7, "cm")) {
			in_pt = false;
		} else if (word_is(7, "pt")) {
			in_pt = true;
		} else {
			err("`set bounding box ...' expecting keyword `pt' or `cm' but got `\\", _word[7], "'", "\\");
			return false;
		}

		if (!getdnum(_word[3], &ll_x_cm)) {
			demonstrate_command_usage();
			err("Can't read .xleft. in `\\", _word[3], "'.", "\\");
			return false;
		}
		if (!getdnum(_word[4], &ll_y_cm)) {
			demonstrate_command_usage();
			err("Can't read .ybottom. in `\\", _word[4], "'.", "\\");
			return false;
		}
		if (!getdnum(_word[5], &ur_x_cm)) {
			demonstrate_command_usage();
			err("Can't read .xright. in `\\", _word[5], "'.", "\\");
			return false;
		}
		if (!getdnum(_word[6], &ur_y_cm)) {
			demonstrate_command_usage();
			err("Can't read .ytop. in `\\", _word[6], "'.", "\\");
			return false;
		}
		if (in_pt) {
			ll_x_cm /= PT_PER_CM;	// convert points to cm
			ll_y_cm /= PT_PER_CM;
			ur_x_cm /= PT_PER_CM;
			ur_y_cm /= PT_PER_CM;
		}

	} else {
		err("Must specify .xleft. .ybottom. .xright. .ytop. [cm]");
	}
	_bounding_box_user.set(ll_x_cm, ll_y_cm, ur_x_cm, ur_y_cm);
	_user_gave_bounding_box = true;
	return true;
}

// set clip to curve [4 words]
bool
set_clipCmd()
{
	Require(_nword > 2, err("Must specify `set clip on' or `set clip off'"));
	if (_nword == 4 && word_is(2, "to") && word_is(3, "curve")) {
		unsigned int xlen = _colX.size();
		if (xlen < 1) {
			warning("`set clip to curve' noticed that no curve exists");
			return true;
		}
		unsigned int ylen = _colY.size();
		if (xlen != ylen) {
			warning("`set clip to curve' noticed that curve's x and y lengths disagree");
			return true;
		}
		double *xp = _colX.begin();
		double *yp = _colY.begin();
		gr_set_clip_ps_curve(xp, yp, xlen);
		return true;
	} else {
		if (!strcmp(_word[2], "postscript")) {
			// PostScript clipping
			if (_nword < 4) {
				err("`set clip postscript ...' needs >= 4 words.");
				return false;
			}
			if (!strcmp(_word[3], "off")) {
				gr_set_clip_ps_off();
				return true;
			} else if (!strcmp(_word[3], "on")) {
				double xl, xr, yb, yt;
				if (_nword == 4) { // set clip postscript on
					xl = _xleft;
					xr = _xright;
					yb = _ybottom;
					yt = _ytop;
				} else if (_nword == 8) { // set clip postscript on .llx. ...
					if (!getdnum(_word[4], &xl)) {
						demonstrate_command_usage();
						err("Can't read .xleft. in `\\", _word[4], "'.", "\\");
						return false;
					}
					if (!getdnum(_word[5], &xr)) {
						demonstrate_command_usage();
						err("Can't read .xright. in `\\", _word[5], "'.", "\\");
						return false;
					}
					if (!getdnum(_word[6], &yb)) {
						demonstrate_command_usage();
						err("Can't read .ybottom. in `\\", _word[6], "'.", "\\");
						return false;
					}
					if (!getdnum(_word[7], &yt)) {
						demonstrate_command_usage();
						err("Can't read .ytop. in `\\", _word[7], "'.", "\\");
						return false;
					}
				} else {
					NUMBER_WORDS_ERROR;
					demonstrate_command_usage();
					return false;
				}
				set_environment();
				double llx, lly, urx, ury;
				gr_usertopt(xl, yb, &llx, &lly);
				gr_usertopt(xr, yt, &urx, &ury);
				gr_set_clip_ps_rect(llx, lly, urx, ury);
				return true;
			}
		} else if (!strcmp(_word[2], "on")) {
			// Non-PostScript clipping
			if (_nword == 3)
				_clipData = -1;
			else if (_nword == 7) {
				if (!getdnum(_word[3], &tmp)) {
					READ_WORD_ERROR(".xleft.");
					_clipData = 0;
					_clipxleft = _clipxright = _clipybottom = _clipytop = 0.0;
					return false;
				}
				_clipxleft = tmp;
				if (!getdnum(_word[4], &tmp)) {
					READ_WORD_ERROR(".xright.");
					_clipData = 0;
					_clipxleft = _clipxright = _clipybottom = _clipytop = 0.0;
					return false;
				}
				_clipxright = tmp;
				if (!getdnum(_word[5], &tmp)) {
					READ_WORD_ERROR(".ybottom.");
					_clipData = 0;
					_clipxleft = _clipxright = _clipybottom = _clipytop = 0.0;
					return false;
				}
				_clipybottom = tmp;
				if (!getdnum(_word[6], &tmp)) {
					READ_WORD_ERROR(".ytop.");
					_clipData = 0;
					_clipxleft = _clipxright = _clipybottom = _clipytop = 0.0;
					return false;
				}
				_clipytop = tmp;
				_clipData = 1;
			} else
				err("`set clip on' takes 4 parameters");
		} else if (!strcmp(_word[2], "off")) {
			gr_set_clip_ps_off();
			_clipData = 0;
		} else {
			err("Must specify `set clip on' or `set clip off'");
			return false;
		} 
	}
	return true;
}

#define CHECK_RGB_RANGE(item) {						\
    if ((item) < 0.0) {							\
	warning("`set color rgb' value being clipped to range 0-1");	\
	(item) = 0.0;							\
    } else if ((item) > 1.0) {						\
	warning("`set color rgb' value being clipped to range 0-1");	\
        (item) = 1.0;							\
    }									\
}
#define CHECK_HSB_RANGE(item) {						\
    if ((item) < 0.0) {							\
        warning("`set color hsb' value being clipped to range 0-1");	\
        (item) = 0.0;							\
    } else if ((item) > 1.0) {						\
        warning("`set color hsb' value being clipped to range 0-1");	\
        (item) = 1.0;							\
    }									\
}
bool
set_colorCmd()
{
	double          red, green, blue;
	std::string cname;
	GriColor c;
	switch (_nword) {
	case 3:
		cname.assign(_word[2]);
		un_double_quote(cname);
		if (!look_up_color(cname.c_str(), &red, &green, &blue)) {
			err("`set color' given unknown colorname `\\", cname.c_str(), "'.  Use command `show colornames' to see available colors.", "\\");
			return false;
		}
		PUT_VAR("..red..", red);
		PUT_VAR("..green..", green);
		PUT_VAR("..blue..", blue);
		c.setRGB(red, green, blue);
		_griState.set_color_line(c);
		if (_griState.separate_text_color() == false)
			_griState.set_color_text(c);
		return true;
	case 6:
		if (!strcmp(_word[2], "rgb")) {
			// `set color rgb .red. .green. .blue.'
			Require(getdnum(_word[3], &red), READ_WORD_ERROR(".red."));
			Require(getdnum(_word[4], &green), READ_WORD_ERROR(".green."));
			Require(getdnum(_word[5], &blue), READ_WORD_ERROR(".blue."));
			// Clip if necessary
			CHECK_RGB_RANGE(red);
			CHECK_RGB_RANGE(green);
			CHECK_RGB_RANGE(blue);
			PUT_VAR("..red..", red);
			PUT_VAR("..green..", green);
			PUT_VAR("..blue..", blue);
			c.setRGB(red, green, blue);
			_griState.set_color_line(c);
			if (_griState.separate_text_color() == false)
				_griState.set_color_text(c);
			return true;
		} else if (!strcmp(_word[2], "hsb")) {
			// `set color hsb .hue. .saturation. .brightness.'
			double          hue, saturation, brightness;
			Require(getdnum(_word[3], &hue), READ_WORD_ERROR(".hue."));
			Require(getdnum(_word[4], &saturation), READ_WORD_ERROR(".saturation."));
			Require(getdnum(_word[5], &brightness), READ_WORD_ERROR(".brightness."));
			// Clip if necessary
			CHECK_HSB_RANGE(hue);
			CHECK_HSB_RANGE(saturation);
			CHECK_HSB_RANGE(brightness);
			gr_hsv2rgb(hue, saturation, brightness, &red, &green, &blue);
			PUT_VAR("..red..", red);
			PUT_VAR("..green..", green);
			PUT_VAR("..blue..", blue);
			c.setHSV(hue, saturation, brightness);
			_griState.set_color_line(c);
			if (_griState.separate_text_color() == false)
				_griState.set_color_text(c);
			return true;
		} else {
			err("Can't understand command.");
			demonstrate_command_usage();
			return false;
		}
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
}


// `set colorname \name {rgb .red. .green. .blue.}|{hsb .hue. .saturation. .brightness.}
bool
set_colornameCmd()
{
	double red, green, blue;
	switch (_nword) {
	case 7:
		if (strEQ(_word[3], "rgb")) {
			Require(getdnum(_word[4], &red), READ_WORD_ERROR(".red."));
			Require(getdnum(_word[5], &green), READ_WORD_ERROR(".green."));
			Require(getdnum(_word[6], &blue), READ_WORD_ERROR(".blue."));
		} else if (strEQ(_word[3], "hsb")) {
			double hue, saturation, brightness;
			Require(getdnum(_word[4], &hue), READ_WORD_ERROR(".hue."));
			Require(getdnum(_word[5], &saturation), READ_WORD_ERROR(".saturation."));
			Require(getdnum(_word[6], &brightness), READ_WORD_ERROR(".brightness."));
			hue = pin0_1(hue);
			saturation = pin0_1(saturation);
			brightness = pin0_1(brightness);
			gr_hsv2rgb(hue, saturation, brightness, &red, &green, &blue);
		} else {
			demonstrate_command_usage();
			err("word must be `rgb' or `hsb', not `\\", _word[3], "' as given.", "\\");
			return false;
		}
		red = pin0_1(red);
		green = pin0_1(green);
		blue = pin0_1(blue);
		create_color(_word[2], red, green, blue);
		//printf("set colorname '%s' %f %f %f\n", _word[2], red, green, blue);
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
}

bool
set_x_typeCmd()
{
	extern char      _xtype_map;
	switch (_nword) {
	case 5:
		if (word_is(3, "map")) {
			if (word_is(4, "E")) {
				_xtype_map = 'E';
			} else if (word_is(4, "W")) {
				_xtype_map = 'W';
			} else if (word_is(4, "S")) {
				_xtype_map = 'S';
			} else if (word_is(4, "N")) {
				_xtype_map = 'N';
			} else {
				err("X map type must be `E', `W', `S' or `N'");
				return false;
			}
		} else {
			err("Type must be `linear', `log', or `map'");
			return false;
		}
		break;
	case 4:
		if (!strcmp(_word[3], "linear")) {
			_xtype = gr_axis_LINEAR;
			_xtype_map = ' ';
		} else if (!strcmp(_word[3], "log")) {
			_xtype_map = ' ';
			if (_xtype == gr_axis_LOG)	// only change if necessary 
				return true;
			if (!_xscale_exists) {
				_xtype = gr_axis_LOG;
				gr_setxtransform(_xtype);
				return true;
			}
			if ((_xright > _xleft) && (_xleft > 0.0) && (_xright > 0.0)) {
				_xinc = 1;
				PUT_VAR("..xleft..", _xleft = pow(10.0, floor(0.5 + log10(_xleft))));
				PUT_VAR("..xright..", _xright = pow(10.0, floor(0.5 + log10(_xright))));
				PUT_VAR("..xinc..", _xinc);
				_xtype = gr_axis_LOG;
			} else {
				err("\
Can't convert from linear x axis, present x axis has numbers <= 0.\n\
First `delete x scale', then `set x type log'.");
			}
		} else {
			err("Type must be `linear', `log', or `map'");
			return false;
		}
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	gr_setxtransform(_xtype);
	return true;
}

bool
set_y_typeCmd()
{
	extern char      _ytype_map;
	switch (_nword) {
	case 5:
		if (word_is(3, "map")) {
			if (word_is(4, "N")) {
				_ytype_map = 'N';
			} else if (word_is(4, "S")) {
				_ytype_map = 'S';
			} else if (word_is(4, "E")) {
				_ytype_map = 'E';
			} else if (word_is(4, "W")) {
				_ytype_map = 'W';
			} else {
				err("Y map type must be `E', `W', `S' or `N'");
				return false;
			}
		} else {
			err("Type must be `linear', `log', or `map'");
			return false;
		}
		break;
	case 4:
		if (!strcmp(_word[3], "linear")) {
			_ytype_map = ' ';
			_ytype = gr_axis_LINEAR;
		} else if (!strcmp(_word[3], "log")) {
			_ytype_map = ' ';
			if (_ytype == gr_axis_LOG) {	// only change if necessary
				return true;
			}
			if (!_yscale_exists) {
				_ytype = gr_axis_LOG;
				gr_setytransform(_ytype);
				return true;
			}
			if ((_ytop > _ybottom) && (_ytop > 0.0) && (_ybottom > 0.0)) {
				_yinc = 1;
				PUT_VAR("..ybottom..", _ybottom = pow(10.0, floor(0.5 + log10(_ybottom))));
				PUT_VAR("..ytop..", _ytop = pow(10.0, floor(0.5 + log10(_ytop))));
				PUT_VAR("..yinc..", _yinc);
				_ytype = gr_axis_LOG;
			} else {
				err("\
Can't convert from linear y axis, present x axis has numbers <= 0.\n\
First `delete y scale', then `set y type log'.");
			}
		} else {
			err("Type must be `linear', `log', or `map'");
			return false;
		}
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	gr_setytransform(_ytype);
	return true;
}

// `set z missing above|below .intercept. .slope.'
bool
set_z_missingCmd()
{
	double slope, intercept;
	typedef enum { above, below, dont_know } WHERE;
	WHERE where = dont_know;
	switch (_nword) {
	case 6:
		if (!getdnum(_word[4], &intercept))
			return false;
		if (!getdnum(_word[5], &slope))
			return false;
		if (word_is(3, "above"))
			where = above;
		else if (word_is(3, "below"))
			where = below;
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (_colX.size() < 1) {
		err("First `read columns ... x'");
		return false;
	}
	if (_colY.size() < 1) {
		err("First `read columns ... y'");
		return false;
	}
	if (_colZ.size() < 1) {
		err("First `read columns ... z'");
		return false;
	}
	double missing = gr_currentmissingvalue();
	unsigned int i;
	switch (where) {
	case above:
		for (i = 0; i < _colX.size(); i++) {
			if (_colY[i] > intercept + slope * _colX[i])
				_colZ[i] = missing;
		}
		break;
	case below:
		for (i = 0; i < _colX.size(); i++) {
			if (_colY[i] < intercept + slope * _colX[i])
				_colZ[i] = missing;
		}
		break;
	default:
		demonstrate_command_usage();
		err("Fourth word must be `above' or `below'");
		return false;
	}

	return true;
}



bool
set_font_colorCmd()
{
	double          red, green, blue;
	GriColor       c;
	switch (_nword) {
	case 4:
		if (!look_up_color(_word[3], &red, &green, &blue)) {
			err("`set font color' given unknown colorname `\\", _word[3], "'.  Use command `show colornames' to see available colors.", "\\");
			return false;
		}
		c.setRGB(red, green, blue);
		_griState.set_color_text(c);
		_griState.set_separate_text_color(true);
		return true;
	case 7:
		if (strEQ(_word[3], "rgb")) {
			// `set color rgb .red. .green. .blue.'
			Require(getdnum(_word[4], &red), READ_WORD_ERROR(".red."));
			Require(getdnum(_word[5], &green), READ_WORD_ERROR(".green."));
			Require(getdnum(_word[6], &blue), READ_WORD_ERROR(".blue."));
			// Clip if necessary
			if (red < 0.0) {
				warning("`set color rgb' value being clipped to 0");
				red = 0.0;
			} else if (red > 1.0) {
				warning("`set color rgb' value being clipped to 0");
				red = 1.0;
			}
			if (green < 0.0) {
				warning("`set color rgb' .green. value being clipped to 0");
				green = 0.0;
			} else if (green > 1.0) {
				warning("`set color rgb' value being clipped to 0");
				green = 1.0;
			}
			if (blue < 0.0) {
				warning("`set color rgb' value being clipped to 0");
				blue = 0.0;
			} else if (blue > 1.0) {
				warning("`set color rgb' value being clipped to 0");
				blue = 1.0;
			}
			_griState.set_separate_text_color(true);
			c.setRGB(red, green, blue);
			_griState.set_color_text(c);
			return true;
		} else if (strEQ(_word[3], "hsb")) {
			// `set color hsb .hue. .saturation. .brightness.'
			double          hue, saturation, brightness;
			Require(getdnum(_word[4], &hue), READ_WORD_ERROR(".hue."));
			Require(getdnum(_word[5], &saturation), READ_WORD_ERROR(".saturation."));
			Require(getdnum(_word[6], &brightness), READ_WORD_ERROR(".brightness."));
			// Clip if necessary
			if (hue < 0.0) {
				warning("`set color hsb' value being clipped to 0");
				hue = 0.0;
			} else if (hue > 1.0) {
				warning("`set color hsb' value being clipped to 0");
				hue = 1.0;
			}
			if (saturation < 0.0) {
				warning("`set color hsb' value being clipped to 0");
				saturation = 0.0;
			} else if (saturation > 1.0) {
				warning("`set color hsb' value being clipped to 0");
				saturation = 1.0;
			}
			if (brightness < 0.0) {
				warning("`set color hsb' value being clipped to 0");
				brightness = 0.0;
			} else if (brightness > 1.0) {
				warning("`set color hsb' value being clipped to 0");
				brightness = 1.0;
			}
			c.setHSV(hue, saturation, brightness);
			_griState.set_color_text(c);
			_griState.set_separate_text_color(true);
			return true;
		} else {
			err("Can't understand command.");
			demonstrate_command_usage();
			return false;
		}
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
}

// set font encoding PostscriptStandard|isolatin1
bool
set_font_encodingCmd()
{
	if (_nword < 4) {
		demonstrate_command_usage();
		err("`set font encoding' needs to know which method to use ('PostscriptStandard' or 'isolatin1')");
		return false;
	}
	if (!strcmp(_word[3], "PostscriptStandard")) {
		gr_set_font_encoding(font_encoding_standard);
		return true;
	} else if (!strcmp(_word[3], "isolatin1")) {
		gr_set_font_encoding(font_encoding_isolatin1);
		return true;
	} else if (!strcmp(_word[3], "isolatinl")) {
		demonstrate_command_usage();
		err("`set font encoding' spelling error.  Did you mean `isolatin1' with a `one' at the end??");
		return false;
	} else {
		demonstrate_command_usage();
		err("`set font encoding' doesn't understand encoding method.  Choices are `PostscriptStandard' and `isolatin1'");
		return false;
	}
}
bool
set_font_sizeCmd()
{
	if (_nword < 3) {
		err("`set font' what?");
		return false;
	}
	if (!strcmp(_word[2], "size")) {
		if (_nword < 4) {
			err("`set font size' what?");
			return false;
		}
		if (_nword > 5) {
			err("extra words in `set font size' command");
			return false;
		}
		if (!strcmp(_word[3], "default")) {
			PUT_VAR("..fontsize..", FONTSIZE_PT_DEFAULT);
			gr_setfontsize_pt(FONTSIZE_PT_DEFAULT);
			return false;
		}
		if (!getdnum(_word[3], &tmp))
			return false;
		if (_nword == 5) {
			if (!strcmp(_word[4], "cm"))
				tmp *= PT_PER_CM;
			else {
				err("last word in `set font size' unknown");
				return false;
			}
		}
		if (tmp < 0.0 || tmp > 200.0) {
			err("ignoring bad font size <0 or >200 cm");
			return false;
		}
		PUT_VAR("..fontsize..", tmp);
		gr_setfontsize_pt(tmp);
	} else {
		err("`set font' what?");
		return false;
	}
	return true;
}

bool
set_font_toCmd()
{
	if (_nword != 4) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!strcmp(_word[3], "Courier"))
		gr_setfont(gr_font_Courier);
	else if (!strcmp(_word[3], "Helvetica"))
		gr_setfont(gr_font_Helvetica);
	else if (!strcmp(_word[3], "HelveticaBold"))
		gr_setfont(gr_font_HelveticaBold);
	else if (!strcmp(_word[3], "Palatino") || !strcmp(_word[3], "PalatinoRoman"))
		gr_setfont(gr_font_PalatinoRoman);
	else if (!strcmp(_word[3], "Symbol"))
		gr_setfont(gr_font_Symbol);

	else if (!strcmp(_word[3], "Times"))
		gr_setfont(gr_font_TimesRoman);
	else if (!strcmp(_word[3], "TimesRoman"))
		gr_setfont(gr_font_TimesRoman);
	else if (!strcmp(_word[3], "TimesBold"))
		gr_setfont(gr_font_TimesBold);

	else if (*_word[3] == '\\') {
		demonstrate_command_usage();
		err("Font name, specified as `\\", _word[3], "' should not have a backslash at the start.", "\\");
		return false;
	} else {
		demonstrate_command_usage();
		err("Unknown font `\\", _word[3], "'.\n  Available fonts: Courier, Helvetica, HelveticaBold, Palatino, PalatinoRoman, Symbol, Times, TimesRoman, TimesBold", "\\");
		return false;
	}
	return true;
}

bool
set_dashCmd()
{
	// Start by clearing existing dash list
	_dash.erase(_dash.begin(), _dash.end()); // go to solid
	extern FILE *_grPS;

	// Solid line
	if (word_is(_nword - 1, "off")) { // solid line
		fprintf(_grPS, "[] 0 d\n");
		return true;		// solid line
	}
	if (_nword == 2) {
		// `set dash'
		_dash.push_back(0.2);
		_dash.push_back(0.1);
		return true;
	}
	if (_nword == 3) {
		// `set dash .num.'
		if (!getdnum(_word[2], &tmp)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".n.");
			return false;
		}
		switch ((int) (0.5 + fabs((double) tmp))) {
		case 0:
			return true;
		case 1:
			_dash.push_back(0.2);
			_dash.push_back(0.1);
			return true;
		case 2:
			_dash.push_back(0.4);
			_dash.push_back(0.1);
			return true;
		case 3:
			_dash.push_back(0.6);
			_dash.push_back(0.1);
			return true;
		case 4:
			_dash.push_back(0.8);
			_dash.push_back(0.1);
			return true;
		case 5:
			_dash.push_back(1.0);
			_dash.push_back(0.1);
			return true;
		case 10:
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			return true;
		case 11:
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			_dash.push_back(2.0 * _griState.linewidth_line() / PT_PER_CM);
			return true;
		case 12:
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			_dash.push_back(3.0 * _griState.linewidth_line() / PT_PER_CM);
			return true;
		case 13:
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			_dash.push_back(4.0 * _griState.linewidth_line() / PT_PER_CM);
			return true;
		case 14:
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			_dash.push_back(5.0 * _griState.linewidth_line() / PT_PER_CM);
			return true;
		case 15:
			_dash.push_back(_griState.linewidth_line() / PT_PER_CM);
			_dash.push_back(6.0 * _griState.linewidth_line() / PT_PER_CM);
			return true;
		default:
			_dash.push_back(0.2);
			_dash.push_back(0.1);
			return true;
		}
	} else {
		// Long list of values
		for (unsigned int i = 0; i < _nword - 2; i++) {
			double tmp;
			if (!getdnum(_word[2 + i], &tmp)) {
				demonstrate_command_usage();
				err("Cannot read all dash/blank values");
				return false;
			}
			_dash.push_back(tmp);
		}
	}
	fprintf(_grPS, "[");
	for (unsigned int i = 0; i < _dash.size(); i++)
		fprintf(_grPS, "%.3f ", _dash[i] * PT_PER_CM);
	fprintf(_grPS, "] %d d\n", int(_dash.size()));
	return true;
}

bool
set_ignoreCmd()
{
	switch (_nword) {
	case 4:
		if (!strcmp(_word[1], "ignore")
		    && !strcmp(_word[2], "error")
		    && !strcmp(_word[3], "eof")) {
			_ignore_eof = true;
		} else {
			demonstrate_command_usage();
			err("Can't understand command.");
		}
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		break;
	}
	return false;
}

bool
set_input_data_windowCmd()
{
	double minval, maxval;
	switch (_nword) {
	case 6:			// set input data window x|y off
		if (strcmp(_word[5], "off")) {
			demonstrate_command_usage();
			err("In this context, the last word must be `off'.");
			return false;
		}
		if (!strcmp(_word[4], "x"))
			_input_data_window_x_exists = false;
		else if (!strcmp(_word[4], "y"))
			_input_data_window_y_exists = false;
		else {
			demonstrate_command_usage();
			err("In this context, the fifth word must be \"x\" or \"y\".");
			return false;
		}
		break;
	case 7:
		if (!strcmp(_word[4], "x")) {
			if (!getdnum(_word[5], &minval)) {
				demonstrate_command_usage();
				err("Can't read x window .min.");
				return false;
			}
			if (!getdnum(_word[6], &maxval)) {
				demonstrate_command_usage();
				err("Can't read x window .max.");
				return false;
			}
			if (minval == maxval) {
				demonstrate_command_usage();
				err("Can't have input data window of zero width.");
				return false;
			}
			if (minval < maxval) {
				_input_data_window_x_min = minval;
				_input_data_window_x_max = maxval;
			} else {
				_input_data_window_x_min = maxval;
				_input_data_window_x_max = minval;
			}
			_input_data_window_x_exists = true;
		} else if (!strcmp(_word[4], "y")) {
			if (!getdnum(_word[5], &minval)) {
				demonstrate_command_usage();
				err("Can't read y window .min.");
				return false;
			}
			if (!getdnum(_word[6], &maxval)) {
				demonstrate_command_usage();
				err("Can't read y window .max.");
				return false;
			}
			if (minval == maxval) {
				demonstrate_command_usage();
				err("Can't have input data window of zero width.");
				return false;
			}
			if (minval < maxval) {
				_input_data_window_y_min = minval;
				_input_data_window_y_max = maxval;
			} else {
				_input_data_window_y_min = maxval;
				_input_data_window_y_max = minval;
			}
			_input_data_window_y_exists = true;
		} else {
			demonstrate_command_usage();
			err("In this context, the fifth word must be \"x\" or \"y\".");
			return false;
		}
		break;
	default:
		demonstrate_command_usage();
		err("Can't understand command.");
		return false;
	}
	return true;
}

bool
set_input_data_separatorCmd()
{
	if (_nword < 5) {
		demonstrate_command_usage();
		err("Must specify an input separator (either `TAB' or `default').");
		return false;
	}
	if (_nword > 5) {
		demonstrate_command_usage();
		err("Too many words in `set input data separator' command");
		return false;
	}
	if (!strcmp(_word[4], "default")) {
		extern char _input_data_separator;
		_input_data_separator = ' ';
		return true;
	} else if (!strcmp(_word[4], "TAB")) {
		extern char _input_data_separator;
		_input_data_separator = '\t';
		return true;
	} else {
		demonstrate_command_usage();
		err("`set input data separator' only understands `TAB' and `default', not ``\\", _word[4], "' as given", "\\");
		return false;
	}
}
bool
set_contour_formatCmd()
{
	if (_nword < 4) {
		err("Must specify a format for `set contour format'");
		return false;
	}
	if (!strcmp(_word[3], "default")) {
		_contourFmt.assign(CONTOUR_FMT_DEFAULT);
	} else {
		if (*_word[3] == '"') {
			int len = strlen(_word[3]);
			if (len <= 1) {
				_contourFmt.assign(CONTOUR_FMT_DEFAULT);
			} else {
				if (*(_word[3] + len - 1) == '"')
					_contourFmt.assign(_word[3] + 1, len - 2);
				else
					_contourFmt.assign(_word[3] + 1, len - 1);
			}
		} else {
			_contourFmt.assign(_word[3]);
		}
	}
	return true;
}

// `set contour label position {.start_cm. .between_cm.}|default|centered'
// 0       1     2               3         4   5  6
// `set contour label for lines exceeding .x. cm'
// 0       1     2   3     4         5   6  7
bool
set_contour_labelCmd()
{
	extern double   _contour_space_first, _contour_space_later, _contour_minlength;	// <-> draw.c
	extern bool     _contour_space_centered;	// <-> draw.c
	if (word_is(3, "for")) {
		// `set contour label for lines exceeding .x. cm'
		Require(_nword == 8, NUMBER_WORDS_ERROR);
		Require(getdnum(_word[6], &tmp), READ_WORD_ERROR(".x."));
		_contour_minlength = tmp;
	} else {
		// `set contour label position ...'
		switch (_nword) {
		case 5:
			if (word_is(4, "default")) {
				_contour_space_first = -1.0;
				_contour_space_later = -1.0;
			} else if (word_is(4, "centered") || word_is(4, "centred")) {
				_contour_space_centered = true;
			} else {
				demonstrate_command_usage();
				err("Expecting `default', not '\\", _word[4], "'", "\\");
				return false;
			}
			break;
		case 6:
			Require(getdnum(_word[4], &tmp), READ_WORD_ERROR(".start_cm."));
			Require(getdnum(_word[5], &tmp2), READ_WORD_ERROR(".between_cm."));
			_contour_space_first = tmp;
			_contour_space_later = tmp2;
			_contour_space_centered = false;
			break;
		default:
			demonstrate_command_usage();
			err("Can't understand command.");
			return false;
		}
	}
	return true;
}

// `set contour labels rotated|horizontal'
// `set contour labels whiteunder|nowhiteunder'
// 0       1      2                  3 
bool
set_contour_labelsCmd()
{
	extern bool     _contour_label_rotated;	// <-> draw.c startup.c
	extern bool     _contour_label_whiteunder;	// <-> grcntour.c startup.c
	Require(_nword == 4, NUMBER_WORDS_ERROR);
	if (word_is(3, "rotated"))
		_contour_label_rotated = true;
	else if (word_is(3, "horizontal"))
		_contour_label_rotated = false;
	else if (word_is(3, "whiteunder"))
		_contour_label_whiteunder = true;
	else if (word_is(3, "nowhiteunder"))
		_contour_label_whiteunder = false;
	else {
		err("Last word of `set contour labels' must be `rotated|horizontal|whiteunder|nowhiteunder'");
		return false;
	}
	return true;
}

// Flags for `set flag'
#define NFLAG 100
typedef struct {
	std::string name;
	bool   value;
} FLAG;
FLAG flag[NFLAG];
unsigned int num_flags = 0;
bool
set_flagCmd()
{
	if (_nword != 3 && _nword != 4) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	bool turn_on = true;
	if (_nword == 4 & word_is(3, "off"))
		turn_on = false;
	unsigned int i;
	for (i = 0; i < num_flags; i++) {
		if (flag[i].name == _word[2]) {
			flag[i].value = turn_on;
			return true;
		}
	}
	// It is a new flag
	if (i >= NFLAG) OUT_OF_MEMORY; 
	num_flags = i;
	flag[num_flags].name = _word[2];
	flag[num_flags].value = turn_on;
	num_flags++;
	return true;
}

bool
get_flag(const char *name)
{
	for (unsigned int i = 0; i < num_flags; i++)
		if (flag[i].name == name)
			return flag[i].value;
	return false;		// unknown flag
}
void
show_flags()
{
	extern char _grTempString[];
	if (num_flags) { 
		for (unsigned int i = 0; i < num_flags; i++) {
			sprintf(_grTempString, "Flag `%s' is %d\n",
				flag[i].name.c_str(), flag[i].value);
			gr_textput(_grTempString);
		}
	} else {
		gr_textput("No flags exist\n");
	}
}

// set error action ...
bool
set_error_actionCmd()
{
	// set error action to core dump
	//   0     1      2  3    4    5
	if (_nword == 6 
	    && word_is(2, "action")
	    && word_is(3, "to")  
	    && word_is(4, "core") 
	    && word_is(5, "dump")) {
		_error_action = 1;
		return true;
	} else {
		demonstrate_command_usage();
		err("Wrong syntax");
		return false;
	}
}

bool
set_graylevelCmd()
{
	double          tmp;
	if (_nword != 3) {
		err("`set graylevel' requires 1 parameter");
		return false;
	}
	if (word_is(2, "black")) {
		tmp = 0.0;
	} else if (word_is(2, "white")) {
		tmp = 1.0;
	} else if (!getdnum(_word[2], &tmp)) {
		READ_WORD_ERROR(".brightness.");
		return false;
	}
	if (tmp < 0.0 || tmp > 1.0) {
		err("Ignoring bad graylevel <0 or >1");
		return false;
	}
	PUT_VAR("..graylevel..", tmp);
	PUT_VAR("..red..", tmp);
	PUT_VAR("..green..", tmp);
	PUT_VAR("..blue..", tmp);
	GriColor c;
	c.setRGB(tmp, tmp, tmp);
	_griState.set_color_line(c);
	if (_griState.separate_text_color() == false)
		_griState.set_color_text(c);
	return true;
}

// If x is very close to xmin or xmax, assign to same
double
tweak_to_limit(double x, double xmin, double xmax)
{
	double          range = fabs(xmax - xmin);
	if (fabs(x - xmin) < range / 1.0e4)
		return xmin;
	if (fabs(x - xmax) < range / 1.0e4)
		return xmax;
	return x;
}

// set image colorscale hsb .h. .s. .b. .im_value. hsb .h. .s. .b. .im_value.
// [increment .value.]
// 
// set image colorscale rgb .r. .g. .b. .im_value. rgb .r. .g. .b. .im_value.
// [increment .value.]
// 
// set image colorscale \name .im_value. \name .im_value. [increment .im_value.]
// 
// 0     1          2   3   4   5   6          7   8   9  10  11           12 13
// 14
bool
set_image_colorscaleCmd()
{
	double           rA, rB, gA, gB, bA, bB, hA, hB, sA, sB, brA, brB;
	double           r, g, b, h, s, br;
	double           valA, valB, indexA, indexB, inc, delta_image = 0.0;
	int             i, levels = 0;
	bool            used_rgb = false, gave_increment = false;
	// check number of words, for the 2 forms and with/without increment
	if (_nword != 13 && _nword != 15 && _nword != 7 && _nword != 9) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
	// Extract endpoint values.
	if (_nword == 7) {
		// `set image colorscale \name .im_value. \name .im_value.'
		if (!look_up_color(_word[3], &rA, &gA, &bA)) {
			err("Unknown colorname `\\", _word[3], "'.  Use command `show colornames' to see available colors.", "\\");
			return false;
		}
		if (!getdnum(_word[4], &valA))
			return false;
		if (!look_up_color(_word[5], &rB, &gB, &bB)) {
			err("Unknown colorname `\\", _word[5], "'.  Use command `show colornames' to see available colors.", "\\");
			return false;
		}
		if (!getdnum(_word[6], &valB))
			return false;
		gave_increment = false;
		used_rgb = true;		// that's how I'll blend them
	} else if (_nword == 9) {
// `set image colorscale \name .im_value. \name .im_value. increment
// .im_value.
		if (!look_up_color(_word[3], &rA, &gA, &bA)) {
			err("Unknown colorname `\\", _word[3], "'.  Use command `show colornames' to see available colors.", "\\");
			return false;
		}
		if (!getdnum(_word[4], &valA))
			return false;
		if (!look_up_color(_word[5], &rB, &gB, &bB)) {
			err("Unknown colorname `\\", _word[5], "'.  Use command `show colornames' to see available colors.", "\\");
			return false;
		}
		if (!getdnum(_word[6], &valB))
			return false;
		if (!getdnum(_word[8], &inc))
			return false;
		gave_increment = true;
		used_rgb = true;		// that's how I'll blend them
	} else {
		// Didn't give named color.  Must be rgb or hsb specification
		if (word_is(3, "rgb")) {
			if (!getdnum(_word[4], &rA))
				return false;
			if (!getdnum(_word[5], &gA))
				return false;
			if (!getdnum(_word[6], &bA))
				return false;
			if (!getdnum(_word[7], &valA))
				return false;
			used_rgb = true;
		} else if (word_is(3, "hsb")) {
			if (!getdnum(_word[4], &hA))
				return false;
			if (!getdnum(_word[5], &sA))
				return false;
			if (!getdnum(_word[6], &brA))
				return false;
			if (!getdnum(_word[7], &valA))
				return false;
			used_rgb = false;
		} else {
			err("Cannot understand first rgb/hsb specification");
			return false;
		}
		// Insist that rgb and hsb not be intermixed, since that's how to
		// decide on the blending algorithm
		if (word_is(8, "rgb")) {
			if (!used_rgb) {
				err("Cannot mix `rgb' and `hsb' color specifications");
				return false;
			}
			if (!getdnum(_word[9], &rB))
				return false;
			if (!getdnum(_word[10], &gB))
				return false;
			if (!getdnum(_word[11], &bB))
				return false;
			if (!getdnum(_word[12], &valB))
				return false;
			if (rA < 0.0 || rA > 1.0
			    || gA < 0.0 || gA > 1.0
			    || bA < 0.0 || bA > 1.0) {
				err("Require all RGB values to be in range 0.0 to 1.0");
				return false;
			}
		} else if (word_is(8, "hsb")) {
			if (used_rgb) {
				err("Cannot mix `hsb' and `rgb' color specifications.");
				return false;
			}
			if (!getdnum(_word[9], &hB))
				return false;
			if (!getdnum(_word[10], &sB))
				return false;
			if (!getdnum(_word[11], &brB))
				return false;
			if (!getdnum(_word[12], &valB))
				return false;
			if (hB < 0.0 || hB > 1.0
			    || sB < 0.0 || sB > 1.0
			    || brB < 0.0 || brB > 1.0) {
				err("Require all HSB values to be in range 0.0 to 1.0");
				return false;
			}
		} else {
			err("Cannot understand second rgb/hsb specification");
			return false;
		}
		// Get increment value if it exists.
		if (word_is(13, "increment")) {
			// then last word is the increment value
			if (!getdnum(_word[14], &inc))
				return false;
			gave_increment = true;
		}
	}
	// Check that endpoints distinct
	if (valA == valB) {
		err("Can't have equal image values at endpoints of `set colorscale'");
		return false;
	}
	if (gave_increment) {
		if (!well_ordered(valA, valB, inc))
			inc = -inc;
		if (!gr_multiple(fabs(valB - valA), fabs(inc), fabs(0.001 * inc))) {
			err("\
`set image colorscale ... requires range/increment to be integral to 0.1%");
			return false;
		}
		delta_image = 255.0 * fabs(inc / (valA - valB));
		levels = (int) floor(1.0e-5 + fabs((valA - valB) / inc));
	}
	// Calculate the transforms.  NB: indexA and indexB are of the
	// indices of transform endpoints.  Do RGB and HSB separately, each
	// interpolating linearly in its own space.  (NB: a nonlinear transform
	// links RGB and HSB, so linear movement between equivalent endpoints in
	// these two spaces yields nonequivalent values.)
	//
	indexA = 255.0 * (valA - _image0) / (_image255 - _image0);
	indexB = 255.0 * (valB - _image0) / (_image255 - _image0);
	if (used_rgb) {
		// Used rgb
		double           delta_r = delta_image * fabs(rA - rB);
		double           delta_g = delta_image * fabs(gA - gB);
		double           delta_b = delta_image * fabs(bA - bB);
		// For each target imageTransform[], make a clipped, linear blend
		// from (rA,gA,bA)  to (rB,gB,bB).  Possibly pass this linear blend
		// to 'quantize', to make it into a staircase function.
		for (i = 0; i < 256; i++) {
			r = 255.0
				* (rA + (rB - rA) * pin0_1((i - indexA) / (indexB - indexA)));
			g = 255.0 *
				(gA + (gB - gA) * pin0_1((i - indexA) / (indexB - indexA)));
			b = 255.0 *
				(bA + (bB - bA) * pin0_1((i - indexA) / (indexB - indexA)));
			if (gave_increment) {
				r = quantize(r, levels, delta_r);
				g = quantize(g, levels, delta_g);
				b = quantize(b, levels, delta_b);
			}
			_imageTransform[i] = (unsigned char) pin0_255(r);
			_imageTransform[i + 256] = (unsigned char) pin0_255(g);
			_imageTransform[i + 512] = (unsigned char) pin0_255(b);
		}
		// Ensure that endpoints match (rgb)A or (rgb)B, as appropriate.
		if (indexA < indexB) {
			_imageTransform[0] = (unsigned char) (255.0 * rA);
			_imageTransform[256] = (unsigned char) (255.0 * gA);
			_imageTransform[512] = (unsigned char) (255.0 * bA);
			_imageTransform[0 + 255] = (unsigned char) (255.0 * rB);
			_imageTransform[256 + 255] = (unsigned char) (255.0 * gB);
			_imageTransform[512 + 255] = (unsigned char) (255.0 * bB);
		} else {
			_imageTransform[0] = (unsigned char) (255.0 * rB);
			_imageTransform[256] = (unsigned char) (255.0 * gB);
			_imageTransform[512] = (unsigned char) (255.0 * bB);
			_imageTransform[0 + 255] = (unsigned char) (255.0 * rA);
			_imageTransform[256 + 255] = (unsigned char) (255.0 * gA);
			_imageTransform[512 + 255] = (unsigned char) (255.0 * bA);
		}
	} else {
		// Used hsb
		double           delta_h = delta_image * fabs(hA - hB);
		double           delta_s = delta_image * fabs(sA - sB);
		double           delta_br = delta_image * fabs(brA - brB);
		// For each target imageTransform[], make a clipped, linear blend
		// from (hA,sA,brA)  to (hB,sB,brB).  Possibly pass this linear blend
		// to 'quantize', to make it into a staircase function.
		for (i = 0; i < 256; i++) {
			h = (hA + (hB - hA) * pin0_1((i - indexA) / (indexB - indexA)));
			s = (sA + (sB - sA) * pin0_1((i - indexA) / (indexB - indexA)));
			br = (brA + (brB - brA) * pin0_1((i - indexA) / (indexB - indexA)));
			if (gave_increment) {
				// Quantize HSB *before* conversion to RGB
				h = quantize(255.0 * h, levels, delta_h) / 255.0;
				s = quantize(255.0 * s, levels, delta_s) / 255.0;
				br = quantize(255.0 * br, levels, delta_br) / 255.0;
			}
			gr_hsv2rgb(h, s, br, &r, &g, &b);
			_imageTransform[i] = (unsigned char) pin0_255(255.0 * r);
			_imageTransform[i + 256] = (unsigned char) pin0_255(255.0 * g);
			_imageTransform[i + 512] = (unsigned char) pin0_255(255.0 * b);
		}
		// Ensure that endpoints match (rgb)A or (rgb)B, as appropriate.
		gr_hsv2rgb(hA, sA, brA, &rA, &gA, &bA);
		gr_hsv2rgb(hB, sB, brB, &rB, &gB, &bB);
		if (indexA < indexB) {
			_imageTransform[0] = (unsigned char) (255.0 * rA);
			_imageTransform[256] = (unsigned char) (255.0 * gA);
			_imageTransform[512] = (unsigned char) (255.0 * bA);
			_imageTransform[0 + 255] = (unsigned char) (255.0 * rB);
			_imageTransform[256 + 255] = (unsigned char) (255.0 * gB);
			_imageTransform[512 + 255] = (unsigned char) (255.0 * bB);
		} else {
			_imageTransform[0] = (unsigned char) (255.0 * rB);
			_imageTransform[256] = (unsigned char) (255.0 * gB);
			_imageTransform[512] = (unsigned char) (255.0 * bB);
			_imageTransform[0 + 255] = (unsigned char) (255.0 * rA);
			_imageTransform[256 + 255] = (unsigned char) (255.0 * gA);
			_imageTransform[512 + 255] = (unsigned char) (255.0 * bA);
		}
	}
	// All done.  Everything was OK.
	_imageTransform_exists = true;
	_image_color_model = rgb_model;
	return true;
}

bool
set_image_grayscaleCmd()
{
	int             i;
	double          indexA;
	if (_nword < 3) {
		err("`set image' what?");
		return false;
	}
	// Do a (redundant?) check
	if (!word_is(2, "grayscale") && !word_is(2, "greyscale")) {
		err("`set image' what?");
		return false;
	}
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
	// `set image grayscale'
	if (_nword == 3) {
		for (i = 0; i < 256; i++)
			_imageTransform[i] = (unsigned char) i;
		_imageTransform_exists = true;
		_image_color_model = bw_model;
		return true;
	}
	// `set image grayscale [black .bl. white .wh. [increment .inc.]]'
	if (_nword > 3
	    && (!strcmp(_word[3], "black")
		|| !strcmp(_word[3], "white")
		|| !strcmp(_word[3], "increment"))) {
		double          inc = 0.0, valB, valA, scale;
		bool            gave_increment = false;
		if (1 == get_cmd_values(_word, _nword, "black", 1, _dstack))
			valA = _dstack[0];
		else {
			err("Can't read .bl. in [white .wh. black .bl.]");
			return false;
		}
		if (1 == get_cmd_values(_word, _nword, "white", 1, _dstack))
			valB = _dstack[0];
		else {
			err("Can't read .wh. in [white .wh. black .bl.]");
			return false;
		}
		if (1 == get_cmd_values(_word, _nword, "increment", 1, _dstack)) {
			inc = _dstack[0];
			gave_increment = true;
		}
		indexA = 255.0 * (valA - _image0) / (_image255 - _image0);
		scale = (_image255 - _image0) / (valB - valA);
		if (gave_increment) {
			// Set up quantized gray levels.  For example, `set image
			// grayscale white -1 black 1 increment 0.5' will set up 4 gray
			// levels; thus, _imageTransform[] will be set up with 4 bands,
			// containing the distinct values (1/5, 2/5, 3/5, 4/5) * 255.
			//
			// BUG -- this quantized graylevel code has had lots of bugs
			double          delta_image;	// image change per level
			int             levels;	// number of levels
			if (!well_ordered(valA, valB, inc))
				inc = -inc;
			if (!gr_multiple(valB - valA, inc, 0.001 * inc)) {
				err("\
`set image grayscale black .bl. white .wh. increment .inc.'\n\
  has (.wh. - .bl.) not a multiple of .inc. to within 0.1%");
				return false;
			}
			if (valA == valB) {
				err("\
`set image grayscale black .bl. white .wh. increment .inc.'\n\
  has .wh. = .bl., which is not allowed.");
				return false;
			}
			delta_image = 255.0 * fabs(inc / (valB - valA));
			levels = (int) floor(1.0e-5 + fabs((valB - valA) / inc));
			for (i = 0; i < 256; i++) {
				// The .001 below is to prevent rounding problems.
				_imageTransform[i] = (unsigned char)
					floor(pin0_255((int) floor(.001 + quantize(scale * (i - indexA), levels, delta_image))));
			}
			// BUG -- the following is a total kludge, because I could not
			// find an adequate rounding macro
			_imageTransform[0] = (_imageTransform[0] <= 128) ? 0 : 255;
			_imageTransform[255] = (_imageTransform[255] <= 128) ? 0 : 255;
			_imageTransform_exists = true;
			_image_color_model = bw_model;
			return true;
		} else {
			// Increment was not given.
			for (i = 0; i < 256; i++) {
				_imageTransform[i]
					= (unsigned char) floor(pin0_255((int) floor(scale * (i - indexA))));
			}
		}
		_imageTransform_exists = true;
		_image_color_model = bw_model;
		return true;
	}
	demonstrate_command_usage();
	err("Can't understand command.");
	return false;
}

bool
set_image_grayscale_using_hist()
{
	int             i;
	double          sum = 0.0, range;
	double          wh, bl;
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
	// Since parser got to this point, must at least match `set image
	// grayscale using histogram'
	if (_nword == 5) {
		// `set image grayscale using histogram'
		calculate_image_histogram();
		// The range is sum(255) - sum(0), but sum(255) is unity, because
		// _imageHist[] is defined that way, and sum(0) is _imageHist[0].
		range = 1.0 - _imageHist[0];
		if (range == 0.0)
			range = 1.0;
		for (i = 0; i < 256; i++) {
			_imageTransform[i] = (unsigned char) floor(pin0_255(255.0 * (1.0 - (sum - _imageHist[0]) / range)));
			sum += _imageHist[i];
		}
		_imageTransform_exists = true;
	} else if (_nword == 9) {
		// `set image grayscale using histogram black .bl. white .wh.'
		int             start, end;
		double          sum_to_end = 0.0, sum_to_start = 0.0;
		if (1 == get_cmd_values(_word, _nword, "black", 1, _dstack)) {
			bl = _dstack[0];
		} else {
			err("Can't read .bl. in [black .bl. white .wh.]");
			return false;
		}
		if (1 == get_cmd_values(_word, _nword, "white", 1, _dstack)) {
			wh = _dstack[0];
		} else {
			err("Can't read .wh. in [black .bl. white .wh.]");
			return false;
		}
		calculate_image_histogram();
		start = value_to_image(wh);	// pixel
		end = value_to_image(bl);	// pixel
		for (i = 0; i < start; i++)
			sum_to_start += _imageHist[i];
		for (i = 0; i < end; i++)
			sum_to_end += _imageHist[i];
		range = sum_to_end - sum_to_start;
		if (range == 0.0)
			range = 1.0;
		for (i = 0; i < 256; i++) {
			_imageTransform[i] = (unsigned char) floor(pin0_255(255.0 * (1.0 - (sum - sum_to_start) / range)));
			sum += _imageHist[i];
		}
		_imageTransform_exists = true;
	} else {
		demonstrate_command_usage();
		err("Can't understand command.");
		return false;
	}
	_image_color_model = bw_model;
	return true;
}

// `set image missing value color to white|black|{graylevel .brightness.}'
bool
set_image_missingCmd()
{
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
	if (_nword == 7) { // `set image missing value color to white|black'
		if (word_is(6, "white")) {
			_image_missing_color_red = 1.0;
			_image_missing_color_green = 1.0;
			_image_missing_color_blue = 1.0;
		} else if (word_is(6, "black")) {
			_image_missing_color_red = 0.0;
			_image_missing_color_green = 0.0;
			_image_missing_color_blue = 0.0;
		} else {
			demonstrate_command_usage();
			err("Unknown color specified for image missing-value.");
			return false;
		}
	} else if (_nword == 10) {
		// `set image missing value color to rgb .red. .green. .blue.'
		if (!strcmp(_word[6], "rgb")) {
			double red, green, blue;
			Require(getdnum(_word[7], &red), READ_WORD_ERROR(".red."));
			Require(getdnum(_word[8], &green), READ_WORD_ERROR(".green."));
			Require(getdnum(_word[9], &blue), READ_WORD_ERROR(".blue."));
			// Clip if necessary
			CHECK_RGB_RANGE(red);
			CHECK_RGB_RANGE(green);
			CHECK_RGB_RANGE(blue);
			_image_missing_color_red = red;
			_image_missing_color_green = green;
			_image_missing_color_blue = blue;
		}
	} else {
		// `set image missing value color to {graylevel .brightness.}'
		if (1 == get_cmd_values(_word, _nword, "graylevel", 1, _dstack)
		    || 1 ==get_cmd_values(_word, _nword, "greylevel", 1, _dstack)) {
			if (_dstack[0] < 0.0) {
				warning("`set image missing value color': clipping graylevel to 0.0");
				_image_missing_color_red = 0.0;
				_image_missing_color_green = 0.0;
				_image_missing_color_blue = 0.0;
			} else if (_dstack[0] > 1.0) {
				warning("`set image missing value color': clipping graylevel to 0.0");
				_image_missing_color_red = 1.0;
				_image_missing_color_green = 1.0;
				_image_missing_color_blue = 1.0;
			} else {
				_image_missing_color_red = _dstack[0];
				_image_missing_color_green = _dstack[0];
				_image_missing_color_blue = _dstack[0];
			}
		} else {
			demonstrate_command_usage();
			err("Can't understand color specification.");
			return false;
		}
	}
	return true;
}

bool
set_image_rangeCmd()
{
	double          tmp1, tmp2;
	switch (_nword) {
	case 5:
		if (!getdnum(_word[3], &tmp1))
			return false;
		if (!getdnum(_word[4], &tmp2))
			return false;
		_image0 = tmp1;
		_image255 = tmp2;
		return true;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
}

// `set grid missing inside|outside       curve'
// `set grid missing  above|below   .intercept. .slope'
//    0    1       2            3             4      5
bool
set_grid_missingCmd()
{
	double intercept, slope;
	typedef enum { above, below, inside, outside, dont_know } WHERE;
	WHERE where = dont_know;
	switch(_nword) {
	case 5:
		if (word_is(3, "inside"))
			where = inside;
		else if (word_is(3, "outside"))
			where = outside;
		break;
	case 6:
		if (!getdnum(_word[4], &intercept))
			return false;
		if (!getdnum(_word[5], &slope))
			return false;
		if (word_is(3, "above"))
			where = above;
		else if (word_is(3, "below"))
			where = below;
		break;
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!grid_exists())
		return false;
	unsigned int i, j;
	switch (where) {
	case above:
		for (i = 0; i < _num_xmatrix_data; i++)
			for (j = 0; j < _num_ymatrix_data; j++)
				if (_ymatrix[j] > intercept + slope * _xmatrix[i])
					_legit_xy(i, j) = false;
		return true;		// ok
	case below:
		for (i = 0; i < _num_xmatrix_data; i++)
			for (j = 0; j < _num_ymatrix_data; j++)
				if (_ymatrix[j] < intercept + slope * _xmatrix[i])
					_legit_xy(i, j) = false;
		return true;		// ok
	case inside:
		return set_grid_missing_curve(true);
	case outside:
		return set_grid_missing_curve(false);
	default:
		demonstrate_command_usage();
		err("Fourth word must be `above', `below', `inside' or `outside'");
		return false;
	}
}

bool
set_grid_missing_curve(bool inside)
{
	if (!inside) {
		err("'set grid missing outside curve' not allowed");
		return false;
	}
	unsigned int i, start = 0;
	for (i = 0; i < _colX.size(); i++) {
		if (gr_missingx(_colX[i]) 
		    || gr_missingy(_colY[i])
		    || i > _colX.size() - 1) {
			if (!mask_an_island(_colX.begin() + start,
					    _colY.begin() + start, i - start))
				return false;	// problem
			while ((gr_missingx(_colX[i]) || gr_missingy(_colY[i]))
			       && i < _colX.size())
				i++;
			start = i;
		}
	}
	if (i <= _colX.size()) {
		mask_an_island(_colX.begin() + start,
			       _colY.begin() + start, 
			       i - start);
	}
	return true;
}

bool
mask_an_island(double *x, double *y, unsigned int n)
{
	unsigned right_edge = 0;
	double xmax = x[0];
	unsigned int i;
	for (i = 1; i < n; i++) {
		if (x[i] > xmax) {
			xmax = x[i];
			right_edge = i;
		}
	}
//DEBUG printf("\nIsland:\n");
//DEBUG for (i = 0; i < n; i++) {
//DEBUG     printf("    %f %f\n", x[i], y[i]);
//DEBUG }
//DEBUG printf("Rotates to:\n");
//DEBUG for (i = 0; i < n; i++) {
//DEBUG     int ii = (i + right_edge) % n;
//DEBUG     printf("    %f %f\n", x[ii], y[ii]);
//DEBUG }
	for (i = 0; i < n; i++) {
		int ii = (i + right_edge) % n;
		for (unsigned int ixm = 0; ixm < _num_xmatrix_data; ixm++) {
			if (between(_xmatrix[ixm], x[(ii+1)%n], x[ii])) {
//DEBUG printf("mask out ixm=%d at %f, bracketed by %f - %f\n", ixm, _xmatrix[ixm], x[(ii+1)%n],x[ii]);
				for (int iym = (int)_num_ymatrix_data - 1; iym > -1; iym--) {
					if (_ymatrix[iym] <=
					    interpolate_linear(_xmatrix[ixm], 
							       x[ii], y[ii], 
							       x[(ii+1)%n], y[(ii+1)%n])) {
						// Reverse things below
//DEBUG printf("reverse below y(%d)=%f\n", iym, _ymatrix[iym]);
						while (iym > -1) {
							_legit_xy(ixm, iym) = _legit_xy(ixm, iym) == true
								? false : true;
							iym--;
						}
//DEBUG printf("i=%d; grid=\n", i); show_grid_maskCmd();
						break;	// just in case
					}
				}
			}
		}
	}
	return true;
}

static          bool
width_rapidograph(char *s, double *w)
{
// Named pen sizes, following the notation of Rapidograph(TM)
// technical drawing pens.
#define NUM_RAPIDOGRAPH 16
	typedef struct {
		const char *name;		// allow both 'X' and 'x' in names
		float       width;		// in points
	} r;
	r r_table[NUM_RAPIDOGRAPH] = {
		{"6x0", 0.369},
		{"6X0", 0.369},
		{"4x0", 0.510},
		{"4X0", 0.510},
		{"3x0", 0.709},
		{"3X0", 0.709},
		{"00",  0.850},
		{"0",   0.992},
		{"1",   1.417},
		{"2",   1.701},
		{"2.5", 1.984},
		{"3",   2.268},
		{"3.5", 2.835},
		{"4",   3.402},
		{"6",   3.969},
		{"7",   5.669}
	};
	std::string ss(s);
	un_double_quote(ss);
	for (int i = 0; i < NUM_RAPIDOGRAPH; i++)
		if (!strcmp(ss.c_str(), r_table[i].name)) {
			*w = r_table[i].width;
			return true;
		}
	err("Unknown rapidograph pen size `\\", s, "' requested.\n       Permitted sizes: 6x0, 4x0, 3x0, 00, 0, 1, 2, 2.5, 3, 3.5, 4, 6, 7", "\\");
	return false;
}

bool
set_line_capCmd()
{
	if (_nword != 4) {
		err("`set line cap .type.' has wrong number of words on command line");
		return false;
	}
	double tmp;
	if (!getdnum(_word[3], &tmp)) {
		READ_WORD_ERROR(".type.");
		return false;
	}
	int t = int(floor(0.5 + tmp));
	if (t < 0 || t > 2) {
		err("`set line cap .type.' only permits types 0, 1 and 2");
		return false;
	}
	_griState.set_line_cap(t);
	return true;
}

bool
set_line_joinCmd()
{
	if (_nword != 4) {
		err("`set line join .type.' has wrong number of words on command line");
		return false;
	}
	double tmp;
	if (!getdnum(_word[3], &tmp)) {
		READ_WORD_ERROR(".type.");
		return false;
	}
	int t = int(floor(0.5 + tmp));
	if (t < 0 || t > 2) {
		err("`set line join .type.' only permits types 0, 1 and 2");
		return false;
	}
	_griState.set_line_join(t);
	return true;
}

bool
set_line_widthCmd()
{
	//show_words();
	double          w;		// the width, in pt
	unsigned int    skip = 0;
	int             what = 0;	// -1=curve/rapido 0=curve 1=axis 2=symbol 3=all
	if (word_is(3, "axis")) {
		skip = 1;
		what = 1;
	} else if (word_is(3, "symbol")) {
		skip = 1;
		what = 2;
	} else if (word_is(3, "all")) {
		skip = 1;
		what = 3;
	} else if (word_is(3, "rapidograph")) {
		what = -1;
	} else {
		if (_nword > 4) {
			err("`set line width' found unexpected word `\\", _word[3], "'.", "\\");
			return false;
		}
	}
	// Check for 'set line width ...' with no width specification
	if (_nword <= (skip + 3)) {
		NUMBER_WORDS_ERROR;
		return false;
	}
	// Take care of 'default', simple, and 'rapidograph' styles
	if (word_is(3 + skip, "default")) {
		Require(_nword == (4 + skip), NUMBER_WORDS_ERROR);
		switch (what) {
		case 0:
			w = LINEWIDTH_DEFAULT;
			break;
		case 1:
			w = LINEWIDTHAXIS_DEFAULT;
			break;
		case 2:
			w = LINEWIDTHSYMBOL_DEFAULT;
			break;
		case 3:
			w = LINEWIDTH_DEFAULT;
			break;
		default:
			w = LINEWIDTHSYMBOL_DEFAULT;
		}
	} else if (_nword == (4 + skip) && !word_is(3, "rapidograph")) {
		// Simple case
		if (what == -1) {
			if (!width_rapidograph(_word[3 + skip], &w)) {
				err("`set line width' cannot understand rapidograph name");
				return false;
			}
		} else {
			if (!getdnum(_word[3 + skip], &w))
				return false;
		}
	} else if (_nword == (5 + skip)) {
		// Rapidograph style
		if (word_is(3 + skip, "rapidograph")) {
			if (!width_rapidograph(_word[4 + skip], &w)) {
				err("`set line width' cannot understand rapidograph name");
				return false;
			}
		} else {
			err("`set line width' expecting word `rapidograph'");
			return false;
		}
	} else {
		if (word_is(_nword - 1, "rapidograph")) {
			err("`set line width rapidograph' needs a pen-width name");
		} else {
			err("`set line width' cannot understand the width");
		}
		return false;
	}
	// check that w is not crazily small ... a common blunder.  See if less
	// than 1 dot on a 600dpi printer.
	if (w != 0.0) {
		if (w / PT_PER_CM < 2.54 / 4800) {
			char msg[200];
			sprintf(msg, 
				"a line width of %g points is barely resolved on a 4800 dpi printer",
				w);
			warning(msg);
		} else if (w / PT_PER_CM < 2.54 / 2400) {
			char msg[200];
			sprintf(msg, 
				"a line width of %g points is barely resolved on a 2400 dpi printer",
				w);
			warning(msg);
		} else if (w / PT_PER_CM < 2.54 / 600) {
			char msg[200];
			sprintf(msg, 
				"a line width of %g points is barely resolved on a 600 dpi printer",
				w);
			warning(msg);
		} else if (w / PT_PER_CM < 2.54 / 400) {
			char msg[200];
			sprintf(msg, 
				"a line width of %g points is barely resolved on a 400 dpi printer",
				w);
			warning(msg);
		}
	}
	switch (what) {
	case -1:
	case 0:
		_griState.set_linewidth_line(w);
		PUT_VAR("..linewidth..", w);
		break;
	case 1:
		_griState.set_linewidth_axis(w);
		PUT_VAR("..linewidthaxis..", w);
		break;
	case 2:
		_griState.set_linewidth_symbol(w);
		PUT_VAR("..linewidthsymbol..", w);
		break;
	case 3:
		_griState.set_linewidth_line(w);
		PUT_VAR("..linewidth..", w);
		_griState.set_linewidth_axis(w);
		PUT_VAR("..linewidthaxis..", w);
		_griState.set_linewidth_symbol(w);
		PUT_VAR("..linewidthsymbol..", w);
		break;
	default:
		_griState.set_linewidth_line(w);
		PUT_VAR("..linewidthline..", w);
	}
	return true;
}

bool
set_missing_valueCmd()
{
	// `set missing value #'
	double          tmp;
	if (_nword == 4) {
		if (getdnum(_word[3], &tmp)) {
			gr_setmissingvalue(tmp);
			PUT_VAR("..missingvalue..", gr_currentmissingvalue());
			{
				char tmp[100];
				sprintf(tmp, "%f", gr_currentmissingvalue());
				put_syn("\\.missingvalue.", tmp, true);
			}
		} else {
			// Actually, can not reach this code, with current error checking
			// in getdnum().
			demonstrate_command_usage();
			READ_WORD_ERROR(".missing_value.");
		}
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
}

bool
set_page_sizeCmd()
{
	Require(_nword == 4, err("`set page size' requires 1 parameter"));
	const char *s = _word[3];	// save typing
	extern rectangle _page_size;
	if (!strcmp(s, "letter")) {
		_page_size.set(0.0, 0.0, 8.5, 11.0);
		_page_size.scale(CM_PER_IN);
	} else if (!strcmp(s, "legal")) {
		_page_size.set(0.0, 0.0, 8.5, 14.0);
		_page_size.scale(CM_PER_IN);
	} else if (!strcmp(s, "folio")) {
		_page_size.set(0.0, 0.0, 8.5, 13.0);
		_page_size.scale(CM_PER_IN);
	} else if (!strcmp(s, "tabloid")) {
		_page_size.set(0.0, 0.0, 11.0, 17.0);
		_page_size.scale(CM_PER_IN);
	} else if (!strcmp(s, "A5")) {
		_page_size.set(0.0, 0.0, 14.8, 21.0);
	} else if (!strcmp(s, "A4")) {
		_page_size.set(0.0, 0.0, 21.0, 29.7);
	} else if (!strcmp(s, "A3")) {
		_page_size.set(0.0, 0.0, 29.7, 42.0);
	} else if (!strcmp(s, "A2")) {
		_page_size.set(0.0, 0.0, 42.0, 59.4);
	} else if (!strcmp(s, "A1")) {
		_page_size.set(0.0, 0.0, 59.4, 84.1);
	} else if (!strcmp(s, "A0")) {
		_page_size.set(0.0, 0.0, 84.1, 118.9);
	} else {
		demonstrate_command_usage();
		err("Unknown paper size `\\", s, "'.", "\\");
		return false;
	}
	return true;
}

bool
set_pageCmd()
{
	double          mag, xcm, ycm;
	Require(_nword > 2, err("`set page' requires parameter(s)"));
	if (!strcmp(_word[2], "portrait")) {
		;			// EMPTY
	} else if (!strcmp(_word[2], "landscape")) {
		if (!already_landscape) {
			fprintf(gr_currentPSFILEpointer(), "8.5 72 mul 0 translate 90 rotate %% Landscape\n");
			check_psfile();
		}
		already_landscape = true;
		PUT_VAR("..landscape..", 1.0);
		gr_setup_ps_landscape();
	} else if (!strcmp(_word[2], "factor")) {
		if (_nword != 4) {
			err("Must specify .mag. in `set page factor .mag.'");
			return false;
		}
		if (!getdnum(_word[3], &mag))
			return false;
		if (mag <= 0.0) {
			err("Can't use negative enlargement factor");
			return false;
		}
		gr_setscale(mag, mag);
	} else if (!strcmp(_word[2], "translate")) {
		if (_nword != 5) {
			err("Must specify .xcm. and .ycm. in `set page translate .xcm. .ycm.'");
			return false;
		}
		if (!getdnum(_word[3], &xcm))
			return false;
		if (!getdnum(_word[4], &ycm))
			return false;
		gr_settranslate(xcm, ycm);
	} else {
		err("Unknown `set page' parameter");
		return false;
	}
	return true;
}

//`set path to "\path"|default for data|commands'
bool
set_pathCmd()
{
	if (_nword != 6) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (strNE(_word[4], "for")) {
		err("Fourth word must be `for', not `\\", _word[4], "' as given.", "\\");
		return false;
	}
	char *which_path;
	if (strEQ(_word[5], "data")) 
		which_path = "\\.path_data.";
	else if (strEQ(_word[5], "commands")) 
		which_path = "\\.path_commands.";
	else {
		err("Sixth word must be `data' or `commands', not `\\", _word[5], "' as given.", "\\");
		return false;
	}
	if (strEQ(_word[3], "default")) {
		if (!put_syn(which_path, ".", true)) {
			err("Internal error in setting path to default.");
			return false;
		}
	} else {
		std::string unquoted;
		int ok = ExtractQuote(_word[3], unquoted);
		if (ok) {
			if (!put_syn(which_path, unquoted.c_str(), true)) {	
				err("`set path' cannot save `\\", _word[2], "' in synonym", which_path, "\\");
				return false;
			} 
		} else {
			err("`set path' cannot understand path `\\", _word[2], "'.", "\\");
			return false;
		}
	}
	return true;
}

bool
set_postscript_filenameCmd()
{
	if (_nword != 4) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!gr_reopen_postscript(_word[3])) {
		demonstrate_command_usage();
		warning("Cannot open `\\", _word[3], "', so using old name.", "\\");
	}
	return true;
}

bool
set_symbol_sizeCmd()
{
	if (_nword < 3) {
		err("`set symbol' what?");
		return false;
	}
	if (!strcmp(_word[2], "size")) {
		if (!strcmp(_word[3], "default")) {
			tmp = SYMBOLSIZE_DEFAULT;
			PUT_VAR("..symbolsize..", tmp);
			gr_setsymbolsize_cm(SYMBOLSIZE_DEFAULT);
			return true;
		}
		if (!getdnum(_word[3], &tmp))
			return false;
		if (tmp < 0.0 || tmp > 20.0) {
			err("Ignoring bad symbol size <0 or >20 cm");
			return false;
		}
		PUT_VAR("..symbolsize..", tmp);
		gr_setsymbolsize_cm(tmp);
	} else {
		err("`set symbol' what?");
		return false;
	}
	return true;
}

bool
set_tic_sizeCmd()
{
	if (_nword < 3) {
		err("`set tic' what?");
		return false;
	}
	if (!strcmp(_word[2], "size")) {
		if (_nword < 4) {
			err("`set tic size' what?");
			return false;
		}
		if (_nword > 4) {
			err("Extra words in `set tic size' command");
			return false;
		}
		if (!strcmp(_word[3], "default")) {
			tmp = TICSIZE_DEFAULT;
			PUT_VAR("..tic_size..", tmp);
			return true;
		}
		if (!getdnum(_word[3], &tmp))
			return false;
		if (tmp < 0.0 || tmp > 20.0) {
			err("Ignoring bad tic size <0 or >20 cm");
			return false;
		}
		PUT_VAR("..tic_size..", tmp);
	} else {
		err("`set tic' what?");
		return false;
	}
	return true;
}

bool
set_ticsCmd()
{
	if (_nword != 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!strcmp(_word[2], "in")) {
		PUT_VAR("..tic_direction..", 1.0);
	} else if (!strcmp(_word[2], "out")) {
		PUT_VAR("..tic_direction..", 0.0);
	} else {
		demonstrate_command_usage();
		err("Third word must be \"in\" or \"out\".");
		return false;
	}
	return true;
}

bool
set_u_scaleCmd()
{
	double          xsize;
	if (_nword < 3) {
		demonstrate_command_usage();
		err("`set u' ?WHAT?");
		return false;
	}
	if (strcmp(_word[2], "scale")) {
		demonstrate_command_usage();
		err("`set u ?WHAT?' (try `set u scale ...'");
		return false;
	}
	switch (_nword) {
	case 4:
		// `set u scale .cm_per_unit.'
		if (!getdnum(_word[3], &tmp))
			return false;
		if (tmp != 0.0) {
			_cm_per_u = tmp;
			_uscale_exists = true;
			return true;
		} else {
			err("`set u scale 0' illegal");
			return false;
		}
	case 5:
		// `set u scale as x'
		if (strcmp(_word[3], "as") || strcmp(_word[4], "x")) {
			err("Correct usage `set u scale as x'");
			return false;
		}
		if (!_xscale_exists) {
			err("First `set x axis' or `read columns x ...'");
			return false;
		}
		if (_xtype != gr_axis_LINEAR) {
			err("Can only `set u scale as x' if x is LINEAR");
			return false;
		}
		if (!get_var("..xsize..", &xsize)) {
			err("Can't remember ..xsize..");
			return false;
		}
		_cm_per_u = xsize / (_xright - _xleft);
		_uscale_exists = true;
		return true;
	default:
		err("`set u' what?");
		return false;
	}
}

bool
set_v_scaleCmd()
{
	double          ysize;
	if (_nword < 3) {
		err("`set v' ?WHAT?");
		return false;
	}
	if (strcmp(_word[2], "scale")) {
		err("`set v ?WHAT?' (try `set v scale ...'");
		return false;
	}
	switch (_nword) {
	case 4:
		// `set v scale .cm_per_unit.'
		if (!getdnum(_word[3], &tmp))
			return false;
		if (tmp != 0.0) {
			_cm_per_v = tmp;
			_vscale_exists = true;
			return true;
		} else {
			err("`set v scale 0' illegal");
			return false;
		}
	case 5:
		// `set v scale as y'
		if (strcmp(_word[3], "as") || strcmp(_word[4], "y")) {
			demonstrate_command_usage();
			err("Correct usage `set v scale as y'");
			return false;
		}
		if (!_yscale_exists) {
			demonstrate_command_usage();
			err("First `set y axis' or `read columns y ...'");
			return false;
		}
		if (_ytype != gr_axis_LINEAR) {
			demonstrate_command_usage();
			err("Can only `set u scale as y' if y is LINEAR");
			return false;
		}
		if (!get_var("..ysize..", &ysize)) {
			demonstrate_command_usage();
			err("Can't remember ..ysize..");
			return false;
		}
		_cm_per_v = ysize / (_ytop - _ybottom);
		_vscale_exists = true;
		return true;
	default:
		err("`set v' what?");
		return false;
	}
}

bool
set_traceCmd()
{
	switch (_nword) {
	case 2:
		PUT_VAR("..trace..", 1.0);
		break;
	case 3:
		if (!strcmp(_word[2], "on")) {
			PUT_VAR("..trace..", 1.0);
		} else if (!strcmp(_word[2], "off")) {
			PUT_VAR("..trace..", 0.0);
		}
		break;
	default:
		break;
	}
	return true;
}

bool
well_ordered(double min, double max, double inc)
{
	if (min < max)
		return ((inc > 0.0) ? true : false);
	else
		return ((inc < 0.0) ? true : false);
}
bool
inc_within_range(double min, double max, double inc)
{
	if (min < max)
		return (min + inc < max);
	else
		return (min + inc > max);
}

bool
set_x_axisCmd()
{
	_xatbottom = true;
#if 1				// 2.9.x
	if (word_is(3, "labels")) {
		if (word_is(4, "automatic")) {
			_x_labels.erase(_x_labels.begin(), _x_labels.end());
			_x_label_positions.erase(_x_label_positions.begin(), _x_label_positions.end());
			return true;
		} else {
			unsigned int start = 4;
			if (word_is(start, "add")) {
				start++;
			} else {
				_x_labels.erase(_x_labels.begin(), _x_labels.end());
				_x_label_positions.erase(_x_label_positions.begin(), _x_label_positions.end());
			}
			for (unsigned int i = start; i < _nword; i++) {
				double tmp;
				if (!getdnum(_word[i], &tmp)) {
					READ_WORD_ERROR(".pos.");
					demonstrate_command_usage();
					return false;
				}
				_x_label_positions.push_back(tmp);
				if (i++ == _nword - 1) {
					err("Missing label to be applied at position \\", _word[i-1], "\\");
					demonstrate_command_usage();
					return false;
				}
				std::string l = _word[i];
				un_double_quote(l);
				_x_labels.push_back(l);

			}
			return true;
		}
	}
#endif // 2.9.x
	if (!strcmp(_word[_nword - 1], "bottom")) {
		_xatbottom = true;
		if (_nword == 4) {
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		}
		_nword--;
	} else if (!strcmp(_word[_nword - 1], "top")) {
		_xatbottom = false;
		if (_nword == 4) {
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		}
		_nword--;
	} else if (_nword == 4 && !strcmp(_word[_nword - 1], "increasing")) {
		_xincreasing = true;
		if (_xscale_exists && _xleft > _xright) {
			swap(_xleft, _xright);
			_xinc = -fabs(_xinc);
			PUT_VAR("..xleft..", _xleft);
			PUT_VAR("..xright..", _xright);
			PUT_VAR("..xinc..", _xinc);
		}
		return true;
	} else if (_nword == 4 && !strcmp(_word[_nword - 1], "decreasing")) {
		_xincreasing = false;
		if (_xscale_exists && _xleft < _xright) {
			swap(_xleft, _xright);
			_xinc = fabs(_xinc);
			PUT_VAR("..xleft..", _xleft);
			PUT_VAR("..xright..", _xright);
			PUT_VAR("..xinc..", _xinc);
		}
		return true;
	} else if (_nword == 4 && !strcmp(_word[_nword - 1], "unknown")) {
		_xscale_exists = false;
		_need_x_axis = true;
		_user_set_x_axis = false;
		return true;
	}
	// ... specifying x axis
	// 'set x axis .left. .right.'
	if (_nword == 5) {
		if (!getdnum(_word[3], &xleft) || !getdnum(_word[4], &xright)) {
			READ_WORD_ERROR(".left. and .right.");
			return false;
		}
		if (_xtype == gr_axis_LOG) {
			Require(xleft > 0.0,
				err("`set x axis .left. .right.' cannot have non-positive .left. value for logarithmic axis"));
			Require(xright > 0.0,
				err("`set x axis .left. .right.' cannot have non-positive .right. value for logarithmic axis"));
		}
		_xleft = xleft;
		_xright = xright;
		if (_xtype == gr_axis_LOG)
			_xinc = 1.0;
		else
			_xinc = _xright - _xleft;
		PUT_VAR("..xleft..", _xleft);
		PUT_VAR("..xright..", _xright);
		PUT_VAR("..xinc..", _xinc);
		_xsubdiv = 1;
		_xscale_exists = true;
		_need_x_axis = true;
		_need_y_axis = true;
		_user_set_x_axis = true;
		return true;
	} else if (_nword == 6) { // 'set x axis .left. .right. .inc.'
		if (!getdnum(_word[3], &xleft)
		    || !getdnum(_word[4], &xright)
		    || !getdnum(_word[5], &xinc)) {
			// 'set x axis .left. .right. .incBig.'
			READ_WORD_ERROR(".left. .right. .incBig.");
			return false;
		}
		Require(well_ordered(xleft, xright, xinc),
			err("`set x axis .left. .right. .incBig.' has .incBig. of wrong sign"));
		SUGGEST(inc_within_range(xleft, xright, xinc),
			warning("`set x axis .left. .right. .incBig.' has .incBig. that goes outside range"));
		if (_xtype == gr_axis_LOG) {
			Require(xleft > 0.0,
				err("`set x axis .left. .right. .incBig.' cannot have non-positive .left. value for logarithmic axis"));
			Require(xright > 0.0,
				err("`set x axis .left. .right. .incBig.' cannot have non-positive .right. value for logarithmic axis"));
			Require(xinc > 0.0,
				err("`set x axis .left. .right. .incBig.' cannot have non-positive .incBig. value for logarithmic axis"));
		}
		_xleft = xleft;
		_xright = xright;
		if (_xtype == gr_axis_LOG) {
			_xinc = xinc;
			_xsubdiv = 1;
		} else { 
			_xinc = xinc;
			_xsubdiv = 1;
		}
		PUT_VAR("..xleft..", _xleft);
		PUT_VAR("..xright..", _xright);
		PUT_VAR("..xinc..", _xinc);
		_xscale_exists = true;
		_need_x_axis = true;
		_need_y_axis = true;
		_user_set_x_axis = true;
		return true;
	} else if (_nword == 7) { // 'set x axis .left. .right. .incBig. .incSml.'
		if (!getdnum(_word[3], &xleft)
		    || !getdnum(_word[4], &xright)
		    || !getdnum(_word[5], &xinc)
		    || !getdnum(_word[6], &tmp)) {
			READ_WORD_ERROR(".left. .right. .incBig. .incSml.");
			return false;
		}
		Require(well_ordered(xleft, xright, xinc),
			err("`set x axis .left. .right. .incBig. .incSml.' has .incBig. of wrong sign"));
		if (_xtype == gr_axis_LOG) {
			Require(xleft > 0.0,
				err("`set x axis .left. .right. .incBig. .incSml.' cannot have non-positive .left. value for logarithmic axis"));
			Require(xright > 0.0,
				err("`set x axis .left. .right. .incBig. .incSml.' cannot have non-positive .right. value for logarithmic axis"));
			Require(xinc > 0.0,
				err("`set x axis .left. .right. .incBig. .incSml.' cannot have non-positive .incBig. value for logarithmic axis"));
			Require(tmp > 0.0,
				err("`set x axis .left. .right. .incBig. .incSml.' cannot have non-positive .incSml. value for logarithmic axis"));
		}
		SUGGEST(inc_within_range(xleft, xright, xinc),
			warning("`set x axis .left. .right. .incBig.' has .incBig. that goes outside range"));
		SUGGEST(inc_within_range(xleft, xright, tmp),
			warning("`set x axis .left. .right. .incBig. .incSml.' has .incSml. that goes outside range"));
		_xleft = xleft;
		_xright = xright;
		if (_xtype == gr_axis_LOG) {
			_xinc = xinc;
			_xsubdiv = (tmp > 0) ? 1 : -1;
		} else { 
			_xinc = xinc;
			_xsubdiv = int(floor(0.5 + fabs((double) (xinc / tmp))));
		}
		PUT_VAR("..xleft..", _xleft);
		PUT_VAR("..xright..", _xright);
		PUT_VAR("..xinc..", _xinc);
		_xscale_exists = true;
		_need_x_axis = true;
		_need_y_axis = true;
		_user_set_x_axis = true;
		return true;
	} else {
		err("`set x axis' may have only 2, 3 or 4 parameters");
		return false;
	}
	_user_set_x_axis = true;
	return true;
}

bool
set_x_formatCmd()
{
	if (_nword < 4) {
		err("Must specify a format for `set x format'");
		return false;
	}
	if (!strcmp(_word[3], "off")) {
		_xFmt.assign("");
	} else if (!strcmp(_word[3], "default")){
		_xFmt.assign(X_FMT_DEFAULT);
	} else {
		if (*_word[3] == '"') {
			int len = strlen(_word[3]);
			if (len <= 1) {
				_xFmt.assign(X_FMT_DEFAULT);
			} else {
				if (*(_word[3] + len - 1) == '"')
					_xFmt.assign(_word[3] + 1, len - 2);
				else
					_xFmt.assign(_word[3] + 1, len - 1);
			}
		} else {
			_xFmt.assign(_word[3]);
		}
	}
	return true;
}

bool
set_x_gridCmd()
{
	double          x, xmin, xmax, xinc;
	int             i, nx;
	// get numbers
	if (_nword != 6 && _nword != 7) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	Require(getdnum(_word[3], &xmin), READ_WORD_ERROR(".xmin."));
	Require(getdnum(_word[4], &xmax), READ_WORD_ERROR(".xmax."));
	if (*_word[5] == '/') {
		// kludge in case previous parsing didn't separate / from last number
		if (_nword == 6) {
			Require(getdnum(1 + _word[5], &xinc), err("Can't read /.numx."));
		} else if (_nword == 7) {
			Require(getdnum(_word[6], &xinc), err("Can't read /.numx."));
		} else {
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		Require(xinc >= 1.9, err("Bad /.x.; need >1"));
		nx = (int) floor(0.5 + xinc); // redefined below, using xinc
		xinc = (xmax - xmin) / (nx - 1);
	} else {
		Require(getdnum(_word[5], &xinc), READ_WORD_ERROR(".xinc."));
	}
	// check for stupidity
	Require(xinc != 0.0, err("Can't have .xinc.=0"));
	Require(xmin != xmax, err("Can't have .xmin. = .xmax."));
	Require(well_ordered(xmin, xmax, xinc),
		err("`set x grid .xmin. .xmax. .xinc.' has .xinc. of wrong sign"));
	SUGGEST(inc_within_range(xmin, xmax, xinc),
		warning("`set x grid .xmin. .xmax. .xinc.' has .xinc. that goes outside range"));
	nx = int(floor(1.5 + fabs((double) ((xmax - xmin) / xinc))));
	Require(nx > 0, err(".xinc. too big"));
	// check against existing matrix
	if (_grid_exists == true && nx != (int)_num_xmatrix_data) {
		sprintf(_errorMsg, "# intervals %d disagrees with existing grid size %d",
			nx, _num_xmatrix_data);
		err(_errorMsg);
		return false;
	}
	// get storage space
	Require(allocate_xmatrix_storage(nx), err("Insufficient space for grid x data"));
	// set up x grid 
	for (i = 0, x = xmin; i < nx; i++, x += xinc)
		_xmatrix[i] = x;
	// Override any existing scale
	define_image_scales(_xmatrix[0], 0.0, _xmatrix[nx - 1], 0.0);
	_xgrid_exists = true;
	if (!_xscale_exists)
		create_x_scale();
	if (_xmatrix[1] > _xmatrix[0])
		_xgrid_increasing = true;
	else
		_xgrid_increasing = false;
	return true;
}				// end set_x_gridCmd()

bool
set_y_gridCmd()
{
	double          y, ymin, ymax, yinc;
	int             i, ny;
	if (_nword != 6 && _nword != 7) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!getdnum(_word[3], &ymin)) {
		demonstrate_command_usage();
		READ_WORD_ERROR(".ymin.");
		return false;
	}
	if (!getdnum(_word[4], &ymax)) {
		demonstrate_command_usage();
		READ_WORD_ERROR(".ymax.");
		return false;
	}
	if (*_word[5] == '/') {
		// kludge in case previous parsing didn't separate / from last number
		if (_nword == 6) {
			Require(getdnum(1 + _word[5], &yinc), err("Can't read /.numy."));
		} else if (_nword == 7) {
			Require(getdnum(_word[6], &yinc), err("Can't read /.numy."));
		} else {
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		Require(yinc >= 1.9, err("Bad /.y.; need >1"));
		ny = int(floor(0.5 + yinc));
		yinc = (ymax - ymin) / (yinc - 1);
	} else {
		if (!getdnum(_word[5], &yinc)) {
			demonstrate_command_usage();
			READ_WORD_ERROR(".yinc.");
			return false;
		}
	}
	// check for stupidity
	Require(yinc != 0.0, err("Can't have .yinc.=0"));
	Require(ymin != ymax, err("Can't have .ymin.=.ymax."));
	Require(well_ordered(ymin, ymax, yinc),
		err("`set y grid .ymin. .ymax. .yinc.' has .yinc. of wrong sign"));
	SUGGEST(inc_within_range(ymin, ymax, yinc),
		warning("`set y grid .ymin. .ymax. .yinc.' has .yinc. that goes outside range"));
	ny = 1 + int(floor(0.5 + fabs((double) ((ymax - ymin) / yinc))));
	Require(ny > 0, err(".yinc. too big"));
	// check against existing matrix
	if (_grid_exists == true && ny != int(_num_ymatrix_data)) {
		demonstrate_command_usage();
		sprintf(_errorMsg,
			"# intervals %d disagrees with existing grid %d",
			ny, _num_ymatrix_data);
		err(_errorMsg);
		return false;
	}
	// get storage space
	Require(allocate_ymatrix_storage(ny), err("Insufficient space for grid y data."));
	// set up y grid
	for (i = 0, y = ymin; i < ny; i++, y += yinc)
		_ymatrix[i] = y;
	// Override any existing scale
	define_image_scales(0.0, _ymatrix[0], 0.0, _ymatrix[ny - 1]);
	_ygrid_exists = true;
	if (!_yscale_exists)
		create_y_scale();
	if (_ymatrix[1] > _ymatrix[0])
		_ygrid_increasing = true;
	else
		_ygrid_increasing = false;
	return true;
}				// end set_y_gridCmd()

bool
set_x_marginCmd()
{
	if (_nword == 4) {
		if (!strcmp(_word[3], "default")) {
			tmp = XMARGIN_DEFAULT;
			PUT_VAR("..xmargin..", tmp);
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		}
		if (!getdnum(_word[3], &tmp)) {
			err("Can't understand `set x margin' parameter.");
			return false;
		}
		PUT_VAR("..xmargin..", tmp);
		_need_x_axis = true;
		_need_y_axis = true;
		return true;
	} else if (_nword == 5) {
		double          old = 0.0;
		if (!strcmp(_word[3], "bigger")) {
			if (!getdnum(_word[4], &tmp)) {
				err("can't understand `set x margin bigger' parameter");
				return false;
			}
			get_var("..xmargin..", &old);
			tmp = old + tmp;
			PUT_VAR("..xmargin..", tmp);
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		} else if (!strcmp(_word[3], "smaller")) {
			if (!getdnum(_word[4], &tmp)) {
				err("can't understand `set x margin smaller' parameter");
				return false;
			}
			get_var("..xmargin..", &old);
			tmp = old - tmp;
			PUT_VAR("..xmargin..", tmp);
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		} else {
			err("bad `set x margin' parameter\nvalid list: bigger/smaller");
			return false;
		}
	} else {
		err("Must specify margin in cm");
		return false;
	}
}

bool
set_x_nameCmd()
{
	Require(_nword > 3, err("Must specify a name"));
	if (word_is(3, "default")) {
		_colX.setName("x");
	} else {
		std::string unquoted;
		int status = ExtractQuote(_cmdLine, unquoted);
		if (status == 0) {
			err("`set x name' needs a double-quoted string");
			return false;
		}
		if (status < 0) {
			err("`set x name' found starting double-quote but no ending double-quote");
			return false;
		}
		_colX.setName(unquoted.c_str());
	}
	return true;
}

// set_x_sizeCmd() -- set width of plot
bool
set_x_sizeCmd()
{
	if (_nword != 4) {
		err("Must specify axis length in cm");
		return false;
	}
	if (!strcmp(_word[3], "default")) {
		tmp = XSIZE_DEFAULT;
		PUT_VAR("..xsize..", tmp);
		_need_x_axis = true;
		_need_y_axis = true;
		return true;
	}
	Require(getdnum(_word[3], &tmp), READ_WORD_ERROR(".width_cm."));
	Require(tmp >= 0.0, err("ignoring bad xsize <0"));
	PUT_VAR("..xsize..", tmp);
	_need_x_axis = true;
	_need_y_axis = true;
	return true;
}

bool
set_y_axisCmd()
{
	_yatleft = true;
#if 1				// 2.9.x
	if (word_is(3, "labels")) {
		if (word_is(4, "automatic")) {
			_y_labels.erase(_y_labels.begin(), _y_labels.end());
			_y_label_positions.erase(_y_label_positions.begin(), _y_label_positions.end());
			return true;
		} else {
			unsigned int start = 4;
			if (word_is(start, "add")) {
				start++;
			} else {
				_y_labels.erase(_y_labels.begin(), _y_labels.end());
				_y_label_positions.erase(_y_label_positions.begin(), _y_label_positions.end());
			}
			for (unsigned int i = start; i < _nword; i++) {
				double tmp;
				if (!getdnum(_word[i], &tmp)) {
					READ_WORD_ERROR(".pos.");
					demonstrate_command_usage();
					return false;
				}
				_y_label_positions.push_back(tmp);
				if (i++ == _nword - 1) {
					err("Missing label to be applied at position \\", _word[i-1], "\\");
					demonstrate_command_usage();
					return false;
				}
				std::string l = _word[i];
				un_double_quote(l);
				_y_labels.push_back(l);
			}
			return true;
		}
	}
#endif // 2.9.x

	if (!strcmp(_word[_nword - 1], "left")) {
		_yatleft = true;
		if (_nword == 4) {
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		}
		_nword--;
	} else if (!strcmp(_word[_nword - 1], "right")) {
		_yatleft = false;
		if (_nword == 4) {
			_need_x_axis = true;
			_need_y_axis = true;
			return true;
		}
		_nword--;
	} else if (_nword == 4 && !strcmp(_word[_nword - 1], "increasing")) {
		_yincreasing = true;
		if (_yscale_exists && _ybottom > _ytop) {
			double          tmp = _ybottom;
			_ybottom = _ytop;
			_ytop = tmp;
			_yinc = -_yinc;
			PUT_VAR("..ybottom..", _ybottom);
			PUT_VAR("..ytop..", _ytop);
			PUT_VAR("..yinc..", _yinc);
		}
		return true;
	} else if (_nword == 4 && !strcmp(_word[_nword - 1], "decreasing")) {
		_yincreasing = false;
		if (_yscale_exists && _ybottom < _ytop) {
			double          tmp = _ybottom;
			_ybottom = _ytop;
			_ytop = tmp;
			_yinc = -_yinc;
			PUT_VAR("..ybottom..", _ybottom);
			PUT_VAR("..ytop..", _ytop);
			PUT_VAR("..yinc..", _yinc);
		}
		return true;
	} else if (_nword == 4 && !strcmp(_word[_nword - 1], "unknown")) {
		_yscale_exists = false;
		_need_y_axis = true;
		_user_set_y_axis = false;
		return true;
	}
	// set y axis name ...
	if (_nword == 5 && word_is(3, "name")) {
		set_y_axis_nameCmd(); 
		return true;
	}
	if (_nword == 5 && word_is(3, "label")) {
		//printf("YA YA YA %f %f %d\n",_version,_version_expected,gri_version_exceeds(2,8,99));
		if (gri_version_exceeds(2, 8, 99)) {
			if (_version_expected != 0 && _version_expected < 2.0899) {
				warning("Using compatibility mode, interpreting `set y axis label'\n         as if it were the newly-named command  `set y axis name'.");
				set_y_axis_nameCmd(); 
				return true;
			} else {
				err("The `set y axis label' command is no longer available.\n       Please use `set y axis name' instead, or use the `expecting'\n       command with a version number lower than 2.9.0, to get\n       backwards compatability.");
				return false;
			}
		} else {
			set_y_axis_nameCmd(); 
			return true;
		}
	}
	// ... specifying y axis
	if (_nword == 5) {
		// set y axis .bottom. .top.
		if (!getdnum(_word[3], &ybottom) || !getdnum(_word[4], &ytop)) {
			err("can't understand parameters");
			return false;
		}
		if (_ytype == gr_axis_LOG) {
			Require(ybottom > 0.0,
				err("`set y axis .bottom. .top.' cannot have non-positive .bottom. value for logarithmic axis"));
			Require(ytop > 0.0,
				err("`set y axis .bottom. .top.' cannot have non-positive .top. value for logarithmic axis"));
		}
		_ybottom = ybottom;
		_ytop = ytop;
		if (_ytype == gr_axis_LOG)
			_yinc = 1.0;
		else
			_yinc = _ytop - _ybottom;
		PUT_VAR("..ybottom..", _ybottom);
		PUT_VAR("..ytop..", _ytop);
		PUT_VAR("..yinc..", _yinc);
		_ysubdiv = 1;
		_yscale_exists = true;
		_need_x_axis = true;
		_need_y_axis = true;
		reset_top_of_plot();
		_user_set_y_axis = true;
		return true;
	} else if (_nword == 6) {
		// set y axis .bottom. .top. .incBig.
		if (!getdnum(_word[3], &ybottom)
		    || !getdnum(_word[4], &ytop)
		    || !getdnum(_word[5], &yinc)) {
			err("can't understand parameters");
			return false;
		}
		Require(well_ordered(ybottom, ytop, yinc),
			err("`set y axis .bottom. .top. .incBig.' has .incBig. of wrong sign"));
		if (_ytype == gr_axis_LOG) {
			Require(ybottom > 0.0,
				err("`set y axis .bottom. .top. .incBig.' cannot have non-positive .bottom. value for logarithmic axis"));
			Require(ytop > 0.0,
				err("`set y axis .bottom. .top. .incBig.' cannot have non-positive .top. value for logarithmic axis"));
			Require(yinc > 0.0,
				err("`set y axis .bottom. .top. .incBig.' cannot have non-positive .incBig. value for logarithmic axis"));
		}
		SUGGEST(inc_within_range(ybottom, ytop, yinc),
			warning("`set y axis .bottom. .top. .incBig.' has .incBig. that goes outside range"));
		_ybottom = ybottom;
		_ytop = ytop;
		if (_ytype == gr_axis_LOG) {
			_yinc = yinc;
			_ysubdiv = 1;
		} else { 
			_yinc = yinc;
			_ysubdiv = 1;
		}
		PUT_VAR("..ybottom..", _ybottom);
		PUT_VAR("..ytop..", _ytop);
		PUT_VAR("..yinc..", _yinc);
		_yscale_exists = true;
		_need_x_axis = true;
		_need_y_axis = true;
		reset_top_of_plot();
		_user_set_y_axis = true;
		return true;
	} else if (_nword == 7) { // 'set y axis .bottom. .top. .incBig. .incSml.'
		if (!getdnum(_word[3], &ybottom)
		    || !getdnum(_word[4], &ytop)
		    || !getdnum(_word[5], &yinc)
		    || !getdnum(_word[6], &tmp)) {
			err("can't understand parameters");
			return false;
		}
		Require(well_ordered(ybottom, ytop, yinc),
			err("`set y axis .bottom. .top. .incBig. .incSml.' has .incBig. of wrong sign"));
		if (_ytype == gr_axis_LOG) {
			Require(ybottom > 0.0,
				err("`set y axis .bottom. .top. .incBig. .incSml.' cannot have non-positive .bottom. value for logarithmic axis"));
			Require(ytop > 0.0,
				err("`set y axis .bottom. .top. .incBig. .incSml.' cannot have non-positive .top. value for logarithmic axis"));
			Require(yinc > 0.0,
				err("`set y axis .bottom. .top. .incBig. .incSml.' cannot have non-positive .incBig. value for logarithmic axis"));
			Require(tmp > 0.0,
				err("`set y axis .bottom. .top. .incBig. .incSml.' cannot have non-positive .incSml. value for logarithmic axis"));
		}
		SUGGEST(inc_within_range(ybottom, ytop, yinc),
			warning("`set y axis .bottom. .top. .incBig.' has .incBig. that goes outside range"));
		SUGGEST(inc_within_range(ybottom, ytop, tmp),
			warning("`set y axis .bottom. .top. .incBig. incSml.' has .incSml. that goes outside range"));
		_ybottom = ybottom;
		_ytop = ytop;
		if (_ytype == gr_axis_LOG) {
			_yinc = yinc;
			_ysubdiv = (tmp > 0) ? 1 : -1;
		} else { 
			_yinc = yinc;
			_ysubdiv = int(floor(0.5 + fabs((double) (yinc / tmp))));
		}
		PUT_VAR("..ybottom..", _ybottom);
		PUT_VAR("..ytop..", _ytop);
		PUT_VAR("..yinc..", _yinc);
		_yscale_exists = true;
		_need_x_axis = true;
		_need_y_axis = true;
		reset_top_of_plot();
		_user_set_y_axis = true;
		return true;
	} else {
		err("`set y axis' may have only 2, 3 or 4 parameters");
		return false;
	}
	_user_set_y_axis = true;
	return true;
}

bool
set_y_axis_nameCmd()
{
	if (_nword == 5 && !strcmp(_word[3], "label")) { // Syntax prior to version 2.9.0
		if (!strcmp(_word[4], "horizontal"))
			gr_setyaxisstyle(1);
		else if (!strcmp(_word[4], "vertical"))
			gr_setyaxisstyle(0);
		else {
			err("`set y axis name' expecting 'horizontal' or 'vertical', but got `\\", _word[4], "'", "\\");
			demonstrate_command_usage();
			return false;
		}
	} else if (_nword == 5 && !strcmp(_word[3], "name")) { // From version 2.9.0 onwards
		if (!strcmp(_word[4], "horizontal"))
			gr_setyaxisstyle(1);
		else if (!strcmp(_word[4], "vertical"))
			gr_setyaxisstyle(0);
		else {
			err("`set y axis name' expecting 'horizontal' or 'vertical', but got `\\", _word[4], "'", "\\");
			demonstrate_command_usage();
			return false;
		}
	}
	return true;
}

bool
set_y_formatCmd()
{
	Require(_nword > 3, err("Must specify a format for `set y format'"));
	if (!strcmp(_word[3], "off")) {
		_yFmt.assign("");
	} else if (!strcmp(_word[3], "default")){
		_yFmt.assign(Y_FMT_DEFAULT);
	} else {
		if (*_word[3] == '"') {
			int len = strlen(_word[3]);
			if (len <= 1) {
				_yFmt.assign(Y_FMT_DEFAULT);
			} else {
				if (*(_word[3] + len - 1) == '"')
					_yFmt.assign(_word[3] + 1, len - 2);
				else
					_yFmt.assign(_word[3] + 1, len - 1);
			}
		} else {
			_yFmt.assign(_word[3]);
		}
	}
	return true;
}

bool
set_y_marginCmd()
{
	if (_nword == 4) {
		if (!strcmp(_word[3], "default")) {
			tmp = YMARGIN_DEFAULT;
			PUT_VAR("..ymargin..", tmp);
			_need_x_axis = true;
			_need_y_axis = true;
			reset_top_of_plot();
			return true;
		}
		Require(getdnum(_word[3], &tmp), err("can't understand `set y margin' parameter"));
		PUT_VAR("..ymargin..", tmp);
		_need_x_axis = true;
		_need_y_axis = true;
		reset_top_of_plot();
		return true;
	} else if (_nword == 5) {
		double          old = 0.0;
		if (!strcmp(_word[3], "bigger")) {
			Require(getdnum(_word[4], &tmp), 
				err("can't understand `set y margin bigger' parameter"));
			get_var("..ymargin..", &old);
			tmp = old + tmp;
			PUT_VAR("..ymargin..", tmp);
			_need_x_axis = true;
			_need_y_axis = true;
			reset_top_of_plot();
			return true;
		} else if (!strcmp(_word[3], "smaller")) {
			Require(getdnum(_word[4], &tmp), 
				err("can't understand `set y margin smaller' parameter"));
			get_var("..ymargin..", &old);
			tmp = old - tmp;
			PUT_VAR("..ymargin..", tmp);
			_need_x_axis = true;
			_need_y_axis = true;
			reset_top_of_plot();
			return true;
		} else {
			err("bad `set y margin' parameter\nvalid list: bigger/smaller");
			return false;
		}
	} else {
		err("Must specify margin in cm");
		return false;
	}
}

bool
set_y_nameCmd()
{
	Require(_nword > 3, err("Must specify a name"));
	if (word_is(3, "default")) {
		_colY.setName("y");
	} else {
		std::string unquoted;
		int status = ExtractQuote(_cmdLine, unquoted);
		if (status == 0) {
			err("`set y name' needs a double-quoted string");
			return false;
		}
		if (status < 0) {
			err("`set y name' found starting double-quote but no ending double-quote");
			return false;
		}
		_colY.setName(unquoted.c_str());
	}
	return true;
}

// set_y_sizeCmd() -- store height of plot
bool
set_y_sizeCmd()
{
	Require(_nword == 4, err("Must specify axis length in cm"));
	if (!strcmp(_word[3], "default")) {
		tmp = YSIZE_DEFAULT;
		PUT_VAR("..ysize..", tmp);
		reset_top_of_plot();
		_need_x_axis = true;
		_need_y_axis = true;
		return true;
	}
	Require(getdnum(_word[3], &tmp), READ_WORD_ERROR(".height_cm."));
	Require (tmp >= 0.0, err("ignoring bad ysize <0"));
	PUT_VAR("..ysize..", tmp);
	reset_top_of_plot();
	_need_x_axis = true;
	_need_y_axis = true;
	return true;
}

// Possible calls are as follows; code below may
// be in a different order though!
//  Type 1. \syn = word .n. of "string"
//  Type 2. \syn = system ...
//  Type 3. \syn = tmpname
#if 0
//  Type 4. \syn = &.a_var.
//  Type 5. \syn = &\a_syn
#endif
//  Type 6. \syn = "string"
bool
assign_synonym()
{
#if 0
	printf("DEBUG %s:%d in assign_synonym.  words: ", __FILE__,__LINE__);
	for (unsigned int iw = 0; iw < _nword; iw++) 
		printf("<%s> ", _word[iw]);
	printf("\n");
#endif
	Require (_nword > 2, err("Can't understand command."));
	if (!strncmp(_word[0], "\\@", 2)) {
		err("The purported alias `\\", _word[0], "' doesn't name any known variable or synonym.", "\\");
		return false;
	}

	// If assigning as e.g.
	//     \.word1. = 10
	//     \.word1. = "hi"
	// then see if the calling-arg was a var/syn with an & to the left
	if (!strncmp(_word[0], "\\.word", 6) && *(_word[0] + strlen(_word[0]) - 1) == '.') {
		//printf("DEBUG %s:%d ASSIGNING to word[0] as '%s'\n",__FILE__,__LINE__,_word[0]);
		std::string value;
		if (!get_syn(_word[0], value, false)) {
			err("Cannot access \\.word0. synonym");
			return false;
		}
		//printf("DEBUG %s:%d value of '%s' is '%s'\n",__FILE__,__LINE__,_word[0],value.c_str());
		std::string coded_name;
		int coded_level = -1;
		if (is_coded_string(value, coded_name, &coded_level)) {
			//printf("DEBUG %s:%d '%s' was encoded `%s' at level %d\n",__FILE__,__LINE__, _word[0], coded_name.c_str(), coded_level);
			if (coded_name.c_str()[0] == '.') {
				int index = index_of_variable(coded_name.c_str(), coded_level);
				//printf("DEBUG %s:%d A VAR ... index %d.  to assign '%s'\n",__FILE__,__LINE__,index,_word[2]);
				if (index < 0) {
					err("Cannot assign to non-existing variable `", coded_name.c_str(), "' as inferred from coded word `", value.c_str(), "'.", "\\");
					return false;
				}
				double rhs;
				if (!getdnum(_word[2], &rhs)) {
					err("cannot assign `", _word[2], "' to variable `", coded_name.c_str(), "'.", "\\");
					return false;
				}
				if (!strcmp(_word[1], "=")) {
					variableStack[index].set_value(rhs);
				} else {
					double oldValue = variableStack[index].get_value();
					if (strEQ(_word[1], "*="))
						variableStack[index].set_value(oldValue * rhs);
					else if (strEQ(_word[1], "/="))
						variableStack[index].set_value(oldValue / rhs);
					else if (strEQ(_word[1], "+="))
						variableStack[index].set_value(oldValue +rhs);
					else if (strEQ(_word[1], "-="))
						variableStack[index].set_value(oldValue - rhs);
					else if (strEQ(_word[1], "^="))
						variableStack[index].set_value(pow(oldValue, rhs));
					else if (strEQ(_word[1], "_=")) {
						if (oldValue < 0.0)
							variableStack[index].set_value(gr_currentmissingvalue());
						else
							variableStack[index].set_value(log(oldValue) / log(rhs));
					} else {
						err("`\\", _word[1], "' is not a known operator for variables", "'\\");
						return false;
					}
				}
			} else if (coded_name.c_str()[0] == '\\') {
				int index = index_of_synonym(coded_name.c_str(), coded_level);
				//printf("DEBUG %s:%d '%s' is syn index %d\n",__FILE__,__LINE__, coded_name.c_str(), index);
				std::string unquoted;
				int status = ExtractQuote(_word[2], unquoted);
				if (status == 0) {
					err("`\\synonym = \"value\" found no double-quoted string");
					return false;
				}
				//printf("BEFORE trying to insert at position %d.\n",index);
				//show_syn_stack();
				synonymStack[index].set_value(unquoted.c_str());
				//printf("AFTER.\n");
				//show_syn_stack();
			} else {
				err("Internal error in decoding &\\.word?. for assignment");
				return false;
			}
			return true;
		}
	}

	
#if 0
	// Check for e.g 
	// \syn = &.a_var.
	// \syn = &\a_syn
	if (_nword == 3 && *_word[2] == '&') {
		const char *name = 1 + _word[2];
		//printf("DEBUG %s:%d GOT A & and think name is <%s>\n",__FILE__,__LINE__,name);
		char coded_pointer[200];	// BUG: should be big enough.  Jeeze!
		if (is_var(name)) {
			//printf("DEBUG: %s:%d & on a var named <%s>\n",__FILE__,__LINE__,name);
			int the_index = index_of_variable(name);
			sprintf(coded_pointer, "\\#v%d#", int(variablePointer.size()));
			//printf("DEBUG %s:%d pushing back %d into position %d of variablePointer list\n",__FILE__,__LINE__,the_index,int(variablePointer.size()));
			variablePointer.push_back(the_index);
			Require(put_syn(_word[0], coded_pointer, true),
				err("Cannot store synonym `\\", _word[0], "'", "\\"));
		} else if (is_syn(name)) {
			//printf("DEBUG: %s:%d & on a syn named <%s>\n",__FILE__,__LINE__,name);
			int the_index = index_of_synonym(name);
			//printf("DEBUG %s:%d pushing back %d into position %d of synonymPointer list\n",__FILE__,__LINE__,the_index,int(synonymPointer.size()));
			sprintf(coded_pointer, "\\#s%d#", int(synonymPointer.size()));
			synonymPointer.push_back(the_index);
			Require(put_syn(_word[0], coded_pointer, true),
				err("Cannot store synonym `\\", _word[0], "'", "\\"));
			return true;
		} else {
			err("Cannot do '&' unless item to right is name of variable or synonym");
			return false;
		}
		return true;
	}
#endif
	// Following check should never be needed, but keep for possible future
	// changes.
	Require(is_syn(_word[0]), err("`\\", _word[0], "' must begin with `\\'", "\\"));
	// `\synonym = word .n. of "string"'
	if (_nword == 6
	    && !strcmp(_word[1], "=")
	    && !strcmp(_word[2], "word")
	    && !strcmp(_word[4], "of")) {
		double tmp;
		if (!getdnum(_word[3], &tmp)) {
			READ_WORD_ERROR(".n. in e.g. `\\syn = word .n. of \"string\"'");
			return false;
		}
		int iwhich = int(floor(0.5 + tmp));
		if (iwhich < 0) {
			err("Cannot take a negatively-indexed word");
			return false;
		}
		unsigned int which = (unsigned int)(iwhich);
		std::string to_chop(_word[_nword - 1]);
		if (to_chop[0] == '"')
			to_chop.STRINGERASE(0, 1);
		if (to_chop[to_chop.size() - 1] == '"') {
			to_chop.STRINGERASE(to_chop.size() - 1, 1);
		} else {
			err("`\\syn = word N of \"string\" requires closing double-quote (\") sign");
			return false;
		}
		char *to_chop_in_C = strdup(to_chop.c_str());
		if (to_chop_in_C == NULL) {
			err("Out of memory while trying to assign synonym as n-th word of string");
			return false;
		}
		unsigned int max;
		chop_into_words(to_chop_in_C, _Words2, &max, MAX_nword);
		int i = strlen(_Words2[max - 1]);
		if (i > 2 && *(_Words2[max - 1] + i - 1) == '"')
			*(_Words2[max - 1] + i - 1) = '\0';
		if (which > (max - 1)) {
			err("The string \n`\\",
			    _word[_nword - 1],
			    "'\ndoes not have that many words.  NOTE: the first word is counted as 0.",
			    "\\");
			free(to_chop_in_C);
			return false;
		}
		Require(put_syn(_word[0], _Words2[which], true),
			err("Cannot store synonym `\\", _word[0], "'", "\\"));
		free(to_chop_in_C);
		return true;
	} else if (!strcmp(_word[1], "=") && !strcmp(_word[2], "tmpname")) {
		if (!put_syn(_word[0], tmp_file_name(), true))
			gr_Error("Ran out of storage");			
		return true;
	} else if (!strcmp(_word[1], "=") && !strcmp(_word[2], "system")) {
		// `\synonym = system ...'
#if !defined(HAVE_POPEN)
		err("\
This computer can't `\\synonym = system ...' since no popen() subroutine.");
		return false;
#else
		FILE           *pipefile;
		// Much of following code duplicated in sytemCmd(), so if any
		// problems crop up, check there too.
		char *            s = _cmdLine;
		s += skip_space(s);	// skip any initial space
		s += skip_nonspace(s);	// skip first word "\syn"
		s += skip_space(s);	// skip space
		s += skip_nonspace(s);	// skip "="
		s += skip_space(s);	// skip space
		s += skip_nonspace(s);	// skip "system"
		s += skip_space(s);	// skip space
		if (*s == '\0' || *s == '\n') {
			err("`\\syn = system ...' needs a system command to do.");
			return false;
		}
		
		// See if last word starts with "<<"; if so, then the stuff to be done
		// appears on the lines following, ended by whatever word follows the
		// "<<".
		// ... compare doline.cc near line 510.
		int i = strlen(s) - 2;
		std::string read_until;
		bool            using_read_until = false;
		while (--i) {
			if (!strncmp((s + i), "<<", 2)) {
				bool            quoted_end_string = false;
				int             spaces = 0;
				while (isspace(*(s + i + 2 + spaces))) {
					spaces++;
				}
				if (*(s + i + 2 + spaces) == '"') {
					spaces++;
					quoted_end_string = true;
				}
				read_until.assign(s + i + 2 + spaces);
				using_read_until = true;
				// trim junk from end of the 'read until' string
				std::string::size_type cut_at;
				if (quoted_end_string)
					cut_at = read_until.find("\"");
				else
					cut_at = read_until.find(" ");
				//printf("READING UNTIL '%s' ... i.e.\n", read_until.c_str());
				if (cut_at != STRING_NPOS)
					read_until.STRINGERASE(cut_at, read_until.size() - cut_at);
				if (read_until.size() < 1) {
					err("`system ... <<STOP_STRING' found no STOP_STRING");
					return false;
				}
				//printf("reading until '%s'\n",read_until.c_str());
				break;
			}
		}
		static std::string cmd; // might save time in loops
		cmd.assign(s);
		if (using_read_until) {
			// It is of the <<WORD form
#if 1
			cmd.append("\n");
			extern std::vector<BlockSource> bsStack;
			if (bsStack.size() == 0) {
				if (((unsigned) superuser()) & FLAG_SYS)printf("DEBUG %s:%d GOBBLE from a file\n",__FILE__,__LINE__);
				while (get_command_line()) {
					if (((unsigned) superuser()) & FLAG_SYS)printf("DEBUG %s:%d cmd line is [%s]\n",__FILE__,__LINE__,_cmdLine);
				        // Trim filename/fileline indicator
					unsigned int l = strlen(_cmdLine);
					for (unsigned int ii = 0; ii < l; ii++) {
						if (_cmdLine[ii] == PASTE_CHAR) {
							_cmdLine[ii] = '\0';
							break;
						}
					}
					if (!strncmp(_cmdLine + skip_space(_cmdLine), read_until.c_str(), read_until.size())) {
						cmd.append(_cmdLine + skip_space(_cmdLine));
						cmd.append("\n");
						break;
					}
					cmd.append(_cmdLine);
					cmd.append("\n");
				}
				std::string cmd_sub;
				substitute_synonyms_cmdline(cmd.c_str(), cmd_sub, false);
				cmd = cmd_sub;
			} else {
				extern unsigned int chars_read;
				extern unsigned int offset_for_read;
				extern bool get_line_in_block(const char *block, unsigned int *offset);
				unsigned int offset = offset_for_read + chars_read;
				if (((unsigned) superuser()) & FLAG_SYS)printf("DEBUG %s:%d GOBBLE from block source\n",__FILE__,__LINE__);
				while (get_line_in_block(bsStack.back().get_start(), &offset)) {
					if (((unsigned) superuser()) & FLAG_SYS)printf("DEBUG %s:%d cmd line is [%s]\n",__FILE__,__LINE__,_cmdLine);
					bsStack.back().move_offset(strlen(_cmdLine) + 1);
					chars_read += strlen(_cmdLine) + 1;
					if (!strncmp(_cmdLine + skip_space(_cmdLine), read_until.c_str(), read_until.size())) {
						cmd.append(_cmdLine + skip_space(_cmdLine));
						cmd.append("\n");
						break;
					}
					cmd.append(_cmdLine);
					cmd.append("\n");
				}
				std::string cmd_sub;
				substitute_synonyms_cmdline(cmd.c_str(), cmd_sub, false);
				cmd = cmd_sub;
			}
			if (((unsigned) superuser()) & FLAG_SYS)printf("DEBUG %s:%d COMMAND START...\n%s\nDEBUG %s:%d ... COMMAND END\n",__FILE__,__LINE__,cmd.c_str(),__FILE__,__LINE__);
#endif
		} else {
			// No, it is not of the <<WORD form
			std::string::size_type loc = 0;
			//printf("assigning synonym BEFORE [%s]\n",cmd.c_str());
			while (STRING_NPOS != (loc = cmd.find("\\\\", loc))) {
				cmd.STRINGERASE(loc, 2);
				cmd.insert(loc, "\\");
			}
			//printf("AFTER [%s]\n",cmd.c_str());
		}
		clean_blanks_quotes(cmd);
		cmd.append("\n");
		if (((unsigned) superuser()) & FLAG_SYS) {
			ShowStr("\n`\\synonym = system' sending the following command to the operating system:\n");
			ShowStr(cmd.c_str());
			ShowStr("\n");
		}
		pipefile = (FILE *) popen(cmd.c_str(), "r");
		if (pipefile) {
			std::string result;
			GriString this_line;
			//printf("START.\n");
			do {
				eof_status s = this_line.line_from_FILE(pipefile);
				//printf("<%s> %d (%d=eof_after %d=eofbeforedata, %d=no_eof)\n",this_line.getValue(),s,eof_after_data,eof_before_data,no_eof);
				if (s == eof_before_data)
					break;
				result.append(this_line.getValue());
				//printf("NOW <%s>\n",result.c_str());
			} while (1);
			pclose(pipefile);
			while (result[result.size() - 1] == '\n') {
				//printf("ERASING newline at end ....\n");
				result.STRINGERASE(result.size() - 1, 1);
				//printf("<%s>\n",result.c_str());
			}
			//printf("final <%s>\n",result.c_str());
			if (!put_syn(_word[0], result.c_str(), true)) OUT_OF_MEMORY;
			return true;
		} else {
			err("`\\",
			    _word[0],
			    " = system ...' can't access system",
			    "\\");
			return false;
		}
#endif
	} else {
                // Type 6. \syn = "string"
		std::string unquoted;
		int status = ExtractQuote(_cmdLine, unquoted);
		if (status == 0) {
			err("`\\synonym = \"value\" found no double-quoted string");
			return false;
		}
		if (status < 0) {
			err("`\\synonym = \"value\" found starting double-quote but no ending double-quote");
			return false;
		}
		//printf("%s:%d raw <%s> became <%s>\n",__FILE__,__LINE__,_cmdLine,unquoted.c_str());
		if (!put_syn(_word[0], unquoted.c_str(), true)) OUT_OF_MEMORY;
	}
	return true;
}

// `set "\\syn" to "STRING"'
// `set ".var." to  NUMBER'
bool
setCmd()
{
	if (_nword != 4) {
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (strNE(_word[2], "to")) {
		demonstrate_command_usage();
		err("Third word must be `to', not `\\", _word[2], "' as given.", "\\");
		return false;
	}
	std::string name(_word[1]);
	clean_blanks_quotes(name);
	//printf("<%s>  ... <%s>>\n", _word[1], name.c_str());
	if (is_var(name.c_str())) {
		double value;
		if (!getdnum(_word[3], &value)) {
			demonstrate_command_usage();
			err("Cannot interpret `\\", _word[3], "' as a numerical value.", "\\");
			return false;
		}
		PUT_VAR(name.c_str(), value);
	} else if (is_syn(name.c_str())) {
		std::string value(_word[3]);
		if (value.size() < 2
		    || (value[0] != '"' || value[-1 + value.size()] != '"')) {
			demonstrate_command_usage();
			err("Need a double-quoted string to set the synonym to");
			return false;
		}
		value.STRINGERASE(0, 1);
		value.STRINGERASE(-1 + value.size(), 1);		
		put_syn(name.c_str(), value.c_str(), true);
		//printf("Assigned '%s' to synonym named '%s'\n", value.c_str(), name.c_str());
	} else {
		demonstrate_command_usage();
		err("Second word must be a variable name or a double-backslashed synonym name, in double quotes, not `\\", name.c_str(), "' as given.", "\\");
		return false;
	}
	return true;
}
