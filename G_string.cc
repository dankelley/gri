#if 0				// Maybe keep for later!
#include <string>
#include <stdio.h>
//##include <string.h>
#include "types.hh"
#include "G_string.hh"

// Read line from file.  If there is a newline character at the
// end of the line in the file, keep it.  Otherwise, tack one on.
//
// RETURN true if got to EOF
bool G_string::line_from_FILE(FILE *fp)
{
	printf("line_from_FILE %s:%d\n",__FILE__,__LINE__);
	assign("");
	if (feof(fp)) {
		printf("line_from_FILE %s:%d.  HEY, eof at start!\n",__FILE__,__LINE__);
		return true;
	}
	char c[2];
	c[1] = '\0';
	do {
		c[0] = getc(fp);
		append(c);
		if (c[0] == '\n')
			break;
	} while (!feof(fp));
	printf("line_from_FILE %s:%d.  done loop.  Got '%s'\n",__FILE__,__LINE__,c_str());
	if (feof(fp)) {
		printf("line_from_FILE %s:%d.  had eof so tacking newline\n",__FILE__,__LINE__);
		append("\n");	// tack newline on
	}
	return false;
}

//OLD** #if 0
//OLD** // Read word from file, enlarging if neccessary.  Leave newline if
//OLD** // found, but if hit eof do not insert extra newline.
//OLD**  // CHANGE 28AUG95: DO
//OLD**  // NOT KEEP NEWLINE; PUT IT BACK INTO FILE SO 'READ WORD' CAN FLUSH COMMENTS
//OLD**  // AND EXTRA JUNK AT EOL ... PROVISIONAL CHANGE.
//OLD** // RETURN true if hit EOF
//OLD** 
//OLD** bool G_string::word_from_FILE(FILE *fp)
//OLD** {
//OLD**     if (feof(fp)) {
//OLD** 	value[0] = '\0';
//OLD** 	return true;
//OLD**     }
//OLD**     unsigned int i = 0;
//OLD**     // Flush any existing whitespace
//OLD**     value[i] = getc(fp);
//OLD**     if (feof(fp)) {
//OLD** 	value[0] = '\0';
//OLD** 	return true;
//OLD**     }
//OLD**     while (isspace(value[i])) {
//OLD** 	value[i] = getc(fp);
//OLD** 	if (feof(fp)) {
//OLD** 	    value[i] = '\0';
//OLD** 	    return true;
//OLD** 	}
//OLD**     }
//OLD**     i++;
//OLD**     do {
//OLD** 	value[i] = getc(fp);
//OLD** 	if (i >= capacity - 1) { // Get more space if required
//OLD** 	    capacity += capacity;
//OLD** 	    char *more_space = new char[capacity];
//OLD** 	    if (!more_space) OUT_OF_MEMORY;
//OLD** 	    for (unsigned int j = 0; j <= i; j++)
//OLD** 		more_space[j] = value[j];
//OLD** 	    delete [] value;
//OLD** 	    value = more_space;
//OLD** 	}
//OLD** 	if (value[i] == '\n') {
//OLD** 	    ungetc(value[i], fp);
//OLD** 	    break;
//OLD** 	}
//OLD** 	if (isspace(value[i]))
//OLD** 	    break;
//OLD** 	i++;
//OLD**     } while (!feof(fp));
//OLD**     if (feof(fp))
//OLD** 	i--;
//OLD**     value[i] = '\0';
//OLD**     return false;
//OLD** }
//OLD** #endif

#if 0
void G_string::sed(const char *cmd)
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
		gr_Error("G_string's sed() command can only do 's' commands");
	}
}
#endif

#if 0
void
G_string::draw(double xcm, double ycm, gr_textStyle s, double angle) const
{
	gr_show_at(value, xcm, ycm, s, angle);
	// Figure bounding box
	double width, ascent, descent;
	gr_stringwidth(value, &width, &ascent, &descent);
#if 0
	printf("\n`%s' xcm=%.1f ycm=%.1f width=%.1f ascent=%.1f descent=%.1f angle=%.1f\n",
	       value, xcm,ycm,width,ascent, descent, angle);
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
#endif
#endif
