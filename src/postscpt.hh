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

// BUG: change the "n" and "S" in symbol drawing later.  gr.cc:gr_drawsymbol
#if !defined(_postscpt_)

#define _postscpt_

#define PS_break_path           "stroke newpath\n%.1f %.1f m\n"
#define	PS_closepath		"h\n"
#define	PS_comment		"%%%s\n"
#define	PS_fill			"F\n"
#define	PS_fillrect		"%.1f %.1f %.1f %.1f fr\n"
#define	PS_lineto		"%.1f %.1f l\n"
#define	PS_rlineto		"%.1f %.1f rl\n"
#define	PS_moveto		"%.1f %.1f m\n"
#define	PS_rmoveto		"%.1f %.1f rm\n"
#define	PS_weak_newpath		"n\n"
#define	PS_newpath		"newpath\n"
#define	PS_setgray		"%.3f g\n"
#define	PS_setlinewidth		"%.3f w\n"
#define	PS_showc		"(%c) sh\n"
#define	PS_showpage		"showpage\n"
#define	PS_stroke		"S\n"

// Commands to retire, when get a chance to recode next:
// rl rm ALL TEXT COMMANDS
const static char *PS_dict[] =
{
	"% NOTE: The Gri postscript dictionary is being converted to the Adobe",
	"% Illustrator 3.0 dialect of PostScript, as described in the Adobe",
	"% documents stored at URL",
	"%   http://www.adobe.com/Support/TechNotes.html",
	"% (as of Jan 1996, this doc is number 5007).  When the conversion",
	"% is complete, the Adobe Illustrator drawing program -- and any",
	"% program compatible with AI -- will be able to edit Gri output.",
	"%",
	"% The IslandDraw (TM) program is able to read Gri output",
	"% at this time; remarkably, it can read/edit arbitrary PostScript.",
	"%",
	"% The definitions below are presented in the same order as the Adobe",
	"% manual.  The stack configuration before and after is shown in curly",
	"% brackets.  All the operators are listed, but only some are defined",
	"% here. Most things are faithful, except that no distinction is made",
	"% between colors for stroking and filling paths.  The string 'WRONGLY'",
	"% appears with commands that are approximations.",
	"%",
	"% PDF-style abbreviations:",
	"/rg {setrgbcolor} def   % {red green blue}            {-} set RGB color",
	"/RG {setrgbcolor} def   % {red green blue}            {-} set RGB color",
	"/q {gsave} def",
	"/Q {grestore} def",
	"/W {clip} def",
	"/W* {eoclip} def",
	"%",
	"% Gri-specific abbreviations:",
	"/hsb {sethsbcolor} def  % {hue saturation brightness} {-} set HSB color",
	"%",
	"% Following all try to mimic Adobe Illustrator",
	"%",
	"% Mimicking section 5.1 of Adobe manual:",
	"%A                      % {flag A}          {-} Determine whether",
	"                        %                       following object can",
	"                        %                       be selected. Flag=1",
	"                        %                       prevents selection;",
	"                        %                       flag=0 allows it.",
	"",
	"% Mimicking section 5.2 of Adobe manual:",
	"%u                      % {u}               {-} start group",
	"%U                      % end group",
	"%q                      % as 'u' but first item is a clip path",
	"%Q                      % as 'U' but first item is a clip path",
	"",
	"% Mimicking section 5.3 of Adobe manual:",
	"/g {setgray} def        % {gray g}          {-} Set gray for fill",
	"                        %                       path, WRONGLY used",
	"                        %                       for stroking also. ",
	"/G {setgray} def        % As 'g', but for filling path.",
	"%k                      % Set cmyk color for filling path.",
	"%K                      % As 'k', but for stroking path.",
	"%x                      % Set cmyk custom color for filling path.",
	"%X                      % As 'x' but for stroking path.",
	"%p                      % Define pattern for filling path.",
	"%P                      % As 'p' but for stroking path.",
	"%O                      % Specify whether overprinting for fill paths",
	"%R                      % As 'O' but for stroking path.",
	"",
	"% Mimicking section 5.4 of Adobe manual:",
	"/d {setdash} def        % {[array] phase d} {-} Set dash.",
	"/i {setflat} def        % {flatness i}      {-} Set flatness.",
	"/j {setlinejoin} def    % {linejoin j}      {-} Set line join.",
	"/J {setlinecap} def     % {linecap J}       {-} Set line cap.",
	"/M {setmiterlimit} def  % {miterlimit M}    {-} Set miter limit.",
	"/w {setlinewidth} def   % {linewidth w}     {-} Set line width.",
	"",
	"% Mimicking section 5.5 of Adobe manual:",
	"/m {moveto} def         % {x y m}           {-} Move to locn",
	"/l {lineto} def         % {x y l}           {-} Draw line to locn",
	"                        %                       not a smooth point.",
	"                        %                       WRONGLY, no",
	"                        %                       distinction is made",
	"                        %                       between smooth and",
	"                        %                       corner. ",
	"%L                      % {x y L}           {-} As 'l' but a corner",
	"%c                      % Bezier curve to smooth point.",
	"%C                      % As 'c' but to corner point.",
	"%v                      % Something else to do with Bezier.",
	"%V                      % ",
	"%y                      % ",
	"%Y                      % ",
	"",
	"% Mimicking section 5.6 of Adobe manual:",
	"%N                      % {N}               {-} As 'n' for nondrawn stuff",
	"/n {newpath} def        % {n}               {-} WRONGLY interpreted",
	"                        %                       as path constructor",
	"/F {fill} def           % {F}               {-} Fill current path.",
	"%f                      % {f}               {-} 'F' but close first",
	"/S {stroke} def         % {S}               {-} Stroke current path.",
	"%s                      % {s}               {-} 'S' but close first",
	"%B                      % {B}               {-} As 's' but don't empty path.",
	"%b                      % {b}               {-} As 'f' but don't empty path.",
	"%H                      % no-op (weird huh?)",
	"/h {closepath} def      % {h}               {-} Close  current path",
	"%W                      % Used to create masks.",
	"",
	"% Mimicking section 5.7 of Adobe manual:",
	"%a                      % Begin text block ...",
	"%e                      % Similar to 'a' but ...",
	"%I                      % Similar to 'a' but ...",
	"%o                      % Similar to 'a' but ...",
	"%r                      % Similar to 'a' but ...",
	"%t                      % {len (string) t}  {-} Render string.",
	"%T                      % End block of text",
	"",
	"% That's the end of the Illustrator stuff. Following are some Gri",
	"% definitions which provide a temporary way of handling fonts. ",
	"/sf {setfont} def       % {fontname sf}     {-} Set font name.",
	"/sh {show} def          % {(text) sh}       {-} Show text.",
	"/sc {scalefont} def     % {size sc}         {-} Scale font.",
	"% Gri items which should be translated to Illustrator format:",
	"/rl {rlineto} def",
	"/rm {rmoveto} def",
    
	"% Procedures",
	"/cimdict 7 dict def",
	"/cim {",
	" cimdict begin",
	" /cl exch def",
	" /rw exch def",
	" /yur exch def",
	" /xur exch def",
	" /yll exch def",
	" /xll exch def",
	" q",
	" xll yll translate",
	" xur xll sub yur yll sub scale",
	" /do cl 3 mul string def",
	" cl rw 8 [cl 0 0 rw neg 0 rw] {currentfile do readhexstring pop} false 3 colorimage",
	" Q",
	" end",
	"} def",
    
#if 1				// suggested 2001-mar-01 as workaround for ps2pdf bug
	"/imdict 14 dict def",
	"/im {",
	" imdict begin",
	" /cl exch def",
	" /rw exch def",
	" /yur exch def",
	" /xur exch def",
	" /yll exch def",
	" /xll exch def",
	" /imagemap exch def",
	" q",
	" % Until version 2.6.0 used a 'settransfer' here, but that",
	" % triggers a bug in ps2pdf",
	" xll yll translate",
	" xur xll sub yur yll sub scale",
	" /do cl string def",
	" cl rw 8 [cl 0 0 rw neg 0 rw]",
	" {currentfile do readhexstring pop",
	"  dup length 1 sub 0 1 3 -1 roll",
	"    { 1 index exch",
	"      2 copy get imagemap exch get 255 mul cvi put",
	"    }",
	"  for",
	" }image",
	" Q",
	" end",
	"} bind def",
#else
	"/imdict 14 dict def",
	"/im {",
	" imdict begin",
	" /cl exch def",
	" /rw exch def",
	" /yur exch def",
	" /xur exch def",
	" /yll exch def",
	" /xll exch def",
	" /imagemap exch def",
	" q",
	" % Add the mapping to the transfer function (ref: white book, p 743.",
	" [{255 mul cvi imagemap exch get} /exec load currenttransfer /exec load]",
	"   cvx settransfer",
	" xll yll translate",
	" xur xll sub yur yll sub scale",
	" /do cl string def",
	" cl rw 8 [cl 0 0 rw neg 0 rw] {currentfile do readhexstring pop}image",
	" Q",
	" end",
	"} def",
#endif
    
	"/frdict  5 dict def",
	"/fr {",
	" frdict begin",
	" /yt exch def",
	" /xr exch def",
	" /yb exch def",
	" /xl exch def",
	" n",
	" xl yb m",
	" xl yt l",
	" xr yt l",
	" xr yb l",
	" h",
	" F",
	" n",
	" end",
	"} def",
	"/plusdict 3 dict def",
	"/_plus {",
	" plusdict begin",
	" dup 0.5 mul /t0 exch def",
	" /t1 exch def",
	" 0 t0 rm",
	" 0 t1 neg rl",
	" t0 neg t0 rm",
	" t1 0 rl",
	" t0 neg 0 rm",
	" end",
	"} def",
	"/timesdict 3 dict def",
	"/_times {",
	" timesdict begin",
	" dup 0.353553 mul /t0 exch def",
	" 0.707106 mul /t1 exch def",
	" t0 neg t0 rm",		// upper left
	" t1 dup neg rl",		// lower right
	" t1 neg 0 rm",		// lower left
	" t1 dup rl",		// upper right
	" t0 neg dup rm",		// centre
	" end",
	"} def",
	"/boxdict 3 dict def",
	"/_box {",
	" boxdict begin",
	" dup 0.5 mul /t0 exch def",
	" 1 mul /t1 exch def",
	" t0 neg t0 rm",		// top left
	" t1 0 rl",			// top right 
	" 0 t1 neg rl",		// bottom right 
	" t1 neg 0 rl",		// bottom left 
	" h",			// back to top left 
	" t0 dup neg rm",		// end at centre 
	" end",
	"} def",
	"/filledboxdict 3 dict def",
	"/_filledbox {",
	" filledboxdict begin",
	" dup 0.5 mul /t0 exch def",
	" 1 mul /t1 exch def",
	" t0 neg t0 rm",		// top left 
	" t1 0 rl",			// top right 
	" 0 t1 neg rl",		// bottom right 
	" t1 neg 0 rl",		// bottom left 
	" h",			// back to top left 
	" t0 dup neg rm",		// end at centre 
	" F end",
	"} def",
	"/diamonddict 2 dict def",
	"/_diamond {",
	" diamonddict begin",
	" 0.5 mul /t0 exch def",
	" t0 neg 0 rm",		// left 
	" t0 dup rl",		// top 
	" t0 dup neg rl",		// right 
	" t0 neg dup rl",		// bottom 
	" h",			// back to left 
	" t0 0 rm",			// end at centre 
	" end",
	"} def",
	"/filleddiamonddict 2 dict def",
	"/_filleddiamond {",
	" filleddiamonddict begin",
	" 0.5 mul /t0 exch def",
	" t0 neg 0 rm",		// left 
	" t0 dup rl",		// top 
	" t0 dup neg rl",		// right 
	" t0 neg dup rl",		// bottom 
	" h",			// back to left 
	" t0 0 rm",			// end at centre 
	" F end",
	"} def",
	"/triangleupdict 5 dict def",
	"/_triangleup {",
	" triangleupdict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t1 neg t0 neg rm",	// bottom left 
	" t1 t2 rl",		// top 
	" t1 t2 neg rl",		// bottom right 
	" h",			// back to bottom left 
	" t1 t0 rm",		// end at centre 
	" end",
	"} def",

	"/filledtriangleupdict 5 dict def",
	"/_filledtriangleup {",
	" filledtriangleupdict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t1 neg t0 neg rm",	// bottom left 
	" t1 t2 rl",		// top 
	" t1 t2 neg rl",		// bottom right 
	" h",			// back to bottom left 
	" t1 t0 rm",		// end at centre 
	" F end",
	"} def",

	"/trianglerightdict 5 dict def",
	"/_triangleright {",
	" trianglerightdict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t0 neg t1 rm",		// top 
	" t2 t1 neg rl",		// right 
	" t2 neg t1 neg rl",	// bottom 
	" h",			// back to top 
	" t0 t1 neg rm",		// end at centre 
	" end",
	"} def",

	"/filledtrianglerightdict 5 dict def",
	"/_filledtriangleright {",
	" filledtrianglerightdict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t0 neg t1 rm",		// top 
	" t2 t1 neg rl",		// right 
	" t2 neg t1 neg rl",	// bottom 
	" h",			// back to top 
	" t0 t1 neg rm",		// end at centre 
	" F end",
	"} def",

	"/triangledowndict 5 dict def",
	"/_triangledown {",
	" triangledowndict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t1 neg t0 rm",		// top left 
	" t3 0 rl",			// top right 
	" t1 neg t2 neg rl",	// bottom 
	" h",			// back to top left 
	" t1 t0 neg rm",		// end at centre 
	" end",
	"} def",

	"/filledtriangledowndict 5 dict def",
	"/_filledtriangledown {",
	" filledtriangledowndict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t1 neg t0 rm",		// top left 
	" t3 0 rl",			// top right 
	" t1 neg t2 neg rl",	// bottom 
	" h",			// back to top left 
	" t1 t0 neg rm",		// end at centre 
	" F end",
	"} def",

	"/triangleleftdict 5 dict def",
	"/_triangleleft {",
	" triangleleftdict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t0 t1 rm",		// top 
	" 0 t3 neg rl",		// bottom 
	" t2 neg t1 rl",		// left 
	" h",			// back to top 
	" t0 neg t1 neg rm",	// end at centre 
	" end",
	"} def",

	"/filledtriangleleftdict 5 dict def",
	"/_filledtriangleleft {",
	" filledtriangleleftdict begin",
	" dup 0.25 mul /t0 exch def",
	" dup 0.433013 mul /t1 exch def",
	" dup 0.75 mul /t2 exch def",
	" 0.866026 mul /t3 exch def",
	" t0 t1 rm",		// top 
	" 0 t3 neg rl",		// bottom 
	" t2 neg t1 rl",		// left 
	" h",			// back to top 
	" t0 neg t1 neg rm",	// end at centre 
	" F end",
	"} def",

	"/circdict 5 dict def",
	"/_circ {",
	" circdict begin",
	" 0.5 mul /t0 exch def",
	" currentpoint",
	" /t2 exch def",
	" /t1 exch def",
	" S n t1 t2 t0 0 360 arc",
	" t1 t2 m",
	" end",
	"} def",

	"/bulldict 3 dict def",	// stack: diameter 
	"/_bull {",
	" bulldict begin",
	" 0.5 mul /r exch def",
	" currentpoint /y exch def /x exch def",
	" S n x y r 0 360 arc h F S",
	" end",
	"} def",

	"/filledhalfmoonupdict 3 dict def",	// stack: diameter 
	"/_filledhalfmoonup {",
	" bulldict begin",
	" 0.5 mul /r exch def",
	" currentpoint /y exch def /x exch def",
	" S n x y r 0 180 arc h F S",
	" end",
	"} def",

	"/filledhalfmoondowndict 3 dict def",	// stack: diameter 
	"/_filledhalfmoondown {",
	" bulldict begin",
	" 0.5 mul /r exch def",
	" currentpoint /y exch def /x exch def",
	" S n x y r 180 360 arc h F S",
	" end",
	"} def",
	NULL
};

#endif				// _postscpt_ 
