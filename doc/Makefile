# Makefile for Gri documentation files.

            DOC_DIR = $(prefix)/share/gri/doc
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
                 RM = rm -f
            REFCARD = refcard
        CMD_REFCARD = cmdrefcard
     HTML_LONG_NAME = gri-long
#          INFO_DIR = $(prefix)/info
           INFO_DIR = %{_infodir}

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

html: gri.texi gri.info texinfo2HTML HTML_subdivide
	cd examples ; make png
	-cp examples/*.png ./
	cd examples ; make html
	cp examples/*.html ./
	cd tst_suite ; make html
	cd screenshots; make png
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
	install -d             $(HTML_DIR)/tst_suite
	chmod a+rx             $(HTML_DIR)/tst_suite
	cp tst_suite/*html     $(HTML_DIR)/tst_suite
	-cp examples/*.png      $(HTML_DIR)/
	-chmod 644              $(HTML_DIR)/*.png
	install -d             $(HTML_DIR)/screenshots
	chmod a+rx             $(HTML_DIR)/screenshots
	cp screenshots/*.png   $(HTML_DIR)/screenshots
	chmod a+rx             $(HTML_DIR)/screenshots
	chmod 644              $(HTML_DIR)/screenshots/*.png
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
	-rm -f *.html *.png html
	mv HIDE_FILE/FAQ.html .
	rm -rf HIDE_FILE

gri.texi: gri.texim
	perl texim2texi <gri.texim >gri.texi

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
	(cd examples    ; make clean)
	(cd screenshots ; make clean)
#	(cd resources   ; make clean)
	(cd tst_suite   ; make clean)

help:
	@echo "Options:"
	@grep '^[^ ]*:' Makefile | sed -e "s/^/  make /" -e "s/://"

linux_redhat:
	make info
	make html
	make refcard.ps
	make cmdrefcard.ps

gri.ps: gri.texi
	cd examples    ; make eps
	cd screenshots ; make eps
	cd tst_suite   ; make texi
	tex gri.texi
	texindex gri.cp
	texindex gri.fn
	texindex gri.ky
	texindex gri.pg
	texindex gri.tp
	texindex gri.vr
	tex gri.texi
	dvips -o gri.ps gri.dvi

gri.pdf: gri.texi
	cd examples    ; make pdf
	cd screenshots ; make pdf
	cd tst_suite   ; make texi
	pdftex gri.texi
	texindex gri.cp
	texindex gri.fn
	texindex gri.ky
	texindex gri.pg
	texindex gri.tp
	texindex gri.vr
	pdftex gri.texi



# Debian stuff follows
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

# make debian-html DOC_DIR=../debian/gri-html-doc/usr/share/doc/gri-html-doc
debian-html:
	make html-install

# make debian-ps DOC_DIR=../debian/gri-ps-doc/usr/share/doc/gri-ps-doc
debian-ps: gri.ps
	install -d $(DOC_DIR)
	install -m 644 gri.ps $(DOC_DIR)

debian-clean:
	-rm debian-info
