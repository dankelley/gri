// Do rpn math
#define STRING_END(S) ((S) + strlen((S)) - 1)
#include	<string>
#include	<ctype.h>
#include	<stdio.h>
#include	<math.h>
#if defined(HAVE_ACCESS)
#include        <unistd.h>
#endif

#include        "gr.hh"
#include        "extern.hh"
#include        "private.hh"

#ifdef __DECXX
extern "C" double acosh(double x);         // DEC-cxx needs this
extern "C" double asinh(double x);         // DEC-cxx needs this
extern "C" double atanh(double x);         // DEC-cxx needs this
#endif

static std::vector<RpnItem> rS;
void
erase_rpn_stack()
{
	rS.erase(rS.begin(), rS.end());
}



// *******************************************
// *** Macros to simplify stack operations ***
// *******************************************
// Require index i to be type t.
#define NEED_IS_TYPE(i,t)						\
{									\
    if (rS[rS.size() - (i)].getType() != (t)) {				\
	RpnError = ILLEGAL_TYPE;					\
	return false;							\
    }									\
}
// Retrieve name (i-1) from top of stack (i=1 means top)
#define NAME(i)  (rS[rS.size() - (i)].getName())
// Retrieve value (i-1) from top of stack (i=1 means top)
#define VALUE(i) (rS[rS.size() - (i)].getValue())
// Retrieve type (i-1) from top of stack (i=1 means top)
#define TYPE(i)  (rS[rS.size() - (i)].getType())
// Set (i-1) from top of stack (i=1 means top)
#define SET(i, n, v, t) (rS[rS.size() - (i)].set((n), (v), (t)))

#define Ee 2.7182818284590452354
// rpn - reverse polish notation calculator
//
// RETURN VALUE: see defines below
// 

// Return codes: 
#define NO_ERROR 0		// everything is OK 
#define BAD_WORD 1		// not operator, operand, column, function 
#define STACK_UNDERFLOW 2	// missing operators 
#define STACK_OVERFLOW 3	// extra stuff 
#define DIV_BY_ZERO 4		// cannot divide by zero 
#define OUT_OF_STORAGE 5	// computer limitation 
#define ILLEGAL_TYPE 6		// cannot do with given operand(s) 
#define NEED_GE_0 7		// need operand >= 0 
#define NEED_GT_0 8		// need operand > 0 
#define RANGE_1 9		// need operand in range -1 to 1 
#define NEED_GT_1 10		// need operand > 1  
#define COMPUTER_LIMITATION 11	// can't do on this machine 
#define GENERAL_ERROR  12	// some other error 

// Operator types.
typedef enum {
	ADD = 1, SUBTRACT, MULTIPLY, DIVIDE,
	POWER,
	ASINE, ACOSINE, ATANGENT,
	SINE, COSINE, TANGENT,
	ACOSH, ASINH, ATANH,
	COSH, SINH, TANH,
	SQRT,
	LOG, LN, EXP, EXP10,
	CEIL, FLOOR, REMAINDER,
	ABS,
	LESS_THAN, LESS_THAN_EQUAL,
	GREATER_THAN, GREATER_THAN_EQUAL,
	EQUAL, NOT_EQUAL,
	AND, OR, NOT,
	CMTOPT, PTTOCM,
	DUP, POP, EXCH, ROLL_LEFT, ROLL_RIGHT, PSTACK, CLEAR,
	STRCAT, STRLEN, SUBSTR,
	ATOF,
	SYSTEM,
	SUP,
	INF,
	ASSIGN,
	XYCMTOUSER,
	XYUSERTOCM,
	XCMTOUSER,
	XPTTOUSER,
	XUSERTOCM,
	XUSERTOPT,
	YCMTOUSER,
	YPTTOUSER,
	YUSERTOCM,
	YUSERTOPT,
	AREA, VAL, MIN, MAX, MEDIAN, MEAN, STDDEV, SIZE,
	STRINGWIDTH, STRINGASCENT, STRINGDESCENT,
	DEFINED,
	ISMISSING,
	INTERPOLATE,
	RAND,
	NOT_OPERATOR,
	DIRECTORY_EXISTS,
	FILE_EXISTS,
	ARGC,
	ARGV,
	WORDC,
	WORDV
}               operator_name;

// Rpn functions.
typedef struct {
	char *op_name;
	unsigned int chars;	// for speeding lookup
	operator_name   op_id;
} RPN_DICT;
#define RPN_FCN_CAPACITY 100
#define RPN_W_CAPACITY   100
unsigned int    rpn_fcn_filled = 0;
typedef struct {
	char *            name;
	char *            w[RPN_W_CAPACITY];
	unsigned int    nw;
} RPN_FCN;
RPN_FCN         rpn_fcn[RPN_FCN_CAPACITY];

// Operators, with common (algebraic) ones first to speed lookup.
RPN_DICT        rpn_dict[] =
{
	{"+", 1, ADD},
	{"-", 1, SUBTRACT},
	{".", 1, MULTIPLY},
	{"*", 1, MULTIPLY},
	{"/", 1, DIVIDE},
	{"power", 5, POWER},
	{"asin", 4, ASINE},
	{"acos", 4, ACOSINE},
	{"atan", 4, ATANGENT},
	{"sin", 3, SINE},
	{"cos", 3, COSINE},
	{"tan", 3, TANGENT},
	{"asinh", 5, ASINH},
	{"acosh", 5, ACOSH},
	{"atanh", 5, ATANH},
	{"cosh", 4, COSH},
	{"sinh", 4, SINH},
	{"tanh", 4, TANH},
	{"sqrt", 4, SQRT},
	{"log", 3, LOG},
	{"ln", 2, LN},
	{"exp", 3, EXP},
	{"exp10", 5, EXP10},
	{"ceil", 4, CEIL},
	{"floor", 5, FLOOR},
	{"remainder", 9, REMAINDER},
	{"abs", 3, ABS},
	{"<", 1, LESS_THAN},
	{"<=", 2, LESS_THAN_EQUAL},
	{">", 1, GREATER_THAN},
	{">=", 2, GREATER_THAN_EQUAL},
	{"==", 2, EQUAL},
	{"!=", 2, NOT_EQUAL},
	{"&", 1, AND},
	{"and", 3, AND},
	{"|", 1, OR},
	{"or", 2, OR},
	{"!", 1, NOT},
	{"not", 3, NOT},
	{"cmtopt", 6, CMTOPT},
	{"pttocm", 6, PTTOCM},
	{"dup", 3, DUP},
	{"pop", 3, POP},
	{"exch", 4, EXCH},
	{"roll_left",  9, ROLL_LEFT},
	{"roll_right", 10, ROLL_RIGHT},
	{"pstack", 6, PSTACK},
	{"strcat", 6, STRCAT},
	{"strlen", 6, STRLEN},
	{"substr", 6, SUBSTR},
	{"atof", 4, ATOF},
	{"system", 6, SYSTEM},
	{"sup", 3, SUP},
	{"inf", 3, INF},
	{"=", 1, ASSIGN},
	{"xcmtouser", 9, XCMTOUSER},
	{"xpttouser", 9, XPTTOUSER},
	{"xycmtouser", 10, XYCMTOUSER},
	{"xyusertocm", 10, XYUSERTOCM},
	{"xusertocm", 9, XUSERTOCM},
	{"xusertopt", 9, XUSERTOPT},
	{"ycmtouser", 9, YCMTOUSER},
	{"ypttouser", 9, YPTTOUSER},
	{"yusertocm", 9, YUSERTOCM},
	{"yusertopt", 9, YUSERTOPT},
	{"area", 4, AREA},
	{"@", 1, VAL},
	{"min", 3, MIN},
	{"max", 3, MAX},
	{"median", 6, MEDIAN},
	{"mean", 4, MEAN},
	{"stddev", 6, STDDEV},
	{"size", 4, SIZE},
	{"directory_exists", 16, DIRECTORY_EXISTS},
	{"file_exists", 11, FILE_EXISTS},
	{"argc", 4, ARGC},
	{"argv", 4, ARGV},
	{"width", 5, STRINGWIDTH},
	{"ascent", 6, STRINGASCENT},
	{"descent", 7, STRINGDESCENT},
	{"defined", 7, DEFINED},
	{"ismissing", 9, ISMISSING},
	{"interpolate", 11, INTERPOLATE},
	{"rand", 4, RAND},
	{"wordc", 5, WORDC},
	{"wordv", 5, WORDV},
	{NULL, 0, NOT_OPERATOR}
};



int            RpnError;

#define PT_PER_IN 72.27		// points per inch 
#define PT_PER_CM 28.45		// points per centimetre 
#define deg_per_rad	(57.29577951)

void            gr_usertocm(double x, double y, double *x_cm, double *y_cm);
void            gr_cmtouser(double x_cm, double y_cm, double *x, double *y);
static operator_name is_oper(const char *w);
static operand_type is_operand(const char *w, double *operand);
bool            rpn_create_function(char *name, char ** w, unsigned int nw);
static unsigned int rpn_which_function(const char *w);
int             rpn(int nw, char **w, char ** result);
static bool     do_operation(operator_name oper);

int
rpn(int nw, char **w, char ** result)
{
	if (((unsigned) superuser()) & FLAG_RPN) {
		printf("%s:%d called rpn(", __FILE__,__LINE__);
		for (int i=0; i < nw - 1; i++)
			printf(" '%s',",w[i]);
		printf(" '%s')\n", w[nw-1]);
	}
	unsigned int    NW;
	char           *W[MAX_nword];
	operator_name   oper;
	double          operand_value;
	*result = "";
	if (nw < 1) {
		if (((unsigned) superuser()) & FLAG_RPN) printf("rpn() go stack underflow\n");
		return STACK_UNDERFLOW;
	}
	RpnError = 0;
	// Dump into new array (so can manipulate for funtions)
	NW = nw;
	if (NW < MAX_nword)
		for (unsigned int i = 0; i < NW; i++)
			W[i] = w[i];
	else
		return OUT_OF_STORAGE;
	// Now, scan through list, pushing operands onto stack and obeying
	// operators immediately.
	for (unsigned int i = 0; i < NW; i++) {
		if (((unsigned) superuser()) & FLAG_RPN) printf(" rpn scanning item '%s'\n",W[i]);
		if (NOT_OPERATOR != (oper = is_oper((const char*)W[i]))) {
			// Do indicated operation.
			do_operation(oper);
		} else {
			// Must be an operand
			unsigned int    which;
			char           *Wnew[MAX_nword];	// for function case 
			operand_type type = is_operand((const char*)W[i], &operand_value);
			RpnItem item;
			switch (type) {
			case NUMBER:
				item.set("", operand_value, type);
				rS.push_back(item);
				break;
			case COLUMN_NAME:
				item.set(W[i], operand_value, type);
				rS.push_back(item);
				break;
			case FUNCTION:
				which = rpn_which_function((const char*)W[i]) - 1;
				if ((NW + rpn_fcn[which].nw) >= MAX_nword) {
					fatal_err("Ran out of space in RPN expression");
				}
				// Shuffle words up.
				// Copy words up to function name 
				for (unsigned int ii = 0; ii < i; ii++) {
					Wnew[ii] = W[ii];
				}
				// Copy the function contents 
				for (unsigned int ii = 0; ii < rpn_fcn[which].nw; ii++) {
					Wnew[i + ii] = rpn_fcn[which].w[ii];
				}
				// Copy rest (skip the function name itself) 
				for (unsigned int ii = 0; ii < NW - i - 1; ii++) {
					Wnew[i + ii + rpn_fcn[which].nw] = W[i + ii + 1];
				}
				// Now dump back into W[] 
				NW += rpn_fcn[which].nw - 1;
				for (unsigned int ii = 0; ii < NW; ii++) {
					W[ii] = Wnew[ii];
				}
				i--;		// Must reexamine i-th word 
				break;
			case STRING:
				item.set(W[i], 0.0 , type);
				rS.push_back(item);
				break;
			case NOT_OPERAND:
			default:
				RpnError = BAD_WORD;
				break;
			}
		}
		if (RpnError)
			return RpnError;
	}
	if (rS.size() > 1)
		return STACK_OVERFLOW;
	// If stack is empty, return nothing
	if (rS.size() == 0) {
		return NO_ERROR;
	}
	// Otherwise, save final result into the string 
	switch (TYPE(1)) {
	case NUMBER:
		*result = new char[50];
		sprintf(*result, "%.20g", VALUE(1));
		rS.pop_back();
		break;
	case STRING:
		*result = new char[1 + strlen(NAME(1))];
		strcpy(*result, NAME(1));
		rS.pop_back();
		break;
	case FUNCTION:
		// I think cannot arrive here anyway 
		err("Not allowed to end up with function on stack");
		return GENERAL_ERROR;
	case COLUMN_NAME:
	case NOT_OPERAND:
		break;
	}
	return NO_ERROR;
}

static          operator_name
is_oper(const char *w)
{
	int i;
#if 0
	i = 0;
	while (rpn_dict[i].op_name) {
		if (strlen(rpn_dict[i].op_name) != rpn_dict[i].chars) {
			printf("ERROR in rpn_dict on '%s' ... %d vs %d\n",rpn_dict[i].op_name, strlen(rpn_dict[i].op_name), rpn_dict[i].chars);
		}
		i++;
	}
#endif
	i = 0;
	unsigned int chars_in_w = strlen(w); // checking first speeds a bit
	while (rpn_dict[i].op_name) {
		if (chars_in_w == rpn_dict[i].chars && !strcmp(rpn_dict[i].op_name, w))
			return (operator_name) (rpn_dict[i].op_id);
		i++;
	}
	return NOT_OPERATOR;
}

static          operand_type
is_operand(const char *w, double *operand_value)
{
	double          value;
	if (w[0] == '"' && w[strlen(w) - 1] == '"') {
		return STRING;
	} else if (rpn_which_function(w)) {
		return FUNCTION;
	} else if (!strcmp(w, "x")
		   || !strcmp(w, "y")
		   || !strcmp(w, "z")
		   || !strcmp(w, "u")
		   || !strcmp(w, "v")
		   || !strcmp(w, "grid")) {
		return COLUMN_NAME;
	} else if (getdnum(w, &value)) {	// BUG - if can't scan, will die 
		*operand_value = value;
		return NUMBER;
	} else {
		return NOT_OPERAND;
	}
}

bool
rpn_create_function(char *name, char *w[], unsigned int nw)
{
	unsigned int i;
	rpn_fcn[rpn_fcn_filled].name = new char[1 + strlen(name)];
	if (!rpn_fcn[rpn_fcn_filled].name) OUT_OF_MEMORY;
	strcpy(rpn_fcn[rpn_fcn_filled].name, name);
	if (nw > 0) {
		if (nw >= RPN_W_CAPACITY) {
			fatal_err("internal error: too many words in rpn def");
		}
		for (i = 0; i < nw; i++) {
			rpn_fcn[rpn_fcn_filled].w[i] = new char[1 + strlen(w[i])];
			if (!rpn_fcn[rpn_fcn_filled].w[i]) OUT_OF_MEMORY;
			strcpy(rpn_fcn[rpn_fcn_filled].w[i], w[i]);
		}
	}
	rpn_fcn[rpn_fcn_filled].nw = nw;
	rpn_fcn_filled++;
	return true;
}

// Return 1 + index of function, or 0 if unknown
static unsigned int
rpn_which_function(const char *name)
{
	if (isdigit(name[0]))
		return 0;
	for (unsigned int i = 0; i < rpn_fcn_filled; i++)
		if (!strcmp(rpn_fcn[i].name, name))
			return (i + 1);
	return 0;
}

#define NEED_ON_STACK(num)						\
{									\
    if (rS.size() < (num)) {						\
        RpnError = STACK_UNDERFLOW;					\
        return false;							\
    }									\
}

#define GET_COL_VAL(COL_NAME, I)					\
{									\
if ((COL_NAME).size() <= 0 || (I) > ((COL_NAME).size() - 1)) {  \
   SET(2, "", gr_currentmissingvalue(),NUMBER);				\
} else {								\
   SET(2, "", (COL_NAME)(I), NUMBER);				\
}									\

// Area under y-x curve
 double curve_area()
	 {
		 double sum = 0;
		 int i, n = _colX.size();
		 for (i = 1; i < n; i++) {
			 if (!gr_missingx(_colX[i])
			     && !gr_missingx(_colX[i - 1])
			     && !gr_missingy(_colY[i])
			     && !gr_missingy(_colY[i - 1])) {
				 sum += 0.5 * (_colY[i] + _colY[i - 1]) * (_colX[i] - _colX[i - 1]);
			 }
		 }
		 return sum;
	 }
 
#define GET_GRID_MIN()							\
{									\
unsigned int i, j;							\
bool first = true;							\
double min_val = gr_currentmissingvalue();				\
if (!_grid_exists) {							\
    err("No data in grid yet.");					\
    RpnError = GENERAL_ERROR;						\
    return false;							\
}									\
for (j = 0; j < _num_ymatrix_data; j++) {				\
    for (i = 0; i < _num_xmatrix_data; i++) {				\
        if (inside_box(_xmatrix[i], _ymatrix[j]) && !gr_missing((double)_f_xy(i, j))) { \
            if (first) {						\
                min_val = _f_xy(i, j);					\
                first = false;						\
            } else {							\
                if (_f_xy(i, j) < min_val) {				\
                    min_val = _f_xy(i, j);				\
                }							\
            }								\
        }								\
    }									\
}									\
SET(1, "", min_val, NUMBER);						\
}
 
#define GET_GRID_MAX()							\
{									\
unsigned int i, j;							\
bool first = true;							\
double max_val = gr_currentmissingvalue();				\
if (!_grid_exists) {							\
    err("No data in grid yet.");					\
    RpnError = GENERAL_ERROR;						\
    return false;							\
}									\
for (j = 0; j < _num_ymatrix_data; j++) {				\
    for (i = 0; i < _num_xmatrix_data; i++) {				\
        if (inside_box(_xmatrix[i], _ymatrix[j]) && !gr_missing((double)_f_xy(i, j))) { \
            if (first) {						\
                max_val = _f_xy(i, j);					\
                first = false;						\
            } else {							\
                if (_f_xy(i, j) > max_val) {				\
                    max_val = _f_xy(i, j);				\
                }							\
            }								\
        }								\
    }									\
}									\
SET(1, "", max_val, NUMBER);						\
}

#define GET_GRID_MEAN()							\
{									\
unsigned int i, j;							\
double mean_val = 0.0;							\
int num = 0;								\
if (!_grid_exists) {							\
    err("No data in grid yet.");					\
    RpnError = GENERAL_ERROR;						\
    return false;							\
}									\
for (j = 0; j < _num_ymatrix_data; j++) {				\
    for (i = 0; i < _num_xmatrix_data; i++) {				\
       if (inside_box(_xmatrix[i], _ymatrix[j]) && _legit_xy(i, j)) {   \
            mean_val += _f_xy(i, j);					\
            num++;							\
        }								\
    }									\
}									\
if (num > 0) {								\
    mean_val = mean_val / num;						\
} else {								\
    mean_val = gr_currentmissingvalue();				\
}									\
SET(1, "", mean_val, NUMBER);						\
}

#define GET_GRID_STDDEV()						\
{									\
unsigned int i, j;							\
double stddev_val = 0.0;						\
double mean_val = 0.0;							\
int num = 0;								\
if (!_grid_exists) {							\
    err("No data in grid yet.");					\
    RpnError = GENERAL_ERROR;						\
    return false;							\
}									\
for (j = 0; j < _num_ymatrix_data; j++) {				\
    for (i = 0; i < _num_xmatrix_data; i++) {				\
       if (_legit_xy(i, j)) {			                        \
            mean_val += _f_xy(i, j);					\
            num++;							\
        }								\
    }									\
}									\
if (num > 0) {								\
    mean_val = mean_val / num;						\
    for (j = 0; j < _num_ymatrix_data; j++) {				\
	for (i = 0; i < _num_xmatrix_data; i++) {			\
            if (_legit_xy(i, j)) {		                        \
		stddev_val += (_f_xy(i, j)-mean_val)*(_f_xy(i, j)-mean_val);\
	    }								\
	}								\
    }									\
    if (num > 1) {							\
        stddev_val = sqrt(stddev_val / (num - 1));			\
    } else {								\
        stddev_val = gr_currentmissingvalue();				\
    }									\
} else {								\
    stddev_val = gr_currentmissingvalue();				\
}									\
SET(1, "", stddev_val, NUMBER);						\
}

#define GET_GRID_SIZE()							\
{									\
unsigned int i, j;							\
unsigned int num = 0;							\
if (!_grid_exists) {							\
    err("No data in grid yet.");					\
    RpnError = GENERAL_ERROR;						\
    return false;							\
}									\
for (j = 0; j < _num_ymatrix_data; j++) {				\
    for (i = 0; i < _num_xmatrix_data; i++) {				\
       if (_legit_xy(i, j)) {			                        \
            num++;							\
        }								\
    }									\
}									\
SET(1, "", num, NUMBER);						\
}

static          bool
do_operation(operator_name oper)
{
	//printf("do_operation(%d) vs %d\n",int(oper),int(ARGV));
	if (oper == NOT_OPERATOR) {
		RpnError = BAD_WORD;
		return false;
	}
	double res;			// holds result
	if (oper == ADD) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : VALUE(2) + VALUE(1);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	}
	if (oper == SUBTRACT) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : VALUE(2) - VALUE(1);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == GREATER_THAN) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : (VALUE(1) > VALUE(2) ? 1 : 0);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == GREATER_THAN_EQUAL) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : (VALUE(1) >= VALUE(2) ? 1 : 0);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == EQUAL) {
		NEED_ON_STACK(2);
		if (TYPE(1) == STRING && TYPE(2) == STRING) {
			SET(2, "", !strcmp(NAME(2), NAME(1)) ? 1.0 : 0.0, NUMBER);
			rS.pop_back();
		} else if (TYPE(1) == NUMBER && TYPE(2) == NUMBER) {
			SET(2, "", VALUE(2) == VALUE(1) ? 1.0 : 0.0, NUMBER);
			rS.pop_back();
		} else {
			err("RPN operator `==' cannot handle the items currently on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		}
		return true;
	} 
	if (oper == NOT_EQUAL) {
		NEED_ON_STACK(2);
		if (TYPE(1) == STRING && TYPE(2) == STRING) {
			SET(2, "", !strcmp(NAME(2), NAME(1)) ? 0.0 : 1.0, NUMBER);
			rS.pop_back();
		} else if (TYPE(1) == NUMBER && TYPE(2) == NUMBER) {
			SET(2, "", VALUE(2) != VALUE(1) ? 1.0 : 0.0, NUMBER);
			rS.pop_back();
		} else {
			err("Rpn operator `!=' cannot handle items on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		}
		return true;
	} 
	if (oper == AND) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		SET(2, "", VALUE(2) && VALUE(1) ? 1.0 : 0.0, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == OR) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		SET(2, "", VALUE(2) || VALUE(1) ? 1.0 : 0.0, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == NOT) {
		NEED_ON_STACK(1); NEED_IS_TYPE(1, NUMBER);
		SET(1, "", VALUE(1) ? 0.0 : 1.0, NUMBER);
		return true;
	} 
	if (oper == LESS_THAN) {
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : (VALUE(1) < VALUE(2) ? 1 : 0);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == LESS_THAN_EQUAL) {
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : (VALUE(1) <= VALUE(2) ? 1 : 0);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == MULTIPLY) {
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : VALUE(1) * VALUE(2);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == DIVIDE) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		if (VALUE(1) == 0.0) {
			RpnError = DIV_BY_ZERO;
			rS.pop_back();	// no need, since will die
			return false;
		}
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : VALUE(2) / VALUE(1);
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == POWER) {	// x^p
		// Solve SourceForge bug #113816 for a few legal cases with x<0
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		double x = VALUE(2), p = VALUE(1);
		if (gr_missing(x) || gr_missing(p)) {
			SET(2, "", gr_currentmissingvalue(), NUMBER);
			rS.pop_back();
			return true;
		}
		if (x == 0.0) {	// I bet pow() is ok on zero, but let's be safe
			SET(2, "", 0.0, NUMBER);
			rS.pop_back();
			return true;
		}
		if (x > 0.0) {
			SET(2, "", pow(x, p), NUMBER);
			rS.pop_back();
			return true;
		}
		// If x<0 and p is even integer, ok;
		// If x<0 and p is  odd integer, ok;
		// Otherwise, we're out of luck.
		if (x < 0.0) {
			if (is_even_integer(p)) {
				SET(2, "", pow(-x, p), NUMBER);
				rS.pop_back();
				return true;
			} else if (is_odd_integer(p)) {
				SET(2, "", -pow(-x, p), NUMBER);
				rS.pop_back();
				return true;
			} else {
				RpnError = NEED_GE_0;
				rS.pop_back();	// no need, since will die
				return false;
			}
		}
		// Cannot get here.
		return true;
	}
	if (oper == ACOSINE) {
		NEED_IS_TYPE(1, NUMBER);
		if (VALUE(1) > 1.0 || VALUE(1) < -1.0) {
			RpnError = RANGE_1;
			return false;
		}
		res = gr_missing(VALUE(1)) ? gr_currentmissingvalue() 
			: acos(VALUE(1)) * deg_per_rad;
		SET(1, "", res, NUMBER);
		return true;
	}
	if (oper == ASINE) {
		NEED_IS_TYPE(1, NUMBER);
		if (VALUE(1) > 1.0 || VALUE(1) < -1.0 || gr_missing(VALUE(1))) {
			RpnError = RANGE_1;
			return false;
		}
		res = gr_missing(VALUE(1)) ? gr_currentmissingvalue() 
			: asin(VALUE(1)) * deg_per_rad;
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == ATANGENT) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() 
			: atan(VALUE(1)) * deg_per_rad;
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == SINE) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : sin(VALUE(1) / deg_per_rad);
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == COSINE) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : cos(VALUE(1) / deg_per_rad);
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == TANGENT) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : tan(VALUE(1) / deg_per_rad);
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == SINH) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : sinh(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == ACOSH) {
		NEED_IS_TYPE(1, NUMBER);
		if (VALUE(1) < 1.0) {
			RpnError = NEED_GT_1;
			return false;
		}
#if defined(HAVE_ACOSH)
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : acosh(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
#else
		RpnError = COMPUTER_LIMITATION;
		return false;
#endif
	} 
	if (oper == ATANH) {
		NEED_IS_TYPE(1, NUMBER);
		if (VALUE(1) > 1.0 || VALUE(1) < -1.0) {
			RpnError = RANGE_1;
			return false;
		}
#if defined(HAVE_ACOSH)
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : atanh(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
#else
		RpnError = COMPUTER_LIMITATION;
		return false;
#endif
	} 
	if (oper == ASINH) {
		NEED_IS_TYPE(1, NUMBER);
#if defined(HAVE_ACOSH)
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : asinh(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
#else
		RpnError = COMPUTER_LIMITATION;
		return false;
#endif
	} 
	if (oper == COSH) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : cosh(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == TANH) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : tanh(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	}
	if (oper == SQRT) {
		NEED_IS_TYPE(1, NUMBER);
		if (gr_missing(VALUE(1))) {
			SET(1, "", gr_currentmissingvalue(), NUMBER);
			return true;
		}
		if (VALUE(1) < 0.0) {
			RpnError = NEED_GE_0;
			return false;
		}
		res = sqrt(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	}
	if (oper == LOG) {
		NEED_IS_TYPE(1, NUMBER);
		if (gr_missing(VALUE(1))) {
			SET(1, "", gr_currentmissingvalue(), NUMBER);
			return true;
		}
		if (VALUE(1) <= 0.0) {
			RpnError = NEED_GT_0;
			return false;
		}
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : log10(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	}
	if (oper == LN) {
		NEED_IS_TYPE(1, NUMBER);
		if (gr_missing(VALUE(1))) {
			SET(1, "", gr_currentmissingvalue(), NUMBER);
			return true;
		}
		if (VALUE(1) <= 0) {
			RpnError = NEED_GT_0;
			return false;
		}
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : log(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == EXP) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : pow(Ee, VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == EXP10) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : pow(10.0, VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == FLOOR) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : floor(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == REMAINDER) {
		NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ?
			gr_currentmissingvalue() : fmod(VALUE(2), VALUE(1));
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == CEIL) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : ceil(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == ABS) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : fabs(VALUE(1));
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == CMTOPT) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : VALUE(1) * PT_PER_CM;
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == PTTOCM) {
		NEED_IS_TYPE(1, NUMBER);
		res = gr_missing(VALUE(1)) ?
			gr_currentmissingvalue() : VALUE(1) / PT_PER_CM;
		SET(1, "", res, NUMBER);
		return true;
	} 
	if (oper == DUP) {
		RpnItem item;
		item.set(NAME(1), VALUE(1), TYPE(1));
		rS.push_back(item);
		return true;
	} 
	if (oper == POP) {
		if (rS.size() < 2) {
			warning("An 'RPN' pop is leaving a blank stack.  Do you want this?");
		}
		rS.pop_back();
		return true;
	} 
	if (oper == EXCH) {
		NEED_ON_STACK(2);
		RpnItem old;		// cannot do old=... here
		old = rS[rS.size() - 1];
		rS[rS.size() - 1] = rS[rS.size() - 2];
		rS[rS.size() - 2] = old;
		return true;
	} 
	if (oper == ROLL_LEFT) {
		NEED_ON_STACK(2);
		RpnItem         old;	// cannot do old=... here
		old = rS[0];
		for (unsigned int i = 0; i < rS.size() - 1; i++) 
			rS[i] = rS[i + 1];	
		rS[rS.size() - 1] = old;
		return true;
	}
	if (oper == ROLL_RIGHT) {
		NEED_ON_STACK(2);
		RpnItem         old;	// cannot do old=... here
		old = rS[rS.size() - 1];
		for (int i = rS.size() - 1; i > 0; i--)
			rS[i] = rS[i - 1];
		rS[0] = old;
		return true;
	}
	if (oper == PSTACK) {
		return print_rpn_stack();
	} 
	if (oper == STRLEN) {
		NEED_ON_STACK(1); NEED_IS_TYPE(1, STRING);
		SET(1, "", double(strlen(NAME(1))), NUMBER);
		return true;
	}
	if (oper == SUBSTR) {
		NEED_ON_STACK(3);
		NEED_IS_TYPE(1, STRING);
		NEED_IS_TYPE(2, NUMBER);
		NEED_IS_TYPE(3, NUMBER);
		std::string s(NAME(1)), ss;
		un_double_quote(s);
		int stop  = int(VALUE(2));
		int start = int(VALUE(3));
		if (stop < 0 || start < 0) {
			RpnError = NEED_GE_0;
			return false;
		}
		rS.pop_back();
		rS.pop_back();
		rS.pop_back();
		RpnItem item;
		ss = "\"";
		ss.append(s.substr(start, stop));
		ss.append("\"");
		item.set(ss.c_str(), 0.0, STRING);
		rS.push_back(item);
		return true;
 	}
	if (oper == STRCAT) {
		// Need to remove the last quote (") of first and first quote of second.
		NEED_ON_STACK(2); NEED_IS_TYPE(1, STRING); NEED_IS_TYPE(2, STRING);
		std::string res(NAME(2));
		res.STRINGERASE(res.size()-1, 1);
		res.append(NAME(1) + 1);
		SET(2, res.c_str(), 0.0, STRING);
		rS.pop_back();
		return true;
	} 
	if (oper == ATOF) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, STRING);
		double tmp;
		sscanf(NAME(1), "\"%lf\"", &tmp);
		SET(1, "", tmp, NUMBER);
		return true;
	}
	if (oper == SYSTEM) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, STRING);
#if !defined(HAVE_POPEN)
		err("This computer can't do `system' in RPN, since no popen() subroutine.");
		return false;
#else
		{
			char *output_lines = new char[LineLength];
			if (!output_lines) OUT_OF_MEMORY;
			char *thisline = new char[LineLength];
			if (!thisline) OUT_OF_MEMORY;
			FILE           *pipefile;
			// double duty for this 
			strcpy(output_lines, 1 + NAME(1));
			if (*STRING_END(output_lines) == '"')
				*STRING_END(output_lines) = '\0';
			pipefile = (FILE *) popen(output_lines, "r");
			if (!pipefile) {
				err("Sorry, cannot do `system' in RPN; failed popen() call");
				return false;
			}
			strcpy(output_lines, "");
			// BUG -- should check for overwrite! 
			while (NULL != fgets(thisline, LineLength_1, pipefile))
				strcat(output_lines, thisline);
			pclose(pipefile);
			if (*STRING_END(output_lines) == '\n') {
				*STRING_END(output_lines) = '\0';
			}
			std::string tmp("\"");
			tmp.append(output_lines);
			tmp.append("\"");
			SET(1, tmp.c_str(), 0.0, STRING);
			delete [] thisline;
			delete [] output_lines;
		}
		return true;
#endif
	} 
	if (oper == SUP) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ? 
			gr_currentmissingvalue() 
			: (VALUE(1) > VALUE(2) ? VALUE(1) : VALUE(2));
		SET(2, "", res, NUMBER);
		rS.pop_back();
		return true;
	} 
	if (oper == INF) {
		NEED_ON_STACK(2); NEED_IS_TYPE(1, NUMBER); NEED_IS_TYPE(2, NUMBER);
		res = (gr_missing(VALUE(1)) || gr_missing(VALUE(2))) ? 
			gr_currentmissingvalue() 
			: (VALUE(1) < VALUE(2) ? VALUE(1) : VALUE(2));
		SET(2, "", res, NUMBER);
		rS.pop_back();
	        return true;
	}
	if (oper == ASSIGN) {
		// {rpn 10 ".a." =} # assigns 10 to the variable named ".a."
		NEED_ON_STACK(2);
		NEED_IS_TYPE(1, STRING);
		char *unadorned = new char[1 + strlen(NAME(1))];
		if (!unadorned) OUT_OF_MEMORY;
		strcpy(unadorned, 1 + NAME(1));
		// Remove quote (check, although MUST be there 
		if (*STRING_END(unadorned) == '"')
			*STRING_END(unadorned) = '\0';
		//printf("%s:%d assigning to [%s]\n",__FILE__,__LINE__,NAME(1));
		switch (TYPE(2)) {
		case NUMBER:
			if (is_var(unadorned)) {
				PUT_VAR(unadorned, VALUE(2));
				//printf("%s:%d debug\n",__FILE__,__LINE__);
			} else {
				//printf("%s:%d [%s]\n",__FILE__,__LINE__,unadorned);
				//printf("%s:%d [%s]\n",__FILE__,__LINE__,NAME(1));
				err("Invalid variable name `\\", unadorned, "' in assignment", "\\");
				RpnError = ILLEGAL_TYPE;
				return false;
			}
			break;
		case STRING:
			if (*(1 + unadorned) == '\\') {
				char *s = new char[1 + strlen(NAME(2))];
				if (!s) OUT_OF_MEMORY;
				strcpy(s, 1 + NAME(2));
				if (s[strlen(s) - 1] == '"')
					s[strlen(s) - 1] = '\0';
				if (!put_syn(1 + unadorned, s, true)) OUT_OF_MEMORY;
				delete [] s;
			} else {
				err("Invalid synonym name in assignment");
				RpnError = ILLEGAL_TYPE;
				return false;
			}
			break;
		default:
			err("RPN operator `=' cannot handle the items currently on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		}
		rS.pop_back();
		rS.pop_back();
		delete [] unadorned;
		return true;
	} 
	if (oper == XYCMTOUSER) {	// should check for missingvalue?
		NEED_IS_TYPE(2, NUMBER);
		set_x_scale();
		set_y_scale();
		double          tmpy, tmpx;
		gr_cmtouser(VALUE(2), VALUE(1), &tmpx, &tmpy);
		SET(2, "", tmpx, NUMBER);
		SET(1, "", tmpy, NUMBER);
		return true;
	} 
	if (oper == XYUSERTOCM) {	// should check for missingvalue?
		NEED_IS_TYPE(2, NUMBER);
		set_x_scale();
		set_y_scale();
		double          tmpy, tmpx;
		gr_usertocm(VALUE(2), VALUE(1), &tmpx, &tmpy);
		SET(2, "", tmpx, NUMBER);
		SET(1, "", tmpy, NUMBER);
		return true;
	} 
	if (oper == XCMTOUSER) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_x_scale();
		double          tmpy, tmpx;
		gr_cmtouser(VALUE(1), 1.0, &tmpx, &tmpy);
		SET(1, "", tmpx, NUMBER);
		return true;
	} 
	if (oper == XPTTOUSER) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_x_scale();
		double          tmpy, tmpx;
		gr_cmtouser(VALUE(1) / PT_PER_CM, 1.0, &tmpx, &tmpy);
		SET(1, "", tmpx, NUMBER);
		return true;
	} 
	if (oper == XUSERTOCM) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_x_scale();
		double          tmpy, tmpx;
		gr_usertocm(VALUE(1), 1.0, &tmpx, &tmpy);
		SET(1, "", tmpx, NUMBER);
		return true;
	} 
	if (oper == XUSERTOPT) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_x_scale();
		double          tmpy, tmpx;
		gr_usertopt(VALUE(1), 1.0, &tmpx, &tmpy);
		SET(1, "", tmpx, NUMBER);
		return true;
	} 
	if (oper == YUSERTOCM) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_y_scale();
		double          tmpy, tmpx;
		gr_usertocm(1.0, VALUE(1), &tmpx, &tmpy);
		SET(1, "", tmpy, NUMBER);
		return true;
	} 
	if (oper == YUSERTOPT) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_y_scale();
		double          tmpy, tmpx;
		gr_usertopt(1.0, VALUE(1), &tmpx, &tmpy);
		SET(1, "", tmpy, NUMBER);
		return true;
	} 
	if (oper == YCMTOUSER) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_y_scale();
		double          tmpy, tmpx;
		gr_cmtouser(1.0, VALUE(1), &tmpx, &tmpy);
		SET(1, "", tmpy, NUMBER);
		return true;
	} 
	if (oper == YPTTOUSER) {	// should check for missingvalue?
		NEED_IS_TYPE(1, NUMBER);
		set_y_scale();
		double          tmpy, tmpx;
		gr_cmtouser(1.0, VALUE(1) / PT_PER_CM, &tmpx, &tmpy);
		SET(1, "", tmpy, NUMBER);
		return true;
	}
	if (oper == STRINGWIDTH) {
		double          width, ascent, descent;	// in cm 
		NEED_ON_STACK(1);
		if (TYPE(1) != STRING) {
			err("RPN string operator `width' needs a string to be top item on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		} else {
			double          fontsize;
			gr_fontID       old_font;
			old_font = gr_currentfont();
			if (!get_var("..fontsize..", &fontsize))
				warning("(rpn width), ..fontsize.. undefined so using 12");
			gr_setfontsize_pt(fontsize);
			gr_setfont(old_font);
			std::string       no_quotes(NAME(1));
			un_double_quote(no_quotes);
			gr_stringwidth(no_quotes.c_str(), &width, &ascent, &descent);
			SET(1, "", width, NUMBER);
		}
		return true;
	}
	if (oper == STRINGASCENT) {
		double          width, ascent, descent;	// in cm 
		NEED_ON_STACK(1);
		if (TYPE(1) != STRING) {
			err("RPN string operator `ascent' needs a string to be top item on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		} else {
			double          fontsize;
			gr_fontID       old_font;
			old_font = gr_currentfont();
			if (!get_var("..fontsize..", &fontsize))
				warning("(rpn width), ..fontsize.. undefined so using 12");
			gr_setfontsize_pt(fontsize);
			gr_setfont(old_font);
			std::string       no_quotes(NAME(1));
			un_double_quote(no_quotes);
			gr_stringwidth(no_quotes.c_str(), &width, &ascent, &descent);
			SET(1, "", ascent, NUMBER);
		}
		return true;
	}
	if (oper == STRINGDESCENT) {
		double          width, ascent, descent;	// in cm 
		NEED_ON_STACK(1);
		if (TYPE(1) != STRING) {
			err("RPN string operator `descent' needs a string to be top item on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		} else {
			double          fontsize;
			gr_fontID       old_font;
			old_font = gr_currentfont();
			if (!get_var("..fontsize..", &fontsize))
				warning("(rpn width), ..fontsize.. undefined so using 12");
			gr_setfontsize_pt(fontsize);
			gr_setfont(old_font);
			std::string       no_quotes(NAME(1));
			un_double_quote(no_quotes);
			gr_stringwidth(no_quotes.c_str(), &width, &ascent, &descent);
			SET(1, "", descent, NUMBER);
		}
		return true;
	}
	if (oper == DIRECTORY_EXISTS) {
		NEED_ON_STACK(1);
		if (TYPE(1) != STRING) {
			err("RPN string operator `directory_exists' needs a string to be top item on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		} else {
#if defined(HAVE_ACCESS)
			std::string fname(NAME(1));
			un_double_quote(fname);
			if (fname[0] == '~') {
				fname.STRINGERASE(0, 1);
				std::string home(egetenv("HOME"));
				home.append(fname);
				fname = home;
			}
			//printf("DEBUG: should check if file named '%s' or '%s' exists\n",NAME(1),fname.c_str());
			if (0 == access(fname.c_str(), R_OK | X_OK))
				SET(1, "", 1.0, NUMBER);
			else
				SET(1, "", 0.0, NUMBER);
#else
			warning("Can't determine whether directory exists (no 'access' subroutine on this system) so guessing answer is yes.");
			SET(1, "", 1.0, NUMBER);
#endif
		}
		return true;
	}
	if (oper == WORDC) {
		extern int      _num_command_word;
		extern char    *_command_word[MAX_cmd_word];
		extern char    *_command_word_separator;
		int cmd;
		for (cmd = _num_command_word - 1; cmd > -1; cmd--)
			if (!strcmp(_command_word[cmd], _command_word_separator))
				break;
		//printf("DEBUG cmd %d  num %d   stacksize %d\n",cmd,_num_command_word,rS.size());
		RpnItem item;
		if (cmd > -1)
			item.set("", double(_num_command_word - cmd - 1), NUMBER);
		else 
			item.set("", 0.0, NUMBER);
		rS.push_back(item);
		return true;
	}
	if (oper == WORDV) {
		if (rS.size() < 1) {
			err("`wordv' needs an argument, e.g. {rpn 0 wordv} gives first word of command\n.");
			return false;
		}
		NEED_IS_TYPE(1, NUMBER);
		int index = int(VALUE(1));
		if (index < 0) {
			printf("`wordv' needs index >= 0\n");
			RpnError = NEED_GT_1;
			return false;
		}
		extern int      _num_command_word;
		extern char    *_command_word[MAX_cmd_word];
		extern char    *_command_word_separator;
		int cmd;
		// Trace back through the stack until at next level deep, then
		// move forward to indicated word.
		for (cmd = _num_command_word - 1; cmd > -1; cmd--) {
			//printf("\t%d of %d <%s>\n",cmd,_num_command_word,_command_word[cmd]);
			if (!strcmp(_command_word[cmd], _command_word_separator))
				break;
		}
		//printf("cmd is %d max is %d ... value '%s'\n",cmd,_num_command_word,_command_word[cmd+index+1]);
		std::string rv;
		if (*_command_word[cmd + index + 1] == '\"') { 
			rv.append(_command_word[cmd + index + 1]);
		} else {
			rv.append("\"");
			rv.append(_command_word[cmd + index + 1]);
			rv.append("\"");
		}
		SET(1, rv.c_str(), 0.0, STRING);
		//printf("\t\trv is '%s'\n",rv.c_str());
		return true;
	}
	if (oper == ARGC) {
		extern std::vector<const char*>_gri_argv;
		RpnItem item;
		item.set("", double(_gri_argv.size()), NUMBER);
		rS.push_back(item);
		return true;
	}
	if (oper == ARGV) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, NUMBER);
		int index = int(VALUE(1));
		if (index < 0) {
			printf("'argv' needs index >= 0\n");
			RpnError = NEED_GT_1;
			return false;
		}
		extern std::vector<const char*>_gri_argv;
		if (index >= int(_gri_argv.size())) {
			SET(1, "\" \"", 0.0, STRING);
			return true;
		}
		std::string rv("\"");
		rv.append(_gri_argv[index]);
		rv.append("\"");
		SET(1, rv.c_str(), 0.0, STRING);
		return true;
	} 
	if (oper == FILE_EXISTS) {
		NEED_ON_STACK(1);
		if (TYPE(1) != STRING) {
			err("RPN string operator `file_exists' needs a string to be top item on stack.");
			RpnError = ILLEGAL_TYPE;
			return false;
		} else {
#if defined(HAVE_ACCESS)
			std::string fname(NAME(1));
			un_double_quote(fname);
			if (fname[0] == '~') {
				fname.STRINGERASE(0, 1);
				std::string home(egetenv("HOME"));
				home.append(fname);
				fname = home;
			}
			//printf("DEBUG: should check if file named '%s' or '%s' exists\n",NAME(1),fname.c_str());
			if (0 == access(fname.c_str(), R_OK))
				SET(1, "", 1.0, NUMBER);
			else
				SET(1, "", 0.0, NUMBER);
#else
			warning("Can't determine whether file exists (no 'access' subroutine on this system) so guessing answer is yes.");
			SET(1, "", 1.0, NUMBER);
#endif
		}
		return true;
	}
	if (oper == DEFINED) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, STRING);
		std::string n1(NAME(1));
		un_double_quote(n1);
		// It's either a synonym or a variable, or not defined 
		if (is_syn(n1)) {
			//printf("\n");
			//printf("DEBUG %s:%d defined on <%s>\n",__FILE__,__LINE__,n1.c_str());
			int w_index = -1;
			if (1 == sscanf(n1.c_str(), "\\.word%d.", &w_index)) {
				std::string w("");
				if (get_cmdword(w_index, w)) {
					// If such a \.word?. exists, look up pointed-to item
					//printf("DEBUG %s:%d w= <%s>\n",__FILE__,__LINE__,w.c_str());
					std::string coded_name;
					int coded_level;
					if (is_coded_string(w, coded_name, &coded_level)) {
						//printf("DEBUG %s:%d encoded `%s' at level %d\n",__FILE__,__LINE__, coded_name.c_str(), coded_level);
						std::string value;
						if (get_coded_value(coded_name, coded_level, value)) {
							//printf(" ** YES [%s] is defined\n", coded_name.c_str());
							SET(1, "", 1.0, NUMBER);
						} else {
							//printf(" ** NO [%s] is NOT defined\n", coded_name.c_str());
							SET(1, "", 0.0, NUMBER);
						}							
					} else {
						// Nothing pointed-to, so \.word?. existence enough
						SET(1, "", 1.0, NUMBER);
					}
				} else {
					// If no such \.word?., certainly nothing pointed-to.
					SET(1, "", 0.0, NUMBER);
				}
			} else {
				//printf("CASE 2.  n1 is [%s]\n",n1.c_str());
				bool exists;
				std::string syn_value; // not used, actually
				if (n1[1] == '@') {
					std::string d("\\");
					d.append(n1.substr(2, n1.size()));
					exists = get_syn(d.c_str(), syn_value);
					//printf("CASE 2B    d is [%s] returning %d\n",d.c_str(),exists);
				} else {
					exists = get_syn(n1.c_str(), syn_value);
					//printf("CASE 2B.  n1 is [%s] returning %d\n",n1.c_str(),exists);
				}
				if (exists)
					SET(1, "", 1.0, NUMBER);
				else
					SET(1, "", 0.0, NUMBER);
			}
		} else if (is_var(n1)) {
			double tmp;
			bool exists = get_var(n1.c_str(), &tmp);
			if (exists)
				SET(1, "", 1.0, NUMBER);
			else
				SET(1, "", 0.0, NUMBER);
		} else {
			err("Can only use `defined' on a variable or synonym (e.g., `.var.' or `\\syn'), not on `\\", NAME(1), "' as found", "\\");
			RpnError = ILLEGAL_TYPE;
			return false;
		}
		return true;
	} 
	if (oper == ISMISSING) {
		NEED_IS_TYPE(1, NUMBER);
		SET(1, "", gr_missing(VALUE(1)) == true ? 1.0 : 0.0, NUMBER);
		return true;
	}
	if (oper == INTERPOLATE) {
		// Next 2 functions in convert.cc
		NEED_ON_STACK(3);
		NEED_IS_TYPE(3, COLUMN_NAME); // must be `grid', actually
		NEED_IS_TYPE(2, NUMBER);      // x
		NEED_IS_TYPE(1, NUMBER);      // y
		int i, j;
		double x = VALUE(2), y = VALUE(1), grid_value;
		if (!locate_i_j(x, y, &i, &j)) {
			SET(3, "", gr_currentmissingvalue(), NUMBER);
		} else {
#if defined(OLD_IMAGE_INTERPOLATION)
			value_i_j(i, j, x, y, &grid_value);
#else
			value_i_j(i, j, x, y, &grid_value);
#endif
			SET(3, "", grid_value, NUMBER);
		}
		rS.pop_back();
		rS.pop_back();
		return true;
	}
	if (oper == RAND) {
		RpnItem item;
#if defined(HAVE_DRAND48)
		item.set("", drand48(), NUMBER);
#else
		item.set("", rand(), NUMBER);
#endif
		rS.push_back(item);
		return true;
	}

	if (oper == VAL) {
		extern char     _grTempString[];
		NEED_ON_STACK(2);
		NEED_IS_TYPE(2, COLUMN_NAME);
		int index = (int) (floor(0.5 + VALUE(1)));
		if (index < 0) {
			err("Can't take negative index of the `\\",
			    NAME(1),
			    "' column.",
			    "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		if (!strcmp(NAME(2), "x")) {
			if (index > int(_colX.size() - 1)) {
				sprintf(_grTempString, "Cannot index %d-th value of x column; valid range is 0 to %d", index, int(_colX.size() - 1));
				err(_grTempString);
				RpnError = GENERAL_ERROR;
				return false;
			}
			SET(2, "", _colX[index], NUMBER);
			rS.pop_back();
		} else if (!strcmp(NAME(2), "y")) {
			if (index > int(_colY.size() - 1)) {
				sprintf(_grTempString, "Cannot index %d-th value of y column; valid range is 0 to %d", index, int(_colY.size() - 1));
				err(_grTempString);
				RpnError = GENERAL_ERROR;
				return false;
			}
			SET(2, "", _colY[index], NUMBER);
			rS.pop_back();
		} else if (!strcmp(NAME(2), "z")) {
			if (index > int(_colZ.size() - 1)) {
				sprintf(_grTempString, "Cannot index %d-th value of z column; valid range is 0 to %d", index, int(_colZ.size() - 1));
				err(_grTempString);
				RpnError = GENERAL_ERROR;
				return false;
			}
			SET(2, "", _colZ[index], NUMBER);
			rS.pop_back();
		} else if (!strcmp(NAME(2), "u")) {
			if (index > int(_colU.size() - 1)) {
				sprintf(_grTempString, "Cannot index %d-th value of u column; valid range is 0 to %d", index, int(_colU.size() - 1));
				err(_grTempString);
				RpnError = GENERAL_ERROR;
				return false;
			}
			SET(2, "", _colU[index], NUMBER);
			rS.pop_back();
		} else if (!strcmp(NAME(2), "v")) {
			if (index > int(_colV.size() - 1)) {
				sprintf(_grTempString, "Cannot index %d-th value of v column; valid range is 0 to %d", index, int(_colV.size() - 1));
				err(_grTempString);
				RpnError = GENERAL_ERROR;
				return false;
			}
			SET(2, "", _colV[index], NUMBER);
			rS.pop_back();
		} else if (!strcmp(NAME(2), "weight")) {
			if (index > int(_colWEIGHT.size() - 1)) {
				sprintf(_grTempString, "Cannot index %d-th value of weight column; valid range is 0 to %d", index, int(_colWEIGHT.size() - 1));
				err(_grTempString);
				RpnError = GENERAL_ERROR;
				return false;
			}
			SET(2, "", _colWEIGHT[index], NUMBER);
			rS.pop_back();

		} else {
			err("Column `\\", NAME(2), "' is invalid", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	}
	if (oper == AREA) {
		NEED_ON_STACK(2);
		NEED_IS_TYPE(1, COLUMN_NAME);
		NEED_IS_TYPE(2, COLUMN_NAME);
		if (!(!strcmp(NAME(2), "y") && !strcmp(NAME(1), "x"))) {
			err("To get area under curve, must use syntax `y x area'");
			RpnError = GENERAL_ERROR;
			return false;
		}
		SET(2, "", curve_area(), NUMBER);
		rS.pop_back();
		return true;
	}
	if (oper == MIN) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, COLUMN_NAME);
		if (!strcmp(NAME(1), "x")) {
			SET(1, "", _colX.min(), NUMBER);
		} else if (!strcmp(NAME(1), "y")) {
			SET(1, "", _colY.min(), NUMBER);
		} else if (!strcmp(NAME(1), "z")) {
			SET(1, "", _colZ.min(), NUMBER);
		} else if (!strcmp(NAME(1), "u")) {
			SET(1, "", _colU.min(), NUMBER);
		} else if (!strcmp(NAME(1), "v")) {
			SET(1, "", _colV.min(), NUMBER);
		} else if (!strcmp(NAME(1), "grid")) {
			GET_GRID_MIN();
		} else {
			err("Can't find min of item `\\", NAME(1), "'", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	}
	if (oper == MAX) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, COLUMN_NAME);
		if (!strcmp(NAME(1), "x")) {
			SET(1, "", _colX.max(), NUMBER);
		} else if (!strcmp(NAME(1), "y")) {
			SET(1, "", _colY.max(), NUMBER);
		} else if (!strcmp(NAME(1), "z")) {
			SET(1, "", _colZ.max(), NUMBER);
		} else if (!strcmp(NAME(1), "u")) {
			SET(1, "", _colU.max(), NUMBER);
		} else if (!strcmp(NAME(1), "v")) {
			SET(1, "", _colV.max(), NUMBER);
		} else if (!strcmp(NAME(1), "grid")) {
			GET_GRID_MAX();
		} else {
			err("Can't find max of item `\\", NAME(1), "'", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	} 
	if (oper == MEDIAN) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, COLUMN_NAME);
		if (!strcmp(NAME(1), "x")) {
			SET(1, "", _colX.median(), NUMBER);
		} else if (!strcmp(NAME(1), "y")) {
			SET(1, "", _colY.median(), NUMBER);
		} else if (!strcmp(NAME(1), "z")) {
			SET(1, "", _colZ.median(), NUMBER);
		} else if (!strcmp(NAME(1), "u")) {
			SET(1, "", _colU.median(), NUMBER);
		} else if (!strcmp(NAME(1), "v")) {
			SET(1, "", _colV.median(), NUMBER);
		} else if (!strcmp(NAME(1), "grid")) {
			err("This version of Gri cannot do 'median' of grid yet.");
			RpnError = GENERAL_ERROR;
			return false;
		} else {
			err("Column `\\", NAME(1), "' is invalid", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	}
	if (oper == MEAN) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, COLUMN_NAME);
		if (!strcmp(NAME(1), "x")) {
			SET(1, "", _colX.mean(), NUMBER);
		} else if (!strcmp(NAME(1), "y")) {
			SET(1, "", _colY.mean(), NUMBER);
		} else if (!strcmp(NAME(1), "z")) {
			SET(1, "", _colZ.mean(), NUMBER);
		} else if (!strcmp(NAME(1), "u")) {
			SET(1, "", _colU.mean(), NUMBER);
		} else if (!strcmp(NAME(1), "v")) {
			SET(1, "", _colV.mean(), NUMBER);
		} else if (!strcmp(NAME(1), "grid")) {
			GET_GRID_MEAN();
		} else {
			err("Column `\\", NAME(1), "' is invalid", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	}
	if (oper == STDDEV) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, COLUMN_NAME);
		if (!strcmp(NAME(1), "x")) {
			SET(1, "", _colX.stddev(), NUMBER);
		} else if (!strcmp(NAME(1), "y")) {
			SET(1, "", _colY.stddev(), NUMBER);
		} else if (!strcmp(NAME(1), "z")) {
			SET(1, "", _colZ.stddev(), NUMBER);
		} else if (!strcmp(NAME(1), "u")) {
			SET(1, "", _colU.stddev(), NUMBER);
		} else if (!strcmp(NAME(1), "v")) {
			SET(1, "", _colV.stddev(), NUMBER);
		} else if (!strcmp(NAME(1), "grid")) {
			GET_GRID_STDDEV();
		} else {
			err("Column `\\", NAME(1), "' is invalid", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	}
	if (oper == SIZE) {
		NEED_ON_STACK(1);
		NEED_IS_TYPE(1, COLUMN_NAME);
		if (!strcmp(NAME(1), "x")) {
			SET(1, "", _colX.size_legit(), NUMBER);
		} else if (!strcmp(NAME(1), "y")) {
			SET(1, "", _colY.size_legit(), NUMBER);
		} else if (!strcmp(NAME(1), "z")) {
			SET(1, "", _colZ.size_legit(), NUMBER);
		} else if (!strcmp(NAME(1), "u")) {
			SET(1, "", _colU.size_legit(), NUMBER);
		} else if (!strcmp(NAME(1), "v")) {
			SET(1, "", _colV.size_legit(), NUMBER);
		} else if (!strcmp(NAME(1), "grid")) {
			GET_GRID_SIZE();
		} else {
			err("Column `\\", NAME(1), "' is invalid", "\\");
			RpnError = GENERAL_ERROR;
			return false;
		}
		return true;
	}
	gr_Error("Internal error: should not be able to get to\n    this line.  Please email bug report to author");
	return true;
}

// Used by rpn.cc also, in case of stack overflow
bool
print_rpn_stack(const char *msg="")
{
	int             i;
	int             stack_len = rS.size();
	if (strlen(msg) > 0) 
		ShowStr(msg);
	ShowStr("Operands on rpn stack: (");
	for (i = 0; i < stack_len; i++) {
		char            str[100];
		if (TYPE(stack_len - i) == NUMBER) {
			sprintf(str, "%.20g", VALUE(stack_len - i));
			ShowStr(str);
		} else {
			ShowStr(NAME(i));
		}
		if (i != (stack_len - 1))
			ShowStr(", ");
	}
	ShowStr(")\n");
	return true;
}
#undef NEED_IS_TYPE
#undef NEED_ON_STACK
#undef GET_COL_VAL
#undef GET_COL_MIN
#undef GET_COL_MAX
#undef GET_COL_MEAN
#undef GET_COL_STDDEV
#undef VALUE
#undef NAME
#undef VALUE
#undef TYPE
