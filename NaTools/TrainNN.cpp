/* TrainNN.cpp */
static char rcsid[] = "$Id$";

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


/* Purpose:
   ~~~~~~~~
   Test the method of NN learning.

   Arguments:
   ~~~~~~~~~~
   argv[1] - NN file (source and target in case of argc==6)
   argv[2] - learning input
   argv[3] - learning output
   argv[4] - test input        {"-" or absent means learning input}
   argv[5] - test output       {"-" or absent means learning output}
   argv[6] - NN file (target)  {optional}
*/
int main (int argc, char* argv[])
{
  if(argc < 4)
    {
      printf("Usage: %s NN-file LearnIn LearnOut [TestIn TestOut [NN-out]]\n",
	     argv[0]);
      return 1;
    }

  NaOpenLogFile("TrainNN.log");

  /* Let's handle SIGINT(2) */
  signal(SIGINT, handle_sigint);

  try{
    NaDataFile	*dfLeIn = OpenInputDataFile(argv[2]);
    NaDataFile	*dfLeOut = OpenInputDataFile(argv[3]);

    NaDataFile	*dfTeIn = dfLeIn, *dfTeOut = dfLeOut;

    if(argc > 4 && strcmp(argv[4], "-"))
      dfTeIn = OpenInputDataFile(argv[4]);
    if(argc > 5 && strcmp(argv[5], "-"))
      dfTeOut = OpenInputDataFile(argv[5]);

    NaNNUnit            nn;
    nn.Load(argv[1]);

    NaStdBackProp	bpe(nn);

    /* Prepare buffers for input, target and output data */
    NaReal	*pIn = new NaReal[nn.descr.InputsNumber()];
    NaReal	*pTar = new NaReal[nn.descr.nOutNeurons];
    NaReal	*pOut = new NaReal[nn.descr.nOutNeurons];

    NaReal	*pLeMSE = new NaReal[nn.descr.nOutNeurons];
    NaReal	*pTeMSE = new NaReal[nn.descr.nOutNeurons];

    /* Let's start the learning */
    int	iEpoch = 0, iLayer, j;

    while(!bTerminate)
      {
	++iEpoch;

	/* Reset error counters */
	for(j = 0; j < nn.descr.nOutNeurons; ++j)
	  pLeMSE[j] = pTeMSE[j] = 0.0;

	/* Learning epoch */
	if(dfLeIn->GoStartRecord() && dfLeOut->GoStartRecord())
	  {
	    int	nSamples = 0;

	    do{
	      /* Read the learning pair */
	      for(j = 0; j < nn.descr.InputsNumber(); ++j)
		pIn[j] = dfLeIn->GetValue(j);
	      for(j = 0; j < nn.descr.nOutNeurons; ++j)
		pTar[j] = dfLeOut->GetValue(j);

	      /* Pass through the net in forward direction */
	      nn.Function(pIn, pOut);

	      /* Compute sum of squared error */
	      for(j = 0; j < nn.descr.nOutNeurons; ++j)
		pLeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);

	      /* Pass through the net in backward direction */
	      for(iLayer = nn.OutputLayer(); iLayer >= 0; --iLayer)
		{
		  if(nn.OutputLayer() == iLayer)
		    /* Output layer */
		    bpe.DeltaRule(pTar);
		  else
		    /* Hidden layer */
		    bpe.DeltaRule(iLayer, iLayer + 1);
		}

	      ++nSamples;

	    }while(dfLeIn->GoNextRecord() && dfLeOut->GoNextRecord());

	    /* Compute mean squared error */
	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
	      pLeMSE[j] /= nSamples;
	  }

	/* Testing epoch */
	if(dfLeIn->GoStartRecord() && dfLeOut->GoStartRecord())
	  {
	    int	nSamples = 0;

	    do{
	      /* Read the learning pair */
	      for(j = 0; j < nn.descr.InputsNumber(); ++j)
		pIn[j] = dfTeIn->GetValue(j);
	      for(j = 0; j < nn.descr.nOutNeurons; ++j)
		pTar[j] = dfTeOut->GetValue(j);

	      /* Pass through the net in forward direction */
	      nn.Function(pIn, pOut);

	      /* Compute sum of squared error */
	      for(j = 0; j < nn.descr.nOutNeurons; ++j)
		pTeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);

	      ++nSamples;

	    }while(dfTeIn->GoNextRecord() && dfTeOut->GoNextRecord());

	    /* Compute mean squared error */
	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
	      pTeMSE[j] /= nSamples;
	  }

	/* Update the NN weights at the end of each epoch */
	bpe.UpdateNN();

	/* Display epoch number, learning and testing errors */
	printf("%d\t", iEpoch);

	for(j = 0; j < nn.descr.nOutNeurons; ++j)
	  printf(" %g", pLeMSE[j]);

	putchar('\t');

	for(j = 0; j < nn.descr.nOutNeurons; ++j)
	  printf(" %g", pTeMSE[j]);

	putchar('\n');

      }/* End of learning loop */

    if(argc > 6)
      nn.Save(argv[6]);
    else
      nn.Save(argv[1]);
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }
}
