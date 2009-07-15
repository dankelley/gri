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

// Some classes for Gri
//#define DEBUG 1
#if !defined(_gr_coll_hh_)
#define _gr_coll_hh_
//  GriString:          -- string
//  GriDvector:         -- vector of doubles
//  GriColumn:          -- column (e.g., "x", "y")
//  BlockSource:        -- used by command.cc
//  RpnItem:            -- used by rpncalc.cc
// Some things used in gr.hh also

#include <stddef.h>
#include <stdio.h>

//#include "gr.hh"
#include "errors.hh"
#include "types.hh"
#include "CmdFile.hh"
#include "DataFile.hh"
#include "GCounter.hh"
#include "GriColor.hh"
// EOF indicator.
enum eof_status {
	no_eof,
	eof_after_data,
	eof_before_data
};


class GriString
{
public:
	GriString(const char *s = "");
	GriString(const GriString& n);
	GriString(unsigned int len);
	~GriString() {delete [] value;}
	void convert_slash_to_MSDOS();
	eof_status line_from_FILE(FILE *fp); // Get Line from file, true if EOF
	bool word_from_FILE(FILE *fp, unsigned int *eol); // Get word from file, true if EOF
	bool word_from_DataFile(DataFile& f); // Get word from file, true if EOF
	void fromSTR(const char *s);   // Grow if necessary
	void catSTR(const char *s);	   // Grow if necessary
	char *getValue() const {return value; }
	unsigned int size(void) const {return strlen(value);}
	unsigned int getCapacity() const {return capacity;}
#if 0
	void sed(const char *cmd);	  // Modify by (limited) sed command BUG
#endif
	char& operator[](unsigned int offset); // Access element if it exists
	void draw(double xcm, double ycm, gr_textStyle s, double angle) const;
private:
	char *value;		         // The string, null-term
	unsigned int capacity;		 // Can hold capacity - 1 chars
};

class GriDvector
{
public:
	GriDvector();
	GriDvector(unsigned int length);
	~GriDvector();
	void expand();		// Get more space
	void expand(unsigned int capacity); // Get more space
	void compact();		// Remove things beyond depth
	void push_back(double value); // Store on top
	void pop_back();	// Remove top element
	double topElement();	// Retrieve top
	void erase(double *pos_); // Remove element
	double& operator[](unsigned int offset); // Retrieve at index
	double min(void);	// Min of non-missing
	double max(void);	// Max of non-missing
	double median(void);	// Median of non-missing
	double skewness(void);	// skewness of non-missing
	double kurtosis(void);	// skewness of non-missing
	double mean(void);	// Mean of non-missing
	double stddev(void);	// Std-deviation of non-mis
	double *begin(void);	// Pointer to contents
	void setDepth(unsigned int theDepth); // Set depth
	size_t size();		// Get depth
	unsigned int size_legit(); // Number of good values
	unsigned int capacity(); // Get total capacity
private:
	size_t the_depth;	// Number stored
	size_t the_capacity;	// Number storable
	double *contents;	// Contents
};

class GriColumn : public GriDvector
{
public:
	GriColumn();		
	~GriColumn();
	void setName(const char *theName); // Set name of column
	char *getName();		       // Get name of column
private:
	char *name;
};

// Keep track of the file:line sources for commandline blocks
class BlockSource
{
public:
	BlockSource() {
		filename.assign("");
		line = 0;
		start = NULL;
	}
	BlockSource& operator=(const BlockSource& n) {
#if defined(DEBUG)
		printf("DEBUG:gr_coll.hh BlockSource operator=() [line now is %d]\n",n.get_line());
#endif
		filename = n.get_filename();
		line = n.get_line();
		start = n.get_start();
		offset = n.get_offset();
		return *this;
	}
	BlockSource(const BlockSource& n) {
#if defined(DEBUG)
		printf("DEBUG:gr_coll.hh BlockSource(BlockSource) [line now is %d]\n",n.get_line());
#endif
		filename = n.get_filename();
		line = n.get_line();
		start = n.get_start();
	}
	BlockSource(const char* the_start, const char *the_filename, unsigned int the_line) {
#if defined(DEBUG)
		printf("DEBUG:gr_coll.hh BlockSource([see below],%s,%d)\n[%s]\n",the_filename,the_line,the_start);
#endif
		start = the_start;
		filename = the_filename;
		line = the_line;
	}
	~BlockSource() {
		//filename.string::~string(); 2001-feb-17
	}
	void increment_line(unsigned int line_length) {
		line++;                        
		offset += line_length + 1; // the 1 is for newline
	}
	const char  *get_filename()	const	{return filename.c_str(); }
	unsigned int get_line()		const	{return line; }
	const char  *get_start()        const   {return start; }
	unsigned int get_offset()       const   {return offset; }
	void         move_offset(unsigned int o) {offset += o; }
private:
	std::string filename;	// Name of file containing this src
	unsigned int line;	// Which line we're executing that file
	const char* start;		// Points to next line to execute
	unsigned int offset;	// Next line is at (start+offset)
};

// Operand types.
enum operand_type {
	UNKNOWN,
	VARIABLE_WITH_MISSING_VALUE,
	NOT_OPERAND,
	NUMBER,
	STRING,
	COLUMN_NAME,
	FUNCTION
};

class RpnItem
{
private:
	char *name;
	double value;
	bool valid;
	operand_type type;
public:
	RpnItem();
	RpnItem(const RpnItem& n);
	~RpnItem()				{delete name;}
	void set(const char *the_name, double the_value, operand_type the_type, bool the_valid = true);
	char *getName()		const	{return name;}
	double getValue()	const	{return value;}
	operand_type getType()	const	{return type;}
	bool  getValid()        const   {return valid;}
	RpnItem& operator=(const RpnItem& n);
};

#endif				// ifdef _gr_coll_hh_
