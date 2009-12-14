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

//#define DEBUG 1
#include <vector>
#include <string>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include "gr.hh"
#include "extern.hh"
#include "GriPath.hh"
#include "superus.hh"
#include "defaults.hh"
#define NCODES 100
// symbol_code (p 604 new PostScript book): (1) define name, (2) Postscript
// code, (3) symbol-font crossref code (used for estimage of symbol
// size, by index_for_math_symbol() ... a bad idea, really), and (4)
// SVG code [broken; need to transcribe the codes from
// http://www.w3.org/TR/html4/sgml/entities.html#h-24.3 one by one]
static char *symbol_code[NCODES][4] = {
	// name, code in Table E.11, p604 new ps book, char-equivalent}
	// 
	// Organization of list below is as in the tables in 
	// Lamport's Latex book
	//
	// Table 3.3 Greek Letters
	// lowercase
	{(char *)"alpha", (char *)"\\141", (char *)"a", (char *)"&#945;"},
	{(char *)"beta", (char *)"\\142", (char *)"b", (char *)"&#946;"},
	{(char *)"gamma", (char *)"\\147", (char *)"g", (char *)"&#947;"},
	{(char *)"delta", (char *)"\\144", (char *)"d", (char *)"&#948;"},
	{(char *)"epsilon", (char *)"\\145", (char *)"e", (char *)"&#949;"},
        // varepsilon
	{(char *)"zeta", (char *)"\\172", (char *)"z", (char *)"&#952;"},
	{(char *)"eta", (char *)"\\150", (char *)"h", (char *)"&#951;"},
	{(char *)"theta", (char *)"\\161", (char *)"q", (char *)"&#952;"},
	{(char *)"vartheta", (char *)"\\112", (char *)"q", (char *)"&#977;"},
	{(char *)"iota", (char *)"\\151", (char *)"i", (char *)"&#9531;"},
	{(char *)"kappa", (char *)"\\153", (char *)"k", (char *)"&#954;"},
	{(char *)"lambda", (char *)"\\154", (char *)"l", (char *)"&#955;"},
	{(char *)"mu", (char *)"\\155", (char *)"m", (char *)"&#956;"},
	{(char *)"nu", (char *)"\\156", (char *)"n", (char *)"&#957;"},
	{(char *)"xi", (char *)"\\170", (char *)"x", (char *)"&#958;"},
        // o [not needed, really]
	{(char *)"pi", (char *)"\\160", (char *)"p", (char *)"&#960;"},
	{(char *)"varpi", (char *)"\\166", (char *)"p", (char *)"&#960;"},
	{(char *)"rho", (char *)"\\162", (char *)"r", (char *)"&#961;"},
	{(char *)"sigma", (char *)"\\163", (char *)"s", (char *)"&#963;"}, // or 962
	{(char *)"varsigma", (char *)"\\126", (char *)"s", (char *)"&#962;"},
	{(char *)"tau", (char *)"\\164", (char *)"t", (char *)"&#964;"},
	{(char *)"upsilon", (char *)"\\165", (char *)"u", (char *)"&#965;"},
	{(char *)"psi", (char *)"\\171", (char *)"y", (char *)"&#968;"},
	{(char *)"chi", (char *)"\\143", (char *)"c", (char *)"&#967;"},
	{(char *)"phi", (char *)"\\146", (char *)"f", (char *)"&#966;"},
	{(char *)"varphi", (char *)"\\152", (char *)"f", (char *)"&#966;"}, //?
	{(char *)"omega", (char *)"\\167", (char *)"w", (char *)"&#969;"},
        //
        // Uppercase
	{(char *)"Gamma", (char *)"\\107", (char *)"G", (char *)"&#915;"},
	{(char *)"Delta", (char *)"\\104", (char *)"D", (char *)"&#916;"},
	{(char *)"Theta", (char *)"\\121", (char *)"Q", (char *)"&#920;"},
	{(char *)"Lambda", (char *)"\\114", (char *)"L", (char *)"&#923;"},
	{(char *)"Xi", (char *)"\\130", (char *)"X", (char *)"&#926;"},
	{(char *)"Pi", (char *)"\\120", (char *)"P", (char *)"&#928;"},
	{(char *)"Sigma", (char *)"\\123", (char *)"S", (char *)"&#931;"},
	{(char *)"Upsilon", (char *)"\\241", (char *)"Y", (char *)"&#933;"},
	{(char *)"Phi", (char *)"\\106", (char *)"F", (char *)"&#934;"},
	{(char *)"Psi", (char *)"\\131", (char *)"Y", (char *)"&#936;"},
	{(char *)"Omega", (char *)"\\127", (char *)"W", (char *)"&#937;"},
        // Table 3.4: Binary Operation Symbols
	{(char *)"pm", (char *)"\\261", (char *)"+", (char *)"&#177;"}, // guess that size is same as +
        // mp
	{(char *)"times", (char *)"\\264", (char *)"x", (char *)"&#215;"}, // guess that size is same as x
	{(char *)"div", (char *)"\\270", (char *)"x", (char *)"&#247;"}, // guess that size is same as x
	{(char *)"ast", (char *)"\\052", (char *)"*", (char *)"&#8727;"}, // star
	{(char *)"circ", (char *)"\\260", (char *)".", (char *)"&#176;"},
	{(char *)"bullet", (char *)"\\267", (char *)"*", (char *)"&#8226;"}, // guess that size is same as *
	{(char *)"cdot", (char *)"\\327", (char *)",", (char *)"&#183;"}, // ? Georgian comma
        // cap
        // cup
        // uplus
        // sqcap
        // sqcup
        // vee
	{(char *)"wedge", (char *)"\\331", (char *)"M", (char *)"&#8743;"}, // guess that size is same as M
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
	{(char *)"oplus", (char *)"\\305", (char *)"o", (char *)"&#8853;"},
        // ominus
	{(char *)"otimes", (char *)"\\304", (char *)"o", (char *)"&#8855;"},
        // oslash
        // odot
        // bigcirc
        // dagger
        // ddagger
        // amalg
        //
        // Table 3.5: Relation Symbols
	{(char *)"leq", (char *)"\\243", (char *)"<", (char *)"&#8804;"}, // guess that size is same as <
        // prec
        // preceq
        // ll
	{(char *)"subset", (char *)"\\314", (char *)"<", (char *)"&#8834;"}, // guess that size is same as <
	{(char *)"subseteq", (char *)"\\315", (char *)"<", (char *)"&#8838;"}, // guess that size is same as <
        // sqsubset
        // sqsubseteq
        // MOVE 'in' to after 'infty'
        // vdash
	{(char *)"geq", (char *)"\\263", (char *)">", (char *)"&#8805;"},
        // succ
        // succeq
        // gg
	{(char *)"supset", (char *)"\\311", (char *)">", (char *)"&#8835;"},
	{(char *)"supseteq", (char *)"\\312", (char *)">", (char *)"&#8839;"},
        // sqsupset
        // sqsupseteq    
        // ni
        // dashv
	{(char *)"equiv", (char *)"\\272", (char *)"=", (char *)"&#8801;"},
	{(char *)"sim", (char *)"\\176", (char *)"~", (char *)"&#8764;"},
        // simeq
        // asymp
	{(char *)"approx", (char *)"\\273", (char *)"~", (char *)"&#8774;"}, // ?
	{(char *)"cong", (char *)"\\100", (char *)"=", (char *)"&#8764;"}, // ?
	{(char *)"neq", (char *)"\\271", (char *)"=", (char *)"&#8800;"},
        // doteq
	{(char *)"propto", (char *)"\\265", (char *)"~", (char *)"&#8733;"},
        // models
	{(char *)"perp", (char *)"\\136", (char *)"M", (char *)"&#8869;"},
        // mid
        // parallel
        // bowtie
        // join
        // smile
        // frown
        // 
        // Table 3.6: Arrow Symbols
	{(char *)"leftarrow", (char *)"\\254", (char *)"M", (char *)"&#8592;"},
	{(char *)"Leftarrow", (char *)"\\334", (char *)"M", (char *)"&#8656;"},
	{(char *)"rightarrow", (char *)"\\256", (char *)"M", (char *)"&#8594;"},
	{(char *)"Rightarrow", (char *)"\\336", (char *)"M", (char *)"&#8658;"},
	{(char *)"leftrightarrow", (char *)"\\253", (char *)"M", (char *)"&#8596;"},
	{(char *)"Leftrightarrow", (char *)"\\333", (char *)"M", (char *)"&#8660;"},
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
	{(char *)"uparrow", (char *)"\\255", (char *)"|", (char *)"&#8593;"},
	{(char *)"Uparrow", (char *)"\\335", (char *)"|", (char *)"&#8657;"},
	{(char *)"downarrow", (char *)"\\257", (char *)"|", (char *)"&#8595;"},
	{(char *)"Downarrow", (char *)"\\337", (char *)"|", (char *)"&#8659;"},
        // updownarrow
        // Updownarrow
        // neararrow
        // searrow
        // swarrow
        // nwarrow
        //
        // Table 3.7: Miscellaneous Symbols
	{(char *)"aleph", (char *)"\\300", (char *)"M", (char *)"&#8501;"}, // ?
        // hbar
        // imath
        // jmath
        // ell
	{(char *)"wp", (char *)"\\303", (char *)"M", (char *)"&#;8476"}, // BUG: figure out what this is
	{(char *)"Re", (char *)"\\302", (char *)"R", (char *)"&#;8476"},
	{(char *)"Im", (char *)"\\301", (char *)"M", (char *)"&#8465;"},
        // mho
	{(char *)"prime", (char *)"\\242", (char *)"'", (char *)"&#8242;"},
	{(char *)"emptyset", (char *)"\\306", (char *)"M", (char *)"&#;8709"},
	{(char *)"nabla", (char *)"\\321", (char *)"M", (char *)"&#8711;"},
	{(char *)"surd", (char *)"\\326", (char *)"M", (char *)"&#8730;"},
	{(char *)"sqrt", (char *)"\\326", (char *)"M", (char *)"&#8730;"},
        // top
	{(char *)"bot", (char *)"\\136", (char *)"M", (char *)"&#;8730"}, // BUG: no idea what this is
        // |
	{(char *)"angle", (char *)"\\320", (char *)"M", (char *)"&#8736;"},
	{(char *)"forall", (char *)"\\042", (char *)"M", (char *)"&#8704;"},
	{(char *)"exists", (char *)"\\044", (char *)"M", (char *)"&#8707;"},
	{(char *)"neg", (char *)"\\330", (char *)"M", (char *)"&#172;"},
        // flat
        // natural
        // sharp
        // backslash
	{(char *)"partial", (char *)"\\266", (char *)"d", (char *)"&#8706;"},
	{(char *)"infty", (char *)"\\245", (char *)"M", (char *)"&#8734;"},
        // Interpose 'int' and 'in' here to avoid clashes with 'infty'
	{(char *)"int", (char *)"\\362", (char *)"M", (char *)"&#8747;"},
	{(char *)"in", (char *)"\\316", (char *)"<", (char *)"&#8712;"},
        // Box
        // Diamond
        // triangle
	{(char *)"clubsuit", (char *)"\\247", (char *)"M", (char *)"&#9827;"},
	{(char *)"diamondsuit", (char *)"\\340", (char *)"M", (char *)"&#9830;"},
        // heartsuit
	{(char *)"spadesuit", (char *)"\\252", (char *)"M", (char *)"&#9829;"},
        //
        // Table 3.8 Variable-sized symbols
	{(char *)"sum", (char *)"\\345", (char *)"M", (char *)"&#8721;"},
	{(char *)"prod", (char *)"\\325", (char *)"M", (char *)"&#8719;"},
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
	{(char *)"lfloor", (char *)"\\353", (char *)"M", (char *)"&#8970;"},
	{(char *)"lceil", (char *)"\\351", (char *)"M", (char *)"&#8968;"},
	{(char *)"langle", (char *)"\\341", (char *)"<", (char *)"&#9001;"},
	{(char *)"rfloor", (char *)"\\373", (char *)"M", (char *)"&#8971;"},
	{(char *)"rceil", (char *)"\\371", (char *)"M", (char *)"&#8969;"},
	{(char *)"rangle", (char *)"\\361", (char *)">", (char *)"&#9002;"}
        // backslash SEE ABOVE
        // \|
        // uparrow SEE ABOVE
        // downarrow SEE ABOVE
        // updownarrow SEE ABOVE
        // Uparrow SEE ABOVE
        // Downarrow SEE ABOVE
        // Updownarrow SEE ABOVE
};
double gr_current_descender(void);
std::vector<std::string> part_string(const std::string &s)
{
	using namespace std;
	string::size_type i, lasti = 0, len = s.size();
	bool inmath = false;
	vector<string> parts;
	for (i = 0; i < len; i++) {
		if (s[i] == '$') {
			// \$ escapes but not \\$
			if (i > 0 && s[i-1] == '\\') {
				if (!(i > 1 && s[i-2] == '\\'))
					continue;
			}
			if (inmath) i++; // keep the $ at the end
			parts.push_back(s.substr(lasti, i-lasti));
			inmath = !inmath;
			lasti = i;
		}
	}
	parts.push_back(s.substr(lasti, len-lasti));
#if 0
	printf("\n\"%s\"\n", s.c_str());
	for (unsigned int l = 0; l < parts.size(); l++)
		printf("    \"%s\"\n", parts[l].c_str());
#endif
	return parts;
}

const char* gr_fontname_from_id(int id);

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


enum position {Superscript, Subscript, Inline};	// Baseline indicator
static std::stack<position> pstack;   // baseline position stack

// Use spacing patterned on results of a TeX example (using Large font). All
// quantities are multiples of Mspace.
static const double SubSize = 0.75; // relative height of subscripts = 6/8
static const double SuperSize = 0.75; // relative height of superscripts = 6/8 
static const double SuperMoveUp =0.625;	// Move up for super = 5/8
static const double SubMoveDown =0.375;	// Move down for sub = 3/8 



#define PS_showpage     "grestore\nshowpage\n"
#define PS_stroke       "s\n"

extern FILE    *_grPS;
extern FILE    *_grSVG;
extern bool     _grNeedBegin;
extern bool     _grPathExists;
extern bool     _grWritePS;

static void     gr_drawstring(const char *s);
static void     gr_drawchar_svg(char c, double xcm, double ycm, gr_fontID font_id);
static void     gr_drawsymbol_svg(int index, double xcm, double ycm, gr_fontID font_id);
static void     gr_drawstring_svg(const char *s, double xcm, double ycm, double angle);
//static int      index_for_math_symbol(const char *s);	// base routine
static double   gr_charwidth_cm(int c, int font, double fontsize_pt);
static void     gr_DrawChar(const char *c);
static void     gr_setfont_fontsize(gr_fontID newID, bool force = false);
static void     pstack_erase();
static void     MoveDown(void);
static void     MoveUp(void);
static void     MoveUp_svg(double *xcm, double *ycm);
static void     MoveDn_svg(double *xcm, double *ycm);
static void     MoveHorizontally(double em_distance);
static int      symbol_in_math(const char *sPtr, int *inc);

gr_font_info font_list[] =
{
	{gr_font_Courier, (char *)"Courier"},
	{gr_font_CourierOblique, (char *)"Courier-Oblique"},
	{gr_font_CourierBold, (char *)"Courier-Bold"},
	{gr_font_CourierBoldOblique, (char *)"Courier-BoldOblique"},
	{gr_font_Helvetica, (char *)"Helvetica"},
	{gr_font_HelveticaBold, (char *)"Helvetica-Bold"},
	{gr_font_HelveticaOblique, (char *)"Helvetica-Oblique"},
	{gr_font_PalatinoRoman, (char *)"Palatino-Roman"},
	{gr_font_PalatinoItalic, (char *)"Palatino-Italic"},
	{gr_font_PalatinoBold, (char *)"Palatino-Bold"},
	{gr_font_PalatinoBoldItalic, (char *)"Palatino-BoldItalic"},
	{gr_font_Symbol, (char *)"Symbol"},
	{gr_font_TimesRoman, (char *)"Times-Roman"},
	{gr_font_TimesItalic, (char *)"Times-Italic"},
	{gr_font_TimesBold, (char *)"Times-Bold"},
	{gr_font_TimesBoldItalic, (char *)"Times-BoldItalic"},
	{gr_font_Century, (char *)"Century"},
	{gr_font_end_of_list, (char *)""}
};

// Draw text at specified location.
void
gr_show_at(/*const*/ char *s, double xcm, double ycm, gr_textStyle style, double angle_deg)
{
#ifdef DEBUG
        printf("DEBUG %s:%d gr_show_at(\"%s\",xcm,ycm,style,%f)\n",__FILE__,__LINE__,s,angle_deg);
#endif
	if (0.0 == gr_currentfontsize_pt() || !strlen(s)) {
		return;
	}
	double          oldfontsize_pt = gr_currentfontsize_pt();
	gr_fontID       oldfontID = gr_currentfont();
	double          width_cm, ascent_cm, descent_cm;
	rectangle box;
	extern bool _warn_offpage;
	if (_warn_offpage 
	    && ( xcm < OFFPAGE_LEFT 
		 || xcm > OFFPAGE_RIGHT
		 || ycm < OFFPAGE_BOTTOM
		 || ycm > OFFPAGE_TOP)) {
		warning("Drawing text at a location that is offpage.");
	}
	const char *fn_svg = NULL;
	double r, g, b;
	_griState.color_text().getRGB(&r, &g, &b);
	switch (_output_file_type) {
	case  postscript:
		break;
	case svg: 
		switch (CurrentFont.id) {
		case gr_font_Courier:            fn_svg = "Courier";            break;
		case gr_font_CourierOblique:     fn_svg = "Courier-Italic";     break;
		case gr_font_CourierBold:        fn_svg = "Courier-Bold";       break;
		case gr_font_CourierBoldOblique: fn_svg = "Courier-BoldItalic"; break;
		case gr_font_Helvetica:          fn_svg = "Helvetica";          break;
		case gr_font_HelveticaOblique:   fn_svg = "Helvetica-Italic";   break;
		case gr_font_HelveticaBold:      fn_svg = "Helvetica-Bold";     break;
		case gr_font_PalatinoRoman: 
		case gr_font_PalatinoItalic:
		case gr_font_PalatinoBold:
		case gr_font_PalatinoBoldItalic:
			fn_svg = "Times";
			warning("SVG cannot handle Palatino font yet");
			break;
		case gr_font_Symbol:               fn_svg = "Symbol";               break;
		case gr_font_TimesRoman:           fn_svg = "Times";                break;
		case gr_font_TimesItalic:          fn_svg = "Times-Italic";         break;
		case gr_font_TimesBold:            fn_svg = "Times-Bold";           break;
		case gr_font_TimesBoldItalic:      fn_svg = "Times-BoldItalic";     break;
		case gr_font_Century:     	   fn_svg = "Century";  	    break;
		default: 
			fn_svg = "Times";
			warning("SVG defaulting to Times font");
			break;
		}
		break;
	case  gif:
		fprintf(stderr, "INTERNAL error at %s:%d -- nothing known for GIF\n\n", __FILE__, __LINE__);
		exit(99);
		break;
	}

//	if (_output_file_type == svg) {
//		fprintf(stderr, "%s:%d approximating drawing of '%s' NOTE: subscripts, etc won't work\n", __FILE__, __LINE__, s);
//	}

	void set_ps_color(char what);
	set_ps_color('t');
	gr_setfontsize_pt(oldfontsize_pt);
	gr_setfont(oldfontID);
	gr_stringwidth(s, &width_cm, &ascent_cm, &descent_cm);
	switch (style) {
	case TEXT_LJUST:
		gr_moveto_cm(xcm, ycm);
		if (_output_file_type == postscript) {
			if (_grWritePS) {
				if (fabs(angle_deg) > 0.1)
					fprintf(_grPS, "%.2f rotate ", angle_deg);
				gr_drawstring(s);
			}
		} else if (_output_file_type == svg) {
			gr_drawstring_svg(s, xcm, ycm, angle_deg);
		} else {
			fprintf(stderr, "%s:%d unknown file output type\n",__FILE__,__LINE__);
		}
		// This box not tested specifically
		box.set(0, -descent_cm, width_cm, ascent_cm);
		box.rotate(angle_deg);
		box.shift_x(xcm);
		box.shift_y(ycm);
		break;
	case TEXT_RJUST:
		if (_output_file_type == postscript) {
			if (_grWritePS) {
				fprintf(_grPS, "%.1f %.1f m ",
					PT_PER_CM * (xcm - width_cm * cos(angle_deg / DEG_PER_RAD)),
					PT_PER_CM * (ycm - width_cm * sin(angle_deg / DEG_PER_RAD)));
				if (fabs(angle_deg) > 0.1)
					fprintf(_grPS, "%.2f rotate ", angle_deg);
				gr_drawstring(s);
			}
		} else if (_output_file_type == svg) {
			if (_grWritePS) {
				gr_drawstring_svg(s, 
						  xcm - width_cm * cos(angle_deg / DEG_PER_RAD),
						  ycm - width_cm * sin(angle_deg / DEG_PER_RAD),
						  angle_deg);
			}
		} else {
			fprintf(stderr, "%s:%d unknown file output type\n",__FILE__,__LINE__);
		}

		// This box not tested specifically
		box.set(-width_cm, -descent_cm, 0.0, ascent_cm);
		box.rotate(angle_deg);
		box.shift_x(xcm);
		box.shift_y(ycm);
		break;
	case TEXT_CENTERED:
		if (_output_file_type == postscript) {
			if (_grWritePS) {
#ifdef DEBUG
			        fprintf(_grPS, "%% DEBUG %s:%d '%s' at angle %f\n",__FILE__,__LINE__,s,angle_deg);
#endif
				fprintf(_grPS, "%.1f %.1f m ",
					PT_PER_CM * (xcm - 0.5 * width_cm * cos(angle_deg / DEG_PER_RAD)),
					PT_PER_CM * (ycm - 0.5 * width_cm * sin(angle_deg / DEG_PER_RAD)));
				if (fabs(angle_deg) > 0.1)
					fprintf(_grPS, "%.2f rotate ", angle_deg);
				gr_drawstring(s);
			}
		} else if (_output_file_type == svg) {
			if (_grWritePS) {
				gr_drawstring_svg(s, 
						  xcm - 0.5 * width_cm * cos(angle_deg / DEG_PER_RAD),
						  ycm - 0.5 * width_cm * sin(angle_deg / DEG_PER_RAD),
						  angle_deg);
			}
		} else {
			fprintf(stderr, "%s:%d unknown file output type\n",__FILE__,__LINE__);
		}
		box.set(-width_cm/2, -descent_cm, width_cm/2, ascent_cm);
		box.rotate(angle_deg);
		box.shift_x(xcm);
		box.shift_y(ycm);
		break;
	default:
		warning("gr_show_at type is UNKNOWN\n");
	}
	switch (_output_file_type) {
	case  postscript:
		if (_grWritePS) {
			if (fabs(angle_deg) > 0.1)
				fprintf(_grPS, "%.2f rotate ", -angle_deg);
			check_psfile();
			//fprintf(_grPS, "%% gr_show_at() END\n");
		}
		break;
	case svg:
		//fprintf(_grSVG, "</text>\n");
		break;
	case gif:
		fprintf(stderr, "INTERNAL error at %s:%d -- nothing known for GIF\n\n", __FILE__, __LINE__);
		exit(99);
		break;
	default:
		fprintf(stderr, "%s:%d unknown file output type\n",__FILE__,__LINE__);
		break;		// BUG: should check filetype here
	}
	// Update bounding box
	bounding_box_update(box);
	_drawingstarted = true;
}

// gr_drawstring() -- draw string, including font changes &super/subscripts
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
#ifdef DEBUG
	printf("DEBUG %s:%d gr_drawstring(\"%s\")\n",__FILE__,__LINE__,s);
#endif
	if (slen <= 0)
		return;
	if (0.0 == gr_currentfontsize_pt())
		return;
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
        case gr_font_Century:
                slant_font = gr_font_TimesItalic; // BUG: should be Century Italic
		know_slant_font = true;
		break;
	default:
		know_slant_font = false;
	}
	// Scan through whole string.
	START_NEW_TEXT;
	while (*s != '\0') {
#ifdef DEBUG
//		printf("DEBUG(%s:%d) *s= '%c'\n",__FILE__,__LINE__,*s);
#endif
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
#ifdef DEBUG
				printf("DEBUG(%s:%d) got $ so leave math mode\n",__FILE__,__LINE__);
				printf("DEBUG(%s:%d) pstack size is %d\n",__FILE__,__LINE__,int(pstack.size()));
#endif
					
				// Were in math; now go back to original font.
				inmath = false;
				if (current_font != original_font) {
					current_font = original_font;
					STOP_OLD_TEXT;
					gr_setfont(current_font);
					START_NEW_TEXT;
				}
				if (!pstack.empty()) {
					warning("a text string ended without completing a mathematical grouping (superscript, subscript, or {block})");
					pstack_erase();
				}
			} else {
#ifdef DEBUG
				printf("DEBUG(%s:%d) got $ so enter math mode\n",__FILE__,__LINE__);
#endif
				// Go to Italic/Oblique font, as case may be.  Unfortunately,
				// PostScript uses different names for this slanted font.
				inmath = true;
				if (know_slant_font) {
					current_font = slant_font;
					STOP_OLD_TEXT;
					gr_setfont(current_font);
					START_NEW_TEXT;
				}
				if (!pstack.empty()) {
					warning("a text string started without an empty mathematical grouping (superscript, subscript, or {block})");
					pstack_erase();
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
					pstack.push(Superscript);
#ifdef DEBUG
					printf("DEBUG(%s:%d) pushed superscript=%d onto stack to make length %d\n",__FILE__,__LINE__,Superscript, int(pstack.size()));
#endif
					MoveUp();
				} else if (*s == '\\') {
					// Math character to superscript
					int inc;
					int symbol_index = symbol_in_math(s, &inc);
					if (inc) {
						gr_fontID       oldfontID = gr_currentfont();
						pstack.push(Superscript);
#ifdef DEBUG
						printf("DEBUG(%s:%d) pushed subscript=%d onto stack to make length %d\n",__FILE__,__LINE__,Superscript, int(pstack.size()));
#endif
						MoveUp();
						STOP_OLD_TEXT;
						gr_setfont(gr_font_Symbol);
						if (_grWritePS) {
							fprintf(_grPS, "(%s) sh\n", symbol_code[symbol_index][1]);
							check_psfile();
						}
						gr_setfont(oldfontID);
						START_NEW_TEXT;
						s += inc;
#ifdef DEBUG
						printf("DEBUG(%s:%d) about to pop stack (was %d) to make length %d\n",__FILE__,__LINE__,pstack.top(), int(pstack.size())-1);
#endif
						MoveDown();
						pstack.pop();
					}
				} else {
					// Single character to superscript
					pstack.push(Superscript);
#ifdef DEBUG
					printf("DEBUG(%s:%d) pushed subscript=%d onto stack to make length %d\n",__FILE__,__LINE__,Subscript, int(pstack.size()));
#endif
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
					MoveDown();
#ifdef DEBUG
					printf("DEBUG(%s:%d) about to pop stack (was %d) to make length %d\n",__FILE__,__LINE__,pstack.top(), int(pstack.size())-1);
#endif
					pstack.pop();
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
					pstack.push(Subscript);
#ifdef DEBUG
					printf("DEBUG(%s:%d) pushed subscript=%d onto stack to make length %d\n",__FILE__,__LINE__,Subscript, int(pstack.size()));
#endif
					MoveDown();
				} else if (*s == '\\') {
					// Math character to subscript
                                        int inc;
					int symbol_index = symbol_in_math(s, &inc);
					if (symbol_index > -1) {
						gr_fontID oldfontID = gr_currentfont();
						pstack.push(Subscript);
#ifdef DEBUG
						printf("DEBUG(%s:%d) pushed subscript=%d onto stack to make length %d\n",__FILE__,__LINE__,Subscript, int(pstack.size()));
#endif
						MoveDown();
						STOP_OLD_TEXT;
						gr_setfont(gr_font_Symbol);
						if (_grWritePS) {
							fprintf(_grPS, "(%s) sh\n", symbol_code[symbol_index][1]);
							check_psfile();
						}
						gr_setfont(oldfontID);
						START_NEW_TEXT;
						s += inc;
						MoveUp();
#ifdef DEBUG
						printf("DEBUG(%s:%d) about to pop stack (was %d) to make length %d\n",__FILE__,__LINE__,pstack.top(), int(pstack.size())-1);
#endif
						pstack.pop();
					}
				} else {
					// Single character to subscript
					pstack.push(Subscript);
#ifdef DEBUG
					printf("DEBUG(%s:%d) pushed subscript=%d onto stack to make length %d\n",__FILE__,__LINE__,Subscript, int(pstack.size()));
#endif
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
					MoveUp();
#ifdef DEBUG
					printf("DEBUG(%s:%d) about to pop stack (was %d) to make length %d\n",__FILE__,__LINE__,pstack.top(), int(pstack.size())-1);
#endif
					pstack.pop();
				}
			} else if (*s == '{') { // just a grouping, not a baseline shift
				pstack.push(Inline);
#ifdef DEBUG
				printf("DEBUG(%s:%d) pushed Inline=%d onto stack to make length %d\n",__FILE__,__LINE__,Inline,int(pstack.size()));
#endif

			} else if (*s == '}') {	// finished with Superscript/Subscript/Inline
				if (pstack.size() > 0) {
					position p = pstack.top();
					if (p == Superscript) {
						MoveDown();
					} else if (p == Subscript) {
						MoveUp();
					} // ignore inline
#ifdef DEBUG
					printf("DEBUG(%s:%d) about to pop stack (was %d) to make length %d\n",__FILE__,__LINE__,pstack.top(), int(pstack.size())-1);
#endif
					pstack.pop();
				} else {
					warning("unmatched \"}\" in a mathematicsal string");
				}
			} else if (*s == '\\') {
				// Substitute math symbol, unless it's
				// an escaped string
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
                                        int inc;
					int symbol_index = symbol_in_math(s, &inc);
					if (inc) {
						// math symbol in symbol font
						gr_fontID oldfontID = gr_currentfont();
						STOP_OLD_TEXT;
						gr_setfont(gr_font_Symbol);
						if (_grWritePS) {
							fprintf(_grPS, "(%s) sh\n", symbol_code[symbol_index][1]);
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
	if (!pstack.empty()) {
		warning("a text string ended without completing a mathematical grouping (superscript, subscript, or {block})");
		pstack_erase();
	}
	return;
}

const char* gr_fontname_from_id(int id)
{
	switch (id) {
	case gr_font_Courier:            return("Courier");
	case gr_font_CourierOblique:     return("Courier-Italic");
	case gr_font_CourierBold:        return("Courier-Bold");
	case gr_font_CourierBoldOblique: return("Courier-BoldItalic");
	case gr_font_Helvetica:          return("Helvetica");
	case gr_font_HelveticaOblique:   return("Helvetica-Italic");
	case gr_font_HelveticaBold:      return("Helvetica-Bold");
	case gr_font_PalatinoRoman: 
	case gr_font_PalatinoItalic:
	case gr_font_PalatinoBold:
	case gr_font_PalatinoBoldItalic:
		warning("SVG cannot handle Palatino font yet");
		return("Times");
	case gr_font_Symbol:               return("Symbol");
	case gr_font_TimesRoman:           return("Times");
	case gr_font_TimesItalic:          return("Times-Italic");
	case gr_font_TimesBold:            return("Times-Bold");
	case gr_font_TimesBoldItalic:      return("Times-BoldItalic");
	case gr_font_Century:     	   return("Century");
	}
	warning("SVG defaulting to Times font");
	return("Times");
}

static void gr_drawchar_svg(char c, double xcm, double ycm, gr_fontID font_id)
{
	double size = gr_currentfontsize_pt();
	const char *font_style;
        //printf("%s:%d | gr_drawchar_svg('%c', ...) decodes to '%c' font_id=.s\n", __FILE__, __LINE__, c, symbol_code[int(c)][1]);//, gr_fontID);
	if (pstack.size() > 0)
		size *= SuperSize;
	if (isdigit(c) || font_id == gr_font_Symbol)
		font_style = "normal";
	else
		font_style = "italic";
	const char *fill = _griState.color_text().get_hexcolor().c_str();
	double transparency = _griState.color_text().getT();
        fprintf(_grSVG, "<g><text x=\"%.1f\" y=\"%.1f\" font-family=\"%s\" font-size=\"%.1f\" font-style=\"%s\" fill=\"%s\" opacity=\"%.2f\" style=\"fill:%s\">%c</text></g>\n",
                xcm * PT_PER_CM, 
                /*gr_page_height_pt() -*/ -ycm * PT_PER_CM,
                gr_fontname_from_id(font_id), size, font_style,
                fill,
                1.0 - transparency,
                fill,
                c);
	gr_setfont(font_id);
	char st[2];
	st[0] = c; 
	st[1] = '\0';
	double w, a, d;
	double oldfontsize = gr_currentfontsize_pt();
	gr_setfontsize_pt(size);
	gr_stringwidth(st, &w, &a, &d); // BUG: NEED TO SET FONT FIRST
	gr_setfontsize_pt(oldfontsize);
	//*xcm += w;
}
static void gr_drawsymbol_svg(int index, double xcm, double ycm, gr_fontID font_id)
{
	double size = gr_currentfontsize_pt();
	const char *font_style;
        //printf("%s:%d | gr_drawchar_svg('%c', ...) decodes to '%c' font_id=.s\n", __FILE__, __LINE__, c, symbol_code[int(c)][1]);//, gr_fontID);
	if (pstack.size() > 0)
		size *= SuperSize;
	if (font_id == gr_font_Symbol)
		font_style = "normal";
	else
		font_style = "italic";
	const char *fill = _griState.color_text().get_hexcolor().c_str();
	double transparency = _griState.color_text().getT();
        if (font_id == gr_font_Symbol) {
                fprintf(_grSVG, "<g><text x=\"%.1f\" y=\"%.1f\" font-family=\"%s\" font-size=\"%.1f\" font-style=\"%s\" fill=\"%s\" opacity=\"%.2f\" style=\"fill:%s\">%s</text></g>\n",
                        xcm * PT_PER_CM, 
                        /*gr_page_height_pt() -*/ -ycm * PT_PER_CM,
                        gr_fontname_from_id(font_id), size, font_style,
                        fill,
                        1.0 - transparency,
                        fill,
                        symbol_code[index][3]);
        } else {
                fprintf(_grSVG, "<g><text x=\"%.1f\" y=\"%.1f\" font-family=\"%s\" font-size=\"%.1f\" font-style=\"%s\" fill=\"%s\" opacity=\"%.2f\" style=\"fill:%s\">%s</text></g>\n",
                        xcm * PT_PER_CM, 
                        /*gr_page_height_pt() -*/ -ycm * PT_PER_CM,
                        gr_fontname_from_id(font_id), size, font_style,
                        fill,
                        1.0 - transparency,
                        fill,
                        "?");
        }
	gr_setfont(font_id);
	char st[2];
	st[0] = 'm';            // guess at width
	st[1] = '\0';
	double w, a, d;
	double oldfontsize = gr_currentfontsize_pt();
	gr_setfontsize_pt(size);
	gr_stringwidth(st, &w, &a, &d); // BUG: NEED TO SET FONT FIRST
	gr_setfontsize_pt(oldfontsize);
                                //*xcm += w; // BUG
}
static void
gr_drawstring_svg(const char *s, double xcm, double ycm, double angle)
{
#ifdef DEBUG
	printf("DEBUG %s:%d gr_drawstring_svg(s=\"%s\", xcm=%f, ycm=%f, angle=%f)\n", __FILE__,__LINE__, s, xcm, ycm, angle);
#endif
	int             slen = strlen(s);
	gr_fontID       original_font = gr_currentfont();
	gr_fontID       slant_font = original_font;	// prevent compiler warning
	double          original_fontsize = gr_currentfontsize_pt();
	bool            know_slant_font = false;
	if (slen <= 0)
		return;
	if (0.0 == gr_currentfontsize_pt())
		return;
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
        case gr_font_Century:
                slant_font = gr_font_TimesItalic; // BUG: should be Century Italic
		know_slant_font = true;
		break;
	default:
		know_slant_font = false;
	}
	std::vector<std::string> parts;
	parts = part_string(s);
	int nparts = parts.size();
	double xxcm = xcm, yycm = ycm;
	fprintf(_grSVG, "<g transform=\"translate(%.2f,%.2f)\">\n", xxcm*PT_PER_CM,
		gr_page_height_pt() - yycm*PT_PER_CM);
	xxcm = yycm = 0.0;
	fprintf(_grSVG, "<g transform=\"rotate(%.1f)\">\n", -angle);
	angle = 0.0;		// BUG: not sure if this is the best method
	//printf("DEBUG (%s)\n", _griState.color_text().get_hexcolor().c_str());
	for (int i = 0; i < nparts; i++) {
		double w, a, d;
		std::string p = parts[i];
		gr_stringwidth(p.c_str(), &w, &a, &d);
		//printf("    \"%s\" w=%f a=%f d=%f\n", parts[i].c_str(),w,a,d);
		if (p[0] != '$') {
			// Normal text
			fprintf(_grSVG, "<g><text x=\"%.1f\" y=\"%.1f\" font-family=\"%s\" font-size=\"%.1f\" font-style=\"normal\" fill=\"%s\" opacity=\"%.2f\" style=\"fill:%s\">%s</text></g>\n",
				xxcm * PT_PER_CM,
				/*gr_page_height_pt() -*/ -yycm * PT_PER_CM,
				gr_fontname_from_id(original_font),
				original_fontsize,
				_griState.color_text().get_hexcolor().c_str(),
				1.0 - _griState.color_text().getT(),
				_griState.color_text().get_hexcolor().c_str(),
				p.c_str());
			xxcm += w;
			//FIXME: should s/\$/$/ first
		} else {
			// Math text
			unsigned int ic, nc = p.size();
//			w = gr_thinspace_cm() / 1.0; // put a bit of space before math
//			xxcm += w;
			if (p[nc-1] == '$') nc--;
			fprintf(_grSVG, "<!-- math mode -->\n<g>\n");
			for (ic = 1; ic < nc; ic++) {
#ifdef DEBUG
				printf(" -- %d [%c] --\n", ic, p[ic]);
#endif
				if (p[ic] == '\\') { // HEREHEREHERE
					if (p[ic+1] == '\\') {
						gr_drawsymbol_svg(-1, xxcm, yycm, original_font);
						ic += 1;
						continue;
					}
					int inc;
					int symbol_index = symbol_in_math(p.c_str() + ic, &inc);
					if (inc) {
						gr_drawsymbol_svg(symbol_index, xxcm, yycm, gr_font_Symbol);
						ic += inc;
					} else {
						gr_drawstring_svg("?", xxcm, yycm, gr_font_Symbol);
					}
				} else if (p[ic] == '{') {
					// ignore
					pstack.push(Inline);
					ic++;
					continue;
				} else if (p[ic] == '}') {
					if (pstack.size() > 0) {
						position p = pstack.top();
						if (p == Superscript) {
							MoveDn_svg(&xxcm, &yycm);
						} else if (p == Subscript) {
							MoveUp_svg(&xxcm, &yycm);
						}
#ifdef DEBUG
						printf("DEBUG(%s:%d) about to pop stack (was %d) to make length %d\n",__FILE__,__LINE__,pstack.top(), int(pstack.size())-1);
#endif
						pstack.pop();
					} else {
						warning("unmatched \"}\" in a mathematical string");
					}
				} else if (p[ic] == '_') {
					pstack.push(Subscript);
					MoveDn_svg(&xxcm, &yycm);
					if (p[ic+1] != '{') { // BUG: should check for symbol
						gr_drawchar_svg(p[ic+1], xxcm, yycm, original_font);
						MoveUp_svg(&xxcm, &yycm);
						pstack.pop();
					}
					ic++;
					continue;
				} else if (p[ic] == '^') {
					pstack.push(Superscript);
					MoveUp_svg(&xxcm, &yycm);
					if (p[ic+1] != '{') { // BUG: should check for symbol
						gr_drawchar_svg(p[ic+1], xxcm, yycm, original_font);
						MoveDn_svg(&xxcm, &yycm);
						pstack.pop();
					}
					ic++;
					continue;
				} else {
					gr_drawchar_svg(p[ic], xxcm, yycm, original_font);
				}
			}
			fprintf(_grSVG, "</g>\n");
		}
	}
	fprintf(_grSVG, "</g>\n"); // rotate
	fprintf(_grSVG, "</g>\n"); // translate
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
 * fonts are predefined: TimesRoman Helvetica Courier Symbol Palatino-Roman
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

static int symbol_in_math(const char *sPtr, int *inc) // handle greek letter or symbol in math mode
{
	sPtr++;
	*inc = 0;
	for (int i = 0; i < NCODES; i++) {
		int len = strlen(symbol_code[i][0]);
		if (!strncmp(sPtr, symbol_code[i][0], len)) {
			*inc = len;
			return i;
		}
	}
	return -1;
}

// Clear the position stack (doesn't STL do this??)
static void
pstack_erase()
{
	while (!pstack.empty()) {
		position p = pstack.top();
		if (p == Superscript) {
			MoveDown();
		} else if (p == Subscript) {
			MoveUp();
		} // ignore inline
#ifdef DEBUG
		if (p == Superscript)
			printf("\tcleared Superscript from position stack\n");
		else if (p == Subscript)
			printf("\tcleared Subscript from position stack\n");
#endif
		pstack.pop();
	}
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
static void MoveUp_svg(double *xcm, double *ycm)
{
#ifdef DEBUG
	printf("DEBUG(%s:%d) moving text position up one level.  Stack size on entry = %d\n", __FILE__,__LINE__,(int)pstack.size());
#endif
	if (!pstack.size())
		return;
	double dy;
	// See if already in subscript.
	position p = pstack.top();
	if (p == Subscript) {
		// Moving up from subscript, so enlarge font, then undo last move
		// down.
		gr_setfontsize_pt(gr_currentfontsize_pt() / SubSize);
		dy = SubMoveDown * gr_currentCapHeight_cm();
	} else {
		// Moving up from inline or superscript, so move up, then reduce font.
		dy = SuperMoveUp * gr_currentCapHeight_cm();
		gr_setfontsize_pt(gr_currentfontsize_pt() * SuperSize);
	} // ignore Inline
	*ycm += dy;
}
// MoveDown() -- move down, shifting to smaller/larger size if necessary
static void MoveDn_svg(double *xcm, double *ycm)
{
#ifdef DEBUG
	printf("DEBUG(%s:%d) moving text position down one level\n", __FILE__,__LINE__);
#endif
	if (!pstack.size())
		return;
	double dy;
	position p = pstack.top();
	// See if already in superscript.
	if (p == Superscript) {
		// Moving down from superscript, so enlarge font, then undo last move up.
		gr_setfontsize_pt(gr_currentfontsize_pt() / SuperSize);
		dy = -SuperMoveUp * gr_currentCapHeight_cm();
	} else {
		// Moving down from inline or subscript, so move down, then reduce font.
		dy = -SubMoveDown * gr_currentCapHeight_cm();
		gr_setfontsize_pt(gr_currentfontsize_pt() * SubSize);
	}
	*ycm += dy;
}

static void
MoveUp()
{
#ifdef DEBUG
	printf("DEBUG(%s:%d) moving text position up one level\n", __FILE__,__LINE__);
#endif
	STOP_OLD_TEXT;
	// See if already in subscript.
	position p = pstack.top();
	if (p == Subscript) {
		// Moving up from subscript, so enlarge font, then undo last move
		// down.
		gr_setfontsize_pt(gr_currentfontsize_pt() / SubSize);
		gr_rmoveto_pt(0.0, SubMoveDown * gr_currentCapHeight_cm() * PT_PER_CM);
	} else {
		// Moving up from inline or superscript, so move up, then reduce font.
		gr_rmoveto_pt(0.0, SuperMoveUp * gr_currentCapHeight_cm() * PT_PER_CM);
		gr_setfontsize_pt(gr_currentfontsize_pt() * SuperSize);
	} // ignore Inline
	START_NEW_TEXT;
}

// MoveDown() -- move down, shifting to smaller/larger size if necessary
static void
MoveDown()
{
#ifdef DEBUG
	printf("DEBUG(%s:%d) moving text position down one level\n", __FILE__,__LINE__);
#endif
	STOP_OLD_TEXT;
	position p = pstack.top();
	// See if already in superscript.
	if (p == Superscript) {
		// Moving down from superscript, so enlarge font, then undo last move up.
		gr_setfontsize_pt(gr_currentfontsize_pt() / SuperSize);
		gr_rmoveto_pt(0.0, -SuperMoveUp * gr_currentCapHeight_cm() * PT_PER_CM);
	} else {
		// Moving down from inline or subscript, so move down, then reduce font.
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
#ifdef DEBUG
			printf("DEBUG %s:%d toggling inmath; rest of string is \"%s\"\n",__FILE__,__LINE__,s);
#endif
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
			else if (*s == '{')	// ignore groups while computing string length
				;		// EMPTY
			else if (*s == '}')	// ignore groups
				;		// EMPTY
			else if (*s == '\\') {	// handle synonym
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
                                        int inc;
					int symbol_index = symbol_in_math(s, &inc);
					if (symbol_index > -1) {
						gr_fontID oldfontID = CurrentFont.id;
						s += inc;
                                                // *w += gr_charwidth_cm("m" symbol_code[symbol_index][1], gr_font_Symbol, CurrentFont.size_pt);
                                                *w += gr_charwidth_cm('m', gr_font_Symbol, CurrentFont.size_pt); //  BUG
						CurrentFont.id = oldfontID;
					} else {
						// it's not a known math symbol
						*w += gr_charwidth_cm('\\', CurrentFont.id, CurrentFont.size_pt);
					}
				}
			} else {
				// We are in math-mode, but it's not a special character. Add
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
#ifdef DEBUG
	printf("DEBUG %s:%d gr_stringwidth(s=\"%s\",...) RETURNING w=%f, a=%f d=%f\n", __FILE__,__LINE__,s,*w,*a,*d);
#endif
	// reset fontsize ... can't do with gr_setfontsize_pt() 
	// because that would call this function in infinite recursion.
	CurrentFont.size_pt = oldfontsize_pt;
	_grWritePS = oldWritePS;
}

#if 0
// return index (for size-table) for a character (given as integer)
static int index_for_math_symbol(const char *s)
{
	//printf("index_for_math_symbol(%s)\n",s);
	if (!s) return (int)'?';
	for (int i = 0; i < NCODES; i++) {
		//printf("  %3d (%s) (%s)\n", i, symbol_code[i][1],symbol_code[i][2]);
		if (!strncmp(s, symbol_code[i][1], strlen(symbol_code[i][1]))) {
			//printf(" match\n");
			return (int) *symbol_code[i][2];
		}
		//printf("  no match\n");
	}
	//printf("index_for_math_symbol(%s) cannot find a match\n", s);
	return (int) 'M';	// a guess, since we have no clue
}
#endif

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

// Created by Perl script get_font_metrics.pl
struct font_metric CenturyRoman = {
    0.016369, // XHeight
    0.025471, // CapHeight
    0.026000, // Ascender
    -0.007232, // Descender
    { // Widths of first 128 characters
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0098072, 0.0104422, 0.0137231, 
        0.0196144, 0.0196144, 0.0293864, 0.0287514, 0.0071967, 
        0.0117475, 0.0117475, 0.0176389, 0.0213783, 0.0098072, 
        0.0117475, 0.0098072, 0.0098072, 0.0196144, 0.0196144, 
        0.0196144, 0.0196144, 0.0196144, 0.0196144, 0.0196144, 
        0.0196144, 0.0196144, 0.0196144, 0.0098072, 0.0098072, 
        0.0213783, 0.0213783, 0.0213783, 0.0156633, 0.0259997, 
        0.0254706, 0.0254706, 0.0254706, 0.0274461, 0.0254706, 
        0.0235303, 0.0274461, 0.0293864, 0.0143581, 0.0196144, 
        0.0274461, 0.0235303, 0.0333022, 0.0287514, 0.0274461, 
        0.0235303, 0.0274461, 0.0254706, 0.0222250, 0.0235303, 
        0.0287514, 0.0254706, 0.0346075, 0.0248356, 0.0248356, 
        0.0215547, 0.0117475, 0.0213783, 0.0117475, 0.0213783, 
        0.0176389, 0.0071967, 0.0196144, 0.0196144, 0.0156633, 
        0.0202494, 0.0176389, 0.0117475, 0.0189442, 0.0215547, 
        0.0111125, 0.0104422, 0.0209197, 0.0111125, 0.0313619, 
        0.0215547, 0.0176389, 0.0202494, 0.0196144, 0.0156633, 
        0.0163336, 0.0137231, 0.0215547, 0.0189442, 0.0274461, 
        0.0189442, 0.0189442, 0.0169686, 0.0117475, 0.0213783, 
        0.0117475, 0.0213783, 0.0000000
    }
};

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
struct font_metric PalatinoRoman = {
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
// Created by Perl script get_font_metrics.pl
struct font_metric PalatinoItalic = {
    0.017004, // XHeight
    0.024412, // CapHeight
    0.025859, // Ascender
    -0.009737, // Descender
    { // Widths of first 128 characters
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0088194, 0.0117475, 0.0176389, 
        0.0176389, 0.0176389, 0.0313619, 0.0274461, 0.0098072, 
        0.0117475, 0.0117475, 0.0137231, 0.0213783, 0.0088194, 
        0.0117475, 0.0088194, 0.0104422, 0.0176389, 0.0176389, 
        0.0176389, 0.0176389, 0.0176389, 0.0176389, 0.0176389, 
        0.0176389, 0.0176389, 0.0176389, 0.0088194, 0.0088194, 
        0.0213783, 0.0213783, 0.0213783, 0.0176389, 0.0263525, 
        0.0254706, 0.0215547, 0.0235303, 0.0274461, 0.0215547, 
        0.0196144, 0.0254706, 0.0274461, 0.0117475, 0.0117475, 
        0.0235303, 0.0196144, 0.0333022, 0.0274461, 0.0274461, 
        0.0215547, 0.0274461, 0.0235303, 0.0196144, 0.0215547, 
        0.0274461, 0.0254706, 0.0333022, 0.0254706, 0.0235303, 
        0.0235303, 0.0117475, 0.0213783, 0.0117475, 0.0213783, 
        0.0176389, 0.0098072, 0.0156633, 0.0163336, 0.0143581, 
        0.0176389, 0.0137231, 0.0098072, 0.0176389, 0.0176389, 
        0.0098072, 0.0098072, 0.0156633, 0.0098072, 0.0274461, 
        0.0196144, 0.0156633, 0.0176389, 0.0163336, 0.0137231, 
        0.0137231, 0.0117475, 0.0196144, 0.0176389, 0.0254706, 
        0.0176389, 0.0176389, 0.0156633, 0.0117475, 0.0213783, 
        0.0117475, 0.0213783, 0.0000000
    }
};



// Created by Perl script get_font_metrics.pl
struct font_metric PalatinoBold = {
    0.016616, // XHeight
    0.024024, // CapHeight
    0.025400, // Ascender
    -0.009102, // Descender
    { // Widths of first 128 characters
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0000000, 0.0000000, 0.0000000, 
        0.0000000, 0.0000000, 0.0088194, 0.0098072, 0.0141817, 
        0.0176389, 0.0176389, 0.0313619, 0.0293864, 0.0098072, 
        0.0117475, 0.0117475, 0.0156633, 0.0213783, 0.0088194, 
        0.0117475, 0.0088194, 0.0104422, 0.0176389, 0.0176389, 
        0.0176389, 0.0176389, 0.0176389, 0.0176389, 0.0176389, 
        0.0176389, 0.0176389, 0.0176389, 0.0088194, 0.0088194, 
        0.0213783, 0.0213783, 0.0213783, 0.0156633, 0.0263525, 
        0.0274461, 0.0235303, 0.0254706, 0.0293864, 0.0215547, 
        0.0196144, 0.0293864, 0.0293864, 0.0137231, 0.0137231, 
        0.0274461, 0.0215547, 0.0352778, 0.0293864, 0.0293864, 
        0.0215547, 0.0293864, 0.0254706, 0.0215547, 0.0235303, 
        0.0274461, 0.0274461, 0.0352778, 0.0235303, 0.0235303, 
        0.0235303, 0.0117475, 0.0213783, 0.0117475, 0.0213783, 
        0.0176389, 0.0098072, 0.0176389, 0.0215547, 0.0156633, 
        0.0215547, 0.0176389, 0.0137231, 0.0196144, 0.0215547, 
        0.0117475, 0.0117475, 0.0215547, 0.0117475, 0.0313619, 
        0.0215547, 0.0196144, 0.0215547, 0.0215547, 0.0137231, 
        0.0156633, 0.0117475, 0.0215547, 0.0196144, 0.0293864, 
        0.0176389, 0.0196144, 0.0176389, 0.0109361, 0.0213783, 
        0.0109361, 0.0213783, 0.0000000
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
struct font_metric TimesRoman = {
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
		return fontsize_pt * TimesRoman.width[i];
	case gr_font_Courier:
		return fontsize_pt * 0.0211663;	/* Courier has fixed width */
	case gr_font_Symbol:
		return fontsize_pt * Symbol.width[i];
	case gr_font_Helvetica:
		return fontsize_pt * Helvetica.width[i];
	case gr_font_HelveticaBold:
		return fontsize_pt * Helvetica_Bold.width[i];
	case gr_font_PalatinoRoman:
		return fontsize_pt * PalatinoRoman.width[i];
	case gr_font_Century:
                return fontsize_pt * CenturyRoman.width[i];
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
		return(-CurrentFont.size_pt * TimesRoman.Descender);
	case gr_font_Century:
		return(-CurrentFont.size_pt * CenturyRoman.Descender);
	case gr_font_PalatinoRoman:
		return(-CurrentFont.size_pt * PalatinoRoman.Descender);
	case gr_font_PalatinoItalic:
		return(-CurrentFont.size_pt * PalatinoItalic.Descender);
	case gr_font_PalatinoBold:
		return(-CurrentFont.size_pt * PalatinoBold.Descender);
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
		height_cm = (CurrentFont.size_pt * TimesRoman.CapHeight);
		break;
	case gr_font_Century:
	        height_cm = (CurrentFont.size_pt * CenturyRoman.CapHeight);
		break;
	case gr_font_PalatinoRoman:
	        height_cm = (CurrentFont.size_pt * PalatinoRoman.CapHeight);
		break;
	case gr_font_PalatinoItalic:
	        height_cm = (CurrentFont.size_pt * PalatinoItalic.CapHeight);
		break;
	case gr_font_PalatinoBold:
	        height_cm = (CurrentFont.size_pt * PalatinoBold.CapHeight);
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
