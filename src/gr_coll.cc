/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// Classes for Gri.  See gr_coll.hh for docs
//#define DEBUG_GR_COLL 1		// uncomment to debug

#include <string>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stddef.h>

#include "gr.hh"
#include "extern.hh"
#include "gr_coll.hh"

static const unsigned int CAPACITY_DEFAULT = 32;


GriString::GriString(const char *s)
{
	capacity = 1 + strlen(s);
	value = new char[capacity];
	if (!value) OUT_OF_MEMORY;
	strcpy(value, s);
}
GriString::GriString(const GriString& n)
{
	char *cp = n.getValue();
	capacity = 1 + strlen(cp);
	value = new char[capacity];
	if (!value) OUT_OF_MEMORY;
	strcpy(value, cp);
}
GriString::GriString(unsigned int len)
{
	capacity = 1 + len;
	value = new char[capacity];
	if (!value) OUT_OF_MEMORY;
	value[0] = '\0';
}

void GriString::convert_slash_to_MSDOS()
{
	unsigned int l = strlen(value);
	for (unsigned int i = 0; i < l; i++)
		if (value[i] == '/')
			value[i] = '\\';
}
// Read line from file, enlarging space if needed.  Leave newline
// at end; if file ends before newline, tack one on anyhow.
//
// RETURN true if file at EOF before reading any data
eof_status GriString::line_from_FILE(FILE *fp)
{
	//printf("DEBUG %s:%d line_from_FILE fp at %x\n",__FILE__,__LINE__,int(fp));
	unsigned int i = 0;
	bool got_eof = false;
	if (feof(fp)) {
		value[0] = '\0';
		return eof_before_data;
	}
	do {
		value[i] = getc(fp);
		if ((got_eof = feof(fp)))
			break;
		if (i >= capacity - 1) { // Get more space if required
			capacity += capacity;
			char *more_space = new char[capacity];
			if (!more_space) OUT_OF_MEMORY;
			for (unsigned int j = 0; j <= i; j++)
				more_space[j] = value[j];
			delete [] value;
			value = more_space;
		}
		if (value[i++] == '\n')
			break;
	} while (!got_eof);
	if (got_eof) {
		//printf("got eof i= %d\n",i);
		value[i++] = '\n';	// tack newline on
		value[i] = '\0';
		//printf("%s:%d {{{{{{%s}}}}}}}}}}}\n",__FILE__,__LINE__,value);
		return eof_after_data;
	}
	value[i] = '\0';
	return no_eof;
}

// Read word from file, enlarging if neccessary.  Leave newline if
// found, but if hit eof do not insert extra newline.

// RETURN true if hit EOF

// CHANGE 28AUG95: DO
// NOT KEEP NEWLINE; PUT IT BACK INTO FILE SO 'READ WORD' CAN FLUSH COMMENTS
// AND EXTRA JUNK AT EOL ... PROVISIONAL CHANGE.

bool GriString::word_from_DataFile(DataFile& f)
{
	bool status;
	unsigned int eol;
	status = GriString::word_from_FILE(f.get_fp(), &eol);
	for (unsigned int e = 0; e < eol; e++) {
		//printf("dd %s:%d got eol\n", __FILE__,__LINE__);
		f.increment_line();
	}
	return status;
}
bool GriString::word_from_FILE(FILE *fp, unsigned int *eol)
{
	*eol = 0;
	//printf("dd eol at start is %d\n", *eol);
        if (feof(fp)) {
                value[0] = '\0';
                return true;
        }
	// The default (single or multiple whitespace) is VERY 
	// different from TAB, since the latter requires precisely
	// one TAB.  Also, in the default case, whitespace is skipped, 
	// but not in the TAB case.
        extern char _input_data_separator;
        if (_input_data_separator == ' ') {
		unsigned int i = 0;
		value[i] = getc(fp);
		if (feof(fp)) {
			value[0] = '\0';
			return true;
		}
		// Flush any existing whitespace
		while (isspace(value[i])) {
			//printf(" flushing whitespace [%c] at i=%d\n",value[i],i);
			value[i] = getc(fp);
			if (value[i] == '\n')
				*eol++;
			if (feof(fp)) {
 				value[i] = '\0';
				return true;
			}
		}
		i++;
		do {
			value[i] = getc(fp);
			if (i >= capacity - 1) { // Get more space if required
				capacity += capacity;
				char *more_space = new char[capacity];
				if (!more_space) OUT_OF_MEMORY;
				for (unsigned int j = 0; j <= i; j++)
					more_space[j] = value[j];
				delete [] value;
				value = more_space;
			}
			if (value[i] == '\n') {
				*eol = *eol + 1;
				//printf("dd *  POSITION 1 newline.  eol is now %d\n", *eol);
				ungetc(value[i], fp);
				break;
			}
			if (isspace(value[i]))
				break;
			i++;
		} while (!feof(fp));
		if (feof(fp))
			i--;
		value[i] = '\0';
		return false;
        } else if (_input_data_separator == '\t') {
		// For 'set input data separator TAB' case, require
		// just one tab.  Two tabs implies missing data.
		unsigned int i = 0;
		value[i] = getc(fp);
		if (feof(fp)) {
			value[0] = '\0';
			return true;
		}
		if (value[i] == '\t') {
			value[0] = '\0';
			return false;
		}
		i++;
		do {
			value[i] = getc(fp);
			if (i >= capacity - 1) { // Get more space if required
				capacity += capacity;
				char *more_space = new char[capacity];
				if (!more_space) OUT_OF_MEMORY;
				for (unsigned int j = 0; j <= i; j++)
					more_space[j] = value[j];
				delete [] value;
				value = more_space;
			}
			if (value[i] == '\n') {
				*eol = *eol + 1;
				//printf("dd ** POSITION 2 newline.  eol is now %d\n", *eol);
				ungetc(value[i], fp);
				break;
			}
			if (value[i] == '\t') {
				break;
			}
			i++;
		} while (!feof(fp));
		if (feof(fp))
			i--;
		value[i] = '\0';
		return false;
        } else {
		gr_Error("Internal error _input_data_separator value is not understood.");
		return false;
	}
}


void GriString::fromSTR(const char *s)
{
	unsigned int len = strlen(s);
	if (capacity < len + 1) {
		delete [] value;
		capacity = len + 1;
		value = new char[capacity];
		if (!value) OUT_OF_MEMORY;
	}
	strcpy(value, s);
}
void GriString::catSTR(const char *s)
{
	unsigned int len    = strlen(s);
	unsigned int oldlen = strlen(value);
	if ((1 + len + oldlen) > capacity) {
		capacity += len + 1;
		char *tmp = new char[capacity];
		if (!tmp) OUT_OF_MEMORY;
		strcpy(tmp, value);
		strcat(tmp, s);
		delete [] value;
		value = tmp;
	} else {
		strcat(value, s);
	}
}
#if 0
void GriString::sed(const char *cmd)
// Assume, *WITHOUT CHECKING*, that cmd is of one of the forms
// 	s/A/B/	where 'A' and 'B' are strings
// 	s:A:B:	or any other 'stop-char', as in sed unix command
// Also, require B to be a shorter string than A. (easily fixed)
{
	switch (cmd[0]) {
	case 's': {
		int len_cmd = strlen(cmd);
		int len_value = strlen(value);
		char *copy = new char[1 + strlen(value)];
		char stop = cmd[1];
		char *A = new char [1 + len_cmd];
		char *B = new char [1 + len_cmd];
		int lenA = 0;
		int iA;
		for (iA = 2; iA < len_cmd; iA++) {
			if (cmd[iA] == stop)
				break;
			A[lenA++] = cmd[iA];
		}
		A[lenA] = '\0';
		int lenB = 0;
		int iB;
		for (iB = iA + 1; iB < len_cmd; iB++) {
			if (cmd[iB] == stop)
				break;
			B[lenB++] = cmd[iB];
		}
		B[lenB] = '\0';
		Require2(lenB <= lenA, gr_Error("sed requires lenB <= lenA\n"));
		int iCOPY = 0;
		for (int iVALUE = 0; iVALUE < len_value; iVALUE++) {
			for (iA = 0; iA < lenA; iA++) {
				if (iVALUE + iA > len_value - 1) {
					break;
				}
				if (value[iVALUE + iA] != A[iA]) {
					break;
				}
			}
			if (iA == lenA) {
				for (iB = 0; iB < lenB; iB++) {
					copy[iCOPY++] = B[iB++];
				}
				iVALUE += lenA - 1;
			} else {
				copy[iCOPY++] = value[iVALUE];
			}
		}
		copy[iCOPY] = '\0';
		delete [] A;
		delete [] B;
		strcpy(value, copy);
		delete [] copy;
		break;
	}
	default:
		gr_Error("GriString's sed() command can only do 's' commands");
	}
}
#endif

// Return reference to indicated item, getting new space if exceeds
// capacity.
char&
GriString::operator[](unsigned int offset)
{
	if (offset < capacity)
		return value[offset];
	// Must get more space
	char *cp = new char[capacity = offset + 1];
	if (!cp) OUT_OF_MEMORY;
	strcpy(cp, value);
	delete [] value;
	value = cp;
	// Set to null characters, to prevent returning junk
	for (unsigned int i = strlen(value); i < capacity; i++)
		value[i] = '\0';
	return value[offset];
}

void
GriString::draw(double xcm, double ycm, gr_textStyle s, double angle) const
{
	if (strlen(value) == 0)
		return;
	gr_show_at(value, xcm, ycm, s, angle);
	// Figure bounding box
	double width, ascent, descent;
	gr_stringwidth(value, &width, &ascent, &descent);
#if 0
	printf("DEBUG: %s:%d GriString::Draw with value `%s' xcm=%.1f ycm=%.1f width=%.1f ascent=%.1f descent=%.1f angle=%.1f\n",__FILE__,__LINE__,value, xcm,ycm,width,ascent, descent, angle);
#endif
	double tmpx[4], tmpy[4];	// 0123 from lower-left anticlockwise
	switch (s) {
	case TEXT_LJUST:
		gr_rotate_xy(     0.0, -descent, angle, tmpx + 0, tmpy + 0);
		gr_rotate_xy(   width, -descent, angle, tmpx + 1, tmpy + 1);
		gr_rotate_xy(   width,   ascent, angle, tmpx + 2, tmpy + 2);
		gr_rotate_xy(     0.0,   ascent, angle, tmpx + 3, tmpy + 3);
		break;
	case TEXT_RJUST:
		gr_rotate_xy(  -width, -descent, angle, tmpx + 0, tmpy + 0);
		gr_rotate_xy(     0.0, -descent, angle, tmpx + 1, tmpy + 1);
		gr_rotate_xy(     0.0,   ascent, angle, tmpx + 2, tmpy + 2);
		gr_rotate_xy(  -width,   ascent, angle, tmpx + 3, tmpy + 3);
		break;
	case TEXT_CENTERED:
		gr_rotate_xy(-width/2, -descent, angle, tmpx + 0, tmpy + 0);
		gr_rotate_xy( width/2, -descent, angle, tmpx + 1, tmpy + 1);
		gr_rotate_xy( width/2,   ascent, angle, tmpx + 2, tmpy + 2);
		gr_rotate_xy(-width/2,   ascent, angle, tmpx + 3, tmpy + 3);
		break;
	}
	tmpx[0] += xcm, tmpy[0] += ycm;
	tmpx[1] += xcm, tmpy[1] += ycm;
	tmpx[2] += xcm, tmpy[2] += ycm;
	tmpx[3] += xcm, tmpy[3] += ycm;
	rectangle box(vector_min(tmpx, 4), vector_min(tmpy, 4),
		      vector_max(tmpx, 4), vector_max(tmpy, 4));
	bounding_box_update(box);
}

GriDvector::GriDvector() 
{
	the_depth = 0;
	the_capacity = CAPACITY_DEFAULT;
	contents = new double [the_capacity];
	if (!contents) OUT_OF_MEMORY;
}
GriDvector::GriDvector(unsigned int length)
{
	the_depth = 0;
	the_capacity = length;
	contents = new double [the_capacity];
	if (!contents) OUT_OF_MEMORY;
}
GriDvector::~GriDvector()
{
	delete [] contents;
}
// Get more storage 
void GriDvector::expand()
{
	if (!the_capacity)
		the_capacity = CAPACITY_DEFAULT;
	the_capacity *= 2;
	double *tmp;
	tmp = new double[the_capacity];
	if (!tmp) OUT_OF_MEMORY;
	for (unsigned i = 0; i < the_depth; i++)
		tmp[i] = contents[i];
	delete [] contents;
	contents = tmp;
}
// Get more storage, to specified level
void GriDvector::expand(unsigned int capacity)
{
	if (capacity == the_capacity)
		return;
	if (capacity < the_capacity)
		return;			// Ignore 
	the_capacity = capacity == 0 ? CAPACITY_DEFAULT : capacity;
	double *tmp;
	tmp = new double[the_capacity];
	if (!tmp) OUT_OF_MEMORY;
	for (unsigned int i = 0; i < the_depth; i++)
		tmp[i] = contents[i];
	delete [] contents;
	contents = tmp;
}
// Compact down to 'the_depth' or CAPACITY_DEFAULT, whichever is smaller
void GriDvector::compact()
{
	unsigned int old_capacity = the_capacity;
	the_capacity = the_depth > CAPACITY_DEFAULT 
		? the_depth : CAPACITY_DEFAULT;
	if (the_capacity != old_capacity) {
		double *tmp;
		tmp = new double[the_capacity];
		if (!tmp) OUT_OF_MEMORY;
		for (unsigned int i = 0; i < the_depth; i++)
			tmp[i] = contents[i];
		delete [] contents;
		contents = tmp;
	}
}
void GriDvector::push_back(double value)
{
	while (the_depth > the_capacity - 1)
		expand();
	contents[the_depth++] = value;
}
void GriDvector::pop_back()	// retains storage
{
	if (the_depth)
		the_depth--;
}
double GriDvector::topElement()
{
	return contents[the_depth - 1];
}
void GriDvector::erase(/* iterator */ double *pos_ /*unsigned int offset*/)
{
	unsigned offset = pos_ - contents;
	for (unsigned i = offset; i < the_depth - 1; i++)	
		contents[i] = contents[i + 1];
	the_depth--;
}
double& GriDvector::operator[](unsigned int offset)
{
	if (offset <= the_depth - 1)
		return contents[offset];
	else {
		gr_Error("Trying to access GriDvector out of bounds.");
		return contents[0];	// never done, prevent compiler warning
	}
}
double GriDvector::min()
{
        //printf("%s:%d\n",__FILE__,__LINE__);
	bool first = true;
	double the_min = gr_currentmissingvalue();
	for (unsigned int i = 0; i < the_depth; i++) {
		double tmp = contents[i];
		//printf("%s:%d contents[%d] = %lf ... ",__FILE__,__LINE__,i,tmp);
		if (!gr_missing(tmp)) {
		        //printf("NOT MISSING\n");
			if (first) {
				the_min = tmp;
			} else {
				if (tmp < the_min) {
					the_min = tmp;
				}
			}
			first = false;
		}// else {
		//  printf("MISSING\n");
		//}
	}
	//printf("%s:%d the_min = %lf\n",__FILE__,__LINE__,the_min);
	return the_min;
}
double GriDvector::max()
{
	bool first = true;
	double the_max = gr_currentmissingvalue();
	for (unsigned int i = 0; i < the_depth; i++) {
		double tmp = contents[i];
		//printf("tmp= %lf   ",tmp);
		if (!gr_missing(tmp)) { 
		        //printf("NOT MISSING.\n");
			if (first) {
				the_max = tmp;
			} else {
				if (tmp > the_max) {
					the_max = tmp;
				}
			}
			first = false;
		}// else {
		       //printf("MISSING\n");
		//}
	}
	//printf("%s:%d the_max = %lf\n",__FILE__,__LINE__,the_max);
	return the_max;
}
double GriDvector::median()
{
	double q1, q2, q3;
	histogram_stats(contents, the_depth, &q1, &q2, &q3) ;
	return q2;
}
double GriDvector::mean()
{
	double sum = 0.0;
	long good = 0;
	for (unsigned int i = 0; i < the_depth; i++)
		if (!gr_missing(contents[i])) {
			sum += contents[i];
			good++;
		}
	if (good)
		return double(sum / good);
	else
		return gr_currentmissingvalue();
}
double GriDvector::stddev()
{
	double the_mean = mean();
	double sum = 0.0;
	long good = 0;
	for (unsigned int i = 0; i < the_depth; i++)
		if (!gr_missing(contents[i])) {
			double tmp = (contents[i] - the_mean);
			sum += tmp * tmp;
			good++;
		}
	if (good > 1)
		return double(sqrt(sum / (good - 1)));
	else
		return gr_currentmissingvalue();
}
double GriDvector::skewness()
{
	double the_mean = mean();
	double sum = 0.0;
	long good = 0;
	for (unsigned int i = 0; i < the_depth; i++)
		if (!gr_missing(contents[i])) {
			double tmp = (contents[i] - the_mean);
			sum += tmp * tmp * tmp;
			good++;
		}
	double s = stddev();
	if (good > 0 && s > 0)
		return double((sum / good) / s / s / s);
	else
		return gr_currentmissingvalue();
}
double GriDvector::kurtosis()
{
	double the_mean = mean();
	double sum = 0.0;
	long good = 0;
	for (unsigned int i = 0; i < the_depth; i++)
		if (!gr_missing(contents[i])) {
			double tmp = (contents[i] - the_mean);
			sum += tmp * tmp * tmp * tmp;
			good++;
		}
	double s = stddev();
	// BUG: which kurtosis definition to use???
	// some subtract 3 from the below
	if (good > 1)
		return double(sum / good / s / s / s / s);
	else 
		return gr_currentmissingvalue();
}
double * GriDvector::begin()
{
	return contents;
}
void GriDvector::setDepth(unsigned int depth)
{
	if (depth < the_depth) {
		the_depth = depth;
		compact();
	} else {
		expand(depth);
		the_depth = depth;
	}
}
size_t GriDvector::size()
{
	return the_depth;
}
unsigned int GriDvector::size_legit()
{
	unsigned int num = 0;
	for (unsigned int i = 0; i < the_depth; i++)
		if (!gr_missing(contents[i]))
			num++;
	return num;
}
unsigned int GriDvector::capacity()
{
	return the_capacity;
}

GriColumn::GriColumn()
{
	name = new char[2];
	if (!name) OUT_OF_MEMORY;
	name[0] = '\0';
}
GriColumn::~GriColumn()
{
	delete [] name;
}
void GriColumn::setName(const char *theName)
{
	delete [] name;
	name = new char[1 + strlen(theName)];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, theName);
}
char *GriColumn::getName()
{
	return name;
}

RpnItem::RpnItem()
{
	name = new char[1];
	if (!name) OUT_OF_MEMORY;
	name[0] = '\0';
	value = 0.0;
	valid = true;
	type = UNKNOWN;
}
RpnItem::RpnItem(const RpnItem& n)
{
	name = new char[1 + strlen(n.getName())];
	if (!name) OUT_OF_MEMORY;
	strcpy(name, n.getName());
	value = n.getValue();
	valid = n.getValid();
	type = n.getType();
}
void
RpnItem::set(const char *the_name, double the_value, operand_type the_type, bool the_valid)
{
	if (strlen(the_name) > strlen(name)) {
		delete [] name;
		name = new char[1 + strlen(the_name)];
		if (!name) OUT_OF_MEMORY;
	}
	strcpy(name, the_name);
	value = the_value;
	valid = the_valid;
	type = the_type;
}
RpnItem& RpnItem::operator=(const RpnItem& n)
{
	char *cp = n.getName();
	if (strlen(cp) > strlen(name)) {
		delete [] name;
		name = new char[1 + strlen(cp)];
		if (!name) OUT_OF_MEMORY;
	}
	strcpy(name, cp);
	value = n.getValue();
	valid = n.getValid();
	type = n.getType();
	return *this;
}
