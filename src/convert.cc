#include <string>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stddef.h>

#include "gr.hh"
#include "extern.hh"
#include "image_ex.hh"
#include "defaults.hh"
#include "GriTimer.hh"
#include "GMatrix.hh"
extern char     _grTempString[];

#define USE_APPROX_EXP

#ifdef USE_APPROX_EXP
// Compute exp(-x) approximately, as efficiency measure.
// See [97/1/25] for demonstration of factor of 3 speedup, with
// 1000 column data and a 10 by 10 grid, and demonstration
// that error is < 0.1% in the final grid.
inline double exp_approx(double x)
{
	return 1.0
		/ (0.999448 
		   + x * (1.023820 
			  + x * (0.3613967
				 + x * (0.4169646
					+ x * (-0.1292509
					       + x * 0.0499565)))));
}
#endif

#if 0
static void display_f_xy(const char *msg);
#endif

static bool  create_grid_barnes(double xr,
				double yr,
				double gamma,
				unsigned int iter,
				const std::vector<double> &xgood,
				const std::vector<double> &ygood,
				const std::vector<double> &zgood,
				const std::vector<double> &wgood);
static bool create_grid_barnes_cv(double xr,
				  double yr,
				  double gamma,
				  unsigned int iter,
				  const std::vector<double> &xgood,
				  const std::vector<double> &ygood,
				  const std::vector<double> &zgood,
				  const std::vector<double> &wgood);
static unsigned int create_grid_objectiveCmd(double xr,
					     double yr,
					     const std::vector<double> &xgood,
					     const std::vector<double> &ygood,
					     const std::vector<double> &zgood);

static double interpolate_barnes(double xx,
				 double yy,
				 double zz,
				 int skip,
				 unsigned int n_k,
				 const std::vector<double> &x,
				 const std::vector<double> &y,
				 const std::vector<double> &z,
				 const std::vector<double> &weight,
				 const std::vector<double> &z_last,
				 double xr,
				 double yr);

static int create_grid_boxcarCmd(double xr,
				 double yr,
				 const std::vector<double> &x,
				 const std::vector<double> &y,
				 const std::vector<double> &z);

static int create_grid_neighborCmd(const std::vector<double> &xgood,
				   const std::vector<double> &ygood,
				   const std::vector<double> &zgood);

int convert_col_to_gridCmd(void);
bool convert_col_to_splineCmd(void);
bool convert_grid_to_columnsCmd(void);
bool convert_grid_to_imageCmd(void);
bool convert_image_to_gridCmd(void);

bool image_range_exists(void);
bool locate_i_j(double xx, double yy, int *ii, int *jj);
int number_good_xyz(double x[], double y[], double f[], int n);
bool value_i_j(unsigned int ii, unsigned int jj, double xx, double yy, double *value);


// Spline things
static inline double  dmin(double a, double b);

int interv(double *xt, int *lxt, double *x, int *left, int *mflag);

double ppvalu(double *break_, double *coef, int *l, int *k, double *x, int *jderiv);

int tautsp(double *x, double *y, unsigned int *n, double *gamma, double *scrtch, double *break_, double *coef, int *l, int *k, int *iflag);


#if 0
static void display_f_xy(const char *msg)
{
	printf("%s\n", msg);
	for (unsigned int j = _num_ymatrix_data - 1; j > -1; j--) {
		for (unsigned int i = 0; i < _num_xmatrix_data; i++)
			printf("%f ", _f_xy(i,j));
		printf("\n");
	}
}
#endif

#define DEFAULT_GRID_LENGTH 20
bool
create_default_xgrid()
{
	double xmin = _colX.min();
	double xmax = _colX.max();
	unsigned int nx = DEFAULT_GRID_LENGTH;
	double xinc = (xmax - xmin) / (nx - 1);
	Require(allocate_xmatrix_storage(nx),
		err("Insufficient space for grid x data"));
	for (unsigned int i = 0; i < nx; i++)
		_xmatrix[i] = xmin + double(i) * xinc;
	_xgrid_exists = true;
	if (!_xscale_exists)
		create_x_scale();
	if (_xmatrix[1] > _xmatrix[0])
		_xgrid_increasing = true;
	else
		_xgrid_increasing = false;
	return true;

}
bool
create_default_ygrid()
{
	double ymin = _colY.min();
	double ymax = _colY.max();
	unsigned int ny = DEFAULT_GRID_LENGTH;
	double yinc = (ymax - ymin) / (ny - 1);
	Require(allocate_ymatrix_storage(ny),
		err("Insufficient space for grid y data"));
	for (unsigned int i = 0; i < ny; i++)
		_ymatrix[i] = ymin + double(i) * yinc;
	_ygrid_exists = true;
	if (!_yscale_exists)
		create_y_scale();
	if (_ymatrix[1] > _ymatrix[0])
		_ygrid_increasing = true;
	else
		_ygrid_increasing = false;
	return true;
}
#undef DEFAULT_GRID_LENGTH

int
convert_col_to_gridCmd()
{
	int             found = 0;
	Require(_colZ.size() > 0, 
		err("No z data exist yet.  First `read columns ... z'"));
	Require(_colX.size() == _colY.size() && _colX.size() == _colZ.size(),
		err("The x, y, and z columns are of unequal lengths"));
	if (!_xgrid_exists) {
		Require(create_default_xgrid(),
			err("Cannot create default x grid"));
	}
	if (!_ygrid_exists) {
		Require(create_default_ygrid(),
			err("Cannot create default y grid"));
	}
	// Check for archaic usage
	if (word_is(4, "planar")) {
		err("Sorry, `convert columns to grid planar' no longer available.\nTry using the `boxcar' method, which is virtually identical");
		return 0;
	}
	// Seem to have data.  Now proceed, first checking to see if neighbor
	// method (which takes no extra params).  But first dump to 
	// vectors known to be nonmissing
	std::vector<double> xgood; xgood.reserve(_colX.size());
	std::vector<double> ygood; ygood.reserve(_colX.size());
	std::vector<double> zgood; zgood.reserve(_colX.size());
	std::vector<double> wgood; wgood.reserve(_colX.size());
	unsigned int num = _colX.size();
	bool have_weights = (_colWEIGHT.size() == _colX.size());
	for (unsigned int g = 0; g < num; g++) {
		if (!gr_missingx(_colX[g]) && !gr_missingy(_colY[g]) && !gr_missing(_colZ[g])) {
			xgood.push_back(_colX[g]);
			ygood.push_back(_colY[g]);
			zgood.push_back(_colZ[g]);
			if (have_weights)
				wgood.push_back(_colWEIGHT[g]);
			else
				wgood.push_back(1.0);
		}
	}
	if (!xgood.size())
		return true;		// no data
	if (word_is(4, "neighbor")) {
		if (_nword != 5) {
			demonstrate_command_usage();
			NUMBER_WORDS_ERROR;
			return 0;
		}
		found = create_grid_neighborCmd(xgood, ygood, zgood);
	} else {
		bool use_default = _nword == 4;
		// Not neighbor method.  Must be one of below
		//
		//`convert columns to grid'
		//`convert columns to grid boxcar    [.xr. .yr. [.n.    .e.]]'
		//`convert columns to grid objective [.xr. .yr. [.n.    .e.]]'
		//`convert columns to grid barnes    [.xr. .yr. .gamma. .iter.]'
		//`convert columns to grid barnes_cross_validate    [.xr. .yr. .gamma. .iter.]'
		//       0       1  2    3      4      5    6      7     8
		if (_chatty > 0 && !word_is(4, "barnes_cross_validate"))
			ShowStr("`convert columns to grid' diagnostics:\n");
		if (word_is(4, "barnes") || word_is(4, "barnes_cross_validate")
		    || word_is(4, "boxcar")
		    || word_is(4, "objective")
		    || use_default) {
			double          xr = 0.0, yr = 0.0;
			double          gamma = 0.5; // .gamma.
			int             iter = 2;	 // .iter.
			// Figure out (xr, yr), either automatically or from cmd
			if (_nword >= 7) {
				// Get from cmdline
				if (!getdnum(_word[5], &xr)) {
					READ_WORD_ERROR(".xr.");
					demonstrate_command_usage();
					return 0;
				}
				if (!getdnum(_word[6], &yr)) {
					READ_WORD_ERROR(".yr.");
					demonstrate_command_usage();
					return 0;
				}
			}
			if (word_is(4, "barnes") || word_is(4, "barnes_cross_validate")) {
				switch(_nword) {
				case 5:
					gamma = 0.5;
					iter = 2;
					break;
				case 9:
					if (!getdnum(_word[7], &gamma)) {
						READ_WORD_ERROR(".gamma.");
						demonstrate_command_usage();
						return 0;
					}
					if (gamma < 0) {
						warning("\
`convert columns to grid barnes' changing sign of .gamma. to be >0");
						gamma = -gamma;
					}
					if (gamma > 1) {
						warning("\
`convert columns to grid barnes' clipping .gamma. to max value of1");
						gamma = 1.0;
					}
					if (!getinum(_word[8], &iter)) {
						READ_WORD_ERROR(".iter.");
						demonstrate_command_usage();
						return 0;
					}
					break;
				default:
					demonstrate_command_usage();
					NUMBER_WORDS_ERROR;
					return 0;
				}
			}
			// Now proceed to calculate
			// If .xr. < 0, of if not supplied calculate .xr./.yr.
			unsigned int numgood = xgood.size();
			if (_nword == 4 || _nword == 5 || xr < 0.0) {
				double          dx, dy;
				dx = (_colX.max() - _colX.min()) / sqrt(double(numgood));
				dy = (_colY.max() - _colY.min()) / sqrt(double(numgood));
				if (_chatty > 0) {
					sprintf(_grTempString, "\
  Data spacing, computed from areal fraction, is dx=%f, dy=%f).\n", dx, dy);
					ShowStr(_grTempString);
				}
				// Either use fabs(xr) to get real xr, or use 1.4*dx
				if (xr < 0.0) {
					xr = GRI_ABS(xr) * dx;
					yr = GRI_ABS(yr) * dy;
				} else {
					// REF on why using 1.4: Equation 13 in S. E. Koch and M.
					// DesJardins and P. J. Kocin, 1983.  ``An interactive
					// Barnes objective map anlaysis scheme for use with
					// satellite and conventional data,'', J. Climate Appl.
					// Met., vol 22, p. 1487-1503.
					xr = 1.4 * dx;
					yr = 1.4 * dy;
				}
				if (_chatty > 0) {
					sprintf(_grTempString, "\
  Therefore Gri will use smoothing scales xr=%f, yr=%f.  To resolve the small scale\n\
  features, the (x,y) grid spacing should be 1/3 to 1/2 of these values.\n\n", xr, yr);
					ShowStr(_grTempString);
				}
			}			// had to calculate (xr,yr)
			// OK, now do the gridding
			if (word_is(4, "barnes") || use_default) {
				// Barnes fills whole grid for now, anyway.
				create_grid_barnes(xr,
						   yr, 
						   gamma, 
						   (unsigned int)iter,
						   xgood,
						   ygood,
						   zgood,
						   wgood);
				found = _num_xmatrix_data * _num_ymatrix_data;
			} else if (word_is(4, "barnes_cross_validate")) {
				// Barnes fills whole grid for now, anyway.
				create_grid_barnes_cv(xr,
						      yr,
						      gamma, 
						      (unsigned int)iter,
						      xgood,
						      ygood,
						      zgood,
						      wgood);
				found = _num_xmatrix_data * _num_ymatrix_data;
			} else if (word_is(4, "boxcar")) {
				found = create_grid_boxcarCmd(xr, 
							      yr,
							      xgood,
							      ygood,
							      zgood);
			} else if (word_is(4, "objective")) {
				found = create_grid_objectiveCmd(xr, 
								 yr,
								 xgood,
								 ygood,
								 zgood);
			} else {
				err("Method must be `boxcar', `objective', `barnes' or `barnes_cross_validate'");
				return 0;
			}
		} else {
			err("Method must be `boxcar', `objective', `barnes' or `barnes_cross_validate'");
			return 0;
		}
	}				// method
	if (_chatty > 0 && !word_is(4, "barnes_cross_validate")) {
		sprintf(_grTempString, "\
  Filled %.3f%% of the %d row, %d col grid.\n",
			100.0 * (double) found / (double) (_num_xmatrix_data * _num_ymatrix_data),
			_num_ymatrix_data, _num_xmatrix_data);
		ShowStr(_grTempString);
	}
	// Locate and flag missing values
	for (unsigned int i = 0; i < _num_xmatrix_data; i++)
		for (unsigned int j = 0; j < _num_ymatrix_data; j++)
			if (gr_missing(_f_xy(i, j)))
				_legit_xy(i, j) = false;
	matrix_limits(&_f_min, &_f_max);
	if (get_flag("jinyu1")) {
		char *name = "tmp.dat";
		printf("Flag 'jinyu1' set, so writing x, y, z, zpredicted to file '%s'\n",
		       name);
		FILE *tmp = fopen (name, "w");
		Require(tmp, err("Cannot open the file"));
		unsigned int num = _colX.size();
		for (unsigned int i = 0; i < num; i++) {
			double zpred;
			grid_interp(_colX[i], _colY[i], &zpred);
			if (!gr_missing(zpred)) {
				fprintf(tmp, "%f %f %f %f\n", 
					_colX[i], _colY[i], _colZ[i], zpred);
			} else {
				fprintf(tmp, "%f %f %f %f\n", 
					_colX[i], _colY[i], _colZ[i], gr_currentmissingvalue());
			}
		}
	}
	return found;
}

static inline double  dmin(double a, double b)
{
	if (a > b) 
		return b;
	else 
		return a;
}

int interv(double *xt, int *lxt, double *x, int *left, int *mflag)
{
	static int ilo = 1;
	static int istep, middle, ihi;
	// Parameter adjustments
	--xt;
	//  from  * a practical guide to splines *  by C. de Boor
	// computes  left = max( i :  xt(i) .lt. xt(lxt) .and.  xt(i) .le. x )
	//    
	// ******  i n p u t  ****** 
	//  xt.....a double sequence, of length  lxt , assumed to be nondecreasing
	//  lxt.....number of terms in the sequence  xt
	//  x.....the point whose location with respect to the sequence  xt  is
	//        to be determined
	//    
	// ******  o u t p u t  ******
	//  left, mflag.....both integers, whose value is
	//    
	//   1     -1      if               x .lt.  xt(1)
	//   i      0      if   xt(i)  .le. x .lt. xt(i+1)
	//   i      0      if   xt(i)  .lt. x .eq. xt(i+1) .eq. xt(lxt)
	//   i      1      if   xt(i)  .lt.        xt(i+1) .eq. xt(lxt) .lt. x
	//    
	// In particular,  mflag = 0  is the 'usual' case.  mflag .ne. 0
	//
	// indicates that  x  lies outside the CLOSED interval
	// xt(1) .le. y .le. xt(lxt) . The asymmetric treatment of the
	// intervals is due to the decision to make all pp functions 
	// continuous from the right, but, by returning  mflag = 0  even if
	// x = xt(lxt), there is the option of having the computed pp 
	// function continuous from the left at  xt(lxt).
	//
	// ******  m e t h o d  ******
	// The program is designed to be efficient in the common situation that 
	// it is called repeatedly, with  x  taken from an increasing or decrea-
	// sing sequence. This will happen, e.g., when a pp function is to be
	// graphed. The first guess for  left  is therefore taken to be the val- 
	// ue returned at the previous call and stored in the  l o c a l  varia- 
	// ble  ilo . A first check ascertains that  ilo .lt. lxt (this is nec-
	// essary since the present call may have nothing to do with the previ- 
	// ous call). Then, if  xt(ilo) .le. x .lt. xt(ilo+1), we set  
	// left = ilo  and are done after just three comparisons
	// Otherwise, we repeatedly double the difference  istep = ihi - ilo 
	// while also moving  ilo  and  ihi  in the direction of  x , until
	//     xt(ilo) .le. x .lt. xt(ihi),
	// after which we use bisection to get, in addition,
	// ilo+1 = ihi. left = ilo  is then returned
    
	ihi = ilo + 1;
	if (ihi < *lxt) {
		goto L20;
	}
	if (*x >= xt[*lxt]) {
		goto L110;
	}
	if (*lxt <= 1) {
		goto L90;
	}
	ilo = *lxt - 1;
	ihi = *lxt;
    
 L20:
	if (*x >= xt[ihi]) {
		goto L40;
	}
	if (*x >= xt[ilo]) {
		goto L100;
	}
    
// **** now x .lt. xt(ilo) . decrease  ilo  to capture  x .
	istep = 1;
 L31:
	ihi = ilo;
	ilo = ihi - istep;
	if (ilo <= 1) {
		goto L35;
	}
	if (*x >= xt[ilo]) {
		goto L50;
	}
	istep <<= 1;
	goto L31;
 L35:
	ilo = 1;
	if (*x < xt[1]) {
		goto L90;
	}
	goto L50;
	// **** now x .ge. xt(ihi) . increase  ihi  to capture  x .    
 L40:
	istep = 1;
 L41:
	ilo = ihi;
	ihi = ilo + istep;
	if (ihi >= *lxt) {
		goto L45;
	}
	if (*x < xt[ihi]) {
		goto L50;
	}
	istep <<= 1;
	goto L41;
 L45:
	if (*x >= xt[*lxt]) {
		goto L110;
	}
	ihi = *lxt;
    
	// Now xt(ilo) .le. x .lt. xt(ihi) . narrow the interval. 
 L50:
	middle = (ilo + ihi) / 2;
	if (middle == ilo) {
		goto L100;
	}
	// Note. it is assumed that middle = ilo in case ihi = ilo+1
	if (*x < xt[middle]) {
		goto L53;
	}
	ilo = middle;
	goto L50;
 L53:
	ihi = middle;
	goto L50;
	// Set output and return.
 L90:
	*mflag = -1;
	*left = 1;
	return 0;
 L100:
	*mflag = 0;
	*left = ilo;
	return 0;
 L110:
	*mflag = 1;
	if (*x == xt[*lxt]) {
		*mflag = 0;
	}
	*left = *lxt;
 L111:
	if (*left == 1) {
		return 0;
	}
	--(*left);
	if (xt[*left] < xt[*lxt]) {
		return 0;
	}
	goto L111;
}

double ppvalu(double *break_,
	      double *coef, 
	      int *l,
	      int *k,
	      double *x,
	      int *jderiv)
{
	int coef_dim1, coef_offset, i_1;
	double ret_val;

	static double h;
	static int i, m;
	static double fmmjdr;
	static int ndummy;

	// Parameter adjustments
	--break_;
	coef_dim1 = *k;
	coef_offset = coef_dim1 + 1;
	coef -= coef_offset;

	// Function Body
	//  from  * a practical guide to splines *  by c. de boor
	// calls  interv
	// calculates value at  x  of  jderiv-th derivative of pp fct from pp-rep

	// ******  i n p u t  ******
	//  break, coef, l, k.....forms the pp-representation of the function  f 
	//        to be evaluated. specifically, the j-th derivative of  f  is
	//        given by
	//     (d**j)f(x) = coef(j+1,i) + h*(coef(j+2,i) + h*( ... (coef(k-1,i) + 
	//                             + h*coef(k,i)/(k-j-1))/(k-j-2) ... )/2)/1
	//        with  h = x - break(i),  and
	//       i  =  max( 1 , max( j ,  break(j) .le. x , 1 .le. j .le. l ) ).
	//  x.....the point at which to evaluate.
	//  jderiv.....int giving the order of the derivative to be evaluat-     
	//        ed.  a s s u m e d  to be zero or positive.
    
	// ******  o u t p u t  ******
	//  ppvalu.....the value of the (jderiv)-th derivative of  f  at  x.
    
	// ******  m e t h o d  ******
	//     the interval index  i , appropriate for  x , is found through a
	//  call to  interv . the formula above for the  jderiv-th derivative
	//  of  f  is then evaluated (by nested multiplication).
    
	ret_val = 0.0;
	fmmjdr = (double) (*k - *jderiv);
	//              derivatives of order  k  or higher are identically zero.     
	if (fmmjdr <= 0.0) {
		goto L99;
	}
    
	//              find index  i  of largest breakpoint to the left of  x .     
	i_1 = *l + 1;
	interv(&break_[1], &i_1, x, &i, &ndummy);
    
	//      Evaluate  jderiv-th derivative of  i-th polynomial piece at  x .     
	h = *x - break_[i];
	m = *k;
 L9:
	ret_val = ret_val / fmmjdr * h + coef[m + i * coef_dim1];
	--m;
	fmmjdr += -1.0;
	if (fmmjdr > 0.0) {
		goto L9;
	}
 L99:
	return ret_val;
}

// Use iflag to indicate if call was ok
int tautsp(double *tau,		// input data, x, of length ntau
	   double *gtau,		// input data, y, of length ntau
	   unsigned int *ntau,		// number data
	   double *gamma,	// tension parameter
	   double *s,		// length 6*ntau
	   double *break_,	// knot locations, length=l
	   double *coef,		// coefficients, length=4l
	   int *l,		// returned # knots, max maybe 2*ntau
	   int *k,		// always returned as 4
	   int *iflag)		// 0 indicates call was ok
{
	int s_dim1, s_offset;
	double r_1, r_2, r_3;

	// Local variables
	static double zeta, temp, c, d;
	static unsigned int i;
	static double alpha, z, denom, ratio, sixth, entry_, factr2, onemg3;
	static unsigned int ntaum1;
	static double entry3, divdif, factor;
	static int method;
	static double onemzt, zt2, del, gam;

	// Parameter adjustments
	--tau;
	--gtau;
	s_dim1 = *ntau;
	s_offset = s_dim1 + 1;
	s -= s_offset;
	--break_;
	coef -= 5;

	// From  A PRACTICAL GUIDE TO SPLINES by C. de Boor
	//  Constructs cubic spline interpolant to given data
	//         tau(i), gtau(i), i=1,...,ntau.
	//  if  gamma .gt. 0., additional knots are introduced where needed to
	//  make the interpolant more flexible locally. this avoids extraneous
	//  inflection points typical of cubic spline interpolation at knots to
	//  rapidly changing data.
	//
	// INPUT PARAMETERS
	//  tau      sequence of data points. must be strictly increasing.
	//  gtau     corresponding sequence of function values.
	//  ntau     number of data points. must be at least  4 .
	//  gamma    indicates whether additional flexibility is desired.
	//          = 0., no additional knots
	//          in (0.,3.), under certain conditions on the given data at
	//                points i-1, i, i+1, and i+2, a knot is added in the
	//                i-th interval, i=2,...,ntau-2. see description of meth-
	//                od below. the interpolant gets rounded with increasing
	//                gamma. a value of  2.5  for gamma is typical.
	//          in (3.,6.), same , except that knots might also be added in
	//                intervals in which an inflection point would be permit-
	//                ted.  a value of  5.5  for gamma is typical.
	// OUTPUT PARAMETERS
	//  break, coef, l, k  give the pp-representation of the interpolant.
	//          specifically, for break(i) .le. x .le. break(i+1), the
	//        interpolant has the form
	//  f(x) = coef(1,i) +dx(coef(2,i) +(dx/2)(coef(3,i) +(dx/3)coef(4,i)))
	//        with  dx = x - break(i) and i=1,...,l .
	//  iflag   = 1, ok
	//          = 2, input was incorrect. a printout specifying the mistake
	//            was made.
	//            workspace
	//  s     is required, of size (ntau,6). the individual columns of this
	//        array contain the following quantities mentioned in the write-
	//        up and below.
	//     s(.,1) = dtau = tau(.+1) - tau
	//     s(.,2) = diag = diagonal in linear system
	//     s(.,3) = u = upper diagonal in linear system
	//     s(.,4) = r = right side for linear system (initially)
	//            = fsecnd = solution of linear system , namely the second
	//                       derivatives of interpolant at  tau
	//     s(.,5) = z = indicator of additional knots
	//     s(.,6) = 1/hsecnd(1,x) with x = z or = 1-z. see below.

	//  ------  m e t h o d  ------
	//  on the i-th interval, (tau(i), tau(i+1)), the interpolant is of the

	//  form
	//  (*)  f(u(x)) = a + b*u + c*h(u,z) + d*h(1-u,1-z) ,
	//  with  u = u(x) = (x - tau(i))/dtau(i). here,
	//       z = z(i) = addg(i+1)/(addg(i) + addg(i+1))
	//  (= .5, in case the denominator vanishes). with
	//       addg(j) = abs(ddg(j)), ddg(j) = dg(j+1) - dg(j),
	//       dg(j) = divdif(j) = (gtau(j+1) - gtau(j))/dtau(j)
	//  and
	//       h(u,z) = alpha*u**3 + (1 - alpha)*(max(((u-zeta)/(1-zeta)),0)**3
	//  with
	//       alpha(z) = (1-gamma/3)/zeta
	//       zeta(z) = 1 - gamma*min((1 - z), 1/3)
	//  thus, for 1/3 .le. z .le. 2/3,  f  is just a cubic polynomial on
	//  the interval i. otherwise, it has one additional knot, at
	//         tau(i) + zeta*dtau(i) .
	//  as  z  approaches  1, h(.,z) has an increasingly sharp bend  near 1,
	//  thus allowing  f  to turn rapidly near the additional knot.
	//     in terms of f(j) = gtau(j) and
	//       fsecnd(j) = 2.derivative of  f  at  tau(j),
	//  the coefficients for (*) are given as
	//       a = f(i) - d
	//       b = (f(i+1) - f(i)) - (c - d)
	//       c = fsecnd(i+1)*dtau(i)**2/hsecnd(1,z)
	//       d = fsecnd(i)*dtau(i)**2/hsecnd(1,1-z)
	//  hence can be computed once fsecnd(i),i=1,...,ntau, is fixed.
	//   f  is automatically continuous and has a continuous second derivat-
	//  ive (except when z = 0 or 1 for some i). we determine fscnd(.) from
	//  the requirement that also the first derivative of  f  be contin-
	//  uous. in addition, we require that the third derivative be continuous
	//  across  tau(2) and across  tau(ntau-1) . this leads to a strictly
	//  diagonally dominant tridiagonal linear system for the fsecnd(i)
	//  which we solve by gauss elimination without pivoting.

	// there must be at least 4  interpolation points.
	if (*ntau < 4) {
		err("Need more than 3 data points");
		*iflag = 2;
		return 0;
	}
	// construct delta tau and first and second (divided) differences of data
	ntaum1 = *ntau - 1;
	for (i = 1; i <= ntaum1; ++i) {
		s[i + s_dim1] = tau[i + 1] - tau[i];
		if (s[i + s_dim1] <= 0.) {
			sprintf(_grTempString, "\
X data must be ordered and distinct.\n\
       Problem at x[%d]=%f and x[%d]=%f\n", i, tau[i], i+1, tau[i+1]);
			err(_grTempString);
			*iflag = 2;
			return 0;
		}
		s[i + 1 + (s_dim1 << 2)] = (gtau[i + 1] - gtau[i]) / s[i + s_dim1];
	}
	for (i = 2; i <= ntaum1; ++i) {
		s[i + (s_dim1 << 2)] = s[i + 1 + (s_dim1 << 2)] - s[i + (s_dim1 << 2)];
	}
	// Construct system of equations for second derivatives at  tau. at each
	//  interior data point, there is one continuity equation, at the first
	//  and the last interior data point there is an additional one for a
	//  total of  ntau  equations in  ntau  unknowns.
	i = 2;
	s[(s_dim1 << 1) + 2] = s[s_dim1 + 1] / 3.0;
	sixth = 1.0 / 6.0;
	method = 2;
	gam = *gamma;
	if (gam <= 0.0) {
		method = 1;
	}
	if (gam > 3.0) {
		method = 3;
		gam += -3.0;
	}
	onemg3 = 1.0 - gam / 3.0;
	// ------ loop over i ------
 L10:
	// construct z(i) and zeta(i)
	z = 0.5;
	switch (method) {
	case 1:  goto L19;
	case 2:  goto L11;
	case 3:  goto L12;
	}
 L11:
	if (s[i + (s_dim1 << 2)] * s[i + 1 + (s_dim1 << 2)] < 0.) {
		goto L19;
	}
 L12:
	temp = (r_1 = s[i + 1 + (s_dim1 << 2)], GRI_ABS(r_1));
	denom = (r_1 = s[i + (s_dim1 << 2)], GRI_ABS(r_1)) + temp;
	if (denom == 0.0) {
		goto L19;
	}
	z = temp / denom;
	if ((r_1 = z - 0.5, GRI_ABS(r_1)) <= sixth) {
		z = 0.5;
	}
 L19:
	s[i + s_dim1 * 5] = z;
	//   ******set up part of the i-th equation which depends on
	//         the i-th interval
	if ((r_1 = z - 0.5) < 0.0) {
		goto L21;
	} else if (r_1 == 0) {
		goto L22;
	} else {
		goto L23;
	}
 L21:
	zeta = gam * z;
	onemzt = 1.0 - zeta;
	zt2 = zeta * zeta;
	// Computing MAX
	r_1 = 1.0, r_2 = onemg3 / onemzt;
	alpha = dmin(r_2,r_1);
	factor = zeta / (alpha * (zt2 - 1.0) + 1.0);
	s[i + s_dim1 * 6] = zeta * factor / 6.0;
	s[i + (s_dim1 << 1)] += s[i + s_dim1] * ((1.0 - alpha * onemzt) * 
						 factor / 2.0 - s[i + s_dim1 * 6]);
	// if z = 0 and the previous z = 1, then d(i) = 0. since then
	// also u(i-1) = l(i+1) = 0, its value does not matter. reset
	// d(i) = 1 to insure nonzero pivot in elimination.
	if (s[i + (s_dim1 << 1)] <= 0.0) {
		s[i + (s_dim1 << 1)] = 1.0;
	}
	s[i + s_dim1 * 3] = s[i + s_dim1] / 6.0;
	goto L25;
 L22:
	s[i + (s_dim1 << 1)] += s[i + s_dim1] / 3.0;
	s[i + s_dim1 * 3] = s[i + s_dim1] / 6.0;
	goto L25;
 L23:
	onemzt = gam * (1.0 - z);
	zeta = 1.0 - onemzt;
	// Computing MAX
	r_1 = 1.0, r_2 = onemg3 / zeta;
	alpha = dmin(r_2,r_1);
	factor = onemzt / (1.0 - alpha * zeta * (onemzt + 1.0));
	s[i + s_dim1 * 6] = onemzt * factor / 6.0;
	s[i + (s_dim1 << 1)] += s[i + s_dim1] / 3.0;
	s[i + s_dim1 * 3] = s[i + s_dim1 * 6] * s[i + s_dim1];
 L25:
	if (i > 2) {
		goto L30;
	}
	s[s_dim1 * 5 + 1] = 0.5;
	//  ******the first two equations enforce continuity of the first and of
	//        the third derivative across tau(2).
	s[(s_dim1 << 1) + 1] = s[s_dim1 + 1] / 6.0;
	s[s_dim1 * 3 + 1] = s[(s_dim1 << 1) + 2];
	entry3 = s[s_dim1 * 3 + 2];
	if ((r_1 = z - 0.5) < 0.0) {
		goto L26;
	} else if (r_1 == 0) {
		goto L27;
	} else {
		goto L28;
	}
 L26:
	factr2 = zeta * (alpha * (zt2 - 1.0) + 1.0) / (alpha * (zeta *
								zt2 - 1.0) + 1.0);
	ratio = factr2 * s[s_dim1 + 2] / s[(s_dim1 << 1) + 1];
	s[(s_dim1 << 1) + 2] = factr2 * s[s_dim1 + 2] + s[s_dim1 + 1];
	s[s_dim1 * 3 + 2] = -(double)factr2 * s[s_dim1 + 1];
	goto L29;
 L27:
	ratio = s[s_dim1 + 2] / s[(s_dim1 << 1) + 1];
	s[(s_dim1 << 1) + 2] = s[s_dim1 + 2] + s[s_dim1 + 1];
	s[s_dim1 * 3 + 2] = -(double)s[s_dim1 + 1];
	goto L29;
 L28:
	ratio = s[s_dim1 + 2] / s[(s_dim1 << 1) + 1];
	s[(s_dim1 << 1) + 2] = s[s_dim1 + 2] + s[s_dim1 + 1];
	s[s_dim1 * 3 + 2] = -(double)s[s_dim1 + 1] * 6.0 * alpha 
		* s[s_dim1 * 6 + 2];
	//       at this point, the first two equations read
	//              diag(1)*x1 + u(1)*x2 + entry3*x3 = r(2)
	//       -ratio*diag(1)*x1 + diag(2)*x2 + u(2)*x3 = 0.
	//       eliminate first unknown from second equation
 L29:
	s[(s_dim1 << 1) + 2] = ratio * s[s_dim1 * 3 + 1] + s[(s_dim1 << 1) + 2];
	s[s_dim1 * 3 + 2] = ratio * entry3 + s[s_dim1 * 3 + 2];
	s[(s_dim1 << 2) + 1] = s[(s_dim1 << 2) + 2];
	s[(s_dim1 << 2) + 2] = ratio * s[(s_dim1 << 2) + 1];
	goto L35;
 L30:
	//  ******the i-th equation enforces continuity of the first derivative

	//        across tau(i). it has been set up in statements 35 up to 40
	//        and 21 up to 25 and reads now
	//         -ratio*diag(i-1)*xi-1 + diag(i)*xi + u(i)*xi+1 = r(i) .
	//        eliminate (i-1)st unknown from this equation
	s[i + (s_dim1 << 1)] = ratio * s[i - 1 + s_dim1 * 3] + s[i + (s_dim1 << 1)
		];
	s[i + (s_dim1 << 2)] = ratio * s[i - 1 + (s_dim1 << 2)] + s[i + (s_dim1 <<
									 2)];

	//  ******set up the part of the next equation which depends on the
	//        i-th interval.
 L35:
	if ((r_1 = z - 0.5) < 0.0) {
		goto L36;
	} else if (r_1 == 0) {
		goto L37;
	} else {
		goto L38;
	}
 L36:
	ratio = -(double)s[i + s_dim1 * 6] * s[i + s_dim1] / s[i + (s_dim1 << 
								    1)];
	s[i + 1 + (s_dim1 << 1)] = s[i + s_dim1] / 3.0;
	goto L40;
 L37:
	ratio = -(double)(s[i + s_dim1] / 6.0) / s[i + (s_dim1 << 1)];
	s[i + 1 + (s_dim1 << 1)] = s[i + s_dim1] / 3.0;
	goto L40;
 L38:
	ratio = -(double)(s[i + s_dim1] / 6.0) / s[i + (s_dim1 << 1)];
	s[i + 1 + (s_dim1 << 1)] = s[i + s_dim1] * ((1.0 - zeta * alpha) * 
						    factor / 2.0 - s[i + s_dim1 * 6]);
	//         ------  end of i loop ------
 L40:
	++i;
	if (i < ntaum1) {
		goto L10;
	}
	s[i + s_dim1 * 5] = 0.5;

	//        ------  last two equations  ------
	//  the last two equations enforce continuity of third derivative and
	//  of first derivative across  tau(ntau-1).
	entry_ = ratio * s[i - 1 + s_dim1 * 3] + s[i + (s_dim1 << 1)] + s[i + 
									 s_dim1] / 3.0;
	s[i + 1 + (s_dim1 << 1)] = s[i + s_dim1] / 6.0;
	s[i + 1 + (s_dim1 << 2)] = ratio * s[i - 1 + (s_dim1 << 2)] 
		+ s[i + (s_dim1 << 2)];
	if ((r_1 = z - 0.5) < 0.0) {
		goto L41;
	} else if (r_1 == 0) {
		goto L42;
	} else {
		goto L43;
	}
 L41:
	ratio = s[i + s_dim1] * 6.0 * s[i - 1 + s_dim1 * 6] * alpha / s[i - 
								       1 + (s_dim1 << 1)];
	s[i + (s_dim1 << 1)] = ratio * s[i - 1 + s_dim1 * 3] + s[i + s_dim1] + s[
		i - 1 + s_dim1];
	s[i + s_dim1 * 3] = -(double)s[i - 1 + s_dim1];
	goto L45;
 L42:
	ratio = s[i + s_dim1] / s[i - 1 + (s_dim1 << 1)];
	s[i + (s_dim1 << 1)] = ratio * s[i - 1 + s_dim1 * 3] + s[i + s_dim1] + s[
		i - 1 + s_dim1];
	s[i + s_dim1 * 3] = -(double)s[i - 1 + s_dim1];
	goto L45;
 L43:
	// Computing 2nd power
	r_1 = onemzt;
	// Computing 3rd power
	r_2 = onemzt, r_3 = r_2;
	factr2 = onemzt * (alpha * (r_1 * r_1 - 1.0) + 1.0) / (alpha *
							       (r_3 * (r_2 * r_2) - 1.0) + 1.0);
	ratio = factr2 * s[i + s_dim1] / s[i - 1 + (s_dim1 << 1)];
	s[i + (s_dim1 << 1)] = ratio * s[i - 1 + s_dim1 * 3] + factr2 * s[i - 1 + 
									 s_dim1] + s[i + s_dim1];
	s[i + s_dim1 * 3] = -(double)factr2 * s[i - 1 + s_dim1];
	//     at this point, the last two equations read
	//             diag(i)*xi + u(i)*xi+1 = r(i)
	//      -ratio*diag(i)*xi + diag(i+1)*xi+1 = r(i+1)
	//     eliminate xi from last equation
 L45:
	s[i + (s_dim1 << 2)] = ratio * s[i - 1 + (s_dim1 << 2)];
	ratio = -(double)entry_ / s[i + (s_dim1 << 1)];
	s[i + 1 + (s_dim1 << 1)] = ratio * s[i + s_dim1 * 3] + s[i + 1 + (s_dim1 
									  << 1)];
	s[i + 1 + (s_dim1 << 2)] = ratio * s[i + (s_dim1 << 2)] + s[i + 1 + (
		s_dim1 << 2)];

	//        ------ back substitution ------

	s[*ntau + (s_dim1 << 2)] /= s[*ntau + (s_dim1 << 1)];

	do {
		s[i + (s_dim1 << 2)] 
			= (s[i + (s_dim1 << 2)] - s[i + s_dim1 * 3] 
			   * s[i + 1 + (s_dim1 << 2)]) / s[i + (s_dim1 << 1)];
	} while (--i > 1);

	s[(s_dim1 << 2) + 1] = (s[(s_dim1 << 2) + 1] - s[s_dim1 * 3 + 1] * s[(
		s_dim1 << 2) + 2] - entry3 * s[(s_dim1 << 2) + 3]) / s[(s_dim1 << 
									1) + 1];

	//        ------ construct polynomial pieces ------

	break_[1] = tau[1];
	*l = 1;
	for (i = 1; i <= ntaum1; ++i) {
		coef[(*l << 2) + 1] = gtau[i];
		coef[(*l << 2) + 3] = s[i + (s_dim1 << 2)];
		divdif = (gtau[i + 1] - gtau[i]) / s[i + s_dim1];
		z = s[i + s_dim1 * 5];
		if ((r_1 = z - 0.5) < 0.0) {
			goto L61;
		} else if (r_1 == 0) {
			goto L62;
		} else {
			goto L63;
		}
	L61:
		if (z == 0.0) {
			goto L65;
		}
		zeta = gam * z;
		onemzt = 1.0 - zeta;
		c = s[i + 1 + (s_dim1 << 2)] / 6.0;
		d = s[i + (s_dim1 << 2)] * s[i + s_dim1 * 6];
		++(*l);
		del = zeta * s[i + s_dim1];
		break_[*l] = tau[i] + del;
		// Computing 2nd power
		zt2 = zeta * zeta;
		// Computing MAX
		r_1 = 1.0, r_2 = onemg3 / onemzt;
		alpha = dmin(r_2,r_1);
		// Computing 2nd power
		r_1 = onemzt;
		factor = r_1 * r_1 * alpha;
		// Computing 2nd power
		r_1 = s[i + s_dim1];
		coef[(*l << 2) + 1] = gtau[i] + divdif * del + r_1 * r_1 * (d * 
									    onemzt * (factor - 1.0) + c * zeta * (zt2 - 1.0));

		coef[(*l << 2) + 2] = divdif + s[i + s_dim1] * (d * (1.0 - 
								     factor * 3.) + c * (zt2 * 3. - 1.));
		coef[(*l << 2) + 3] = (d * alpha * onemzt + c * zeta) * 6.;
		coef[(*l << 2) + 4] = (c - d * alpha) * 6. / s[i + s_dim1];
		coef[(*l - 1 << 2) + 4] = coef[(*l << 2) + 4] - d * 6. * ((
			double)1. - alpha) / (del * zt2);
		coef[(*l - 1 << 2) + 2] = coef[(*l << 2) + 2] - del * (coef[(*l << 2) 
									   + 3] - del / 2. * coef[(*l - 1 << 2) + 4]);
		goto L68;
	L62:
		coef[(*l << 2) + 2] = divdif - s[i + s_dim1] * (s[i + (s_dim1 << 2)] *
								2. + s[i + 1 + (s_dim1 << 2)]) / 6.;
		coef[(*l << 2) + 4] = (s[i + 1 + (s_dim1 << 2)] - s[i + (s_dim1 << 2)]
			) / s[i + s_dim1];
		goto L68;
	L63:
		onemzt = gam * (1. - z);
		if (onemzt == 0.) {
			goto L65;
		}
		zeta = 1. - onemzt;
		// Computing MAX
		r_1 = 1., r_2 = onemg3 / zeta;
		alpha = dmin(r_2,r_1);
		c = s[i + 1 + (s_dim1 << 2)] * s[i + s_dim1 * 6];
		d = s[i + (s_dim1 << 2)] / 6.;
		del = zeta * s[i + s_dim1];
		break_[*l + 1] = tau[i] + del;
		coef[(*l << 2) + 2] = divdif - s[i + s_dim1] * (d * 2. + c);
		coef[(*l << 2) + 4] = (c * alpha - d) * 6. / s[i + s_dim1];
		++(*l);
		// Computing 3rd power
		r_1 = onemzt, r_2 = r_1;
		coef[(*l << 2) + 4] = coef[(*l - 1 << 2) + 4] + (1. - alpha) * 
			6. * c / (s[i + s_dim1] * (r_2 * (r_1 * r_1)));
		coef[(*l << 2) + 3] = coef[(*l - 1 << 2) + 3] + del * coef[(*l - 1 << 
									    2) + 4];
		coef[(*l << 2) + 2] = coef[(*l - 1 << 2) + 2] + del * (coef[(*l - 1 <<
									     2) + 3] + del / 2. * coef[(*l - 1 << 2) + 4]);
		coef[(*l << 2) + 1] = coef[(*l - 1 << 2) + 1] + del * (coef[(*l - 1 <<
									     2) + 2] + del / 2. * (coef[(*l - 1 << 2) + 3] + del / 
												   3. * coef[(*l - 1 << 2) + 4]));
		goto L68;
	L65:
		coef[(*l << 2) + 2] = divdif;
		coef[(*l << 2) + 3] = 0.;
		coef[(*l << 2) + 4] = 0.;
	L68:
		++(*l);
		// L70:
		break_[*l] = tau[i + 1];
		if (*l > 1 + 2 * int(*ntau)) {
			gr_error("Too many knots.  Kelley thought max was 2*n\n");
			return 0;		// not reached
		}
	}
	--(*l);
	*k = 4;
	*iflag = 1;
	return 0;
} // tautsp


// `convert columns to spline \[.gamma.\] \[.xmin. .xmax. .xinc.\]\'
bool
convert_col_to_splineCmd()
{
	Require(_columns_exist,
		err("No (x,y) data exist yet.  First `read columns'"));
	double gamma, xmin, xmax, xinc;
	switch (_nword) {
	case 4:			// `convert columns to spline'
		gamma = 0;
		xmin = _colX.min();
		xmax = _colX.max();
		xinc = (xmax - xmin) / 200;
		break;
	case 5:			// `convert columns to spline .gamma.'
		Require(getdnum(_word[4], &gamma), READ_WORD_ERROR(".gamma."));
		xmin = _colX.min();
		xmax = _colX.max();
		xinc = (xmax - xmin) / 200;
		break;
	case 8:			// `... .gamma. .xmin. .xmax. .xinc.'
		Require(getdnum(_word[4], &gamma), READ_WORD_ERROR(".gamma."));	
		Require(getdnum(_word[5], &xmin),  READ_WORD_ERROR(".xmin."));	
		Require(getdnum(_word[6], &xmax),  READ_WORD_ERROR(".xmax."));	
		Require(getdnum(_word[7], &xinc),  READ_WORD_ERROR(".xinc."));	
		break;
	default:
		NUMBER_WORDS_ERROR;
		return false;
	}
	Require(xmax != xmin, err("Cannot have .xmin. equaling .xmax."));
	Require(xinc != 0, err("Cannot have .xinc. equal to zero"));
	Require(xmax > xmin, err("Cannot have .xmin. exceeding .xmax."));
	Require(xinc > 0.0, err("Cannot have .xinc. < 0"));
	int steps = int(1 + (xmax - xmin) / xinc);
	Require(steps > 0, err("Cannot have 0 or fewer elements in spline"));
	// Calculate spline
	if (_colX.size() < 4) {
		err("Must have more than 4 data points to convert to spline");
		return false;
	}
	// Get storage
	double *xs, *ys, *coef, *break_point, *scrtch;
	xs = ys = coef = break_point = scrtch = (double*)NULL;
	GET_STORAGE(xs, double, (size_t)steps);
	GET_STORAGE(ys, double, (size_t)steps);
	GET_STORAGE(coef, double, (size_t)(4 * 2 * steps));
	GET_STORAGE(break_point, double, (size_t)(2 * steps));
	GET_STORAGE(scrtch, double, (size_t)(6 * steps));
	//std::vector<double> xs((size_t)steps, 0.0);
	//std::vector<double> ys((size_t)steps, 0.0);
	//std::vector<double> coef((size_t)(4 * 2 * steps), 0.0);
	//std::vector<double> break_point((size_t)(2 * steps), 0.0);
	//std::vector<double> scrtch((size_t)(6 * steps), 0.0);
	unsigned int n = _colX.size();
	int l, k, iflag;
	tautsp(_colX.begin(),
	       _colY.begin(),
	       &n,
	       &gamma,
	       scrtch,
	       break_point,
	       coef, 
	       &l,
	       &k,
	       &iflag);
	int zero = 0;
	for (unsigned int i = 0; i < (unsigned int)steps; i++) {
		xs[i] = xmin + i * xinc;
		ys[i] = ppvalu(break_point, coef, &l, &k, &xs[i], &zero);
	}
	// Dump spline output into (x,y)
	_colX.setDepth(steps);
	_colY.setDepth(steps);
	for (unsigned int i = 0; i < (unsigned int)steps; i++) {
		_colX[i] = xs[i];
		_colY[i] = ys[i];
	}
	free(xs);
	free(ys);
	free(coef);
	free(break_point);
	free(scrtch);
	return true;
}

// Do interpolation search, using bisection rule on possibly irregular
// array g[].
//
// If 'x' is in the range of the grid, defined by g[0] to g[ng-1],
// then set 'b' and 'f' such that
//     x = g[b] + f * (g[b+1] - g[b])
// and return true.
//
// If 'x' is not in the range, set b to the nearest endpoint, 
// set f to the distance to the nearest endpoint and return false.
static bool
nearest(double x, double g[], unsigned int ng, int *b, double *f)
{
	int l = 0;			// left index
	int r = ng - 1;		// right index
	int m;			// middle index
	if (g[0] < g[1]) {		// ascending sequence
		if (x <= g[l])	{ *b = 0; *f = g[l] - x; return false; }
		if (g[r] <= x)	{ *b = r; *f = x - g[r]; return false; }
		m = (l + r) / 2;
		while (r - l > 1) {
			if (x < g[m])
				r = m;
			else if (g[m] < x)
				l = m;
			else { 
				*b = m;
				*f = (x - g[*b]) / (g[*b+1] - g[*b]);
				return true; 
			}
			m = (r + l) / 2;
		}
		*b = l;
		*f = (x - g[*b]) / (g[*b+1] - g[*b]);
		return true;
	} else {			// descending sequence
		if (x >= g[l])	{ *b = 0; *f = g[l] - x; return false; }
		if (g[r] >= x)	{ *b = r; *f = x - g[r]; return false; }
		m = (l + r) / 2;
		while (r - l > 1) {
			if (x > g[m])
				r = m;
			else if (g[m] > x)
				l = m;
			else {
				*b = m;
				*f = (x - g[*b]) / (g[*b+1] - g[*b]);
				return true;
			}
			m = (r + l) / 2;
		}
		*b =  l;
		*f = (x - g[*b]) / (g[*b+1] - g[*b]);
		return true;
	}
}

// Returns number gridpoints filled
static int 
create_grid_neighborCmd(const std::vector<double> &xgood,
			const std::vector<double> &ygood,
			const std::vector<double> &zgood)
{
	if (!allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data)) {
		gr_Error("Insufficient space for matrix");
		return 0;
	}
	// Set up geometrical (scale) factors for calculating distance
	double x_scale = fabs(_xmatrix[1] - _xmatrix[0]);
	double y_scale = fabs(_ymatrix[1] - _ymatrix[0]);
	GriMatrix<double> dist;
	dist.set_size(_num_xmatrix_data, _num_ymatrix_data);
	// Unlegit to start
	_legit_xy.set_value(false);
	unsigned int numgood = xgood.size();
	for (unsigned int ii = 0; ii < numgood; ii++) {
		int bx, by;		// index to left
		double  fx, fy;		// fraction (if inside) or distance
		bool x_in = nearest(xgood[ii], _xmatrix, _num_xmatrix_data, &bx, &fx);
		bool y_in = nearest(ygood[ii], _ymatrix, _num_ymatrix_data, &by, &fy);
		// If bracketed, twiddle to aim at nearest.
		if (x_in && fx > 0.5)
			bx++;
		if (y_in && fy > 0.5)
			by++;
		// Only assign if blank or if this one is closer
		double dx = (xgood[ii] - _xmatrix[bx]) / x_scale;
		double dy = (ygood[ii] - _ymatrix[by]) / y_scale;
		double the_dist = sqrt(dx * dx + dy * dy);
		//
		// Change, vsn 2.1.8: only save the very nearest datum
		if (!_legit_xy(bx, by) || the_dist < dist(bx, by)) {
			dist(bx, by) = the_dist;
			_f_xy(bx, by) = zgood[ii];
			_legit_xy(bx, by) = true;
		}
	}
	// count elements filled
	unsigned int found = 0;
	for (unsigned int i = 0; i < _num_xmatrix_data; i++)
		for (unsigned int j = 0; j < _num_ymatrix_data; j++)
			if (_legit_xy(i, j))
				found++;
	return found;
}

bool
convert_grid_to_columnsCmd(void)
{
	if (!_grid_exists) {
		err("Cannot `convert grid to columns' since no grid data exist yet");
		return false;
	}
	_colX.setDepth(0);
	_colY.setDepth(0);
	_colZ.setDepth(0);
	for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
		for (unsigned int j = 0; j < _num_ymatrix_data; j++) {
			if (_legit_xy(i, j) == true) {
				_colX.push_back(_xmatrix[i]);
				_colY.push_back(_ymatrix[j]);
				_colZ.push_back(_f_xy(i, j));
			}
		}
	}
	_columns_exist = true;
	return true;
}

bool
convert_grid_to_imageCmd()
{
	//printf("%s:%d ENTER convert_grid_to_imageCmd()\n", __FILE__,__LINE__);
	int             i, ii, j, jj, val;
	int             width, height;
	int             clipped = 0, masked = 0;
	double          value, xx, dxx, yy, dyy;
	double          scale;
	if (!_grid_exists) {
		err("Cannot `convert grid to image' since no grid data exist yet");
		return false;
	}
	if (!_xgrid_exists) {
		err("Cannot `convert grid to image' since x-grid doesn't exist yet");
		return false;
	}
	if (!_ygrid_exists) {
		err("Cannot `convert grid to image' since y-grid doesn't exist yet");
		return false;
	}
	if (_nword < 4) {
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
	// If no image range exists, use min/max in image.
	if (!image_range_exists()) {
		_image0   = _f_min;
		_image255 = _f_max;
	}
	// See if the image size was given.
	width = IMAGE_SIZE_WHEN_CONVERTING;
	height = IMAGE_SIZE_WHEN_CONVERTING;
	if (2 == get_cmd_values(_word, _nword, "size", 2, _dstack)) {
		width = (int) (0.5 + _dstack[0]);
		height = (int) (0.5 + _dstack[1]);
	}
	// BEGIN: Code prior to 2.005
	//    if (width % 2) {
	//	warning("Making .width. in `convert grid to image' an even number");
	//	width++;
	//    }
	// END: Code prior to 2.005
	// Check for old-style usage (versions < 1.035)
	if (1 == get_cmd_values(_word, _nword, "white", 1, _dstack)) {
		err("The keyword `white' is no longer allowed; use `set image range'");
		return false;
	}
	if (1 == get_cmd_values(_word, _nword, "black", 1, _dstack)) {
		err("The keyword `black' is no longer allowed; use `set image range'");
		return false;
	}
	// Determine image scales, either from 'box' keyword or from the limits
	// of the data
	switch (get_cmd_values(_word, _nword, "box", 4, _dstack)) {
	case 4:
		define_image_scales(_dstack[0], _dstack[1], _dstack[2], _dstack[3]);
		break;
	case 0:
		define_image_scales(_xmatrix[0],
				    _ymatrix[0],
				    _xmatrix[_num_xmatrix_data - 1],
				    _ymatrix[_num_ymatrix_data - 1]);
		break;
	default:
		err("`box .xleft. .ybottom. .xright. .ytop.' needs exactly 4 parameters");
		return false;
	}
	if (width < 2) { err("Can't have grid < 2 wide"); return false; }
	if (height < 2) { err("Can't have grid < 2 tall"); return false; }

	//printf("%s:%d convert_grid_to_imageCmd() ABOUT TO GET STORAGE FOR %d by %d \n", __FILE__,__LINE__,width,height);

	if (!allocate_image_storage(width, height)) {
		err("Can't allocate storage for image");
		return false;
	}
	if (!allocate_imageMask_storage(width, height)) {
		err("Can't allocate storage for image mask");
		return false;
	}
	scale = 255.0 / (_image255 - _image0);
	dxx = (_image_urx - _image_llx) / (width - 1);
	dyy = (_image_ury - _image_lly) / (height - 1);
	if (_chatty > 0) {
		sprintf(_grTempString, "\
`convert grid to image':\n  Image is %d wide and %d tall, with x in range (%g,%g) and y in range (%g, %g)\n",
			width, height,
			_image_llx, _image_urx,
			_image_lly, _image_ury);
		ShowStr(_grTempString);
	}
	bool warned = false;
	GriTimer t;
	for (i = 0; i < width; i++) {
		xx = _image_llx + i * dxx;
		for (j = 0; j < height; j++) {
			yy = _image_lly + j * dyy;
			if (!locate_i_j(xx, yy, &ii, &jj)
			    || !value_i_j(ii, jj, xx, yy, &value)) {
				*(_image.image + _image.ras_height * i + j) = (unsigned char) _imageBLANK;
				*(_imageMask.image + _imageMask.ras_height * i + j) = 2;
				masked++;
			} else {
				// Method for converting to integer follows that in
				// value_to_image(), but done here to speed up.  Make sure to
				// update this if value_to_image() is updated.
				// XREF value_to_image()
				val = (int) floor(0.5 + scale * (value - _image0));
				if (val < 0) {
					val = 0;
					clipped++;
				} else if (val > 255) {
					val = 255;
					clipped++;
				}
				*(_image.image + _image.ras_height * i + j) = (unsigned char)
					val;
				*(_imageMask.image + _imageMask.ras_height * i + j) = (unsigned char) 0;
			}
			if (!warned) {
				double frac = (height * (1.0 + i)) /(width * height);
				warned = warn_if_slow(&t, frac, "convert grid to image");
			}
		}
	}
	if (_chatty > 0) {
		if (clipped || masked) {
			sprintf(_grTempString, "\
`convert grid to image':\n  Clipped %d (%.3f%%) pixels and masked %d (%.3f%%) pixels)\n",
				clipped, 100.0 * (double) clipped / (double) (width * height),
				masked, 100.0 * (double) masked / (double) (width * height));

		} else {
			sprintf(_grTempString, "\
`convert grid to image':\n  Did not clip or mask any of this image.\n");
		}
		ShowStr(_grTempString);
	}
	return true;
}

// BUG -- just get a near pixel, with no proper interpolation
bool
convert_image_to_gridCmd()
{
	bool            bad = false;
	// See that an image exists
	if (!_image.storage_exists) {
		err("First `read image' or `convert grid to image'");
		bad = true;
	}
	if (!_imageTransform_exists) {
		err("First `set image grayscale'");
		bad = true;
	}
	// Check that x/y grid exists; then get (or reconfigure) storage
	if (!_xgrid_exists) {
		err("First `set x grid' or `read grid x'");
		bad = true;
	}
	if (!_ygrid_exists) {
		err("First `set y grid' or `read grid y'");
		bad = true;
	}
	if (bad) {
		demonstrate_command_usage();
		return false;
	}
	if (!allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data)) {
		err("Insufficient space for `grid' data");
		return false;
	}
	unsigned int i, j;
	unsigned int good = 0;
	double scale = 255.0 / (_image255 - _image0);
	bool warned = false;
	GriTimer t;
	for (i = 0; i < _num_xmatrix_data; i++) {
		for (j = 0; j < _num_ymatrix_data; j++) {
			int ii = (int)floor(0.5 +
					    _image.ras_width
					    * (_xmatrix[i] - _image_llx)
					    / (_image_urx - _image_llx));
			int jj = (int)floor(0.5 +
					    _image.ras_height
					    * (_ymatrix[j] - _image_lly)
					    / (_image_ury - _image_lly));
			if (ii < 0 || jj < 0 || ii >= int(_image.ras_width) || jj >= int(_image.ras_height)) {
				_legit_xy(i, j) = false;	// outside image region
			} else {
				// Inside
				_f_xy(i, j) = _image0
					+ *(_image.image + _image.ras_height * ii + jj) / scale;
				_legit_xy(i, j) = true;
				good++;
			}
		}
		if (!warned) {
			double frac;
			frac = _image.ras_height * (1.0 + i);
			frac /= _image.ras_width * _image.ras_height;
			warned = warn_if_slow(&t, frac, "convert image to grid");
		}
	}
	if (_chatty > 0) {
		sprintf(_grTempString, "`convert image to grid':\n  Filled %.3f%% of the grid\n", 100.0 * (double) good / (double) (_num_xmatrix_data * _num_ymatrix_data));
		ShowStr(_grTempString);
	}
	return true;
}

// Interpolate in box f2 f3 f0 f1 using f = f0 + f1'x + f2'y +f3'xy, where
// ()' means () - f0, and x = xx - x_at_f0, etc.
// NOTE: ii,jj is point to lower-left of desired point.  If 
// at the top or right edge, just return the edge value.
// RETURN whether point is legit.
bool
value_i_j(unsigned int ii, unsigned int jj, double xx, double yy, double *value)
{
	double          Dx, Dy;	// width/height of domain with point
	double          f0, f1, f2, f3;
	double dx;			// x - x_to_left
	double dy;			// y - y_below
	// Fiddle with dx,dy,Dx,Dy, to avoid looking past array
	dx = (ii == _num_xmatrix_data - 1 ? 0.0 : xx - _xmatrix[ii]);
	dy = (jj == _num_ymatrix_data - 1 ? 0.0 : yy - _ymatrix[jj]);
	if (_legit_xy(ii, jj) == false
	    || (dx != 0.0 && _legit_xy(ii + 1, jj) == false)
	    || (dy != 0.0 && _legit_xy(ii, jj + 1) == false)
	    || (dx != 0.0 && dy != 0.0 && _legit_xy(ii + 1, jj + 1) == false)) {
		*value = gr_currentmissingvalue();
		return false;
	}
	f0 = _f_xy(ii, jj);
	f1 = dx != 0 ? _f_xy(ii + 1, jj) - f0 : 0.0;
	f2 = dy != 0 ? _f_xy(ii, jj + 1) - f0 : 0.0;
	f3 = (dx != 0 && dy != 0) ? _f_xy(ii + 1, jj + 1) - f0 - f1 - f2 : 0;
	Dx = dx != 0 ? _xmatrix[ii + 1] - _xmatrix[ii] : 1;
	Dy = dy != 0 ? _ymatrix[jj + 1] - _ymatrix[jj] : 1;
	*value = f0 + f1 * dx / Dx + f2 * dy / Dy + f3 * dx / Dx * dy / Dy;
	return true;
}

// Find (ii,jj) such that _xmatrix[ii]<xx<_xmatrix[ii+1].
// and yy is similarly inside the range.  If either xx or
// yy is outside the range, return false.
//
// Method is brute-force; binary search would be faster.
bool
locate_i_j(double xx, double yy, int *ii, int *jj)
{
	unsigned int i = 0, j = 0;
	*ii = 0;
	*jj = 0;		// assume at lower-left
	if (_xgrid_increasing) {
		if (xx < _xmatrix[0]) {
			return false;	// outside range
		}
		if (xx > _xmatrix[0]) {
			for (i = 0; i < _num_xmatrix_data - 1; i++) {
				if (_xmatrix[i] <= xx && xx <= _xmatrix[i + 1])
					break;
			}
		}
	} else {
		if (xx > _xmatrix[0]) {
			return false;	// outside range
		}
		if (xx < _xmatrix[0]) {
			for (i = 0; i < _num_xmatrix_data - 1; i++) {
				if (_xmatrix[i] >= xx && xx >= _xmatrix[i + 1])
					break;
			}
		}
	}
	if (i == _num_xmatrix_data - 1) {
		return false;
	}
	if (_ygrid_increasing) {
		if (yy < _ymatrix[0]) {
			return false;	// outside range
		}
		if (yy > _ymatrix[0]) {
			for (j = 0; j < _num_ymatrix_data - 1; j++) {
				if (_ymatrix[j] <= yy && yy <= _ymatrix[j + 1])
					break;
			}
		}
	} else {
		if (yy > _ymatrix[0]) {
			return false;	// outside range
		}
		if (yy < _ymatrix[0]) {
			for (j = 0; j < _num_ymatrix_data - 1; j++) {
				if (_ymatrix[j] >= yy && yy >= _ymatrix[j + 1])
					break;
			}
		}
	}
	if (j == _num_ymatrix_data - 1) {
		return false;
	}
	*ii = i;
	*jj = j;
	return true;
}

// Returns number gridpoints filled
static unsigned int
create_grid_objectiveCmd(double xr,
			 double yr,
			 const std::vector<double> &xgood,
			 const std::vector<double> &ygood,
			 const std::vector<double> &zgood)
{
	int             number_to_find = 5, enlargements = 1;
	double          xx, yy;
	if (_nword == 9) {
		if (!getinum(_word[7], &number_to_find)) {
			READ_WORD_ERROR(".n.");
			demonstrate_command_usage();
			return 0;
		}
		if (!getinum(_word[8], &enlargements)) {
			READ_WORD_ERROR(".e.");
			demonstrate_command_usage();
			return 0;
		}
	}
	// Allocate storage.
	if (!allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data)) {
		err("Insufficient space for matrix");
		return 0;
	}
	unsigned int i, j;
	unsigned int found = 0;
	// Zero out legit
	for (i = 0; i < _num_xmatrix_data; i++)
		for (j = 0; j < _num_ymatrix_data; j++)
			_legit_xy(i, j) = false;
	// Do interpolation.
	bool warned = false;
	GriTimer t;
	for (i = 0; i < _num_xmatrix_data; i++) {
		double          fpred;
		xx = _xmatrix[i];
		for (j = 0; j < _num_ymatrix_data; j++) {
			int number_found;
			yy = _ymatrix[j];
			number_found = gr_grid1(xgood, 
						ygood,
						zgood,
						xx, 
						yy,
						xr,
						yr,
						2,	// method
						number_to_find,
						enlargements,
						&fpred);
			_f_xy(i, j) = fpred;
			if (number_found >= (int) (GRI_ABS((double) number_to_find))) {
				_legit_xy(i, j) = true;
				found++;
			} else {
				_legit_xy(i, j) = false;
			}
		}
		if (!warned) {
			double frac = (i + 1.0) * _num_ymatrix_data;
			frac /= _num_xmatrix_data * _num_ymatrix_data;
			warned = warn_if_slow(&t, frac, "convert columns to grid");
		}
	}
	return found;
}

// Returns number gridpoints filled
static int
create_grid_boxcarCmd(double xr,
		      double yr,
		      const std::vector<double> &xgood,
		      const std::vector<double> &ygood,
		      const std::vector<double> &zgood)
{
	int number_to_find = 5, enlargements = 1;
	if (_nword == 9) {
		if (!getinum(_word[7], &number_to_find)) {
			READ_WORD_ERROR(".n.");
			demonstrate_command_usage();
			return 0;
		}
		if (!getinum(_word[8], &enlargements)) {
			READ_WORD_ERROR(".e.");
			demonstrate_command_usage();
			return 0;
		}
	}
	// Allocate storage.
	if (!allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data)) {
		err("Insufficient space for matrix");
		return 0;
	}
	unsigned int i, j;
	// zero out legit
	for (i = 0; i < _num_xmatrix_data; i++)
		for (j = 0; j < _num_ymatrix_data; j++)
			_legit_xy(i, j) = false;
	unsigned int found = 0;
	// Do interpolation.
	for (i = 0; i < _num_xmatrix_data; i++) {
		if (_chatty > 0) 
			printf("\n");
		double xx = _xmatrix[i];
		for (j = 0; j < _num_ymatrix_data; j++) {
			if (_chatty > 0)
				printf(".");
			unsigned int number_found;
			double yy = _ymatrix[j];
			double fpred;
			number_found = gr_grid1(xgood,
						ygood,
						zgood,
						xx, 
						yy,
						xr, 
						yr,
						0,	// means boxcar
						number_to_find,
						enlargements,
						&fpred);
			_f_xy(i, j) = fpred;
			if (number_found >= (unsigned int)number_to_find) {
				_legit_xy(i, j) = true;
				found++;
			} else {
				_legit_xy(i, j) = false;
			}
		}
	}
	return found;
}

// Barnes-interpolate to given (xx,yy), with previously value being zz.
// 'skip' used in cross-validation studies.
static double
interpolate_barnes(double xx,
		   double yy,
		   double zz,
		   int skip,
		   unsigned int n_k,
		   const std::vector<double>& x,
		   const std::vector<double>& y,
		   const std::vector<double>& z,
		   const std::vector<double>& weight, // relative weights
		   const std::vector<double>& z_last,
		   double xr,
		   double yr)
{
	if (gr_missing(zz))
		return zz;
	double sum = 0.0, sum_w = 0.0;
	for (int k = 0; k < (int)n_k; k++) {
		double w;
		if (k != skip) {
#ifdef USE_APPROX_EXP
			double dx = (xx - x[k]) / xr;
			dx *= dx;
			double dy = (yy - y[k]) / yr;
			dy *= dy;
			double arg = dx + dy;
			// Fearing that the 'inline' didn't work on g++ without 
			// optimization, I've reproduced the formula here.
			w = weight[k]
				/ (0.999448 
				   + arg * (1.023820 
					    + arg * (0.3613967
						     + arg * (0.4169646
							      + arg * (-0.1292509
								       + arg * 0.0499565)))));
#else
			double dx = (xx - x[k]) / xr;
			dx *= dx;
			double dy = (yy - y[k]) / yr;
			dy *= dy;
			double arg = dx + dy;
			w = weight[k] * exp(-arg);
#endif
			sum += w * (z[k] - z_last[k]);
			sum_w += w;
		}
	}
	if (sum_w > 0.0)
		return (zz + sum / sum_w);
	else
		return gr_currentmissingvalue();
}

//`convert columns to grid barnes    [.xr. .yr. .gamma. .iter.]'
static bool
create_grid_barnes(double xr,
		   double yr,
		   double gamma,
		   unsigned int iter,
		   const std::vector<double> &xgood,
		   const std::vector<double> &ygood,
		   const std::vector<double> &zgood,
		   const std::vector<double> &wgood)
{
	// Get grid storage if it does not exist already
	if (!_grid_exists) {
		Require(allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data), 
			err("Insufficient space for matrix"));
	}
	unsigned int numgood = xgood.size();
	Require(numgood > 0,
		err("Cannot `convert columns to grid' since no non-missing column data"));
	_f_xy.set_value(0.0);
	_legit_xy.set_value(true);

	std::vector<double> z_last((size_t)numgood, 0.0);
	std::vector<double> z_last2((size_t)numgood, 0.0); 

	bool warned = false;
	GriTimer t;
	double xr2 = xr, yr2 = yr;
	for (unsigned int iteration = 0; iteration < iter; iteration++) {
		// Interpolate on grid
		for (unsigned int i = 0; i < _num_xmatrix_data; i++) {
			unsigned int j;
			for (j = 0; j < _num_ymatrix_data; j++) {
				_f_xy(i, j) = interpolate_barnes(_xmatrix[i],
								 _ymatrix[j],
								 _f_xy(i, j),
								 -1, // no skip
								 numgood,
								 xgood,
								 ygood,
								 zgood,
								 wgood,
								 z_last,
								 xr2,
								 yr2);
			}
			if (!warned) {
				double frac = (iteration + 1.) * (i + 1.) * (j + 1.);
				frac /= iter * _num_xmatrix_data * _num_ymatrix_data;
				warned = warn_if_slow(&t, frac, "convert columns to grid");
			}
		}
		// Interpolate at data
		unsigned int k;
		for (k = 0; k < numgood; k++) {
			int ix, iy;
			double fx, fy;
			bool in_x = nearest(xgood[k], _xmatrix, _num_xmatrix_data, &ix, &fx);
			bool in_y = nearest(ygood[k], _ymatrix, _num_ymatrix_data, &iy, &fy);
			if (in_x && in_y) {
				value_i_j(ix, iy, xgood[k], ygood[k], &z_last2[k]);
			} else {
				z_last2[k]
					= interpolate_barnes(xgood[k],
							     ygood[k],
							     z_last[k],
							     -1, // no skip
							     numgood,
							     xgood,
							     ygood,
							     zgood,
							     wgood,
							     z_last,
							     xr2,
							     yr2);
			}
		}
		// Calculate change in grid
		double rms_change = 0.0;
		int numgood_ok = 0;
		for (k = 0; k < numgood; k++) {
			if (!gr_missing(z_last[k]) && !gr_missing(z_last2[k])) {
				rms_change += (z_last[k] - z_last2[k]) * (z_last[k] - z_last2[k]);
				numgood_ok++;
			}
		}
		if (numgood_ok)
			rms_change = sqrt(rms_change / numgood_ok);
		else
			rms_change = gr_currentmissingvalue();
		if (_chatty > 0) {
			sprintf(_grTempString, "  Iteration %d: lengthscales = (%g,%g); RMS(z change) = %f\n", iteration + 1, xr2, yr2, rms_change);
			ShowStr(_grTempString);
		}
		// Update z_last
		for (k = 0; k < numgood; k++)
			z_last[k] = z_last2[k];
		// Catch case of gamma=0, which means not to iterate
		if (!gamma)
			break;
		// Alter search range
		xr2 *= sqrt(gamma);
		yr2 *= sqrt(gamma);
	}				// iteration
	return true;
}

// Barnes-interpolate to given (xx,yy), with previously value being zz.
// 'skip' used in cross-validation studies.
static double
interpolate_barnes2(unsigned int k,
		    unsigned int cv,
		    unsigned int n,
		    const std::vector<double>& z,
		    const std::vector<double>& weight,
		    const std::vector<double>& z_last,
		    const GriMatrix<double>& W)
{
	double sum = 0.0, sum_w = 0.0;
	for (unsigned int kk = 0; kk < n; kk++) {
		if (kk != cv) {
			double w = weight[kk] * W(k, kk);
			sum += w * (z[kk] - z_last[kk]);
			sum_w += w;
		}
	}
	if (sum_w)
		return (z_last[k] + sum / sum_w);
	else
		return gr_currentmissingvalue();
}

//`convert columns to grid barnes_cross_validate    [.xr. .yr. .gamma. .iter.]'
static bool
create_grid_barnes_cv(double xr,
		      double yr,
		      double gamma,
		      unsigned int iter,
		      const std::vector<double> &xgood,
		      const std::vector<double> &ygood,
		      const std::vector<double> &zgood,
		      const std::vector<double> &wgood)
{
	Require(allocate_grid_storage(_num_xmatrix_data, _num_ymatrix_data),
		err("Insufficient space for matrix"));
	unsigned int numgood = xgood.size();
	std::vector<double> z_last((size_t)numgood, 0.0); // from last iteration
	std::vector<double> z_last2((size_t)numgood, 0.0); // prevent slurring iterations
	std::vector<double> z_cv((size_t)numgood, 0.0); // predicted by cross-validation
	// Pre-calculate weighting factors to save time, about factor of 2
	GriMatrix<double> *W = new GriMatrix<double>[iter];
	double xr2 = xr, yr2 = yr;
	unsigned int k;
	for (unsigned int i = 0; i < iter; i++) {
		W[i].set_size(numgood, numgood);	
		for (k = 0; k < numgood; k++) {
			for (unsigned int l = 0; l < numgood; l++) {
#ifdef USE_APPROX_EXP
				double dx = (xgood[k] - xgood[l]) / xr2;	dx *= dx;
				double dy = (ygood[k] - ygood[l]) / yr2;	dy *= dy;
				W[i](k, l) = exp_approx(dx + dy);
#else
				double arg;
				arg = - (xgood[k] - xgood[l]) * (xgood[k] - xgood[l]) / xr2 / xr2
					- (ygood[k] - ygood[l]) * (ygood[k] - ygood[l]) / yr2 / yr2;
				W[i](k, l) = exp(arg);
#endif
			}
		}
		xr2 *= sqrt(gamma);
		yr2 *= sqrt(gamma);
	}
	for (unsigned int cv = 0; cv < numgood; cv++) {
		for (k = 0; k < numgood; k++)
			z_last[k] = 0.0;
		xr2 = xr;
		yr2 = yr;
		for (unsigned int i = 0; i < iter; i++) {
			// Interpolate at data
			for (k = 0; k < numgood; k++) {
				z_last2[k] = interpolate_barnes2(k,
								 cv,
								 numgood,
								 zgood,
								 wgood,
								 z_last,
								 W[i]);
			}
			// Update z_last
			for (k = 0; k < numgood; k++)
				z_last[k] = z_last2[k];
			// Catch case of gamma=0, which means not to iterate
			if (!gamma)
				break;		// is this right? BUG ?
			// Alter search range
			xr2 *= sqrt(gamma);
			yr2 *= sqrt(gamma);
		}			// i, iteration
		z_cv[cv] = z_last[cv];
	}				// cv
	// Calculate difference field and stats
	double diff_rms = 0.0;
	double diff_max = 0.0;
	int numgood_ok = 0;
	for (k = 0; k < numgood; k++) {
		double diff = GRI_ABS(zgood[k] - z_cv[k]);
		diff_rms += diff * diff;
		if (diff > diff_max)
			diff_max = diff;
		numgood_ok++;
	}
	diff_rms = sqrt(diff_rms / numgood_ok);
	double data_rms = 0.0;
	for (k = 0; k < numgood; k++)
		data_rms += zgood[k] * zgood[k];
	data_rms = sqrt(data_rms / numgood);
	ShowStr("`convert grid to columns barnes_cross_validation' results:\n");
	sprintf(_grTempString,
		"  %10s %10s %10s %5s %5s %10s %10s %10s\n",
		"xr", "yr", "gamma", "iter", "numgood", "diff_rms", "diff_max", "data_rms");
	ShowStr(_grTempString);
	sprintf(_grTempString, 
		"  %10f %10f %10f %5d %5d %10f %10f %10f\n",
		xr, yr, gamma, iter, numgood, diff_rms, diff_max, data_rms);
	ShowStr(_grTempString);
	sprintf(_grTempString, "%15s %15s %15s %15s %15s\n", 
		"x", "y", "z", "z_pred", "|difference|");
	ShowStr(_grTempString);
	for (k = 0; k < numgood; k++) {
		double diff = GRI_ABS(zgood[k] - z_cv[k]);
		sprintf(_grTempString,
			"  %15f %15f %15f %15f %15f\n",
			xgood[k], ygood[k], zgood[k], z_cv[k], diff);
		ShowStr(_grTempString);	
	}
	delete [] W;
	return true;
}

// Find value in grid
bool
grid_interp(double xx, double yy, double *value)
{
	double          Dx, Dy;	// width/height of domain with point
	double          f0, f1, f2, f3;
	double          missing = gr_currentmissingvalue();
	// Find (ii,jj) such that _xmatrix[ii]<xx<_xmatrix[ii+1].
	// In cases where xx is outside the range, return the
	// endpoint.  Method is brute-force; binary search would be faster
	unsigned int i, j;
	*value = missing;
	if (_xgrid_increasing) {
		if (xx > _xmatrix[_num_xmatrix_data - 1] || xx < _xmatrix[0])
			return false;
		if (xx > _xmatrix[0]) {
			for (i = 0; i < _num_xmatrix_data - 1; i++) {
				if (_xmatrix[i] <= xx && xx <= _xmatrix[i + 1])
					break;
			}
		} else {
			i = 0;
		}
	} else {
		if (xx < _xmatrix[_num_xmatrix_data - 1] || xx > _xmatrix[0])
			return false;
		if (xx < _xmatrix[0]) {
			for (i = 0; i < _num_xmatrix_data - 1; i++) {
				if (_xmatrix[i] >= xx && xx >= _xmatrix[i + 1])
					break;
			}
		} else {
			i = 0;
		}
	}
	if (_ygrid_increasing) {
		if (yy > _ymatrix[_num_ymatrix_data - 1] || yy < _ymatrix[0])
			return false;
		if (yy > _ymatrix[0]) {
			for (j = 0; j < _num_ymatrix_data - 1; j++) {
				if (_ymatrix[j] <= yy && yy <= _ymatrix[j + 1])
					break;
			}
		} else {
			j = 0;
		}
	} else {
		if (yy < _ymatrix[_num_ymatrix_data - 1] || yy > _ymatrix[0])
			return false;
		if (yy < _ymatrix[0]) {
			for (j = 0; j < _num_ymatrix_data - 1; j++) {
				if (_ymatrix[j] >= yy && yy >= _ymatrix[j + 1])
					break;
			}
		} else {
			j = 0;
		}
	}
	assert (i < _num_xmatrix_data);
	assert (j < _num_ymatrix_data);
	double dx;			// x - x_to_left
	double dy;			// y - y_below
	// Fiddle with dx,dy,Dx,Dy, to avoid looking past array
	dx = i == _num_xmatrix_data - 1 ? 0.0 : xx - _xmatrix[i];
	dy = j == _num_ymatrix_data - 1 ? 0.0 : yy - _ymatrix[j];
	if (_legit_xy(i, j) == false
	    || (dx != 0.0 && _legit_xy(i + 1, j) == false)
	    || (dy != 0.0 && _legit_xy(i, j + 1) == false)
	    || (dx != 0.0 && dy != 0.0 && _legit_xy(i + 1, j + 1) == false)) {
		*value = missing;
		return false;
	}
	f0 = _f_xy(i, j);
	f1 = dx != 0 ? _f_xy(i + 1, j) - f0 : 0.0;
	f2 = dy != 0 ? _f_xy(i, j + 1) - f0 : 0.0;
	f3 = dx != 0 && dy != 0 ? _f_xy(i + 1, j + 1) - f0 - f1 - f2 : 0;
	Dx = dx != 0 ? _xmatrix[i + 1] - _xmatrix[i] : 1;
	Dy = dy != 0 ? _ymatrix[j + 1] - _ymatrix[j] : 1;
	*value = f0 + f1 * dx / Dx + f2 * dy / Dy + f3 * dx / Dx * dy / Dy;
	return true;
}
