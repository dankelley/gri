## Process this file with automake to produce Makefile.in
# gri: doc/Makefile.am
             srcdir = @srcdir@
              VPATH = @srcdir@
      info_TEXINFOS = gri.texi
    PACKAGE_VERSION = @PACKAGE_VERSION@
if OS_IS_LINUX_REDHAT
            DOC_DIR = $(DESTDIR)$(prefix)/share/doc/gri-${PACKAGE_VERSION}
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
else
            DOC_DIR = $(DESTDIR)$(prefix)/share/gri/doc
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
endif
                 RM = rm -f
            REFCARD = refcard
        CMD_REFCARD = cmdrefcard
     HTML_LONG_NAME = gri-long
#           INFO_DIR = $(DESTDIR)$(prefix)/info
   INFO_DIR_SOLARIS = $(DESTDIR)$(prefix)/info

SUBDIRS = examples resources screenshots tst_suite
EXTRA_DIST = \
 refcard.tex cmdrefcard.tex\
 install-sh archive-to-html.pl cmdrefcard.tex refcard.tex\
 FAQ.html\
 gri-manpage.1 gri-manpage-redhat.1 gri-manpage-SunOS5.1\
 gri_merge.1-skel gri_unpage.1-skel\
 gri2texi texinfo2HTML gri2html HTML_subdivide\
 make_html_index make_html_commandindex make_html_builtinindex

#dekall:
#	make info
#	make html

html: $(srcdir)/gri.texi texinfo2HTML HTML_subdivide
# 	Make the info page (even if rewritten later) to get indices
	makeinfo $(srcdir)/gri.texi
	cat $(srcdir)/gri.texi           	>  TMP
	perl $(srcdir)/make_html_index		>> TMP
	perl $(srcdir)/make_html_commandindex	>> TMP
	perl $(srcdir)/make_html_builtinindex	>> TMP
	cat TMP | perl $(srcdir)/texinfo2HTML	>  $(HTML_LONG_NAME)
	rm -f TMP
	rm -f gri[1-9]*.html
	perl $(srcdir)/HTML_subdivide  $(HTML_LONG_NAME) "The Gri graphing language"
	rm -f $(HTML_LONG_NAME)
	rm -f *.pass2
	if test ! -f index.html ; then ln -s gri1.html index.html ; fi

install-refcards:
	$(INSTALL_DATA) refcard.ps    $(CARD_DIR)
	$(INSTALL_DATA) cmdrefcard.ps $(CARD_DIR)

html-tar:
	make html-install DOC_DIR=./gridoc
	tar -c -f gridoc.tar ./gridoc
	gzip -f --best gridoc.tar

html-install:
	${INSTALL} -d                                 $(HTML_DIR)
	${INSTALL} -d                                 $(HTML_DIR)/resources
	$(INSTALL_DATA) $(srcdir)/resources/*.gif     $(HTML_DIR)/resources
	${INSTALL} -d                                 $(HTML_DIR)/tst_suite
	$(INSTALL_DATA) tst_suite/*html               $(HTML_DIR)/tst_suite
	$(INSTALL_DATA) examples/*.png                $(HTML_DIR)/
	${INSTALL} -d                                 $(HTML_DIR)/screenshots
	$(INSTALL_DATA) $(srcdir)/screenshots/*.png   $(HTML_DIR)/screenshots
	$(INSTALL_DATA) *.html                        $(HTML_DIR)
	${INSTALL} -d          $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/model*     $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/*.dat      $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/*.gri      $(EXAMPLES_DIR)
	$(INSTALL_PROGRAM) $(srcdir)/examples/FEM.pl  $(EXAMPLES_DIR)
	(cd $(HTML_DIR); rm -rf examples ; ln -sf ../examples)

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

info-install-solaris:
	$(INSTALL) -d $(INFO_DIR_SOLARIS)
	chmod 755     $(INFO_DIR_SOLARIS)
	cp gri.info* $(INFO_DIR_SOLARIS)
	chmod 644 $(INFO_DIR_SOLARIS)/gri.info*

refcard.ps: refcard.tex
	tex $(srcdir)/refcard.tex
	dvips -o refcard.ps -t landscape -t letter refcard.dvi

cmdrefcard.ps: cmdrefcard.tex
	tex $(srcdir)/cmdrefcard.tex
	dvips -o cmdrefcard.ps -t landscape -t letter cmdrefcard.dvi

help:
	@echo "Options:"
	@grep '^[^ ]*:' Makefile | sed -e "s/^/  make /" -e "s/:.*//"

#Sun Jun  1 00:21:35 UTC 2003#
#Sun Jun  1 00:21:35 UTC 2003#linux_redhat:
#Sun Jun  1 00:21:35 UTC 2003#	make info
#Sun Jun  1 00:21:35 UTC 2003#	make html
#Sun Jun  1 00:21:35 UTC 2003#	make refcard.ps
#Sun Jun  1 00:21:35 UTC 2003#	make cmdrefcard.ps

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
	dvips -o gri.ps -t letter gri.dvi

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

### Debian stuff follows

# make debian   (does not install; only makes the files)
debian: refcard.ps cmdrefcard.ps info

# make debian-html DOC_DIR=../debian/gri-html-doc/usr/share/doc/gri
# builds and installs under $(DOC_DIR)
debian-html:
	make html-install

# make debian-ps DOC_DIR=../debian/gri-ps-doc/usr/share/doc/gri
# builds and installs under $(DOC_DIR)
debian-ps: gri.ps
	$(INSTALL) -d $(DOC_DIR)
	chmod 755     $(DOC_DIR)
	$(INSTALL) -m 644 gri.ps $(DOC_DIR)

# Add to some of the automake-created targets.
all-local: refcard.ps cmdrefcard.ps
	make html
if OS_IS_LINUX_REDHAT
manpage_name = gri-manpage-redhat.1
else
manpage_name = gri-manpage.1
endif
install-data-local:
	make html-install
	$(INSTALL) -m 644 $(srcdir)/../license.txt         $(DOC_DIR)
if !OS_IS_LINUX_REDHAT
#       This is handled by gri.spec [2003-may-31 Dan Kelley]
	$(INSTALL) -m 644 refcard.ps                       $(DOC_DIR)
	$(INSTALL) -m 644 cmdrefcard.ps                    $(DOC_DIR)
endif
	mkdir -m 755 -p                                    $(DESTDIR)$(prefix)/share/man/man1
	cat $(srcdir)/../doc/$(manpage_name) | sed -e s,VERSION,${PACKAGE_VERSION}, > tmp
	$(INSTALL_DATA)    tmp                             $(DESTDIR)$(prefix)/share/man/man1/gri.1
	rm -f tmp
	$(INSTALL_DATA) $(srcdir)/../doc/gri_merge.1-skel  $(DESTDIR)$(prefix)/share/man/man1/gri_merge.1
	$(INSTALL_DATA) $(srcdir)/../doc/gri_unpage.1-skel $(DESTDIR)$(prefix)/share/man/man1/gri_unpage.1
	mkdir -m 755 -p                                    $(DESTDIR)$(prefix)/share/info
	$(INSTALL_DATA)    $(srcdir)/../doc/gri.info*      $(DESTDIR)$(prefix)/share/info
if !OS_IS_LINUX_REDHAT
#	This is handled by gri.spec [2003-may-31 Dan Kelley]
	gzip $(DESTDIR)$(prefix)/share/man/man1/gri_merge.1
	gzip $(DESTDIR)$(prefix)/share/man/man1/gri_unpage.1
	cd  $(DESTDIR)$(prefix)/share/info ; gzip -f --best gri.info*
endif


uninstall-local:
	rm -f `ls ../*ps ../license.txt *.html gri.info* $(HTML_DIR)/examples/* $(HTML_DIR)/resources/* $(HTML_DIR)/tst_suite/* $(HTML_DIR)/screenshots/* $(HTML_DIR)/*`
	rm -f $(DOC_DIR)/license.txt $(DOC_DIR)/refcard.ps $(DOC_DIR)/cmdrefcard.ps
	rm -f refcard* cmdrefcard*
#	echo "DANDAN.  I am in ..."
#	pwd
#	echo "DANDAN. DESTDIR/prefix IS $(DESTDIR)/$(prefix) with contents"
#	ls $(DESTDIR)$(prefix)
	rm -f $(DESTDIR)$(prefix)/share/man/man1/gri*
	rm -f $(DESTDIR)$(prefix)/share/info/gri.info*
