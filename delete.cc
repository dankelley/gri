// #define DEBUG_STORAGE
#include <string>
#include <stdlib.h>
#include <stddef.h>

#include        "gr.hh"
#include	"extern.hh"
#include        "private.hh"

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
		err("`delete' what?");
		return false;
	} 
	std::string w1(_word[1]); 
	un_double_quote(w1);
	//un_double_slash(w1);

	//de_reference(w1);

	if (is_var(w1) || is_syn(w1)) {
		/* Deleting variable/synonym (s) */
		for (unsigned int i = 1; i < _nword; i++) {
			//printf("DEBUG %s:%d should delete <%s>\n",__FILE__,__LINE__,_word[i]);
			w1.assign(_word[i]);
			un_double_quote(w1);
			//un_double_slash(w1);

			//de_reference(w1);

			if (is_var(w1)) {
				if (!delete_var(w1)) {
					warning("`delete' can't delete non-existent variable `\\", w1.c_str(), "'", "\\");
					return false;
				}
			} else if (is_syn(w1)) {
				if (w1[1] == '@') {
					std::string clean("\\");
					clean.append(w1.substr(2, w1.size()));
					std::string named;
					get_syn(clean.c_str(), named, false);
					//printf("DELETE IS AN ALIAS SYN %s:%d <%s>  [%s]\n",__FILE__,__LINE__,clean.c_str(),named.c_str());
					if (is_var(named.c_str())){
						if (!delete_var(named.c_str())) {
							warning("`delete' can't delete non-existent variable `\\", w1.c_str(), "'", "\\");
							return false;
						}
						return true;
					} else if (is_syn(named.c_str())) {
						if (!delete_syn(named.c_str())) {
							warning("`delete' can't delete non-existent synonym `\\", named.c_str(), "'", "\\");
							return false;
						}
					} else {
						err("`delete' cannot decode `\\", w1.c_str(), "'", "\\");
						return false;
					}
					return true;
				} else {
					// Non-aliased synonym.
					std::string value;
					if (get_syn(w1.c_str(), value, false)) {
						std::string coded_name;
						int coded_level = -1;
						//printf("DEBUG <%s>\n", value.c_str());
						if (is_coded_string(value.c_str(), coded_name, &coded_level)) {
							//printf("DEBUG %s:%d is <%s> <%s> level %d\n",__FILE__,__LINE__,value.c_str(),coded_name.c_str(),coded_level);
							if (coded_name[0] == '.') {
								int index = index_of_variable(coded_name.c_str(), coded_level);
								//printf("should delete var at %d\n",index);
								if (index > -1)
									variableStack.erase(variableStack.begin() + index);
								continue;
							} else if (coded_name[0] == '\\') {
								int index = index_of_synonym(coded_name.c_str(), coded_level);
								//printf("should delete syn at %d\n",index);
								//show_syn_stack();
								if (index > -1)
									synonymStack.erase(synonymStack.begin() + index);
								//printf("OK, is it gone?\n");
								//show_syn_stack();
								continue;
							} else {
								err("`delete' cannot decode `\\", w1.c_str(), "'", "\\");
								return false;
							}
							
						}
					}
					if (!delete_syn(w1)) {
						warning("`delete' can't delete non-existent synonym `\\", w1.c_str(), "'", "\\");
						return false;
					}
				}
			} else {
				warning("`delete' can't delete item `\\", w1.c_str(), "' since it is neither a synonym nor a variable", "\\");
				return false;
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
	_colWEIGHT.setDepth(0);
	_colWEIGHT.compact();
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
	std::vector<int> ok((size_t)length, 0);
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
		if (_colWEIGHT.size() > 0 && (_colWEIGHT[i] == miss))
			continue;
		ok[i] = (char)1;
		good++;
	}
	// Create vector of whether to kill a given index
	std::vector<int> kill((size_t)length, 0);
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
			if (_colWEIGHT.size())
				_colWEIGHT[i] = miss;
			
		}
	}	
	return true;
}

bool
delete_columns_where_missing()
{
	int haveX, haveY, haveZ, haveU, haveV, haveWEIGHT;
	haveX = haveY = haveZ = haveU = haveV = haveWEIGHT = 0;
	unsigned int length = _colX.size();
	unsigned int i;
	for (i = 0; i < length; i++) {
		if (_colX.size()) haveX = 1;
		if (_colY.size()) haveY = 1;
		if (_colZ.size()) haveZ = 1;
		if (_colU.size()) haveU = 1;
		if (_colV.size()) haveV = 1;
		if (_colWEIGHT.size()) haveWEIGHT = 1;
	}
	double *xP = _colX.begin();
	double *yP = _colY.begin();
	double *zP = _colZ.begin();
	double *uP = _colU.begin();
	double *vP = _colV.begin();
	double *weightP = _colWEIGHT.begin();
	std::vector<int> kill((size_t)length, 0);
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
		if (haveWEIGHT && gr_missing(weightP[i])) {
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
			if (haveWEIGHT) _colWEIGHT.erase(_colWEIGHT.begin() + i);
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
