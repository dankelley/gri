## Process this file with automake to produce Makefile.in
# gri: src/Makefile.am
srcdir = @srcdir@
VPATH = @srcdir@
SUBDIRS = popt
DIST_SUBDIRS = popt
bin_PROGRAMS = gri
gri_LDADD = @LIBS@ popt/libgripopt.a
gridir = $(datadir)/gri

gri_DATA = gri.cmd
nodist_gri_SOURCES = startup.debian startup.redhat 

gri_SOURCES = G_string.cc GriColor.cc GriPath.cc GriState.cc assert.cc\
 chopword.cc close.cc command.cc convert.cc debug.cc delete.cc differ.cc\
 doline.cc draw.cc endup.cc expect.cc file.cc filter.cc flip.cc\
 gr.cc gr_coll.cc graxes.cc grcntour.cc gri.cc grimage.cc grinterp.cc\
 grsmooth.cc grstring.cc group.cc heal.cc help.cc if.cc ignore.cc\
 image.cc input.cc insert.cc interp.cc mask.cc math.cc new.cc open.cc\
 popen.cc query.cc quit.cc read.cc regress.cc reorder.cc rescale.cc\
 rewind.cc rpn.cc rpncalc.cc scales.cc set.cc show.cc skip.cc sleep.cc\
 smooth.cc source.cc startup.cc state.cc stats.cc storage.cc\
 synonyms.cc template.cc timer.cc unlink.cc utility.cc variable.cc\
 version.cc while.cc write.cc\
 CmdFile.hh command.hh DataFile.hh debug.hh defaults.hh errors.hh\
 extern.hh files.hh GCounter.hh GMatrix.hh gr_coll.hh gr.hh\
 GriColor.hh GriPath.hh GriState.hh GriTimer.hh G_string.hh image_ex.hh\
 install-sh macro.hh postscpt.hh private.hh superus.hh Synonym.hh\
 tags.hh types.hh Variable.hh

EXTRA_DIST = Makefile.dj2 \
 gri.cmd logo.dat\
 gri-mode.el gri_merge gri_unpage\
 startup.msg startup.debian startup.redhat

PACKAGE_VERSION = @PACKAGE_VERSION@
if OS_IS_LINUX_DEBIAN
DEFS = -DDEFAULT_GRI_DIR=\"$(prefix)/share/gri/$(PACKAGE_VERSION)/\" @DEFS@
else
DEFS = -DDEFAULT_GRI_DIR=\"$(prefix)/share/gri/\" @DEFS@
endif

EXTRA_CFLAGS_TEMPLATE = @EXTRA_CFLAGS_TEMPLATE@ $(LINUX_EXTRA_CFLAGS)

#### End of system configuration section. ####

# Special rule for templates, so as not to use
# -fno-implicit-templates (g++ weirdness).  This special
# case depends on whether -frepo exists ... and whether the installer
# has temerity to try it!
template.o: template.cc
	$(CXX) -c $(CXXFLAGS) $(EXTRA_CFLAGS_TEMPLATE) $(DEFS) -I$(srcdir) $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(EXTRA_CFLAGS) $(AM_CXXFLAGS) $(DEFS) -I$(srcdir) $<

debian_lib = $(debian)/usr/share/gri/$(PACKAGE_VERSION)
install_linux_debian: gri
	@echo "Installing into '$(debian)' directory"
	install -d $(debian_lib)
	if test -f startup.debian; then sed -e s,VSN,${PACKAGE_VERSION}, startup.debian > $(debian_lib)/startup.msg; else echo "WARNING: no startup.debian file!"; fi;
	install -d         $(debian)/usr/bin
	install -m 755 gri $(debian)/usr/bin/gri-$(PACKAGE_VERSION)
	(cd $(debian)/usr/bin; ln -fs gri-$(PACKAGE_VERSION) gri)
	install -m 644 gri.cmd  $(debian_lib)/gri.cmd
	install -m 644 logo.dat $(debian_lib)/logo.dat
	if test -f gri_merge; then install -m 755 gri_merge $(debian)/usr/bin; else echo "WARNING: no gri_merge file!"; fi;	
	if test -f gri_unpage; then install -m 755 gri_unpage $(debian)/usr/bin; else echo "WARNING: no gri_unpage file!";fi;
#	install -d $(debian)/usr/share/emacs/site-lisp
#	install -m 644 gri-mode.el $(debian)/usr/share/emacs/site-lisp/

install_linux_debian_grionly: gri
	@echo "Installing into '$(debian)' directory"
	install -d             $(debian_lib)
	install -m 644 gri.cmd  $(debian_lib)/gri.cmd
	install -m 644 logo.dat $(debian_lib)/logo.dat
	if test -f startup.debian; then sed -e s,VSN,${PACKAGE_VERSION}, startup.debian > $(debian_lib)/startup.msg; else echo "WARNING: no startup.debian file!"; fi;
	install -d         $(debian)/usr/bin
	install -m 755 gri $(debian)/usr/bin/gri-$(PACKAGE_VERSION)

# These targets commented out
# 	Thu Jun  5 19:11:08 UTC 2003
# by Dan Kelley.  Let's leave them commented-out for a while,
# before they are completely removed.  That way we can realize
# if something is missing in the new automake-controlled system.
#2003jun5#SOURCE_NAME = gri-$(PACKAGE_VERSION)
#2003jun5#DEST_DIR = .
#2003jun5#source-copy:
#2003jun5##	Clean directory, if existent.
#2003jun5#	-rm -rf $(SOURCE_NAME)
#2003jun5#	install -d $(SOURCE_NAME)
#2003jun5#	install -d $(SOURCE_NAME)/popt
#2003jun5##	Copy source
#2003jun5#	install -m 644 ChangeLog $(gri_SOURCES) *.hh gri.cmd logo.dat license.txt \
#2003jun5#	    Makefile.am Makefile.in Makefile.dj2\
#2003jun5#	    configure configure.ac \
#2003jun5#            install-sh missing depcomp texinfo.tex mkinstalldirs \
#2003jun5#	    check.pl \
#2003jun5#	    copyright.txt AUTHORS INSTALL NEWS COPYING README* gri-mode.el gri_merge gri_unpage \
#2003jun5#	    startup.msg startup.debian startup.redhat gri*.spec grilogo.gif \
#2003jun5#	    install-SunOS5 \
#2003jun5#	        $(SOURCE_NAME)
#2003jun5#	(cd popt ; \
#2003jun5#	    install -m 644 popt.h findme.h poptint.h \
#2003jun5#	                   ../$(SOURCE_NAME)/popt )
#2003jun5#	(cd popt ; \
#2003jun5#	    install -m 644 popt.c findme.c poptparse.c poptconfig.c popthelp.c \
#2003jun5#	                   ../$(SOURCE_NAME)/popt )
#2003jun5#	install -m 755 configure install-sh $(SOURCE_NAME)
#2003jun5#	install -d $(SOURCE_NAME)/debian
#2003jun5#	(cd debian; install -m 644 0* READ* c* gri* ../$(SOURCE_NAME)/debian)
#2003jun5#	install -m 755 debian/rules                    $(SOURCE_NAME)/debian
#2003jun5##	Copy documentation
#2003jun5#	install -d $(SOURCE_NAME)/doc
#2003jun5#	install -m 755 ../doc/install-sh           $(SOURCE_NAME)/doc
#2003jun5#	install -m 755 ../doc/archive-to-html.pl   $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/cmdrefcard.tex       $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/refcard.tex          $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/Makefile.in          $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/FAQ.html             $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/gri-manpage*.1       $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/gri_merge.1          $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/gri_unpage.1         $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/gri.texi             $(SOURCE_NAME)/doc
#2003jun5#	install -m 755 ../doc/gri2texi             $(SOURCE_NAME)/doc
#2003jun5#	install -m 755 ../doc/texinfo2HTML         $(SOURCE_NAME)/doc
#2003jun5#	install -m 755 ../doc/gri2html             $(SOURCE_NAME)/doc
#2003jun5#	install -m 755 ../doc/HTML_subdivide       $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/make_html_index      $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/make_html_commandindex $(SOURCE_NAME)/doc
#2003jun5#	install -m 644 ../doc/make_html_builtinindex $(SOURCE_NAME)/doc
#2003jun5#	install -d $(SOURCE_NAME)/doc/resources
#2003jun5#	install -m 644 ../doc/resources/*.gri ../doc/resources/*.gif \
#2003jun5#		$(SOURCE_NAME)/doc/resources
#2003jun5#	install -d $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/*.gri          $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/*.txt          $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/*.dat          $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/*.ps           $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 755 ../doc/examples/FEM.pl         $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/model.elements $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/model.nodes    $(SOURCE_NAME)/doc/examples
#2003jun5#	install -m 644 ../doc/examples/Makefile       $(SOURCE_NAME)/doc/examples
#2003jun5#	install -d $(SOURCE_NAME)/../doc/screenshots
#2003jun5#	(cd doc/screenshots/; \
#2003jun5#	    install -m 644 Makefile *.png ../../$(SOURCE_NAME)/doc/screenshots)
#2003jun5#	install -d $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/Makefile        $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/tst_IO.gri      $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/tst_IO_1.dat    $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/tst_IO_2.dat    $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/tst_control.gri $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/tst_rpn.gri     $(SOURCE_NAME)/doc/tst_suite
#2003jun5#	install -m 644 ../doc/tst_suite/tst_var_syn.gri $(SOURCE_NAME)/doc/tst_suite
#2003jun5#
#2003jun5#
#2003jun5#source: source-copy
#2003jun5#	tar -c -f $(SOURCE_NAME).tar $(SOURCE_NAME)
#2003jun5#	rm -rf $(SOURCE_NAME)
#2003jun5#	gzip -f --best $(SOURCE_NAME).tar
#2003jun5#	mv $(SOURCE_NAME).tar.gz $(DEST_DIR)/$(SOURCE_NAME).tgz
#2003jun5#	chmod a+r $(DEST_DIR)/$(SOURCE_NAME).tgz
#2003jun5#
#2003jun5#source-arch-indep: source-copy
#2003jun5#	cd $(SOURCE_NAME)/doc; make refcard.ps cmdrefcard.ps info html
#2003jun5#	tar -c -f $(SOURCE_NAME)-arch-indep.tar $(SOURCE_NAME)
#2003jun5#	rm -rf $(SOURCE_NAME)
#2003jun5#	gzip -f --best $(SOURCE_NAME)-arch-indep.tar
#2003jun5#	-mv $(SOURCE_NAME)-arch-indep.tar.gz $(DEST_DIR)/$(SOURCE_NAME)-arch-indep.tgz
#2003jun5#	chmod a+r $(DEST_DIR)/$(SOURCE_NAME)-arch-indep.tgz

author_check:
	@perl $(srcdir)/check.pl

check:
	@./gri -directory $(srcdir)/. -c 0 $(srcdir)/../doc/tst_suite/tst_var_syn.gri 
	@./gri -directory $(srcdir)/. -c 0 $(srcdir)/../doc/tst_suite/tst_control.gri
	@./gri -directory $(srcdir)/. -c 0 $(srcdir)/../doc/tst_suite/tst_rpn.gri
# 	The next few lines are a kludge since I got bored with trying
# 	to figure out how to redirect things for automake.
	@cp $(srcdir)/../doc/tst_suite/tst_IO.gri   .
	@cp $(srcdir)/../doc/tst_suite/tst_IO_1.dat .
	@cp $(srcdir)/../doc/tst_suite/tst_IO_2.dat .
	@./gri -directory $(srcdir)/. -c 0 tst_IO.gri
	@rm -f tst_IO.gri tst_IO_1.dat tst_IO_2.dat
	@cat $(srcdir)/version.cc    | perl -ane 'if(/_number/)             {$$v=$$F[3]; $$v=~s/[^"]*"//; $$v=~s/".*//; die "ERROR: version number in version.cc is $$v, which disagrees with the version in the Makefile, $(PACKAGE_VERSION).  You MUST fix this.\n" if ("$$v" ne "$(PACKAGE_VERSION)"); print "Version number in version.cc       ... is OK\n";}'
	@cat $(srcdir)/gri.cmd       | perl -ane 'if(/\(version ([^)]*)\)/) {$$v=$$1; die "ERROR: Version in gri.cmd is $$v, which disagrees with the version in the Makefile, $(PACKAGE_VERSION).  You MUST fix this.\n" if ("$$v" ne "$(PACKAGE_VERSION)"); print "Version number in gri.cmd          ... is OK\n";}'
	@cat $(srcdir)/../doc/gri.texi  | perl -ane 'if(/GRI-VERSION (.*)/)    {$$v=$$1; die "ERROR: Version in ../doc/gri.texi is $$v, which disagrees with the version in the Makefile, $(PACKAGE_VERSION).  You MUST fix this.\n" if ("$$v" ne "$(PACKAGE_VERSION)"); print "Version number in ../doc/gri.texi    ... is OK\n";}'
	@cat $(srcdir)/../gri.spec      | perl -ane 'if(/griversion (.*)/)     {$$v=$$1; if ("$$v" ne "$(PACKAGE_VERSION)") {warn "ERROR: Version in gri.spec is $$v, which disagrees with the version in the Makefile, $(PACKAGE_VERSION).  You SHOULD fix this.\n"} else { print "Version number in gri.spec         ... is OK\n";}}'
	@cat $(srcdir)/../debian/changelog | perl -ne 'if (($$. == 1)&&(/\(([^-]+)-/)) {if ("$$1" ne "$(PACKAGE_VERSION)") {print "**WARNING: Version in debian/changelog is $$1,\n** which disagrees with the version in the Makefile, $(PACKAGE_VERSION).\n** You should fix this for official releases.\n"} else { print "Version number in debian/changelog ... is OK\n";}}'
	@echo ""
	@echo "    /-----------------------------------------------------\\"
	@echo "    | NOTE: Developers should do 'make author_check' next |"
	@echo "    \\----------------------------------------------------/"
	@echo ""

# Sun microsystems, sunOS 5.x
# NOTE: this target doesn't depend on 'gri' or 'doc', etc; this
# is to be done manually, the first step perhaps on an 
# entirely different computer!!
SUNOS5_NAME = gri-$(PACKAGE_VERSION)-SunOS5
package-SunOS5:
	-rm -rf          $(SUNOS5_NAME)
	-mkdir -m 755 -p $(SUNOS5_NAME)
	-mkdir -m 755 -p $(SUNOS5_NAME)/bin
	-mkdir -m 755 -p $(SUNOS5_NAME)/lib
	-mkdir -m 755 -p $(SUNOS5_NAME)/doc
	-mkdir -m 755 -p $(SUNOS5_NAME)/doc/html
	-mkdir -m 755 -p $(SUNOS5_NAME)/doc/html/resources
	-mkdir -m 755 -p $(SUNOS5_NAME)/doc/html/screenshots
	-mkdir -m 755 -p $(SUNOS5_NAME)/doc/html/tst_suite
	-mkdir -m 755 -p $(SUNOS5_NAME)/doc/html/examples
	-cp -p gri                                                        $(SUNOS5_NAME)/bin
	-cp  gri.cmd logo.dat copyright.txt license.txt gri-mode.el       $(SUNOS5_NAME)/lib
	-cat startup.msg | sed -e s,VSN,${PACKAGE_VERSION}, | sed -e "s/DATE/`date`/" > tmp
	-cp tmp                                                           $(SUNOS5_NAME)/lib/startup.msg
	-cat install-SunOS5 | sed -e s,VSN,${PACKAGE_VERSION}, > tmp
	-cp tmp                                                           $(SUNOS5_NAME)/install
	-chmod a+rx                                                       $(SUNOS5_NAME)/install
	-cp README-SunOS5                                                 $(SUNOS5_NAME)/README
	-cat ../doc/gri-manpage-SunOS5.1 | sed -e s,VERSION,${PACKAGE_VERSION}, >tmp
	-cp tmp                                                           $(SUNOS5_NAME)/doc/gri.1
	-rm -f tmp
#       Next should really be done by "cd doc; make something" etc.
	-cp ../doc/*html                                                     $(SUNOS5_NAME)/doc/html
	-cp ../doc/*png                                                      $(SUNOS5_NAME)/doc/html
	-cp ../doc/resources/*gif                                            $(SUNOS5_NAME)/doc/html/resources
	-cp ../doc/screenshots/*png                                          $(SUNOS5_NAME)/doc/html/screenshots
	-cp ../doc/tst_suite/*dat ../doc/tst_suite/*gri ../doc/tst_suite/*html     $(SUNOS5_NAME)/doc/html/tst_suite
	-cp ../doc/examples/*dat ../doc/examples/*gri ../doc/examples/*html        $(SUNOS5_NAME)/doc/html/examples
	-tar chof $(SUNOS5_NAME).tar $(SUNOS5_NAME)
	-rm -rf $(SUNOS5_NAME)
	-gzip $(SUNOS5_NAME).tar

# The '-local' part means that these are appended after the automake-created
# targets of names 'all', etc.
if OS_IS_LINUX_REDHAT
the_startup_file = $(srcdir)/startup.redhat
else
the_startup_file = $(srcdir)/startup.msg
endif

all-local:
	cat $(the_startup_file) | sed -e s,VSN,${PACKAGE_VERSION}, -e s,PREFIX,$(prefix), > startup.msg-tmp
install-exec-local:
	mkdir -m 755 -p                                  $(DESTDIR)$(prefix)/bin
	$(INSTALL_PROGRAM) $(srcdir)/gri_unpage          $(DESTDIR)$(prefix)/bin/gri_unpage
	$(INSTALL_PROGRAM) $(srcdir)/gri_unpage          $(DESTDIR)$(prefix)/bin/gri_merge
install-data-local:
	mkdir -m 755 -p                                  $(DESTDIR)$(prefix)/share/emacs/site-lisp
	$(INSTALL_DATA)    $(srcdir)/gri-mode.el         $(DESTDIR)$(prefix)/share/emacs/site-lisp/gri-mode.el
	mkdir -m 755 -p                                  $(DESTDIR)$(prefix)/share/gri
	$(INSTALL_DATA)    startup.msg-tmp               $(DESTDIR)$(prefix)/share/gri/startup.msg
uninstall-local:
	rm -f $(DESTDIR)$(prefix)/bin/gri_unpage
	rm -f $(DESTDIR)$(prefix)/bin/gri_merge
	rm -f $(DESTDIR)$(prefix)/share/emacs/site-lisp/gri-mode.el
	rm -f $(DESTDIR)$(prefix)/share/gri/startup.msg
	rm -f startup.msg-tmp
