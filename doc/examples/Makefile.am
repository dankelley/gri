## Process this file with automake to produce Makefile.in
# gri/doc/examples/
     srcdir = @srcdir@
     VPATH = @srcdir@

txt_files = example1.txt example2.txt example3.txt example4.txt example5.txt\
 example6histogram.txt example6.txt\
 example7.txt example8.txt example9.txt\
 example10color.txt example10.txt\
 example11.txt example12.txt example13.txt\
 FEM.txt
html_files = example1.html example2.html example3.html example4.html \
 example5.html example6.html example6histogram.html \
 example7.html example8.html example9.html example10.html \
 example10color.html example11.html example12.html example13.html \
 FEM.html\
 logo.html
eps_files =  example1.eps example2.eps example3.eps example4.eps example5.eps\
 example6.eps example6histogram.eps\
 example7.eps example8.eps example9.eps example10.eps\
 example10color.eps example11.eps example12.eps example13.eps\
 FEM.eps\
 logo.eps
png_files = FEM.png example1.png example2.png example3.png example4.png \
 example5.png example6.png example6histogram.png \
 example7.png example8.png example9.png example10.png \
 example10color.png example11.png example12.png example13.png \
 FEM-tiny.png example1-tiny.png example2-tiny.png example3-tiny.png \
 example4-tiny.png example5-tiny.png example6-tiny.png \
 example6histogram-tiny.png example7-tiny.png \
 example8-tiny.png example9-tiny.png example10-tiny.png \
 example10color-tiny.png example11-tiny.png example12-tiny.png \
 example13-tiny.png logo.png

EXTRA_DIST = \
 example1.txt example2.txt example3.txt example4.txt example5.txt\
 example6histogram.txt example6.txt\
 example7.txt example8.txt example9.txt\
 example10color.txt example10.txt\
 example11.txt example12.txt example13.txt  FEM.txt\
 example1.gri example2.gri example3.gri example4.gri example5.gri\
 example6.gri example6histogram.gri example7.gri example8.gri\
 example9.gri example10.gri example10color.gri example11.gri\
 example12.gri example13.gri\
 FEM.gri logo.gri\
 example1.dat example5.dat example6image.dat example6mask.dat\
 example7a.dat example7b.dat example7c.dat example7d.dat example7e.dat\
 example7f.dat example7g.dat \
 example8a.dat example8b.dat \
 example9a.dat example9b.dat\
 example10.dat example11.dat\
 example12.dat example13.dat\
 FEM.pl \
 model.elements model.nodes\
 example1.ps example2.ps example3.ps example4.ps example5.ps \
 example6.ps example6histogram.ps example7.ps example8.ps \
 example9.ps example10.ps example10color.ps example11.ps \
 example12.ps example13.ps \
 logo.ps FEM.ps

DISTCLEANFILES = $(png_files) $(eps_files) $(html_files) $(txt_files)

%.txt : %.gri
	echo "$<" | cat >$@
	chmod +w $@
%.eps : %.ps
	cp $< $@
	chmod +w $@
%.png : %.ps
	-convert $< $@
%-tiny.png : %.png
	-convert -geometry 90x999 $< $@
%.pdf : %.ps
	convert $< $@ 
#	ps2pdf $< $@
%.html : %.gri
	perl $(srcdir)/../gri2html $< $@
all: html png eps txt
html: $(html_files)
eps: $(eps_files)
png: $(png_files)
txt: $(txt_files)

# Everything below this line is old, or is provide
# for the developer's use only.
GRITEST = ../../gri -directory ../.. -y
GRI = gri -y -p
#%.ps : %.gri
#	$(GRI) $<

old_all_target:
	${MAKE} eps
	${MAKE} html
	${MAKE} png

pdf: FEM.pdf example1.pdf example2.pdf example3.pdf example4.pdf \
 example5.pdf example6.pdf example6histogram.pdf \
 example7.pdf example8.pdf example9.pdf example10.pdf \
 example10color.pdf example11.pdf example12.pdf example13.pdf logo.pdf

ps: FEM.ps example1.ps example2.ps example3.ps example4.ps \
 example5.ps example6.ps example6histogram.ps \
 example7.ps example8.ps example9.ps example10.ps \
 example10color.ps example11.ps example12.ps example13.ps logo.ps

testps:
	$(GRITEST) FEM.gri
	$(GRITEST) example1.gri
	$(GRITEST) example2.gri
	$(GRITEST) example3.gri
	$(GRITEST) example4.gri
	$(GRITEST) example5.gri
	$(GRITEST) example6.gri
	$(GRITEST) example6histogram.gri
	$(GRITEST) example7.gri
	$(GRITEST) example8.gri
	$(GRITEST) example9.gri
	$(GRITEST) example10.gri
	$(GRITEST) example10color.gri
	$(GRITEST) example11.gri
	$(GRITEST) example12.gri
	$(GRITEST) example13.gri
