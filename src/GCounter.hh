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

// Keep track of references to object
#if !defined(_GriCounter_h_)
#define  _GriCounter_h_
class GriCounter
{
public:	
	GriCounter()                    { count = 0;		}
	GriCounter(const GriCounter& c)	{ count = c.getCount(); }
	~GriCounter()			{ ;			}
	void incrementCount()		{ count++;		}
	void decrementCount()		{ if (count) count--;	}
	unsigned getCount() const	{ return count;		}
private:
	int count;
};
#endif
