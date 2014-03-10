/* BackNN.cpp */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <NaExcept.h>
#include <NaDataIO.h>
#include <NaStdBPE.h>


/* Flag of program termination */
bool	bTerminate = false;

/* Signal handler */
void
handle_sigint (int signo)
{
  if(SIGINT == signo)
    bTerminate = true;
}


/***********************************************************************
 * Write output data vector.
 ***********************************************************************/
void
write_output (NaDataFile* df, unsigned vec_len, const NaReal* vec)
{
  unsigned	vec_i;

  if(NULL == df)
    {
      /* put to stdout */
      for(vec_i = 0; vec_i < vec_len; ++vec_i)
	if(vec_i == vec_len - 1)
	  printf("%g\n", vec[vec_i]);
	else
	  printf("%g ", vec[vec_i]);
    }
  else
    {
      /* put to data file */
      df->AppendRecord();

      for(vec_i = 0; vec_i < vec_len; ++vec_i)
	df->SetValue(vec[vec_i], vec_i);
    }
}


/* Purpose:
   ~~~~~~~~
   Calculate back-propagated deltas, but do not apply them

   Arguments:
   ~~~~~~~~~~
   argv[1] - NN file (source and target in case of argc==6)
   argv[2] - network input (=input dimension) to produce output
   argv[3] - target output (=output dimension)
   argv[4] - output produced by the network 
   argv[5] - error on input produced by the network
*/
int main (int argc, char* argv[])
{
  if(argc != 6)
    {
      printf("Usage: %s NN-file DataIn DesiredOut DataOut ErrOnIn\n",
	     argv[0]);
      return 1;
    }

  NaOpenLogFile("BackNN.log");

  /* Let's handle SIGINT(2) */
  signal(SIGINT, handle_sigint);

  try{
    NaNNUnit            nnu;
    nnu.Load(argv[1]);

    unsigned	nInpDim = nnu.InputDim();
    unsigned	nOutDim = nnu.OutputDim();

    NaDataFile	*dfDataIn = OpenInputDataFile(argv[2]);
    NaDataFile	*dfTargetOut = OpenInputDataFile(argv[3]);
    NaDataFile	*dfDataOut = OpenOutputDataFile(argv[4], bdtAuto, nOutDim);
    NaDataFile	*dfErrOnIn = OpenOutputDataFile(argv[5], bdtAuto, nInpDim);

    NaStdBackProp	bpe(nnu);

    /* Prepare buffers for input, target and output data */
    NaReal	*pIn = new NaReal[nInpDim];
    NaReal	*pTar = new NaReal[nOutDim];
    NaReal	*pOut = new NaReal[nOutDim];
    NaReal	*pErr = new NaReal[nInpDim];

    /* Let's start the forward-backward propagation */
    int	iEpoch = 0, iLayer, j;

    if(dfDataIn->GoStartRecord() && dfTargetOut->GoStartRecord()) {
	int	nRecords = 0;

	do{
	    /* Read the learning pair */
	    for(j = 0; j < nInpDim; ++j)
		pIn[j] = dfDataIn->GetValue(j);
	    for(j = 0; j < nOutDim; ++j)
		pTar[j] = dfTargetOut->GetValue(j);

	    for(j = 0; j < nInpDim; ++j)
		pErr[j] = 0.0;

	    ++nRecords;

	    /* Pass through the net in forward direction */
	    nnu.Function(pIn, pOut);

	    /* Pass through the net in backward direction */
	    for(iLayer = nnu.OutputLayer(); iLayer >= 0; --iLayer)
		{
		    if(nnu.OutputLayer() == iLayer)
			/* Output layer */
			bpe.DeltaRule(pTar);
		    else
			/* Hidden layer */
			bpe.DeltaRule(iLayer, iLayer + 1);
		}

	    /* Compute error on input */
	    for(j = 0; j < nInpDim; ++j){
		// Or may be += ???  I didn't recognize the difference...
		pErr[j] -= bpe.PartOfDeltaRule(0, j);
	    }

	    write_output(dfDataOut, nOutDim, pOut);
	    write_output(dfErrOnIn, nInpDim, pErr);

	    /* Display epoch number, learning and testing errors */
	    printf("%d records done\r", nRecords);

	}while(!bTerminate
	       && dfDataIn->GoNextRecord()
	       && dfTargetOut->GoNextRecord());
    }

    putchar('\n');

    delete dfErrOnIn;
    delete dfDataOut;
    delete dfTargetOut;
    delete dfDataIn;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }
}
