#include        <string>
#include        <math.h>
#include        <stdio.h>
#include        <ctype.h>
#include        "gr.hh"
#include	"extern.hh"
#include        "GriPath.hh"
#include        "superus.hh"
#include	"defaults.hh"
double gr_current_descender(void);


#define	default_fontID		gr_font_Helvetica
#define default_encoding        font_encoding_isolatin1
#define	default_fontsize_pt	12.0

static gr_font  CurrentFont = {
	default_fontID,
	default_encoding,				
	default_fontsize_pt
};

// Q: should this be done in Moveup() routine? [then what about $N$N though]
#define START_NEW_TEXT {\
    if (_output_file_type == postscript && _grWritePS) { \
	fprintf(_grPS, "(");\
	check_psfile();\
    }\
}

#define STOP_OLD_TEXT {\
    if (_output_file_type == postscript && _grWritePS) {\
        fprintf(_grPS, ") sh\n");\
        check_psfile();\
    }\
}


enum position {Superscript, Subscript};	// Indicator

// Use spacing patterned on results of a TeX example (using Large font). All
// quantities are multiples of Mspace.
static const double SubSize = 0.75; // relative height of subscripts = 6/8
static const double SuperSize = 0.75; // relative height of superscripts = 6/8 
static const double SuperMoveUp =0.625;	// Move up for super = 5/8
static const double SubMoveDown =0.375;	// Move down for sub = 3/8 



#define PS_showpage     "showpage\n"
#define PS_stroke       "s\n"

extern FILE    *_grPS;
extern FILE    *_grSVG;
extern bool     _grNeedBegin;
extern bool     _grPathExists;
extern bool     _grWritePS;

static void     gr_drawstring(const char *s);
static int      index_for_math_symbol(char *s);	// base routine
static double   gr_charwidth_cm(int c, int font, double fontsize_pt);
static void     gr_DrawChar(const char *c);
static void     gr_setfont_fontsize(gr_fontID newID, bool force = false);
static void     ClearStack(void);
static void     PopStack(void);
static void     MoveDown(void);
static void     MoveUp(void);
static void     MoveHorizontally(double em_distance);
static char    *symbol_in_math(const char *sPtr, int *inc);

gr_font_info    font_list[] =
{
	{gr_font_Courier, "Courier"},
	{gr_font_CourierOblique, "Courier-Oblique"},
	{gr_font_CourierBold, "Courier-Bold"},
	{gr_font_CourierBoldOblique, "Courier-BoldOblique"},
	{gr_font_Helvetica, "Helvetica"},
	{gr_font_HelveticaBold, "Helvetica-Bold"},
	{gr_font_HelveticaOblique, "Helvetica-Oblique"},
	{gr_font_PalatinoRoman, "Palatino-Roman"},
	{gr_font_PalatinoItalic, "Palatino-Italic"},
	{gr_font_PalatinoBold, "Palatino-Bold"},
	{gr_font_PalatinoBoldItalic, "Palatino-BoldItalic"},
	{gr_font_Symbol, "Symbol"},
	{gr_font_TimesRoman, "Times-Roman"},
	{gr_font_TimesItalic, "Times-Italic"},
	{gr_font_TimesBold, "Times-Bold"},
	{gr_font_TimesBoldItalic, "Times-BoldItalic"},
	{gr_font_end_of_list, ""}
};

// Draw text at specified location.
void
gr_show_at(/*const*/ char *s, double xcm, double ycm, gr_textStyle style, double angle_deg)
{
	double          oldfontsize_pt = gr_currentfontsize_pt();
	gr_fontID       oldfontID = gr_currentfont();
	double          width_cm, ascent_cm, descent_cm;
	if (0.0 == gr_currentfontsize_pt() || !strlen(s)) {
		return;
	}
	rectangle box;
	extern bool _warn_offpage;
	if (_warn_offpage 
	    && ( xcm < OFFPAGE_LEFT 
		 || xcm > OFFPAGE_RIGHT
		 || ycm < OFFPAGE_BOTTOM
		 || ycm > OFFPAGE_TOP)) {
		warning("Drawing text at a location which is offpage.");
	}
	switch (_output_file_type) {
	case  postscript:
		fprintf(_grPS, "%% gr_show_at() BEGIN\n");
		break;
	case svg: {
		double r, g, b;
		_griState.color_text().getRGB(&r, &g, &b);
#if 1
		char *fn;
		switch(CurrentFont.id) {
		case gr_font_Courier:            fn = "Courier";            break;
		case gr_font_CourierOblique:     fn = "Courier-Italic";     break;
		case gr_font_CourierBold:        fn = "Courier-Bold";       break;
		case gr_font_CourierBoldOblique: fn = "Courier-BoldItalic"; break;
		case gr_font_Helvetica:          fn = "Helvetica";          break;
		case gr_font_HelveticaOblique:   fn = "Helvetica-Italic";   break;
		case gr_font_HelveticaBold:      fn = "Helvetica-Bold";     break;
		case gr_font_PalatinoRoman: 
		case gr_font_PalatinoItalic:
		case gr_font_PalatinoBold:
		case gr_font_PalatinoBoldItalic:
			fn = "Times";
			warning("SVG cannot handle Palatino font yet");
			break;
		case gr_font_Symbol:             fn = "Symbol";           break;
		case gr_font_TimesRoman:         fn = "Times";            break;
		case gr_font_TimesItalic:        fn = "Times-Italic";     break;
		case gr_font_TimesBold:          fn = "Times-Bold";       break;
		case gr_font_TimesBoldItalic:    fn = "Times-BoldItalic"; break;
		default: 
			fn = "Times";
			warning("SVG defaulting to Times font");
			break;
		}
#endif
		fprintf(_grSVG, "<text\nx=\"%.3f\"\ny=\"%.3f\"\nstyle=\"font-family:%s; font-size:%.2f; fill:#%02x%02x%02x; font-style:normal;\">\n", xcm * PT_PER_CM, ycm * PT_PER_CM, fn, gr_currentfontsize_pt(), int(255*r+0.5), int(255*g+0.5), int(255*b+0.5));
		}
		break;
	case  gif:
		fprintf(stderr, "INTERNAL error at %s:%d -- nothing known for GIF\n\n", __FILE__, __LINE__);
		exit(99);
		break;
	}
	if (_output_file_type == svg) {
		fprintf(stderr, "%s:%d attempting highly approximate text output of '%s' at %f %f cm\n", __FILE__, __LINE__, s, xcm, ycm);
		fprintf(_grSVG, "%s\n", s);
		if (style != TEXT_LJUST) fprintf(stderr, "%s:%d ignoring justification (code=%d) of text '%s'\n", __FILE__,__LINE__,style,s);
		if (angle_deg != 0.0) fprintf(stderr, "%s:%d ignoring angle %f of text '%s'\n", __FILE__,__LINE__,angle_deg,s);
	}


	void set_ps_color(char what);
	set_ps_color('t');
	gr_setfontsize_pt(oldfontsize_pt);
	gr_setfont(oldfontID);
	gr_stringwidth(s, &width_cm, &ascent_cm, &descent_cm);
	switch (style) {
	case TEXT_LJUST:
		gr_moveto_cm(xcm, ycm);
		if (_grWritePS && fabs(angle_deg) > 0.1)
			fprintf(_grPS, "%.2f rotate ", angle_deg);
		gr_drawstring(s);
		// This box not tested specifically
		box.set(0, -descent_cm, width_cm, ascent_cm);
		box.rotate(angle_deg);
		box.shift_x(xcm);
		box.shift_y(ycm);
		break;
	case TEXT_RJUST:
		if (_grWritePS) {
			fprintf(_grPS, "%.1f %.1f m ",
				PT_PER_CM * (xcm - width_cm * cos(angle_deg / DEG_PER_RAD)),
				PT_PER_CM * (ycm - width_cm * sin(angle_deg / DEG_PER_RAD)));
			if (fabs(angle_deg) > 0.1)
				fprintf(_grPS, "%.2f rotate ", angle_deg);
		}
		gr_drawstring(s);
		// This box not tested specifically
		box.set(-width_cm, -descent_cm, 0.0, ascent_cm);
		box.rotate(angle_deg);
		box.shift_x(xcm);
		box.shift_y(ycm);
		break;
	case TEXT_CENTERED:
		if (_grWritePS) {
			fprintf(_grPS, "%.1f %.1f m ",
				PT_PER_CM * (xcm - 0.5 * width_cm * cos(angle_deg / DEG_PER_RAD)),
				PT_PER_CM * (ycm - 0.5 * width_cm * sin(angle_deg / DEG_PER_RAD)));
			if (fabs(angle_deg) > 0.1)
				fprintf(_grPS, "%.2f rotate ", angle_deg);
		}
		gr_drawstring(s);
		box.set(-width_cm/2, -descent_cm, width_cm/2, ascent_cm);
		box.rotate(angle_deg);
		box.shift_x(xcm);
		box.shift_y(ycm);
		break;
	default:
		warning("gr_show_at type is UNKNOWN\n");
	}
	if (_grWritePS && fabs(angle_deg) > 0.1) {
		fprintf(_grPS, "%.2f rotate ", -angle_deg);
		check_psfile();
	}
	switch (_output_file_type) {
	case  postscript:
		fprintf(_grPS, "%% gr_show_at() END\n");
		break;
	case svg:
		fprintf(_grSVG, "</text>\n");
		break;
	case gif:
		fprintf(stderr, "INTERNAL error at %s:%d -- nothing known for GIF\n\n", __FILE__, __LINE__);
		exit(99);
		break;
	}
	// Update bounding box
	bounding_box_update(box);
	_drawingstarted = true;
}

// gr_drawstring() -- draw string, including font changes &super/subscripts
static position pstack[100];
static int      istack = 0;
static void
gr_drawstring(const char *s)
{
        char slast = '\0';
	int             slen = strlen(s);
	bool            inmath = false;
	gr_fontID       original_font = gr_currentfont();
	gr_fontID       current_font = original_font;
	gr_fontID       slant_font = original_font;	// prevent compiler warning
	double          original_fontsize = gr_currentfontsize_pt();
	bool            know_slant_font = false;
	if (slen <= 0)
		return;
	if (0.0 == gr_currentfontsize_pt())
		return;
	ClearStack();
	// Figure out slant font, if there is an appropriate one
	switch (original_font) {
	case gr_font_TimesRoman:
		slant_font = gr_font_TimesItalic;
		know_slant_font = true;
		break;
	case gr_font_TimesBold:
		slant_font = gr_font_TimesBoldItalic;
		know_slant_font = true;
		break;
	case gr_font_Helvetica:
		slant_font = gr_font_HelveticaOblique;
		know_slant_font = true;
		break;
	case gr_font_HelveticaBold:
		slant_font = gr_font_HelveticaBoldOblique;
		know_slant_font = true;
		break;
	case gr_font_Courier:
		slant_font = gr_font_CourierOblique;
		know_slant_font = true;
		break;
	case gr_font_CourierBold:
		slant_font = gr_font_CourierBoldOblique;
		know_slant_font = true;
		break;
	case gr_font_PalatinoRoman:
		slant_font = gr_font_PalatinoItalic;
		know_slant_font = true;
		break;
	case gr_font_PalatinoBold:
		slant_font = gr_font_PalatinoBoldItalic;
		know_slant_font = true;
		break;
	default:
		know_slant_font = false;
	}
	// Scan through whole string.
	START_NEW_TEXT;
	while (*s != '\0') {
		if (*s == '-' && CurrentFont.encoding == font_encoding_isolatin1) {
			// Use a different character to avoid looking like underscore.
			if (_grWritePS) {
				STOP_OLD_TEXT;
				fprintf(_grPS, "(\\255) sh\n");	// endash
				check_psfile();
				START_NEW_TEXT;
			}
			s++;
			continue;
		}
		// Figure out whether entering or leaving math mode; enter/leave if
		// find $ without preceeding \.  Thus a$b$ has math but a\$b\$ does
		// not.
		if (*s == '$' && slast != '\\') {
			if (inmath) {
				// Were in math; now go back to original font.
				inmath = false;
				if (current_font != original_font) {
					current_font = original_font;
					STOP_OLD_TEXT;
					gr_setfont(current_font);
					START_NEW_TEXT;
				}
			} else {
				// Go to Italic/Oblique font, as case may be.  Unfortunately,
				// PostScript uses different names for this slanted font.
				inmath = true;
				if (know_slant_font) {
					current_font = slant_font;
					STOP_OLD_TEXT;
					gr_setfont(current_font);
					START_NEW_TEXT;
				}
			}
			slast = *s++;
			continue;
		}
		// Handle math mode.  This code is a little kludgy, so be carefull.
		if (inmath) {
			if (*s == '^') {
				// Handle superscripts
				slast = *s++;
				if (*s == '\0') {
					// Odd -- nothing to superscript
					if (current_font != original_font) {
						STOP_OLD_TEXT;
						gr_setfontsize_pt(original_fontsize);
						gr_setfont(original_font);
					}
					return;
				} else if (*s == '{') {
					// Several characters to superscript
					pstack[istack++] = Superscript;
					MoveUp();
				} else if (*s == '\\') {
					// Math character to superscript
					int             inc;
					char *            insert;
					insert = symbol_in_math(s, &inc);
					if (inc) {
						gr_fontID       oldfontID = gr_currentfont();
						pstack[istack++] = Superscript;
						MoveUp();
						STOP_OLD_TEXT;
						gr_setfont(gr_font_Symbol);
						if (_grWritePS) {
							fprintf(_grPS, "(%s) sh\n", insert);
							check_psfile();
						}
						gr_setfont(oldfontID);
						START_NEW_TEXT;
						s += inc;
						PopStack();
					}
				} else {
					// Single character to superscript
					pstack[istack++] = Superscript;
					MoveUp();
					// Draw single character in math mode.  If it's a digit,
					// do not do in italics!
					if (isdigit(*s) || ispunct(*s)) {
						if (*s == '/' && !isdigit(slast)) {
							gr_DrawChar(s);
						} else {
							STOP_OLD_TEXT;
							gr_setfont(original_font);
							START_NEW_TEXT;
							gr_DrawChar(s);
							STOP_OLD_TEXT;
							gr_setfont(slant_font);
							START_NEW_TEXT;
						}
					} else {
						gr_DrawChar(s);
					}
					PopStack();
				}
			} else if (*s == '_') {
				// Handle subscript
				slast = *s++;
				if (*s == '\0') {
					// Odd -- nothing to subscript
					if (current_font != original_font) {
						STOP_OLD_TEXT;
						gr_setfontsize_pt(original_fontsize);
						gr_setfont(original_font);
					}
					return;
				} else if (*s == '{') {
					// Several characters to subscript
					pstack[istack++] = Subscript;
					MoveDown();
				} else if (*s == '\\') {
					// Math character to subscript
					int             inc;
					char *            insert;
					insert = symbol_in_math(s, &inc);
					if (inc) {
						gr_fontID       oldfontID = gr_currentfont();
						pstack[istack++] = Subscript;
						MoveDown();
						STOP_OLD_TEXT;
						gr_setfont(gr_font_Symbol);
						if (_grWritePS) {
							fprintf(_grPS, "(%s) sh\n", insert);
							check_psfile();
						}
						gr_setfont(oldfontID);
						START_NEW_TEXT;
						s += inc;
						PopStack();
					}
				} else {
					// Single character to subscript
					pstack[istack++] = Subscript;
					MoveDown();
					// Draw single character in math mode.  If it's a digit,
					// do not do in italics!
					if (isdigit(*s) || ispunct(*s)) {
						if (*s == '/' && !isdigit(slast)) {
							gr_DrawChar(s);
						} else {
							STOP_OLD_TEXT;
							gr_setfont(original_font);
							START_NEW_TEXT;
							gr_DrawChar(s);
							STOP_OLD_TEXT;
							gr_setfont(slant_font);
							START_NEW_TEXT;
						}
					} else {
						gr_DrawChar(s);
					}
					PopStack();
				}
			} else if (*s == '{') {
				;		// EMPTY
			} else if (*s == '}') {	// finished with super/sub in math
				PopStack();
			} else if (*s == '\\') {
				// Substitute math symbol, unless it's
				// an escaped string
				int inc;
				char *insert;
				if (*(s + 1) == '$') {
					slast = *s++;
				} else if (*(s + 1) == ',') {
					slast = *s++;
					MoveHorizontally(0.1666666);	// thinspace
				} else if (*(s + 1) == '!') {
					slast = *s++;
					MoveHorizontally(-0.1666666);	// neg thinspace
				} else if (*(s + 1) == '"') {
					slast = *s++;
				} else if (*(s + 1) == '\\') {
					slast = *s++;
				} else if (*(s + 1) == '{' || *(s + 1) == '}') {
					STOP_OLD_TEXT;
					gr_setfont(original_font);
					START_NEW_TEXT;
					gr_DrawChar(s + 1);
					STOP_OLD_TEXT;
					gr_setfont(slant_font);
					START_NEW_TEXT;
					slast = *s++;
				} else {
					insert = symbol_in_math(s, &inc);
					if (inc) {
						// math symbol in symbol font
						gr_fontID oldfontID = gr_currentfont();
						STOP_OLD_TEXT;
						gr_setfont(gr_font_Symbol);
						if (_grWritePS) {
							fprintf(_grPS, "(%s) sh\n", insert);
							check_psfile();
						}
						gr_setfont(oldfontID);
						START_NEW_TEXT;
						s += inc;
					} else {
						// Not a known math-mode symbol, so just 
						// draw it.  Is this the right thing to do?
						gr_DrawChar(s + 1);
					}
				}
			} else {
				// Draw single character in math mode.
				// If it's a digit, do not use italics.
				if (isdigit(*s) || ispunct(*s)) {
					if (*s == '/' && !isdigit(slast)) {
						gr_DrawChar(s);
					} else {
						STOP_OLD_TEXT;
						gr_setfont(original_font);
						START_NEW_TEXT;
						gr_DrawChar(s);
						STOP_OLD_TEXT;
						gr_setfont(slant_font);
						START_NEW_TEXT;
					}
				} else {
					gr_DrawChar(s);
				}
			}
		} else {
			// draw simple character outside math mode
			if (*s == '\\') {
				if (*(s + 1) == '$') {
					slast = *s++;
				} else if (*(s + 1) == '"') {
					slast = *s++;
				} else if (*(s + 1) == '\\') {
					slast = *s++;
				}
			}
			gr_DrawChar(s);
		}
		slast = *s++;
	}
	STOP_OLD_TEXT;
	gr_setfontsize_pt(original_fontsize);
	gr_setfont(original_font);
	_drawingstarted = true;
	return;
}

// set fontsize in points
void
gr_setfontsize_pt(double fontsize_pt)
{
	if (fontsize_pt < 0.0)
		CurrentFont.size_pt = default_fontsize_pt;
	else
		CurrentFont.size_pt = fontsize_pt;
	gr_setfont_fontsize(CurrentFont.id);
}

// Set font encoding
void
gr_set_font_encoding(gr_font_encoding encoding)
{
	CurrentFont.encoding = encoding;
}
// Get font encoding
gr_font_encoding
gr_current_font_encoding()
{
	return CurrentFont.encoding;
}

/*
 * gr_currentfont() -- find current font synopsis int gr_currentfont()
 * description:  gets the current font,as set by gr_setfont(). return value:
 * current font number.
 */
gr_fontID
gr_currentfont()
{
	return CurrentFont.id;
}

/*
 * gr_currentfontsize_pt() -- return current fontsize in points
 */
double
gr_currentfontsize_pt()
{
	return CurrentFont.size_pt;
}

/*
 * gr_setfont() --  set new font. SYNOPSIS void	gr_setfont(int new_font)
 * DESCRIPTION:  Sets the font for future string drawing to 'new_font'. These
 * fonts are predefined: Times_Roman Helvetica Courier Symbol Palatino-Roman
 * Palatino-Italic.
 * 
 */
void
gr_setfont(gr_fontID newID, bool force /* default false */)
{
	gr_setfont_fontsize(newID, force);
}

static void
gr_setfont_fontsize(gr_fontID newID, bool force)
{
	int             i = 0;
	static bool     have_set_font = false;
	static gr_font  last_font;
	/* Search the font list */
	while (font_list[i].id != gr_font_end_of_list) {
		if (newID == font_list[i].id) {
			/* Found the font, but ignore request if no change */
			if (force
			    || (!have_set_font
				|| newID != last_font.id
				|| CurrentFont.encoding != last_font.encoding
				|| CurrentFont.size_pt != last_font.size_pt)) {
				CurrentFont.id = newID;
				if (!_grNeedBegin) {
					/*
					 * Don't try to write if haven't done gr_begin() yet,
					 * since then will ruin things like
					 * gr_setup_ps_filename();
					 */
					if (_grWritePS) {
						switch (CurrentFont.encoding) {
						case font_encoding_standard:
							fprintf(_grPS, "/%s findfont ", font_list[i].name);
							break;
						case font_encoding_isolatin1:
							if (CurrentFont.id == gr_font_Symbol) 
								fprintf(_grPS, "/%s findfont ", font_list[i].name);
							else
								fprintf(_grPS, "/%s-ISOLatin1 findfont ", font_list[i].name);
							break;
						}
						fprintf(_grPS, "%.2f sc sf\n", CurrentFont.size_pt);
					}
					have_set_font = true;
					last_font.id = newID;
					last_font.encoding = CurrentFont.encoding;
					last_font.size_pt = CurrentFont.size_pt;
				}
			}
			return;
		}
		i++;
	}
	warning("Ignoring request for unknown font.");
}

#define NCODES 100
// symbol_code (p 604 new PostScript book): (1) define name, (2) Postscript
// code, (3) symbol-font crossref code (used for estimage of symbol
// size, by index_for_math_symbol() ... a bad idea, really).
static char    *symbol_code[NCODES][3] = {
	// name, code in Table E.11, p604 new ps book, char-equivalent}
	// 
	// Organization of list below is as in the tables in 
	// Lamport's Latex book
	//
	// Table 3.3 Greek Letters
	// lowercase
	{"alpha", "\\141", "a"},
	{"beta", "\\142", "b"},
	{"gamma", "\\147", "g"},
	{"delta", "\\144", "d"},
	{"epsilon", "\\145", "e"},
// varepsilon
	{"zeta", "\\172", "z"},
	{"eta", "\\150", "h"},
	{"theta", "\\161", "q"},
	{"vartheta", "\\112", "q"},
	{"iota", "\\151", "i"},
	{"kappa", "\\153", "k"},
	{"lambda", "\\154", "l"},
	{"mu", "\\155", "m"},
	{"nu", "\\156", "n"},
	{"xi", "\\170", "x"},
// o [not needed, really]
	{"pi", "\\160", "p"},
	{"varpi", "\\166", "p"},	// guess that size is same as pi
	{"rho", "\\162", "r"},
	{"sigma", "\\163", "s"},
	{"varsigma", "\\126", "s"},
	{"tau", "\\164", "t"},
	{"upsilon", "\\165", "u"},
	{"psi", "\\171", "y"},
	{"chi", "\\143", "c"},
	{"phi", "\\146", "f"},
	{"varphi", "\\152", "f"},
	{"omega", "\\167", "w"},
//
// Uppercase
	{"Gamma", "\\107", "G"},
	{"Delta", "\\104", "D"},
	{"Theta", "\\121", "Q"},
	{"Lambda", "\\114", "L"},
	{"Xi", "\\130", "X"},
	{"Pi", "\\120", "P"},
	{"Sigma", "\\123", "S"},
	{"Upsilon", "\\241", "Y"},	// guess that size is same as psi
	{"Phi", "\\106", "F"},
	{"Psi", "\\131", "Y"},
	{"Omega", "\\127", "W"},
// 
// Table 3.4: Binary Operation Symbols
	{"pm", "\\261", "+"}, // guess that size is same as +
// mp
	{"times", "\\264", "x"}, // guess that size is same as x
	{"div", "\\270", "x"},	 // guess that size is same as x
	{"ast", "\\052", "*"},
// star
	{"circ", "\\260", "."},	// guess that size is same as .
	{"bullet", "\\267", "*"}, // guess that size is same as *
	{"cdot", "\\327", ","},
// cap
// cup
// uplus
// sqcap
// sqcup
// vee
	{"wedge", "\\331", "M"}, // guess that size is same as M
// setminus
// wr
// diamond
// bigtriangleup
// bigtriangledown
// triangleleft
// triangleright
// lhd
// rhd
// unlhd
// unrhd
	{"oplus", "\\305", "o"},
// ominus
	{"otimes", "\\304", "o"},
// oslash
// odot
// bigcirc
// dagger
// ddagger
// amalg
//
// Table 3.5: Relation Symbols
	{"leq", "\\243", "<"}, // guess that size is same as <
// prec
// preceq
// ll
	{"subset", "\\314", "<"}, // guess that size is same as <
	{"subseteq", "\\315", "<"}, // guess that size is same as <
// sqsubset
// sqsubseteq
// MOVE 'in' to after 'infty'
// vdash
	{"geq", "\\263", ">"}, // guess that size is same as >
// succ
// succeq
// gg
	{"supset", "\\311", ">"}, // guess that size is same as >
	{"supseteq", "\\312", ">"}, // guess that size is same as >
// sqsupset
// sqsupseteq    
// ni
// dashv
	{"equiv", "\\272", "="}, // guess that size is same as =
	{"sim", "\\176", "~"},
// simeq
// asymp
	{"approx", "\\273", "~"}, // guess that size is same as ~
	{"cong", "\\100", "="},	  // guess that size is same as =
	{"neq", "\\271", "="},	  // guess that size is same as =
// doteq
	{"propto", "\\265", "~"}, // guess that size is same as ~    
// models
	{"perp", "\\136", "M"},
// mid
// parallel
// bowtie
// join
// smile
// frown
// 
// Table 3.6: Arrow Symbols
	{"leftarrow", "\\254", "M"},
	{"Leftarrow", "\\334", "M"},
	{"rightarrow", "\\256", "M"},
	{"Rightarrow", "\\336", "M"},
	{"leftrightarrow", "\\253", "M"},
	{"Leftrightarrow", "\\333", "M"},
// mapsto
// hookleftarrow
// leftharpoonup
// leftharpoondown
// rightleftharpoons
// longleftarrow
// Longleftarrow
// longrightarrow
// Longrightarrow
// longleftrightarrow
// Longleftrightarrow
// longmapsto
// hookrightarrow
// rightharpoonup
// rightharpoon down
// leadsto
	{"uparrow", "\\255", "|"}, // guess that size is same as "|"
	{"Uparrow", "\\335", "|"}, // guess that size is same as "|"
	{"downarrow", "\\257", "|"}, // guess that size is same as "|"
	{"Downarrow", "\\337", "|"}, // guess that size is same as "|"
// updownarrow
// Updownarrow
// neararrow
// searrow
// swarrow
// nwarrow
//
// Table 3.7: Miscellaneous Symbols
	{"aleph", "\\300", "M"},
// hbar
// imath
// jmath
// ell
	{"wp", "\\303", "M"},
	{"Re", "\\302", "R"},		// guess that size is same as R
	{"Im", "\\301", "M"},		// guess that size is same as M
// mho
	{"prime", "\\242", "'"},
	{"emptyset", "\\306", "M"},
	{"nabla", "\\321", "M"},	// guess that size is same as M
	{"surd", "\\326", "M"},		// guess that size is same as M
	{"sqrt", "\\326", "M"},		// not in this table, but what the heck
// top
	{"bot", "\\136", "M"},
// |
	{"angle", "\\320", "M"},
	{"forall", "\\042", "M"},	// guess that size is same as M
	{"exists", "\\044", "M"},	// guess that size is same as M
	{"neg", "\\330", "M"},
// flat
// natural
// sharp
// backslash
	{"partial", "\\266", "d"}, // guess that size is same as d
	{"infty", "\\245", "M"},   // guess that size is same as M
// Interpose 'int' and 'in' here to avoid clashes with 'infty'
	{"int", "\\362", "M"}, // guess that size is same as M
	{"in", "\\316", "<"},  // guess that size is same as <
// Box
// Diamond
// triangle
	{"clubsuit", "\\247", "M"},
	{"diamondsuit", "\\340", "M"},
// heartsuit
	{"spadesuit", "\\252", "M"},
//
// Table 3.8 Variable-sized symbols
	{"sum", "\\345", "M"}, // guess that size is same as M
	{"prod", "\\325", "M"},	// guess that size is same as M
// int -- moved up to avoid name clashes
// oint
// bigcap
// bigcup
// bigsqcup
// bigvee
// bigwedge
// bigodot
// bigotimes
// bigoplus
// biguplus
//
// Table 3.10
// (
// [
// {
	{"lfloor", "\\353", "M"},
	{"lceil", "\\351", "M"},
	{"langle", "\\341", "<"},
// /
// |
// )
// ]
// }
	{"rfloor", "\\373", "M"},
	{"rceil", "\\371", "M"},
	{"rangle", "\\361", ">"}
// backslash SEE ABOVE
// \|
// uparrow SEE ABOVE
// downarrow SEE ABOVE
// updownarrow SEE ABOVE
// Uparrow SEE ABOVE
// Downarrow SEE ABOVE
// Updownarrow SEE ABOVE
};

static char    *
symbol_in_math(const char *sPtr, int *inc)
{
	/* handle greek letter or symbol in math mode */
	int             i;
	sPtr++;
	*inc = 0;
	for (i = 0; i < NCODES; i++) {
		int             len = strlen(symbol_code[i][0]);
		if (!strncmp(sPtr, symbol_code[i][0], len)) {
			*inc = len;
			return symbol_code[i][1];
		}
	}
	return NULL;
}

static void
PopStack()
{
	if (istack > 0 && pstack[istack - 1] == Superscript)
		MoveDown();
	else if (istack > 0 && pstack[istack - 1] == Subscript)
		MoveUp();
	istack--;
}

static void
ClearStack()
{
	istack = 0;
}

// Move left/right by indicated number of M spaces
static void
MoveHorizontally(double em_distance)
{
	double          w, a, d;
	gr_stringwidth("M", &w, &a, &d);
	STOP_OLD_TEXT;
	gr_rmoveto_cm(em_distance * w, 0.0);
	START_NEW_TEXT;
}

// MoveUp() -- move up, shifting to smaller/larger size if necessary
static void
MoveUp()
{
	STOP_OLD_TEXT;
	// See if already in subscript.
	if ((istack > 0) && pstack[istack - 1] == Subscript) {
		// Moving up from subscript, so enlarge font, then undo last move
		// down.
		gr_setfontsize_pt(gr_currentfontsize_pt() / SubSize);
		gr_rmoveto_pt(0.0, SubMoveDown * gr_currentCapHeight_cm() * PT_PER_CM);
	} else {
		// Moving up for superscript, so move up, then reduce font.
		gr_rmoveto_pt(0.0, SuperMoveUp * gr_currentCapHeight_cm() * PT_PER_CM);
		gr_setfontsize_pt(gr_currentfontsize_pt() * SuperSize);
	}
	START_NEW_TEXT;
}

// MoveDown() -- move down, shifting to smaller/larger size if necessary
static void
MoveDown()
{
	STOP_OLD_TEXT;
	// See if already in superscript.
	if ((istack > 0) && pstack[istack - 1] == Superscript) {
		// Moving down from superscript, so enlarge font, then undo last move
		// up.
		gr_setfontsize_pt(gr_currentfontsize_pt() / SuperSize);
		gr_rmoveto_pt(0.0, -SuperMoveUp * gr_currentCapHeight_cm() * PT_PER_CM);
	} else {
		// Moving down for subscript, so move down, then reduce font.
		gr_rmoveto_pt(0.0, -SubMoveDown * gr_currentCapHeight_cm() * PT_PER_CM);
		gr_setfontsize_pt(gr_currentfontsize_pt() * SubSize);
	}
	START_NEW_TEXT;
}

static void
gr_DrawChar(const char *c)
{
	extern bool     _grWritePS;
	if (_grWritePS) {
		extern FILE *_grPS;
		switch (*c) {
		case '\\':
			fprintf(_grPS, "\\\\");
			break;
		case '(':
			fprintf(_grPS, "\\(");
			break;
		case ')':
			fprintf(_grPS, "\\)");
			break;
		default:
			fprintf(_grPS, "%c", *c);
			break;
		}
		check_psfile();
	}
	_drawingstarted = true;
}

// Draw indicated text in a "whiteout" box of indicated color, left-right
// centered at the indicated (x,y) locn specified in user-units.  The text
// and box will be rotated by gr_currenttextangle_deg() degrees, measured
// counterclockwise from the horizontal.
void
gr_show_in_box(/*const*/GriString &s,
	       const GriColor& text_color,
	       const GriColor& box_color,
	       double x,	// cm units
	       double y,
	       double angle_deg)
{
	GriColor old_text_color = _griState.color_text();
	GriColor old_line_color = _griState.color_line();

	double          width, ascent, descent;
	double          x0, y0, dx, dy, dx_rot, dy_rot;
	double          thin_space = gr_thinspace_cm();
	if (0.0 == gr_currentfontsize_pt())
		return;
	gr_stringwidth(s.getValue(), &width, &ascent, &descent);
	x0 = x;			// save
	y0 = y;

	// White out below text.
	dx = -0.5 * width - thin_space;
	dy = -thin_space;
	gr_rotate_xy(dx, dy, angle_deg, &dx_rot, &dy_rot);

	static GriPath p(5);
	p.clear();
	p.push_back(x0 + dx_rot, y0 + dy_rot, 'm');

	dx = -dx;
	gr_rotate_xy(dx, dy, angle_deg, &dx_rot, &dy_rot);
	p.push_back(x0 + dx_rot, y0 + dy_rot, 'l');

	dx = 0.5 * width + thin_space;
	dy = ascent + thin_space;
	gr_rotate_xy(dx, dy, angle_deg, &dx_rot, &dy_rot);
	p.push_back(x0 + dx_rot, y0 + dy_rot, 'l');

	dx = -dx;
	gr_rotate_xy(dx, dy, angle_deg, &dx_rot, &dy_rot);
	p.push_back(x0 + dx_rot, y0 + dy_rot, 'l');

	p.push_back(x0 + dx_rot, y0 + dy_rot, 'l');
    
	_griState.set_color_line(box_color);
	p.fill(units_cm);

	bounding_box_update(p.bounding_box(units_cm));

	// Draw text
	_griState.set_color_text(text_color);
	dx = -0.5 * width;
	dy = 0.0;
	gr_rotate_xy(dx, dy, angle_deg, &dx_rot, &dy_rot);
	gr_show_at(s.getValue(),
		   x0 + dx_rot,
		   y0 + dy_rot,
		   TEXT_LJUST,
		   angle_deg);
	_griState.set_color_line(old_line_color);
	_griState.set_color_text(old_text_color);
	_drawingstarted = true;
}

// Rotate (x,y) into (xx,yy), through `angle' degrees counterclockwise.
void
gr_rotate_xy(double x, double y, double angle, double *xx, double *yy)
{
	angle /= DEG_PER_RAD;	// convert to radians
	double c = cos(angle);
	double s = sin(angle);
	*xx = c * x - s * y;
	*yy = s * x + c * y;
}

// Get the width, ascent and descent of string s, in current font.
// BUG: Ascent and descent are inaccurate.
// BUG: Smaller size of super/subscripts not accounted for.
void
gr_stringwidth(const char *s, double *w, double *a, double *d)
{
	*w = *a = *d = 0.0;
	if (strlen(s) == 0)
		return;
	bool            used_supers = false;
	bool            used_subs = false;
	bool            inmath = false;
	bool            oldWritePS = _grWritePS;
	double          oldfontsize_pt = gr_currentfontsize_pt();
	_grWritePS = false;
	while (*s != '\0') {
		// figure out whether entering or leaving math mode
		if (*s == '$' && *(s - 1) != '\\') {
			inmath = (inmath ? false : true);
			s++;
			continue;
		}
		// handle math mode differently
		// ?? BUG Superscripts and subscripts are printed smaller, but
		// ?? BUG their size is assumed to be the same as normal chars.
		if (inmath) {
			if (*s == '^')	// handle superscript
				used_supers = true;
			else if (*s == '_')	// handle subscript
				used_subs = true;
			else if (*s == '{')	// ignore groups
				;		// EMPTY
			else if (*s == '}')	// ignore groups
				;		// EMPTY
			else if (*s == '\\') {	// handle synonym
				int             skip;
				char *            ss;
				// First catch thinspace commands
				if (*(s + 1) == ',') {
					// Thinspace = Mwidth/6
					*w += gr_charwidth_cm((int)'M', CurrentFont.id, CurrentFont.size_pt) / 6.0;
					s += 1;
				} else if (*(s + 1) == '!') {
					// Negative thinspace = -Mwidth/6
					*w -= gr_charwidth_cm((int)'M', CurrentFont.id, CurrentFont.size_pt) / 6.0;
					s += 1;
				} else {
					ss = symbol_in_math(s, &skip); // NULL if can't find
					if (ss != NULL) {
						gr_fontID       oldfontID = CurrentFont.id;
						int             C = index_for_math_symbol(ss);
						s += skip;
						*w += gr_charwidth_cm(C, gr_font_Symbol, CurrentFont.size_pt);
						CurrentFont.id = oldfontID;
					} else {
						// it's not a known math symbol
						*w += gr_charwidth_cm('\\', CurrentFont.id, CurrentFont.size_pt);
					}
				}
			} else {
				// We are in mathmode, but it's not a special character. Add
				// appropriate amount for either super/subscript or normal
				// character.
				*w += gr_charwidth_cm((int) *s, CurrentFont.id, CurrentFont.size_pt);
			}
		} else {
			// not inmath
			*w += gr_charwidth_cm((int) *s, CurrentFont.id, CurrentFont.size_pt);
		}
		s++;
	}
	// Calculate ascent/descent.  BUG: doesn't take math chars into acct
	*a = gr_currentCapHeight_cm() * (1 + (used_supers ? 1 : 0) *
					 (SuperSize + SuperMoveUp - 1));
#if 0				// before 2.054
	*d = gr_currentCapHeight_cm() * (1.0 + 0.5 * (int) used_subs);
#endif
	*d = gr_current_descender() * (1 + (used_subs ? 1.0 : 0.0) *
				       (SubSize + SubMoveDown - 1));
	// reset fontsize ... can't do with gr_setfontsize_pt() 
	// because that would call this function in infinite recursion.
	CurrentFont.size_pt = oldfontsize_pt;
	_grWritePS = oldWritePS;
}

// return index (for size-table) for a character (given as integer)
static int
index_for_math_symbol(char *s)
{
	for (int i = 0; i < NCODES; i++)
		if (!strncmp(s, symbol_code[i][1], strlen(symbol_code[i][1])))
			return (int) *symbol_code[i][2];
	// return index(M) as a guess (since later using for width)
	return (int) 'M';
}

// Return thinspace (=1/6 of width of "M" in current font), in cm
double
gr_thinspace_cm()
{
	return (gr_charwidth_cm(int('M'), CurrentFont.id, CurrentFont.size_pt) / 6.0);
}

// Return width of quad (= width of "M" in current font), in cm
double
gr_quad_cm()
{
	return (gr_charwidth_cm((int) 'M', CurrentFont.id, CurrentFont.size_pt));
}

// Following page should substituted as output from
// ~kelley/src/gri/src/get_font_metrics

struct font_metric {
	float XHeight;
	float CapHeight;
	float Ascender;
	float Descender;
	float width[128];
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Courier.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Courier = {
	0.015028,	// XHeight   
	0.019826,	// CapHeight 
	0.022190,	// Ascender  
	-0.005539,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0211667, 0.0211667, 0.0211667, 
		0.0211667, 0.0211667, 0.0000000
	}
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Helvetica.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Helvetica = {
	0.018450,	// XHeight   
	0.025329,	// CapHeight 
	0.025329,	// Ascender  
	-0.007302,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0098072, 0.0098072, 0.0125236, 
		0.0196144, 0.0196144, 0.0313619, 0.0235303, 0.0078317, 
		0.0117475, 0.0117475, 0.0137231, 0.0206022, 0.0098072, 
		0.0117475, 0.0098072, 0.0098072, 0.0196144, 0.0196144, 
		0.0196144, 0.0196144, 0.0196144, 0.0196144, 0.0196144, 
		0.0196144, 0.0196144, 0.0196144, 0.0098072, 0.0098072, 
		0.0206022, 0.0206022, 0.0206022, 0.0196144, 0.0358069, 
		0.0235303, 0.0235303, 0.0254706, 0.0254706, 0.0235303, 
		0.0215547, 0.0274461, 0.0254706, 0.0098072, 0.0176389, 
		0.0235303, 0.0196144, 0.0293864, 0.0254706, 0.0274461, 
		0.0235303, 0.0274461, 0.0254706, 0.0235303, 0.0215547, 
		0.0254706, 0.0235303, 0.0333022, 0.0235303, 0.0235303, 
		0.0215547, 0.0098072, 0.0098072, 0.0098072, 0.0165453, 
		0.0196144, 0.0078317, 0.0196144, 0.0196144, 0.0176389, 
		0.0196144, 0.0196144, 0.0098072, 0.0196144, 0.0196144, 
		0.0078317, 0.0078317, 0.0176389, 0.0078317, 0.0293864, 
		0.0196144, 0.0196144, 0.0196144, 0.0196144, 0.0117475, 
		0.0176389, 0.0098072, 0.0196144, 0.0176389, 0.0254706, 
		0.0176389, 0.0176389, 0.0176389, 0.0117828, 0.0091722, 
		0.0117828, 0.0206022, 0.0000000
	}
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Helvetica-Oblique.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Helvetica_Oblique = {
	0.018450,	// XHeight   
	0.025329,	// CapHeight 
	0.025329,	// Ascender  
	-0.007302,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0098072, 0.0098072, 0.0125236, 
		0.0196144, 0.0196144, 0.0313619, 0.0235303, 0.0078317, 
		0.0117475, 0.0117475, 0.0137231, 0.0206022, 0.0098072, 
		0.0117475, 0.0098072, 0.0098072, 0.0196144, 0.0196144, 
		0.0196144, 0.0196144, 0.0196144, 0.0196144, 0.0196144, 
		0.0196144, 0.0196144, 0.0196144, 0.0098072, 0.0098072, 
		0.0206022, 0.0206022, 0.0206022, 0.0196144, 0.0358069, 
		0.0235303, 0.0235303, 0.0254706, 0.0254706, 0.0235303, 
		0.0215547, 0.0274461, 0.0254706, 0.0098072, 0.0176389, 
		0.0235303, 0.0196144, 0.0293864, 0.0254706, 0.0274461, 
		0.0235303, 0.0274461, 0.0254706, 0.0235303, 0.0215547, 
		0.0254706, 0.0235303, 0.0333022, 0.0235303, 0.0235303, 
		0.0215547, 0.0098072, 0.0098072, 0.0098072, 0.0165453, 
		0.0196144, 0.0078317, 0.0196144, 0.0196144, 0.0176389, 
		0.0196144, 0.0196144, 0.0098072, 0.0196144, 0.0196144, 
		0.0078317, 0.0078317, 0.0176389, 0.0078317, 0.0293864, 
		0.0196144, 0.0196144, 0.0196144, 0.0196144, 0.0117475, 
		0.0176389, 0.0098072, 0.0196144, 0.0176389, 0.0254706, 
		0.0176389, 0.0176389, 0.0176389, 0.0117828, 0.0091722, 
		0.0117828, 0.0206022, 0.0000000
	}
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Helvetica-Bold.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Helvetica_Bold = {
	0.018768,	// XHeight   
	0.025329,	// CapHeight 
	0.025329,	// Ascender  
	-0.007302,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0098072, 0.0117475, 0.0167217, 
		0.0196144, 0.0196144, 0.0313619, 0.0254706, 0.0098072, 
		0.0117475, 0.0117475, 0.0137231, 0.0206022, 0.0098072, 
		0.0117475, 0.0098072, 0.0098072, 0.0196144, 0.0196144, 
		0.0196144, 0.0196144, 0.0196144, 0.0196144, 0.0196144, 
		0.0196144, 0.0196144, 0.0196144, 0.0117475, 0.0117475, 
		0.0206022, 0.0206022, 0.0206022, 0.0215547, 0.0343958, 
		0.0254706, 0.0254706, 0.0254706, 0.0254706, 0.0235303, 
		0.0215547, 0.0274461, 0.0254706, 0.0098072, 0.0196144, 
		0.0254706, 0.0215547, 0.0293864, 0.0254706, 0.0274461, 
		0.0235303, 0.0274461, 0.0254706, 0.0235303, 0.0215547, 
		0.0254706, 0.0235303, 0.0333022, 0.0235303, 0.0235303, 
		0.0215547, 0.0117475, 0.0098072, 0.0117475, 0.0206022, 
		0.0196144, 0.0098072, 0.0196144, 0.0215547, 0.0196144, 
		0.0215547, 0.0196144, 0.0117475, 0.0215547, 0.0215547, 
		0.0098072, 0.0098072, 0.0196144, 0.0098072, 0.0313619, 
		0.0215547, 0.0215547, 0.0215547, 0.0215547, 0.0137231, 
		0.0196144, 0.0117475, 0.0215547, 0.0196144, 0.0274461, 
		0.0196144, 0.0196144, 0.0176389, 0.0137231, 0.0098778, 
		0.0137231, 0.0206022, 0.0000000
	}
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Palatino-Roman.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Palatino_Roman = {
	0.016545,	// XHeight   
	0.024412,	// CapHeight 
	0.025612,	// Ascender  
	-0.009913,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0088194, 0.0098072, 0.0130881, 
		0.0176389, 0.0176389, 0.0296333, 0.0274461, 0.0098072, 
		0.0117475, 0.0117475, 0.0137231, 0.0213783, 0.0088194, 
		0.0117475, 0.0088194, 0.0213783, 0.0176389, 0.0176389, 
		0.0176389, 0.0176389, 0.0176389, 0.0176389, 0.0176389, 
		0.0176389, 0.0176389, 0.0176389, 0.0088194, 0.0088194, 
		0.0213783, 0.0213783, 0.0213783, 0.0156633, 0.0263525, 
		0.0274461, 0.0215547, 0.0250119, 0.0273050, 0.0215547, 
		0.0196144, 0.0269169, 0.0293511, 0.0118886, 0.0117475, 
		0.0256117, 0.0215547, 0.0333728, 0.0293158, 0.0277283, 
		0.0213078, 0.0277283, 0.0235656, 0.0185208, 0.0216253, 
		0.0274461, 0.0254706, 0.0352778, 0.0235303, 0.0235303, 
		0.0235303, 0.0117475, 0.0213783, 0.0117475, 0.0213783, 
		0.0176389, 0.0098072, 0.0176389, 0.0195086, 0.0156633, 
		0.0215547, 0.0168981, 0.0117475, 0.0196144, 0.0205317, 
		0.0102658, 0.0082550, 0.0196144, 0.0102658, 0.0311503, 
		0.0205317, 0.0192617, 0.0212019, 0.0197556, 0.0139347, 
		0.0149578, 0.0115006, 0.0212725, 0.0199319, 0.0294217, 
		0.0182033, 0.0196144, 0.0176389, 0.0117475, 0.0213783, 
		0.0117475, 0.0213783, 0.0000000
	}
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Symbol.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Symbol = {
	0.000000,	// XHeight   
	0.000000,	// CapHeight 
	0.000000,	// Ascender  
	0.000000,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0088194, 0.0117475, 0.0251531, 
		0.0176389, 0.0193675, 0.0293864, 0.0274461, 0.0154869, 
		0.0117475, 0.0117475, 0.0176389, 0.0193675, 0.0088194, 
		0.0193675, 0.0088194, 0.0098072, 0.0176389, 0.0176389, 
		0.0176389, 0.0176389, 0.0176389, 0.0176389, 0.0176389, 
		0.0176389, 0.0176389, 0.0176389, 0.0098072, 0.0098072, 
		0.0193675, 0.0193675, 0.0193675, 0.0156633, 0.0193675, 
		0.0254706, 0.0235303, 0.0254706, 0.0215900, 0.0215547, 
		0.0269169, 0.0212725, 0.0254706, 0.0117475, 0.0222603, 
		0.0254706, 0.0242006, 0.0313619, 0.0254706, 0.0254706, 
		0.0270933, 0.0261408, 0.0196144, 0.0208844, 0.0215547, 
		0.0243417, 0.0154869, 0.0270933, 0.0227542, 0.0280458, 
		0.0215547, 0.0117475, 0.0304447, 0.0117475, 0.0232128, 
		0.0176389, 0.0176389, 0.0222603, 0.0193675, 0.0193675, 
		0.0174272, 0.0154869, 0.0183797, 0.0144992, 0.0212725, 
		0.0116064, 0.0212725, 0.0193675, 0.0193675, 0.0203200, 
		0.0183797, 0.0193675, 0.0193675, 0.0183797, 0.0193675, 
		0.0212725, 0.0154869, 0.0203200, 0.0251531, 0.0242006, 
		0.0173919, 0.0242006, 0.0174272, 0.0169333, 0.0070556, 
		0.0169333, 0.0193675, 0.0000000
	}
};
//
// Following font metric generated by `get_font_metrics'
// perlscript from Font Metric file `/usr/openwin/lib/X11/fonts/F3/afm/Times-Roman.afm'.
// All measurement in centimetres, given a pointsize of 1.0
//
struct font_metric Times_Roman = {
	0.015875,	// XHeight   
	0.023354,	// CapHeight 
	0.024095,	// Ascender  
	-0.007655,	// Descender 
	{	// Widths of first 128 characters
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
		0.0000000, 0.0000000, 0.0088194, 0.0117475, 0.0143933, 
		0.0176389, 0.0176389, 0.0293864, 0.0274461, 0.0117475, 
		0.0117475, 0.0117475, 0.0176389, 0.0198967, 0.0088194, 
		0.0117475, 0.0088194, 0.0098072, 0.0176389, 0.0176389, 
		0.0176389, 0.0176389, 0.0176389, 0.0176389, 0.0176389, 
		0.0176389, 0.0176389, 0.0176389, 0.0098072, 0.0098072, 
		0.0198967, 0.0198967, 0.0198967, 0.0156633, 0.0324908, 
		0.0254706, 0.0235303, 0.0235303, 0.0254706, 0.0215547, 
		0.0196144, 0.0254706, 0.0254706, 0.0117475, 0.0137231, 
		0.0254706, 0.0215547, 0.0313619, 0.0254706, 0.0254706, 
		0.0196144, 0.0254706, 0.0235303, 0.0196144, 0.0215547, 
		0.0254706, 0.0254706, 0.0333022, 0.0254706, 0.0254706, 
		0.0215547, 0.0117475, 0.0098072, 0.0117475, 0.0165453, 
		0.0176389, 0.0117475, 0.0156633, 0.0176389, 0.0156633, 
		0.0176389, 0.0156633, 0.0117475, 0.0176389, 0.0176389, 
		0.0098072, 0.0098072, 0.0176389, 0.0098072, 0.0274461, 
		0.0176389, 0.0176389, 0.0176389, 0.0176389, 0.0117475, 
		0.0137231, 0.0098072, 0.0176389, 0.0176389, 0.0254706, 
		0.0176389, 0.0176389, 0.0156633, 0.0169333, 0.0070556, 
		0.0169333, 0.0190853, 0.0000000
	}
};

/*
 * gr_charwidth_cm(char c, int font, double fontsize_pt)
 * 
 * RETURN VALUE the width of the character, in centimetres
 * 
 * Font info created by the `get_font_metrics' perlscript, in the Gri src
 * directory.  This looks in the OpenWindows font metrics files to figure the
 * pertintent stuff out.  (You might have to edit this for different
 * machines).
 */
static double
gr_charwidth_cm(int c, int font, double fontsize_pt)
{
	unsigned char   i = (int) c;
	if (i > 127)
		return fontsize_pt * 0.0211663;	/* error, but guess Courier size
						 * anyway */
	switch (font) {
	case gr_font_TimesRoman:
		return fontsize_pt * Times_Roman.width[i];
	case gr_font_Courier:
		return fontsize_pt * 0.0211663;	/* Courier has fixed width */
	case gr_font_Symbol:
		return fontsize_pt * Symbol.width[i];
	case gr_font_Helvetica:
		return fontsize_pt * Helvetica.width[i];
	case gr_font_HelveticaBold:
		return fontsize_pt * Helvetica_Bold.width[i];
	case gr_font_PalatinoRoman:
		return fontsize_pt * Palatino_Roman.width[i];
	default:
		break;
		/* Guess similar size to Helvetica */
	}
	return (fontsize_pt * Helvetica.width[i]);
}

double
gr_current_descender()		// descender, in positive cm
{
	switch (CurrentFont.id) {
	case gr_font_Courier:
		return (-CurrentFont.size_pt * Courier.Descender);
	case gr_font_Helvetica:
		return(-CurrentFont.size_pt * Helvetica.Descender);
	case gr_font_Symbol:
		return(-CurrentFont.size_pt * Symbol.Descender);
	case gr_font_TimesRoman:
		return(-CurrentFont.size_pt * Times_Roman.Descender);
	default:
		break;
	}
	return CurrentFont.size_pt * 0.025329; // Guess similar to Helvetica
}


// gr_currentCapHeight_cm() -- find height of capital characters
double
gr_currentCapHeight_cm()
{
	double          height_cm = 0.0;
	switch (CurrentFont.id) {
	case gr_font_Courier:
		height_cm = (CurrentFont.size_pt * Courier.CapHeight);
		break;
	case gr_font_Helvetica:
		height_cm = (CurrentFont.size_pt * Helvetica.CapHeight);
		break;
	case gr_font_Symbol:
		height_cm = (CurrentFont.size_pt * Symbol.CapHeight);
		break;
	case gr_font_TimesRoman:
		height_cm = (CurrentFont.size_pt * Times_Roman.CapHeight);
		break;
	default:
		/*
		 * Don't know this font.
		 */
		break;
	}
	if (height_cm > 0.0)
		return height_cm;
	else
		return CurrentFont.size_pt * 0.025329;	/* Guess Helvetica */
}
