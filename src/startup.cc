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

//#define TEST_POPT // uncomment this to test the POPT library

#include	<string>
#include	<unistd.h>
#include	<stdio.h>
#include	<time.h>

#ifdef TEST_POPT
#if defined(HAVE_OLD_POPT)
extern "C" {
#include        <popt.h>
}
#else
#include        <popt.h>
#endif
#endif

#include	"gr.hh"
#include	"extern.hh"
#include        "private.hh"
#include	"defaults.hh"
#include	"files.hh"
#include        "gr_coll.hh"
#include        "macro.hh"
#include        "GriColor.hh"
#include        "superus.hh"

// The next few lines are trying to get around an intermittent
// problem with Solaris compilation.  I don't compile on Solaris
// often, and every time I do, I have to either comment-out,
// or to uncomment, the declaration below.  I guess I could put
// in a few more lines here, checking for this or that version of
// this or that compiler, but that would be ugly.  Maybe this
// should go into the configure.in file, but it is difficult for
// me to do that, without access to solaris machines that behave
// differently from one another.  Besides, the error message on 
// this is pretty clear, and this is a reported bug on
// gri.sourceforge.net (number 618041), at
// http://sourceforge.net/tracker/index.php?func=detail&aid=618041&group_id=5511&atid=105511
// and so I'll just leave this here for now, half-broken on
// Solaris.
#if defined(HAVE_GETHOSTNAME)
#if defined(IS_SUN)
//extern "C" unsigned int gethostname(char *name, int namelen);
#endif
#endif

#if defined(OS_IS_BEOS)
#include <netdb.h>
#endif
static std::vector<GriNamedColor> colorStack;

static const char** argv_leftover;

typedef struct {
	unsigned int code;
	char *action;
} superuser_flag;
static superuser_flag sflag[] = {
	{FLAG_SYN,  (char *)"Print cmdline before/after sub synonyms"},
	{FLAG_RPN,  (char *)"Print cmdline before/after sub rpn"},
	{FLAG_NEW,  (char *)"Print new commands being defined"},
	{FLAG_SYS,  (char *)"Print system commands before passing"},
	{FLAG_FLOW, (char *)"Indicate program flow"},
	{FLAG_AUT2, (char *)"Variable; for use by developers only"},
	{FLAG_AUT1, (char *)"Variable; for use by developers only"},
	{0, NULL}
};
static std::string psname("");
static bool user_gave_directory = false;

extern char     _grTempString[];

bool            display_colors();
static void     create_builtin_colors(void);
bool            _no_startup_message = false;
bool            _contour_label_rotated = false;   	// <-> draw.c set.c
bool            _contour_label_whiteunder = true;	// <-> draw.c set.c
bool            _store_cmds_in_ps = true;	  		// <-> read.c
bool 			_private = true; 					// "-private" and "-no_private" commandline options

bool            initialize_image(); 
bool            initialize_imageMask(); 
bool            create_arrays(void);
static void     create_builtin_variables(void);
static void     create_builtin_synonyms(void);
static void     set_defaults(void);
const char**    interpret_optional_arguments(int argc, char *argv[]);
static void     get_input_simulation(int argc, const char *argv[]);
static void     insert_creator_name_in_PS(int argc, char *argv[], const std::string&psname);
static void     dogrirc(void);
#if 0
static void     show_startup_msg(void);
#endif
int             last_optional_arg = 0;


// RETURN 1 if found color and dumped RGB into red, green, blue
bool
look_up_color(const char *name, double *red, double *green, double *blue)
{
	int i, num = colorStack.size();
	for (i = 0; i < num; i++) {
		if (!strcmp(name, colorStack[i].get_name().c_str())) {
			*red   = colorStack[i].getR();
			*green = colorStack[i].getG();
			*blue  = colorStack[i].getB();
			return true;
		}
	}
	return false;
}

bool
create_color(const char *name, double r, double g, double b)
{
	GriNamedColor color(name, r, g, b);
	colorStack.push_back(color);
	return true;
}

#if 0
// RETURN value contents of s following the last ',' in s, or all of s
static          char*
last_name(char *s)
{
	int             len = strlen(s);
	int             i;
	for (i = len - 1; i > -1; i--) {
		if (s[i] == ',') {
			if (i != len - 1)
				i++;
			break;
		}
	}
	char *return_value;
	if (i < 0) {
		// no comma
		return_value = new char[1 + len];
		if (!return_value) OUT_OF_MEMORY;
		strcpy(return_value, s);
	} else {
		return_value = new char[1 + len - i];
		if (!return_value) OUT_OF_MEMORY;
		strcpy(return_value, s + i);
	}
	return return_value;
}
#endif

bool
start_up(int argc, char **argv)
{
	extern rectangle _page_size;
	_page_size.set(0.0, 0.0, 0.0, 0.0);

/*
#if defined(TEST_POPT)
	printf("DEBUG: %s:%d: FYI, start_up() found the raw args to be:\n",__FILE__,__LINE__);
	for (int i = 0; i < argc; i++) printf("DEBUG:                     '%s'\n", argv[i]);
#endif
*/
#ifdef OSX_BUNDLE
//	printf("%s:%d OSX_BUNDLE being used\n",__FILE__,__LINE__);
#endif

	_output_file_type = postscript;
	// Record version number
	int major_version, minor_version, minor_minor_version;
	extern char _input_data_separator; // defined in gri.cc
	_input_data_separator = ' ';
	extern char     _gri_number[];
	sscanf(_gri_number, "%d.%d.%d", &major_version, &minor_version, &minor_minor_version);
	_version = major_version + minor_version / 100.0 
		+ minor_minor_version / 10000.0;
	_arrow_type = 0;		// default
	// Get storage space for arrays, variables, synonyms, etc
	initialize_image();
	initialize_imageMask();
	create_arrays();
	create_builtin_variables();
	create_builtin_synonyms();
	set_up_command_word_buffer();
	set_defaults();
	// Initialize some globals.
	_nword = 0;	// Prevent problems.
	_contour_label_rotated = false; // see `set contour labels rotated'
	_contour_label_whiteunder = true; // see `set contour labels whiteunder
	_axes_offset = 0.0;
	_use_default_for_query = false;
	_warn_offpage = true;
	PUT_VAR("..use_default_for_query..", 0.0);
	PUT_VAR("..words_in_dataline..", 0.0); // just in case tested
	PUT_VAR("..batch..", 0.0);
	_debugFlag = 0;
	PUT_VAR("..debug..", 0.0);
	PUT_VAR("..eof..", 0.0);
	PUT_VAR("..landscape..", 0.0);
	PUT_VAR("..publication..", 0.0);
	PUT_VAR("..xlast..", 0.0);
	PUT_VAR("..ylast..", 0.0);
	PUT_VAR("..image_width..", 0.0);
	PUT_VAR("..image_height..", 0.0);
	PUT_VAR("..q1..",0.0);
	PUT_VAR("..q2..",0.0);
	PUT_VAR("..q3..",0.0);

#ifdef OSX_BUNDLE
        extern std::string _lib_directory;
        _lib_directory.assign(argv[0]);
	std::string::size_type last_slash = _lib_directory.rfind("/");
        if (last_slash != STRING_NPOS) {
		_lib_directory.STRINGERASE(last_slash, _lib_directory.length());
        } else {
		fatal_err("OSX error: cannot find slash in argv[0]\n");
        }
//        printf("%s:%d: OSX_BUNDLE using _lib_directory '%s'\n", __FILE__, __LINE__, _lib_directory.c_str());
#endif
	// Get leftover (non-optional) arguments
	argv_leftover = interpret_optional_arguments(argc, argv);
	unsigned int argc_leftover = 0;
	//printf("%s:%d argc_leftover= %d:\n",__FILE__,__LINE__,argc_leftover);
	while (argv_leftover && argv_leftover[argc_leftover] != NULL) {
		//printf("\t<%s>\n", *argv_leftover);
		argc_leftover++;
	}
	//printf("end. LEFTOVER.  have %d\n",argc_leftover);
	if (argc_leftover == 0) {
		_margin.assign("  ");
		push_cmd_file("stdin", batch() ? false : true, true, "r");
		Require(put_syn("\\.command_file.", "stdin", true), OUT_OF_MEMORY);
	} else {
		std::string fname(argv_leftover[0]);
		Require(put_syn("\\.command_file.", fname.c_str(), true), OUT_OF_MEMORY);
		//printf("FILENAME '%s'\n",fname.c_str());
		// If filename shorter than 4 characters, cannot have .gri suffix,
		// so append it.
		std::string::size_type p = fname.rfind(".gri");
		if (fname.size() < 4 || p != -4 + fname.size())
			fname.append(".gri");

		// If user didn't give psname, create it.  First, must trim the
		// ".gri" suffix (which is sure to be there).  Then, must remove
		// any filename path, since we want the .ps or .eps
		// file to be created in this local directory .
		if (psname.empty()) {
			psname = fname;
			int l = psname.size();
			psname.STRINGERASE(l - 4, l - 1);
			std::string::size_type last_slash = psname.rfind("/");
			if (last_slash != STRING_NPOS)
				psname.STRINGERASE(0, last_slash + 1);
			psname.append(".ps");
		}
		if (!push_cmd_file(fname.c_str(), false, false, "r")) {
			fprintf(stderr, "Gri cannot open commandfile `%s'\n", fname.c_str());
			delete_ps_file();
			gri_exit(1);
		}

#if 0
		// Possibly they gave a ps filename ...
		if (argc > last_optional_arg + 2) {
			std::string tmp(argv[last_optional_arg+2]);
			std::string::size_type p = tmp.rfind(".ps");
			if (p != STRING_NPOS && p == -3 + tmp.size()) {
				warning("\
first argument looks like a PostScript filename.  Older versions\n\
         of Gri allowed you to specify the PostScript name that way,\n\
         but now you must use the \"-output\" option, as for example:\n\
             gri ... -output \\", tmp.c_str(), " ...\n\
         As it is, Gri is using the filename `", psname.c_str(), "'.", "\\");
				//psname = tmp;
			}
		}
#endif
		gr_setup_ps_filename(psname.c_str());
	}

	//printf("DEBUG2 separator= %d\n", separator);
	get_input_simulation(argc_leftover, argv_leftover);

	insert_creator_name_in_PS(argc, argv, psname);
	// Finally, ready to begin plot.
	gr_begin(1);

	// Embed info on how gri was invoked.
	if (!_private) {
		extern FILE *_grPS;
		char host[BUFSIZ];
#if defined(HAVE_GETHOSTNAME)
		if (0 != gethostname(host, BUFSIZ - 1))
			strcpy(host, "unknown");
#else
		strcpy(host, "unknown");
#endif
		fprintf(_grPS, "%%gri:# Gri was invoked by user named\n%%gri:#     %s\n%%gri:# on host named\n%%gri:#     %s\n%%gri:# using the command\n%%gri:#   ",  egetenv("USER"), host);
		for (int i = 0; i < argc; i++)
			fprintf(_grPS, " %s", argv[i]);
		SECOND_TYPE sec;
		time(&sec);
		sprintf(_grTempString, "%s", asctime(localtime(&sec)));
		_grTempString[-1 + strlen(_grTempString)] = '\0'; // trim newline
		fprintf(_grPS, "\n%%gri:# at local time %s.\n", _grTempString);
	}
	put_syn("\\.ps_file.", gr_currentPSfilename(), true);
	// Disable tracing during startup phase, unless in superuser mode.
	double          trace_old;
	get_var("..trace..", &trace_old);
	PUT_VAR("..trace..", 0.0);
	// Do the gri.cmd file.  Note that 'create_commands()' will take care of
	// the searching for the gri.cmd file.
	bool tmp = _store_cmds_in_ps;
	_store_cmds_in_ps = false;
	if (superuser() & FLAG_AUT2)
		printf("Processing gri.cmd ...");
	create_commands(GRI_COMMANDS_FILE, user_gave_directory);
	if (superuser() & FLAG_AUT2)
		printf(" done\n");
	create_builtin_colors();

	// Define default paths
	char* griinputs = egetenv("GRIINPUTS");
	if (griinputs != NULL && strlen(griinputs) > 0) {
		warning("Your GRIINPUTS environment variable has been IGNORED.\n         In this version of gri, you must put the line\n             set path \"\\",
			griinputs, "\" for commands\n         in your ~/.grirc file to get the same effect.\n", "\\");
	}

	put_syn("\\.path_data.", ".", true);
	put_syn("\\.path_commands.", ".", true);

	// Do user's ~/.grirc file.
	_store_cmds_in_ps = tmp;
	dogrirc();

	// Re-enable tracing
	PUT_VAR("..trace..", trace_old);

	// DataFile stack
	DataFile new_data_file;
	if (superuser() & FLAG_AUT1)printf("\nDEBUG: %s:%d pushing back a datafile at address %lx\n",__FILE__,__LINE__,(long unsigned int)(&new_data_file));
	_dataFILE.push_back(new_data_file);

	_first = true;
	_bounding_box.set(0.0, 0.0, 0.0, 0.0);
	//printf("At end of start_up, _cmdLine is <%s>\n",_cmdLine);
	return true;
}

static void
create_builtin_colors()
{
	GriNamedColor color;
	color.setNameRGB("white",		1.000,	1.000,	1.000);
	colorStack.push_back(color);
	color.setNameRGB("LightGray",		0.827,	0.827,	0.827);
	colorStack.push_back(color);
	color.setNameRGB("darkslategray",	0.184,	0.310,	0.310);
	colorStack.push_back(color);
	color.setNameRGB("black",		0.000,	0.000,	0.000);
	colorStack.push_back(color);
	color.setNameRGB("red",			1.000,	0.000,	0.000);
	colorStack.push_back(color);
	color.setNameRGB("brown",		0.647,	0.165,	0.165);
	colorStack.push_back(color);
	color.setNameRGB("tan",			0.824,	0.706,	0.549);
	colorStack.push_back(color);
	color.setNameRGB("orange",		1.000,	0.647,	0.000);
	colorStack.push_back(color);
	color.setNameRGB("yellow",		1.000,	1.000,	0.000);
	colorStack.push_back(color);
	color.setNameRGB("green",		0.000,	1.000,	0.000);
	colorStack.push_back(color);
	color.setNameRGB("ForestGreen",		0.133,	0.545,	0.133);
	colorStack.push_back(color);
	color.setNameRGB("cyan",		0.000,	1.000,	1.000);
	colorStack.push_back(color);
	color.setNameRGB("blue",		0.000,	0.000,	1.000);
	colorStack.push_back(color);
	color.setNameRGB("skyblue",		0.529,	0.808,	0.922);
	colorStack.push_back(color);
	color.setNameRGB("magenta",		1.000,	0.000,	1.000);
	colorStack.push_back(color);
}

bool
display_colors()
{
	char msg[200];
	for (unsigned int i = 0; i < colorStack.size(); i++) {
		sprintf(msg, "Color `%s' has RGB = (%f,%f,%f)\n",
			colorStack[i].get_name().c_str(),
			colorStack[i].getR(),
			colorStack[i].getG(),
			colorStack[i].getB());
		ShowStr(msg);
	}
	return true;
}

#if 0
// get full filename, searching through directory list
char *
file_in_list(const char *name, bool show_nonlocal_files, bool show_local_files)
{
	//printf("file_in_list(%s,...)\n", name);
	char *            return_name;
#if defined(VMS) || defined(MSDOS)
	// BUG -- Cannot search list yet
	return_name = new char[1 + strlen(name)];
	if (!return_name) OUT_OF_MEMORY;
	strcpy(return_name, name);
	return return_name;
#else
	// Obey absolute filenames
	if (*name == '/') {
		return_name = new char[1 + strlen(name)];
		if (!return_name) OUT_OF_MEMORY;
		strcpy(return_name, name);
		return return_name;
	} else {
		// Filename from list
		char* tmp = egetenv("GRIINPUTS");
		if (!tmp || !strlen(tmp))
			tmp = GRIINPUTS;
		char* griinputs = new char[1 + strlen(tmp)];
		if (!griinputs) OUT_OF_MEMORY;
		char* to_free = griinputs;
		strcpy(griinputs, tmp);
		return_name = new char[2 + strlen(griinputs) + strlen(name)];
		if (!return_name) OUT_OF_MEMORY;
		griinputs = strtok(griinputs, ":");
		do {
			FILE           *fp;
			sprintf(return_name, "%s/%s", griinputs, name);
			if ((fp = fopen(return_name, "r"))) {
				if ((return_name[0] == '.' && show_local_files)
				    || (return_name[0] != '.' && show_nonlocal_files)) {
					if (!batch()) {
						ShowStr("Will use commandfile `");
						ShowStr(return_name);
						ShowStr("'\n");
					}
				}
				fclose(fp);
				delete [] to_free;
				return return_name;
			}
		} while ((griinputs = (char *) strtok(NULL, ":")));
		// Could not find commandfile.
		ShowStr("FATAL ERROR: could not locate the commandfile named\n      ");
		ShowStr(name);
		ShowStr("\n  in the current directory, nor in any directory");
		ShowStr("\n  specified in the colon-separated directory list\n      ");
		ShowStr(tmp);
		ShowStr("\n  (To change this directory list, you may modify the");
		ShowStr("\n  GRIINPUTS environment variable.)\n");
	
		extern FILE *_grPS;
		fclose(_grPS);
#ifdef VMS
		char tmp[1024];
		sprintf(tmp, "DEL %s;*", gr_currentPSfilename());
		call_the_OS(tmp, __FILE__, __LINE__);
#else
		sprintf(tmp, "rm -f %s", gr_currentPSfilename());
		call_the_OS(tmp, __FILE__, __LINE__);
#endif
		gri_exit(1);
		return NULL;		// never done, actually
	}
#endif
}
#endif

static void			// save extra words
get_input_simulation(int argc_leftover, const char *argv_leftover[])
{
	if (argc_leftover < 1)
		return;
	extern std::vector<const char*> _gri_argv;
	for (int i = 0; i < argc_leftover; i++) {
		//printf("\t push %d <%s>\n",i,argv_leftover[i]);
#if 0				// 2001-feb-23 vsn 2.6.0 (alpha)
		gr_textsave(argv_leftover[i]);
#endif
		_gri_argv.push_back(argv_leftover[i]);
	}
}

static void
set_defaults()
{
	extern char      _xtype_map, _ytype_map;
	
	_contourFmt.assign(CONTOUR_FMT_DEFAULT);
	_current_directory.assign(".");
	// Set up various other defaults
	_xFmt.assign(X_FMT_DEFAULT);
	_yFmt.assign(Y_FMT_DEFAULT);
	_colU.setName("u");
	_colV.setName("v");
	_colX.setName("x");
	_colY.setName("y");
	_colZ.setName("z");
	_colWEIGHT.setName("weight");
	_prompt.assign("gri: ");
	PUT_VAR("..arrowsize..", ARROWSIZE_DEFAULT);
	PUT_VAR("..exit_status..", 0.0);
	_axesStyle = 0;
	_braceLevel = 0;
	_gri_beep = false;
	_chatty = 1;
	// Fonts
	PUT_VAR("..fontsize..", FONTSIZE_PT_DEFAULT);
	gr_setfontsize_pt(FONTSIZE_PT_DEFAULT);
	gr_setfont(FONT_DEFAULT);
	_clipData = 0;
	_columns_exist = false;
	_done = 0;
	_drawingstarted = false;
	_error_in_cmd = false;
	_exit_value = 0;
	PUT_VAR("..graylevel..", 0.0);
	_ignore_eof = false;
	_ignore_error = false;
	_dash.erase(_dash.begin(), _dash.end()); // go to solid
	PUT_VAR("..linewidth..", LINEWIDTH_DEFAULT);
	PUT_VAR("..linewidthaxis..", LINEWIDTHAXIS_DEFAULT);
	PUT_VAR("..linewidthsymbol..", LINEWIDTHSYMBOL_DEFAULT);
	_grid_exists = false;
	_xgrid_exists = false;
	_ygrid_exists = false;
	// 
	gr_set_missing_value(MISSING_VALUE);
	PUT_VAR("..missingvalue..", gr_currentmissingvalue());
	{
		char tmp[100];
		sprintf(tmp, "%f", gr_currentmissingvalue());
		put_syn("\\.missingvalue.", tmp, true);
	}
	_f_min = _f_max = gr_currentmissingvalue();	// BUG: what if not using missing values?
#if 1				// version > 2.12.7
	gr_set_missing_value_none();
#endif
	_need_x_axis = true;
	_need_y_axis = true;
	_num_xmatrix_data = 0;
	_num_ymatrix_data = 0;
	PUT_VAR("..symbolsize..", SYMBOLSIZE_DEFAULT);
	_top_of_plot = 0.0;
	_uscale_exists = false;
	_vscale_exists = false;
	_xatbottom = true;
	PUT_VAR("..xmargin..", XMARGIN_DEFAULT);
	_xscale_exists = false;
	_xsubdiv = 1;
	_xtype = gr_axis_LINEAR;
	_xtype_map = ' ';
	PUT_VAR("..xsize..", XSIZE_DEFAULT);
	PUT_VAR("..xleft..", 0.0);
	PUT_VAR("..xright..", XSIZE_DEFAULT);
	PUT_VAR("..xinc..", XSIZE_DEFAULT);
	PUT_VAR("..xlabelling..", 0.0);
	_yatleft = true;
	PUT_VAR("..ymargin..", YMARGIN_DEFAULT);
	PUT_VAR("..ysize..", YSIZE_DEFAULT);
	PUT_VAR("..ybottom..", 0.0);
	PUT_VAR("..ytop..", YSIZE_DEFAULT);
	PUT_VAR("..yinc..", YSIZE_DEFAULT);
	PUT_VAR("..ylabelling..", 0.0);
	_yscale_exists = false;
	_ysubdiv = 1;
	_ytype = gr_axis_LINEAR;
	_ytype_map = ' ';
}

static void
create_builtin_synonyms()
{
	extern char       _gri_number[];
	char *            user;
	char *            os;
	char *            wd;
	char *            home;
	// Use this for return codes
	if (!put_syn("\\.return_value.", "", true)) OUT_OF_MEMORY;
	// \.version. (version number)
	if (!put_syn("\\.version.", _gri_number, true)) OUT_OF_MEMORY;
	// \.pid. (process ID)
	sprintf(_grTempString, "%d", int(getpid()));
	if (!put_syn("\\.pid.", _grTempString, true)) OUT_OF_MEMORY;
	// \.wd. (working directory)
	wd = pwd();
	if (!put_syn("\\.wd.", wd, true)) OUT_OF_MEMORY;
	// \.time. (time)
	SECOND_TYPE sec;
	time(&sec);
	strcpy(_grTempString, asctime(localtime(&sec)));
	_grTempString[-1 + strlen(_grTempString)] = '\0'; // trim newline
	if (!put_syn("\\.time.", _grTempString, true)) OUT_OF_MEMORY;
	//  \.user. (user name)
	user = egetenv("USER");
	if (user) {
		if (!put_syn("\\.user.", user, true)) OUT_OF_MEMORY;
	} else {
		if (!put_syn("\\.user.", "unknown", true)) OUT_OF_MEMORY;
	}
	// \.host. (host computer name)
#if defined(HAVE_GETHOSTNAME)
	char            host[BUFSIZ];
	if (0 == gethostname(host, BUFSIZ - 1)) {
		if (!put_syn("\\.host.", host, true)) OUT_OF_MEMORY;
	} else {
		if (!put_syn("\\.host.", "unknown", true)) OUT_OF_MEMORY;
	}
#else
	if (!put_syn("\\.host.", "unknown", true)) OUT_OF_MEMORY;
#endif
	// \.os. (operating system)
	os = egetenv("SYSTEM");
	if (os) {
		if (!put_syn("\\.system.", os, true)) OUT_OF_MEMORY;
	} else {
		if (!put_syn("\\.system.", "unknown", true)) OUT_OF_MEMORY;
	}
	// \.home. (home directory)
	home = egetenv("HOME");
	if (home) {
		if (!put_syn("\\.home.", home, true)) OUT_OF_MEMORY;
	} else {
		if (!put_syn("\\.home.", "unknown", true)) OUT_OF_MEMORY;
	}
}

static void
create_builtin_variables()
{
	// create `..name..' variables
	PUT_VAR("..num_col_data..", _colX.size());
	PUT_VAR("..num_col_data_missing..", 0);
	PUT_VAR("..arrowsize..", ARROWSIZE_DEFAULT);
	PUT_VAR("..batch..", 0.0);
	_debugFlag = 0;
	PUT_VAR("..debug..", 0.0);
	PUT_VAR("..fontsize..", FONTSIZE_PT_DEFAULT);
	PUT_VAR("..graylevel..", 0.0);
	_dash.erase(_dash.begin(), _dash.end()); // go to solid
	PUT_VAR("..linewidth..", LINEWIDTH_DEFAULT);
	PUT_VAR("..linewidthaxis..", LINEWIDTHAXIS_DEFAULT);
	PUT_VAR("..linewidthsymbol..", LINEWIDTHSYMBOL_DEFAULT);
	PUT_VAR("..missingvalue..", gr_currentmissingvalue());
	{
		char tmp[100];
		sprintf(tmp, "%f", gr_currentmissingvalue());
		put_syn("\\.missingvalue.", tmp, true);
	}
	PUT_VAR("..symbolsize..", SYMBOLSIZE_DEFAULT);
	PUT_VAR("..superuser..", 0.0);
	PUT_VAR("..trace..", 0.0);
	PUT_VAR("..tic_direction..", 0.0); // out
	PUT_VAR("..tic_size..", TICSIZE_DEFAULT);
	PUT_VAR("..xmargin..", XMARGIN_DEFAULT);
	PUT_VAR("..xsize..", XSIZE_DEFAULT);
	PUT_VAR("..ymargin..", YMARGIN_DEFAULT);
	PUT_VAR("..ysize..", YSIZE_DEFAULT);
	PUT_VAR("..red..", 0.0);
	PUT_VAR("..blue..", 0.0);
	PUT_VAR("..green..", 0.0);
}

#if defined(HAVE_LIBPOPT) && defined(TEST_POPT)
const char**
interpret_optional_arguments(int argc, char *argv[])
{
#define FLAG_DIRECTORY		1000
#define FLAG_DIRECTORY_DEFAULT	1001
#define FLAG_CREATOR            1002
#define FLAG_NO_BOUNDING_BOX	1003
#define FLAG_NO_CMD_IN_PS	1004
#define FLAG_NO_EXPECTING	1005
#define FLAG_NO_STARTUP_MESSAGE	1006
#define FLAG_NO_WARN_OFFPAGE	1007
#define FLAG_OUTPUT		1008
#define FLAG_PUBLICATION	1009
#define FLAG_SUPERUSER		1010
#define FLAG_WARN_OFFPAGE	1011
	// I use the 'FLAG_...' numbers for options that lack single-character abbreviations.
	static struct poptOption optionsTable[] = {
		{ "batch",             'b',  POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, 'b'                   },
		{ "chatty",            'c',  POPT_ARG_INT    | POPT_ARGFLAG_ONEDASH, NULL, 'c'                   },
		{ "creator",           '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH, NULL, FLAG_CREATOR          },
		{ "debug",             'd',  POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, 'd'                   },
		{ "directory",         '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH, NULL, FLAG_DIRECTORY        },
		{ "directory_default", '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_DIRECTORY_DEFAULT},
		{ "help",              'h',  POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, 'h'                   },
		{ "output",            '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH, NULL, FLAG_OUTPUT           },
		{ "no_bounding_box",   '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_NO_BOUNDING_BOX  },
		{ "no_cmd_in_ps",      '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_NO_CMD_IN_PS     },
		{ "no_startup_message",'\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_NO_STARTUP_MESSAGE},
		{ "no_warn_offpage",   '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_NO_WARN_OFFPAGE  },
		{ "private",           '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_PRIVATE          },
		{ "no_private",        '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_NO_PRIVATE       },
		{ "publication",       '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_PUBLICATION      },
		{ "superuser",         '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH, NULL, FLAG_SUPERUSER        },
		{ "trace",             't',  POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, 't'                   },
		{ "version",           'v',  POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, 'v'                   },
		{ "warn_offpage",      '\0', POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, FLAG_WARN_OFFPAGE     },
		{ "yes",               'y',  POPT_ARG_NONE   | POPT_ARGFLAG_ONEDASH, NULL, 'y'                   },
		{  0,                   0,                                       0,     0, 0                     }
	};
	const poptContext optCon =
		poptGetContext("gri",
			       argc, 
#if !defined(HAVE_OLD_POPT)
			       (const char**)
#endif
			       argv,
			       optionsTable,
			       0);
	poptReadDefaultConfig(optCon, 0); // for aliasing ... this seems broken though
	int arg;
	extern char _gri_number[];
#ifndef OSX_BUNDLE
	_lib_directory.assign(DEFAULT_GRI_DIR);
#endif
#ifdef OSX_BUNDLE
	printf("%s:%d _lib_directory is '%s'\n",__FILE__,__LINE__,_lib_directory.c_str());
#endif
	while ((arg = poptGetNextOpt(optCon)) > 0) {
		const char *optArg = poptGetOptArg(optCon);
		int ival;
		switch (arg) {
		case 'b':
			PUT_VAR("..batch..", 1.0);
		        //printf("DEBUG: %s:%d set to 'batch' mode\n",__FILE__,__LINE__);
			break;
		case 'c':
			//printf("DEBUG: %s:%d <%s>\n",__FILE__,__LINE__,optArg);
			if (1 == sscanf(optArg, "%d", &ival))
				_chatty = ival;
			else
				_chatty = 1;
		        //printf("DEBUG: %s:%d set to 'chatty' level %d\n",__FILE__,__LINE__, _chatty);
			break;
		case 'd':
		        //printf("DEBUG: %s:%d set to 'debug' mode\n",__FILE__,__LINE__);
			PUT_VAR("..debug..", 1);
			_debugFlag = 1;
			break;
		case 'h':
			give_help();
			//printf("DEBUG: %s:%d set the 'help' flag\n",__FILE__,__LINE__);
			gri_exit(0);
			break;	// never executed
		case 't':
			//printf("DEBUG: %s:%d set the 'trace' flag\n",__FILE__,__LINE__);
			PUT_VAR("..trace..", 1.0);
			break;
		case 'v':
			gr_textput("gri version ");
			gr_textput(_gri_number);
			gr_textput("\n");
			gri_exit(0);
			break;	// never done
		case 'y':
			_use_default_for_query = true;
			PUT_VAR("..use_default_for_query..", 1.0);
			break;
		case FLAG_DIRECTORY:
			user_gave_directory = true;
			_lib_directory.assign(optArg);
			//printf("DEBUG: %s:%d got directory as '%s'\n",__FILE__,__LINE__,optArg);
			break;
		case FLAG_DIRECTORY_DEFAULT:
			gr_textput(_lib_directory.c_str());
			gr_textput("\n");
			gri_exit(0);
			break;	// never done
		case FLAG_OUTPUT:
		{
			std::string o(optArg);
			std::string::size_type suffix_index = o.rfind(".");
			if (suffix_index != STRING_NPOS) {
				if (strEQ(o.c_str() + suffix_index, ".ps")) {
					psname.assign(optArg);
					gr_setup_ps_filename(psname.c_str());
				} else if (strEQ(o.c_str() + suffix_index, ".eps")) {
					psname.assign(optArg);
					gr_setup_ps_filename(psname.c_str());
				} else if (strEQ(o.c_str() + suffix_index, ".gif")) {
					fprintf(stderr, "%s:%d: GIF output does not work yet\n", __FILE__,__LINE__);
					_output_file_type = gif;
				} else if (strEQ(o.c_str() + suffix_index, ".svg")) {
					extern FILE *_grSVG;
					fprintf(stderr, "%s:%d: SVG output to file '%s' does not work yet\n", __FILE__,__LINE__, o.c_str());
					_output_file_type = svg;
					psname.assign(optArg);
					gr_setup_ps_filename(o.c_str());
					_grSVG = fopen(o.c_str(), "w");
					if (!_grSVG) {
						fatal_err("Cannot open SVG file named `\\", o.c_str(), "'", "\\");
					}
					fprintf(_grSVG, "<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
					//fprintf(stderr, "%s:%d: SVG error: assuming height and width both 500 pixels, for now\n", __FILE__,__LINE__);
					extern rectangle _page_size;
					_page_size.set(0.0, 0.0, 8.5, 11.0); // BUG: SVG setting fixed paper size
					_page_size.scale(CM_PER_IN);
					fprintf(_grSVG, "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n", int(8.5*PT_PER_IN), int(11.0*PT_PER_IN));
				} else {
					warning("Sorry, cannot determine type of output file; using default postscript filename instead");
				}
			} else {
				// Assume to be (an odd) ps name
				psname.assign(optArg);
				gr_setup_ps_filename(psname.c_str());
			}
		}
			break;
		case FLAG_WARN_OFFPAGE:
			_warn_offpage = true;
			break;
		case FLAG_NO_BOUNDING_BOX:
			{
				extern bool _no_bounding_box;
				_no_bounding_box = true;
			}
			break;
		case FLAG_NO_CMD_IN_PS:
			_store_cmds_in_ps = false; // <-> read.c
			break;
		case FLAG_NO_STARTUP_MESSAGE:
			_no_startup_message = true;
			break;
		case FLAG_NO_WARN_OFFPAGE:
			_warn_offpage = false;
			break;
		case FLAG_PRIVATE:
			_private = true;
			break;
		case FLAG_NO_PRIVATE:
			_private = false;
			break;
		case FLAG_PUBLICATION:
			PUT_VAR("..publication..", 1.0);
			break;
		case FLAG_CREATOR:
			{
				FILE *fp;
				if (NULL == (fp = fopen(optArg, "r")))
					fatal_err("`gri -creator' cannot open file `\\", optArg, "'", "\\");
				GriString inLine(128); // Start short
				while (!inLine.line_from_FILE(fp)) {
					if (!strncmp(inLine.getValue(), "%gri:", 5)) {
						ShowStr(inLine.getValue() + 5);
					}
				}
			}
			gri_exit(0);
			break;	// never executed
		case FLAG_SUPERUSER:
			if ('?' == *optArg) {
				superuser_flag *sf = sflag;
				printf("Superuser flags, with actions:\n");
				while (sf->action != NULL) {
					printf(" -superuser %d\t => %s\n", sf->code, sf->action);
					sf++;
				}
				gri_exit(0);
			}
			if (1 == sscanf(optArg, "%d", &ival)) {
				PUT_VAR("..superuser..", double(ival));
				printf("got superuser as %d\n",ival);
			} else {
				PUT_VAR("..superuser..", 1.0);
				printf("Gri cannot read in '%s'\n",optArg);
			}
			break;
		default:
			printf("Unknown option\n");
			break;
		}
	}
	const char *last_option = poptBadOption(optCon,arg);
	//printf("DEBUG %s:%d last_option = '%s'  arg=%d\n",__FILE__,__LINE__,last_option,arg);
	if (arg <= 0 && *last_option == '-') {
		fprintf(stderr, "Unknown option `%s'.  Type `gri -h' for valid options\n", last_option);
		gri_exit(1);
	}
	put_syn("\\.lib_dir.", _lib_directory.c_str(), true);
	return (const char**) poptGetArgs(optCon);
	//printf("DEBUG: %s:%d last_option [%s]\n",__FILE__,__LINE__,last_option);
}

#else // #if defined(HAVE_LIBPOPT) && defined(TEST_POPT)

const char**
interpret_optional_arguments(int argc, char *argv[])
{
	// BUG: REMOVE THIS (LONG) BLOCK WHEN POPT IS FINALLY WORKING!
	extern char     _gri_number[];
	int             number_optional_arg = 0;
#ifndef OSX_BUNDLE
	_lib_directory.assign(DEFAULT_GRI_DIR);
#endif
#ifdef OSX_BUNDLE
	//printf("%s:%d _lib_directory is '%s'\n",__FILE__,__LINE__,_lib_directory.c_str());
#endif
	// Interpret optional [-] arguments if they exist.
	if (argc > 1) {
		int             i;
		for (i = 1; i < argc; i++) {
#if 0
			printf("argv[%d] = '%s'\n", i, argv[i]);
#endif
			// `gri -creator PostScript_filename' is a special case.  It now
			// replaces the old (unix-only) system command grilog.
			if (!strcmp(argv[i], "-creator")) {
				FILE           *fp;
				if (i + 1 > argc - 1)
					fatal_err("`gri -creator' needs a filename");
				if (NULL == (fp = fopen(argv[i + 1], "r")))
					fatal_err("`gri -creator' cannot open file `\\", argv[2], "'", "\\");
				GriString inLine(128); // Start short
				while (!inLine.line_from_FILE(fp)) {
					if (!strncmp(inLine.getValue(), "%gri:", 5)) {
						ShowStr(inLine.getValue() + 5);
					}
				}
				gri_exit(0);
			}

			// First character tells whether optional arg
			if (*argv[i] == '-') {
				int             val;
				extern bool _no_bounding_box;
				if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "-version")) {
					gr_textput("gri version ");
					gr_textput(_gri_number);
					gr_textput("\n");
					gri_exit(0);
				} else if (!strcmp(argv[i], "-no_bounding_box")) {
					_no_bounding_box = true;
				} else if (!strcmp(argv[i], "-no_startup_message")) {
					_no_startup_message = true;
				} else if (!strcmp(argv[i], "-no_cmd_in_ps")) {
					_store_cmds_in_ps = false; // <-> read.c
				} else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "-batch")) {
					PUT_VAR("..batch..", 1.0);
				} else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "-debug")) {
					PUT_VAR("..debug..", 1);
					_debugFlag = 1;
				} else if (!strcmp(argv[i], "-warn_offpage")){
					_warn_offpage = true;
				} else if (!strcmp(argv[i], "-nowarn_offpage")){
					_warn_offpage = false;
				} else if (!strcmp(argv[i], "-directory")) {
					user_gave_directory = true;
					number_optional_arg++;
					i++;
					if (i < argc)
						_lib_directory.assign(argv[i]);
					else {
						err("`-directory' needs an argument.");
						gri_exit(1);
					}
				} else if (!strcmp(argv[i], "-output")) {
					number_optional_arg++;
					i++;
					if (i < argc) {
						std::string o(argv[i]);
						std::string::size_type suffix_index = o.rfind(".");
						if (suffix_index != STRING_NPOS) {
							if (strEQ(o.c_str() + suffix_index, ".ps")) {
								psname.assign(argv[i]);
								gr_setup_ps_filename(psname.c_str());
							} else if (strEQ(o.c_str() + suffix_index, ".eps")) {
								psname.assign(argv[i]);
								gr_setup_ps_filename(psname.c_str());
							} else if (strEQ(o.c_str() + suffix_index, ".gif")) {
								fprintf(stderr, "%s:%d: GIF output does not work yet\n", __FILE__,__LINE__);
								_output_file_type = gif;
							} else if (strEQ(o.c_str() + suffix_index, ".svg")) {
								extern FILE *_grSVG;
								_output_file_type = svg;
								_grSVG = fopen(o.c_str(), "w");
								if (!_grSVG) {
									fatal_err("Cannot open SVG file named `\\", o.c_str(), "'", "\\");
								}
								psname.assign(o.c_str());
								gr_setup_ps_filename(o.c_str());
								fprintf(_grSVG, "<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
								//fprintf(stderr, "%s:%d: SVG error: assuming height and width both 500 pixels, for now\n", __FILE__,__LINE__);
								extern rectangle _page_size;
								_page_size.set(0.0, 0.0, 8.5, 11.0); // BUG: SVG setting fixed paper size
								_page_size.scale(CM_PER_IN);
								fprintf(_grSVG, "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n", int(8.5*PT_PER_IN), int(11.0*PT_PER_IN));
							} else {
								warning("Sorry, cannot determine type of output file; using default postscript filename instead");
							}
						} else {
							// Assume to be (an odd) ps name
							psname.assign(argv[i]);
							gr_setup_ps_filename(psname.c_str());
						}
					} else {
						err("`gri ... -output FILENAME' needs the FILENAME!");
						gri_exit(1);
					}
				} else if (!strcmp(argv[i], "-directory_default")) {
					gr_textput(_lib_directory.c_str());
					gr_textput("\n");
					gri_exit(0);
				} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help")) {
					give_help();
					gri_exit(0);
				} else if (!strcmp(argv[i], "-private")) {
					_private = true;
				} else if (!strcmp(argv[i], "-no_private")) {
					_private = false;
				} else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "-publication")) {
					PUT_VAR("..publication..", 1.0);
				} else if (!strncmp(argv[i], "-c", 2)) {
					if (1 == sscanf(argv[i], "-c%d", &val)) {
						_chatty = val;
					} else if (1 == sscanf(argv[i + 1], "%d", &val)) {
						_chatty = val;
						number_optional_arg++;
						i++;
					} else {
						_chatty = 1;
					}
				} else if (!strcmp(argv[i], "-chatty")) {
					if (1 == sscanf(argv[i], "-chatty%d", &val)) {
						_chatty = val;
					} else if (1 == sscanf(argv[i + 1], "%d", &val)) {
						_chatty = val;
						number_optional_arg++;
						i++;
					} else {
						_chatty = 1;
					}
				} else if (!strcmp(argv[i], "-no_expecting")) {
					warning("Gri no longer demands to find an `expecting' command, so the `-no_expecting' option can be dropped.");
				} else if (!strncmp(argv[i], "-superuser", 10)) {
					if ('?' == *(argv[i] + 10)) {
						superuser_flag *sf = sflag;
						printf("Superuser flags:\n");
						while (sf->action != NULL) {
							printf("-superuser%d -- %s\n", sf->code, sf->action);
							sf++;
						}
						gri_exit(0);
					}
					if (1 == sscanf(argv[i], "-superuser%d", &val)) {
						PUT_VAR("..superuser..", ((double) val));
					} else {
						PUT_VAR("..superuser..", ((double) 1.0));
					}
				} else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "-trace")) {
					PUT_VAR("..trace..", 1.0);
				} else if (!strcmp(argv[i], "-y") || !strcmp(argv[i], "-yes")) {
					_use_default_for_query = true;
					PUT_VAR("..use_default_for_query..", 1.0);
#if 0
				} else if (!strcmp(argv[i], "-e")) {
					// User wants to do cmd in argv[i+1]
					if (argc < i + 2)
						fatal_err("\n The -e switch needs a command to do.");
					printf("SHOULD BE EXECUTING `%s'\n", argv[i + 1]);
					i++;
					number_optional_arg++;
#endif
				} else {
					fatal_err("unknown commandline flag `\\",
						  argv[i],
						  "'.\n  Type 'gri -help' to see how to run Gri.",
						  "\\");
				}
				number_optional_arg++;
			} else {
				//printf("%s:%d DEBUG last option was argv[%d] = '%s'\n",__FILE__,__LINE__,i,argv[i]);
				break;
			}
		}
	}
        return (const char**)(&argv[1 + number_optional_arg]);
}
#endif // #if defined(HAVE_LIBPOPT) && defined(TEST_POPT)

void
give_help()
{
        gr_textput("NAME\n");
        gr_textput("     gri - draw scientific graphs\n");
        gr_textput("\n");
        gr_textput("SYNOPSIS\n");
        gr_textput("     gri [OPTIONS] [command_file [optional_arguments]]\n");
        gr_textput("\n");
        gr_textput("DESCRIPTION\n");
        gr_textput("     If a command file (command_file) is named, commands are read from that file;\n");
        gr_textput("     otherwise they are read from the keyboard.  If a command_file is named, then\n");
        gr_textput("     a file in which to store the PostScript output may also be named; otherwise\n");
        gr_textput("     it is stored in a file named gr-00.ps (or gr-01.ps if gr-00.ps exists, etc).\n");
        gr_textput("\n");
        gr_textput("     There are 3 special forms that do no graphing:\n");
        gr_textput("       `gri -creator postscript_file'\n");
        gr_textput("         Extracts the Gri commands that created the Gri PostScript file,\n");
		gr_textput("         but only if the Gri invocation that created the PostScript file\n");
		gr_textput("         had used the -no_private commandline option, or if the version of\n");
		gr_textput("         Gri that produced the file was earlier than 2.12.10.\n");
        gr_textput("       `gri -help' or `gri -h'\n");
        gr_textput("         Prints this help message.\n");
        gr_textput("       `gri -version' or `gri -v'\n");
        gr_textput("         Prints the version number of Gri.\n");
        gr_textput("\n");
        gr_textput("     In normal usage, where drawing is expected, Gri takes these options:\n");
        gr_textput("     -batch or -b\n");
        gr_textput("             Stops printing of prompts and hints.\n");
        gr_textput("     -chatty[N] or -c[N]\n");
        gr_textput("             Let gri print info messages\n");
        gr_textput("     -debug or -d\n");
        gr_textput("             Turns debugging on (sets variable ..debug.. to value 1).\n");
        gr_textput("     -warn_offpage\n");
        gr_textput("             Warn if any item is drawn far off a 8.5x11\" page.\n");
        gr_textput("             (This is the default.)\n");
        gr_textput("     -no_warn_offpage\n");
        gr_textput("             Don't warn if any item is drawn far off a 8.5x11\" page\n");

        gr_textput("     -directory pathname\n");
        gr_textput("             Specifies the directory where Gri looks for startup files;\n");
        gr_textput("             otherwise it looks in /opt/gri/lib or at whatever\n");
        gr_textput("             directory is defined in configure shellscript, at compile time.\n");
        gr_textput("     -directory_default\n");
        gr_textput("             Reports directory where gri.cmd should be found,\n");
        gr_textput("             if not supplied by -directory.\n");


        gr_textput("     -no_bounding_box\n");
        gr_textput("             Make bounding-box be full page.\n");

        gr_textput("     -no_expecting\n");
        gr_textput("             Prevent warning message if `expecting version .n.'\n");
        gr_textput("             command is missing.\n");

        gr_textput("     -no_startup_message\n");
        gr_textput("             Stops printing of startup message.\n");

        gr_textput("     -output file_name\n");
        gr_textput("             Specify the name of the file to hold the graphical output.  If\n");
        gr_textput("             this flag is not specified, the file will be PostScript,\n");
		gr_textput("             and its name will be derived from the name of the\n");
		gr_textput("             commandfile, e.g. `mygraph.gri'\n");
        gr_textput("             will produce `mygraph.ps'), or, for interactive use,\n");
        gr_textput("             it will have a name like `gri-00.ps', or\n");
        gr_textput("             `gri-01.ps' if the former file exists, etc.\n");

       	gr_textput("     -private\n");
		gr_textput("             Prevents inserting any information about the user into\n");
	  	gr_textput("             the PostScript file (see -no_private, next).  As of\n");
	    gr_textput("             version 2.12.10, this privacy option is assumed by default.\n");

       	gr_textput("     -no_private\n");
  		gr_textput("             Instructs Gri to include comments in the PostScript file that\n"); 
		gr_textput("             identify the user, state the commandline arguments used in\n"); 
		gr_textput("             invoking Gri, and that list all the commands that were executed.\n"); 
		gr_textput("             This information can be recovered by calling Gri on the\n"); 
		gr_textput("             PostScript file, with the -creator commandline argument.\n"); 
		gr_textput("             Until version 2.12.10, the default was to include this \n"); 
		gr_textput("             information, but a change was made out of privacy concerns.\n"); 

        gr_textput("     -publication or -p\n");
        gr_textput("             Sets the builtin variable ..publication.. to 1; normally it is 0.\n");
        gr_textput("             One might use if statements (`if !..publication..' ...) on drafts.\n");

        gr_textput("     -superuser\n");
        gr_textput("             Used mainly by Gri programmers (who can check the value with the\n");
        gr_textput("             C function `superuser()'.)  An optional value can be supplied\n");
        gr_textput("             without spaces (e.g. `-s2') to set the debugging level.\n");
        gr_textput("             The flags are as follows:\n");
        gr_textput("                 1: print cmdline before/after substituting synonyms\n");
        gr_textput("                 2: print cmdline before/after substituting rpn expressions\n");
        gr_textput("                 4: print new commands being defined\n");
        gr_textput("                 8: print system commands and `open \"... | \"'\n");
        gr_textput("                    commands before they are passed to the system\n");
        gr_textput("               128: for author's use only\n");
        gr_textput("               256: for author's use only\n");
        gr_textput("             Note that all flags are equal to 2 raised to an\n");
        gr_textput("             integer power.  Since the flag values are detected by\n");
        gr_textput("             a bitwise OR, you can combine flags by adding; thus\n");
        gr_textput("             specifying a flag of 5 yields flags 1 and 4 together; specifying\n");
        gr_textput("             15 yields flags 1, 2, 4 and 8.\n");
        gr_textput("     -trace or -t\n");
        gr_textput("             Makes Gri print out command lines as they are executed.\n");
        gr_textput("     -true or -y\n");
        gr_textput("             Makes Gri think the answer to all `query's is RETURN.\n");
#if 0
        gr_textput("     -e cmd\n");
        gr_textput("             BUG: NOT IMPLEMENTED YET!\n");
        gr_textput("             Makes Gri perform indicated command after\n");
        gr_textput("             doing any commands in the ~/.grirc file.\n");
#endif
}



// Insert Creator info in PS file
static void
insert_creator_name_in_PS(int argc, char *argv[], const std::string& psname)
{
	extern char _gri_release_time[];
	extern char _gri_number[]; // see version.c
	gr_setup_creatorname("Gri");
	gr_setup_creatorname(_gri_number);
	gr_setup_creatorname(" (released ");
	gr_setup_creatorname(_gri_release_time);
	if (_private)
		gr_setup_creatorname(")");
	else
		gr_setup_creatorname(").  User=");
	if (egetenv("USER"))
		gr_setup_creatorname(egetenv("USER"));
#if 0				// SF bug 711354
	if (argc > 1 + last_optional_arg) {
		// Commandfile name was supplied on cmdline.
		Require2(put_syn("\\.command_file.",
				 filename_sans_dir(argv[last_optional_arg + 1]),
				 true),
			 OUT_OF_MEMORY);
		Require2(put_syn("\\.readfrom_file.", 
				 filename_sans_dir(argv[last_optional_arg + 1]),
				 true),
			 OUT_OF_MEMORY);
		gr_setup_creatorname(", commandfile=");
		printf("argc=%d  last_optional_arg=%d [%s] [%s]\n",argc,last_optional_arg,argv[last_optional_arg],argv[last_optional_arg+1]);
		gr_setup_creatorname(argv[last_optional_arg + 1]);
		Require2(put_syn("\\.ps_file.", psname.c_str(), true), OUT_OF_MEMORY);
	} else {
		// No commandfile supplied on cmdline.
		Require2(put_syn("\\.command_file.", "stdin", true), OUT_OF_MEMORY);
		Require2(put_syn("\\.readfrom_file.", "stdin", true), OUT_OF_MEMORY);
	}
#endif
}

static void
dogrirc()
{
	char            grircname[100];
	_done = 0;
	_margin.assign("| ");
#if defined(HAVE_GETENV)
	char *home = egetenv("HOME");
	if (!home) {
		warning("Cannot getenv(HOME), so cannot perform grirc file");
		return;
	}
	sprintf(grircname, "%s/%s", home, GRIRC_FILE);
#else
	strcpy(grircname, GRIRC_FILE);
#endif
	if (push_cmd_file(grircname, false, false, "r")) {
		gr_comment("gri:#\n");
		gr_comment("gri:# The user's ~/.grirc file ...\n");
		while (do_command_line()){
			;
		}
		_done = 0;
		gr_comment("gri:# ... end of users ~/.grirc file.\n");
		gr_comment("gri:\n");
	}
}

#if 0
static void
show_startup_msg()
{
	std::string fullfilename(_lib_directory.c_str());
	// Must check for '/' as file separator, on some machines.
#if !defined(VMS)
#if defined(MSDOS)
	// Insert a '\' if required 
	if (fullfilename[fullfilename.length() - 1] != '\\') {
		fullfilename += "\\";
	}
#else
	// Insert a '/' if required 
	if (fullfilename[fullfilename.length() - 1] != '/') {
		fullfilename += "/";
	}
#endif
#endif
	fullfilename += "startup.msg";
	FILE *fp = fopen(fullfilename.c_str(), "r");
	if (fp) {
		GriString inLine(128);
		while (!inLine.line_from_FILE(fp)) {
			ShowStr(inLine.getValue());
		}
		fclose(fp);
	}
}
#endif
