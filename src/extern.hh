#if !defined(_extern_h_)
#define _extern_h_

#include <string>
#include <vector>		// part of STL

#if defined(IS_MINGW32)
#include <process.h>
#endif

#include "private.hh"
#include "gr.hh"
#include "errors.hh"
#include "gr_coll.hh"
#include "GMatrix.hh"
#include "GriState.hh"
#include "Variable.hh"
#include "Synonym.hh"


#include "macro.hh"
#define AXES_RECTANGULAR	0	// normal axes style (rect)

using namespace std;		// needed for g++-3 
extern GriState _griState;	// <-> gri.cc

// The following globals have symbolic names associated with them, and
// MUST be updated whenever these names are assigned to.  See the note in
// put_var() in variable.c.  The reason for the parallel C storage is that
// the following are accessed for every data point plotted.  Certain other
// symbolic variables (like  ..publication.. for example) are not accessed
// frequently, and hence have no parallel C storage as the following do. Thus
// they are safe against breakage.

extern output_file_type _output_file_type;
extern bool      _user_gave_bounding_box;
extern rectangle _page_box;
extern rectangle _bounding_box_user;
extern rectangle _bounding_box;
extern bool     _user_set_x_axis;
extern bool     _user_set_y_axis;
extern double   _xleft;		// ..xleft..
extern double   _xright;	// ..xright..
extern double   _ybottom;	// ..ybottom..
extern double   _ytop;		// ..ytop..
// Column data
extern GriColumn _colU;
extern GriColumn _colV;
extern GriColumn _colX;
extern GriColumn _colY;
extern GriColumn _colZ;
extern GriColumn _colWEIGHT;
extern bool     _uscale_exists;
extern bool     _vscale_exists;
extern bool     _columns_exist;
extern std::string   _xFmt;
extern std::string   _yFmt;
extern double _clipxleft, _clipxright, _clipybottom, _clipytop;
extern double _clip_ps_xleft, _clip_ps_xright, _clip_ps_ybottom, _clip_ps_ytop;
extern bool   _clipping_postscript, _clipping_is_postscript_rect;
// Axes
extern double   _cm_per_u;
extern double   _cm_per_v;
extern gr_axis_properties _xtype;
extern double   _xinc;
extern bool     _xscale_exists;
extern bool     _need_x_axis;
extern bool     _xatbottom;
extern bool     _xincreasing;
extern int      _xsubdiv;
extern vector<string> _x_labels;
extern vector<double> _x_label_positions;
extern gr_axis_properties _ytype;
extern double   _yinc;
extern bool     _yscale_exists;
extern bool     _need_y_axis;
extern bool     _yatleft;
extern bool     _yincreasing;
extern int      _ysubdiv;
extern vector<string> _y_labels;
extern vector<double> _y_label_positions;

// Data/Command file stacks
extern std::vector<DataFile> _dataFILE;
extern std::vector<CmdFile>  _cmdFILE;

// Pointers to variables and synonyms, for & syntax
extern std::vector<int> variablePointer;
extern std::vector<GriVariable> variableStack;
extern std::vector<int> synonymPointer;
extern std::vector<GriSynonym> synonymStack;

// Grid data
extern GriMatrix<double> _f_xy;
extern double     _f_min, _f_max, *_xmatrix, *_ymatrix;
extern GriMatrix<bool> _legit_xy;
extern bool     _xgrid_exists;
extern bool     _xgrid_increasing;
extern bool     _ygrid_exists;
extern bool     _ygrid_increasing;
extern bool     _grid_exists;
// Commands
extern char    *_cmd_being_done_IP[];
extern int      _cmd_being_done_code[];
extern int      _cmd_being_done;
extern char    *_cmdLine;
extern char    *_cmdLineCOPY;
extern char    *_word[];
extern char    *_Words2[];
extern char    *_Words3[];
// Misc
extern std::string   _contourFmt;
extern std::string   _current_directory;
extern char    *_errorMsg;
extern int      _error_action;
extern std::string   _lib_directory;
extern std::string   _margin;
extern std::string   _prompt;
extern double  *_dstack;
extern double   _gri_eof;
extern double   _top_of_plot;
extern double   _axes_offset;
extern int      _axesStyle;
extern bool     _gri_beep;
extern int      _braceLevel;
extern int      _chatty;
extern int      _clipData;
extern int      _debugFlag;
extern int      _done;
extern bool     _drawingstarted;
extern bool     _error_in_cmd;
extern int      _exit_value;
extern bool     _first;
extern gr_font  _font;
extern bool     _ignore_eof;
extern bool     _ignore_error;

extern int      _arrow_type;
extern bool     _warn_offpage;

extern std::vector<double> _dash;

// Numbers of things.
extern int      _num_command;
extern unsigned int _num_xmatrix_data;
extern unsigned int _num_ymatrix_data;
extern unsigned int _nword;

// Version number.
extern double   _version; // this version
extern double   _version_expected; // expected version (if any)


// Flags indicating whether things exist yet.
extern bool     _use_default_for_query;


// Command stack
#define	COMMAND_STACK_SIZE	1000
typedef struct {
	char           *syntax;	// The 'name' of command
	char           *help;	// Help, if any
	char           *procedure;	// Commands to do
	char           *filename;	// Where defined
	int             fileline;	// Where defined
}               GRI_COMMAND;
extern GRI_COMMAND _command[COMMAND_STACK_SIZE];

extern int _function_indent;

#endif				// _extern_h_
