#include <string>
#include <math.h>
#include <stdio.h>

#include	"gr.hh"
#include	"extern.hh"

extern char     _grTempString[];
bool            regressCmd(void);
bool            regress_linearCmd(void);
static int      fit(double x[], double y[], int ndata, std::vector<double>::iterator, double *a, double *b, double *siga, double *sigb, double *chi2, double *q);
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
	std::vector<double> sigma(_colX.size(), 0.0);
	if (_colWEIGHT.size() == _colX.size()) {
		for (i = 0; i < _colWEIGHT.size(); i++) {
			if (_colWEIGHT[i])
				sigma[i] = 1.0 / sqrt(_colWEIGHT[i]);
			else {
				sigma[i] = 1.0e10; // something big
			}
		}
	} else {
		for (i = 0; i < _colX.size(); i++)
			if (!gr_missing(_colX[i]) && !gr_missing(_colY[i]))
				sigma[i] = 1.0;
			else
				sigma[i] = 1.0e10;
	}
	if (!strcmp(_word[1], "y") && !strcmp(_word[3], "x")) {
		// regress y vs x
		int good = fit(_colX.begin(),
			       _colY.begin(),
			       _colX.size(), 
			       sigma.begin(),
			       &a, &b, &siga, &sigb, &chi2, &q);
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
		// regress x vs y
		int good;
		good = fit(_colY.begin(),
			   _colX.begin(),
			   _colX.size(),
			   sigma.begin(),
			   &a, &b, &siga, &sigb, &chi2, &q);
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
static int
fit(double x[], double y[], int ndata,
    std::vector<double>::iterator sig, // std-deviation in y
    double *a, double *b,
    double *siga, double *sigb,
    double *chi2, double *q)
{
	int             i;
	int             good = 0;
	double          wt, t, sxoss, sx = 0.0, sy = 0.0, st2 = 0.0, ss, sigdat;
	*b = 0.0;
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
	sxoss = sx / ss;
	for (i = 0; i < ndata; i++) {
		if (!gr_missing(x[i]) && !gr_missing(y[i]) && !gr_missing(sig[i])) {
			t = (x[i] - sxoss) / sig[i];
			st2 += t * t;
			*b += t * y[i] / sig[i];
		}
	}
	*b /= st2;
	*a = (sy - sx * (*b)) / ss;
	*siga = sqrt((1.0 + sx * sx / (ss * st2)) / ss);
	*sigb = sqrt(1.0 / st2);
	*chi2 = 0.0;
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
	return good;
}
#undef SQR

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
