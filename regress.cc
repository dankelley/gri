#include <string>
#include <math.h>
#include <stdio.h>

#include	"gr.hh"
#include	"extern.hh"

extern char     _grTempString[];
bool            regressCmd(void);
bool            regress_linearCmd(void);
int             fit(double x[], double y[], int ndata, double sig[], int mwt, double *a, double *b, double *siga, double *sigb, double *chi2, double *q);
double          gammln(double xx);
double          gammq(double a, double x);
void            gcf(double *gammcf, double a, double x, double *gln);
void            gser(double *gamser, double a, double x, double *gln);
double          R_linear(double x[], double y[], int n);
double          rms_deviation(double x[], double y[], int n, double a, double b);
static double student_t_025(int nu);

// regressCmd() - handle 'regress' command
bool
regressCmd()
{
	if (!_columns_exist) {
		err("first 'read columns'\n");
		return false;
	}
	switch (_nword) {
	case 4:
		// regress y vs x
		regress_linearCmd();
		break;
	case 5:
		// regress y vs x linear
		if (!strcmp(_word[4], "linear"))
			regress_linearCmd();
		else {
			err("only linear regression permitted now");
			return false;
		}
		break;
	default:
		err("proper: regress y vs x [linear]");
		return false;
	}
	return true;
}

bool
regress_linearCmd()
{
	double          a, b, siga, sigb, chi2, q;
	double          r2;
	unsigned int    i;
	if (_colX.size() < 2 || _colY.size() < 2) {
		err("need more than 2 data points\n");
		return false;
	}
	if (strcmp(_word[2], "vs")) {
		err("keyword 'vs' required. proper: regress y vs x");
		return false;
	}
	if (!strcmp(_word[1], "y") && !strcmp(_word[3], "x")) {
		vector<double> errx(_colX.size(), 0.0);
		// regress y vs x
		for (i = 0; i < _colX.size(); i++)
			if (!gr_missing(_colX[i])
			    && !gr_missing(_colY[i]))
				errx[i] = 1.0;
			else
				errx[i] = 1.0e10;
		int good = fit(_colX.begin(),
			       _colY.begin(),
			       _colX.size(), 
			       errx.begin(),
			       1, &a, &b, &siga, &sigb, &chi2, &q);
		r2 = R_linear(_colX.begin(), _colY.begin(), _colX.size());
		r2 = r2 * r2;
		double deviation = rms_deviation
			(_colX.begin(),
			 _colY.begin(),
			 _colX.size(),
			 a,
			 b);
		if (good > 2) {
			sprintf(_grTempString, "\
y = [%g +/- %g] + [%g +/- %g]x (95%% CI); chi2=%g; q=%5g; R^2=%g; rms deviation=%g (%d good data)\n",
				a, student_t_025(good-2)*siga,
				b, student_t_025(good-2)*sigb,
				chi2, q, r2, deviation, good);
		} else {
			sprintf(_grTempString, "\
y = %g + %g x; chi2=%g; R^2=%g (%d good data)\n",
				a, b, chi2, r2, good);
		}
		PUT_VAR("..coeff0..", a);
		PUT_VAR("..coeff1..", b);
		PUT_VAR("..coeff0_sig..", student_t_025(good-2)*siga);
		PUT_VAR("..coeff1_sig..", student_t_025(good-2)*sigb);
		PUT_VAR("..R2..", r2);
		PUT_VAR("..regression_deviation..", deviation);
		gr_textput(_grTempString);
		return true;
	} else if (!strcmp(_word[1], "x") && !strcmp(_word[3], "y")) {
		vector<double> errx(_colX.size(), 0.0);
		// regress x vs y
		for (i = 0; i < _colX.size(); i++)
			if (!gr_missing(_colX[i])
			    && !gr_missing(_colY[i]))
				errx[i] = 1.0;
			else
				errx[i] = 1.0e10;
		int good;
		good = fit(_colY.begin(),
			   _colX.begin(),
			   _colX.size(),
			   errx.begin(),
			   1, &a, &b, &siga, &sigb, &chi2, &q);
		r2 = R_linear(_colY.begin(), _colX.begin(), _colX.size());
		r2 = r2 * r2;
		double deviation = rms_deviation
			(_colY.begin(),
			 _colX.begin(),
			 _colY.size(),
			 a,
			 b);
		if (good > 2) {
			sprintf(_grTempString, "\
x = [%g +/- %g] + [%g +/- %g]y (95%% CI); chi2=%g; q=%5g; R^2=%g; rms deviation=%g (%d good data)\n",
				a, student_t_025(good-2)*siga,
				b, student_t_025(good-2)*sigb,
				chi2, q, r2, deviation, good);
		} else {
			sprintf(_grTempString, "\
x = %g + %g y; chi2=%g; R^2=%g (%d good data)\n",
				a, b, chi2, r2, good);
		}
		PUT_VAR("..coeff0..", a);
		PUT_VAR("..coeff1..", b);
		PUT_VAR("..coeff0_sig..", student_t_025(good-2)*siga);
		PUT_VAR("..coeff1_sig..", student_t_025(good-2)*sigb);
		PUT_VAR("..R2..", r2);
		PUT_VAR("..regression_deviation..", deviation);
		gr_textput(_grTempString);
		return true;
	} else {
		err("proper: regress y vs x [linear] or regress x vs y [linear]");
		return false;
	}
}

// Compute Pearson correlation coefficient, R.
double
R_linear(double x[], double y[], int n)
{
	// Use formulae in terms of demeaned variables, 
	// for numerical accuracy.
	int             i, non_missing = 0;
	double          xmean = 0.0, ymean = 0.0;
	for (i = 0; i < n; i++) {
		if (!gr_missing(x[i]) && !gr_missing(y[i])) {
			xmean += x[i];
			ymean += y[i];
			non_missing++;
		}
	}
	if (non_missing == 0)
		return 0.0;
	xmean /= non_missing;
	ymean /= non_missing;
	double          syy = 0.0, sxy = 0.0, sxx = 0.0;
	double          xtmp, ytmp;
	for (i = 0; i < n; i++) {
		if (!gr_missing(x[i]) && !gr_missing(y[i])) {
			xtmp = x[i] - xmean;
			ytmp = y[i] - ymean;
			sxx += xtmp * xtmp;
			syy += ytmp * ytmp;
			sxy += xtmp * ytmp;
		}
	}
	return (sxy / sqrt(sxx * syy));
}

// RMS deviation to model y=a+bx
double
rms_deviation(double x[], double y[], int n, double a, double b)
{
	int non_missing = 0;
	double sum = 0.0, dev;
	for (int i = 0; i < n; i++) {
		if (!gr_missing(x[i]) && !gr_missing(y[i])) {
			dev = y[i] - a - b * x[i];
			sum += dev * dev;
			non_missing++;
		}
	}
	if (non_missing == 0)
		return gr_currentmissingvalue();
	return sqrt(sum / non_missing);
}

// Returns number good data
static double   sqrarg;
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)
int
fit(double x[], double y[], int ndata,
    double sig[], int mwt,
    double *a, double *b,
    double *siga, double *sigb,
    double *chi2, double *q)
{
	int             i;
	int             good = 0;
	double          wt, t, sxoss, sx = 0.0, sy = 0.0, st2 = 0.0, ss, sigdat;
	*b = 0.0;
	if (mwt) {
		ss = 0.0;
		for (i = 0; i < ndata; i++) {
			if (!gr_missing(x[i]) && !gr_missing(y[i]) && !gr_missing(sig[i])) {
				wt = 1.0 / SQR(sig[i]);
				sx += x[i] * wt;
				sy += y[i] * wt;
				ss += wt;
				good++;
			}
		}
	} else {
		ss = 0.0;
		for (i = 0; i < ndata; i++) {
			if (!gr_missing(x[i]) && !gr_missing(y[i])) {
				sx += x[i];
				sy += y[i];
				ss += 1.0;
				good++;
			}
		}
	}
	sxoss = sx / ss;
	if (mwt) {
		for (i = 0; i < ndata; i++) {
			if (!gr_missing(x[i]) && !gr_missing(y[i]) && !gr_missing(sig[i])) {
				t = (x[i] - sxoss) / sig[i];
				st2 += t * t;
				*b += t * y[i] / sig[i];
			}
		}
	} else {
		for (i = 0; i < ndata; i++) {
			if (!gr_missing(x[i]) && !gr_missing(y[i])) {
				t = x[i] - sxoss;
				st2 += t * t;
				*b += t * y[i];
			}
		}
	}
	*b /= st2;
	*a = (sy - sx * (*b)) / ss;
	*siga = sqrt((1.0 + sx * sx / (ss * st2)) / ss);
	*sigb = sqrt(1.0 / st2);
	*chi2 = 0.0;
	if (mwt == 0) {
		for (i = 0; i < ndata; i++)
			if (!gr_missing(x[i]) && !gr_missing(y[i]))
				*chi2 += SQR(y[i] - (*a) - (*b) * x[i]);
		*q = 1.0;
		if (good > 2) {
			sigdat = sqrt((*chi2) / (good - 2));
			*siga *= sigdat;
			*sigb *= sigdat;
		} else {
			*siga = -1.0;
			*sigb = -1.0;
		}
	} else {
		for (i = 0; i < ndata; i++)
			if (!gr_missing(x[i]) && !gr_missing(y[i]) && !gr_missing(sig[i]))
				*chi2 += SQR((y[i] - (*a) - (*b) * x[i]) / sig[i]);
		if (good > 2) {
			*q = gammq(0.5 * (good - 2), 0.5 * (*chi2));
			sigdat = sqrt((*chi2) / (good - 2));
			*siga *= sigdat;
			*sigb *= sigdat;
		} else {
			*q = -1.0;
			*siga = -1.0;
			*sigb = -1.0;
		}
	}
	return good;
}

#undef SQR

double
gammln(double xx)
{
	double          x, tmp, ser;
	static double   cof[6] =
	{76.18009173, -86.50532033, 24.01409822,
	 -1.231739516, 0.120858003e-2, -0.536382e-5};
	int             j;
	x = xx - 1.0;
	tmp = x + 5.5;
	tmp -= (x + 0.5) * log(tmp);
	ser = 1.0;
	for (j = 0; j <= 5; j++) {
		x += 1.0;
		ser += cof[j] / x;
	}
	return -tmp + log(2.50662827465 * ser);
}

double
gammq(double a, double x)
{
	double          gamser, gammcf, gln;
	if (x < 0.0 || a <= 0.0) {
		err("regress: Invalid arguments in routine GAMMQ");
		return 0;
	}
	if (x < (a + 1.0)) {
		gser(&gamser, a, x, &gln);
		return 1.0 - gamser;
	} else {
		gcf(&gammcf, a, x, &gln);
		return gammcf;
	}
}

#define ITMAX 100
#define EPS 3.0e-7

void
gcf(double *gammcf, double a, double x, double *gln)
{
	int             n;
	double          gold = 0.0, g, fac = 1.0, b1 = 1.0;
	double          b0 = 0.0, anf, ana, an, a1, a0 = 1.0;
	*gln = gammln(a);
	a1 = x;
	for (n = 0; n < ITMAX; n++) {
		an = (double) n;
		ana = an - a;
		a0 = (a1 + a0 * ana) * fac;
		b0 = (b1 + b0 * ana) * fac;
		anf = an * fac;
		a1 = x * a0 + anf * a1;
		b1 = x * b0 + anf * b1;
		if (a1) {
			fac = 1.0 / a1;
			g = b1 * fac;
			if (fabs((g - gold) / g) < EPS) {
				*gammcf = exp(-x + a * log(x) - (*gln)) * g;
				return;
			}
			gold = g;
		}
	}
	err("regress: a too large, ITMAX too small in routine GCF");
	return;
}

#undef ITMAX
#undef EPS

#define ITMAX 100
#define EPS 3.0e-7
void
gser(double *gamser, double a, double x, double *gln)
{
	int             n;
	double          sum, del, ap;
	*gln = gammln(a);
	if (x <= 0.0) {
		if (x < 0.0) {
			err("regress:x less than 0 in routine GSER");
			return;
		}
		*gamser = 0.0;
		return;
	} else {
		ap = a;
		sum = 1.0 / a;
		del = sum;
		for (n = 0; n < ITMAX; n++) {
			ap += 1.0;
			del *= x / ap;
			sum += del;
			if (fabs(del) < fabs(sum) * EPS) {
				*gamser = sum * exp(-x + a * log(x) - (*gln));
				return;
			}
		}
		err("regress:a too large, ITMAX too small in routine GSER");
		return;
	}
}

#undef ITMAX
#undef EPS

// From table in a book.
double
student_t_025(int nu)
{
	static double t_025[30] = {
		12.706,			// for nu=1
		4.303,			// for nu=2
		3.182,
		2.776,

		2.571,
		2.447,
		2.365,
		2.306,
		2.262,

		2.228,
		2.201,
		2.179,
		2.160,
		2.145,
	
		2.131,
		2.120,
		2.110,
		2.101,
		2.093,
	
		2.086,
		2.080,
		2.074,
		2.069,
		2.064,
	
		2.060,
		2.056,
		2.052,
		2.048,
		2.045,

		2.042			// for nu=30
	};
	if (nu < 1)
		return t_025[0];	// dunno what to do
	else if (nu <= 30)
		return t_025[nu - 1];
	else if (nu <= 40)
		return 2.021;
	else if (nu <= 60)
		return 2.000;
	else if (nu <= 120)
		return 1.98;
	else
		return 1.96;
}
