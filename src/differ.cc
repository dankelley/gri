#include <string>
#include <stdio.h>
#include <stddef.h>

#include        "gr.hh"
#include	"extern.hh"
#include	"private.hh"

bool            differentiateCmd(void);

// `differentiate {{x|y} wrt index|{y|x}} | {grid wrt x|y}'
bool
differentiateCmd()
{
	switch (_nword) {
	case 4:
		if (!strcmp(_word[2], "wrt") && !strcmp(_word[1], "grid")) {
			// `differentiate grid wrt ...'
			if (!_xgrid_exists && !_ygrid_exists && !_grid_exists) {
				err("`differentiate grid' -- no grid exists yet\n");
				return false;
			}
			if (!strcmp(_word[3], "x")) {
				// `differentiate grid wrt x'
				if (_num_xmatrix_data < 3) {
					err("`differentiate grid wrt x' -- too few data");
					return false;
				}
				std::vector<bool> ok((size_t)_num_xmatrix_data, false);
				std::vector<double> tmp((size_t)_num_xmatrix_data, 0.0);
				for (unsigned int j = 0; j < _num_ymatrix_data; j++) {
					for (unsigned int i = 1; i < _num_xmatrix_data - 1; i++) {
						if (_legit_xy(i - 1, j) == true
						    && _legit_xy(i + 1, j) == true
						    && _xmatrix[i + 1] != _xmatrix[i - 1]) {
							tmp[i] = (_f_xy(i + 1, j) - _f_xy(i - 1, j))
								/ (_xmatrix[i + 1] - _xmatrix[i - 1]);
							ok[i] = true;
						} else {
							ok[i] = false;
						}
					}
					for (unsigned int i = 1; i < _num_xmatrix_data - 1; i++) {
						_f_xy(i, j) = tmp[i];
						if (ok[i] == true) {
							_f_xy(i, j) = tmp[i];
							_legit_xy(i, j) = true;
						} else {
							_legit_xy(i, j) = false;
						}
					}
					_f_xy(0, j) = _f_xy(1, j);
					_legit_xy(0, j) = _legit_xy(1, j);
					_f_xy(_num_xmatrix_data - 1, j) = _f_xy(_num_xmatrix_data - 2, j);
					_legit_xy(_num_xmatrix_data - 1, j) = _legit_xy(_num_xmatrix_data - 2, j);
				}
			} else if (!strcmp(_word[3], "y")) {
				// `differentiate grid wrt y'
				if (_num_ymatrix_data < 3) {
					err("`differentiate grid wrt y' -- too few data");
					return false;
				}
				std::vector<bool>   ok((size_t)_num_ymatrix_data, false);
				std::vector<double> tmp((size_t)_num_ymatrix_data, 0.0);
				for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
					for (unsigned int j = 1; j < _num_ymatrix_data - 1; j++) {
						if (_legit_xy(i, j - 1) == true
						    && _legit_xy(i, j + 1) == true
						    && _ymatrix[j + 1] != _ymatrix[j - 1]) {
							tmp[j] = (_f_xy(i, j + 1) - _f_xy(i, j - 1))
								/ (_ymatrix[j + 1] - _ymatrix[j - 1]);
							ok[j] = true;
						} else {
							ok[j] = false;
						}
					}
					for (unsigned int j = 1; j < _num_ymatrix_data - 1; j++) {
						if (ok[j] == true) {
							_f_xy(i, j) = tmp[j];
							_legit_xy(i, j) = true;
						} else {
							_legit_xy(i, j) = false;
						}
					}
					_f_xy(i, 0) = _f_xy(i, 1);
					_legit_xy(i, 0) = _legit_xy(i, 1);
					_f_xy(i, _num_ymatrix_data - 1) = _f_xy(i, _num_ymatrix_data - 2);
					_legit_xy(i, _num_ymatrix_data - 1) = _legit_xy(i, _num_ymatrix_data - 2);
				}
			} else {
				err("Can only differentiate grid wrt to `x' or `y'");
				return false;
			}
			matrix_limits(&_f_min, &_f_max);
			return true;
		} else if (!strcmp(_word[2], "wrt")) {
			// `differentiate ... wrt ...'
			if (!strcmp(_word[1], "x")) {
				// `differentiate x wrt ...'
				if (_colX.size() < 1) {
					err("No x column exists yet\n");
					return false;
				}
				if (_colX.size() < 2) {
					err("Sorry, x column has only 1 element\n");
					return false;
				}
				if (!strcmp(_word[3], "index")) {
					// `differentiate x wrt index'
					for (unsigned int i = 1; i < _colX.size(); i++) {
						double x0 = _colX[i];
						double xleft = _colX[i - 1];
						if (!gr_missingx(x0) && !gr_missingx(xleft)) {
							_colX[i - 1] = x0 - xleft;
						} else {
							_colX[i - 1] = gr_currentmissingvalue();
						}
					}
				} else if (!strcmp(_word[3], "y")) {
					// `differentiate x wrt y'
					for (unsigned int i = 1; i < _colX.size(); i++) {
						double x0 = _colX[i];
						double xleft = _colX[i - 1];
						double y0 = _colY[i];
						double yleft = _colY[i - 1];
						if (!gr_missingx(x0) && !gr_missingy(y0)
						    && !gr_missingx(xleft) && !gr_missingy(yleft)) {
							_colX[i - 1] = (x0  - xleft) / (y0 - yleft);
						} else {
							_colX[i - 1] = gr_currentmissingvalue();
						}
					}
				} else {
					err("Wrong word; must be `index' or `x'");
					return false;
				}
				// make something up for last point
				_colX[_colX.size() - 1] = _colX[_colX.size() - 2];
				create_x_scale();
				return true;
			} else if (!strcmp(_word[1], "y")) {
				// `differentiate y wrt ...'
				if (_colY.size() < 1) {
					err("No y column exists yet\n");
					return false;
				}
				if (_colY.size() < 2) {
					err("Sorry, y column has only 1 element\n");
					return false;
				}
				if (!strcmp(_word[3], "index")) {
					// `differentiate y wrt index'
					for (unsigned int i = 1; i < _colY.size(); i++) {
						double y0 = _colY[i];
						double yleft = _colY[i - 1];
						if (!gr_missingy(y0) && !gr_missingy(yleft)) {
							_colY[i - 1] = y0 - yleft;
						} else {
							_colY[i - 1] = gr_currentmissingvalue();
						}
					}
				} else if (!strcmp(_word[3], "x")) {
					// `differentiate y wrt x'
					for (unsigned int i = 1; i < _colY.size(); i++) {
						double x0 = _colX[i];
						double xleft = _colX[i - 1];
						double y0 = _colY[i];
						double yleft = _colY[i - 1];
						if (!gr_missingy(y0) && !gr_missingx(x0)
						    && !gr_missingy(y0) && !gr_missingy(yleft)) {
							_colY[i - 1] = (y0 - yleft) / (x0 - xleft);
						} else {
							_colY[i - 1] = gr_currentmissingvalue();
						}
					}
				} else {
					err("Wrong word; must be `index' or `y'");
					return false;
				}
				// make something up for last point
				_colY[_colY.size() - 1] = _colY[_colY.size() - 2];
				create_y_scale();
				return true;
			} else {
				err("Must be `differentiate x|y ...'");
				demonstrate_command_usage();
				err("Can't understand command.");
				return false;
			}
		} else {
			err("Need word `wrt'");	// not sure can get here
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return false;
		}
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
}
