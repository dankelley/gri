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
DEFS = -DDEFAULT_GRI_DIR=\"$(prefix)/share/gri/\" @DEFS@

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

# This seems not to be used anymore [Dan Kelley]
#Sat May 31 14:05:06 UTC 2003#install-bin:
#Sat May 31 14:05:06 UTC 2003#	cat startup.msg | sed -e s,VSN,${PACKAGE_VERSION}, -e s,PREFIX,$(prefix), > tmp
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_PROGRAM) -d $(libdir)
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_DATA) tmp $(libdir)/startup.msg
#Sat May 31 14:05:06 UTC 2003#	rm -f tmp
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_DATA)         gri.cmd         $(libdir)/gri.cmd
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_DATA)         logo.dat        $(libdir)/logo.dat
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_DATA)         license.txt     $(libdir)/license.txt
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_PROGRAM) -d                   $(prefix)/bin
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_PROGRAM)      gri             $(prefix)/bin/gri-$(PACKAGE_VERSION)
#Sat May 31 14:05:06 UTC 2003#	(cd $(prefix)/bin/ ; rm -f gri ; ln -sf gri-$(PACKAGE_VERSION) gri)
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_PROGRAM)      gri_merge       $(prefix)/bin/gri_merge
#Sat May 31 14:05:06 UTC 2003#	$(INSTALL_PROGRAM)      gri_unpage      $(prefix)/bin/gri_unpage

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

#Sat May 31 22:16:58 UTC 2003## This target is no longer needed, but keep it as a 
#Sat May 31 22:16:58 UTC 2003## commented-out chunk, until the whole automake
#Sat May 31 22:16:58 UTC 2003## conversion is completed.  [Dan Kelley]
#Sat May 31 22:16:58 UTC 2003#install_linux_redhat:
#Sat May 31 22:16:58 UTC 2003##	Create the needed dirs in the BuildRoot (this is what $(DESTDIR) is being
#Sat May 31 22:16:58 UTC 2003##	used for)
#Sat May 31 22:16:58 UTC 2003#	mkdir -m 755 -p $(DESTDIR)$(prefix)/bin
#Sat May 31 22:16:58 UTC 2003#	mkdir -m 755 -p $(DESTDIR)$(prefix)/share/gri
#Sat May 31 22:16:58 UTC 2003#	mkdir -m 755 -p $(DESTDIR)$(prefix)/share/emacs/site-lisp
#Sat May 31 22:16:58 UTC 2003#	mkdir -m 755 -p $(DESTDIR)$(prefix)/share/man/man1
#Sat May 31 22:16:58 UTC 2003#	mkdir -m 755 -p $(DESTDIR)$(prefix)/share/info
#Sat May 31 22:16:58 UTC 2003#	if test -f startup.redhat; then cat startup.redhat | sed -e s,VSN,${PACKAGE_VERSION}, > tmp; $(INSTALL_DATA) tmp $(DESTDIR)$(prefix)/share/gri/startup.msg; rm -f tmp; else echo "WARNING: no startup.redhat file!"; fi;
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 755  gri                           $(DESTDIR)$(prefix)/bin/gri
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 755  gri_merge                     $(DESTDIR)$(prefix)/bin/gri_merge
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 755  gri_unpage                    $(DESTDIR)$(prefix)/bin/gri_unpage
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  gri.cmd                       $(DESTDIR)$(prefix)/share/gri/gri.cmd
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  logo.dat                      $(DESTDIR)$(prefix)/share/gri/logo.dat
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  $(srcdir)/../license.txt      $(DESTDIR)$(prefix)/share/gri/license.txt
#Sat May 31 22:16:58 UTC 2003#	sed -e "s,WWW-page \"http://gri.sourceforge.net/gridoc/html/index.html\",WWW-page \"file:$(datadir)/doc/gri-$(PACKAGE_VERSION)/html/index.html\",;s,gri\*directory-tree \"/usr/local/share/gri\",gri*directory-tree \"/usr/share/gri\"," gri-mode.el > tmp
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  tmp                           $(DESTDIR)$(prefix)/share/emacs/site-lisp/gri-mode.el
#Sat May 31 22:16:58 UTC 2003#	-rm -f tmp
#Sat May 31 22:16:58 UTC 2003#	cat ../doc/gri-manpage-redhat.1 | sed -e s,VERSION,${PACKAGE_VERSION}, > tmp
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  tmp                           $(DESTDIR)$(prefix)/share/man/man1/gri.1
#Sat May 31 22:16:58 UTC 2003#	rm -f tmp
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  ../doc/gri_merge.1            $(DESTDIR)$(prefix)/share/man/man1/gri_merge.1
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  ../doc/gri_unpage.1           $(DESTDIR)$(prefix)/share/man/man1/gri_unpage.1
#Sat May 31 22:16:58 UTC 2003#	-rm -f $(DESTDIR)$(prefix)/share/info/gri.info*
#Sat May 31 22:16:58 UTC 2003#	/usr/bin/install -m 644  ../doc/gri.info*              $(DESTDIR)$(prefix)/share/info
#Sat May 31 22:16:58 UTC 2003#	cd $(DESTDIR)$(prefix)/share/info ; gzip -f --best gri.info*


# NOTE: these "source" targets may be candidates for removal, since
# they are covered by "dist" target [Dan Kelley]
# Source distribution
SOURCE_NAME = gri-$(PACKAGE_VERSION)
DEST_DIR = .
source-copy:
#	Clean directory, if existent.
	-rm -rf $(SOURCE_NAME)
	install -d $(SOURCE_NAME)
	install -d $(SOURCE_NAME)/popt
#	Copy source
	install -m 644 ChangeLog $(gri_SOURCES) *.hh gri.cmd logo.dat license.txt \
	    Makefile.am Makefile.in Makefile.dj2\
	    configure configure.ac \
            install-sh missing depcomp texinfo.tex mkinstalldirs \
	    check.pl \
	    copyright.txt AUTHORS INSTALL NEWS COPYING README* gri-mode.el gri_merge gri_unpage \
	    startup.msg startup.debian startup.redhat gri*.spec grilogo.gif \
	    install-SunOS5 \
	        $(SOURCE_NAME)
	(cd popt ; \
	    install -m 644 popt.h findme.h poptint.h \
	                   ../$(SOURCE_NAME)/popt )
	(cd popt ; \
	    install -m 644 popt.c findme.c poptparse.c poptconfig.c popthelp.c \
	                   ../$(SOURCE_NAME)/popt )
	install -m 755 configure install-sh $(SOURCE_NAME)
	install -d $(SOURCE_NAME)/debian
	(cd debian; install -m 644 0* READ* c* gri* ../$(SOURCE_NAME)/debian)
	install -m 755 debian/rules                    $(SOURCE_NAME)/debian
#	Copy documentation
	install -d $(SOURCE_NAME)/doc
	install -m 755 ../doc/install-sh           $(SOURCE_NAME)/doc
	install -m 755 ../doc/archive-to-html.pl   $(SOURCE_NAME)/doc
	install -m 644 ../doc/cmdrefcard.tex       $(SOURCE_NAME)/doc
	install -m 644 ../doc/refcard.tex          $(SOURCE_NAME)/doc
	install -m 644 ../doc/Makefile.in          $(SOURCE_NAME)/doc
	install -m 644 ../doc/FAQ.html             $(SOURCE_NAME)/doc
	install -m 644 ../doc/gri-manpage*.1       $(SOURCE_NAME)/doc
	install -m 644 ../doc/gri_merge.1          $(SOURCE_NAME)/doc
	install -m 644 ../doc/gri_unpage.1         $(SOURCE_NAME)/doc
	install -m 644 ../doc/gri.texi             $(SOURCE_NAME)/doc
	install -m 755 ../doc/gri2texi             $(SOURCE_NAME)/doc
	install -m 755 ../doc/texinfo2HTML         $(SOURCE_NAME)/doc
	install -m 755 ../doc/gri2html             $(SOURCE_NAME)/doc
	install -m 755 ../doc/HTML_subdivide       $(SOURCE_NAME)/doc
	install -m 644 ../doc/make_html_index      $(SOURCE_NAME)/doc
	install -m 644 ../doc/make_html_commandindex $(SOURCE_NAME)/doc
	install -m 644 ../doc/make_html_builtinindex $(SOURCE_NAME)/doc
	install -d $(SOURCE_NAME)/doc/resources
	install -m 644 ../doc/resources/*.gri ../doc/resources/*.gif \
		$(SOURCE_NAME)/doc/resources
	install -d $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/*.gri          $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/*.txt          $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/*.dat          $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/*.ps           $(SOURCE_NAME)/doc/examples
	install -m 755 ../doc/examples/FEM.pl         $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/model.elements $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/model.nodes    $(SOURCE_NAME)/doc/examples
	install -m 644 ../doc/examples/Makefile       $(SOURCE_NAME)/doc/examples
	install -d $(SOURCE_NAME)/../doc/screenshots
	(cd doc/screenshots/; \
	    install -m 644 Makefile *.png ../../$(SOURCE_NAME)/doc/screenshots)
	install -d $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/Makefile        $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/tst_IO.gri      $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/tst_IO_1.dat    $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/tst_IO_2.dat    $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/tst_control.gri $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/tst_rpn.gri     $(SOURCE_NAME)/doc/tst_suite
	install -m 644 ../doc/tst_suite/tst_var_syn.gri $(SOURCE_NAME)/doc/tst_suite

source: source-copy
	tar -c -f $(SOURCE_NAME).tar $(SOURCE_NAME)
	rm -rf $(SOURCE_NAME)
	gzip -f --best $(SOURCE_NAME).tar
	mv $(SOURCE_NAME).tar.gz $(DEST_DIR)/$(SOURCE_NAME).tgz
	chmod a+r $(DEST_DIR)/$(SOURCE_NAME).tgz

source-arch-indep: source-copy
	cd $(SOURCE_NAME)/doc; make refcard.ps cmdrefcard.ps info html
	tar -c -f $(SOURCE_NAME)-arch-indep.tar $(SOURCE_NAME)
	rm -rf $(SOURCE_NAME)
	gzip -f --best $(SOURCE_NAME)-arch-indep.tar
	-mv $(SOURCE_NAME)-arch-indep.tar.gz $(DEST_DIR)/$(SOURCE_NAME)-arch-indep.tgz
	chmod a+r $(DEST_DIR)/$(SOURCE_NAME)-arch-indep.tgz

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
	@cat $(srcdir)/doc/gri.texi  | perl -ane 'if(/GRI-VERSION (.*)/)    {$$v=$$1; die "ERROR: Version in ../doc/gri.texi is $$v, which disagrees with the version in the Makefile, $(PACKAGE_VERSION).  You MUST fix this.\n" if ("$$v" ne "$(PACKAGE_VERSION)"); print "Version number in ../doc/gri.texi    ... is OK\n";}'
	@cat $(srcdir)/gri.spec      | perl -ane 'if(/griversion (.*)/)     {$$v=$$1; if ("$$v" ne "$(PACKAGE_VERSION)") {warn "ERROR: Version in gri.spec is $$v, which disagrees with the version in the Makefile, $(PACKAGE_VERSION).  You SHOULD fix this.\n"} else { print "Version number in gri.spec         ... is OK\n";}}'
	@cat $(srcdir)/debian/changelog | perl -ne 'if (($$. == 1)&&(/\(([^-]+)-/)) {if ("$$1" ne "$(PACKAGE_VERSION)") {print "**WARNING: Version in debian/changelog is $$1,\n** which disagrees with the version in the Makefile, $(PACKAGE_VERSION).\n** You should fix this for official releases.\n"} else { print "Version number in debian/changelog ... is OK\n";}}'
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
