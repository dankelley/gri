the_texi = tst_IO.texi tst_control.texi tst_rpn.texi tst_var_syn.texi
the_html = tst_IO.html tst_control.html tst_rpn.html tst_var_syn.html

TO_TEXI=perl ../gri2texi
%.texi : %.gri
	perl ../gri2texi $< $@
%.html : %.gri
	perl ../gri2html $< $@
texi: $(the_texi)

html: $(the_html)

