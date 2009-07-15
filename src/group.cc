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

#include	"extern.hh"
extern FILE *_grSVG;
static std::vector<std::string> group_name;

bool
groupCmd()
{
	printf("DEBUG: in 'group'\n");
	return group_start();
}
bool
end_groupCmd()
{
	printf("DEBUG: in 'end group'\n");
	return group_end();
}
bool
group_start(const char *id)
{
	if (_output_file_type == svg) {
		std::string name = id;
		if (strlen(id) > 0)
			fprintf(_grSVG, "<g> <!-- %s -->\n", id);
		else
			fprintf(_grSVG, "<g> <!-- anonymous group -->\n");
		group_name.push_back(name);
	}
	return true;
}
bool
group_end()
{
	if (_output_file_type == svg) {
		if (group_name.size() > 0) {
			fprintf(_grSVG, "</g> <!-- end of %s -->\n", group_name.back().c_str());
			group_name.pop_back();
		} else {
			fprintf(_grSVG, "</g> <!-- end of anonymous group -->\n");
		}
	}
	return true;
}
