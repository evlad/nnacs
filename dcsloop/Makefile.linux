PROGRAM	=dcsloop

all:	$(PROGRAM)

install:	$(PROGRAM)
	install -d $(PREFIX)/bin
	install -m 775 -t $(PREFIX)/bin $(PROGRAM)

CXXFLAGS=$(FLAGS) -I../Matrix.041 -I../NeuArch

LIBDEPS	=../NeuArch/libpnna.so ../Matrix.041/libmatrix.a
LIBS	=-L ../NeuArch -L ../Matrix.041 -lpnna -lmatrix


SRCS	=dcsloop.cpp \
	NaCSM.cpp

OBJS	=dcsloop.o \
	NaCSM.o

$(PROGRAM):	$(OBJS) $(LIBDEPS)
	g++ -o $@ $(OBJS) $(LIBS) -ldl -lm

clean:
	-@rm -f $(OBJS) $(PROGRAM)

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
