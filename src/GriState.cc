// #define DASH

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
	if (n.color_text().get_type() == GriColor::rgb)
		colorText.setRGB((n.color_text()).getR(),
				 (n.color_text()).getG(),
				 (n.color_text()).getB());
	else
		colorText.setHSV((n.color_text()).getH(),
				 (n.color_text()).getS(),
				 (n.color_text()).getV());
	the_font.id          = (n.font()).id;
	the_font.size_pt     = (n.font()).size_pt;
	the_line_cap         = n.line_cap();
	the_line_join        = n.line_join();
	the_linewidth_axis   = n.linewidth_axis();
	the_linewidth_line   = n.linewidth_line();
	the_linewidth_symbol = n.linewidth_symbol();
	the_superuser        = n.superuser();
	the_trace            = n.trace();
#ifdef DASH
	the_dash.erase();
	for (unsigned int i = 0; i < (n.dash).size(); i++)
		the_dash.push_back(n.dash[i]);
#endif
}
GriState& GriState::operator=(const GriState& n)
{
	if (n.color_line().get_type() == GriColor::rgb)
		colorLine.setRGB((n.color_line()).getR(),
				 (n.color_line()).getG(),
				 (n.color_line()).getB());
	else 
		colorLine.setHSV((n.color_line()).getH(),
				 (n.color_line()).getS(),
				 (n.color_line()).getV());
	if (n.color_text().get_type() == GriColor::rgb)
		colorText.setRGB((n.color_text()).getR(),
				 (n.color_text()).getG(),
				 (n.color_text()).getB());
	else
		colorText.setHSV((n.color_text()).getH(),
				 (n.color_text()).getS(),
				 (n.color_text()).getV());
	the_font.id          = (n.font()).id;
	the_font.size_pt     = (n.font()).size_pt;
	the_line_cap         = n.line_cap();
	the_line_join        = n.line_join();
	the_linewidth_axis   = n.linewidth_axis();
	the_linewidth_line   = n.linewidth_line();
	the_linewidth_symbol = n.linewidth_symbol();
	the_superuser        = n.superuser();
	the_trace            = n.trace();
#ifdef DASH
	the_dash.erase();
	for (unsigned int i = 0; i < (n.dash).size(); i++)
		the_dash.push_back(n.dash[i]);
#endif
	return *this;
}

GriState::~GriState()
{
	colorText.~GriColor();
	colorLine.~GriColor();
}

void GriState::set_color_line(const GriColor& c)
{
	colorLine = c;
}

void GriState::set_color_text(const GriColor& c)
{
	colorText = c;
}
