# Gri cookbook Makefile
VERSION = 0.1.4

%.html : %.gri
	./gri2html $(basename $<)

%.gif : %.ps
	convert $< $@

%-small.gif : %.gif
	convert -geometry 999x200 $< $@

all:	1/example.gif 1/example-small.gif 1/example.html \
	2/example.gif 2/example-small.gif 2/example.html \
	3/example.gif 3/example-small.gif 3/example.html \
	4/example.gif 4/example-small.gif 4/example.html \
	5/example.gif 5/example-small.gif 5/example.html \
	6/example.gif 6/example-small.gif 6/example.html \
	7/example.gif 7/example-small.gif 7/example.html \
	8/example.gif 8/example-small.gif 8/example.html \
	9/example.gif 9/example.html \
	10/example.gif 10/example.html \
	11/example.gif 11/example-small.gif 11/example.html \
	12/example.gif 12/example-small.gif 12/example.html \
	13/example.gif 13/example-small.gif 13/example.html \
	14/example.gif 14/example-small.gif 14/example.html 


# Override rule for specific case
14/example.gif:
	convert -rotate 90 14/example.ps 14/example.gif

HTMLDIR=gri-cookbook-$(VERSION)
html-tar:
	rm -rf $(HTMLDIR)
	make install_html_tree TARGETDIR=$(HTMLDIR)
	tar -cf gri-cookbook-$(VERSION).tar $(HTMLDIR)
	gzip -f --best gri-cookbook-$(VERSION).tar
	rm -rf $(HTMLDIR)

DIR_redhat=/usr/doc/gri-cookbook-$(VERSION)
install_linux_redhat:
	make install_html_tree TARGETDIR=$(DIR_redhat)

uninstall_linux_redhat:
	rm -rf $(DIR_redhat)

clean:
	-rm -f *.lof *.dvi *.idx *.ilg *.ind *.log *.toc *.aux *.bbl *.blg cookbook.ps *~ */example-small.gif */example.gif */example.html 12/example-vertical.html

TARDIR=gri-cookbook-src-$(VERSION)
source:
	rm -rf $(TARDIR)
	install -d -m 755 $(TARDIR)/{1,2,3,4,5,6,7,8,9,10,11,12,13,14,debian}
	cp *.spec Makefile gri2html cookbook.html discussion.html index.html recipes.html submitting.html $(TARDIR)
	(cd debian; cp README.Debian changelog control copyright doc-base menu.ex rules ../$(TARDIR)/debian)
	cp -P */example* 4/*db 12/seawifs.palette $(TARDIR)
	chmod -R u+rw $(TARDIR)
	tar -cf gri-cookbook-src-$(VERSION).tar $(TARDIR)
	gzip --best gri-cookbook-src-$(VERSION).tar
	rm -rf $(TARDIR)

# This target is not usually called directly.
# Variable $(TARGETDIR) must be specified!
TARGETDIR?= /tmp/safetynet
install_html_tree:	all
	@echo "Installing HTML tree to $(TARGETDIR)"
	install -d -m 755 $(TARGETDIR)/{1,2,3,4,5,6,7,8,9,10,11,12,13,14}
	cp -P *.html 12/seawifs.palette */*.gif */*.html $(TARGETDIR)
	chmod -R u+rw $(TARGETDIR)

help:
	@echo "Various \"make\" commands can be used with this Makefile."
	@echo " make all      : Create all .html and .gif files."
	@echo " make html-tar : Create gri-cookbook-$(VERSION).tar archive."
	@echo "                 suitable for end users (drop-in HTML tree)."
	@echo " make source   : Create gri-cookbook-src-$(VERSION).tar archive"
	@echo "                 suitable for backup for developers."
	@echo " make clean    : Delete all created files."
	@echo " make install_linux_redhat : Create Red Hat directory tree for"
	@echo "                 use in RPM package creation."
#	@echo "The possibilities are as follows."
#	@echo
#	@grep ":" Makefile | awk '{print($$1)}' | sed -e "s/://g" -e "s/^/make /" | sort | uniq


# following is broken!
cookbook:
	latex cookbook
	makeindex cookbook
	latex cookbook
	dvips cookbook.dvi > cookbook.ps


