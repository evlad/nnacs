/* ErrInvNN.cpp */
static char rcsid[] = "$Id$";

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <NaExcept.h>
#include <NaDataIO.h>
#include <NaStdBPE.h>
#include <NaQProp.h>


/* Flag of program termination */
bool	bTerminate = false;

/* Signal handler */
void
handle_sigint (int signo)
{
  if(SIGINT == signo)
    bTerminate = true;
}


/* Purpose:
   ~~~~~~~~
   Error inversion/investigation for the trained NN.

   Arguments:
   ~~~~~~~~~~
   argv[1] - NN file )trained before)
   argv[2] - test input
   argv[3] - test output
   argv[4] - 
*/
int main (int argc, char* argv[])
{
  if(argc < 6)
    {
      printf("Usage: %s NN-file TestIn TestOut ErrorIn ErrorOut\n", argv[0]);
      return 1;
    }

  NaOpenLogFile("ErrInvNN.log");

  /* Let's handle SIGINT(2) */
  signal(SIGINT, handle_sigint);

  try{
    NaDataFile	*dfTeIn = OpenInputDataFile(argv[2]);
    NaDataFile	*dfTeOut = OpenInputDataFile(argv[3]);

    NaDataFile	*dfErrIn = OpenOutputDataFile(argv[4]);
    NaDataFile	*dfErrOut = OpenOutputDataFile(argv[5]);

    NaNNUnit            nn;
    nn.Load(argv[1]);

    NaStdBackProp	bpe(nn);
    NaQuickProp		qpe(nn);
    NaStdBackProp	*nnteacher = &bpe;

    char		*szQPropMu = getenv("QPROP_MU");
    if(NULL != szQPropMu) {
	qpe.mu = atof(szQPropMu);
	nnteacher = &qpe;
    }

    /* Prepare buffers for input, target and output data */
    NaReal	*pIn = new NaReal[nn.descr.InputsNumber()];
    NaReal	*pTar = new NaReal[nn.descr.nOutNeurons];
    NaReal	*pOut = new NaReal[nn.descr.nOutNeurons];

    NaVector	vInErr(nn.descr.InputsNumber());
    NaVector	vOutErr(nn.descr.nOutNeurons);

    /* Let's start the learning */
    int	iRecord = 0, iLayer, j;

    /* Learning epoch */
    if(dfTeIn->GoStartRecord() && dfTeOut->GoStartRecord())
	{
	    do{

		/* Read the test pair */
		for(j = 0; j < nn.descr.InputsNumber(); ++j)
		    pIn[j] = dfTeIn->GetValue(j);
		for(j = 0; j < nn.descr.nOutNeurons; ++j)
		    pTar[j] = dfTeOut->GetValue(j);

		/* Pass through the net in forward direction */
		nn.Function(pIn, pOut);

		/* Compute sum of squared error */
		dfErrOut->AppendRecord();
		for(j = 0; j < nn.descr.nOutNeurons; ++j) {
		    vOutErr[j] = (pTar[j] - pOut[j]);
		    dfErrOut->SetValue(vOutErr[j], j);
		}

		/* Pass through the net in backward direction */
		for(iLayer = nn.OutputLayer(); iLayer >= 0; --iLayer)
		    {
			if(nn.OutputLayer() == iLayer)
			    /* Output layer */
			    nnteacher->DeltaRule(pTar);
			else
			    /* Hidden layer */
			    nnteacher->DeltaRule(iLayer, iLayer + 1);
		    }

		nnteacher->DeltaRuleOnInput(&vInErr[0]);

		dfErrIn->AppendRecord();
		for(j = 0; j < nn.descr.InputsNumber(); ++j) {
		    dfErrIn->SetValue(vInErr[j], j);
		}

		printf("%d\t%g\t%g\n", iRecord, vInErr.metric(), vOutErr.metric());

		iRecord++;

		if(bTerminate)
		    break;

	    } while(dfTeIn->GoNextRecord() && dfTeOut->GoNextRecord());

	    delete dfErrIn;
	    delete dfErrOut;
	}

  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }
}
