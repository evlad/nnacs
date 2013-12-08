/* nninvf.cpp */

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
   Make forward and backward step to get a function inversion.

   Arguments:
   ~~~~~~~~~~
   argv[1] - NN file (source)
   argv[2] - probe input file or just value
   argv[3] - output values file or just value to be inverted
   argv[4] - resulted file with inverted inputs (optional, "-" means stdout)
   argv[5] - probe output file (optional)
*/
int main (int argc, char* argv[])
{
  if(argc < 4)
    {
      printf("Usage: %s NN-file [ProbeIn] [OutputIn] [InversionOut|-] [ProbeOut]\n",
	     argv[0]);
      return 1;
    }

  NaOpenLogFile("nninvf.log");

  /* Let's handle SIGINT(2) */
  signal(SIGINT, handle_sigint);

  try{
    NaReal fProbeX, fAskedY;
    NaDataFile	*dfProbeX = NULL, *dfAskedY = NULL;

    char *szEnd = NULL;

    fProbeX = strtod(argv[2], &szEnd);
    if(argv[2] == szEnd)
	// Not a floating point number, let's try as file
	dfProbeX = OpenInputDataFile(argv[2]);

    fAskedY = strtod(argv[3], &szEnd);
    if(argv[3] == szEnd)
	// Not a floating point number, let's try as file
	dfAskedY = OpenInputDataFile(argv[3]);

    NaDataFile	*dfResultX = NULL;
    NaDataFile	*dfProbeY = NULL;

    NaNNUnit            nn;
    nn.Load(argv[1]);

    if(argc > 4 && strcmp(argv[4], "-"))
	dfResultX = OpenOutputDataFile(argv[4], bdtAuto, nn.descr.nOutNeurons);

    if(argc > 5)
	dfProbeY = OpenOutputDataFile(argv[5], bdtAuto, nn.descr.nOutNeurons);

    NaStdBackProp	bpe(nn);

    /* Prepare buffers for input, target and output data */
    NaReal	*pProbeX = new NaReal[nn.descr.InputsNumber()];
    NaReal	*pProbeY = new NaReal[nn.descr.nOutNeurons];
    NaReal	*pAskedY = new NaReal[nn.descr.nOutNeurons];
    NaReal	*pResultedX = new NaReal[nn.descr.InputsNumber()];

    /* Let's start the learning */
    int	iEpoch = 0, iLayer, j;

    /* Forward-backward step for each input record */
    bool	bLetsGo = true;
    if(NULL != dfProbeX)
	bLetsGo = bLetsGo && dfProbeX->GoStartRecord();
    if(NULL != dfAskedY)
	bLetsGo = bLetsGo && dfAskedY->GoStartRecord();

    if(bLetsGo) {
	do{
	    /* Read the learning pair */
	    if(NULL == dfProbeX)
		pProbeX[0] = fProbeX;
	    else
		for(j = 0; j < nn.descr.InputsNumber(); ++j)
		    pProbeX[j] = dfProbeX->GetValue(j);
	    if(NULL == dfAskedY)
		pAskedY[0] = fAskedY;
	    else
		for(j = 0; j < nn.descr.nOutNeurons; ++j)
		    pAskedY[j] = dfAskedY->GetValue(j);

	    /* Pass through the net in forward direction */
	    nn.Function(pProbeX, pProbeY);

	    /* Compute sum of squared error */
	    //for(j = 0; j < nn.descr.nOutNeurons; ++j)
//		pLeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);

	    /* Pass through the net in backward direction */
	    for(iLayer = nn.OutputLayer(); iLayer >= 0; --iLayer)
		{
		    if(nn.OutputLayer() == iLayer)
			/* Output layer */
			bpe.DeltaRule(pAskedY);
		    else
			/* Hidden layer */
			bpe.DeltaRule(iLayer, iLayer + 1);
		}

	    // Compute estimated input for given target
	    for(j = 0; j < nn.descr.InputsNumber(); ++j)
		// Or may be + ???  I didn't recognize the difference...
		pResultedX[j] = pProbeX[j] - bpe.PartOfDeltaRule(0, j);

	    write_output(dfResultX, nn.descr.InputsNumber(), pResultedX);

	    if(NULL != dfProbeY)
		write_output(dfProbeY, nn.descr.nOutNeurons, pProbeY);

	    //++nSamples;

	    bLetsGo = (NULL != dfProbeX && dfProbeX->GoStartRecord())
		&& (NULL != dfAskedY && dfAskedY->GoStartRecord());

	}while(!bTerminate && bLetsGo);
    }

    delete dfProbeX;
    delete dfAskedY;

    delete dfResultX;
    delete dfProbeY;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }
}
