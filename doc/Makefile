# Makefile for Gri documentation files.

            GRI_DIR = /opt/gri
           DEST_BIN = /doc/info
                 RM = rm -f
            REFCARD = refcard
        CMD_REFCARD = cmdrefcard
     HTML_LONG_NAME = gri-long
           INFO_DIR = $(PREFIX)/usr/info

FORCE:

all:
	make info
	make html

install:
	make info-install
	make html-install

read:
	chmod a+r Makefile
	chmod a+rx texinfo2HTML
	chmod a+rx HTML_subdivide
	chmod a+r gri*

FAQ: FORCE
	rm -f FAQ
	lynx -dump FAQ.html > FAQ

the_resources:
	cd resources ; make

test:
	cat test.texi | texinfo2HTML > test.html

html:
	cat gri.texi     		>  TMP
	perl make_html_index		>> TMP
	cat TMP | perl texinfo2HTML	>  $(HTML_LONG_NAME)
	rm -f TMP
	make read
	rm -f gri[1-9]*.html
	perl HTML_subdivide  $(HTML_LONG_NAME) "The Gri graphing language"
	rm -f $(HTML_LONG_NAME)
	rm -f *.pass2
	if test ! -f index.html ; then ln -s gri1.html index.html ; fi
	perl archive-to-html.pl > Newsgroup.html

html-install:
	# GRI_DIR = /opt/gri
	if test ! -d $(GRI_DIR)                    ; then mkdir -m 755 -p $(GRI_DIR)                    ; fi
	if test ! -d $(GRI_DIR)/doc                ; then mkdir -m 755 -p $(GRI_DIR)/doc                ; fi
	if test ! -d $(GRI_DIR)/doc/html           ; then mkdir -m 755 -p $(GRI_DIR)/doc/html           ; fi
	if test ! -d $(GRI_DIR)/doc/html/examples  ; then mkdir -m 755 -p $(GRI_DIR)/doc/html/examples  ; fi
	if test ! -d $(GRI_DIR)/doc/html/resources ; then mkdir -m 755 -p $(GRI_DIR)/doc/html/resources ; fi
	-cp ./resources/*.gif    $(GRI_DIR)/doc/html/resources
	-chmod a+r               $(GRI_DIR)/doc/html/resources/*
	-cp ./examples/*         $(GRI_DIR)/doc/html/examples
	-chmod a+r               $(GRI_DIR)/doc/html/examples/*
	-cp ./*.html             $(GRI_DIR)/doc/html
	-cp ./resources/logo.eps $(GRI_DIR)/doc/html/resources
	-cp ./resources/logo.gif $(GRI_DIR)/doc/html/resources
	-cp ./resources/logo.gri $(GRI_DIR)/doc/html/resources
	-chmod a+r               $(GRI_DIR)/*
	-make html-clean

html-clean:
	-cp FAQ.html save
	-rm -f [A-Z]*.html gri1.html index.html
	-mv save FAQ.html

#
#
info: gri.texi
	rm -f a
	sed -e "s/setfilename gri.info/setfilename gri/" < gri.texi > a
	makeinfo a
	\rm -f a

info-install:
	gzip gri gri-*[0-9]
	cp ./gri.gz ./gri-*[0-9].gz $(INFO_DIR)
	chmod a+r   $(INFO_DIR)/gri*
	make info-clean

info-clean:
	rm -f ./gri
	rm -f ./gri-1 ./gri-2 ./gri-3 ./gri-4 ./gri-5 ./gri-6 
	rm -f ./gri-7 ./gri-8 ./gri-9 ./gri-10 ./gri-11
	rm -f ./gri-13
	rm -f ./gri.gz
	rm -f ./gri-1.gz ./gri-2.gz ./gri-3.gz ./gri-4.gz ./gri-5.gz ./gri-6.gz
	rm -f ./gri-7.gz ./gri-8.gz ./gri-9.gz ./gri-10.gz ./gri-11.gz
	rm -f  ./gri-12.gz ./gri-13.gz

#
# FIXME: These refcards are missing!
#
refcard:
	make refcard1
	make refcard2
refcard1:
	tex refcard.tex
	dvips -o refcard.ps -t landscape refcard.dvi
	chmod a+r refcard.*
	ghostview -land refcard.ps
refcard2:
	echo "***"
	echo "*** First, do cmdrefcard.pl and manually put output in cmdrefcard.tex"
	echo "***"
	tex cmdrefcard.tex
	dvips -o cmdrefcard.ps -t landscape cmdrefcard.dvi
	chmod a+r cmdrefcard.*
	ghostview -land cmdrefcard.ps
#
#
postscript:
	cd examples ; make eps
	tex gri.texi
	texindex gri.cp
	texindex gri.fn
	texindex gri.ky
	texindex gri.pg
	texindex gri.tp
	texindex gri.vr
	tex gri.texi
	dvips -o gri.ps gri.dvi
	cd examples ; make clean

pdf: gri.ps
	ps2pdf gri.ps

#
#
clean:	info-clean html-clean
	$(RM)				\
		gri.pdf gri.dvi gri.ps	\
		gri gri.1 gri-[0-9]*	\
		gri.hlp			\
		*~			\
		*.log *.dvi *.aux	\
		gri.toc			\
		gri.cp gri.cps		\
		gri.fn gri.fns		\
		gri.ky gri.kys		\
		gri.pg gri.pgs		\
		gri.tp gri.tps		\
		gri.vr gri.vrs 		\
		cmdrefcard.ps refcard.ps
	(cd examples; make clean)

help:
	@echo "Options:"
	@grep '^[^ ]*:' Makefile | sed -e "s/^/  make /" -e "s/://"

linux_redhat:
	make info
	make html

linux_redhat_clean:
	rm -f gri    gri-*[1-9]
	rm -f gri.gz gri-*[1-9].gz
	cp FAQ.html save
	rm -f *.html gri.dvi gri.ps index.html
	mv save FAQ.html

gri.ps: gri.texi gri.dvi
###LATER#### example Makefile is missing, and probably requires Gri installed.
###LATER#### Should include .ps files instead of making them
###LATER#### example13 also needs GRI_EOS_DIR 
###LATER####	cd ./examples; make clean; make examples
	cd examples ; make eps
	tex gri.texi
	texindex gri.cp
	texindex gri.fn
	texindex gri.ky
	texindex gri.pg
	texindex gri.tp
	texindex gri.vr
	tex gri.texi
	dvips -o gri.ps gri.dvi

gri.gz: gri.texi
	make info

gri1.html: gri.texi
	make html
	rm -f index.html

# Debian stuff follows
# $(PREFIX) is either ../debian/gri-html-doc/usr/doc
#                     ../debian/gri-ps-doc/usr/doc
#                     ../debian/tmp/usr/doc
# or the same with /usr/share/doc instead.
refcard.ps:	refcard.tex
	tex refcard.tex
	dvips -o refcard.ps -t landscape refcard.dvi
	chmod a+r refcard.*

cmdrefcard.ps:
	tex cmdrefcard.tex
	dvips -o cmdrefcard.ps -t landscape cmdrefcard.dvi
	chmod a+r cmdrefcard.*

gri-1: gri.texi
	make info

debian:
	make refcard.ps
	install -d $(PREFIX)/gri
	install -m 644 refcard.ps $(PREFIX)/gri
	make cmdrefcard.ps
	install -m 644 cmdrefcard.ps $(PREFIX)/gri
	make gri-1
	install -d $(INFO_DIR)
	install -m644 ./gri ./gri-[0-9] ./gri-1[0-9] $(INFO_DIR)

debian-html:
	make gri1.html
	install -d $(PREFIX)/gri-html-doc/html
	install -m 644 *.html $(PREFIX)/gri-html-doc/html
	rm -f $(PREFIX)/gri-html-doc/html/index.html
	(cd $(PREFIX)/gri-html-doc/html; ln -sf gri1.html index.html)
	install -d $(PREFIX)/gri-html-doc/html/resources
	-cp ./resources/*.gif $(PREFIX)/gri-html-doc/html/resources
	-cp ./resources/logo.gri $(PREFIX)/gri-html-doc/html/resources
	-cp ./resources/logo.eps $(PREFIX)/gri-html-doc/html/resources
	install -d $(PREFIX)/gri-html-doc/examples
	install -m 644 ./examples/* $(PREFIX)/gri-html-doc/examples
	chmod +x $(PREFIX)/gri-html-doc/examples/FEM.pl
	(cd $(PREFIX)/gri-html-doc/html; ln -sf ../examples)

debian-ps:
	cd examples ; make eps
	make gri.ps
	install -d $(PREFIX)/gri-ps-doc
	install -m 644 gri.ps $(PREFIX)/gri-ps-doc
