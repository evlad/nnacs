PROGRAM	=dcontrf

all:	$(PROGRAM) NaNNOCL2.o

install:	$(PROGRAM)
	install -d $(PREFIX)/bin
	install -m 775 -t $(PREFIX)/bin $(PROGRAM)

CXXFLAGS=$(FLAGS) -I../Matrix.041 -I../NeuArch

LIBDEPS	=../NeuArch/libpnna.so ../Matrix.041/libmatrix.a
LIBS	=-L ../NeuArch -L ../Matrix.041 -lpnna -lmatrix


SRCS	=dcontrf.cpp \
	NaNNOCL.cpp \
	NaNNOCL2.cpp

OBJS	=dcontrf.o \
	NaNNOCL.o

NaNNOCL2.o: NaNNOCL2.h

$(PROGRAM):	$(OBJS) $(LIBDEPS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS) -ldl -lm

clean:
	-@rm -f $(OBJS) $(PROGRAM)

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
