# Makefile for Gri documentation files.

           HTML_DIR = $(prefix)/doc/gri/html
                 RM = rm -f
            REFCARD = refcard
        CMD_REFCARD = cmdrefcard
     HTML_LONG_NAME = gri-long
           INFO_DIR = $(prefix)/info

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

html: gri.texi
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
	touch html

html-install:	html
	install -d $(HTML_DIR)
	install -d $(HTML_DIR)/examples
	install -d $(HTML_DIR)/resources
	install -m 644 ./resources/logo.eps $(HTML_DIR)/resources
	install -m 644 ./resources/logo.gif $(HTML_DIR)/resources
	install -m 644 ./resources/logo.gri $(HTML_DIR)/resources
	install -m 644 ./resources/*.gif    $(HTML_DIR)/resources
	install -m 644 ./examples/*         $(HTML_DIR)/examples
	install -m 644 ./*.html             $(HTML_DIR)

html-clean:
	-cp FAQ.html save
	-rm -f [A-Z]*.html gri1.html index.html html
	-mv save FAQ.html

#
#

info:	gri.texi
	makeinfo gri.texi
	touch info

gri.info: gri.texi
	makeinfo gri.texi

info-install:
	gzip --best *.info
	cp *info.gz $(INFO_DIR)
	chmod a+r   $(INFO_DIR)/gri*
	make info-clean

info-clean:
	rm -f gri.info* info debian-info

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
pdf: gri.ps
	ps2pdf gri.ps

#
#
clean:	info-clean html-clean debian-clean
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
	rm -f gri.info*
	cp FAQ.html save
	rm -f *.html gri.dvi gri.ps index.html
	mv save FAQ.html

gri.ps: gri.texi
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

gri1.html: gri.texi
	make html
	rm -f index.html

# Debian stuff follows
# $(prefix) is either ../debian/gri-html-doc/usr/doc
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

# make debian PREFIX=../debian/tmp/usr/doc INFO_DIR=../debian/tmp/usr/info
debian: refcard.ps cmdrefcard.ps debian-info
	install -d $(prefix)/gri
	install -m 644 refcard.ps cmdrefcard.ps $(prefix)/gri
	install -d $(INFO_DIR)
	install -m 644 gri.info* $(INFO_DIR)

# make debian-info  (does not install; only makes the files)
debian-info: gri.texi
	rm -f a
	sed -e "s/@dircategory Scientific Applications/@dircategory Math/;\
                s/\* GRI: (gri).  Scientific graphics language./* Gri: (gri).              Extensible plotting program designed for scientists/;" \
	< gri.texi > a
	makeinfo a
	rm -f a
	touch debian-info

# make debian-html PREFIX=../debian/gri-html-doc/usr/doc
debian-html: gri1.html
	install -d $(prefix)/gri-html-doc/html
	install -m 644 *.html $(prefix)/gri-html-doc/html
	rm -f $(prefix)/gri-html-doc/html/index.html
	(cd $(prefix)/gri-html-doc/html; ln -sf gri1.html index.html)
	install -d $(prefix)/gri-html-doc/html/resources
	-cp ./resources/*.gif $(prefix)/gri-html-doc/html/resources
	-cp ./resources/logo.gri $(prefix)/gri-html-doc/html/resources
	-cp ./resources/logo.eps $(prefix)/gri-html-doc/html/resources
	install -d $(prefix)/gri-html-doc/examples
	install -m 644 ./examples/* $(prefix)/gri-html-doc/examples
	chmod +x $(prefix)/gri-html-doc/examples/FEM.pl
	(cd $(prefix)/gri-html-doc/html; ln -sf ../examples)

# make debian-ps PREFIX=../debian/gri-ps-doc/usr/doc
debian-ps: gri.ps
	install -d $(prefix)/gri-ps-doc
	install -m 644 gri.ps $(prefix)/gri-ps-doc

debian-clean:
	-rm debian-info
