all:
	make eps
	make png

png:	gri-screenshot-1-tiny.png gri-screenshot-2-tiny.png \
	gri-screenshot-3-tiny.png gri-screenshot-4-tiny.png

eps:	gri-screenshot-1.eps gri-screenshot-2.eps gri-screenshot-3.eps \
	gri-screenshot-4.eps

pdf:	gri-screenshot-1.pdf gri-screenshot-2.pdf gri-screenshot-3.pdf \
	gri-screenshot-4.pdf


%.pgm : %.png
	-convert  -geometry 500x999 $< $@

%.eps : %.pgm
	-convert $< $@

%.pdf : %.eps
	ps2pdf $< $@

# This was good for color; 
# now let's put them in grayscale for the PostScript manual
#%.eps : %.png
#	-convert  -geometry 500x999 $< eps2:$@

%-tiny.png : %.png
	-convert -geometry 999x120 $< $@

clean:
	rm -f *-tiny.png *.eps *.pgm *.pdf *~ 
