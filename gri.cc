#include	<string>
#include	<stdio.h>
#include        "gr.hh"
#include	"defaults.hh"
#include	"private.hh"
#include	"types.hh"
#include        "gr_coll.hh"
#include        "GMatrix.hh"
#include        "GriState.hh"
#include        "Synonym.hh"
#include        "Variable.hh"

using namespace std;		// needed for g++-3 
char source_indicator[256];	// BUG: length not checked

// Globals from gr.
extern char     _grTempString[]; /* String available to all code. */
/*
 * The following globals have symbolic names associated with
 * them, and MUST be updated whenever these names are assigned to.  See
 * the note in put_var() in variable.c.  The reason for the parallel C
 * storage is that the following are accessed for every data point
 * plotted.  Certain other symbolic variables (like ..publication.. for
 * example) are not accessed frequently, and hence have no parallel C
 * storage as the following do.  Thus they are safe against breakage.
 */
output_file_type _output_file_type;
GriState        _griState;	// present state
bool            _user_set_x_axis = false;
bool            _user_set_y_axis = false;
double          _xleft;		/* ..xleft.. */
double          _xright;	/* ..xright.. */
double          _ybottom;	/* ..ybottom.. */
double          _ytop;		/* ..ytop.. */
// Globals used elsewhere (variables begin with _).
int             _arrow_type;
bool            _warn_offpage;
std::vector<double>       _dash;
std::vector<DataFile>     _dataFILE;
std::vector<CmdFile>      _cmdFILE;
std::vector<const char*>  _gri_argv;


char            _input_data_separator; // ' ' (general whitespace) or '\t'
char *          _cmd_being_done_IP[cmd_being_done_LEN];
int             _cmd_being_done_code[cmd_being_done_LEN];
int             _cmd_being_done = 0;
char *          _cmdLine;
char *          _cmdLineCOPY;
std::string     _contourFmt;
std::string     _current_directory;
int             _error_action = 0; // 0=message/exit(1), 1=message/dump core/exit(1)
char *          _errorMsg;
std::string     _lib_directory;
std::string     _margin;
std::string     _prompt;
char           *_word[MAX_nword];
char           *_Words2[MAX_nword];
char           *_Words3[MAX_nword];
std::string     _xFmt;
std::string     _yFmt;
double          _clipxleft, _clipxright, _clipybottom, _clipytop;
double          _cm_per_u, _cm_per_v;
double         *_dstack;
double          _gri_eof = 0.0;
double          _top_of_plot;
GriMatrix<double>  _f_xy;
double          _f_min, _f_max, *_xmatrix, *_ymatrix;
double          _xinc;
double          _yinc;
double          _zinc;
double          _axes_offset = 0.0;
int             _axesStyle;
bool            _gri_beep = false;
bool            _user_gave_bounding_box = false;
rectangle       _page_size;
rectangle       _bounding_box_user;
rectangle       _bounding_box;	// in cm on page
int             _braceLevel = 0;
int             _chatty;
int             _clipData = 0;
int             _debugFlag = 0;		      // used in debugging
int             _done = 0;		      // 1=`quit'; 2=`return'
bool            _drawingstarted = false;
bool            _error_in_cmd;
int             _exit_value = 0;
gr_font         _font;
bool            _first;
bool            _ignore_eof = false;
bool            _ignore_error;
GriMatrix<bool>  _legit_xy;
bool            _need_x_axis = true;
bool            _need_y_axis = true;
unsigned int    _nword;
bool            _xatbottom;
bool            _xincreasing = true;
int             _xsubdiv = 1;
vector<double>  _x_label_positions;
vector<string>  _x_labels;
gr_axis_properties _xtype;
char            _xtype_map = ' '; // could be "EWNS "
gr_axis_properties _ytype;
char            _ytype_map = ' '; // could be "EWNS "
bool            _yatleft;
bool            _yincreasing = true;
int             _ysubdiv = 1;
vector<string>  _y_labels;
vector<double>  _y_label_positions;
/* numbers of items existing */
int             _num_command = 0;	/* commands */
unsigned int    _num_xmatrix_data;
unsigned int    _num_ymatrix_data;
/* Version number */
double          _version;	/* this version */
double          _version_expected = 0.0;	/* expected version (if any) */
/* flags for whether things exist yet (set in  */
bool            _columns_exist;
bool            _grid_exists;
bool            _uscale_exists;
bool            _vscale_exists;
bool            _xgrid_exists;
bool            _xgrid_increasing;
bool            _xscale_exists;
bool            _ygrid_exists;
bool            _ygrid_increasing;
bool            _yscale_exists;
bool            _use_default_for_query = false;

/*
 * Matrices
 */
GriColumn _colU;
GriColumn _colV;
GriColumn _colX;
GriColumn _colY; 
GriColumn _colZ; 
GriColumn _colWEIGHT;

// Command stack
#define	COMMAND_STACK_SIZE	1000
typedef struct {
	char           *syntax;	        // The 'name' of command
	char           *help;           // Help, if any
	char           *procedure;	// Commands to do
	char           *filename;	// Where defined
	int             fileline;	// Where defined
}               GRI_COMMAND;
GRI_COMMAND     _command[COMMAND_STACK_SIZE];

int _function_indent = 4;



int
main(int argc, char **argv)
{
	start_up(argc, argv);
	do {
		do_command_line();
	} while (!_done);
	warning("\\\\");		// notify of pending repeated warnings
	end_up();
	return _exit_value;
}
