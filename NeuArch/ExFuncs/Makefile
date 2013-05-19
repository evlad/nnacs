EXFUNCMODULES=deadzone.so \
	saturat.so \
	luft.so \
	sine.so \
	watertank.so \
	polynomial.so \
	cstr.so \
	randmeander.so \
	randunified.so \
	dummy.so

SYSLIBS	=-lm
LIBDEPS	=../libpnna.so ../../Matrix.041/libmatrix.a
LIBS	=-L .. -L ../../Matrix.041 -lpnna -lmatrix

SRCS	=deadzone.cpp \
	saturat.cpp \
	luft.cpp \
	sine.cpp \
	watertank.cpp \
	polynomial.cpp \
	cstr.cpp \
	randmeander.cpp \
	randunified.cpp \
	dummy.cpp

OBJS	=$(deadzone_OBJS) \
	$(saturat_OBJS) \
	$(luft_OBJS) \
	$(sine_OBJS) \
	$(watertank_OBJS) \
	$(polynomial_OBJS) \
	$(cstr_OBJS) \
	$(randmeander_OBJS) \
	$(randunified_OBJS) \
	$(dummy_OBJS)

LINK.so	=g++ -shared
CXXFLAGS=$(FLAGS) -fPIC -I../../Matrix.041 -I.. -I.

all:	$(EXFUNCMODULES)

install:	$(EXFUNCMODULES)
	install -d $(PREFIX)/lib
	install -t $(PREFIX)/lib $(EXFUNCMODULES)

deadzone_OBJS	=deadzone.o
deadzone.so:	$(deadzone_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(deadzone_OBJS) $(LIBS) $(SYSLIBS)

saturat_OBJS	=saturat.o
saturat.so:	$(saturat_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(saturat_OBJS) $(LIBS) $(SYSLIBS)

luft_OBJS	=luft.o
luft.so:	$(luft_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(luft_OBJS) $(LIBS) $(SYSLIBS)

sine_OBJS	=sine.o
sine.so:	$(sine_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(sine_OBJS) $(LIBS) $(SYSLIBS)

watertank_OBJS	=watertank.o
watertank.so:	$(watertank_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(watertank_OBJS) $(LIBS) $(SYSLIBS)

polynomial_OBJS	=polynomial.o
polynomial.so:	$(polynomial_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(polynomial_OBJS) $(LIBS) $(SYSLIBS)

cstr_OBJS	=cstr.o
cstr.so:	$(cstr_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(cstr_OBJS) $(LIBS) $(SYSLIBS)

randmeander_OBJS	=randmeander.o
randmeander.so:	$(randmeander_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(randmeander_OBJS) $(LIBS) $(SYSLIBS)

randunified_OBJS	=randunified.o
randunified.so:	$(randunified_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(randunified_OBJS) $(LIBS) $(SYSLIBS)

dummy_OBJS	=dummy.o
dummy.so:	$(dummy_OBJS) $(LIBDEPS)
	$(LINK.so) -o $@ $(dummy_OBJS) $(LIBS) $(SYSLIBS)

clean:
	rm -f $(EXFUNCMODULES) $(OBJS)

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

.PHONY:	all install clean

###
deadzone.o:	deadzone.h
saturat.o:	saturat.h
luft.o:	luft.h
sine.o:	sine.h
watertank.o:	watertank.h
polynomial.o:	polynomial.h
cstr.o:	cstr.h
randmeander.o:	randmeander.h
randunified.o:	randunified.h
dummy.o:	dummy.h
