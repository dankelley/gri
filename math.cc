#include	<math.h>
#include	<string.h>
#include	"extern.hh"
#include	"image_ex.hh"

bool            mathCmd();
static bool     column_math(double *Ptr, int n, int operator_position = 1);
static bool     image_math(void);
static bool     grid_data_math(void);
extern void     highpass_image(void);
extern void     lowpass_image(void);

bool
mathCmd()
{
	if (_nword < 3) {
		err("Proper format: `.x. = 10.0', `.x. *= 2', or `.x. = { rpn ... }'");
		return false;
	}
	double          number, old_value = 0.0;
	// Variable
	if (is_var(_word[0])) {
		if (!getdnum(_word[2], &number))
			return false;
		if (word_is(1, "=")) {
			// `.variable. = .value.'
			PUT_VAR(_word[0], number);
		} else if (word_is(1, "+=")) {
			get_var(_word[0], &old_value);
			old_value += number;
			PUT_VAR(_word[0], old_value);
		} else if (word_is(1, "-=")) {
			get_var(_word[0], &old_value);
			old_value -= number;
			PUT_VAR(_word[0], old_value);
		} else if (word_is(1, "*=")) {
			get_var(_word[0], &old_value);
			old_value *= number;
			PUT_VAR(_word[0], old_value);
		} else if (word_is(1, "/=")) {
			get_var(_word[0], &old_value);
			old_value /= number;
			PUT_VAR(_word[0], old_value);
		} else if (word_is(1, "^=")) {
			get_var(_word[0], &old_value);
			old_value = pow(old_value, number);
			PUT_VAR(_word[0], old_value);
		} else if (word_is(1, "_=")) {
			get_var(_word[0], &old_value);
			if (number < 0.0) {
				err("Cannot do log to negative base");
				return false;
			}
			if (!gr_missing(old_value) && old_value > 0.0) {
				old_value = log(old_value) / log(number);
				PUT_VAR(_word[0], old_value);
			} else {
				PUT_VAR(_word[0], gr_currentmissingvalue());
			}
		} else {
			err("`\\", _word[1], "' is not a known operator for variables", "'\\");
			return false;
		}
	} else if (_nword > 3 && word_is(0, "grid") && word_is(1, "data")) {
		grid_data_math();
	} else if (_nword > 3 && word_is(0, "grid") && word_is(1, "x")) {
		if (_num_xmatrix_data)
			column_math(_xmatrix, _num_xmatrix_data, 2);
		else {
			warning("Can't do math on xmatrix with no data in it already.");
			return true;
		}
	} else if (_nword > 3 && word_is(0, "grid") && word_is(1, "y")) {
		if (_num_ymatrix_data)
			column_math(_ymatrix, _num_ymatrix_data, 2);
		else {
			warning("Can't do math on ymatrix with no data in it already.");
			return true;
		}
	} else if (word_is(0, "u")) {
		if (_colU.size())
			column_math(_colU.begin(), _colU.size());
		else {
			warning("Can't do math on u-column with no data in it already.");
			return true;
		}
	} else if (word_is(0, "v")) {
		if (_colV.size())
			column_math(_colV.begin(), _colV.size());
		else {
			warning("Can't do math on v-column with no data in it already.");
			return true;
		}
	} else if (word_is(0, "x")) {
		if (_colX.size())
			column_math(_colX.begin(), _colX.size());
		else {
			warning("Can't do math on x-column with no data in it already.");
			return true;
		}
	} else if (word_is(0, "y")) {
		if (_colY.size())
			column_math(_colY.begin(), _colY.size());
		else {
			warning("Can't do math on y-column with no data in it already.");
			return true;
		}
	} else if (word_is(0, "z")) {
		if (_colZ.size())
			column_math(_colZ.begin(), _colZ.size());
		else {
			warning("Can't do math on z-column with no data in it already.");
			return true;
		}
	} else if (word_is(0, "image")) {
		image_math();
	} else {
		err("Cannot do mathematical operation on unrecognized item `\\",
		    _word[0],
		    "'.",
		    "\\");
		return false;
	}
	return true;
}

static bool
column_math(double *Ptr, int n, int operator_position /*= 1*/)
{
	Require(n > 0, gr_Error("Trying to do column-math on non-existent column."));
	// Ensure enough space
	if      (word_is(0, "x"))	  {_colX.setDepth(n);      Ptr = _colX.begin();}
	else if (word_is(0, "y"))	  {_colY.setDepth(n);      Ptr = _colY.begin();}
	else if (word_is(0, "u"))	  {_colU.setDepth(n);      Ptr = _colU.begin();}
	else if (word_is(0, "v"))	  {_colV.setDepth(n);      Ptr = _colV.begin();}
	else if (word_is(0, "z"))	  {_colZ.setDepth(n);      Ptr = _colZ.begin();}
	else if (word_is(0, "WEIGHT"))    {_colWEIGHT.setDepth(n); Ptr = _colWEIGHT.begin();}

	// special case of `y = x...'
	if (_nword == 3 && word_is(0, "y") && word_is(1, "=") && word_is(2, "x")) {
		for (int i = 0; i < n; i++)
			_colY[i] = _colX[i];
		if (word_is(0, "y") && _need_y_axis && !_user_set_y_axis)
			create_y_scale();
		return true;
	}

	double          number;
	if (!getdnum(_word[operator_position + 1], &number))
		return false;
    
	// Special case of assignment
	if (word_is(operator_position, "=")) {
		// Make sure column can hold the data
		for (int i = 0; i < n; i++, Ptr++) {
			if (!gr_missing(*Ptr)) {
				*Ptr = number;
			}
		}
		if (word_is(0, "y") && _need_y_axis && !_user_set_y_axis)
			create_y_scale();
		return true;
	}

	// Do modification (OP=) cases
	if (word_is(operator_position, "+=")) {
		for (int i = 0; i < n; i++, Ptr++) 
			if (!gr_missing(*Ptr))
				*Ptr += number;
	} else if (word_is(operator_position, "-=")) {
		for (int i = 0; i < n; i++, Ptr++)
			if (!gr_missing(*Ptr))	
				*Ptr -= number;
	} else if (word_is(operator_position, "*=")) {
		for (int i = 0; i < n; i++, Ptr++)
			if (!gr_missing(*Ptr))
				*Ptr *= number;
	} else if (word_is(operator_position, "/=")) {
		for (int i = 0; i < n; i++, Ptr++)
			if (!gr_missing(*Ptr))	
				*Ptr /= number;
	} else if (word_is(operator_position, "^=")) {
		for (int i = 0; i < n; i++, Ptr++)
			if (!gr_missing(*Ptr))
				*Ptr = pow(*Ptr, number);
	} else if (word_is(operator_position, "_=")) {
		if (number < 0.0) {
			err("Cannot do log to negative base");
			return false;
		}
		double lbase = log(number);
		for (int i = 0; i < n; i++, Ptr++)
			if (!gr_missing(*Ptr) && *Ptr > 0.0)
				*Ptr = log(*Ptr) / lbase;
			else
				*Ptr = gr_currentmissingvalue();
	} else {
		err("`\\", _word[operator_position], "' not a known operator", "'\\");
		return false;
	}
	if (word_is(0, "x") && _need_x_axis && !_user_set_x_axis)
		create_x_scale();
	if (word_is(0, "y") && _need_y_axis && !_user_set_y_axis)
		create_y_scale();
	return true;
}

static bool
grid_data_math()
{
	if (_nword < 4) {
		err("No number for operation.");
		return false;
	}
	if (!_grid_exists) {
		err("First `read grid data'");
		return false;
	}
	double          number;
	if (!getdnum(_word[3], &number)) {
		err("Can't read number");
		return false;
	}
	unsigned int row, col;
	if (word_is(2, "=")) {
		for (col = 0; col < _num_xmatrix_data; col++) {
			for (row = 0; row < _num_ymatrix_data; row++) {
				_f_xy(col, row) = number;
				_legit_xy(col, row) = true;
			}
		}
		_f_min = _f_max = number;
		return true;
	} else if (word_is(2, "+=")) {
		for (col = 0; col < _num_xmatrix_data; col++)
			for (row = 0; row < _num_ymatrix_data; row++)
				_f_xy(col, row) += number;
		_f_min += number;
		_f_max += number;
		return true;
	} else if (word_is(2, "-=")) {
		for (col = 0; col < _num_xmatrix_data; col++)
			for (row = 0; row < _num_ymatrix_data; row++)
				_f_xy(col, row) -= number;
		_f_min -= number;
		_f_max -= number;
		return true;
	} else if (word_is(2, "*=")) {
		for (col = 0; col < _num_xmatrix_data; col++)
			for (row = 0; row < _num_ymatrix_data; row++)
				_f_xy(col, row) *= number;
		_f_min *= number;
		_f_max *= number;
		return true;
	} else if (word_is(2, "/=")) {
		if (number == 0.0) {
			warning("Can't divide by zero");
			return true;
		}
		for (col = 0; col < _num_xmatrix_data; col++)
			for (row = 0; row < _num_ymatrix_data; row++)
				_f_xy(col, row) /= number;
		_f_min /= number;
		_f_max /= number;
		return true;
	} else if (word_is(2, "^=")) {
		for (col = 0; col < _num_xmatrix_data; col++)
			for (row = 0; row < _num_ymatrix_data; row++)
				_f_xy(col, row) = pow(_f_xy(col, row), number);
		matrix_limits(&_f_min, &_f_max);
		return true;
	} else if (word_is(2, "_=")) {
		if (number < 0.0) {
			err("Cannot do log to negative base");
			return false;
		}
		double lbase = log(number);
		for (col = 0; col < _num_xmatrix_data; col++)
			for (row = 0; row < _num_ymatrix_data; row++)
				if (!gr_missing(_f_xy(col, row)) && _f_xy(col, row) > 0.0)
					_f_xy(col, row) = log(_f_xy(col, row)) / lbase;
				else 
					_f_xy(col, row) = gr_currentmissingvalue();
		matrix_limits(&_f_min, &_f_max);
		return true;
	} else {
		err("`grid data' given unknown operator `\\", _word[2], ";", "\\");
		return false;
	}
}

// `image [grayscale|colorscale] OPERATOR OPERAND'
static bool
image_math()
{
	int i, n;
	unsigned char  *Ptr;
	double          number;
	switch (_nword) {
	case 3:
		// `image OPERATOR OPERAND'
		if (word_is(1, "=")) {
			if (word_is(2, "highpass"))
				highpass_image();
			else if (word_is(2, "lowpass"))
				lowpass_image();
			else {
				err("`image =' what?");
				return false;
			}
			return true;
		}
		if (!getdnum(_word[2], &number))
			return false;
		if (!image_exists()) {
			err("First `read image' or `convert grid to image'");
			return false;
		}
		// Convert to image units
		number *= 255.0 / (_image255 - _image0);
		Ptr = _image.image;
		n = _image.ras_length;
		if (word_is(1, "+=")) {
			for (i = 0; i < n; i++, Ptr++)
				*Ptr = int(*Ptr + number);
		} else if (word_is(1, "-=")) {
			for (i = 0; i < n; i++, Ptr++)
				*Ptr = int(*Ptr - number);
		} else if (word_is(1, "*=")) {
			for (i = 0; i < n; i++, Ptr++)
				*Ptr = int(*Ptr * number);
		} else if (word_is(1, "/=")) {
			for (i = 0; i < n; i++, Ptr++)
				*Ptr = int(*Ptr / number);
		} else if (word_is(1, "^=")) {
			for (i = 0; i < n; i++, Ptr++)
				*Ptr = int(pow(double(*Ptr), number));
		} else if (word_is(1, "_=")) {
			if (number < 0.0) {
				err("Cannot do log to negative base");
				return false;
			}
			double lbase = log(number);
			for (i = 0; i < n; i++, Ptr++)
				if (!gr_missing(*Ptr) && *Ptr > 0)
					*Ptr = int(log(*Ptr) / lbase);
				else
					*Ptr = 0;	// BUG -- should be something else
		} else {
			err("Cannot use operator `\\", _word[1], "' on images.", "'\\");
			return false;
		}
		break;
	case 4:
		if (word_is(1, "colorscale") || word_is(1, "colourscale")) {
			n = 3 * 256;
		} else if (word_is(1, "greyscale") || word_is(1, "grayscale")) {
			n = 256;
		} else {
			err("Second word must be `colorscale', `colourscale', `grayscale' or `greyscale'");
			return false;
		}
		if (!getdnum(_word[3], &number))
			return false;
		Ptr = _imageTransform;
		if (word_is(2, "+=")) {
			for (i = 0; i < n; i++, Ptr++) {
				*Ptr = (int) (255.0 * (double(*Ptr) / 255.0 + number));
			}
		} else if (word_is(2, "-=")) {
			for (i = 0; i < n; i++, Ptr++) {
				*Ptr = (int) (255.0 * (double(*Ptr) / 255.0 - number));
			}
		} else if (word_is(2, "*=")) {
			for (i = 0; i < n; i++, Ptr++) {
				*Ptr = (int) (255.0 * (double(*Ptr) / 255.0 * number));
			}
		} else if (word_is(2, "/=")) {
			for (i = 0; i < n; i++, Ptr++) {
				*Ptr = (int) (255.0 * (double(*Ptr) / 255.0 / number));
			}
		} else if (word_is(2, "^=")) {
			for (i = 0; i < n; i++, Ptr++) {
				*Ptr = (int) (255.0 * pow(double(*Ptr) / 255.0, number));
			}
		} else if (word_is(2, "_=")) {
			if (number < 0.0) {
				err("Cannot do log to negative base");
				return false;
			}
			double lbase = log(number);
			for (i = 0; i < n; i++, Ptr++)
				*Ptr = int(255.0 * log(double(*Ptr) / 255.0) / lbase);
		} else {
			err("Cannot use operator `\\", _word[2], "' on image colorscale|grayscale", "'\\");
			return false;
		}
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	return true;
}
