TO_TEXI=perl ../gri2texi
texi:
	@$(TO_TEXI) tst_IO
	@$(TO_TEXI) tst_control
	@$(TO_TEXI) tst_rpn
	@$(TO_TEXI) tst_var_syn

TO_HTML=perl ../gri2html
html:
	@$(TO_HTML) tst_IO
	@$(TO_HTML) tst_control
	@$(TO_HTML) tst_rpn
	@$(TO_HTML) tst_var_syn


#tarfile:
#	rm -rf gre_tst_suite
#	mkdir gre_tst_suite
#	cp Makefile *.gre *.dat gre_tst_suite
#	tar -cof gre_tst_suite.tar gre_tst_suite
#	gzip gre_tst_suite.tar
#	mv gre_tst_suite.tar.gz gre_tst_suite.tgz
#	rm -rf gre_tst_suite

help:
	@echo "Things you can do here:"
	@grep '^[^ ]*:' Makefile		\
	 | sed -e "s/^/  make /" -e "s/://"	\
	 | gawk '{print " ", $$1, $$2}'

clean:
	rm -f	*.html *.ps *.texi
