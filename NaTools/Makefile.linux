PROGRAMS=UNGen MakeNN Distr2D Distr1D StatAn ResetNN FileCvt EvalNN TrainNN

all:	$(PROGRAMS)

install:	$(PROGRAMS)
	install -d $(PREFIX)/bin
	install -m 775 -t $(PREFIX)/bin $(PROGRAMS)

CCC	=g++

CXXFLAGS=$(FLAGS) -I../Matrix.041 -I../NeuArch

LIBDEPS	=../NeuArch/libpnna.so ../Matrix.041/libmatrix.a
LIBS	=-L ../NeuArch -L ../Matrix.041 -lpnna -lmatrix -ldl -lm

StatAn:	StatAn.o $(LIBDEPS)
	$(CCC) -o $@ $@.o $(LDFLAGS) $(LIBS)

Distr2D:	Distr2D.o $(LIBDEPS)
	$(CCC) -o $@ Distr2D.o $(LDFLAGS) $(LIBS)

Distr1D:	Distr1D.o $(LIBDEPS)
	$(CCC) -o $@ Distr1D.o $(LDFLAGS) $(LIBS)

MakeNN:	MakeNN.o $(LIBDEPS)
	$(CCC) -o $@ MakeNN.o $(LDFLAGS) $(LIBS)

ResetNN:	ResetNN.o $(LIBDEPS)
	$(CCC) -o $@ ResetNN.o $(LDFLAGS) $(LIBS)

EvalNN:	EvalNN.o $(LIBDEPS)
	$(CCC) -o $@ EvalNN.o $(LDFLAGS) $(LIBS)

TrainNN:	TrainNN.o $(LIBDEPS)
	$(CCC) -o $@ TrainNN.o $(LDFLAGS) $(LIBS)

UNGen:	UNGen.o $(LIBDEPS)
	$(CCC) -o $@ UNGen.o $(LDFLAGS) $(LIBS)

FileCvt:	FileCvt.o $(LIBDEPS)
	$(CCC) -o $@ FileCvt.o $(LDFLAGS) $(LIBS)

clean:
	-@rm -f *.o $(PROGRAMS)

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
