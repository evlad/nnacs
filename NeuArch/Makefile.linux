LIBROOT	=pnna
DYNLIB	=lib$(LIBROOT).so
STATLIB	=lib$(LIBROOT).a

all:	$(STATLIB) $(DYNLIB)

install:	$(DYNLIB)
	install -d $(PREFIX)/lib
	install -t $(PREFIX)/lib $(DYNLIB)

LINK.so	=g++ -shared
CXXFLAGS=$(FLAGS) -fPIC -I../Matrix.041 -I.

SRCS	=NaConLaw.cpp \
	NaConfig.cpp \
	NaDDir.cpp \
	NaDataIO.cpp \
	NaBinrIO.cpp \
	NaDynAr.cpp \
	NaExcept.cpp \
	NaInfoSt.cpp \
	NaLiNois.cpp \
	NaLinCon.cpp \
	NaLogFil.cpp \
	NaMath.cpp \
	NaMatrix.cpp \
	NaNDescr.cpp \
	NaNNCon.cpp \
	NaNNLrn.cpp \
	NaNNPObj.cpp \
	NaNNUnit.cpp \
	NaPCnInp.cpp \
	NaPCnOut.cpp \
	NaPID.cpp \
	NaPNAct.cpp \
	NaPNBu21.cpp \
	NaPNBu12.cpp \
	NaPNChPt.cpp \
	NaPNCmp.cpp \
	NaPNCnst.cpp \
	NaPNDely.cpp \
	NaPNFIn.cpp \
	NaPNFOut.cpp \
	NaPNFetc.cpp \
	NaPNGen.cpp \
	NaPNKalm.cpp \
	NaPNRand.cpp \
	NaPNStat.cpp \
	NaPNSum.cpp \
	NaPNSwap.cpp \
	NaPNSwit.cpp \
	NaPNTchr.cpp \
	NaPNTimD.cpp \
	NaPNTime.cpp \
	NaPNTran.cpp \
	NaPNTrig.cpp \
	NaPNLAnd.cpp \
	NaPetCn.cpp \
	NaPetNet.cpp \
	NaPetNod.cpp \
	NaPetri.cpp \
	NaPlotIO.cpp \
	NaQProp.cpp \
	NaRandom.cpp \
	NaSSCtrl.cpp \
	NaSSPlan.cpp \
	NaStdBPE.cpp \
	NaStrOps.cpp \
	NaSysSch.cpp \
	NaTextIO.cpp \
	NaTimer.cpp \
	NaTrFunc.cpp \
	NaCoFunc.cpp \
	NaCuFunc.cpp \
	NaExFunc.cpp \
	NaUnit.cpp \
	NaVector.cpp \
	NaParams.cpp \
	NaPNDerv.cpp \
	NaPNQIn.cpp \
	NaPNQOut.cpp \
	NaPNSkip.cpp \
	NaPNFill.cpp \
	NaPNNNUn.cpp \
	NaPNNNCo.cpp \
	NaPNCuSu.cpp \
	NaPNWatc.cpp \
	NaPNTDG.cpp \
	NaInvPnd.cpp \
	NaWMR.cpp \
	kbdif.cpp

#	NaStaIO.cpp \
#	PNPrint.cpp \
#	PNPrinter.cpp \
#	PNPrintr.cpp \
#	PNRandom.cpp

OBJS	=$(SRCS:.cpp=.o)

TestNN:	TestNN.o $(STATLIB)
	$(LINK.cpp) -o $@ TestNN.o $(STATLIB) ../Matrix.041/libmatrix.a -lm

TestTF:	TestTF.o $(STATLIB)
	$(LINK.cpp) -o $@ TestTF.o $(STATLIB) ../Matrix.041/libmatrix.a -lm

TestCF:	TestCF.o $(STATLIB)
	$(LINK.cpp) -o $@ TestCF.o $(STATLIB) ../Matrix.041/libmatrix.a -ldl -lm

TestQIO:	TestQIO.o $(STATLIB)
	$(LINK.cpp) -o $@ TestQIO.o $(STATLIB) ../Matrix.041/libmatrix.a -lm

clean:
	-@rm -f $(OBJS)

$(DYNLIB):	$(OBJS)
	$(LINK.so) $@ $(OBJS)

$(STATLIB):	$(OBJS)
	ar -ruv $@ $(OBJS)

.cpp.o:
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

.PHONY:	all install clean
