# Makefile for Gri documentation files.

            DOC_DIR = $(prefix)/share/gri/doc
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
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

html: gri.texi gri.info
	cd examples ; make jpg
	cp examples/*.jpg ./
	cd examples ; make html
	cp examples/*.html ./
	cd screenshots; make jpg
	cat gri.texi     		>  TMP
	perl make_html_commandindex	>> TMP
	perl make_html_builtinindex	>> TMP
	perl make_html_index		>> TMP
	cat TMP | perl texinfo2HTML	>  $(HTML_LONG_NAME)
	rm -f TMP
	make read
	rm -f gri[1-9]*.html
	perl HTML_subdivide  $(HTML_LONG_NAME) "The Gri graphing language"
	rm -f $(HTML_LONG_NAME)
	rm -f *.pass2
#	if test ! -f index.html ; then ln -s gri1.html index.html ; fi
#	perl archive-to-html.pl > Newsgroup.html
	touch html

card-install-redhat: FORCE
	install -m 644 refcard.ps    $(CARD_DIR)
	install -m 644 cmdrefcard.ps $(CARD_DIR)

html-tar: html
	make html-install DOC_DIR=./gridoc
	tar -c -f gridoc.tar ./gridoc
	gzip -f --best gridoc.tar

html-install:	html
	install -d             $(HTML_DIR)
	chmod a+rx             $(HTML_DIR)
	install -d             $(HTML_DIR)/resources
	chmod a+rx             $(HTML_DIR)/resources
	cp resources/*.gif     $(HTML_DIR)/resources
	chmod 644              $(HTML_DIR)/resources/*.gif
	cp examples/*.jpg      $(HTML_DIR)/
	chmod 644              $(HTML_DIR)/*.jpg
	install -d             $(HTML_DIR)/screenshots
	chmod a+rx             $(HTML_DIR)/screenshots
	cp screenshots/*.jpg   $(HTML_DIR)/screenshots
	chmod a+rx             $(HTML_DIR)/screenshots
	chmod 644              $(HTML_DIR)/screenshots/*.jpg
	cp *.html              $(HTML_DIR)
	chmod 644              $(HTML_DIR)/*.html
#       Now install the examples
	install -d             $(EXAMPLES_DIR)
	chmod a+rx             $(EXAMPLES_DIR)
	cp examples/model*     $(EXAMPLES_DIR)
	chmod 644              $(EXAMPLES_DIR)/model*
	cp examples/*.dat      $(EXAMPLES_DIR)
	chmod 644              $(EXAMPLES_DIR)/*.dat
	cp examples/*.gri      $(EXAMPLES_DIR)
	chmod 644              $(EXAMPLES_DIR)/*.gri
	cp examples/*.ps       $(EXAMPLES_DIR)
	chmod 644              $(EXAMPLES_DIR)/*.ps
	cp examples/FEM.pl     $(EXAMPLES_DIR)
	chmod 755              $(EXAMPLES_DIR)/FEM.pl
	(cd $(HTML_DIR); ln -sf ../examples)

card-clean:
	-rm -f refcard.dvi refcard.log refcard.ps
	-rm -f cmdrefcard.dvi cmdrefcard.log cmdrefcard.ps
	-rm -f card

html-clean:
	rm -rf HIDE_FILE
	mkdir HIDE_FILE
	mv FAQ.html HIDE_FILE
	-rm -f [A-Z]*.html *.jpg example*.html gri1.html index.html html
	mv HIDE_FILE/FAQ.html .
	rm -rf HIDE_FILE

info:	gri.texi
	makeinfo gri.texi
	touch info

gri.info: gri.texi
	makeinfo gri.texi

info-install:
	install -d $(INFO_DIR)
	cp gri.info* $(INFO_DIR)
	(cd $(INFO_DIR) ; gzip -f --best gri.info gri.info-[0-9] gri.info-[0-9][0-9])
	chmod 644 $(INFO_DIR)/gri.info*

info-install-solaris:
	$(INSTALL) -d $(INFO_DIR_SOLARIS)
	cp gri.info* $(INFO_DIR_SOLARIS)
	chmod 644 $(INFO_DIR_SOLARIS)/gri.info*

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

pdf: gri.ps
	ps2pdf gri.ps

clean:	info-clean html-clean debian-clean card-clean
	$(RM)				\
		gri.pdf gri.dvi gri.ps	\
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
		FAQ cmdrefcard.ps refcard.ps
	(cd examples; make clean)
	(cd screenshots; make clean)
	(cd resources; make clean)

help:
	@echo "Options:"
	@grep '^[^ ]*:' Makefile | sed -e "s/^/  make /" -e "s/://"

linux_redhat:
	make info
	make html
	make refcard.ps
	make cmdrefcard.ps

gri.ps: gri.texi
###LATER#### example Makefile is missing, and probably requires Gri installed.
###LATER#### Should include .ps files instead of making them
###LATER#### example13 also needs GRI_EOS_DIR 
###LATER####	cd ./examples; make clean; make examples
	cd examples ; make eps
	cd screenshots; make eps
	tex gri.texi
	texindex gri.cp
	texindex gri.fn
	texindex gri.ky
	texindex gri.pg
	texindex gri.tp
	texindex gri.vr
	tex gri.texi
	dvips -o gri.ps gri.dvi

# Debian stuff follows
# $(prefix) is either ../debian/gri-html-doc/usr/share/doc
#                     ../debian/gri-ps-doc/usr/share/doc
#                     ../debian/tmp/usr/share/doc
refcard.ps:	refcard.tex
	tex refcard.tex
	dvips -o refcard.ps -t landscape refcard.dvi

cmdrefcard.ps:
	tex cmdrefcard.tex
	dvips -o cmdrefcard.ps -t landscape cmdrefcard.dvi

# make debian   (does not install; only makes the files)
debian: refcard.ps cmdrefcard.ps debian-info

# make debian-info  (does not install; only makes the files)
debian-info: gri.texi
	rm -f a
	sed -e "s/@dircategory Scientific Applications/@dircategory Math/;\
                s/\* GRI: (gri).  Scientific graphics language./* Gri: (gri).              Extensible plotting program designed for scientists/;" \
	< gri.texi > a
	makeinfo a
	rm -f a
	touch debian-info

# make debian-html prefix=../debian/gri-html-doc/usr/doc
debian-html:
	rm -fR $(prefix)/gri-html-doc/examples
	rm -fR $(prefix)/gri-html-doc/html/examples
	make html-install HTML_DIR=$(prefix)/gri-html-doc/html

# make debian-ps PREFIX=../debian/gri-ps-doc/usr/doc
debian-ps: gri.ps
	install -d $(prefix)/gri-ps-doc
	install -m 644 gri.ps $(prefix)/gri-ps-doc

debian-clean:
	-rm debian-info
