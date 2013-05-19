PROGRAMS	=dtf d2ts drand dwtest dwtest2 dmse dcorr dsum dstat djacob \
		dsin dmean dstddev dsmooth dconst dmult drandmea dsub dstatsb \
		dsteps dranduni dmeander

SCRIPTS		=

LD	=g++

all:	$(PROGRAMS)

install:	$(SCRIPTS) $(PROGRAMS)
	install -d $(PREFIX)/bin
	install -m 775 -t $(PREFIX)/bin $(SCRIPTS) $(PROGRAMS)

CXXFLAGS=$(FLAGS) -I../Matrix.041 -I../NeuArch

SYSLIBS	=-ldl -lm
LIBDEPS	=../NeuArch/libpnna.so ../Matrix.041/libmatrix.a
LIBS	=-L ../NeuArch -L ../Matrix.041 -lpnna -lmatrix

SRCS	=dtf.cpp \
	d2ts.cpp \
	drand.cpp \
	dwtest.cpp \
	dwtest2.cpp \
	dmse.cpp \
	dcorr.cpp \
	dsum.cpp \
	dsub.cpp \
	dstat.cpp \
	dstatsb.cpp \
	djacob.cpp \
	dstddev.cpp \
	dsin.cpp \
	dmean.cpp \
	dconst.cpp \
	dsmooth.cpp \
	dmult.cpp \
	drandmea.cpp \
	dranduni.cpp \
	dmeander.cpp \
	dsteps.cpp

OBJS	=$(dtf_OBJS) $(d2ts_OBJS) $(drand_OBJS) $(dwtest_OBJS) \
	$(dmse_OBJS) $(dcorr_OBJS) $(dsum_OBJS) $(dsub_OBJS) $(dstat_OBJS) \
	$(djacob_OBJS) $(dsin_OBJS) $(dmean_OBJS) $(dstddev_OBJS) \
	$(dconst_OBJS) $(dsmooth_OBJS) $(dmult_OBJS) $(drandmea_OBJS) \
	$(dstatsb_OBJS)  $(dsteps_OBJS) $(dranduni_OBJS) $(dmeander_OBJS)

dtf_OBJS	=dtf.o
dtf:	$(dtf_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dtf_OBJS) $(LIBS) $(SYSLIBS)

d2ts_OBJS	=d2ts.o
d2ts:	$(d2ts_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(d2ts_OBJS) $(LIBS) $(SYSLIBS)

drand_OBJS	=drand.o
drand:	$(drand_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(drand_OBJS) $(LIBS) $(SYSLIBS)

dwtest_OBJS	=dwtest.o
dwtest:	$(dwtest_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dwtest_OBJS) $(LIBS) $(SYSLIBS)

dwtest2_OBJS	=dwtest2.o
dwtest2:	$(dwtest2_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dwtest2_OBJS) $(LIBS) $(SYSLIBS)

dmse_OBJS	=dmse.o
dmse:	$(dmse_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dmse_OBJS) $(LIBS) $(SYSLIBS)

dcorr_OBJS	=dcorr.o
dcorr:	$(dcorr_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dcorr_OBJS) $(LIBS) $(SYSLIBS)

dsum_OBJS	=dsum.o
dsum:	$(dsum_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dsum_OBJS) $(LIBS) $(SYSLIBS)

dsub_OBJS	=dsub.o
dsub:	$(dsub_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dsub_OBJS) $(LIBS) $(SYSLIBS)

dstat_OBJS	=dstat.o
dstat:	$(dstat_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dstat_OBJS) $(LIBS) $(SYSLIBS)

dstatsb_OBJS	=dstatsb.o
dstatsb:	$(dstatsb_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dstatsb_OBJS) $(LIBS) $(SYSLIBS)

djacob_OBJS	=djacob.o
djacob:	$(djacob_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(djacob_OBJS) $(LIBS) $(SYSLIBS)

dsin_OBJS	=dsin.o
dsin:	$(dsin_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dsin_OBJS) $(LIBS) $(SYSLIBS)

dmean_OBJS	=dmean.o
dmean:	$(dmean_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dmean_OBJS) $(LIBS) $(SYSLIBS)

dstddev_OBJS	=dstddev.o
dstddev:	$(dstddev_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dstddev_OBJS) $(LIBS) $(SYSLIBS)

dconst_OBJS	=dconst.o
dconst:	$(dconst_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dconst_OBJS) $(LIBS) $(SYSLIBS)

dsmooth_OBJS	=dsmooth.o
dsmooth:	$(dsmooth_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dsmooth_OBJS) $(LIBS) $(SYSLIBS)

dmult_OBJS	=dmult.o
dmult:		$(dmult_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dmult_OBJS) $(LIBS) $(SYSLIBS)

drandmea_OBJS	=drandmea.o
drandmea:	$(drandmea_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(drandmea_OBJS) $(LIBS) $(SYSLIBS)

dranduni_OBJS	=dranduni.o
dranduni:	$(dranduni_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dranduni_OBJS) $(LIBS) $(SYSLIBS)

dsteps_OBJS	=dsteps.o
dsteps:	$(dsteps_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dsteps_OBJS) $(LIBS) $(SYSLIBS)

dmeander_OBJS	=dmeander.o
dmeander:	$(dmeander_OBJS) $(LIBDEPS)
	$(LD) -o $@ $(dmeander_OBJS) $(LIBS) $(SYSLIBS)

clean:
	-@rm -f $(OBJS) $(PROGRAMS)

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
