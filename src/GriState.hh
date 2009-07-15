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

#define DASH

#if !defined(_GriState_h_)
#define _GriState_h_

//#define DEBUG_GRISTATE // Debug

#include <stdio.h>
#include <vector>		// part of STL
#include "gr.hh"
#include "GriColor.hh"

// The graphics state.  Important global object; see also state.cc.
class GriState
{
public:
	GriState();
	GriState(const GriState& n);
	GriState& operator=(const GriState& n);
	~GriState();
	// 
	// Members to set values...
	void set_color_line(const GriColor& c)  {colorLine = c;			}
	void set_color_text(const GriColor& c)  {colorText = c;			}
#ifdef DASH
	void set_dash(std::vector<double> d)     {the_dash = d;			}
#endif
	void set_transparency_line(double tr)   {colorLine.setT(tr);		}
	void set_transparency_text(double tr)   {colorText.setT(tr);		}
	void set_separate_text_color(bool f)	{is_separate_text_color = f;    }
	void set_fontsize(double fs)		{the_font.size_pt     = fs;	}
	void set_font(gr_font f)		{the_font             = f;	}
	void set_line_cap(int c)		{the_line_cap         = c;      }
	void set_line_join(int j)		{the_line_join        = j;      }
	void set_linewidth_axis(double w)	{the_linewidth_axis   = w;	}
	void set_linewidth_line(double w)	{the_linewidth_line   = w;	}
	void set_linewidth_symbol(double w)	{the_linewidth_symbol = w;	}
	void set_superuser(unsigned int s)	{the_superuser	      = s;	}
	void set_trace(bool t)			{the_trace	      = t;	}
	// 
	// Members to get values...
	const GriColor& color_line() const 	        {return colorLine;		}
	const GriColor& color_text() const	        {return colorText;		}
	bool	        separate_text_color() const	{return is_separate_text_color; }
	gr_font	        font() const			{return the_font;		}
	int             line_cap() const		{return the_line_cap;           }
	int             line_join() const		{return the_line_join;          }
	double	        linewidth_axis() const		{return the_linewidth_axis;	}
	double          linewidth_line() const		{return the_linewidth_line;	}
	double	        linewidth_symbol() const	{return the_linewidth_symbol;	}
	unsigned int    superuser() const		{return the_superuser;		}
	bool	        trace() const			{return the_trace;		}
#ifdef DASH
	const std::vector<double> dash() const   {
		//printf("DEBUG: GriState returning dash of length %d\n",the_dash.size());
		return the_dash;}
#endif
private:
	double		the_linewidth_axis;
	double		the_linewidth_line;
	double		the_linewidth_symbol;
	int		the_line_cap;
	int		the_line_join;
	gr_font		the_font;
	GriColor	colorLine;
	GriColor	colorText;
	bool		is_separate_text_color;
	bool		the_trace;
	unsigned int	the_superuser;
#ifdef DASH
	std::vector<double> the_dash;
#endif
	// FOR MORE, SEE extern.hh:21
};

#endif
