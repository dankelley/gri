#include <string>
#include <stdio.h>
main()
{
    string l("\"hi dan \"hi");
    printf("'%s'\n", l.c_str());
    if (l[0] == '"')		l.remove(0,1);
    if (l[l.size() - 1] == '"')	l.remove(l.size()-1,1);
    printf("-> '%s'\n", l.c_str());
}
