all:
	make eps
	make jpg

eps:	gri-screenshot-1.eps gri-screenshot-2.eps gri-screenshot-3.eps \
	gri-screenshot-4.eps

jpg:	gri-screenshot-1.jpg gri-screenshot-2.jpg gri-screenshot-3.jpg \
	gri-screenshot-4.jpg \
	gri-screenshot-1-tiny.jpg gri-screenshot-2-tiny.jpg \
	gri-screenshot-3-tiny.jpg gri-screenshot-4-tiny.jpg

%.pgm : %.png
	convert  -geometry 500x999 $< $@

%.eps : %.pgm
	convert $< $@

# This was good for color; 
# now let's put them in grayscale for the PostScript manual
#%.eps : %.png
#	convert  -geometry 500x999 $< eps2:$@

%.jpg : %.png
	convert $< $@

%-tiny.jpg : %.jpg
	convert -geometry 999x120 $< $@

clean:
	rm -f *.eps *.jpg *.pgm *~ 
