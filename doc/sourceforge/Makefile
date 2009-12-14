TAB_IMAGES=news.png bugs.png developers.png contact.png docs.png download.png main.png
.SUFFIXES: .gri
%.ps: %.gri
	gri $<
	chmod a+r $@
%.png: %.ps
	convert $< $@
	make read
images: $(TAB_IMAGES)
read:
	chmod a+r *
clean:
	rm *~

install: force
	sh ./INSTALL

force:
