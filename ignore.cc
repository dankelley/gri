#include	<math.h>
#include	<string.h>
#include	"extern.hh"
static bool reduce_columns(unsigned int n);

bool
ignoreCmd()
{
	double           tmp;
	if (_nword != 3) {
		err("Proper syntax 'ignore last n'");
		return false;
	}
	if (!strcmp(_word[1], "last")) {
		if (!getdnum(_word[2], &tmp)) {
			err("Can't read n in 'ignore last n'");
			return false;
		}
		if (tmp < 0) {
			err("Can't have n<0 in 'ignore last n'");
			return false;
		}
		unsigned int n = unsigned(floor(0.5 + tmp));
		return reduce_columns(n);
	} else {
		err("Proper syntax 'ignore last n'");
		return false;
	}
}

static bool
reduce_columns(unsigned int n)
{
	if (!_columns_exist) {
		err("First 'read columns'");
		return false;
	}
	if (n > 0) {
		if (_colX.size()) {
			if (_colX.size() >= n) {
				_colX.setDepth(_colX.size() - n);
			} else {
				err("Too few columns to do that");
				return false;
			}
		}
		if (_colY.size()) {
			if (_colY.size() >= n) {
				_colY.setDepth(_colY.size() - n);
			} else {
				err("Too few columns to do that");
				return false;
			}
		}
		if (_colZ.size()) {
			if (_colZ.size() >= n) {
				_colZ.setDepth(_colZ.size() - n);
			} else {
				err("Too few columns to do that");
				return false;
			}
		}
		if (_colU.size()) {
			if (_colU.size() >= n) {
				_colU.setDepth(_colU.size() - n);
			} else {
				err("Too few columns to do that");
				return false;
			}
		}
		if (_colV.size()) {
			if (_colV.size() >= n) {
				_colV.setDepth(_colV.size() - n);
			} else {
				err("Too few columns to do that");
				return false;
			}
		}
		if (_colWEIGHT.size()) {
			if (_colWEIGHT.size() >= n) {
				_colWEIGHT.setDepth(_colWEIGHT.size() - n);
			} else {
				err("Too few columns to do that");
				return false;
			}
		}
	}
	return true;
}
