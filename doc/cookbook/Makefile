# Create cookbook.

VERSION = 0.1.3
CONVERT  = convert		# /usr/X11R6/bin/convert in redhat-6.1 linux
PSTOPDF = ps2pdf

all:
	for f in 1 2 3 4 5 6 7 8 9 10 11 12 13; do make $$f; done

install_linux_redhat:
	if test ! -d /opt/gri-cookbook       ; then mkdir -m 755 -p /opt/gri-cookbook       ; fi
	if test ! -d /opt/gri-cookbook/1     ; then mkdir -m 755 -p /opt/gri-cookbook/1     ; fi
	if test ! -d /opt/gri-cookbook/2     ; then mkdir -m 755 -p /opt/gri-cookbook/2     ; fi
	if test ! -d /opt/gri-cookbook/3     ; then mkdir -m 755 -p /opt/gri-cookbook/3     ; fi
	if test ! -d /opt/gri-cookbook/4     ; then mkdir -m 755 -p /opt/gri-cookbook/4     ; fi
	if test ! -d /opt/gri-cookbook/5     ; then mkdir -m 755 -p /opt/gri-cookbook/5     ; fi
	if test ! -d /opt/gri-cookbook/6     ; then mkdir -m 755 -p /opt/gri-cookbook/6     ; fi
	if test ! -d /opt/gri-cookbook/7     ; then mkdir -m 755 -p /opt/gri-cookbook/7     ; fi
	if test ! -d /opt/gri-cookbook/8     ; then mkdir -m 755 -p /opt/gri-cookbook/8     ; fi
	if test ! -d /opt/gri-cookbook/9     ; then mkdir -m 755 -p /opt/gri-cookbook/9     ; fi
	if test ! -d /opt/gri-cookbook/10    ; then mkdir -m 755 -p /opt/gri-cookbook/10    ; fi
	if test ! -d /opt/gri-cookbook/11    ; then mkdir -m 755 -p /opt/gri-cookbook/11    ; fi
	if test ! -d /opt/gri-cookbook/12    ; then mkdir -m 755 -p /opt/gri-cookbook/12    ; fi
	if test ! -d /opt/gri-cookbook/13    ; then mkdir -m 755 -p /opt/gri-cookbook/13    ; fi
	install -m 644 *html /opt/gri-cookbook
	install -m 644  1/*gif  1/*html   /opt/gri-cookbook/1
	install -m 644  2/*gif  1/*html   /opt/gri-cookbook/2
	install -m 644  3/*gif  3/*html   /opt/gri-cookbook/3
	install -m 644  4/*gif  4/*html   /opt/gri-cookbook/4
	install -m 644  5/*gif  5/*html   /opt/gri-cookbook/5
	install -m 644  6/*gif  6/*html   /opt/gri-cookbook/6
	install -m 644  7/*gif  7/*html   /opt/gri-cookbook/7
	install -m 644  8/*gif  8/*html   /opt/gri-cookbook/8
	install -m 644  9/*gif  9/*html   /opt/gri-cookbook/9
	install -m 644 10/*gif 10/*html   /opt/gri-cookbook/10
	install -m 644 11/*gif 11/*html   /opt/gri-cookbook/11
	install -m 644 12/*gif 12/*html   /opt/gri-cookbook/12
	install -m 644 12/seawifs.palette /opt/gri-cookbook/12
	install -m 644 12/example-vertical.gri /opt/gri-cookbook/12
	install -m 644 13/*pdf 13/*gif 13/*html   /opt/gri-cookbook/13

uninstall_linux_redhat:
	rm -rf /opt/gri-cookbook


FORCE:

1: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
2: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
3: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
4: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
5: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
6: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
7: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
8: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
9: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
10: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
11: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
12: FORCE
	perl ./gri2html $@/example
	$(CONVERT) $@/example.ps $@/example.gif
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*
13: FORCE
	perl ./gri2html $@/example
	$(PSTOPDF) $@/example.ps $@/example.pdf
	$(CONVERT) -geometry 20x20% $@/example.ps $@/example-small.gif
	chmod a+r $@/*

clean:
	-rm -f *.lof *.dvi *.idx *.ilg *.ind *.log *.toc *.aux *.bbl *.blg cookbook.ps *~

TAR_DIR_NAME=gri-cookbook-$(VERSION)
source:
	rm -rf $(TAR_DIR_NAME)
	mkdir $(TAR_DIR_NAME)
	cp $(TAR_DIR_NAME).spec Makefile gri2html cookbook.html discussion.html index.html recipes.html submitting.html $(TAR_DIR_NAME)
	mkdir $(TAR_DIR_NAME)/1  ; cp  1/*gri  1/*ps  1/*html $(TAR_DIR_NAME)/1
	mkdir $(TAR_DIR_NAME)/2  ; cp  2/*gri  2/*ps  2/*html $(TAR_DIR_NAME)/2
	mkdir $(TAR_DIR_NAME)/3  ; cp  3/*gri  3/*ps  3/*html $(TAR_DIR_NAME)/3
	mkdir $(TAR_DIR_NAME)/4  ; cp  4/*gri  4/*ps  4/*html $(TAR_DIR_NAME)/4
	mkdir $(TAR_DIR_NAME)/5  ; cp  5/*gri  5/*ps  5/*html $(TAR_DIR_NAME)/5
	mkdir $(TAR_DIR_NAME)/6  ; cp  6/*gri  6/*ps  6/*html $(TAR_DIR_NAME)/6
	mkdir $(TAR_DIR_NAME)/7  ; cp  7/*gri  7/*ps  7/*html $(TAR_DIR_NAME)/7
	mkdir $(TAR_DIR_NAME)/8  ; cp  8/*gri  8/*ps  8/*html $(TAR_DIR_NAME)/8
	mkdir $(TAR_DIR_NAME)/9  ; cp  9/*gri  9/*ps  9/*html $(TAR_DIR_NAME)/9
	mkdir $(TAR_DIR_NAME)/10 ; cp 10/*gri 10/*ps 10/*html $(TAR_DIR_NAME)/10
	mkdir $(TAR_DIR_NAME)/11 ; cp 11/*gri 11/*ps 11/*html $(TAR_DIR_NAME)/11
	mkdir $(TAR_DIR_NAME)/12 ; cp 12/seawifs.palette 12/*gri 12/*ps 12/*html $(TAR_DIR_NAME)/12
	mkdir $(TAR_DIR_NAME)/13 ; cp 13/*gri 13/*ps 13/*html $(TAR_DIR_NAME)/13
	tar -zchof gri-cookbook-$(VERSION).tgz $(TAR_DIR_NAME)
	rm -rf $(TAR_DIR_NAME)

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


