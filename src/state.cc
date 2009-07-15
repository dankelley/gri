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

// #define DEBUG_STATE
#include	<math.h>
#include	"gr.hh"
#include        "extern.hh"
#include        "GriState.hh"


bool state_save();
bool state_restore();
bool state_display();

static std::vector<GriState> stateStack;


bool
stateCmd()
{
	switch (_nword) {
	case 2:
		if (word_is(1, "save"))
			return state_save();
		else if (word_is(1, "restore"))
			return state_restore();
		else if (word_is(1, "display"))
			return state_display();
		else {
			demonstrate_command_usage();
			err("Second word must be `save', `restore', or `display'");
			return false;
		}
		// NOT REACHED
	default:
		demonstrate_command_usage();
		NUMBER_WORDS_ERROR;
		return false;
	}
}


bool
state_save()
{
	//printf("state_save() before push, the dash is size %d\n",_dash.size());
	stateStack.push_back(_griState);
	return true;
}

bool
state_restore()
{
	if (stateStack.size() < 1) {
		warning("Ignoring `state restore' because no `state save' done yet");
		return true;
	}
	if (stateStack.size() > 0) {
		_griState = stateStack.back();
		stateStack.pop_back();
	}
	// Set these since used globally (see e.g. draw_axes)
	gr_setfontsize_pt(_griState.font().size_pt);
	gr_setfont(_griState.font().id);
	_dash = _griState.dash();
	gr_set_dash(_dash);
#ifdef DEBUG_STATE
	printf("restored dash ... size %d\n",_dash.size());
#endif
	PUT_VAR("..fontsize..", _griState.font().size_pt);
	return true;
}

bool
state_display()
{
	int d = stateStack.size();
	if (d < 1) {
		warning("No `state' stack exists yet.  Do `state save' first.");
		return true;
	}
	char msg[200];
	for (int i = d - 1; i >= 0; i--) {
		if (i == d - 1)
			sprintf(msg, "State at top of stack (most recent):\n");
		else
			sprintf(msg, "State at distance %d from top of stack:\n", d - i);
		ShowStr(msg);
		sprintf(msg, "  line width (curve)  = %f pt\n", stateStack[i].linewidth_line());
		ShowStr(msg);
		sprintf(msg, "  line width (axis)   = %f pt\n", stateStack[i].linewidth_axis());
		ShowStr(msg);
		sprintf(msg, "  line width (symbol) = %f pt\n", stateStack[i].linewidth_symbol());
		ShowStr(msg);
		if ((stateStack[i].color_text()).isRGB() == true) {
			sprintf(msg, "  color (font) = (%f,%f,%f) rgb\n",
				(stateStack[i].color_text()).getR(),
				(stateStack[i].color_text()).getG(),
				(stateStack[i].color_text()).getB());
		} else {
			sprintf(msg, "  color (font) = (%f,%f,%f) hsb\n",
				(stateStack[i].color_text()).getH(),
				(stateStack[i].color_text()).getS(),
				(stateStack[i].color_text()).getV());
		}
		ShowStr(msg);
		if ((stateStack[i].color_line()).isRGB() == true) {
			sprintf(msg, "  color (font) = (%f,%f,%f) rgb\n",
				(stateStack[i].color_line()).getR(),
				(stateStack[i].color_line()).getG(),
				(stateStack[i].color_line()).getB());
		} else {
			sprintf(msg, "  color (font) = (%f,%f,%f) hsb\n",
				(stateStack[i].color_line()).getH(),
				(stateStack[i].color_line()).getS(),
				(stateStack[i].color_line()).getV());
		}
		ShowStr(msg);
	}
	return true;
}
