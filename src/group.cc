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
