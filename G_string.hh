#if 0
#if !defined(_GString_hh_)
#define _GString_hh_

#include <string>
#include "types.hh"


class G_string : public string
{
public:
	bool line_from_FILE(FILE *fp); // Get Line from file, true if EOF
#if 0
	bool word_from_FILE(FILE *fp); // Get a word from file, true if EOF
#endif
#if 0
	void draw(double xcm, double ycm, gr_textStyle s, double angle) const;
#endif
#if 0
	void sed(const char *cmd);	  // Modify by (limited) sed command
#endif
};
#endif
#endif
