all:
	gri logo
	mv logo.ps logo.eps
	perl ../gri2html logo

	gri bottom_banner.gri
	convert bottom_banner.ps bottom_banner.gif
	chmod a+r bottom_banner.gif
	rm -f bottom_banner.ps

	gri top_banner.gri
	convert top_banner.ps top_banner.gif
	chmod a+r top_banner.gif
	rm -f top_banner.ps
clean:
	rm -rf logo.eps logo.html *~
