//#define DEBUG_READ 1 // Debug

//     #define REMOVE_COMMENTS_FROM_DATA 1 
// Uncomment the preceding line to make Gri remove comments
// from data lines.  It used to do this by default, but
// E.N. pointed out that it was a silly idea from the get-go.


#include <string>
#include <algorithm>	// for reverse
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include	"gr.hh"
#include	"extern.hh"
#include	"image_ex.hh"
#include	"defaults.hh"
#include        "gr_coll.hh"
#include        "files.hh"
#include        "errors.hh"
#include        "DataFile.hh"

#if defined(HAVE_LIBNETCDF)
#include        "netcdf.h"
#endif

unsigned int chars_read = 0;
unsigned int offset_for_read = 0;

bool            read_colornamesCmd(void);
bool            read_columnsCmd(void);
void            skip_hash_headers(FILE * fp);
bool            record_number_unclipped_data(void);
bool            read_gridCmd(void);
bool            read_grid_xCmd(void);
static double   vector_repeats(double *v, int n);
bool            read_grid_yCmd(void);
bool            read_grid_dataCmd(void);
bool            read_image_pgmCmd(void);
bool            read_image_mask_rasterfileCmd(void);
bool            read_image_rasterfileCmd(void);
static bool     read_pgm_image(FILE * fp, IMAGE * im);
static bool     read_raster_image(FILE * fp, IMAGE * im);
bool            read_imageCmd(void);
bool            read_image_colorscaleCmd(void);
bool            read_image_grayscaleCmd(void);
bool            read_image_maskCmd(void);
#if defined(HAVE_LIBNETCDF)
static bool     read_netCDF_column(unsigned int iword, GriColumn *col, int *expected_length, bool append);
#endif
bool            read_synonym_or_variableCmd(void);
bool            read_lineCmd();
static eof_status get_next_data_line(const char *prompt, unsigned int expected_fields);
static bool     get_next_data_word(void);

// Defined elsewhere
extern bool     ignore_initial_newline(void);

extern char     _grTempString[];
// following defined in set.c
extern double   _input_data_window_x_min;
extern double   _input_data_window_x_max;
extern double   _input_data_window_y_min;
extern double   _input_data_window_y_max;
extern bool     _input_data_window_x_exists;
extern bool     _input_data_window_y_exists;

static bool     maybe_make_grids(void);
static GriString inLine(128);	// Start short
static double   tmpf, tmpf2;
static int colu, colv, colx, coly, colz, colweight;

bool
read_from_filenameCmd()
{
	if (_nword == 3) {
		std::string fname(_word[2]);
		un_double_quote(fname);
		if (!push_data_file_to_top(fname.c_str())) {
			err("Must `open \\",
			    fname.c_str(),
			    "' before doing `read from",
			    fname.c_str(), "'", "\\");
			return false;
		} else {
			return true;
		}
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
}

// `read colornames from RGB "\filename"'
bool
read_colornamesCmd()
{
	chars_read = 0;
	FILE *fp;
	char name[100];		// should be long enough ??
	std::string fname(_word[4]);
	un_double_quote(fname);
	fp = fopen(fname.c_str(), "r");
	Require(fp != NULL,
		err("`read colornames' cannot open file `\\", fname.c_str(), "'", "\\"));
	while(!feof(fp)) {
		unsigned num;
		char name2[100], name3[100], name4[100];
		int rr, gg, bb;
		double r, g, b;
		fgets(_grTempString, _grTempStringLEN - 1, fp);
		if (_grTempString[skip_space(_grTempString)] != '!') {
			//printf("<%s>\n",_grTempString);
			num = sscanf(_grTempString, 
				     "%d %d %d %s %s %s %s",
				     &rr, &gg, &bb, name, name2, name3, name4);
			switch (num - 3) {
			case 1:
				break;
			case 2:
				strcat(name, " ");
				strcat(name, name2);
				break;
			case 3:
				strcat(name, " ");
				strcat(name, name2);
				strcat(name, " ");
				strcat(name, name3);
				break;
			case 4:
				strcat(name, " ");
				strcat(name, name2);
				strcat(name, " ");
				strcat(name, name3);
				strcat(name, " ");
				strcat(name, name4);
				break;
			default:
				warning("`read colornames' found too many words in RGB colorname; truncating to `\\", name, "'", "\\");
			}
			r = rr / 255.0;
			g = gg / 255.0;
			b = bb / 255.0;
			//printf("COLOR '%s' %f %f %f\n",name,r,g,b);
			create_color(name, r, g, b);
		} else {
			//printf("HEADER <%s>\n",_grTempString);
		}
	}
	return true;
}

#if defined(HAVE_LIBNETCDF)
static bool
read_netCDF_column(unsigned int iword, GriColumn *col, int *expected_length, bool append)
{
	if (iword + 1 < _nword 
	    && !strcmp(_word[iword + 1], "=")) {
		// ASSUME have 2 more words
		long start[1], edges[1];
		long length;		// Length of column 
		int var_id, dim_ids[10], dims;
		std::string varname(_word[iword + 2]);
		un_double_quote(varname);
		var_id = ncvarid(_dataFILE.back().get_netCDF_id(), varname.c_str());
		Require(var_id != -1, err("Cannot find netCDF variable `\\", varname.c_str(), "'", "\\"));
		nc_type type;
		Require(-1 != ncvarinq(_dataFILE.back().get_netCDF_id(),
				       var_id, (char *) 0, &type,
				       &dims, dim_ids, (int *) 0),
			err("Cannot find info on netCDF variable `\\", varname.c_str(), "'", "\\"));
		Require(dims == 1,
			err("Cannot `read columns ... x=\"\\", varname.c_str(),
			    "\" since it is not a vector", "\\"));
		Require(-1 != ncdiminq(_dataFILE.back().get_netCDF_id(),
				       dim_ids[0], (char *)0, &length),
			err("Cannot determine length of netCDF variable `\\", varname.c_str(), "'", "\\"));
		Require(length > 0,
			err("netCDF variable `\\", varname.c_str(), "' has zero length", "\\"));
		if (*expected_length && length != *expected_length) {
			err("Length of netCDF variable `\\", varname.c_str(),
			    "' disagrees with length of a previously scanned column", "\\");
			return false;
		}
		*expected_length = int(length);
		unsigned int old_length;
		if (append) {
			old_length = col->size();
			col->setDepth((unsigned int) (length + old_length));
		} else {
			old_length = 0;
			col->setDepth((unsigned int)length);
		}
		start[0] = 0;
		edges[0] = length;
		switch(type) {
		case NC_FLOAT:
		{
			float *tmp = (float*)NULL;
			GET_STORAGE(tmp, float, (size_t)length);
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(),
					   var_id, start, edges, (void *)tmp)) {
				err("Error reading variable `\\", varname.c_str(), "' from netCDF file", "\\");
			}
			for (unsigned int ii = 0; ii < (unsigned int) length; ii++)
				(*col)[ii + old_length] = tmp[ii];
			free(tmp);
		}
		break;
		case NC_DOUBLE:
		{
			double *tmp = (double*)NULL;
			GET_STORAGE(tmp, double, (size_t)length);
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(),
					   var_id, start, edges, (void *)tmp)) {
				err("Error reading variable `\\", varname.c_str(), "' from netCDF file", "\\");
			}
			for (unsigned int ii = 0; ii < (unsigned int) length; ii++)
				(*col)[ii + old_length] = tmp[ii];
			free(tmp);
		}
		break;
		default:
			err("Cannot handle the type of netCDF variable `\\", varname.c_str(), "'", "\\");
		}
		_columns_exist = true;
		return true;
	} else { 
		err("Cannot parse command");
		return false; 
	}
}
#endif

static int
figure_column(const char *w, int def)
{
	char *ptr = strstr(w, "=");
	if (ptr == NULL)
		return def;
	double res;
	if (!getdnum(ptr + 1, &res))
		err("Cannot read column number in `\\", w, "'", "\\");
	return int(floor(res + 0.5));
}

bool
read_columnsCmd()
{
	chars_read = 0;
	int             lines_with_missing_data = 0;
	double          missing = gr_currentmissingvalue(); // call once to speed
	unsigned int    i;
	bool            last_point_was_inside = false;
	int             number_to_read, number_read = 0;
	bool            number_specified;
	int             maxCol, row;
	eof_status      end_of_data = no_eof;	// flag for end of data
	int             number_outside_window = 0, number_made_missing = 0;
	bool            old = _ignore_error;
	bool            append = false; // appending to end of existing?
	if (word_is(_nword - 1, "appending")) {
		_nword--;		// will undo later
		append = true;
	}
	if (_nword < 3) {
		err("`read columns' must specify columns [eg `read columns x y']");
		if (append)
			_nword++;
		return false;
	}
	// Figure out how many lines to read, if user specified it
	_ignore_error = true;
	if (getdnum(_word[2], &tmpf)) {
		if (_dataFILE.back().get_type() == DataFile::bin_netcdf) {
			demonstrate_command_usage();
			err("You may not specify number of data with netCDF files.");
			if (append)
				_nword++;
			return false;
		}
		number_specified = true;
		number_to_read = (int) floor(0.5 + fabs((double) tmpf));
	} else {
		number_specified = false;
		number_to_read = 0;	// irrelevant; prevent compiler warnings
	}
	_ignore_error = old;
	if (number_specified) {
		if (number_to_read == 0) {
			warning("`read columns 0 ...' is legal but suspicious");
			if (append)
				_nword++;
			RETURN_VALUE("0 rows 0 non-missing 0 inside-clip-region");
			return true;
		} else if (number_to_read < 0) {
			NO_NEGATIVE_ERROR(".n.");
			if (append)
				_nword++;
			return false;
		}
	}
	// Zero out columns, unless appending
	if (!append) {
		_colX.setDepth(0);
		_colY.setDepth(0);
		_colZ.setDepth(0);
		_colU.setDepth(0);
		_colV.setDepth(0);
		_colWEIGHT.setDepth(0);
	}
	//
	// Handle netCDF as special case
	if (_dataFILE.back().get_type() == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
		int expected_length = 0;
		for (i = 2 + (int) number_specified; i < _nword; i += 3) {
			if (!strcmp(_word[i], "u")) {
				if (!read_netCDF_column(i, &_colU, &expected_length, append)) {
					if (append)
						_nword++;
					return false;
				}
			} else if (!strcmp(_word[i], "v")) {
				if (!read_netCDF_column(i, &_colV, &expected_length, append)) {
					if (append)
						_nword++;
					return false;
				}
			} else if (!strcmp(_word[i], "x")) {
				if (!read_netCDF_column(i, &_colX, &expected_length, append)) {
					if (append)
						_nword++;
					return false;
				}
			} else if (!strcmp(_word[i], "y")) {
				if (!read_netCDF_column(i, &_colY, &expected_length, append)) {
					if (append)
						_nword++;
					return false;
				}
			} else if (!strcmp(_word[i], "z")) {
				if (!read_netCDF_column(i, &_colZ, &expected_length, append)) {
					if (append)
						_nword++;
					return false;
				}
			} else if (!strcmp(_word[i], "weight")) {
				if (!read_netCDF_column(i, &_colWEIGHT, &expected_length, append)) {
					if (append)
						_nword++;
					return false;
				}
			} else {
				err("unknown item `\\", _word[i], "' in `read columns'", "\\");
				if (append)
					_nword++;
				return false;
			}
		}
#else
		err("Not compiled with netCDF library");	
#endif
	} else {
		// Data in a normal ascii or binary file
		// Find what cols are data in.
		colu = colv = colx = coly = colz = colweight =-1;
		for (i = 2 + (int) number_specified; i < _nword; i++) {
			if (!strncmp(_word[i], "u", 1)) {
				colu = figure_column(_word[i], i - 1 - number_specified);
			} else if (!strncmp(_word[i], "v", 1)) {
				colv = figure_column(_word[i], i - 1 - number_specified);
			} else if (!strncmp(_word[i], "x", 1)) {
				colx = figure_column(_word[i], i - 1 - number_specified);
			} else if (!strncmp(_word[i], "y", 1)) {
				coly = figure_column(_word[i], i - 1 - number_specified);
			} else if (!strncmp(_word[i], "z", 1)) {
				colz = figure_column(_word[i], i - 1 - number_specified);
			} else if (!strncmp(_word[i], "weight", 5)) {
				colweight = figure_column(_word[i], i - 1 - number_specified);
			} else if (!strcmp(_word[i], "*")) {
				// should this have something?
			} else {
				err("unknown item `\\",
				    _word[i],
				    "' in `read columns'\n",
				    "       Possibly you meant to read a named vector from a netCDF\n       file? If so, you should have opened file using netCDF keyword", "\\");
				if (append)
					_nword++;
				return false;
			}
		}
		//
		// Figure out column numbers.
		maxCol = colu;
		if (colv > maxCol)
			maxCol = colv;
		if (colx > maxCol)
			maxCol = colx;
		if (coly > maxCol)
			maxCol = coly;
		if (colz > maxCol)
			maxCol = colz;
		if (colweight > maxCol)
			maxCol = colweight;
		// Read data.
		row = 0;
		while (end_of_data == no_eof && (!number_specified || number_read < number_to_read)) {
			unsigned int numCols;
			// Keep an eye on storage space.
			char prompt[20];
			sprintf(prompt, "row %3d: ", row);
			// Dump data into inLine.
			end_of_data = get_next_data_line(prompt, maxCol);
			number_read++;
#if 0
			if (end_of_data == eof_after_data) {
				warning("Got EOF on end of data line; should have a newline there");
			}
#endif

			//printf("DEBUG %s:%d.  'read columns' got [%s]\n",__FILE__,__LINE__,inLine.getValue());

			//unsigned int this_line_len = strlen(inLine.getValue()) + 1;

#ifdef REMOVE_COMMENTS_FROM_DATA
			remove_comment(inLine.getValue());
#endif
			chop_into_data_words(inLine.getValue(), _word, &numCols, MAX_nword);
#if 0
			printf("%s:%d LINE IS:\n", __FILE__,__LINE__);
			for (int iii = 0; iii < numCols; iii++) {
				printf("<%s> ", _word[iii]);
			}
			printf("\n");
#endif
			PUT_VAR("..words_in_dataline..", double(numCols));
			if (numCols < 1) {	// blank line means done
				end_of_data = eof_before_data; // trick
				break;
			}
			//printf("numCols= %d        maxCol= %d\n", numCols, maxCol);
			if (maxCol > int(numCols)) {
				sprintf(_grTempString,
					"`read columns' -- line %d has %d columns but need %d columns",
					_dataFILE.back().get_line() - 1,
					numCols, maxCol);
				warning(_grTempString);
				if (append)
					_nword++;
				lines_with_missing_data++;
			}
			if (colx <= 0) {
				_colX.push_back((double)row);
			} else {
				if (*_word[colx-1] == '\0' || colx > int(numCols)) {
					_colX.push_back(missing);
				} else {
					if (!getdnum(_word[colx - 1], &tmpf)) {
						err("Can't read x");
						continue;
					}
					_colX.push_back(tmpf);
				}
			}
			if (coly > 0) {
				if (*_word[coly-1] == '\0' || coly > int(numCols)) {
					_colY.push_back(missing);
				} else {
					if (!getdnum(_word[coly - 1], &tmpf)) {
						err("Can't read y");
						continue;
					}
					_colY.push_back(tmpf);
				}
			}
			if (colu > 0) {
				if (*_word[colu-1] == '\0' || colu > int(numCols)) {
					_colU.push_back(missing);
				} else {
					if (!getdnum(_word[colu - 1], &tmpf)) {
						err("Can't read u");
						continue;
					}
					_colU.push_back(tmpf);
				}
			}
			if (colv > 0) {
				if (*_word[colv-1] == '\0' || colv > int(numCols)) {
					_colV.push_back(missing);
				} else {
					if (!getdnum(_word[colv - 1], &tmpf)) {
						err("Can't read v");
						continue;
					}
					_colV.push_back(tmpf);
				}
			}
			if (colz > 0) {
				if (*_word[colz-1] == '\0' || colz > int(numCols)) {
					_colZ.push_back(missing);
				} else {
					if (!getdnum(_word[colz - 1], &tmpf)) {
						err("Can't read z");
						continue;
					}
					_colZ.push_back(tmpf);
				}
			}
			if (colweight > 0) {
				if (*_word[colweight-1] == '\0' || colweight > int(numCols)) {
					_colWEIGHT.push_back(missing);
				} else {
					if (!getdnum(_word[colweight - 1], &tmpf)) {
						err("Can't read weight");
						continue;
					}
					_colWEIGHT.push_back(tmpf);
				}
			}
			// Check whether this is outside a x or y data window
			if (_input_data_window_x_exists) {
				// If an input window exists, check the point to see if it is
				// inside.
				tmpf = _colX.topElement();
				if (tmpf < _input_data_window_x_min
				    || tmpf > _input_data_window_x_max) {
					// The current point is outside.  Insert a missing value if
					// last point was inside; if not, though, simply skip this
					// datum altogether, since the last point will have been
					// flagged as missing anyway. This keeps storage low.
					number_outside_window++;
					if (last_point_was_inside) {
						if (colx > 0) {
							_colX.pop_back();
							_colX.push_back(missing);
						}
						if (coly > 0) {
							_colY.pop_back();
							_colY.push_back(missing);
						}
						if (colu > 0) {
							_colU.pop_back();
							_colU.push_back(missing);
						}
						if (colv > 0) {
							_colV.pop_back();
							_colV.push_back(missing);
						}
						if (colz > 0) {
							_colZ.pop_back();
							_colZ.push_back(missing);
						}
						if (colweight > 0) {
							_colWEIGHT.pop_back();
							_colWEIGHT.push_back(missing);
						}
						row++;
						last_point_was_inside = false;
						number_made_missing++;
						continue;
					} else {
						if (colx > 0)
							_colX.pop_back();
						if (coly > 0)
							_colY.pop_back();
						if (colu > 0)
							_colU.pop_back();
						if (colv > 0)
							_colV.pop_back();
						if (colz > 0)
							_colZ.pop_back();
						last_point_was_inside = false;
						continue;	//row will not be incremented
					}
				} else {
					// The current point is inside.  Set flag and continue with
					// the normal processing.
					last_point_was_inside = true;
				}
			}
			if (_input_data_window_y_exists) {
				// If an input window exists, check the point to see if it is
				// inside.
				tmpf = _colY.topElement();
				if (tmpf < _input_data_window_y_min 
				    || tmpf > _input_data_window_y_max) {
					// The current point is outside.  Insert a missing value
					// if last point was inside; if not, though, simply skip
					// this datum altogether, since the last point will have
					// been flagged as missing anyway. This keeps storage
					// low.
					number_outside_window++;
					if (last_point_was_inside) {
						if (colx > 0) {
							_colX.pop_back();
							_colX.push_back(missing);
						}
						if (coly > 0) {
							_colY.pop_back();
							_colY.push_back(missing);
						}
						if (colu > 0) {
							_colU.pop_back();
							_colU.push_back(missing);
						}
						if (colv > 0) {
							_colV.pop_back();
							_colV.push_back(missing);
						}
						if (colz > 0) {
							_colZ.pop_back();
							_colZ.push_back(missing);
						}
						if (colweight > 0) {
							_colWEIGHT.pop_back();
							_colWEIGHT.push_back(missing);
						}
						row++;
						last_point_was_inside = false;
						number_made_missing++;
						continue;
					} else {
						if (colx > 0)
							_colX.pop_back();
						if (coly > 0)
							_colY.pop_back();
						if (colu > 0)
							_colU.pop_back();
						if (colv > 0)
							_colV.pop_back();
						if (colz > 0)
							_colZ.pop_back();
						last_point_was_inside = false;
						continue;	// row will not be incremented
					}
				} else {
					// The current point is inside.  Set flag and continue
					// with the normal processing.
					last_point_was_inside = true;
				}
			}
			row++;
		}
	}
	if (lines_with_missing_data > 0) {
		sprintf(_grTempString, "`read columns' encountered %d %s with missing data", lines_with_missing_data, lines_with_missing_data == 1 ? "line" : "lines");
		warning(_grTempString);
	}

	// All done reading.
	PUT_VAR("..num_col_data..", double(_colX.size()));
	PUT_VAR("..num_col_data_missing..", double(number_missing_cols()));
	if (end_of_data == eof_before_data && _colX.size() == 0 && !batch())
		warning("`read columns' found EOF or blank line before finding data.");
	if (number_specified) {
		double          trace = 0.0;
		get_var("..trace..", &trace);
		// number was specified
		if (end_of_data != no_eof) {
			sprintf(_grTempString, "%sOnly found %d rows\n", 
				_margin.c_str(), int(_colX.size()));
			gr_textput(_grTempString);
		} else if (trace) {
			sprintf(_grTempString, "%sRead %d data points into columns.\n",
				_margin.c_str(), int(_colX.size()));
			gr_textput(_grTempString);
			if (_input_data_window_x_exists || _input_data_window_y_exists) {
				sprintf(_grTempString, "\
%sIgnored %d data outside window, yielding %d missing values in columns.\n",
					_margin.c_str(), number_outside_window, number_made_missing);
				gr_textput(_grTempString);
			}
		}
	} else {
		double          trace = 0.0;
		get_var("..trace..", &trace);
		// number not specified
		if (trace) {
			sprintf(_grTempString, "%sRead %d data points into columns.\n",
				_margin.c_str(), int(_colX.size()));
			gr_textput(_grTempString);
			if (_input_data_window_x_exists || _input_data_window_y_exists) {
				sprintf(_grTempString, "\
%sIgnored %d data outside window, yielding %d missing values in columns.\n",
					_margin.c_str(), number_outside_window, number_made_missing);
				gr_textput(_grTempString);
			}
		}
	}
	if (_colX.size() > 0) {
		_columns_exist = true;
		if (!_xscale_exists) {
			if (!create_x_scale()) {
				warning("Problem autoscaling -- no data or all x values equal");
			}
		}
	}
	if (_colY.size() > 0) {
		_columns_exist = true;
		if (!_yscale_exists) {
			if (!create_y_scale()) {
				warning("Problem autoscaling -- no data or all y values equal");
			}
		}
	}
	// Recover unused space
	_colX.compact();
	_colY.compact();
	_colZ.compact();
	_colU.compact();
	_colV.compact();
	_colWEIGHT.compact();
	if (append)
		_nword++;
	sprintf(_grTempString, "%d rows %d non-missing %d inside-clip-region",
		int(_colX.size()), 
		int(_colX.size() - number_missing_cols()),
		int(_colX.size() - number_outside_window++));
	RETURN_VALUE(_grTempString);
	return true;
}

// read_gridCmd() -- navigate `read grid' command
bool
read_gridCmd()
{
	chars_read = 0;
	if (_nword < 3) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!strcmp(_word[2], "x")) {
		return read_grid_xCmd();
	} else if (!strcmp(_word[2], "y")) {
		return read_grid_yCmd();
	} else if (!strcmp(_word[2], "data")) {
		return read_grid_dataCmd();
	} else {
		err("`Valid format: `read grid x|y|data'");
		return false;
	}
}

// read_grid_xCmd() -- read x-values for contour matrix
bool
read_grid_xCmd()
{
	chars_read = 0;
	double           repeat = 0.0;
	unsigned int i;
	unsigned int number_to_read;
	eof_status        end_of_data = no_eof;	// flag for end of data
	bool            number_specified;	// was number given?
	if (_dataFILE.back().get_type() == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
		long start[1], edges[1];
		long length;
		int var_id, dim_ids[10], dims;
		if (_nword > 5) {
			demonstrate_command_usage();
			err("Extra words in command.  Expect e.g `read grid x = \"name\"'");
			return false;
		}
		if (_nword != 5 || !word_is(3, "=") || !quoted(_word[4])) {
			demonstrate_command_usage();
			err("Must specify variable name for a netCDF file");
			return false;
		}
		char *varname = new char [strlen(_word[4])]; // skip first char
		if (!varname) OUT_OF_MEMORY;
		strcpy(varname, 1 + _word[4]);
		varname[strlen(varname) - 1] = '\0';
		var_id = ncvarid(_dataFILE.back().get_netCDF_id(), varname);
		if (var_id == -1) {
			err("Cannot find netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		nc_type type;
		if (-1 == ncvarinq(_dataFILE.back().get_netCDF_id(),
				   var_id, (char *) 0, &type,
				   &dims, dim_ids, (int *) 0)) {
			err("Cannot find info on netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		if (dims != 1) {
			err("Cannot `read grid x' from variable `\\", varname,
			    "' since it is not a vector", "\\");
			return false;
		}
		if (-1 == ncdiminq(_dataFILE.back().get_netCDF_id(),
				   dim_ids[0], (char *)0, &length)) {
			err("Cannot determine length of netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		start[0] = 0;
		edges[0] = length;
		if (!allocate_xmatrix_storage((int)length))
			gr_Error("ran out of storage");
		switch(type) {
		case NC_FLOAT:
		{
			float *tmp = (float*)NULL;
			GET_STORAGE(tmp, float, (size_t)length);
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(),
					   var_id, start, edges, (void *)tmp)) {
				err("Error reading variable `\\", varname, "' from netCDF file", "\\");
			}
			for (unsigned int ii = 0; ii < (unsigned int) length; ii++)
				_xmatrix[ii] = tmp[ii];
			free(tmp);
		}
		break;
		case NC_DOUBLE:
		{
			double *tmp = (double*)NULL;
			GET_STORAGE(tmp, double, (size_t)length);
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(),
					   var_id, start, edges, (void *) tmp)) {
				err("Error reading variable `\\", varname, "' from netCDF file", "\\");
			}
			for (unsigned int ii = 0; ii < (unsigned int) length; ii++)
				_xmatrix[ii] = tmp[ii];
			free(tmp);
		}
		break;
		default:
			err("Cannot handle the type of netCDF variable `\\", varname, "'", "\\");
		}
		delete [] varname;
		_num_xmatrix_data = (int)length;
		_xgrid_exists = true;
#else
		demonstrate_command_usage();
		err("Gri internal error: attempting to use nonexistent netCDF library");
#endif
	} else {
		// Figure out how many lines to read; check against existing grid if
		// possible
		switch (_nword) {
		case 3:
			// make initial guess
			number_specified = false;
			_num_xmatrix_data = number_to_read = COLUMN_LEN_DEFAULT;
			break;
		case 4:
			if (!getdnum(_word[3], &tmpf)) {
				demonstrate_command_usage();
				READ_WORD_ERROR(".cols.");
				return false;
			}
			number_to_read = (int) floor(0.5 + fabs((double) tmpf));
			if (number_to_read <= 1) {
				err("Need .cols. > 1");
				return false;
			}
			if (_grid_exists && number_to_read != _num_xmatrix_data) {
				sprintf(_errorMsg, "\
Grid width %d disagrees with existing x-grid (%d); first `delete grid'",
					number_to_read, _num_xmatrix_data);
				err(_errorMsg);
				return false;
			} else {
				_num_xmatrix_data = number_to_read;
			}
			number_specified = true;
			break;
		default:
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		// Get storage
		if (!allocate_xmatrix_storage(number_to_read))
			gr_Error("ran out of storage");
		// Read data
		i = 0;
		while (end_of_data == no_eof && (!number_specified || i < number_to_read)) {
			// Keep an eye on storage space.
			if (i >= _num_xmatrix_data) {
				_num_xmatrix_data += COLUMN_LEN_DEFAULT;
				_xmatrix = (double *)
					realloc((char *) _xmatrix,
						(unsigned) _num_xmatrix_data * sizeof(double));
				if (!_xmatrix)
					gr_Error("ran out of storage");
			}
			char prompt[20];
			sprintf(prompt, "row %3d: ", i);
			end_of_data = get_next_data_line(prompt, 1);
#if 0
			if (end_of_data == eof_after_data) {
				warning("Got EOF on end of data line; should have a newline there");
			}
#endif
#ifdef REMOVE_COMMENTS_FROM_DATA
			remove_comment(inLine.getValue());
#endif
			unsigned int numCols_ui;
			chop_into_data_words(inLine.getValue(), _word, &numCols_ui, MAX_nword);
			PUT_VAR("..words_in_dataline..", double(numCols_ui));
			if (numCols_ui < 1) {	// blank line means done
				end_of_data = eof_before_data; // trick
				break;
			}
			if (!getdnum(_word[0], _xmatrix + i))
				continue;
			i++;
		}
		if (i <= 0) {
			err("`read grid x' found no data");
			return false;
		}
		_num_xmatrix_data = i;
	}
	// All done reading.
	if (!_xscale_exists)
		if (!create_x_scale()) {
			warning("Problem autoscaling -- no data or all x values equal");
		}
	_xgrid_exists = true;
	if (_xmatrix[1] > _xmatrix[0])
		_xgrid_increasing = true;
	else
		_xgrid_increasing = false;
	if ((repeat = vector_repeats(_xmatrix, _num_xmatrix_data))) {
		sprintf(_errorMsg, "\
x-grid has some adjacent values equal (e.g., value %f)",
			repeat);
		warning(_errorMsg);
	}
	sprintf(_grTempString, "%d cols\n", _num_xmatrix_data);
	RETURN_VALUE(_grTempString);
	return true;
}

// check for approximately equal adjacent values
static          double
vector_repeats(double *v, int n)
{
	if (n <= 1)
		return 0.0;
	double range = fabs(v[0] - v[n - 1]) / 1.0e10;
	if (range == 0.0)
		return v[0];
	for (int i = 1; i < n; i++)
		if (fabs(v[i] - v[i - 1]) < range)
			return v[i];
	return 0.0;
}

// read_grid_yCmd() -- read y-values for contour matrix
bool
read_grid_yCmd()
{
	chars_read = 0;
	double           repeat = 0.0;
	unsigned int number_to_read;
	eof_status        end_of_data = no_eof;	// flag for end of data
	bool            number_specified;	// was number given?
	unsigned int i, n;
	if (_dataFILE.back().get_type() == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
		long start[1], edges[1];
		long length;
		int var_id, dim_ids[10], dims;
		if (_nword > 5) {
			demonstrate_command_usage();
			err("Extra words in command.  Expect e.g `read grid y = \"name\"'");
			return false;
		}
		if (_nword != 5 || !word_is(3, "=") || !quoted(_word[4])) {
			demonstrate_command_usage();
			err("Must specify variable name for a netCDF file");
			return false;
		}
		char *varname = new char [strlen(_word[4])]; // skip first char
		if (!varname) OUT_OF_MEMORY;
		strcpy(varname, 1 + _word[4]);
		varname[strlen(varname) - 1] = '\0';
		var_id = ncvarid(_dataFILE.back().get_netCDF_id(), varname);
		if (var_id == -1) {
			err("Cannot find netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		nc_type type;
		if (-1 == ncvarinq(_dataFILE.back().get_netCDF_id(),
				   var_id, (char *) 0, &type,
				   &dims, dim_ids, (int *) 0)) {
			err("Cannot find info on netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		if (dims != 1) {
			err("Cannot `read grid x' from variable `\\", varname,
			    "' since it is not a vector", "\\");
			return false;
		}
		if (-1 == ncdiminq(_dataFILE.back().get_netCDF_id(), dim_ids[0], (char *)0, &length)) {
			err("Cannot determine length of netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		start[0] = 0;
		edges[0] = length;
		if (!allocate_ymatrix_storage((int)length))
			gr_Error("ran out of storage");
		switch(type) {
		case NC_FLOAT:
		{
			float *tmp = (float*)NULL;
			GET_STORAGE(tmp, float, (size_t)length);
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(),
					   var_id, start, edges, (void *)tmp)) {
				err("Error reading variable `\\", varname, "' from netCDF file", "\\");
			}
			for (unsigned int ii = 0; ii < (unsigned int) length; ii++)
				_ymatrix[ii] = tmp[ii];
			free(tmp);
		}
		break;
		case NC_DOUBLE:
		{
			double *tmp = (double*)NULL;
			GET_STORAGE(tmp, double, (size_t)length);
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(),
					   var_id, start, edges, (void *)tmp)) {
				err("Error reading variable `\\", varname, "' from netCDF file", "\\");
			}
			for (unsigned int ii = 0; ii < (unsigned int) length; ii++)
				_ymatrix[ii] = tmp[ii];
			free(tmp);
		}
		break;
		default:
			err("Cannot handle the type of netCDF variable `\\", varname, "'", "\\");
		}
		delete [] varname;
		_num_ymatrix_data = (int)length;
		// Reverse order of vector, because Gri normally reads
		// from top down
		std::reverse(_ymatrix, _ymatrix + _num_ymatrix_data);
		_ygrid_exists = true;
#else
		demonstrate_command_usage();
		err("Gri internal error: attempting to use nonexistent netCDF library");
#endif
	} else {
		// Figure out how many lines to read; check against existing grid if
		// possible
		switch (_nword) {
		case 3:
			// make initial guess
			number_specified = false;
			_num_ymatrix_data = number_to_read = COLUMN_LEN_DEFAULT;
			break;
		case 4:
			if (!getdnum(_word[3], &tmpf)) {
				demonstrate_command_usage();
				READ_WORD_ERROR(".cols.");
				return false;
			}
			number_to_read = (int) floor(0.5 + fabs((double) tmpf));
			if (number_to_read <= 1) {
				err("Need .cols. > 1");
				return false;
			}
			if (_grid_exists && number_to_read != _num_ymatrix_data) {
				sprintf(_errorMsg, "\
Grid height %d disagrees with existing y-grid (%d); first `delete grid'",
					number_to_read, _num_ymatrix_data);
				err(_errorMsg);
				return false;
			} else {
				_num_ymatrix_data = number_to_read;
			}
			number_specified = true;
			break;
		default:
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		// Get storage
		if (!allocate_ymatrix_storage(number_to_read))
			gr_Error("ran out of storage");
		// Read data
		i = 0;
		while (end_of_data == no_eof && (!number_specified || i < number_to_read)) {
			// Keep an eye on storage space.
			if (i >= _num_ymatrix_data) {
				_num_ymatrix_data += COLUMN_LEN_DEFAULT;
				_ymatrix = (double *)
					realloc((char *) _ymatrix,
						(unsigned) _num_ymatrix_data * sizeof(double));
				if (!_ymatrix)
					gr_Error("ran out of storage");
			}
			char prompt[20];
			sprintf(prompt, "row %3d: ", i);
			end_of_data = get_next_data_line(prompt, 1);
#if 0
			if (end_of_data == eof_after_data) {
				warning("Got EOF on end of data line; should have a newline there");
			}
#endif
#ifdef REMOVE_COMMENTS_FROM_DATA
			remove_comment(inLine.getValue());
#endif
			unsigned int numCols_ui;
			chop_into_data_words(inLine.getValue(), _word, &numCols_ui, MAX_nword);
			PUT_VAR("..words_in_dataline..", double(numCols_ui));
			if (numCols_ui < 1) {	// blank line means done
				end_of_data = eof_before_data; // trick
				break;
			}
			if (!getdnum(_word[0], _ymatrix + i))
				continue;
			i++;
		}
		if (i <= 0) {
			err("`read grid y' found no data");
			return false;
		}
		_num_ymatrix_data = i;
	}
	// Now flip them, since reading in from top to bottom.
	n = _num_ymatrix_data / 2;
	for (i = 0; i < n; i++) {
		double          tmpd = _ymatrix[i];
		_ymatrix[i] = _ymatrix[_num_ymatrix_data - i - 1];
		_ymatrix[_num_ymatrix_data - i - 1] = tmpd;
	}
	// All done reading.
	if (!_yscale_exists)
		if (!create_y_scale()) {
			warning("Problem autoscaling -- no data or all y values equal");
		}
	_ygrid_exists = true;
	if (_ymatrix[1] > _ymatrix[0])
		_ygrid_increasing = true;
	else
		_ygrid_increasing = false;
	if ((repeat = vector_repeats(_ymatrix, _num_ymatrix_data))) {
		sprintf(_errorMsg, "\
y-grid has some adjacent values equal (e.g., value %f)",
			repeat);
		warning(_errorMsg);
	}
	sprintf(_grTempString, "%d rows\n", _num_ymatrix_data);
	RETURN_VALUE(_grTempString);
	return true;
}

bool
read_grid_binary(bool bycolumns, char bintype)
{
	chars_read = 0;
	if (bycolumns) {
		err("`read grid data' cannot use `bycolumn' with binary data.");
		return false;
	}
	if (_nword == 5) {	// gave .cols. and .rows.
		double tmp;
		if (!getdnum(_word[3], &tmp))
			return false;
		if (tmp <= 0) {
			NO_NEGATIVE_ERROR("rows");
			return false;
		}
		_num_ymatrix_data = (unsigned int)floor(0.5 + tmp);
		if (!getdnum(_word[4], &tmp))
			return false;
		if (tmp <= 0) {
			NO_NEGATIVE_ERROR("cols");
			return false;
		}
		_num_xmatrix_data = (unsigned int)floor(0.5 + tmp);
	} else {
		// Ensure that dimensions are known
		if (_num_xmatrix_data == 0) {
			err("Cannot `read grid data' unless grid dimensions are known\n    First specify x grid by `set x grid' or `read grid x',\n    or specify `.cols.' and `.rows.' in present `read grid data' command");
			return false;
		}
		if (_num_ymatrix_data == 0) {
			err("Cannot `read grid data' unless grid dimensions are known\n    First specify x grid by `set y grid' or `read grid y',\n    or specify `.cols.' and `.rows.' in present `read grid data' command");
			return false;
		}
	}
	if (!allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data)) {
		err("Insufficient space for matrix");
		return false;
	}
	unsigned int row = _num_ymatrix_data - 1;
	FILE *the_file = _dataFILE.back().get_fp();
	do {
		double        value_d;	// for data
		float         value_f;
		int           value_i;
		unsigned char value_uc;
		unsigned short int value_16bit;
		for (unsigned int col = 0; col < _num_xmatrix_data; col++) {
			int items;
			double value;
			if (bintype == 'f') {
				items = fread((char *)& value_f, sizeof(value_f), 1, the_file);
				value = value_f;
			} else if (bintype == 'd') {
				items = fread((char *)& value_d, sizeof(value_d), 1, the_file);
				value = value_d;
			} else if (bintype == 'i') {
				items = fread((char *)& value_i, sizeof(value_i), 1, the_file);
				value = value_i;
			} else if (bintype == 'u') {
				items = fread((char *)& value_uc, sizeof(value_uc), 1, the_file);
				value = value_uc;
			} else if (bintype == 'U') {
				items = fread((char *)& value_16bit, sizeof(value_16bit), 1, the_file);
				value = value_16bit;
			} else {
				gr_Error("Internal error in read_grid_binary().  Please report to author\n");
				return false;
			}
			if (items != 1) {
				char msg[100];
				sprintf(msg, "Can't read grid datum at col=%d, row=%d\n", col, row);
				err(msg);
				return false;
			}
			_f_xy(col, row) = value;
			if (value < 0.0)
				printf("_f_xy(col=%d, row=%d)=%f\n",col,row,value);
			_legit_xy(col, row) = gr_missing(value) ? false : true;
		}
		if (_chatty > 1) printf("Read row %d of grid data\n", row);
	} while (row-- != 0);
	return true;		// ok
}

// read_grid_dataCmd() -- read grid data 
bool
read_grid_dataCmd()
{
	chars_read = 0;
	bool            bycolumns = false;
	if (!strcmp(_word[_nword - 1], "bycolumns")) {
		bycolumns = true;
		_nword--;
	}
	if (_dataFILE.back().get_type() == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
		long start[2], edges[2];
		int var_id, grid_dim_ids[10]; // only need 2
		int grid_dims;		// Dimensionality (should be 2)
		long grid_height, grid_width;
		if (_nword > 5) {
			demonstrate_command_usage();
			err("Extra words in command.  Expect e.g `read grid data = \"name\"'");
			return false;
		}
		if (_nword != 5 || !word_is(3, "=") || !quoted(_word[4])) {
			demonstrate_command_usage();
			err("Must specify variable name for a netCDF file");
			return false;
		}
		char *varname = new char [strlen(_word[4])]; // skip first char
		if (!varname) OUT_OF_MEMORY;
		strcpy(varname, 1 + _word[4]);
		varname[strlen(varname) - 1] = '\0';
		var_id = ncvarid(_dataFILE.back().get_netCDF_id(), varname);
		if (var_id == -1) {
			err("Cannot find netCDF variable `\\", varname, "'", "\\");
			return false;
		}
		nc_type type;
		if (-1 == ncvarinq(_dataFILE.back().get_netCDF_id(),
				   var_id, (char *) 0, &type,
				   &grid_dims, grid_dim_ids, (int *) 0)) {
			err("Cannot determine dimensions of `grid data' in netCDF file");
			return false;
		}
		if (grid_dims != 2) {
			err("The dimension of netCDF variable `\\",
			    varname, "' is not 2 as required", "\\");
			return false;
		}
		if (-1 == ncdiminq(_dataFILE.back().get_netCDF_id(), grid_dim_ids[0], (char *)0, &grid_height)) {
			err("Cannot determine height of grid from netCDF file");
			return false;
		}
		if (-1 == ncdiminq(_dataFILE.back().get_netCDF_id(), grid_dim_ids[1], (char *)0, &grid_width)) {
			err("Cannot determine height of grid from netCDF file");
			return false;
		}
		if (_xgrid_exists && _num_xmatrix_data != (unsigned int)grid_width) {
			sprintf(_errorMsg, "\
Grid width %ld disagrees with existing x-grid, which is %d long",
				grid_width, _num_xmatrix_data);
			err(_errorMsg);
			return false;
		}
		if (_ygrid_exists && _num_ymatrix_data != (unsigned int)grid_height) {
			sprintf(_errorMsg, "\
Grid height %ld disagrees with existing y-grid, which is %d high",
				grid_height, _num_ymatrix_data);
			err(_errorMsg);
			return false;
		}
		start[0] = 0;
		start[1] = 0;
		edges[0] = grid_height;
		edges[1] = grid_width;
		if (!allocate_grid_storage(int(grid_width), int(grid_height))) {
			err("Insufficient space for matrix");
			return false;
		}
#if 0
		if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(), var_id, start, edges, (void *) f_xy_tmp)) {
			err("Error reading grid data from variable `\\", varname, "' from netCDF file", "\\");
			return false;
		}
#else
		switch(type) {
		case NC_FLOAT:
		{
			float *f_xy_tmp = (float*)NULL;
			GET_STORAGE(f_xy_tmp, float, (size_t)(grid_width * grid_height));
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(), var_id, start, edges, (void *) f_xy_tmp)) {
				err("Error reading grid data from variable `\\", varname, "' from netCDF file", "\\");
				return false;
			}
			for (int row = grid_height - 1; row > -1; row--) {
				double val;
				for (int col = 0; col < grid_width; col++) {
					val = f_xy_tmp[col + grid_width * row];
					_f_xy(col, row) = val;
					_legit_xy(col, row) = gr_missing(val) ? false : true;
				}
			}
			free(f_xy_tmp);
		}
		break;
		case NC_DOUBLE:
		{
			float *f_xy_tmp = (float*)NULL;
			GET_STORAGE(f_xy_tmp, float, (size_t)(grid_width * grid_height));
			if (-1 == ncvarget(_dataFILE.back().get_netCDF_id(), var_id, start, edges, (void *) f_xy_tmp)) {
				err("Error reading grid data from variable `\\", varname, "' from netCDF file", "\\");
				return false;
			}
			for (int row = grid_height - 1; row > -1; row--) {
				double val;
				for (int col = 0; col < grid_width; col++) {
					val = f_xy_tmp[col + grid_width * row];
					_f_xy(col, row) = val;
					_legit_xy(col, row) = gr_missing(val) ? false : true;
				}
			}
			free(f_xy_tmp);
		}
		break;
		default:
			err("Cannot handle the type of netCDF variable `\\", varname, "'", "\\");
		}
#endif
		delete [] varname;
#else
		gr_Error("Gri internal error: attempting to use nonexistent netCDF library");
		return false;
#endif
	} else if (_dataFILE.back().get_type() == DataFile::bin_unknown) {
                // If not supplied in `open', assume 32 bit float
		if (!read_grid_binary(bycolumns, 'f'))
			return false;
	} else if (_dataFILE.back().get_type() == DataFile::bin_uchar) {
		if (!read_grid_binary(bycolumns, 'u'))
			return false;
	} else if (_dataFILE.back().get_type() == DataFile::bin_16bit) {
		if (!read_grid_binary(bycolumns, 'U'))
			return false;
	} else if (_dataFILE.back().get_type() == DataFile::bin_int) {
		if (!read_grid_binary(bycolumns, 'i'))
			return false;
	} else if (_dataFILE.back().get_type() == DataFile::bin_double) {
		if (!read_grid_binary(bycolumns, 'd'))
			return false;
	} else if (_dataFILE.back().get_type() == DataFile::bin_float) {
		if (!read_grid_binary(bycolumns, 'f'))
			return false;
	} else if (_dataFILE.back().get_type()    == DataFile::ascii
		   || _dataFILE.back().get_type() == DataFile::from_cmdfile) {
		int             cantread = 0;
		unsigned int    startcol = 0, skip_at_end = 0;
		unsigned int row, col, nrow, ncol, nx, ny;
		//
		// Check for 'by columns', a common error
		if (word_is(_nword - 1, "columns")) {
			err("Correct usage is `read grid data ... bycolumns', not `... by columns'");
			return false;
		}
		// Find out how many initial columns to skip, if any
		if (_nword > 3) {
			while (*_word[3 + startcol] == '*') {
				if (startcol + 5 > _nword) {
					demonstrate_command_usage();
					err("Can't understand command.");
					return false;
				}
				startcol++;
			}
			while (*_word[_nword - 1 - skip_at_end] == '*') {
				skip_at_end++;
			}
		}
		// Read .nrow. and .ncol. if supplied; otherwise use 
		// pre-existing or give error.
		if ((startcol + 5) == _nword) {
			// Cmdline has .nrow. and .ncol.
			if (!getdnum(_word[3 + startcol], &tmpf)
			    || !getdnum(_word[4 + startcol], &tmpf2)) {
				return false;
			}
			// Check for crazy numbers
			nrow = (int) floor(0.5 + tmpf);
			if (nrow < 1) {
				err("Need .rows. > 1");
				return false;
			}
			ncol = (int) floor(0.5 + tmpf2);
			if (ncol < 1) {
				err("Need .cols. > 1");
				return false;
			}
			// Check for agreement with existing grid
			if (bycolumns) {
				nx = nrow;
				ny = ncol;
			} else {
				nx = ncol;
				ny = nrow;
			}
			if (_ygrid_exists && _num_ymatrix_data != ny) {
				if (bycolumns) {
					sprintf(_errorMsg, "\
.cols. %d disagrees with existing y-grid (%d); first `delete grid'",
						ny, _num_ymatrix_data);
				} else {
					sprintf(_errorMsg, "\
.rows. %d disagrees with existing y-grid (%d); first `delete grid'",
						ny, _num_ymatrix_data);
				}
				err(_errorMsg);
				return false;
			}
			if (_xgrid_exists && _num_xmatrix_data != nx) {
				if (bycolumns) {
					sprintf(_errorMsg, "\
.rows. %d disagrees with existing x-grid (%d); first `delete grid'",
						nx, _num_xmatrix_data);
				} else {
					sprintf(_errorMsg, "\
.cols. %d disagrees with existing x-grid (%d); first `delete grid'",
						nx, _num_xmatrix_data);
				}
				err(_errorMsg);
				return false;
			}
		} else if ((startcol + 3) == _nword) {
			// Using pre-existing .nrow. and .ncol.
			if (_xgrid_exists && _ygrid_exists) {
				if (bycolumns) {
					ncol = _num_ymatrix_data;
					nrow = _num_xmatrix_data;
					nx = nrow;
					ny = ncol;
				} else {
					ncol = _num_xmatrix_data;
					nrow = _num_ymatrix_data;
					nx = ncol;
					ny = nrow;
				}
			} else {
				demonstrate_command_usage();
				err("Must set/read x-grid and y-grid first");
				return false;
			}
		} else {
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
		// Finally know geometry.  Now allocate storage, then read data.
		if (!allocate_grid_storage(nx, ny)) {
			err("Insufficient space for matrix");
			return false;
		}
		for (row = 0; row < nrow; row++) {
			int             expected_words = ncol + startcol + skip_at_end;
			eof_status        end_of_data = no_eof;	// flag for end of data
			char prompt[20];
			sprintf(prompt, "row %3d: ", row);
			end_of_data = get_next_data_line(prompt, expected_words);
#ifdef REMOVE_COMMENTS_FROM_DATA
			remove_comment(inLine.getValue());
#endif
			chop_into_data_words(inLine.getValue(), _word, &_nword, MAX_nword);
			PUT_VAR("..words_in_dataline..", double(_nword));
			if (end_of_data == eof_before_data || _nword == 0) {
				char msg[100];
				sprintf(msg, "\
`read grid data' encountered early blank-line or end-of-file while\n\
       trying to read line %d of grid data.", row + 1);
				err(msg);
				_grid_exists = true;
				return false;
			}
			if (expected_words != int(_nword)) {
				char msg[1024];
				if (bycolumns) {
					sprintf(msg,
						"Want %d data but line %d has %d numbers.\n",
						_num_ymatrix_data, row + 1, _nword);
				} else {
					sprintf(msg,
						"Want %d data but line %d has %d numbers\n",
						_num_xmatrix_data, row + 1, _nword);
				}
				err(msg);
				_grid_exists = false;
				return false;
			}
			if (bycolumns) {
				for (col = 0; col < ncol; col++) {
					if (getdnum(_word[col + startcol], &tmpf)) {
						_f_xy(row, col) = tmpf;
						_legit_xy(row, col) = gr_missing(tmpf) ? false : true;
					} else {
						cantread++;
						_legit_xy(row, col) = false;
					}
				}
			} else {
				int             j = _num_ymatrix_data - row - 1;
				for (col = 0; col < ncol; col++) {
					if (getdnum(_word[col + startcol], &tmpf)) {
						_f_xy(col, j) = tmpf;
						_legit_xy(col, j) = gr_missing(tmpf) ? false : true;
					} else {
						cantread++;
						_legit_xy(col, j) = false;
					}
				}
			}
		}
		if (_chatty > 0 && cantread) {
			sprintf(_errorMsg, "%d elements couldn't be read", cantread);
			warning(_errorMsg);
		}
	} else {
		gr_Error("Unrecognized file type");
	}
	matrix_limits(&_f_min, &_f_max);	// Figure grid limits
	_grid_exists = true;
	sprintf(_grTempString, "%d rows %d cols\n", _num_ymatrix_data, _num_xmatrix_data);
	RETURN_VALUE(_grTempString);
	return true;
}

bool
read_image_mask_rasterfileCmd()
{
	chars_read = 0;
	if (!_dataFILE.back().get_type()) {
		err("Can only read images from binary files");
		demonstrate_command_usage();
		return false;
	}
	if (!read_raster_image(_dataFILE.back().get_fp(), &_imageMask)) {
		blank_image();
		return false;
	} else {
		return true;
	}
}

bool
read_image_pgmCmd()
{
	chars_read = 0;
	// get scale and box specifications if they exist
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
#if 0 // removed 14/05/96; couldn't have worked anyway
	if (!_dataFILE.back().get_type()) {
		err("Can only read images from binary files");
		demonstrate_command_usage();
		return false;
	}
#endif
	switch (_nword) {
	case 3:
		if (!read_pgm_image(_dataFILE.back().get_fp(), &_image)) {
			blank_imageMask();
			return false;
		} else {
			PUT_VAR("..image_width..",  double(_image.ras_width));
			PUT_VAR("..image_height..", double(_image.ras_height));
			return maybe_make_grids();
		}
		// NOT REACHED
	case 8:
		if (4 == get_cmd_values(_word, _nword, "box", 4, _dstack)) {
			define_image_scales(_dstack[0], _dstack[1], _dstack[2], _dstack[3]);
		} else {
			err("Cannot read `box' parameters");
			demonstrate_command_usage();
			return false;
		}
		if (!read_pgm_image(_dataFILE.back().get_fp(), &_image)) {
			blank_imageMask();
			return false;
		} else {
			PUT_VAR("..image_width..", double(_image.ras_width));
			PUT_VAR("..image_height..", double(_image.ras_height));
			return maybe_make_grids();
		}
		// NOT REACHED
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	// NOT REACHED
}

// Create integral grids if they don't exist
static          bool
maybe_make_grids()
{
	if (_xgrid_exists || _ygrid_exists)
		return true;
	_num_xmatrix_data = _image.ras_width;
	// get storage space
	if (!allocate_xmatrix_storage(_num_xmatrix_data)) {
		err("Insufficient space for grid x data");
		return false;
	}
	// set up x grid
	unsigned int i;
	for (i = 0; i < _num_xmatrix_data; i++)
		_xmatrix[i] = i;
	_xgrid_exists = true;
	_xgrid_increasing = true;
	if (!_ygrid_exists) {
		_num_ymatrix_data = _image.ras_height;
		// get storage space
		if (!allocate_ymatrix_storage(_num_ymatrix_data)) {
			err("Insufficient space for grid y data");
			return false;
		}
		// set up y grid
		for (i = 0; i < _num_ymatrix_data; i++)
			_ymatrix[i] = i;
		_ygrid_exists = true;
		_ygrid_increasing = true;
	}
	define_image_scales(0, 0, _num_xmatrix_data, _num_ymatrix_data);
	return true;
}

bool
read_image_rasterfileCmd()
{
	chars_read = 0;
	// get scale and box specifications if they exist
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
	if (!_dataFILE.back().get_type()) {
		err("Can only read images from binary files");
		demonstrate_command_usage();
		return false;
	}
	switch (_nword) {
	case 3:
		if (!read_raster_image(_dataFILE.back().get_fp(), &_image)) {
			blank_imageMask();
			return false;
		} else {
			PUT_VAR("..image_width..",  double(_image.ras_width));
			PUT_VAR("..image_height..", double(_image.ras_height));
			return maybe_make_grids();
		}
		// NOT REACHED
	case 8:
		if (4 == get_cmd_values(_word, _nword, "box", 4, _dstack))
			define_image_scales(_dstack[0], _dstack[1], _dstack[2], _dstack[3]);
		if (!read_raster_image(_dataFILE.back().get_fp(), &_image)) {
			blank_imageMask();
			return false;
		} else {
			PUT_VAR("..image_width..",  double(_image.ras_width));
			PUT_VAR("..image_height..", double(_image.ras_height));
			return maybe_make_grids();
		}
		// NOT REACHED
	default:
		NUMBER_WORDS_ERROR;
		demonstrate_command_usage();
		return false;
	}
	// NOT REACHED
}

// REF: man 3 pgm.  I'm confused on P4, P5, etc, and I've changed the
// code back and forth, from the looks of it.
//
// 14/5/96: found http://cephag.observ-gv.fr/documentation/man_pbmplus.html
// and am now moving to it.

typedef enum {
	P2_type,	// ascii portable graymap: B values 14/5/96 working
	P3_type,	// ascii portable pixmap: RGB triplets 14/5/96 not working
	P4_type,	// cannot do
	P5_type,	// binary portable graymap: B values 14/5/96 working
	P6_type		// as P3 but binary 14/5/96 not working
} FILE_TYPE;

static          bool
read_pgm_image(FILE * fp, IMAGE * im)
{
	chars_read = 0;
	FILE_TYPE file_type;
	//int             i, j;
	int             width, height, max_gray;
	unsigned char   tmpB;
	char            type[3];
	skip_hash_headers(fp);
	if (1 != fscanf(fp, "%2s", type)) {
		err("Cannot read MAGIC on this pgm file");
		return false;
	}
	skip_hash_headers(fp);
	if (!strcmp(type, "P2"))		
		file_type = P2_type;	// can do
	else if (!strcmp(type, "P3"))
		file_type = P3_type;	// cannot do
	else if (!strcmp(type, "P4"))
		file_type = P4_type;	// cannot do
	else if (!strcmp(type, "P5"))
		file_type = P5_type;	// can do
	else if (!strcmp(type, "P6"))
		file_type = P6_type;	// cannot do
	else  {
		sprintf(_grTempString, "\
This is not a PGM file, since the first 2 characters\n\
       are \"%s\", not \"P2\", \"P3\", \"P4\", \"P5\", \"P6\"as they must be for a PGM file.",
			type);
		err(_grTempString);
		return false;
	}
	if (file_type == P3_type) {
		err("Cannot read ascii portable-pixmap P3-type images");
		return false;
	}
	if (file_type == P4_type) {
		err("Cannot read P4-type images");
		return false;
	}
	if (file_type == P6_type) {
		err("Cannot read binary portable-pixmap P6-type images");
		return false;
	}
	skip_hash_headers(fp);
	if (1 != fscanf(fp, "%d", &width)) {
		err("Cannot read `width' of pgm file");
		return false;
	}
	skip_hash_headers(fp);
	if (1 != fscanf(fp, "%d", &height)) {
		err("Cannot read `height' of pgm file");
		return false;
	}
	skip_hash_headers(fp);
	if (file_type == P2_type || file_type == P5_type) {
		if (1 != fscanf(fp, "%d", &max_gray)) {
			err("Cannot read `maximum-gray-value' of pgm file");
			return false;
		}
	} else {
		max_gray = 255;
	}
	skip_hash_headers(fp);
	if (max_gray != 255) {
		err("Can only do pgm files with maximum-graylevel equal to 255");
		return false;
	}
	im->ras_width = width;
	im->ras_height = height;
	im->ras_depth = 8;
	im->ras_length = width * height;
	im->ras_type = RT_STANDARD;
	im->ras_maptype = RMT_NONE;
	im->ras_maplength = 0;
	if (im->ras_width < 1 || im->ras_height < 1) {
		err("Cannot read image with negative or zero width or height");
		return false;
	}
	GET_STORAGE(im->image, unsigned char, (im->ras_width * im->ras_height));
	if (_dataFILE.back().get_type() != DataFile::from_cmdfile
	    &&_dataFILE.back().get_type() != DataFile::ascii) {
		// It's binary -- check for correct type
		if (file_type != P5_type) {
			err("A binary `pgm' image needs first 2 chars to be \"P5\"");
			return false;
		}
		// Read byte by byte
		//printf("DEBUG: %s:%d image_width=%d  image_height=%d\n",__FILE__,__LINE__,im->ras_width,im->ras_height);
		for (int j = int(im->ras_height - 1); j > -1; j--) {
			for (int i = 0; i < int(im->ras_width); i++) {
				if (1 != fread((char *) & tmpB, sizeof(tmpB), 1, fp)) {
					sprintf(_grTempString, "Could not read all PGM image data.  Ran out after reading %d bytes", (im->ras_height - 1 - j) * im->ras_width + i);
					err(_grTempString);
					return false;
				}
				*(im->image + i * im->ras_height + j) = tmpB;
				//printf("i=%d j=%d ... value %x_hex = %d_dec\n",i,j,tmpB,tmpB);
			}
		}
	} else {
		// It's ascii -- check for correct type
		char            nextWord[50];
		if (file_type == P2_type) {
			for (int j = im->ras_height - 1; j > -1; j--) {
				for (int i = 0; i < int(im->ras_width); i++) {
					// Get 1 ascii datum.
					if (1 != fscanf(fp, "%s", nextWord)) {
						sprintf(_grTempString, "Could not read all PGM image data.  Ran out after reading %d bytes", (im->ras_height - 1 - j) * im->ras_width + i);
						err(_grTempString);
						return false;
					}
					if (!getdnum(nextWord, &tmpf)) {
						tmpB = 0;
					}
					tmpB = (int) floor((double) (0.5 + tmpf));
					*(im->image + i * im->ras_height + j) = tmpB;
				}
			}
		} else if (file_type == P5_type) {
			err("Mixed up -- it's a P5 type file but data are ascii");
			return false;
		} else {
			err("Can only handle P2 and P5 subtypes of the PGM type");
			return false;
		}
	}
	return true;
}

// If the very next character is '#', skip rest of this line
void
skip_hash_headers(FILE * fp)
{
	int             c;
	// Skip initial whitespace (left from previous read)
	do {
		c = getc(fp);
	} while (c == ' ' || c == '\n' || c == '\t');
	if (c == '#') {
		while ('\n' != (c = getc(fp))) {
			;
		}
		// Maybe next line is a comment too
		skip_hash_headers(fp);
	} else {
		ungetc(c, fp);
	}
}

// REF: man 5 rasterfile; /usr/include/rasterfile.h on a sun computer
static bool
read_raster_image(FILE * fp, IMAGE * im)
{
	chars_read = 0;
	int             i, j;
	unsigned char   tmpB;
	unsigned char   b0, b1, b2, b3;

        // The "man rasterfile" page on solaris suggests to check the 
	// first word against "#define RAS_MAGIC 0x59a66a95" but
	// we have to check it byte by byte (0x59 0xa6 0x6a 0x95)
	// because of endian issues.  An ugly issue.

	if (1 != fread((char*)&b0, sizeof(unsigned char), 1, fp)) {
		err("Cannot read the first 'magic' byte at start of image");
		return false;
	}
	if (1 != fread((char*)&b1, sizeof(unsigned char), 1, fp)) {
		err("Cannot read the second 'magic' byte at start of image");
		return false;
	}
	if (1 != fread((char*)&b2, sizeof(unsigned char), 1, fp)) {
		err("Cannot read the third 'magic' byte at start of image");
		return false;
	}
	if (1 != fread((char*)&b3, sizeof(unsigned char), 1, fp)) {
		err("Cannot read the fourth 'magic' byte at start of image");
		return false;
	}
	// Sun is big endian; intel is little endian.
	bool big_endian = false;
	if (b0 == 0x59 && b1 == 0xa6 && b2 == 0x6a && b3 == 0x95) 
		big_endian = true;
	else if (b3 == 0x59 && b2 == 0xa6 && b1 == 0x6a && b0 == 0x95)
		big_endian = false;
	else {
		sprintf(_grTempString, "This is not a Sun rasterfile, since first bytes are %x,%x,%x,%x instead of the expected %x,%x,%x,%x or %x,%x,%x,%x\n", b0, b1, b2, b3, 0x59, 0xa6, 0x6a, 0x95, 0x95, 0x6a, 0xa6, 0x59); 
		err(_grTempString);
		
	}

	bool switch_bytes = false;
#if defined(__GNUC__) // BUG: should do this endian work a lot more cleanly
	if ((!GRI_IS_BIG_ENDIAN && big_endian) || (GRI_IS_BIG_ENDIAN && ! big_endian)) {
		switch_bytes = true;
#if defined(DEBUG_READ)
		printf("DEBUG: %s:%d must switch bytes\n",__FILE__,__LINE__);
#endif
	}
#endif
#if defined(DEBUG_READ)
	printf("DEBUG. %s:%d endian status = %s\n",__FILE__,__LINE__,big_endian? "BIG":"LITTLE");
#endif
	if (1 != fread((char *) & im->ras_width, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image width");
		return false;
	}
#if defined(DEBUG_READ)
	printf("DEBUG: %s:%d switch raster width from %d to ", __FILE__,__LINE__,im->ras_width);
#endif
	if (switch_bytes)
		im->ras_width = endian_swap_uint(im->ras_width);
#if defined(DEBUG_READ)
	printf("%d\n", im->ras_width);
#endif
	bool need_zero_padding = (im->ras_width == 2 * (im->ras_width / 2)) ? false : true;

	if (1 != fread((char *) & im->ras_height, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image height");
		return false;
	}
	if (switch_bytes)
		im->ras_height = endian_swap_uint(im->ras_height);
	if (1 != fread((char *) & im->ras_depth, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image depth");
		return false;
	}
	if (switch_bytes)
		im->ras_depth = endian_swap_uint(im->ras_depth);
	if (1 != fread((char *) & im->ras_length, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image length");
		return false;
	}
	if (switch_bytes)
		im->ras_length = endian_swap_uint(im->ras_length);
	if (1 != fread((char *) & im->ras_type, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image type");
		return false;
	}
	if (switch_bytes)
		im->ras_type = endian_swap_uint(im->ras_type);
	if (1 != fread((char *) & im->ras_maptype, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image maptype");
		return false;
	}
	if (switch_bytes)
		im->ras_maptype = endian_swap_uint(im->ras_maptype);
	if (1 != fread((char *) & im->ras_maplength, sizeof(unsigned int), 1, fp)) {
		err("Cannot read image maplength");
		return false;
	}
	if (switch_bytes)
		im->ras_maplength = endian_swap_uint(im->ras_maplength);
	if (need_zero_padding)
	if (im->ras_length 
	    != (need_zero_padding ? 1 + im->ras_width : im->ras_width)
	    * im->ras_height) {
		sprintf(_grTempString, "Cannot read compressed images.  This seems to be compressed, since it's %d wide and %d tall, but the length is %d\n", im->ras_width, im->ras_height, im->ras_length);
		err(_grTempString);
		return false;
	}
	if (im->ras_depth != 8) {
		err("Can only read 8 bit images");
		return false;
	}
	if (im->ras_type != RT_STANDARD) {
		sprintf(_grTempString, "Can only read images of type RT_STANDARD (%d) but this is type %d\n", RT_STANDARD, im->ras_type);
		err(_grTempString);
		return false;
	}
	// Skip colormap if there is one BUG: should use it
	switch (im->ras_maptype) {
	case RMT_NONE:
		break;
	case RMT_RAW:
		warning("Skipping image map of type RMT_RAW.");
		for (i = 0; i < int(im->ras_maplength); i++)
			fread((char *) & tmpB, sizeof(tmpB), 1, fp);
		break;
	case RMT_EQUAL_RGB:	// raw
		warning("Skipping image map of type RMT_RGB.");
		for (i = 0; i < int(im->ras_maplength); i++)
			fread((char *) & tmpB, sizeof(tmpB), 1, fp);
		break;
	default:
		err("Can only read images of maptype RMT_NONE or RMT_RAW");
		return false;
	}
	if (im->ras_width < 1 || im->ras_height < 1) {
		err("Cannot read image with negative or zero width or height");
		return false;
	}
	GET_STORAGE(im->image, unsigned char, (im->ras_width * im->ras_height));
	for (j = int(im->ras_height - 1); j > -1; j--) {
		for (i = 0; i < int(im->ras_width); i++) {
			if (1 != fread((char *) & tmpB, sizeof(tmpB), 1, fp)) {
				sprintf(_grTempString, "Could not read all RasterFile image data.  Ran out after reading %d bytes", (im->ras_height - 1 - j) * im->ras_width + i);
				err(_grTempString);
				return false;
			}
			//printf("i=%d j=%d ... value %x_hex = %d_dec\n",i,j,tmpB,tmpB);
			*(im->image + i * im->ras_height + j) = tmpB;
		}
		if (need_zero_padding) {
			fread((char *) & tmpB, sizeof(tmpB), 1, fp);
		}
	}
	return true;
}

// read_imageCmd() -- read image data
bool
read_imageCmd()
{
	chars_read = 0;
	bool            bycolumns = false;
	int             i, j, nrow, ncol;
	unsigned int nx, ny;
	unsigned char   tmpB;
	char            nextWord[50];
	if (!image_range_exists()) {
		err("First `set image range'");
		return false;
	}
	if (!_dataFILE.back().get_type()) {
		err("Can only read images from binary files");
		demonstrate_command_usage();
		return false;
	}
	// Figure out if reading by columns (by rows is default).
	if (!strcmp(_word[_nword - 1], "bycolumns")) {
		bycolumns = true;
		_nword--;
	}
	// get box specifications if it exists
	if (4 == get_cmd_values(_word, _nword, "box", 4, _dstack))
		define_image_scales(_dstack[0], _dstack[1], _dstack[2], _dstack[3]);
	if (2 == get_cmd_values(_word, _nword, "image", 2, _dstack)) {
		nrow = (int) floor(0.5 + _dstack[0]);
		ncol = (int) floor(0.5 + _dstack[1]);
	} else {
		demonstrate_command_usage();
		err("Can't understand command.");
		return false;
	}
	if (nrow < 0 || ncol < 0) {
		NO_NEGATIVE_ERROR(".rows., .cols.");
		return false;
	}
	if (bycolumns) {
		nx = (unsigned int)nrow;
		ny = (unsigned int)ncol;
	} else {
		nx = (unsigned int)ncol;
		ny = (unsigned int)nrow;
	}
	// Check for agreement with existing mask grid.
	if (imageMask_exists()
	    && (_imageMask.ras_width != nx || _imageMask.ras_height != ny)) {
		sprintf(_errorMsg, "%dx%d grid contradicts existing %dx%d mask grid\n",
			nx, ny, _imageMask.ras_width, _imageMask.ras_height);
		err(_errorMsg);
		return false;
	}
	if (image_exists()
	    && (_image.ras_width != nx || _image.ras_height != ny)) {
		sprintf(_errorMsg, "%dx%d grid contradicts existing %dx%d grid\n",
			nx, ny, _imageMask.ras_width, _imageMask.ras_height);
		err(_errorMsg);
		return false;
	}
	// Allocate storage for image and mask
	if (!allocate_image_storage(nx, ny)) {
		err("No space for image");
		return false;
	}
	if (!allocate_imageMask_storage(nx, ny)) {
		err("No space for image mask");
		return false;
	}
	// Don't allow reading from cmd-file right now.
	if (_dataFILE.back().get_type() == DataFile::from_cmdfile) {
		err("Can't read images from command-file");
		if (_cmdFILE.back().get_interactive())
			return false;
	}
	// Read in the data, number by number.
	char emessage[200];
	FILE *imfile = _dataFILE.back().get_fp();
	DataFile::type imtype = _dataFILE.back().get_type();
	for (j = int(ny - 1); j > -1; j--) {
		for (i = 0; i < int(nx); i++) {
			tmpB = 0;
			if (imtype == DataFile::bin_unknown || imtype == DataFile::bin_uchar) {
				// Get 1 binary datum.
				if (1 != fread((char *) & tmpB, 1, 1, imfile)) {
					sprintf(emessage, "Cannot read whole %dx%d 8-bit image.  Only read %d bytes of the %d bytes expected in image", nx, ny, (i + j * nx), (nx * ny));
					err(emessage);
					return false;
				}
			} else if (imtype == DataFile::bin_16bit) {
				unsigned short int v;
				if (1 != fread((char *) & v, sizeof(v), 1, imfile)) {
					sprintf(emessage, "Cannot read whole %dx%d 16-bit image.\n       Only read %d bytes of the %d bytes expected in image", nx, ny, 2*(i + j * nx), 2*(nx * ny));
					err(emessage);
					return false;
				} else {
					tmpB = (unsigned char)v;
				}
			} else if (imtype == DataFile::bin_int) {
				unsigned int v;
				if (1 != fread((char *) & v, sizeof(v), 1, imfile)) {
					sprintf(emessage, "Cannot read whole %dx%d 32-bit image.\n       Only read %d bytes of the %d bytes expected in image", nx, ny, 4*(i + j * nx), 4*(nx * ny));
					err(emessage);
					return false;
				} else {
					tmpB = (unsigned char)v;
				}
			} else if (imtype == DataFile::bin_float) {
				float v;
				if (1 != fread((char *) & v, sizeof(v), 1, imfile)) {
					sprintf(emessage, "Cannot read whole %dx%d 32-bit image.\n       Only read %d bytes of the %d bytes expected in image", nx, ny, 4*(i + j * nx), 4*(nx * ny));
					err(emessage);
					return false;
				} else {
					tmpB = (unsigned char)v;
				}
			} else if (imtype == DataFile::bin_double) {
				double v;
				if (1 != fread((char *) & v, sizeof(v), 1, imfile)) {
					sprintf(emessage, "Cannot read whole %dx%d 64-bit image.\n       Only read %d bytes of the %d bytes expected in image", nx, ny, 8*(i + j * nx), 8*(nx * ny));
					err(emessage);
					return false;
				} else {
					tmpB = (unsigned char)v;
				}
			} else if (imtype == DataFile::ascii) {
				// Get 1 ascii datum.
				if (1 != fscanf(imfile, "%s", nextWord))  {
					sprintf(emessage, "Cannot read whole %dx%d ascii image.\n       Only read %d bytes of the %d values expected in image", nx, ny, (i + j * nx), (nx * ny));
					err(emessage);
					return false;
				}
				if (!getdnum(nextWord, &tmpf)) {
					sprintf(emessage, "Cannot read whole %dx%d ascii image.\n       Only read %d bytes of the %d values expected in image", nx, ny, (i + j * nx), (nx * ny));
					err(emessage);
					return false;
				}
				tmpB = (int) floor((double) (0.5 + tmpf));
				// Ignore comments.
				if (!strcmp(nextWord, "//"))
					while (fgetc(imfile) != '\n')
						;
			} else {
				err("Internal error -- unknown image type.");
				return false;
			}
			// Now have 1 datum.  Store it into image.
			if (bycolumns) {
				*(_image.image + j * nx + i) = tmpB;
			} else {
				*(_image.image + i * ny + j) = tmpB;
			}
		}
	}
	return true;
}

// read_image_colorscaleCmd() -- read image colorscale as 256 lines of
// (R,G,B), clipped to range 0,1 `read image colorscale' `read image
// colorscale rgb' `read image colorscale hsb'
bool
read_image_colorscaleCmd()
{
	chars_read = 0;
	double          R, G, B;
	double          H, S, V;
	bool            using_rgb = true;
	if (_nword == 3) {
		using_rgb = true;
	} else if (_nword == 4) {
		if (word_is(3, "rgb")) {
			using_rgb = true;
		} else if (word_is(3, "hsb")) {
			using_rgb = false;
		} else {
			demonstrate_command_usage();
			err("Fourth word must be `rgb' or `hsb'");
			return false;
		}
	} else {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	// Table must have 256 lines
	for (int i = 0; i < 256; i++) {
		if (get_next_data_line("Image colorscale", 3)) {
			err("Can't read image grayscale; found EOF on line.");
			return false;
		}
#ifdef REMOVE_COMMENTS_FROM_DATA
		remove_comment(inLine.getValue());
#endif
		unsigned int nword;
		chop_into_data_words(inLine.getValue(), _Words2, &nword, MAX_nword);
		if (using_rgb) {
			getdnum(_Words2[0], &R);
			getdnum(_Words2[1], &G);
			getdnum(_Words2[2], &B);
		} else {
			getdnum(_Words2[0], &H);
			getdnum(_Words2[1], &S);
			getdnum(_Words2[2], &V);
			H = pin0_1(H);
			S = pin0_1(S);
			B = pin0_1(B);
			gr_hsv2rgb(H, S, V, &R, &G, &B);
		}
		_imageTransform[i] = (int) (floor(0.5 + pin0_255(255.0 * R)));
		_imageTransform[i + 256] = (int) (floor(0.5 + pin0_255(255.0 * G)));
		_imageTransform[i + 512] = (int) (floor(0.5 + pin0_255(255.0 * B)));
	}
	_imageTransform_exists = true;
	_image_color_model = rgb_model;
	return true;
}

// read_image_grayscaleCmd() -- read image grayscale (clipped to range 0-1)
bool
read_image_grayscaleCmd()
{
	chars_read = 0;
	// Get data into inLine.
	if (get_next_data_line("Image grayscale", 256)) {
		err("Can't read image grayscale; found EOF on line.");
		return false;
	}
#ifdef REMOVE_COMMENTS_FROM_DATA
	remove_comment(inLine.getValue());
#endif
	unsigned int nword;
	chop_into_data_words(inLine.getValue(), _Words2, &nword, MAX_nword);
	PUT_VAR("..words_in_dataline..", double(nword));
	if (nword == 256) {
		// Table all on 1 line (required before version 1.049)
		for (int i = 0; i < 256; i++) {
			getdnum(_Words2[i], &tmpf);
			_imageTransform[i] = (int) (floor(0.5 + pin0_255(255.0 * tmpf)));
		}
	} else {
		// Table has 256 lines
		getdnum(_Words2[0], &tmpf);
		_imageTransform[0] = (int) (floor(0.5 + pin0_255(255.0 * tmpf)));
		for (int i = 1; i < 256; i++) {
			if (get_next_data_line("Image grayscale", 1)) {
				err("Can't read image grayscale; found EOF on line.");
				return false;
			}
#ifdef REMOVE_COMMENTS_FROM_DATA
			remove_comment(inLine.getValue());
#endif
			chop_into_data_words(inLine.getValue(), _Words2, &nword, MAX_nword);
			PUT_VAR("..words_in_dataline..", double(nword));
			getdnum(_Words2[0], &tmpf);
			_imageTransform[i] = (int) (floor(0.5 + pin0_255(255.0 * tmpf)));
		}
	}
	_imageTransform_exists = true;
	return true;
}

// read_image_maskCmd() -- read image mask
bool
read_image_maskCmd()
{
	chars_read = 0;
	bool            bycolumns = false, outbounds = false;
	int             cantread = 0;
	int             i, j, nrow, ncol;
	unsigned int nx, ny;
	unsigned char   tmpB;
	char            nextWord[50];
	// Figure out if reading by columns (by rows is default).
	if (!strcmp(_word[_nword - 1], "bycolumns")) {
		bycolumns = true;
		_nword--;
	}
	// Do initial check for bad command.
	if (_nword != 5) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	if (!getdnum(_word[3], &tmpf)) {
		demonstrate_command_usage();
		err("Can't read .rows. PROPER: `read image mask .rows. .cols.'");
		return false;
	}
	nrow = (int) floor(0.5 + tmpf);
	if (!getdnum(_word[4], &tmpf)) {
		demonstrate_command_usage();
		err("Can't read .cols. PROPER: `read image mask .rows. .cols.'");
		return false;
	}
	ncol = (int) floor(0.5 + tmpf);
	if (nrow < 0 || ncol < 0) {
		NO_NEGATIVE_ERROR(".rows., .cols.");
		return false;
	}
	if (bycolumns) {
		nx = (unsigned int)nrow;
		ny = (unsigned int)ncol;
	} else {
		nx = (unsigned int)ncol;
		ny = (unsigned int)nrow;
	}
	// Check for agreement with existing grid.
	if (imageMask_exists()
	    && (_imageMask.ras_width != nx || _imageMask.ras_height != ny)) {
		sprintf(_errorMsg, "%dx%d grid contradicts existing %dx%d mask grid\n",
			nx, ny, _imageMask.ras_width, _imageMask.ras_height);
		err(_errorMsg);
		return false;
	}
	if (image_exists()
	    && (_image.ras_width != nx || _image.ras_height != ny)) {
		sprintf(_errorMsg, "%dx%d grid contradicts existing %dx%d grid\n",
			nx, ny, _imageMask.ras_width, _imageMask.ras_height);
		err(_errorMsg);
		return false;
	}
	// Allocate storage.
	if (!allocate_imageMask_storage(nx, ny)) {
		err("No space for image mask");
		return false;
	}
	// Don't allow reading from cmd-file right now.
	if (_dataFILE.back().get_type() == DataFile::from_cmdfile) {
		demonstrate_command_usage();
		err("Can't read images from command-file");
		if (!_cmdFILE.back().get_interactive())
			return false;
	}
	// Read in the data, number by number.
	for (j = int(ny - 1); j > -1; j--) {
		for (i = 0; i < int(nx); i++) {
			if (_dataFILE.back().get_type()) {
				// Get 1 binary datum.
				if (1 != fread((char *) & tmpB, 1, 1, _dataFILE.back().get_fp())) {
					cantread++;
					tmpB = 0;
				}
			} else {
				// Get 1 ascii datum.
				if (1 != fscanf(_dataFILE.back().get_fp(), "%s", nextWord)) {
					err("Ran out of data");
					return false;
				}
				if (!getdnum(nextWord, &tmpf)) {
					cantread++;
					tmpB = 0;
				}
				tmpB = (int) floor((double) (0.5 + tmpf));
				// Ignore comments.
				if (!strcmp(nextWord, "//"))
					while (fgetc(_dataFILE.back().get_fp()) != '\n');
			}
			// Now have 1 datum.  Store it into imageMask or into image.
			if (bycolumns) {
				*(_imageMask.image + j * nx + i) = tmpB;
			} else {
				*(_imageMask.image + i * ny + j) = tmpB;
			}
		}
	}
	if (cantread) {
		sprintf(_errorMsg, "%d elements unreadable", cantread);
		err(_errorMsg);
		return false;
	}
	if (outbounds) {
		sprintf(_errorMsg, "%d elements <0 or >255", outbounds);
		err(_errorMsg);
		return false;
	}
	return true;
}

bool
read_synonym_or_variableCmd()
{
	chars_read = 0;
	if (_nword < 2) {
		demonstrate_command_usage();
		err("`read' what? (Need more words on command line.)");
		return false;
	}
	unsigned int start = 0;
#ifdef REMOVE_COMMENTS_FROM_DATA
	bool read_raw_flag = false;
	if (strEQ(_word[1], "raw")) {
		read_raw_flag = true;
		start = 1;
	}
#endif
	if (_dataFILE.back().get_type() == DataFile::bin_netcdf) {
#if defined(HAVE_LIBNETCDF)
		// For netCDF files, only allow `read \name' (with one synonym)
		int len = strlen(_word[1 + start]) - 1;// NB: skipping first char
		char *varname = new char [1 + len];
		if (!varname) OUT_OF_MEMORY;
		char *varname_base;
		varname_base = varname;	// for later deletion
		strcpy(varname, _word[1 + start] + 1);
#if 1 // 2.060
		//
		// Remove '{' and '}' if present
		if (varname[0] == '{') {
			varname++;
			len--;
		}
		if (varname[len - 1] == '}') {
			varname[len - 1] = '\0';
			len--;
		}
#endif
		bool has_colon = false;
		int i, where_colon = -1;
		for (i = 0; i < len; i++) {
			if (varname[i] == ':') {
				has_colon = true;
				where_colon = i;
			}
		}
		struct ncatt {
			int var;
			char name[MAX_NC_NAME];
			nc_type type;
			int len;
			void *val;
		};
		struct ncatt att;
		int varid;
		if (has_colon && where_colon != 0) {
			// Attribute of a variable
			varname[where_colon] = '\0';
			varid = ncvarid(_dataFILE.back().get_netCDF_id(), varname);
			if (varid == -1) {
				err("No netCDF variable `\\", varname, "' exists", "\\");
				delete [] varname_base;
				return false;
			}
			varname += where_colon + 1;
		} else {
			varid = NC_GLOBAL;
			if (where_colon == 0)
				varname++;
		}
		if (-1 == ncattinq(_dataFILE.back().get_netCDF_id(),
				   varid,
				   varname,
				   &att.type,
				   &att.len)) {
			err("No netCDF attribute `\\", varname, "' exists", "\\");
			return false;
		}
		att.val = (void *) malloc((unsigned)att.len*nctypelen(att.type));
		Require(att.val, OUT_OF_MEMORY);
		if (-1 == ncattget(_dataFILE.back().get_netCDF_id(),
				   varid,
				   varname,
				   att.val)) {
			err("Cannot get netCDF attribute `\\", varname, "'", "\\");
			return false;
		}
		switch(att.type) {
		case NC_FLOAT:
		{
			float *tf = (float *) att.val;
			sprintf(_grTempString, "%f", *tf);
		}
		break;
		case NC_DOUBLE:
		{
			double *td = (double *) att.val;
			sprintf(_grTempString, "%f", *td);
		}
		break;
		case NC_LONG:
		{
			long *tl = (long *) att.val;
			sprintf(_grTempString, "%ld", *tl);
		}
		break;
		case NC_CHAR:
		{
			char *tc = (char *) att.val;
			int i;
			for (i = 0; i < att.len; i++)
				_grTempString[i] = *(tc + i);
			_grTempString[i] = '\0';
		}
		break;
		case NC_SHORT:
		{
			short *ts = (short *) att.val;
			sprintf(_grTempString, "%d", *ts);
		}
		break;
		default:
			warning("Unknown attribute type for `\\", varname, "'", "\\");
			strcpy(_grTempString, "");
		}
		if (!put_syn(_word[1 + start], _grTempString, true)) {
			err("Synonym stack exhausted");
			delete [] varname_base;
			return false;
		}
		delete [] varname_base;
		return true;
#else
		err("Not compiled with netCDF library");
		return false;
#endif
	} else if (_dataFILE.back().get_type() == DataFile::bin_uchar) {
		gr_Error("Cannot read unsigned char grid data yet");
	} else if (_dataFILE.back().get_type() == DataFile::bin_16bit) {
		gr_Error("Cannot read 16-bit grid data yet");
	} else if (_dataFILE.back().get_type() == DataFile::bin_int) {
		gr_Error("Cannot read int grid data yet");
	} else if (_dataFILE.back().get_type() == DataFile::bin_float) {
		for (unsigned int w = 1 + start; w < _nword; w++) {
			std::string the_word(_word[w]);
			un_double_quote(the_word);
			un_double_slash(the_word);
			de_reference(the_word);
			if (the_word == "*") {
				if (is_var(the_word)) {
					float tmp;
					if (1 == fread((char *)&tmp, sizeof(float), 1, _dataFILE.back().get_fp())) {
						PUT_VAR(the_word.c_str(), tmp);
					} else {
						err("Can't read \\", _word[w], "'", "\\");
					}
				} else if (is_syn(the_word)) {
					err("Cannot read synonyms from binary file");
					return false;
				} else {
					// Probably impossible to get here anyway.
					err("`read' what? (Item `\\",
					    _word[w],
					    "' is neither a synonym-name nor a variable-name.)",
					    "\\");
					return false;
				}
			}
		}
	} else if (_dataFILE.back().get_type() == DataFile::bin_double) {
		gr_Error("Cannot read double grid data yet");
	} else { 
		// Ascii file [in read_synonym_or_variableCmd()]
		for (unsigned int w = 1 + start; w < _nword; w++) {
			if (true == get_next_data_word()) {
				PUT_VAR("..words_in_dataline..", 0.0);
				if (_ignore_eof) {
					warning("`read synonym|variable' hit end-of-file");
					return false;
				} else {
					//printf("DEBUG 1\n");
					return true;
				}
			}
#ifdef REMOVE_COMMENTS_FROM_DATA
			if (!read_raw_flag)
				if (remove_comment(inLine.getValue()))
					return true;
#endif
			std::string the_word(_word[w]);
			un_double_quote(the_word);
			un_double_slash(the_word);
			de_reference(the_word);
			if (the_word == "*") {
				continue;
			} else if (is_var(the_word)) {
				if (0 < strlen(inLine.getValue())) {
					double tmp;
					if (getdnum(inLine.getValue(), &tmp)) {
						PUT_VAR(the_word.c_str(), tmp);
					} else {
						err("Can't read \\", _word[w], " in `", inLine.getValue(), "'", "\\");
					}
				} else {
					PUT_VAR(the_word.c_str(), gr_currentmissingvalue());
				} 
			} else if (is_syn(the_word)) {
				char *copy = new char [1 + strlen(inLine.getValue())];
				if (!copy) OUT_OF_MEMORY;
				strcpy(copy, inLine.getValue());
				if (copy[strlen(copy) - 1] == '\n')
					copy[strlen(copy) - 1] = '\0';
				if (!put_syn(the_word.c_str(), copy, true)) {
					delete [] copy;
					err("Synonym stack exhausted");
					return false;
				}
				delete [] copy;
			} else {
				err("`read' what? (Item `\\",
				    _word[w],
				    "' is neither a synonym-name nor a variable-name.)",
				    "\\");
				demonstrate_command_usage();
				return false;
			}
		}
	}
	// flush to end of line, skipping comment if any
	int c = 0;
	while (c != '\n' && !feof(_dataFILE.back().get_fp()))
		c = getc(_dataFILE.back().get_fp());
	return true;
}

// `read line [raw] \syn'
bool
read_lineCmd()
{
	chars_read = 0;
	Require (_nword > 2, err("`read line' what?"));
	int start = 0;
#ifdef REMOVE_COMMENTS_FROM_DATA
	bool read_raw_flag = false;
	if (strEQ(_word[2], "raw")) {
		read_raw_flag = true;
		start = 1;
	}
#endif
	Require(is_syn(_word[2 + start]), err("`read line' what?"));
	
	eof_status        end_of_data = no_eof;	// flag for end of data
	end_of_data = get_next_data_line("", 1);
	//printf("\n  %s:%d (read_lineCmd) <%s>  end_of_data= %d\n",__FILE__,__LINE__,inLine.getValue(),end_of_data);
	if (end_of_data == eof_before_data) {
		if (_ignore_eof) {
			warning("`read line' hit end-of-file");
			return false;
		} else {
			set_eof_flag_on_data_file();
			return false;
		}
	} else {
		char *s = inLine.getValue();
#ifdef REMOVE_COMMENTS_FROM_DATA
		if (!read_raw_flag) {
			remove_comment(s);
		} else {
			// remove trailing newlines
			int len = strlen(s);
			while (len > 1 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
				s[--len] = '\0';
		}
#else
		// remove trailing newlines
		int len = strlen(s);
		while (len > 1 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
			s[--len] = '\0';
#endif
		if (!put_syn(_word[2 + start], s, true)) {
			err("Synonym stack exhausted");
			return false;
		}
	}
	return true;
}

// get next data line, giving the indicated prompt
// if interactive, and store the result in the global string inLine.
// 
// If expected_fields > 0 then, for binary files, that number of items will be
// read and put into inLine; for ascii files, the parameter will be ignored.
// 
// RETURN VALUE: 0 if didn't hit EOF
//               1 if hit EOF before anything else
//               2 if hit EOF after some material on the line 
// Thus, an return value of 2 means that the data line was not terminated
// with newline, but that data were found nonetheless.
static eof_status
get_next_data_line(const char *prompt, unsigned int expected_fields)
{
	//printf("DEBUG %s:%d (get_next_data_line) interactive=%d (from_cmdfile = %d)\n",__FILE__,__LINE__,int(_dataFILE.back().get_type()),DataFile::from_cmdfile);

 	eof_status got_eof = no_eof;
	// Get line from either commandfile, data-file, or new-command.
	extern bool     _store_cmds_in_ps;	// startup.c
	if (_dataFILE.back().get_type() == DataFile::from_cmdfile) {
		// Get line from cmd-file.
		if (_cmdFILE.back().get_interactive()) {
			// Interactive case:  write prompt and then get line.
			if (!batch())
				gr_textput(prompt);
			gr_textget(inLine.getValue(), inLine.getCapacity() - 1);
			_cmdFILE.back().increment_line();
		} else {
#if 1				// TRYING to get 'read columns' to work in blocks
			extern std::vector<BlockSource> bsStack;
			//printf("DEBUG %s:%d non-interactive case.  bsStack.size %d ***\n",__FILE__,__LINE__,bsStack.size());
			// If not in block, use file; if in block, read from it.
			if (bsStack.size() == 0) {
				got_eof = inLine.line_from_FILE(_cmdFILE.back().get_fp());
				if (got_eof != no_eof) {
					set_eof_flag_on_data_file();
					return got_eof;
				}
				_cmdFILE.back().increment_line();
				// Take care of special case of data files created by VAX
				// FORTRAN; they have a spurious newline at the start.
				if (_dataFILE.back().get_line() == 1
				    && *(inLine.getValue()) == '\n'
				    && ignore_initial_newline()) {
					warning("Skipping initial empty line in data file");
					got_eof = inLine.line_from_FILE(_cmdFILE.back().get_fp());
					if (got_eof != no_eof) {
						set_eof_flag_on_data_file();
						return got_eof;
					}
				}
			} else {
				extern bool get_line_in_block(const char *block, unsigned int *offset);
				unsigned int offset = offset_for_read + chars_read;
				//printf("READ has offset= %d  for block\n||%s||\n",offset,bsStack.back().get_start()+offset);
				if (!get_line_in_block(bsStack.back().get_start(), &offset)) {
					set_eof_flag_on_data_file();
					return got_eof;
				}
				bsStack.back().move_offset(strlen(_cmdLine) + 1);
				//printf("DEBUG %s:%d think line is [%s]\n",__FILE__,__LINE__,_cmdLine);
				inLine.fromSTR(_cmdLine);
				chars_read += strlen(_cmdLine) + 1;
				//printf("DEBUG %s:%d inLine is [%s]; chars_read now %d since adding %d\n",__FILE__,__LINE__,inLine.getValue(),chars_read,strlen(_cmdLine) + 1);
				return no_eof;
			}
#endif
		}
		// Display the line as a comment in PostScript file.
		if (_store_cmds_in_ps) {
			strcpy(_grTempString, "gri:");
			gr_comment(strcat(_grTempString, inLine.getValue()));
		}
	} else {
		// Get line from data-file.
		if (_dataFILE.back().get_type() == DataFile::ascii) { // ASCII file, non interactive
			got_eof = inLine.line_from_FILE(_dataFILE.back().get_fp());
			//printf("get_next_data_line %s:%d got_eof=%d\n",__FILE__,__LINE__,got_eof);
		} else {		// binary file
			unsigned int    i;
			char            achar[LineLength];
			inLine.fromSTR("");
			switch (_dataFILE.back().get_type()) {
			case DataFile::bin_uchar:
				for (i = 0; i < expected_fields; i++) {
					unsigned char   a;
					if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
						set_eof_flag_on_data_file();
						return i == 0 ? eof_before_data : eof_after_data;
					}
					sprintf(achar, "%f ", double(a));
					inLine.catSTR(achar);
				}
				break;
			case DataFile::bin_16bit:
				for (i = 0; i < expected_fields; i++) {
					unsigned short int a;
					if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
						set_eof_flag_on_data_file();
						return i == 0 ? eof_before_data : eof_after_data;
					}
					sprintf(achar, "%f ", double(a));
					inLine.catSTR(achar);
				}
				break;
			case DataFile::bin_int:
				for (i = 0; i < expected_fields; i++) {
					int             a;
					if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
						set_eof_flag_on_data_file();
						return i == 0 ? eof_before_data : eof_after_data;
					}
					sprintf(achar, "%f ", double(a));
					inLine.catSTR(achar);
				}
				break;
			case DataFile::bin_float:
				for (i = 0; i < expected_fields; i++) {
					float           a;
					if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
						set_eof_flag_on_data_file();
						return i == 0 ? eof_before_data : eof_after_data;
					}
					sprintf(achar, "%f ", double(a));
					inLine.catSTR(achar);
				}
				break;
			case DataFile::bin_double:
				for (i = 0; i < expected_fields; i++) {
					double          a;
					if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
						set_eof_flag_on_data_file();
						return i == 0 ? eof_before_data : eof_after_data;
					}
					sprintf(achar, "%f ", double(a));
					inLine.catSTR(achar);
				}
				break;
			default:
				fatal_err("`read' confused about binary file type.  Check `open'");
			}
		}
		// If ran out of data, set a flag and return.
		if (got_eof != no_eof) {
			//printf("get_next_data_line %s:%d setting eof flag.\n",__FILE__,__LINE__);
			set_eof_flag_on_data_file();
			return got_eof;
		}
	}
	_dataFILE.back().increment_line(); // BUG: should only do this if reading from datafile
	// If no newline, over-ran buffer.
	// Check that newline-terminated; note already checked for EOF
	if (inLine.size() > 0 && inLine[inLine.size() - 1] != '\n') {
		err("Internal bug with inLine buffer: please report to author.");
		return eof_before_data;
	}
	// Return flag that didn't run out of data.
	return no_eof;
}

// get next word from datafile.  Fail if file not open.  Return true if EOF
static bool
get_next_data_word()
{
        // Get line from cmd-file.
	if (_dataFILE.back().get_type() == DataFile::from_cmdfile) { 
		// from get_next_data_line()
		if (_cmdFILE.back().get_interactive()) {
			err("cannot `read variable/synonym' from     interactive cmdfile\n");
		} else {
			// from get_next_data_line()
			if (true == inLine.word_from_FILE(_cmdFILE.back().get_fp())) {
				set_eof_flag_on_data_file();
				return true;
			}
		}
		return false;
	} else {	
		// Get line from data-file.
		if (_dataFILE.back().get_type() == DataFile::ascii) { 
                        // ASCII file, non interactive
			if (1 == inLine.word_from_FILE(_dataFILE.back().get_fp())) {
				set_eof_flag_on_data_file();
				return true;
			}
		} else {
			// binary file
			char            achar[LineLength];
			inLine.fromSTR("");
			switch (_dataFILE.back().get_type()) {
			case DataFile::bin_uchar:
			{
				unsigned char   a;
				if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
					set_eof_flag_on_data_file();
					return true;
				}
				sprintf(achar, "%f ", ((double) a));
				inLine.catSTR(achar);
			}
			break;
			case DataFile::bin_16bit:
			{
				unsigned short int a;
				if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
					set_eof_flag_on_data_file();
					return true;
				}
				sprintf(achar, "%f ", ((double) a));
				inLine.catSTR(achar);
			}
			break;
			case DataFile::bin_int:
			{
				int             a;
				if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
					set_eof_flag_on_data_file();
					return true;
				}
				sprintf(achar, "%f ", (double) a);
				inLine.catSTR(achar);
			}
			break;
			case DataFile::bin_float:
			{
				float           a;
				if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
					set_eof_flag_on_data_file();
					return true;
				}
				sprintf(achar, "%f ", (double) a);
				inLine.catSTR(achar);
			}
			break;
			case DataFile::bin_double:
			{
				double          a;
				if (1 != fread((char *) & a, sizeof(a), 1, _dataFILE.back().get_fp())) {
					set_eof_flag_on_data_file();
					return true;
				}
				sprintf(achar, "%f ", (double) a);
				inLine.catSTR(achar);
			}
			break;
			default:
				fatal_err("`read' confused about binary file type.  Check `open'");
			}
		}
#if 0				// 2.065: should have been caught anyway ***
		// If ran out of data, set a flag and return.
		if (feof(_dataFILE.back().get_fp())) {
			set_eof_flag_on_data_file();
			return true;
		}
#endif
		_dataFILE.back().increment_line();
		// Return flag that didn't run out of data.
		return false;
	}
	// NOT REACHED
}

void
set_eof_flag_on_data_file()
{
	PUT_VAR("..eof..", 1.0);
}

void
clear_eof_flag_on_data_file()
{
	PUT_VAR("..eof..", 0.0);
}
