PROGRAMS	=dplantid \
		dnnplant

all:	$(PROGRAMS)

install:	$(PROGRAMS)
	install -d $(PREFIX)/bin
	install -m 775 -t $(PREFIX)/bin $(PROGRAMS)

CXXFLAGS=$(FLAGS) -I../Matrix.041 -I../NeuArch

LIBDEPS	=../NeuArch/libpnna.so ../Matrix.041/libmatrix.a
LIBS	=-L ../NeuArch -L ../Matrix.041 -lpnna -lmatrix


SRCS	=dplantid.cpp \
	dnnplant.cpp \
	NaNNRPL.cpp

OBJS	=$(common_OBJS) \
	$(dplantid_OBJS) \
	$(dnnplant_OBJS)

common_OBJS	=NaNNRPL.o

dplantid_OBJS	=dplantid.o

dnnplant_OBJS	=dnnplant.o

dplantid:	$(dplantid_OBJS) $(common_OBJS) $(LIBDEPS)
	g++ -o $@ $(dplantid_OBJS) $(common_OBJS) $(LIBS) -ldl -lm

dnnplant:	$(dnnplant_OBJS) $(common_OBJS) $(LIBDEPS)
	g++ -o $@ $(dnnplant_OBJS) $(common_OBJS) $(LIBS) -ldl -lm

clean:
	-@rm -f $(OBJS) $(PROGRAMS)

dplantid.o:	NaNNRPL.h
dnnplant.o:	NaNNRPL.h
NaNNRPL.o:	NaNNRPL.h

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
