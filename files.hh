// define some filenames
#if !defined(_files_hh_)

#define _files_hh_

#if !defined(HAVE_TMPNAM)
#define GRI_TMP_FILE                    "TMP.GRI"
#endif

#if defined(VMS)
#define GRIRC_FILE              	"GRI$MACROS:GRI.RC"
#define GRI_COMMANDS_FILE       	"GRI.CMD"
#else
#define GRIRC_FILE              	".grirc"
#define GRI_COMMANDS_FILE       	"gri.cmd"
#endif
#if defined(HAVE_LIBNETCDF)
#include        <netcdf.h>
#endif

#endif				// _files_hh_
