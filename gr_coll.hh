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

#include "errors.hh"
#include "types.hh"
#include "CmdFile.hh"
#include "DataFile.hh"
#include "GCounter.hh"
#include "GriColor.hh"

class GriString
{
public:
	GriString(const char *s = "");
	GriString(const GriString& n);
	GriString(unsigned int len);
	~GriString() {delete [] value;}
	void convert_slash_to_MSDOS();
	bool line_from_FILE(FILE *fp); // Get Line from file, true if EOF
	bool word_from_FILE(FILE *fp); // Get word from file, true if EOF
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
	void expand();		                 // Get more space
	void expand(unsigned int capacity);		 // Get more space
	void compact();				 // Remove things beyond depth
	void push_back(double value);		 // Store on top
	void pop_back();				 // Remove top element
	double topElement();			 // Retrieve top
	void erase(double *pos_);	                 // Remove element
	double& operator[](unsigned int offset);	 // Retrieve at index
	double min(void);				 // Min of non-missing
	double max(void);				 // Max of non-missing
	double median(void);			 // Median of non-missing
	double mean(void);				 // Mean of non-missing
	double stddev(void);			 // Std-deviation of non-mis
	double *begin(void);			 // Pointer to contents
	void setDepth(unsigned int theDepth);	 // Set depth
	size_t size();			         // Get depth
	unsigned int size_legit();	                 // Number of good values
	unsigned int capacity();			 // Get total capacity
private:
	size_t the_depth;		// Number stored
	size_t the_capacity;	// Number storable
	double *contents;		// Contents
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
		filename = new char[1];
		if (!filename) OUT_OF_MEMORY;
		filename[0] = '\0';
		line = 0;
		offset = 0;
	}
	BlockSource& operator=(const BlockSource& n) {
#if defined(DEBUG)
		printf("DEBUG:gr_coll.hh BlockSource operator=() [line now is %d]\n",n.getLine());
#endif
		if (strlen(n.getFilename()) > strlen(filename)) {
			filename = new char[1 + strlen(n.getFilename())];
			if (!filename) OUT_OF_MEMORY;
		}
		strcpy(filename, n.getFilename());
		line = n.getLine();
		offset =  n.getOffset();
		return *this;
	}
	BlockSource(const BlockSource& n) {
#if defined(DEBUG)
		printf("DEBUG:gr_coll.hh BlockSource(BlockSource) [line now is %d]\n",n.getLine());
#endif
		filename = new char[1 + strlen(n.getFilename())];
		if (!filename) OUT_OF_MEMORY;
		strcpy(filename, n.getFilename());
		line = n.getLine();
		offset =  n.getOffset();
	}
	BlockSource(const char *the_filename, unsigned int the_line) {
#if defined(DEBUG)
		printf("DEBUG:gr_coll.hh BlockSource(%s,%d)\n",the_filename,the_line);
#endif
		filename = new char [1 + strlen(the_filename)];
		if (!filename) OUT_OF_MEMORY;
		strcpy(filename, the_filename);
		line = the_line;
		offset = 0;		// ???
	}
	~BlockSource() {
		// Oddly, a memory fault occurs on HP computers with g++ 2.7.x
		// compilers, when cleaning up at the deletion of the global
		// BlockSource vector.  Since this only happens at exit() time
		// anyway, may as well just skip it I guess.  We should really
		// be cleaning up memory, but what the heck, it's better to 
		// be messy than crash.
		//
		// This measure introduced 23 April 1996, in version 2.6.2.
		//
		// Thanks to Toru Suzuki for help on this (see email archive for 
		// more details).
#if !defined(OS_IS_HPUX)
		delete [] filename;
#endif
	}
#if 0
	void setFilenameLine(const char *the_filename, unsigned int the_line) {
		delete [] filename;
		filename = new char [1 + strlen(the_filename)];
		if (!filename) OUT_OF_MEMORY;
		strcpy(filename, the_filename);
		line = the_line;
	}
#endif
	void incrementOffset()		{offset++;}
	unsigned int getOffset()	const	{return offset;}
	char *getFilename()		const	{return filename;}
	unsigned int getLine() 	const	{/*printf("DEBUG:gr_coll.hh: getLine returning %d\n",line);*/return line;}
private:
	char *filename;		// File containing this src
	unsigned int line;		// Line in that file
	unsigned int offset;	// Offset from this line
};

// Operand types.
typedef enum {
	NOT_OPERAND,
	NUMBER,
	STRING,
	COLUMN_NAME,
	FUNCTION
}               operand_type;

class RpnItem
{
private:
	char *name;
	double value;
	operand_type type;
public:
	RpnItem();
	RpnItem(const RpnItem& n);
	~RpnItem()				{delete name;}
	void set(const char *the_name, double the_value, operand_type the_type);
	char *getName()		const	{return name;}
	double getValue()		const	{return value;}
	operand_type getType()	const	{return type;}
	RpnItem& operator=(const RpnItem& n);
};

#endif				// ifdef _gr_coll_hh_
