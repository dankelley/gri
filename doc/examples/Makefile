GRITEST = ../../gri -directory ../.. -y
GRI = gri -y -p

%.ps : %.gri
	$(GRI) $<

%.eps : %.ps
	cp $< $@

%.png : %.ps
	-convert $< $@

%.pdf : %.ps
	ps2pdf $< $@

%.html : %.gri
	perl ../gri2html `basename $< .gri`

%-tiny.png : %.png
	-convert -geometry 90x999 $< $@

example10color.eps : example10color.ps
	-convert example10color.ps example10color.eps

all:
	make eps
	make html
	make png

html:	FEM.html example1.html example2.html example3.html example4.html \
	example5.html example6.html example6color.html example6histogram.html \
	example7.html example8.html example9.html example10.html \
	example10color.html example11.html example12.html example13.html \
	logo.html

eps:	FEM.eps example1.eps example2.eps example3.eps example4.eps \
	example5.eps example6.eps example6color.eps example6histogram.eps \
	example7.eps example8.eps example9.eps example10.eps \
	example10color.eps example11.eps example12.eps example13.eps logo.eps

pdf:	FEM.pdf example1.pdf example2.pdf example3.pdf example4.pdf \
	example5.pdf example6.pdf example6color.pdf example6histogram.pdf \
	example7.pdf example8.pdf example9.pdf example10.pdf \
	example10color.pdf example11.pdf example12.pdf example13.pdf logo.pdf

png:	FEM.png example1.png example2.png example3.png example4.png \
	example5.png example6.png example6color.png example6histogram.png \
	example7.png example8.png example9.png example10.png \
	example10color.png example11.png example12.png example13.png \
	FEM-tiny.png example1-tiny.png example2-tiny.png example3-tiny.png \
	example4-tiny.png example5-tiny.png example6-tiny.png \
	example6color-tiny.png example6histogram-tiny.png example7-tiny.png \
	example8-tiny.png example9-tiny.png example10-tiny.png \
	example10color-tiny.png example11-tiny.png example12-tiny.png \
	example13-tiny.png logo.png

ps:	FEM.ps example1.ps example2.ps example3.ps example4.ps \
	example5.ps example6.ps example6color.ps example6histogram.ps \
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
	$(GRITEST) example6color.gri
	$(GRITEST) example6histogram.gri
	$(GRITEST) example7.gri
	$(GRITEST) example8.gri
	$(GRITEST) example9.gri
	$(GRITEST) example10.gri
	$(GRITEST) example10color.gri
	$(GRITEST) example11.gri
	$(GRITEST) example12.gri
	$(GRITEST) example13.gri

clean:
	rm -rf *.eps *.png *.html *.pdf *~ 
