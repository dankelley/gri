// #define DEBUG_STORAGE
#include <string>
#include <stdlib.h>
#include <stddef.h>

#include        "gr.hh"
#include	"extern.hh"

bool            delete_columnsCmd(void);
static void     delete_all_columns(void);
static bool     delete_columns_randomly(void);
bool            delete_columns_where_missing(void);
bool            delete_gridCmd(void);
extern char     _grTempString[];

// `delete .variable.'
// `delete \synonym'
// `delete columns [randomly .fraction.]'
// `delete grid'
// `delete x|y scale'
bool
deleteCmd()
{
	if (_nword == 1) {
		/* Missing item */
		err("`delete' what?");
		return false;
	} else if (is_var(_word[1]) || is_syn(_word[1])) {
		/* Deleting variable/synonym (s) */
		int             i;
		for (i = 1; i < _nword; i++) {
			if (is_var(_word[i])) {
				if (!delete_var(_word[i])) {
					warning("`delete' can't delete non-existent variable `\\",
						_word[i], "'", "\\");
					return false;
				}
			} else if (is_syn(_word[i])) {
				if (!delete_syn(_word[i])) {
					warning("`delete' can't delete non-existent synonym `\\",
						_word[i], "'", "\\");
					return false;
				}
			}
		}
		return true;
	} else if (word_is(1, "columns")) {
		return delete_columnsCmd();
	} else if (word_is(1, "grid")) {
		/* Delete the grid */
		if (_nword == 2) {
			delete_gridCmd();
			return true;
		} else {
			demonstrate_command_usage();
			err("Extra words in command.");
			return false;
		}
	} else if (word_is(1, "scale")) {
		/* Delete both the x and y scales */
		_yincreasing = true;
		_xscale_exists = false;
		_need_x_axis = true;
		_yscale_exists = false;
		_need_y_axis = true;
		_user_set_x_axis = false;
		_user_set_y_axis = false;
		gr_setxtransform(gr_axis_LINEAR);
		gr_setxlabel("x");
		gr_setytransform(gr_axis_LINEAR);
		gr_setylabel("y");
		return true;
	} else if (word_is(1, "x")) {
		/* Delete the x scale */
		if (word_is(2, "scale") && _nword == 3) {
			_xscale_exists = false;
			_need_x_axis = true;
			_user_set_x_axis = false;
			_xtype = gr_axis_LINEAR;
			gr_setxtransform(gr_axis_LINEAR);
			gr_setxlabel("x");
			return true;
		} else {
			demonstrate_command_usage();
			err("`delete x' what?");
			return false;
		}
	} else if (word_is(1, "y")) {
		/* Delete the y scale */
		if (word_is(2, "scale") && _nword == 3) {
			_yscale_exists = false;
			_need_y_axis = true;
			_yincreasing = true;
			_user_set_y_axis = false;
			_ytype = gr_axis_LINEAR;
			gr_setytransform(gr_axis_LINEAR);
			gr_setylabel("y");
			return true;
		} else {
			demonstrate_command_usage();
			err("`delete y' what?");
			return false;
		}
	} else {
		demonstrate_command_usage();
		err("`delete' what?");
		return false;
	}
}

// `delete columns [{randomly .fraction.}|{where missing}]'
bool
delete_columnsCmd()
{
	switch(_nword) {
	case 2:
		// `delete columns'
		delete_all_columns();
		return true;
	case 4:
		// `delete columns randomly .fraction.'
		if (word_is(2, "randomly")) {
			return delete_columns_randomly();
		} else if (word_is(2, "where") || word_is(3, "missing")) {
			return delete_columns_where_missing();
		} else {
			demonstrate_command_usage();	    
			err("Syntax must be `randomly .f.' or `where missing'");
			return false;
		}
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
}

static void
delete_all_columns()
{
	_colX.setDepth(0);
	_colX.compact();
	_colY.setDepth(0);
	_colY.compact();
	_colZ.setDepth(0);
	_colZ.compact();
	_colU.setDepth(0);
	_colU.compact();
	_colV.setDepth(0);
	_colV.compact();
	_colR.setDepth(0);
	_colR.compact();
	_colTHETA.setDepth(0);
	_colTHETA.compact();
}

static bool
delete_columns_randomly()
{
	double fraction;
	if (!getdnum(_word[3], &fraction)) {
		READ_WORD_ERROR(".fraction.");
		return false;
	}
	if (fraction < 0.0) {
		warning("`delete columns randomly' clipping .fraction. to 0 (did no deletions)");
		return true;		// do nothing
	}
	if (fraction > 1.0) {
		warning("`delete columns randomly' clipping .fraction. to 1");
		delete_all_columns();
		return true;
	}
	unsigned int length = _colX.size();
	double miss = gr_currentmissingvalue();
	int good = 0;
	vector<int> ok((size_t)length, 0);
	unsigned int i;
	for (i = 0; i < length; i++) {
		ok[i] = (char)0;
		if (_colX.size() > 0 && (_colX[i] == miss))
			continue;
		if (_colY.size() > 0 && (_colY[i] == miss))
			continue;
		if (_colZ.size() > 0 && (_colZ[i] == miss))
			continue;
		if (_colU.size() > 0 && (_colU[i] == miss))
			continue;
		if (_colV.size() > 0 && (_colV[i] == miss))
			continue;
		if (_colR.size() > 0 && (_colR[i] == miss))
			continue;
		if (_colTHETA.size() > 0 && (_colTHETA[i] == miss))
			continue;
		ok[i] = (char)1;
		good++;
	}
	// Create vector of whether to kill a given index
	vector<int> kill((size_t)length, 0);
	// Laborously get correct number of data to discard.  Originally
	// I just tried to remove the given number, but that adds
	// an extra element of randomness.
	unsigned int subset = (unsigned int)(floor(good * fraction + 0.5));
	unsigned int collisions = 0;
#if defined(HAVE_DRAND48)
	srand48(getpid());
#else
	srand(getpid());
#endif
	for (i = 0; i < subset; i++) {
#if defined(HAVE_DRAND48)	// range is 0 to 1, but do modulus in case
		int index = int(drand48() * length) % length;
#else
		int index = int(rand() % length);
#endif
		if (ok[index]) {
			if (kill[index]) {
				if (collisions++ > length) {
					sprintf(_grTempString, "`delete columns randomly' could only delete %d columns\n", i);
					warning(_grTempString);
					break;
				}
				i--;
			} else {
				kill[index] = 1;
			}
		} else {
			i--;
		}
	}
	for (i = 0; i < length; i++) {
		if (kill[i]) {
			if (_colX.size())
				_colX[i] = miss;
			if (_colY.size())
				_colY[i] = miss;
			if (_colZ.size())
				_colZ[i] = miss;
			if (_colU.size())
				_colU[i] = miss;
			if (_colV.size())
				_colV[i] = miss;
			if (_colR.size())
				_colR[i] = miss;
			if (_colTHETA.size())
				_colTHETA[i] = miss;
		}
	}	
	return true;
}

bool
delete_columns_where_missing()
{
	int haveX, haveY, haveZ, haveU, haveV, haveR, haveTHETA;
	haveX = haveY = haveZ = haveU = haveV = haveR = haveTHETA = 0;
	unsigned int length = _colX.size();
	unsigned int i;
	for (i = 0; i < length; i++) {
		if (_colX.size()) haveX = 1;
		if (_colY.size()) haveY = 1;
		if (_colZ.size()) haveZ = 1;
		if (_colU.size()) haveU = 1;
		if (_colV.size()) haveV = 1;
		if (_colR.size()) haveR = 1;
		if (_colTHETA.size()) haveTHETA = 1;
	}
	double *xP = _colX.begin();
	double *yP = _colY.begin();
	double *zP = _colZ.begin();
	double *uP = _colU.begin();
	double *vP = _colV.begin();
	double *rP = _colR.begin();
	double *thetaP = _colTHETA.begin();
	vector<int> kill((size_t)length, 0);
	int num_to_kill = 0;
	for (i = 0; i < length; i++) {
		if (haveX && gr_missing(xP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
		if (haveY && gr_missing(yP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
		if (haveZ && gr_missing(zP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
		if (haveU && gr_missing(uP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
		if (haveV && gr_missing(vP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
		if (haveR && gr_missing(rP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
		if (haveTHETA && gr_missing(thetaP[i])) {
			kill[i] = 1; num_to_kill++; continue;
		}
	}
	if (!num_to_kill) {
		return true;
	}
	for (i = length - 1; i != 0; i--) {
		if (kill[i]) {
			if (haveX) _colX.erase(_colX.begin() + i);
			if (haveY) _colY.erase(_colY.begin() + i);
			if (haveZ) _colZ.erase(_colZ.begin() + i);
			if (haveU) _colU.erase(_colU.begin() + i);
			if (haveV) _colV.erase(_colV.begin() + i);
			if (haveR) _colR.erase(_colR.begin() + i);
			if (haveTHETA) _colTHETA.erase(_colTHETA.begin() + i);
		}
	}
	PUT_VAR("..num_col_data..", double(_colX.size()));
	PUT_VAR("..num_col_data_missing..", 0);
	length -= num_to_kill;
	return true;
}

bool
delete_gridCmd()
{
	Require(_nword == 2, err("Must have `delete grid'"));
	if (!strcmp(_word[1], "grid")) {
		_f_xy.set_size(0, 0);
		_legit_xy.set_size(0, 0);
		if (_grid_exists == true) {
			_f_min = _f_max = gr_currentmissingvalue();
			_grid_exists = false;
		}
		if (_xgrid_exists == true) {
#if defined(DEBUG_STORAGE)
			printf("delete clearing _xmatrix=%x\n", _xmatrix);
#endif
			delete [] _xmatrix;
			_num_xmatrix_data = 0;
			_xgrid_exists = false;
		}
		if (_ygrid_exists == true) {
#if defined(DEBUG_STORAGE)
			printf("delete clearing _ymatrix=%x\n", _ymatrix);
#endif
			delete [] _ymatrix;
			_num_ymatrix_data = 0;
			_ygrid_exists = false;
		}
	} else {
		err("Must have `delete grid'");
		return false;
	}
	return true;
}
