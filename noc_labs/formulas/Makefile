all:
	rm -rf formulas *.gif
	latex2html formulas.tex
	awk '$$1=="%%file:" {printf "convert formulas/%s.png %s.gif\n", $$2, $$3}' formulas.tex | bash
	for i in *.gif ; do \
	  newf=../templates/$$i ; \
	  newg=`identify $$i | awk '{print $$3}' | awk -Fx '{printf "%dx%d+0+0", $$1, $$2-1}'` ; \
	  convert -crop $$newg -trim +repage $$i -bordercolor none -border 5 $$newf ; \
	done
