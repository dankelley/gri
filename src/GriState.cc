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

//#define DEBUG_DASH 1
#include <string>
#include "GriColor.hh"
#include "GriState.hh"
#include "gr.hh"
#include "types.hh"
#include "errors.hh"
#include "defaults.hh"

GriState::GriState()
{
	the_line_cap           = 0;
	the_line_join          = 1;
	the_linewidth_axis     = LINEWIDTHAXIS_DEFAULT;
	the_linewidth_line     = LINEWIDTH_DEFAULT;
	the_linewidth_symbol   = LINEWIDTHSYMBOL_DEFAULT;
	the_font.size_pt       = FONTSIZE_PT_DEFAULT;
	the_font.id            = FONT_DEFAULT;
	is_separate_text_color = false;
	the_trace              = false;
	the_superuser          = 0;
	colorLine.setRGB(0.0, 0.0, 0.0);
	colorText.setRGB(0.0, 0.0, 0.0);
}

GriState::GriState(const GriState& n)
{
	if ((n.color_line()).get_type() == GriColor::rgb)
		colorLine.setRGB((n.color_line()).getR(),
				 (n.color_line()).getG(),
				 (n.color_line()).getB());
	else 
		colorLine.setHSV((n.color_line()).getH(),
				 (n.color_line()).getS(),
				 (n.color_line()).getV());
	colorLine.setT(n.color_line().getT());
	if (n.color_text().get_type() == GriColor::rgb)
		colorText.setRGB((n.color_text()).getR(),
				 (n.color_text()).getG(),
				 (n.color_text()).getB());
	else
		colorText.setHSV((n.color_text()).getH(),
				 (n.color_text()).getS(),
				 (n.color_text()).getV());
	colorText.setT(n.color_text().getT());
	the_font.id          = (n.font()).id;
	the_font.size_pt     = (n.font()).size_pt;
	the_line_cap         = n.line_cap();
	the_line_join        = n.line_join();
	the_linewidth_axis   = n.linewidth_axis();
	the_linewidth_line   = n.linewidth_line();
	the_linewidth_symbol = n.linewidth_symbol();
	the_superuser        = n.superuser();
	the_trace            = n.trace();
	the_dash.erase(the_dash.begin(), the_dash.end());
	for (unsigned int i = 0; i < (n.the_dash).size(); i++)
		the_dash.push_back(n.the_dash[i]);
#ifdef DEBUG_DASH
	printf("GriState COPY CONSTRUCTOR LEAVING dash: ");
	for (unsigned int i = 0; i < (n.the_dash).size(); i++)
		printf(" %f ", the_dash[i]);
	printf("\n");
#endif

}
GriState& GriState::operator=(const GriState& n)
{
	if (n.color_line().get_type() == GriColor::rgb) {
		colorLine.setRGB((n.color_line()).getR(),
				 (n.color_line()).getG(),
				 (n.color_line()).getB());
	} else {
		colorLine.setHSV((n.color_line()).getH(),
				 (n.color_line()).getS(),
				 (n.color_line()).getV());
	}
	colorLine.setT(n.color_line().getT());
	if (n.color_text().get_type() == GriColor::rgb) {
		colorText.setRGB((n.color_text()).getR(),
				 (n.color_text()).getG(),
				 (n.color_text()).getB());
	} else {
		colorText.setHSV((n.color_text()).getH(),
				 (n.color_text()).getS(),
				 (n.color_text()).getV());
	}
	colorText.setT(n.color_text().getT());
	the_font.id          = (n.font()).id;
	the_font.size_pt     = (n.font()).size_pt;
	the_line_cap         = n.line_cap();
	the_line_join        = n.line_join();
	the_linewidth_axis   = n.linewidth_axis();
	the_linewidth_line   = n.linewidth_line();
	the_linewidth_symbol = n.linewidth_symbol();
	the_superuser        = n.superuser();
	the_trace            = n.trace();
	the_dash.erase(the_dash.begin(), the_dash.end());
//	for (unsigned int i = 0; i < (n.the_dash).size(); i++)
//		the_dash.push_back(n.the_dash[i]);
	std::vector<double> n_dash(n.dash());
	for (unsigned int i = 0; i < n_dash.size(); i++)
		the_dash.push_back(n_dash[i]);
#ifdef DEBUG_DASH
	printf("GriState ASSIGNMENT CONSTRUCTOR LEAVING dash: ");
	for (unsigned int i = 0; i < (n.the_dash).size(); i++) 
		printf(" %f ", the_dash[i]); 
	printf("\n");
#endif
	return *this;
}

GriState::~GriState()
{
	colorText.~GriColor(); // BUG: should I be calling the destructor??
	colorLine.~GriColor(); // BUG: should I be calling the destructor??
}
