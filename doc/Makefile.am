## Process this file with automake to produce Makefile.in
# gri: doc/Makefile.am
             srcdir = @srcdir@
              VPATH = @srcdir@
      info_TEXINFOS = gri.texi
    PACKAGE_VERSION = @PACKAGE_VERSION@
if OS_IS_LINUX_REDHAT
            MAN_DIR = $(DESTDIR)$(prefix)/share/man/man1
           INFO_DIR = $(DESTDIR)$(prefix)/share/info
            DOC_DIR = $(DESTDIR)$(prefix)/share/doc/gri-${PACKAGE_VERSION}
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
else
if OS_IS_LINUX_DEBIAN
            MAN_DIR = $(DESTDIR)$(prefix)/share/man/man1
           INFO_DIR = $(DESTDIR)$(prefix)/share/info
            DOC_DIR = $(DESTDIR)$(prefix)/share/doc/gri
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
else
if OS_IS_FINK
            MAN_DIR = $(DESTDIR)$(prefix)/share/man/man1
           INFO_DIR = $(DESTDIR)$(prefix)/share/info
            DOC_DIR = $(DESTDIR)$(prefix)/share/doc/gri-${PACKAGE_VERSION}
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
else
if OS_IS_FREEBSD
            MAN_DIR = $(DESTDIR)$(prefix)/man/man1
           INFO_DIR = $(DESTDIR)$(prefix)/info
            DOC_DIR = $(DESTDIR)$(prefix)/share/doc/gri
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DESTDIR)$(prefix)/share/examples/gri
else
            MAN_DIR = $(DESTDIR)$(prefix)/share/man/man1
           INFO_DIR = $(DESTDIR)$(prefix)/share/info
            DOC_DIR = $(DESTDIR)$(prefix)/share/gri/doc
           HTML_DIR = $(DOC_DIR)/html
       EXAMPLES_DIR = $(DOC_DIR)/examples
endif
endif
endif
endif
                 RM = rm -f
#           REFCARD = refcard
#       CMD_REFCARD = cmdrefcard
     HTML_LONG_NAME = gri-long
#   INFO_DIR_SOLARIS = $(DESTDIR)$(prefix)/info

TEX = ${SHELL} ../missing --run tex
TEXINDEX = ${SHELL} ../missing --run texindex


SUBDIRS = examples resources screenshots tst_suite
EXTRA_DIST = \
 refcard.tex cmdrefcard.tex\
 install-sh archive-to-html.pl cmdrefcard.tex refcard.tex\
 FAQ.html\
 gri-manpage.1 gri-manpage-redhat.1 gri-manpage-SunOS5.1\
 gri_merge.1-skel gri_unpage.1-skel\
 gri2texi texinfo2HTML gri2html HTML_subdivide\
 make_html_index make_html_commandindex make_html_builtinindex\
 mdate-sh

html: $(srcdir)/gri.texi
	cd examples ; ${MAKE}
	cd tst_suite ; ${MAKE}
# 	Make the info page (even if rewritten later) to get indices ...
	$(MAKEINFO) $(AM_MAKEINFOFLAGS) $(MAKEINFOFLAGS) -I$(srcdir) gri.texi
#	... but this is clumsy and should be done with a dependency.
	cat $(srcdir)/gri.texi           	>  TMP
	perl $(srcdir)/make_html_index		>> TMP
	perl $(srcdir)/make_html_commandindex	>> TMP
	perl $(srcdir)/make_html_builtinindex	>> TMP
	cat TMP | perl $(srcdir)/texinfo2HTML	>  $(HTML_LONG_NAME)
	$(RM) TMP
	$(RM) gri[1-9]*.html
	perl $(srcdir)/HTML_subdivide  $(HTML_LONG_NAME) "The Gri graphing language"
	$(RM) $(HTML_LONG_NAME)
	$(RM) *.pass2
	if test ! -f index.html ; then ln -s gri1.html index.html ; fi
	touch html

install-refcards:
	$(INSTALL_DATA) refcard.ps    $(CARD_DIR)
	$(INSTALL_DATA) cmdrefcard.ps $(CARD_DIR)

html-tar:
	${MAKE} html-install DOC_DIR=./gridoc
	tar -c -f gridoc.tar ./gridoc
	gzip -f --best gridoc.tar

html-install:
# 	NB. Be careful in editing this target.  Things that are
# 	created during the building process do not have $(srcdir)
#	in their names; only things in the tarball have that prefix.
	${INSTALL} -d                                 $(HTML_DIR)
	${INSTALL} -d                                 $(HTML_DIR)/resources
	$(INSTALL_DATA) $(srcdir)/resources/*.gif     $(HTML_DIR)/resources
	${INSTALL} -d                                 $(HTML_DIR)/tst_suite
	$(INSTALL_DATA)           tst_suite/*html     $(HTML_DIR)/tst_suite
	$(INSTALL_DATA) $(srcdir)/examples/*.gri      $(HTML_DIR)/
	$(INSTALL_DATA)           examples/*.html     $(HTML_DIR)/
	$(INSTALL_DATA)           examples/*.png      $(HTML_DIR)/
	${INSTALL} -d                                 $(HTML_DIR)/screenshots
	$(INSTALL_DATA) $(srcdir)/screenshots/*.png   $(HTML_DIR)/screenshots
	$(INSTALL_DATA)           *.html              $(HTML_DIR)
	${INSTALL} -d                                 $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/model*     $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/*.dat      $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/*.gri      $(EXAMPLES_DIR)
	$(INSTALL_DATA) $(srcdir)/examples/e*.ps      $(EXAMPLES_DIR)
	$(INSTALL_SCRIPT) $(srcdir)/examples/FEM.pl  $(EXAMPLES_DIR)
	(cd $(HTML_DIR); rm -rf examples ; ln -sf ../examples)

# Sun Jun  8 11:40:52 UTC 2003  [Dan Kelley]
# This is commented-out because it's not used.  Later it will be deleted.
#card-clean:
#	-$(RM) refcard.dvi refcard.log refcard.ps
#	-$(RM) cmdrefcard.dvi cmdrefcard.log cmdrefcard.ps
#	-$(RM) card

# Sun Jun  8 11:40:52 UTC 2003  [Dan Kelley]
# This is commented-out because it's not used.  Later it will be deleted.
#html-clean:
#	rm -rf HIDE_FILE
#	mkdir HIDE_FILE
#	mv FAQ.html HIDE_FILE
#	-$(RM) *.html *.png html
#	mv HIDE_FILE/FAQ.html .
#	rm -rf HIDE_FILE

#info-install-solaris:
#	$(INSTALL) -d $(INFO_DIR_SOLARIS)
#	chmod 755     $(INFO_DIR_SOLARIS)
#	cp gri.info* $(INFO_DIR_SOLARIS)
#	chmod 644 $(INFO_DIR_SOLARIS)/gri.info*

refcard.ps: refcard.tex
	$(TEX) $(srcdir)/refcard.tex
	dvips -o refcard.ps -t landscape -t letter refcard.dvi

cmdrefcard.ps: cmdrefcard.tex
	$(TEX) $(srcdir)/cmdrefcard.tex
	dvips -o cmdrefcard.ps -t landscape -t letter cmdrefcard.dvi

gri.ps: gri.texi
	cd examples    ; ${MAKE} eps
	cd screenshots ; ${MAKE} eps
	cd tst_suite   ; ${MAKE} texi
	$(TEX) gri.texi
	$(TEXINDEX) gri.cp
	$(TEXINDEX) gri.fn
	$(TEXINDEX) gri.ky
	$(TEXINDEX) gri.pg
	$(TEXINDEX) gri.tp
	$(TEXINDEX) gri.vr
	$(TEX) gri.texi
	dvips -o gri.ps -t letter gri.dvi

#ps-install: gri.ps
#	$(INSTALL) -d $(DOC_DIR)
#	chmod 755     $(DOC_DIR)
#	$(INSTALL) -m 644 gri.ps $(DOC_DIR)

gri.pdf: gri.texi
	cd examples    ; ${MAKE} pdf
	cd screenshots ; ${MAKE} pdf
	cd tst_suite   ; ${MAKE} texi
	pdftex gri.texi
	$(TEXINDEX) gri.cp
	$(TEXINDEX) gri.fn
	$(TEXINDEX) gri.ky
	$(TEXINDEX) gri.pg
	$(TEXINDEX) gri.tp
	$(TEXINDEX) gri.vr
	pdftex gri.texi
	pdftex gri.texi
	pdftex gri.texi

# Add to some of the automake-created targets.
all-local: refcard.ps cmdrefcard.ps
	${MAKE} html
if OS_IS_LINUX_REDHAT
gri_manpage_name = gri-manpage-redhat.1
else
gri_manpage_name = gri-manpage.1
endif

install-data-local:
	${MAKE} html-install
	$(MKDIR_P)                                       $(DOC_DIR)
	$(INSTALL) -m 644 $(srcdir)/../license.txt       $(DOC_DIR)
if !OS_IS_LINUX_REDHAT
#       This is handled by gri.spec [2003-may-31 Dan Kelley]
	$(INSTALL_DATA) refcard.ps                       $(DOC_DIR)
	$(INSTALL_DATA) cmdrefcard.ps                    $(DOC_DIR)
endif
	mkdir -m 755 -p                                    $(MAN_DIR)
	cat $(srcdir)/../doc/$(gri_manpage_name) | sed -e s,VERSION,${PACKAGE_VERSION}, > tmp
	$(INSTALL_DATA)    tmp                             $(MAN_DIR)/gri.1
	$(RM) tmp
#	Install manpages manually; man_MANS puts them in /usr/man/man1, which is not where
#	they are supposed to go, at least on linux/redhat systems [2003-jun-8 Dan Kelley]
	$(INSTALL_DATA) $(srcdir)/../doc/gri_merge.1-skel  $(MAN_DIR)/gri_merge.1
	$(INSTALL_DATA) $(srcdir)/../doc/gri_unpage.1-skel $(MAN_DIR)/gri_unpage.1
	mkdir -m 755 -p                                    $(INFO_DIR)
	echo "INSTALLING INTO INFO_DIR WHICH IS [$(INFO_DIR)]"
if !OS_IS_FINK
	$(INSTALL_DATA)    $(srcdir)/../doc/gri.info*      $(INFO_DIR)
else
	@echo "DEBUG: The OS is fink"
endif
if !OS_IS_LINUX_REDHAT
#	This is handled by gri.spec [2003-may-31 Dan Kelley]
	gzip -f --best $(MAN_DIR)/gri_merge.1
	gzip -f --best $(MAN_DIR)/gri_unpage.1
if !OS_IS_FINK
	cd  $(INFO_DIR) ; gzip -f --best gri.info*
endif
endif


uninstall-local:
	$(RM) `ls ../*ps ../license.txt *.html gri.info* $(HTML_DIR)/examples/* $(HTML_DIR)/resources/* $(HTML_DIR)/tst_suite/* $(HTML_DIR)/screenshots/* $(HTML_DIR)/*`
	$(RM) html
	$(RM) $(DOC_DIR)/license.txt $(DOC_DIR)/refcard.ps $(DOC_DIR)/cmdrefcard.ps
	$(RM) refcard* cmdrefcard*
	$(RM) $(MAN_DIR)/gri.* $(MAN_DIR)/gri_unpage.* $(MAN_DIR)/gri_merge.*
	$(RM) $(INFO_DIR)/gri.info*
