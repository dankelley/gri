## Process this file with automake to produce Makefile.in
# gri: doc/resources/Makefile.am
     srcdir = @srcdir@
     VPATH = @srcdir@
EXTRA_DIST = top_banner.gif bottom_banner.gif math_symbols.gif\
 pixel.gif sm_bg.gif symbols.gif top_banner.gri bottom_banner.gri

developer_all:
	gri bottom_banner.gri
	convert bottom_banner.ps bottom_banner.gif
	chmod a+r bottom_banner.gif
	rm -f bottom_banner.ps
	gri top_banner.gri
	convert top_banner.ps top_banner.gif
	chmod a+r top_banner.gif
	rm -f top_banner.ps
