//#define _POSIX_SOURCE 1		// for gethostname ... not that it works!

#include	<string>
#include	<unistd.h>
#include	<stdio.h>
#include	<time.h>
#include	"gr.hh"
#include	"extern.hh"
#include        "private.hh"
#include	"defaults.hh"
#include	"files.hh"
#include        "gr_coll.hh"
#include        "macro.hh"
#include        "GriColor.hh"
#include        "superus.hh"

#if defined(OS_IS_BEOS)
#include <netdb.h>
#endif
static vector <GriNamedColor> colorStack;


extern char     _grTempString[];

bool            display_colors();
static void     create_builtin_colors(void);
bool            _no_startup_message = false;
bool            _contour_label_rotated = false;   // <-> draw.c set.c
bool            _contour_label_whiteunder = true; // <-> draw.c set.c
bool            _store_cmds_in_ps = true;	  // <-> read.c

int             create_arrays(void);
static void     create_builtin_variables(void);
static void     create_builtin_synonyms(void);
static void     set_defaults(void);
static int      interpret_optional_arguments(int argc, char *argv[]);
static void     get_input_simulation(int argc, char *argv[], int separator);
static void     insert_creator_name_in_PS(int argc, char *argv[], const string&psname);
static void     dogrirc(void);
#if 0
static void     show_startup_msg(void);
#endif
int             last_optional_arg = 0;
#define SEPARATOR ":"		// used to separate query input

// RETURN 1 if found color and dumped RGB into red, green, blue
bool
look_up_color(const char *name, double *red, double *green, double *blue)
{
	int             i = 0, num = colorStack.size();
	for (i = 0; i < num; i++) {
		if (!strcmp(name, colorStack[i].getName())) {
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

bool
start_up(int argc, char **argv)
{
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
	PUT_VAR("..debug..", 0.0);
	PUT_VAR("..eof..", 0.0);
	PUT_VAR("..landscape..", 0.0);
	PUT_VAR("..publication..", 0.0);
	PUT_VAR("..lastx..", 0.0);
	PUT_VAR("..lasty..", 0.0);
	PUT_VAR("..image_width..", 0.0);
	PUT_VAR("..image_height..", 0.0);
	last_optional_arg = interpret_optional_arguments(argc, argv);
	// Get name of PostScript file if it was provided.  Must do this before
	// getting cmd file because of demands of gr_setup.  Also, check here for
	// input_simulation words.
	int separator = argc;
	for (int i = argc - 1; i > -1; i--)
		if (!strcmp(argv[i], SEPARATOR))
			separator = i;
	get_input_simulation(argc, argv, separator);

	string psname("");
	if (separator == 3 + last_optional_arg) {
		// Cmdline has both cmdfile name and psfile name
		if (!strcmp(argv[last_optional_arg + 1], argv[last_optional_arg + 2]))
			fatal_err("Gri requires Command filename to differ from PostScript filename");
		psname = last_name(argv[last_optional_arg + 2]);
		gr_setup_ps_filename(psname.c_str());
	} else if (separator == 2 + last_optional_arg) {
		// Only have cmdname, so have to make up a psname
		psname = last_name(argv[last_optional_arg + 1]);
		//printf("psname -> '%s'\n", psname.c_str());
		string::size_type slash_location = psname.find_last_of("/");
		if (slash_location != STRING_NPOS)
			psname.STRINGERASE(0, slash_location+1);
		//printf("psname -> '%s'\n", psname.c_str());
		unsigned int l = psname.length();
		if (l > 4
		    && psname[l-1] == 'i'
		    && psname[l-2] == 'r' 
		    && psname[l-3] == 'g'
		    && psname[l-4] == '.') {
			psname.STRINGERASE(l-4,4);
		}
		//printf("psname -> '%s'\n", psname.c_str());
		psname.append(".ps");
		//printf("psname -> '%s'\n", psname.c_str());
		gr_setup_ps_filename(psname.c_str());
	} else {
		// Let Gri make a default name, of the form gr.ps##
		psname = "";
	}
	insert_creator_name_in_PS(argc, argv, psname);
	// Finally, ready to begin plot.
	gr_begin(1);
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
	create_commands(GRI_COMMANDS_FILE);
	if (superuser() & FLAG_AUT2)
		printf(" done\n");
	create_builtin_colors();
	// Do user's ~/.grirc file.
	_store_cmds_in_ps = tmp;
	dogrirc();
#if 0				// 1999-11-27 do instead in doline.cc
	// Show startup msg
	if (_no_startup_message == false && !batch())
		show_startup_msg();
#endif
	// Re-enable tracing
	PUT_VAR("..trace..", trace_old);

	// DataFile stack
	if (superuser() & FLAG_AUT1) {
		printf("DEBUG: %s:%d about to create a new datafile\n",__FILE__,__LINE__);
		printf("DEBUG: cmdfile length is %d\n",_cmdFILE.size());
	}
	DataFile new_data_file;
	if (superuser() & FLAG_AUT1)printf("DEBUG: %s:%d pushing back a datafile at address %x\n",__FILE__,__LINE__,int(&new_data_file));
	_dataFILE.push_back(new_data_file);

	//printf("last optional arg @ %d\n",last_optional_arg);
	//printf("last optional arg is '%s'\n",argv[last_optional_arg]);
	//printf("1+last optional arg is '%s'\n",argv[1+last_optional_arg]);

	// Figure out if there was a cmdfile supplied on options line.
	if (argc < 2 + last_optional_arg) {
		// There was no cmdfile given, so take commands from stdin
		_margin.assign("  ");
		push_cmd_file("stdin", batch() ? false : true, true, "r");
	} else if (argc >= 2 + last_optional_arg
		   || argc >= 3 + last_optional_arg) {
		// Command-file name given, but no postscript-file given
		string fname(argv[1 + last_optional_arg]);
		// Ensure that it's not a 'gre' commandfile
		string::size_type p = fname.rfind(".gre");
		if (fname.size() > 3 && (p == -4 + fname.size())) {
			fprintf(stderr, "Error: gri cannot execute gre files (i.e. files ending in .gre)\n");
			delete_ps_file();
			gri_exit(1);
		}
		// If filename shorter than 4 characters, cannot have .gri suffix,
		// so append it.
		if (fname.size() < 4) {
			fname.append(".gri");
		} else {
			string::size_type p = fname.rfind(".gri");
			if (p != -4 + fname.size())
				fname.append(".gri");
		}
		if (!push_cmd_file(fname.c_str(), false, false, "r")) {
			fprintf(stderr, "Cannot open commandfile `%s'\n", fname.c_str());
			delete_ps_file();
			gri_exit(1);
		}
	} else {
		delete_ps_file();
		fatal_err("Too many items on command line (2 allowed): `\\",
			  argv[1 + last_optional_arg], "', `",
			  argv[2 + last_optional_arg], "', `",
			  argv[3 + last_optional_arg], "' ...", "\\");
		return false;           // never executed
	}
	_first = true;
	_bounding_box.set(0., 0., 0., 0.);
	return true;
}

static void
create_builtin_colors()
{
	GriNamedColor       color;

	color.setNameRGB(		"white",		1.000,	1.000,	1.000);
	colorStack.push_back(color);

	color.setNameRGB(		"LightGray",		0.827,	0.827,	0.827);
	colorStack.push_back(color);

	color.setNameRGB(		"darkslategray",	0.184,	0.310,	0.310);
	colorStack.push_back(color);

	color.setNameRGB(		"black",		0.000,	0.000,	0.000);
	colorStack.push_back(color);

	color.setNameRGB(		"red",			1.000,	0.000,	0.000);
	colorStack.push_back(color);

	color.setNameRGB(		"brown",		0.647,	0.165,	0.165);
	colorStack.push_back(color);

	color.setNameRGB(		"tan",			0.824,	0.706,	0.549);
	colorStack.push_back(color);

	color.setNameRGB(		"orange",		1.000,	0.647,	0.000);
	colorStack.push_back(color);

	color.setNameRGB(		"yellow",		1.000,	1.000,	0.000);
	colorStack.push_back(color);

	color.setNameRGB(		"green",		0.000,	1.000,	0.000);
	colorStack.push_back(color);

	color.setNameRGB(		"ForestGreen",		0.133,	0.545,	0.133);
	colorStack.push_back(color);

	color.setNameRGB(		"cyan",			0.000,	1.000,	1.000);
	colorStack.push_back(color);

	color.setNameRGB(		"blue",			0.000,	0.000,	1.000);
	colorStack.push_back(color);

	color.setNameRGB(		"skyblue",		0.529,	0.808,	0.922);
	colorStack.push_back(color);

	color.setNameRGB(		"magenta",		1.000,	0.000,	1.000);
	colorStack.push_back(color);
}

bool
display_colors()
{
	char msg[200];
	for (unsigned int i = 0; i < colorStack.size(); i++) {
		sprintf(msg, "Color `%s' has RGB = (%f,%f,%f)\n",
			colorStack[i].getName(),
			colorStack[i].getR(),
			colorStack[i].getG(),
			colorStack[i].getB());
		ShowStr(msg);
	}
	return true;
}

// get full filename, searching through directory list in the environment
// variable GRIINPUTS, or, if that environment variable doesn't exist, in the
// symbol GRIINPUTS, defined in defaults.h
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
		//printf("Should system '%s'\n", tmp);
		system(tmp);
#else
		sprintf(tmp, "rm -f %s", gr_currentPSfilename());
		//printf("Should system '%s'\n", tmp);
		system(tmp);
#endif
		gri_exit(1);
		return NULL;		// never done, actually
	}
#endif
}

static void
get_input_simulation(int argc, char *argv[], int separator)
{
	if (separator >= argc)
		return;
	// Save the words following SEPARATOR into the stdin io buffer.
	extern vector<char*> _argv;
	for (int i = separator + 1; i < argc; i++) {
		gr_textsave(argv[i]);
		_argv.push_back(argv[i]);
	}
}
#undef SEPARATOR

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
	_colR.setName("r");
	_colTHETA.setName("theta");
	_colWEIGHT.setName("weight");
	_prompt.assign("gri: ");
	PUT_VAR("..arrowsize..", ARROWSIZE_DEFAULT);
	PUT_VAR("..exit_status..", 0.0);
	_axesStyle = 0;
	_braceLevel = 0;
	_beep = false;
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
	blank_image();
	blank_imageMask();
	_dash.erase(_dash.begin(), _dash.end()); // go to solid
	PUT_VAR("..linewidth..", LINEWIDTH_DEFAULT);
	PUT_VAR("..linewidthaxis..", LINEWIDTHAXIS_DEFAULT);
	PUT_VAR("..linewidthsymbol..", LINEWIDTHSYMBOL_DEFAULT);
	_grid_exists = false;
	_xgrid_exists = false;
	_ygrid_exists = false;
	gr_setmissingvalue(MISSING_VALUE);
	PUT_VAR("..missingvalue..", gr_currentmissingvalue());
	{
		char tmp[100];
		sprintf(tmp, "%f", gr_currentmissingvalue());
		put_syn("\\.missingvalue.", tmp, true);
	}
	_f_min = _f_max = gr_currentmissingvalue();	// no grid data
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
	_yatleft = true;
	PUT_VAR("..ymargin..", YMARGIN_DEFAULT);
	PUT_VAR("..ysize..", YSIZE_DEFAULT);
	PUT_VAR("..ybottom..", 0.0);
	PUT_VAR("..ytop..", YSIZE_DEFAULT);
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
#if defined(VMS) || defined(MSDOS) || defined(IS_DEC_ALPHA)
	time_t          sec;
#else
#if defined(__DECCXX)
	int             sec;
#else
	long            sec;
#endif
#endif
	// Use this for return codes
	if (!put_syn("\\.return_value.", "", true)) OUT_OF_MEMORY;
	// \.awk. (name of awk program)
	if (!put_syn("\\.awk.", AWK, true)) OUT_OF_MEMORY;
	// \.version. (version number)
	if (!put_syn("\\.version.", _gri_number, true)) OUT_OF_MEMORY;
	//
	// \.pid. (process ID)
	sprintf(_grTempString, "%d", int(getpid()));
	if (!put_syn("\\.pid.", _grTempString, true)) OUT_OF_MEMORY;
	// \.wd. (working directory)
	wd = pwd();
	if (!put_syn("\\.wd.", wd, true)) OUT_OF_MEMORY;
	// \.time. (time)
	time(&sec);
	strcpy(_grTempString, asctime(localtime(&sec)));
	_grTempString[strlen(_grTempString) - 1] = '\0';
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

// return value: number of optional arguments
static int
interpret_optional_arguments(int argc, char *argv[])
{
	extern char     _gri_number[];
	int             number_optional_arg = 0;
	_lib_directory.assign(DEFAULT_GRI_DIR);
	// Interpret optional [-] arguments if they exist.
	if (argc > 1) {
		int             i;
		for (i = 1; i < argc; i++) {
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
			} else if (!strcmp(argv[i], "-repair")) {
				// Repair a postscript file made by a gri job that died
				// midway, so didn't finish off some "paths" and didn't
				// showpage.
				// ??BUG?? Assumes that no words follow "stroke", etc
				FILE           *fpin, *fpout;
				int             newpaths = 0;
				if (i != argc - 3)
					fatal_err("`gri -repair' needs 2 filenames");
				if (NULL == (fpin = fopen(argv[i + 1], "r")))
					fatal_err("`gri -repair' cannot open file `\\",
						  argv[i + 1], "'", "\\");
				if (NULL == (fpout = fopen(argv[i + 2], "w")))
					fatal_err("`gri -repair' cannot open file `\\",
						  argv[i + 2], "'", "\\");
				while (NULL != fgets(_cmdLine, LineLength_1, fpin)) {
					if (!strcmp(_cmdLine, "n\n")
					    || !strcmp(_cmdLine, "newpath\n")) {
						printf("+");
						newpaths++;
					} else if (!strcmp(_cmdLine, "s\n")	// stroke
						   ||!strcmp(_cmdLine, "S\n")	// stroke
						   ||!strcmp(_cmdLine, "stroke\n")
						   || !strcmp(_cmdLine, "c\n")	// closepath
						   ||!strcmp(_cmdLine, "h\n")	// closepath
						   ||!strcmp(_cmdLine, "closepath\n")) {
						printf("-");
						newpaths--;
					}
					fputs(_cmdLine, fpout);
				}
				if (newpaths) {
					for (i = 0; i < newpaths; i++) {
						fprintf(fpout,
							"stroke  %% inerted by `gri -repair %s %s'\n",
							argv[2], argv[3]);
					}
					fprintf(fpout,
						"showpage %% inerted by `gri -repair %s %s'\n",
						argv[2], argv[3]);
					fprintf(fpout, "%%Trailer\n");
					fprintf(fpout, "%%DocumentFonts: Times-Roman Courier Helvetica Symbol Palatino-Roman Palatino-Italic\n");
					fprintf(fpout, "%%Pages: 1\n");
					fprintf(fpout, "%%BoundingBox: 0 0 612 792\n");
				}
				fclose(fpin);
				fclose(fpout);
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
				} else if (!strcmp(argv[i], "-warn_offpage")){
					_warn_offpage = true;
				} else if (!strcmp(argv[i], "-nowarn_offpage")){
					_warn_offpage = false;
				} else if (!strcmp(argv[i], "-directory")) {
					number_optional_arg++;
					i++;
					if (i < argc - 1)
						_lib_directory.assign(argv[i]);
					else {
						err("`-directory' needs an argument.");
						gri_exit(1);
					}
				} else if (!strcmp(argv[i], "-directory_default")) {
					gr_textput(_lib_directory.c_str());
					gr_textput("\n");
					gri_exit(0);
				} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help")) {
					give_help();
					gri_exit(0);
				} else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "-publication")) {
					PUT_VAR("..publication..", 1.0);

				} else if (!strncmp(argv[i], "-c", 2)) {
					if (1 == sscanf(argv[i], "-c%d", &val)) {
						_chatty = val;
					} else {
						_chatty = 1;
					}
				} else if (!strcmp(argv[i], "-chatty")) {
					if (1 == sscanf(argv[i], "-chatty%d", &val)) {
						_chatty = val;
					} else {
						_chatty = 1;
					}
				} else if (!strcmp(argv[i], "-no_expecting")) {
					warning("Gri no longer demands to find an `expecting' command, so the `-no_expecting' option can be dropped.");
				} else if (!strncmp(argv[i], "-s", 2)) {
					if (1 == sscanf(argv[i], "-s%d", &val)) {
						PUT_VAR("..superuser..", ((double) val));
					} else {
						PUT_VAR("..superuser..", ((double) 1.0));
					}
				} else if (!strncmp(argv[i], "-superuser", 10)) {
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
				break;
			}
		}
	}
	put_syn("\\.lib_dir.", _lib_directory.c_str(), true);
	return number_optional_arg;
}

void
give_help()
{
	gr_textput("NAME\n");
	gr_textput("     gri - draw scientific graphs\n");
	gr_textput("\n");
	gr_textput("SYNOPSIS\n");
	gr_textput("     gri [OPTIONS] [command_file [postscript_file]]\n");
	gr_textput("\n");
	gr_textput("DESCRIPTION\n");
	gr_textput("     If a command file (command_file) is named, commands are read from that file;\n");
	gr_textput("     otherwise they are read from the keyboard.  If a command_file is named, then\n");
	gr_textput("     a file in which to store the PostScript output may also be named; otherwise\n");
	gr_textput("     it is stored in a file named gr-00.ps (or gr-01.ps if gr-00.ps exists, etc).\n");
	gr_textput("\n");
	gr_textput("     There are 3 special forms that do no graphing:\n");
	gr_textput("       `gri -creator postscript_file'\n");
	gr_textput("         Extracts the Gri commands that created the Gri PostScript file.\n");
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
	gr_textput("     -nowarn_offpage\n");
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
	gr_textput("     -publication or -p\n");
	gr_textput("             Sets the builtin variable ..publication.. to 1; normally it is 0.\n");
	gr_textput("             One might use if statements (`if !..publication..' ...) on drafts.\n");
	gr_textput("     -superuser or -s\n");
	gr_textput("	     Used only by Gri programmers (who can check the value with the\n");
	gr_textput("	     C function `superuser()'.)  An optional value can be supplied\n");
	gr_textput("	     without spaces (e.g. `-s2') to set the debugging level.\n");
	gr_textput("             Flags are listed below; add flags to get several actions at once\n");
	gr_textput("	       1: print cmdline before/after substituting synonyms\n");
	gr_textput("	       2: print cmdline before/after substituting rpn expressions\n");
	gr_textput("	       4: print new commands being defined\n");
	gr_textput("	       8: print system commands and `open \"... | \"' commands before\n");
	gr_textput("	          they are passed to the system\n");
	gr_textput("	     128: for author's use only\n");
	gr_textput("	     256: for author's use only\n");
	gr_textput("	      Note that all flags are equal\n");
	gr_textput("	      to 2 raised to an integer power.  Since the flag values are\n");
	gr_textput("	      detected by a bitwise OR, you can combine flags by adding; thus\n");
	gr_textput("	      specifying a flag of 5 yields flags 1 and 4 together; specifying\n");
	gr_textput("	      15 yields flags 1, 2, 4 and 8.\n");
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
insert_creator_name_in_PS(int argc, char *argv[], const string& psname)
{
	extern char _gri_release_time[];
	extern char _gri_number[]; // see version.c
	gr_setup_creatorname("Gri");
	gr_setup_creatorname(_gri_number);
	gr_setup_creatorname(" (released ");
	gr_setup_creatorname(_gri_release_time);
	gr_setup_creatorname(").  User=");
	if (egetenv("USER"))
		gr_setup_creatorname(egetenv("USER"));
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
		gr_setup_creatorname(argv[last_optional_arg + 1]);
		Require2(put_syn("\\.ps_file.", psname.c_str(), true), OUT_OF_MEMORY);
	} else {
		// No commandfile supplied on cmdline.
		Require2(put_syn("\\.command_file.", "stdin", true), OUT_OF_MEMORY);
		Require2(put_syn("\\.readfrom_file.", "stdin", true), OUT_OF_MEMORY);
		Require2(put_syn("\\.ps_file.", "gri-??.ps", true), OUT_OF_MEMORY);
	}
}

static void
dogrirc()
{
	char            grircname[100];
	_done = 0;
	_margin.assign("| ");
#if defined(HAVE_GETENV)
	char *            home = egetenv("HOME");
	if (!home) {
		warning("Cannot getenv(HOME), so cannot perform grirc file");
		return;
	}
	sprintf(grircname, "%s/%s", home, GRIRC_FILE);
#else
	strcpy(grircname, GRIRC_FILE);
#endif
	if (push_cmd_file(grircname, false, false, "r")) {
		gr_comment("gri:// BEGIN (~/.grirc)\n");
		while (do_command_line()){
			;
		}
		_done = 0;
		gr_comment("gri:// END   (~/.grirc)\n");
	}
}

#if 0
static void
show_startup_msg()
{
	string fullfilename(_lib_directory.c_str());
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
