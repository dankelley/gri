#include <string>
#include "gr.hh"
#include "extern.hh"
#include "private.hh"
#include "superus.hh"


void
end_up()
{
	extern bool postscript_clipping_on;
	if (_chatty > 0) {
		display_unused_var();
		display_unused_syn();
	}
	extern FILE *_grPS;
	if (postscript_clipping_on)
		fprintf(_grPS, "Q %% turn clipping off (user forgot to)\n");
	if (((unsigned) superuser()) & FLAG_AUT1)printf("IN end_up, about to close...\n");
	close_data_files();
	if (((unsigned) superuser()) & FLAG_AUT1)printf("... IN end_up, close went OK.  Next gr_end.\n");
	gr_end("!");
	if (((unsigned) superuser()) & FLAG_AUT1)printf("gr_end was ok\n");
}
