#include	"extern.hh"
extern FILE *_grSVG;


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
		if (strlen(id) > 0)
			fprintf(_grSVG, "<g id=\"%s\">\n", id);
		else
			fprintf(_grSVG, "<g>\n");
	}
	return true;
}
bool
group_end()
{
	if (_output_file_type == svg) {
		fprintf(_grSVG, "</g>\n");
	}
	return true;
}
