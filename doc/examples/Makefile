GRITEST = ../../gri -directory ../.. -y
GRI = gri -y
eps:
	cp FEM.ps FEM.eps
	cp example1.ps example1.eps
	cp example2.ps example2.eps
	cp example3.ps example3.eps
	cp example4.ps example4.eps
	cp example5.ps example5.eps
	cp example6.ps example6.eps
	cp example6color.ps example6color.eps
	cp example6histogram.ps example6histogram.eps
	cp example7.ps example7.eps
	cp example8.ps example8.eps
	cp example9.ps example9.eps
	cp example10.ps example10.eps
	cp example10color.ps example10color.eps
	cp example11.ps example11.eps
	cp example12.ps example12.eps
	cp example13.ps example13.eps

ps:
	$(GRI) FEM.gri
	$(GRI) example1.gri
	$(GRI) example2.gri
	$(GRI) example3.gri
	$(GRI) example4.gri
	$(GRI) example5.gri
	$(GRI) example6.gri
	$(GRI) example6color.gri
	$(GRI) example6histogram.gri
	$(GRI) example7.gri
	$(GRI) example8.gri
	$(GRI) example9.gri
	$(GRI) example10.gri
	$(GRI) example10color.gri
	$(GRI) example11.gri
	$(GRI) example12.gri
	$(GRI) example13.gri
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
	rm -rf *.eps *~
