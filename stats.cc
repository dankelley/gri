#include <string>
#include <algorithm>		// for sort
#include <vector>		// part of STL
#include <math.h>
#include "gr.hh"
#include "private.hh"


extern char     _grTempString[];

static double    array_at_i(const std::vector<double>& x,
			    double idouble,
			    int n);

void
moment(double *data,
       int n,
       double *ave,
       double *adev,
       double *sdev,
       double *svar,
       double *skew,
       double *kurt)
{
	if (n < 2)
		*ave = *adev = *sdev = *svar = *skew = *kurt = 0.0;
	else {
		int             ngood = 0, i;
		double          s = 0.0, p;
		for (i = 0; i < n; i++)
			if (!gr_missing((double) (*(data + i)))) {
				s += *(data + i);
				ngood++;
			}
		*ave = s / ngood;
		*adev = (*svar) = (*skew) = (*kurt) = 0.0;
		for (i = 0; i < n; i++) {
			if (!gr_missing((double) (*(data + i)))) {
				*adev += fabs(s = *(data + i) - (*ave));
				*svar += (p = s * s);
				*skew += (p *= s);
				*kurt += (p *= s);
			}
		}
		*adev /= ngood;
		*svar /= (ngood - 1);
		*sdev = sqrt(*svar);
		if (*svar) {
			*skew /= (ngood * (*svar) * (*sdev));
			*kurt = (*kurt) / (ngood * (*svar) * (*svar)) - 3.0;
		}
	}
}

// calculate q1, q2 = median, and q3 for n data in x
void
histogram_stats(const double* x,
		unsigned int n,
		double *q1,
		double *q2,
		double *q3)
{
	//void sort(vector<double>::iterator, vector<double>::iterator);
	if (n < 2)
		*q1 = *q2 = *q3 = 0.0;
	else {
		unsigned int ngood = 0;
		std::vector<double> xcopy;
		for (unsigned int i = 0; i < n; i++)
			if (!gr_missing(*(x + i)))
				xcopy.push_back(*(x + i));
		ngood = xcopy.size();
		std::sort(xcopy.begin(), xcopy.end());
		*q1 = array_at_i(xcopy, 0.25 * (ngood - 1), ngood);
		*q2 = array_at_i(xcopy, 0.50 * (ngood - 1), ngood);
		*q3 = array_at_i(xcopy, 0.75 * (ngood - 1), ngood);
	}
}

static double
array_at_i(const std::vector<double>& x, double idouble, int n)
{
	int i = int(floor(idouble));
	if (i < 0)
		return x[0];
	else if (i >= n)
		return x[n - 1];
	else {
		double r = idouble - i;
		return (x[i + 1] * r + x[i] * (1.0 - r));
	}
}
