## Process this file with automake to produce Makefile.in
# gri: doc/tst_suite/Makefile.am
     srcdir = @srcdir@
     VPATH = @srcdir@
the_texi = tst_IO.texi tst_control.texi tst_rpn.texi tst_var_syn.texi
the_html = tst_IO.html tst_control.html tst_rpn.html tst_var_syn.html
EXTRA_DIST =  tst_IO.gri tst_IO_1.dat tst_IO_2.dat tst_control.gri\
 tst_rpn.gri tst_var_syn.gri $(the_texi)
DISTCLEANFILES = $(the_html)
%.texi : %.gri
	perl $(srcdir)/../gri2texi $< $@
%.html : %.gri
	perl $(srcdir)/../gri2html $< $@

all: texi html
texi: $(the_texi)
html: $(the_html)
