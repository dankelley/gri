## Process this file with automake to produce Makefile.in
# gri: doc/screenshots/Makefile.am

EXTRA_DIST =  gri-screenshot-1.png  gri-screenshot-1-tiny.png\
 gri-screenshot-2.png  gri-screenshot-2-tiny.png\
 gri-screenshot-3.png  gri-screenshot-3-tiny.png\
 gri-screenshot-4.png  gri-screenshot-4-tiny.png

the_eps_files = gri-screenshot-1.eps gri-screenshot-2.eps gri-screenshot-3.eps gri-screenshot-4.eps
the_png_files = gri-screenshot-1-tiny.png gri-screenshot-2-tiny.png gri-screenshot-3-tiny.png gri-screenshot-4-tiny.png
the_pdf_files = gri-screenshot-1.pdf gri-screenshot-2.pdf gri-screenshot-3.pdf gri-screenshot-4.pdf

DISTCLEANFILES = $(the_eps_files)

all: eps png
png: $(the_png_files)
eps: $(the_eps_files)

pdf: $(the_pdf_files)


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

# OLD STUFF, PROBABLY SHOULD BE REMOVED.
old_clean:
	rm -f *-tiny.png *.eps *.pgm *.pdf *~ 
