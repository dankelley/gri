# Create cookbook.

VERSION = 0.1.4
CONVERT  = convert		# /usr/X11R6/bin/convert in redhat-6.1 linux
PSTOPDF = ps2pdf

all:
	for f in 1 2 3 4 5 6 7 8 9 10 11 12 13 14; do make $$f; done
DIR_redhat=/usr/doc/gri-cookbook-$(VERSION)
install_linux_redhat:
	if test ! -d $(DIR_redhat)       ; then mkdir -m 755 -p $(DIR_redhat)       ; fi
	if test ! -d $(DIR_redhat)/1     ; then mkdir -m 755 -p $(DIR_redhat)/1     ; fi
	if test ! -d $(DIR_redhat)/2     ; then mkdir -m 755 -p $(DIR_redhat)/2     ; fi
	if test ! -d $(DIR_redhat)/3     ; then mkdir -m 755 -p $(DIR_redhat)/3     ; fi
	if test ! -d $(DIR_redhat)/4     ; then mkdir -m 755 -p $(DIR_redhat)/4     ; fi
	if test ! -d $(DIR_redhat)/5     ; then mkdir -m 755 -p $(DIR_redhat)/5     ; fi
	if test ! -d $(DIR_redhat)/6     ; then mkdir -m 755 -p $(DIR_redhat)/6     ; fi
	if test ! -d $(DIR_redhat)/7     ; then mkdir -m 755 -p $(DIR_redhat)/7     ; fi
	if test ! -d $(DIR_redhat)/8     ; then mkdir -m 755 -p $(DIR_redhat)/8     ; fi
	if test ! -d $(DIR_redhat)/9     ; then mkdir -m 755 -p $(DIR_redhat)/9     ; fi
	if test ! -d $(DIR_redhat)/10    ; then mkdir -m 755 -p $(DIR_redhat)/10    ; fi
	if test ! -d $(DIR_redhat)/11    ; then mkdir -m 755 -p $(DIR_redhat)/11    ; fi
	if test ! -d $(DIR_redhat)/12    ; then mkdir -m 755 -p $(DIR_redhat)/12    ; fi
	if test ! -d $(DIR_redhat)/13    ; then mkdir -m 755 -p $(DIR_redhat)/13    ; fi
	if test ! -d $(DIR_redhat)/14    ; then mkdir -m 755 -p $(DIR_redhat)/14    ; fi
	install -m 644 *html $(DIR_redhat)
	install -m 644  1/*gif  1/*html   $(DIR_redhat)/1
	install -m 644  2/*gif  1/*html   $(DIR_redhat)/2
	install -m 644  3/*gif  3/*html   $(DIR_redhat)/3
	install -m 644  4/*gif  4/*html   $(DIR_redhat)/4
	install -m 644  5/*gif  5/*html   $(DIR_redhat)/5
	install -m 644  6/*gif  6/*html   $(DIR_redhat)/6
	install -m 644  7/*gif  7/*html   $(DIR_redhat)/7
	install -m 644  8/*gif  8/*html   $(DIR_redhat)/8
	install -m 644  9/*gif  9/*html   $(DIR_redhat)/9
	install -m 644 10/*gif 10/*html   $(DIR_redhat)/10
	install -m 644 11/*gif 11/*html   $(DIR_redhat)/11
	install -m 644 12/*gif 12/*html   $(DIR_redhat)/12
	install -m 644 12/seawifs.palette $(DIR_redhat)/12
	install -m 644 12/example-vertical.gri $(DIR_redhat)/12
	install -m 644 13/*gif 13/*html   $(DIR_redhat)/13
	install -m 644 14/*gif 14/*html   $(DIR_redhat)/14

uninstall_linux_redhat:
	rm -rf $(DIR_redhat)

FORCE:

1: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
2: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
3: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
4: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
5: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
6: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
7: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
8: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
9: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	chmod a+r $@/*
10: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	chmod a+r $@/*
11: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
12: FORCE
	perl ./gri2html $@/example
	perl ./gri2html $@/example-vertical
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
13: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(PSTOPDF) $@/example.ps $@/example.pdf
	$(CONVERT) -geometry 999x200 $@/example.ps $@/example-small.gif
	chmod a+r $@/*
14: FORCE
	perl ./gri2html $@/example
	$(CONVERT) -rotate 90 $@/example.ps $@/example.gif
	$(CONVERT) -geometry 999x200 $@/example.gif $@/example-small.gif
	chmod a+r $@/*

clean:
	-rm -f *.lof *.dvi *.idx *.ilg *.ind *.log *.toc *.aux *.bbl *.blg cookbook.ps *~ */example-small.gif */example.gif */example.pdf */example.html 12/example-vertical.html

HTMLDIR=gri-cookbook-$(VERSION)
html-tar:
	rm -rf $(HTMLDIR)
	mkdir $(HTMLDIR)
#	cp $(HTMLDIR).spec Makefile gri2html cookbook.html discussion.html index.html recipes.html submitting.html $(HTMLDIR)
	cp *.html $(HTMLDIR)
	mkdir $(HTMLDIR)/1;  cp  1/*gri  1/*gif  1/*html $(HTMLDIR)/1
	mkdir $(HTMLDIR)/2;  cp  2/*gri  2/*gif  2/*html $(HTMLDIR)/2
	mkdir $(HTMLDIR)/3;  cp  3/*gri  3/*gif  3/*html $(HTMLDIR)/3
	mkdir $(HTMLDIR)/4;  cp  4/*gri  4/*gif  4/*html $(HTMLDIR)/4
	mkdir $(HTMLDIR)/5;  cp  5/*gri  5/*gif  5/*html $(HTMLDIR)/5
	mkdir $(HTMLDIR)/6;  cp  6/*gri  6/*gif  6/*html $(HTMLDIR)/6
	mkdir $(HTMLDIR)/7;  cp  7/*gri  7/*gif  7/*html $(HTMLDIR)/7
	mkdir $(HTMLDIR)/8;  cp  8/*gri  8/*gif  8/*html $(HTMLDIR)/8
	mkdir $(HTMLDIR)/9;  cp  9/*gri  9/*gif  9/*html $(HTMLDIR)/9
	mkdir $(HTMLDIR)/10; cp 10/*gri 10/*gif 10/*html $(HTMLDIR)/10
	mkdir $(HTMLDIR)/11; cp 11/*gri 11/*gif 11/*html $(HTMLDIR)/11
	mkdir $(HTMLDIR)/12; cp 12/seawifs.palette 12/*gri 12/*gif 12/*html $(HTMLDIR)/12
	mkdir $(HTMLDIR)/13; cp 13/*gri 13/*gif 13/*html $(HTMLDIR)/13
	mkdir $(HTMLDIR)/14; cp 14/*gri 14/*gif 14/*html $(HTMLDIR)/14
	tar -chf gri-cookbook-$(VERSION).tar $(HTMLDIR)
	gzip --best gri-cookbook-$(VERSION).tar
	rm -rf $(HTMLDIR)

TARDIR=gri-cookbook-src-$(VERSION)
source: clean
	rm -rf $(TARDIR)
	mkdir $(TARDIR)
	cp *.spec Makefile gri2html cookbook.html discussion.html index.html recipes.html submitting.html $(TARDIR)
	mkdir $(TARDIR)/1;  cp  1/example* $(TARDIR)/1
	mkdir $(TARDIR)/2;  cp  2/example* $(TARDIR)/2
	mkdir $(TARDIR)/3;  cp  3/example* $(TARDIR)/3
	mkdir $(TARDIR)/4;  cp  4/example* 4/*db $(TARDIR)/4
	mkdir $(TARDIR)/5;  cp  5/example* $(TARDIR)/5
	mkdir $(TARDIR)/6;  cp  6/example* $(TARDIR)/6
	mkdir $(TARDIR)/7;  cp  7/example* $(TARDIR)/7
	mkdir $(TARDIR)/8;  cp  8/example* $(TARDIR)/8
	mkdir $(TARDIR)/9;  cp  9/example* $(TARDIR)/9
	mkdir $(TARDIR)/10; cp 10/example* $(TARDIR)/10
	mkdir $(TARDIR)/11; cp 11/example* $(TARDIR)/11
	mkdir $(TARDIR)/12; cp 12/example* 12/seawifs.palette $(TARDIR)/12
	mkdir $(TARDIR)/13; cp 13/example* $(TARDIR)/13
	mkdir $(TARDIR)/14; cp 14/example* $(TARDIR)/14
	tar -chf gri-cookbook-src-$(VERSION).tar $(TARDIR)
	gzip --best gri-cookbook-src-$(VERSION).tar
	rm -rf $(TARDIR)

help:
	@echo "Various \"make\" commands can be used with this Makefile."
	@echo "The possibilities are as follows."
	@echo
	@grep ":" Makefile | awk '{print($$1)}' | sed -e "s/://g" -e "s/^/make /" | sort | uniq



# following is broken!
cookbook:
	latex cookbook
	makeindex cookbook
	latex cookbook
	dvips cookbook.dvi > cookbook.ps


