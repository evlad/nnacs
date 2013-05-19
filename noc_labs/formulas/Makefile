all:
	rm -rf formulas
	latex2html formulas.tex
	awk '$$1=="%%file:" {printf "convert formulas/%s.png ../templates/%s.gif\n", $$2, $$3}' formulas.tex | bash
