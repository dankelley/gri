GRITEST = ../../gri -directory ../.. -y
GRI = gri -y -p

%.ps : %.gri
	$(GRI) $<

%.eps : %.ps
	cp $< $@

%.jpg : %.ps
	convert $< $@

%.html : %.gri
	../gri2html $(basename $<)

%-tiny.jpg : %.jpg
	convert -geometry 90x999 $< $@

example8.jpg : example8.ps
	convert -rotate 90 example8.ps example8.jpg

example9.jpg : example9.ps
	convert -rotate 90 example9.ps example9.jpg

example8-tiny.jpg : example8.ps
	convert -geometry 90x999 -rotate 90 example8.ps example8-tiny.jpg

example9-tiny.jpg : example9.ps
	convert -geometry 90x999 -rotate 90 example9.ps example9-tiny.jpg

example10color.eps : example10color.ps
	-convert example10color.ps example10color.eps

all:
	make eps
	make html
	make jpg

html:	FEM.html example1.html example2.html example3.html example4.html \
	example5.html example6.html example6color.html example6histogram.html \
	example7.html example8.html example9.html example10.html \
	example10color.html example11.html example12.html example13.html \
	logo.html

eps:	FEM.eps example1.eps example2.eps example3.eps example4.eps \
	example5.eps example6.eps example6color.eps example6histogram.eps \
	example7.eps example8.eps example9.eps example10.eps \
	example10color.eps example11.eps example12.eps example13.eps logo.eps

jpg:	FEM.jpg example1.jpg example2.jpg example3.jpg example4.jpg \
	example5.jpg example6.jpg example6color.jpg example6histogram.jpg \
	example7.jpg example8.jpg example9.jpg example10.jpg \
	example10color.jpg example11.jpg example12.jpg example13.jpg \
	FEM-tiny.jpg example1-tiny.jpg example2-tiny.jpg example3-tiny.jpg \
	example4-tiny.jpg example5-tiny.jpg example6-tiny.jpg \
	example6color-tiny.jpg example6histogram-tiny.jpg example7-tiny.jpg \
	example8-tiny.jpg example9-tiny.jpg example10-tiny.jpg \
	example10color-tiny.jpg example11-tiny.jpg example12-tiny.jpg \
	example13-tiny.jpg logo.jpg

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
	rm -rf *.eps *.jpg *.html *~ 
