#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>
#include        <stdlib.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"image_ex.hh"
#include	"private.hh"
#include	"files.hh"
#include	"DataFile.hh"


extern char     _grTempString[];

bool            show_expression_or_stringCmd(void);
bool            show_axesCmd(void);
bool            show_licenseCmd(void);
bool            show_miscCmd(void);
bool            show_colorCmd(void);
bool            show_colornamesCmd(void);
bool            show_columnsCmd(void);
bool            show_columns_statisticsCmd(void);
bool            show_flagsCmd(void);
static bool     show_grid(void);
bool            show_gridCmd(void);
bool            show_hintCmd(void);
bool            show_imageCmd(void);
bool            show_next_lineCmd(void);
#if 0
bool            show_stopwatchCmd(void); // moved to timer.cc
#endif
bool            show_allCmd(void);
bool            show_variablesCmd(void);
bool            show_timeCmd(void);
bool            show_synonymsCmd(void);
static const unsigned int GRID_MAX_WIDTH_TO_SHOW = 32;
#define IMAGE_MAX_WIDTH_TO_SHOW 32
#define MISSING "  ---missing--- "

// If end in "...", no newline added
bool
show_expression_or_stringCmd()
{
    int             i;
    bool            no_newline = false;
    double          value;
    Require(_nword > 1,
	err("`show' what?"));
    for (i = 1; i < _nword; i++) {
	if (i == (_nword - 1) && !strcmp(_word[i], "...")) {
	    no_newline = true;
	    break;
	}
	if (*_word[i] == '"') {
	    // Copy to 's' the unquoted string.
	    int             len = strlen(_word[i]);
	    if (len < 3)
		return true;	// nothing to show anyway
	    char *s = new char[1 + len];
	    if (!s) OUT_OF_MEMORY;
	    strncpy(s, 1 + _word[i], len - 2);
	    *(s + len - 2) = '\0';
	    remove_esc_quotes(s); // may shorten, but can't lengthen
	    ShowStr(s);
	    delete [] s;
	} else if (getdnum(_word[i], &value)) {
	    sprintf(_grTempString, "%g", value);
	    ShowStr(_grTempString);
	} else {
	    err("`show' doesn't understand item `\\", _word[i], "'", "\\");
	    return false;
	}
    }
    if (!no_newline)
	ShowStr("\n");
    return true;
}

bool
show_axesCmd()
{
    double          tmp;
    if (get_var("..xsize..", &tmp)) {
	sprintf(_grTempString, "Plot is %.1f cm wide, ", tmp);
	ShowStr(_grTempString);
    }
    if (get_var("..ysize..", &tmp)) {
	sprintf(_grTempString, "%.1f cm tall; margins ", tmp);
	ShowStr(_grTempString);
    }
    if (get_var("..xmargin..", &tmp)) {
	sprintf(_grTempString, "%.1f cm at left, ", tmp);
	ShowStr(_grTempString);
    }
    if (get_var("..ymargin..", &tmp)) {
	sprintf(_grTempString, "%.1f cm at bottom.\n", tmp);
	ShowStr(_grTempString);
    }
    if (_xscale_exists) {
	if (_xtype == gr_axis_LINEAR) {
	    sprintf(_grTempString,
		    "Linear x axis, named `%s'\n  ranges from %f to %f stepping by %f\n",
		    _colX.getName(), _xleft, _xright, _xinc);
	} else if (_xtype == gr_axis_LOG) {
	    sprintf(_grTempString,
	       "Logarithmic x axis, named `%s'\n  ranges from %f to %f stepping by %f\n",
		    _colX.getName(), _xleft, _xright, _xinc);
	} else {
	    sprintf(_grTempString,
	       "Unknown-type x axis, named `%s'\n  ranges from %f to %f stepping by %f\n",
		    _colX.getName(), _xleft, _xright, _xinc);
	}
	ShowStr(_grTempString);
	if (_clipData) {
	    ShowStr("  Data are clipped to these limits\n");
	} else {
	    ShowStr("  Data are not clipped to these limits\n");
	}
    } else {
	ShowStr("x scale not defined yet\n");
    }
    if (_yscale_exists) {
	if (_ytype == gr_axis_LINEAR) {
	    sprintf(_grTempString,
		    "Linear y axis, named `%s'\n  ranges from %f to %f stepping by %f\n",
		    _colY.getName(), _ybottom, _ytop, _yinc);
	} else if (_ytype == gr_axis_LOG) {
	    sprintf(_grTempString,
	       "Logarithmic y axis, named `%s'\n  ranges from %f to %f stepping by %f\n",
		    _colY.getName(), _ybottom, _ytop, _yinc);
	} else {
	    sprintf(_grTempString,
	      "Unknown-type y axis, named `%s'\n  ranges from %f to %f stepping by %f\n",
		    _colY.getName(), _ybottom, _ytop, _yinc);
	}
	ShowStr(_grTempString);
	if (_clipData) {
	    ShowStr("  Data are clipped to these limits\n");
	} else {
	    ShowStr("  Data are not clipped to these limits\n");
	}
    } else {
	ShowStr("y scale not defined yet\n");
	return false;
    }
    return true;
}

bool
show_licenseCmd()
{
    extern string _lib_directory;
    string license(_lib_directory.c_str());
    license.append("/license.txt");
    more_file_to_terminal(license.c_str());
    return true;
}
bool
show_miscCmd()
{
    double          tmp;
    ShowStr("Miscellaneous information:\n");
    sprintf(_grTempString, "Missing value= %g\n", gr_currentmissingvalue());
    ShowStr(_grTempString);
    if (get_var("..fontsize..", &tmp)) {
	sprintf(_grTempString, "Fontsize     = %5.0f points\n", tmp);
	ShowStr(_grTempString);
    }
    double r, g, b, h, s, v;
    if (_griState.color_line().isRGB()) {
	r = _griState.color_line().getR();
	g = _griState.color_line().getG();
	b = _griState.color_line().getB();
	sprintf(_grTempString, "Line color: red=%.2f green=%.2f blue=%.2f\n",
		r, g, b);
    } else {
	h = _griState.color_line().getH();
	s = _griState.color_line().getS();
	v = _griState.color_line().getV();
	gr_hsv2rgb(h, s, v, &r, &g, &b);
	sprintf(_grTempString, "Line color: hue=%.2f saturation=%.2f brightness=%.2f\n", h, s, v);
    }
    if (r == g && g == b)
	sprintf(_grTempString, "Line graylevel    = %5.1f (note: 0=black, 1=white)\n", r);
    ShowStr(_grTempString);
    if (_griState.color_text().isRGB()) {
	r = _griState.color_text().getR();
	g = _griState.color_text().getG();
	b = _griState.color_text().getB();
	sprintf(_grTempString, "Text color: red=%.2f green=%.2f blue=%.2f\n",
		r, g, b);
    } else {
	h = _griState.color_text().getH();
	s = _griState.color_text().getS();
	v = _griState.color_text().getV();
	gr_hsv2rgb(h, s, v, &r, &g, &b);
	sprintf(_grTempString, "Text color: hue=%.2f saturation=%.2f brightness=%.2f\n", h, s, v);
    }
    if (r == g && g == b)
	sprintf(_grTempString, "Text graylevel    = %5.1f (note: 0=black, 1=white)\n", r);
    ShowStr(_grTempString);
    if (get_var("..linewidth..", &tmp)) {
	sprintf(_grTempString, "Line width   = %5.1f points\n", tmp);
	ShowStr(_grTempString);
    }
    // Dash
    if (_dash.size() == 0) {
	ShowStr("Line is solid, not dashed\n");
    } else {
	ShowStr("Line is dashed.  Length of dash/blank/dash/blank... = ");
	for (unsigned int i = 0; i < _dash.size(); i++) {
	    sprintf(_grTempString, "%.2fcm ", _dash[i]);
	    ShowStr(_grTempString);
	}
	ShowStr("\n");
    }
    return true;
}

bool
show_colorCmd()
{
    char buffer[256];
    if (_griState.color_line().isRGB() == true) {
	sprintf(buffer, "Lines drawn in RGB color (red=%f, green=%f, blue=%f)\n",
		_griState.color_line().getR(),
		_griState.color_line().getG(),
		_griState.color_line().getB());
	ShowStr(buffer);
    } else {
	sprintf(buffer, "Lines drawn in HSV color (hue=%f, saturation=%f, brightness=%f)\n",
		_griState.color_line().getH(),
		_griState.color_line().getS(),
		_griState.color_line().getV());
	ShowStr(buffer);
    }

    if (_griState.color_text().isRGB() == true) {
	sprintf(buffer, "Text drawn in RGB color (red=%f, green=%f, blue=%f)\n",
		_griState.color_text().getR(),
		_griState.color_text().getG(),
		_griState.color_text().getB());
	ShowStr(buffer);
    } else {
	sprintf(buffer, "Text drawn in HSV color (hue=%f, saturation=%f, brightness=%f)\n",
		_griState.color_text().getH(),
		_griState.color_text().getS(),
		_griState.color_text().getV());
	ShowStr(buffer);
    }
    return true;
}

bool
show_colornamesCmd()
{
    extern bool display_colors();
    display_colors();
    return true;
}

bool
show_columnsCmd()
{
    unsigned int    i;
    unsigned        xlength = _colX.size();
    unsigned        rlength = _colR.size();
    if (xlength < 1 && rlength < 1) {
	warning("`show columns' -- no columns exist");
	return true;
    }

    unsigned int length = xlength == 0 ? rlength : xlength;

    if (_nword == 2) {
	// `show columns'
	// print label line
	sprintf(_grTempString, "\"x\" column has name `%s'\n", 
		unbackslash(_colX.getName()));
	ShowStr(_grTempString);
	sprintf(_grTempString, "\"y\" column has name `%s'\n", 
		unbackslash(_colY.getName()));
	ShowStr(_grTempString);
	sprintf(_grTempString, "%s:\n", "column data");
	ShowStr(_grTempString);
	if (xlength > 0) {
	    sprintf(_grTempString, "%15s\t", "x");
	    ShowStr(_grTempString);
	}
	if (_colY.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "y");
	    ShowStr(_grTempString);
	}
	if (_colU.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "u");
	    ShowStr(_grTempString);
	}
	if (_colV.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "v");
	    ShowStr(_grTempString);
	}
	if (_colZ.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "z");
	    ShowStr(_grTempString);
	}
	if (_colR.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "r");
	    ShowStr(_grTempString);
	}
	if (_colTHETA.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "theta");
	    ShowStr(_grTempString);
	}
	if (_colWEIGHT.size() > 0) {
	    sprintf(_grTempString, "%15s\t", "weight");
	    ShowStr(_grTempString);
	}
	ShowStr("\n");
	for (i = 0; i < length; i++) {
	    double tmp;
	    if (xlength > 0) {
		tmp = _colX[i];
		if (gr_missingx(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colY.size() > 0) {
		tmp = _colY[i];
		if (gr_missingy(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colU.size() > 0) {
		tmp = _colU[i];
		if (gr_missing(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colV.size() > 0) {
		tmp = _colV[i];
		if (gr_missing(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colZ.size() > 0) {
		tmp = _colZ[i];
		if (gr_missing(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colR.size() > 0) {
		tmp = _colR[i];
		if (gr_missing(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colTHETA.size() > 0) {
		tmp = _colTHETA[i];
		if (gr_missing(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    if (_colWEIGHT.size() > 0) {
		tmp = _colWEIGHT[i];
		if (gr_missing(tmp)) {
		    ShowStr(MISSING);
		} else {
		    sprintf(_grTempString, "%15g\t", tmp);
		    ShowStr(_grTempString);
		}
	    }
	    ShowStr("\n");
	}
    } else if (_nword == 3 && !strncmp(_word[2], "sta", 3)) {
	// `show columns statistics'
	show_columns_statisticsCmd();
    }
    return true;
}

bool
show_columns_statisticsCmd()
{
    double          ave, adev, sdev, svar, skew, kurt, q1, q2, q3;
    if (_colX.size() > 0) {
	moment(_colX.begin(),
	       _colX.size(),
	       &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colX.begin(), _colX.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
x[1-%4d]:         mean       stddev     skewness     kurtosis\n\
          %12g %12g %12g %12g\n",
		_colX.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colX.min(), q1, q2, q3, _colX.max());
	gr_textput(_grTempString);
    }
    if (_colY.size() > 0) {
	moment(_colY.begin(), _colY.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colY.begin(), _colY.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
y[1-%4d]:         mean       stddev     skewness     kurtosis\n\
           %12g %12g %12g %12g\n",
		_colY.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colY.min(), q1, q2, q3, _colY.max());
	gr_textput(_grTempString);
    }
    if (_colZ.size() > 0) {
	moment(_colZ.begin(), _colZ.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colZ.begin(), _colZ.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
z[1-%4d]:         mean       stddev     skewness     kurtosis\n\
           %12g %12g %12g %12g\n",
		_colZ.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colZ.min(), q1, q2, q3, _colZ.max());
	gr_textput(_grTempString);
    }
    if (_colU.size() > 0) {
	moment(_colU.begin(), _colU.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colU.begin(), _colU.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
u[1-%4d]:         mean       stddev     skewness     kurtosis\n\
           %12g %12g %12g %12g\n",
		_colU.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colU.min(), q1, q2, q3, _colU.max());
	gr_textput(_grTempString);
    }
    if (_colV.size() > 0) {
	moment(_colV.begin(), _colV.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colV.begin(), _colV.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
v[1-%4d]:         mean       stddev     skewness     kurtosis\n\
           %12g %12g %12g %12g\n",
		_colV.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colV.min(), q1, q2, q3, _colV.max());
	gr_textput(_grTempString);
    }
    if (_colR.size() > 0) {
	moment(_colR.begin(), _colR.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colR.begin(), _colR.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
r[1-%4d]:         mean       stddev     skewness     kurtosis\n\
           %12g %12g %12g %12g\n",
		_colR.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colR.min(), q1, q2, q3, _colR.max());
	gr_textput(_grTempString);
    }
    if (_colTHETA.size() > 0) {
	moment(_colTHETA.begin(), _colTHETA.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colTHETA.begin(), _colTHETA.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
theta[1-%4d]:         mean       stddev     skewness     kurtosis\n\
                   %12g %12g %12g %12g\n",
		_colY.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                    minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colTHETA.min(), q1, q2, q3, _colTHETA.max());
	gr_textput(_grTempString);
    }
    if (_colWEIGHT.size() > 0) {
	moment(_colWEIGHT.begin(), _colWEIGHT.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
	histogram_stats(_colWEIGHT.begin(), _colWEIGHT.size(), &q1, &q2, &q3);
	sprintf(_grTempString, "\
theta[1-%4d]:         mean       stddev     skewness     kurtosis\n\
                   %12g %12g %12g %12g\n",
		_colWEIGHT.size(), ave, sdev, skew, kurt);
	gr_textput(_grTempString);
	sprintf(_grTempString, "\
                    minimum           q1       median           q3      maximum\n\
          %12g %12g %12g %12g %12g\n",
		_colWEIGHT.min(), q1, q2, q3, _colWEIGHT.max());
	gr_textput(_grTempString);
    }
    return true;
}

bool
show_flagsCmd(void)
{
    extern void show_flags();	// set.cc
    show_flags();
    return true;
}

bool
show_gridCmd()
{
    if (_nword == 2) 
	return show_grid();
    else if (_nword == 3 && word_is(2, "mask"))
	return show_grid_maskCmd();
    else {
	demonstrate_command_usage();
	NUMBER_WORDS_ERROR;
	return false;
    }
}

bool
show_grid_maskCmd()
{
    if (_num_ymatrix_data < 1)
	return true;
    unsigned int j = _num_ymatrix_data - 1;
    do {
	for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
	    if (_legit_xy(i, j) == true) {
		ShowStr("1 ");
	    } else {
		ShowStr("0 ");
	    }
	}
	ShowStr("\n");
    } while (j-- != 0);
    return true;
}

static bool
show_grid(void)
{
    if (!_grid_exists && !_xgrid_exists && !_ygrid_exists) {
	ShowStr("No grid data exist yet.\n");
	return true;
    }
    if (_xgrid_exists) {
	if (_num_xmatrix_data < GRID_MAX_WIDTH_TO_SHOW) {
	    ShowStr("x-locations of matrix are:\n");
	    for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
		sprintf(_grTempString, "%f\n", _xmatrix[i]);
		ShowStr(_grTempString);
	    }
	} else {
	    sprintf(_grTempString, "Grid `x' has %d elements (too many to display)\n",
		    _num_xmatrix_data);
	    ShowStr(_grTempString);
	}
    } else {
	ShowStr("Grid `x' not defined yet\n");
    }
    if (_ygrid_exists) {
	if (_num_ymatrix_data < GRID_MAX_WIDTH_TO_SHOW) {
	    ShowStr("y-locations of matrix are:\n");
	    for (unsigned int j = 0; j < _num_ymatrix_data; j++) {
		sprintf(_grTempString, "%f\n", _ymatrix[j]);
		ShowStr(_grTempString);
	    }
	} else {
	    sprintf(_grTempString, "Grid `y' has %d elements (too many to display)\n",
		    _num_ymatrix_data);
	    ShowStr(_grTempString);
	}
    } else {
	ShowStr("Grid `y' not defined yet\n");
    }
    if (!_grid_exists) {
	ShowStr("Grid data don't exist yet\n");
	return true;
    }
    if (_num_xmatrix_data < GRID_MAX_WIDTH_TO_SHOW
	&& _num_ymatrix_data < GRID_MAX_WIDTH_TO_SHOW) {
	if (_f_min == _f_max) {
	    if (gr_missing((double) _f_min)) {
		ShowStr("Grid data don't exist yet\n");
		return true;
	    } else {
		sprintf(_grTempString, "Grid data all equal to %f", _f_min);
		ShowStr(_grTempString);
		return true;
	    }
	} else {
	    ShowStr("Grid data:\n");
	    unsigned int j = _num_ymatrix_data - 1;
	    do {
		for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
		    if (_legit_xy(i, j) == true) {
			sprintf(_grTempString, "%g\t", _f_xy(i, j));
			ShowStr(_grTempString);
		    } else {
			ShowStr("   *   \t");
		    }
		}
		ShowStr("\n");
	    } while (j-- != 0);
	}
    } else {
	ShowStr("Grid `data' matrix is too big to display\n");
    }
    sprintf(_grTempString, 
	    "Grid is %d wide and %d tall, with values ranging from %f to %f\n",
	    _num_xmatrix_data,
	    _num_ymatrix_data,
	    _f_min,
	    _f_max);
    ShowStr(_grTempString);

#if 1
    vector<double> tmp;
    for (unsigned int i = 0; i < _num_xmatrix_data; i++)
	for (unsigned int j = 0; j < _num_ymatrix_data; j++)
	    if (_legit_xy(i, j) == true)
		tmp.push_back(_f_xy(i, j));
    double          ave, adev, sdev, svar, skew, kurt;
    moment(tmp.begin(), tmp.size(), &ave, &adev, &sdev, &svar, &skew, &kurt);
    sprintf(_grTempString, 
	    "Grid statistics, for %d legitimate data: mean=%f stddev=%f skewness=%f kurtosis=%f\n",
	    tmp.size(), ave, sdev, skew, kurt);
    ShowStr(_grTempString);
#endif

    return true;
}

// Display hint of the day, possibly downloading it from gri
// www site, if no hint file exists, or if it is out of date.
bool
show_hintCmd()
{
    char *lynx_cmd = "\
lynx -dump \"http://www.phys.ocean.dal.ca/cgi-bin/ocean/gri_hints\" | tail +7 >> %s";
    char *home = getenv("HOME");
    char *name = "/.gri-hint-cache";
    char *filename = new char [1 + strlen(home) + strlen(name)];
    if (!filename) OUT_OF_MEMORY;
    strcpy(filename, home);
    strcat(filename, name);
#if defined(VMS) || defined(MSDOS) || defined(IS_DEC_ALPHA)
    time_t          sec;
#else
#if defined(__DECCXX)
    int             sec;
#else
    long            sec;
#endif
#endif
    time(&sec);
    char now[30];		// 27 should be enough by Sun manpage
    strcpy(now, asctime(localtime(&sec)));
    // String now is e.g. "Thu Jul 20 19:57:26 1995\n\0"; trim to get
    // just the dayname/month/day
    unsigned int i, count = 0;
    for (i = 0; i < strlen(now); i++) {
	if (now[i] == ' ')
	    count++;
	if (count == 3)
	    break;
    }
    if (count != 3)
	gri_exit(2);
    now[i] = '\0';
    bool get_new_hints = false;
    FILE *fp = fopen(filename, "r");
    if (fp) {
	char file_time[100];
	fgets(file_time, 99, fp);
	file_time[strlen(file_time) - 1 ] = '\0';
	if (strcmp(file_time, now))
	    get_new_hints = true;
	fclose(fp);
    } else {
	get_new_hints = true;
    }
    char cmd[200];
    if (get_new_hints) {
	sprintf(cmd, "echo '%s' > %s\n", now, filename);
	system(cmd);
	sprintf(cmd, lynx_cmd, filename);
	ShowStr("Getting new hints file from Gri WWW site ...");
	system(cmd);
	ShowStr(" done\n");
    }
    // Now see if we have an up-to-date hints file
    fp = fopen(filename, "r");
    if (!fp) {
	warning("Sorry, but the ~/gri-hint-cache file does not exist\n");
	return true;		// do not fail only because of this
    }
    sprintf(cmd, "cat %s\n", filename);
    system(cmd);
    return true;
}

bool
show_imageCmd()
{
    double          sum;
    if (_nword != 2) {
	demonstrate_command_usage();
	NUMBER_WORDS_ERROR;
	return false;
    }
    if (!image_exists()) {
	ShowStr("No image exists yet\n");
	return false;
    }
    sprintf(_grTempString, "Image: %d cols,  %d rows, %f<=x<=%f and %f<=y<=%f\n",
	    _image.ras_width, _image.ras_height,
	    _image_llx, _image_urx, _image_lly, _image_ury);
    ShowStr(_grTempString);
    sprintf(_grTempString, "Image=000 <-> %f;  ", _image0);
    ShowStr(_grTempString);
    sprintf(_grTempString, "Image=255 <-> %f\n", _image255);
    ShowStr(_grTempString);
    sprintf(_grTempString, "\
Image histogram:\n\
 Image #      Physical value       Percentage  Each * represents 2%% of pixels\n\
--------  -----------------------  ----------  ------------------------------\n");
    ShowStr(_grTempString);
    calculate_image_histogram();
    sum = 0.0;
    for (unsigned int i = 0; i < 256; i++) {
	sum += _imageHist[i];
	if (i && (!(i % 16) || i == 255)) {
	    sprintf(_grTempString, "%3d->%3d ", i - 16, i);
	    ShowStr(_grTempString);
	    sprintf(_grTempString, "%11g->%11g  %10.3f ",
		    _image0 + (i - 16) * (_image255 - _image0) / 255.0,
		    _image0 + i * (_image255 - _image0) / 255.0,
		    100.0 * sum);
	    ShowStr(_grTempString);
	    if (sum > 0.0) {
		unsigned int num = (unsigned int) floor(50.0 * sum);
		for (unsigned int j = 0; j < num; j++) {
		    ShowStr("*");
		}
	    }
	    sum = 0.0;
	    ShowStr("\n");
	}
    }
    if (_image.ras_width < IMAGE_MAX_WIDTH_TO_SHOW
	&& _image.ras_height < IMAGE_MAX_WIDTH_TO_SHOW) {
        int j;
	ShowStr("Image is, in terms of internal values from 0 to 255:\n");
	for (j = int(_image.ras_height - 1); j > -1; j--) {
	    for (unsigned int i = 0; i < _image.ras_width; i++) {
		if (!*(_imageMask.image + i * _image.ras_height + j)) {
		    sprintf(_grTempString, "%03d ",
			    *(_image.image + i * _image.ras_height + j));
		    ShowStr(_grTempString);
		} else {
		    ShowStr("*** ");
		}
	    }
	    ShowStr("\n");
	}
	ShowStr("\nImage is, in terms of user values:\n");
	float scale = 255.0 / (_image255 - _image0);
	for (j = int(_image.ras_height - 1); j > -1; j--) {
	    for (unsigned int i = 0; i < _image.ras_width; i++) {
		if (!*(_imageMask.image + i * _image.ras_height + j)) {
		    int val = *(_image.image + i * _image.ras_height + j);
		    float val_float;
		    if (_imageTransform == NULL)
			val_float = _image0 + float(val) / scale;
		    else 
			val_float = _image0 + _imageTransform[val] / scale;
		    sprintf(_grTempString, "%10f ", val_float);
		    ShowStr(_grTempString);
		} else {
		    ShowStr("********* ");
		}
	    }
	    ShowStr("\n");
	}
    } else {
	ShowStr("Image is too big to display here\n");
    }
    return true;
}

bool
show_next_lineCmd()
{
    int             result_code;
#if defined(VMS)
    warning("`show next line' does not work on VMS");
    return true;
#endif
    if (_dataFILE.back().get_type() == DataFile::from_cmdfile)
	return true;
    // Cannot show for binary files -- don't bother with warning though
    if (_dataFILE.back().get_type() != DataFile::ascii)
	return true;
    if (feof(_dataFILE.back().get_fp())) {
	sprintf(_grTempString, "%s is at END-OF-FILE.\n",
		_dataFILE.back().get_name());
	ShowStr(_grTempString);
	return true;
    }
    fgets(_errorMsg, LineLength_1, _dataFILE.back().get_fp());
    if (feof(_dataFILE.back().get_fp())) {
	sprintf(_grTempString, "%s is at END-OF-FILE\n",
		_dataFILE.back().get_name());
	ShowStr(_grTempString);
	return true;
    }
    sprintf(_grTempString, "%sNext line (%d): %s", _margin.c_str(),
	    _dataFILE.back().get_line(), _errorMsg);
    ShowStr(_grTempString);
    result_code = fseek(_dataFILE.back().get_fp(), long(-strlen(_errorMsg)), 1);
    if (result_code) {
	sprintf(_errorMsg, "`show next line' had internal problems [%d]", result_code);
	warning(_errorMsg);
    }
    return true;
}

// show_stopwatchCmd() -- moved to timer.cc

bool
show_allCmd()
{
    // vsn0.97: need to test _word because `show' gets parsed to here
    if (_nword == 2) {
	show_miscCmd();
	show_axesCmd();
	show_columnsCmd();
	show_gridCmd();
	show_imageCmd();
	show_variablesCmd();
	show_synonymsCmd();
	if (!_cmdFILE.back().get_interactive()) {
	    show_next_lineCmd();
	}
    } else {
	err("`show' what?");
	return false;
    }
    return true;
}

bool
show_tracebackCmd()
{
    display_cmd_being_done_stack();
    return true;
}

bool
show_timeCmd()
{
#if defined(VMS) || defined(MSDOS) || defined(IS_DEC_ALPHA)
    time_t          sec;
#else
#if defined(__DECCXX)
    int             sec;
#else
    long            sec;
#endif
#endif
    time(&sec);
    sprintf(_grTempString, "%s", asctime(localtime(&sec)));
    ShowStr(_grTempString);
    return true;
}
