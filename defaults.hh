/* set up default geometry of plots */
#if !defined(_defaults_)

#define _defaults_

#define GRIINPUTS ".:/usr/local/lib/gri"	/* where to search */

#define ARROWSIZE_DEFAULT               0.2	/* cm */
#define COLUMN_LEN_DEFAULT              256	/* data column length */
#define CONTOUR_FMT_DEFAULT             "%lg"	/* format for contour labels */
#define FONT_DEFAULT            gr_font_Helvetica
#define FONTSIZE_PT_DEFAULT             12.	/* pt */
#define IMAGE_SIZE_WHEN_CONVERTING 	128	/* `convert grid to image' */

/* before 1.043, following numbers were 0.25, 0.5, and 0.25 */
#define LINEWIDTHAXIS_DEFAULT           0.369	/* pt (=rapidograph 6x0) */
#define LINEWIDTH_DEFAULT               0.709	/* pt (=rapidograph 3x0) */
#define LINEWIDTHSYMBOL_DEFAULT         0.369	/* pt (=rapidograph 6x0) */


#define MISSING_VALUE                   1.0e22	/* missing value */
#define SYMBOLSIZE_DEFAULT              0.1	/* radius in cm */
#define TICSIZE_DEFAULT                 0.2	/* cm */
#define X_FMT_DEFAULT                   "%lg"
#define XMARGIN_DEFAULT                 6.0	/* cm */
#define XSIZE_DEFAULT                   10.	/* cm */
#define Y_FMT_DEFAULT                   "%lg"
#define YMARGIN_DEFAULT                 6.0	/* cm */
#define YSIZE_DEFAULT                   10.	/* cm */

// Page limits
#define OFFPAGE_LEFT   -21.6
#define OFFPAGE_RIGHT   43.2
#define OFFPAGE_BOTTOM -27.9
#define OFFPAGE_TOP     55.9

#endif				/* _defaults_ */
