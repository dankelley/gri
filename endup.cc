#include <string>
#include "gr.hh"
#include "extern.hh"
#include "private.hh"
#include "superus.hh"


void
end_up()
{
#if 0				// inaccurate anyhow!
	if (_chatty > 0) {
		display_unused_var();
		display_unused_syn();
	}
#endif
	gr_set_clip_ps_off();
	close_data_files();
	gr_end("!");
}
