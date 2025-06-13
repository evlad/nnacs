/* AnTrainNN.cpp */
static char rcsid[] = "$Id$";

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <NaExcept.h>
#include <NaDataIO.h>
#include <NaStdBPE.h>
#include <NaQProp.h>
#include <NaDynAr.h>
#include <NaParams.h>


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
   Test the method of NN antagonistic learning.

   Arguments:
   ~~~~~~~~~~
   argv[1] - NN file (source and target in case of argc==6)
   argv[2] - training input configuration
   argv[3] - training output (one line per class)
   argv[4] - test input        {absent means learning input}
   argv[5] - test output       {absent means learning output}
   argv[6] - NN file (target)  {optional}

   Training input configuration file (.ids - input data set):
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   training input class1 data file
   training input class2 data file
   ...
   training input classN data file

   Examples:
   ~~~~~~~~~
   AnTrainNN file.nn xtraining.ids ytraining.dat                                (argc=4)
   AnTrainNN file.nn xtraining.ids ytraining.dat result.nn                      (argc=5)
   AnTrainNN file.nn xtraining.ids ytraining.dat xtest.dat ytest.dat            (argc=6)
   AnTrainNN file.nn xtraining.ids ytraining.dat xtest.dat ytest.dat result.nn  (argc=7)

   Parameters:
   ~~~~~~~~~~~
   MAX_EPOCHS - number of epochs for training (unlimited if not set)
   ANTAGONISM - the gain of antagonistic training application (no
                antagonistic training if not set)
*/
int main (int argc, char* argv[])
{
    if(argc < 4) {
	printf("Usage: %s NN-file LearnInConf LearnOut [TestIn TestOut [NN-out]]\n",
	       argv[0]);
	return 1;
    }

    NaOpenLogFile("AnTrainNN.log");

    /* Let's handle SIGINT(2) */
    signal(SIGINT, handle_sigint);

    double fAntGain = 0.0;  /* not set by default */
    char	*szAntGain = getenv("ANTAGONISM");
    if(NULL != szAntGain) {
	fAntGain = atof(szAntGain);
    }

    int	nMaxEpochs = -1;	// unlimited
    char	*szMaxEpochs = getenv("MAX_EPOCHS");
    if(NULL != szMaxEpochs) {
	nMaxEpochs = atoi(szMaxEpochs);
    }

    try{
	NaDynAr<NaDataFile*> dfInDS;

	NaPrintLog("Reading input data set configuration file \"%s\"\n", argv[2]);
	FILE *fpInDS = fopen(argv[2], "r");
	if(NULL == fpInDS)
	    throw(na_cant_open_file);
	char szLineBuf[NaPARAM_LINE_MAX_LEN+1];
	int  nClass = 0;
	NaDataFile	*dfLeIn = NULL;
	while(NULL != fgets(szLineBuf, NaPARAM_LINE_MAX_LEN+1, fpInDS)) {
	    const char* szFName = strtok(szLineBuf, " \t\n\r");
	    NaPrintLog("Class %d: reading input data set \"%s\"\n", ++nClass, szFName);
	    dfLeIn = OpenInputDataFile(szFName);
	    if(NULL == dfLeIn)
		throw(na_cant_open_file);
	    dfInDS.addh(dfLeIn);
	}
	fclose(fpInDS);
	NaPrintLog("Total %d classes on input\n", nClass);

	NaDataFile	*dfLeOut = OpenInputDataFile(argv[3]);

	NaDataFile	*dfTeIn = dfLeIn/* Error!!!! */, *dfTeOut = dfLeOut;

	if(argc > 5) {
	    dfTeIn = OpenInputDataFile(argv[4]);
	    dfTeOut = OpenInputDataFile(argv[5]);
	}

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

	NaReal	*pLeMSE = new NaReal[nn.descr.nOutNeurons];
	NaReal	*pTeMSE = new NaReal[nn.descr.nOutNeurons];

	/* Let's start the learning */
	int	iEpoch = 0, iLayer, j;

	while(!bTerminate) {

	    ++iEpoch;

	    /* Reset error counters */
	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
		pLeMSE[j] = pTeMSE[j] = 0.0;

	    /* Learning epoch for every class */
	    int iClass = 0;
	    if(dfLeOut->GoStartRecord()) {
		int	nSamples = 0;

		do{
		    NaPrintLog("### Start with class %d ###\n", iClass+1);

		    /* Read the target class value vector */
		    for(j = 0; j < nn.descr.nOutNeurons; ++j)
			pTar[j] = dfLeOut->GetValue(j);

		    /*
		     *  Native course
		     */

		    /* Read the native input data */
		    if(dfInDS[iClass]->GoStartRecord()) {
			int	nNativeSamples = 0;

			do {
			    for(j = 0; j < nn.descr.InputsNumber(); ++j)
				pIn[j] = dfInDS[iClass]->GetValue(j);

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
					nnteacher->DeltaRule(pTar);
				    else
					/* Hidden layer */
					nnteacher->DeltaRule(iLayer, iLayer + 1);
				}

			    ++nNativeSamples;
			    ++nSamples;

			}while(dfInDS[iClass]->GoNextRecord());

			NaPrintLog("  has %d native samples\n", nNativeSamples);
		    }

		    /* Update the NN weights at the end of native course */
		    nnteacher->UpdateNN();

		    /* Compute mean squared error */
		    for(j = 0; j < nn.descr.nOutNeurons; ++j)
			pLeMSE[j] /= nSamples;

		    /*
		     *  Antagonistic course
		     */
		    if(fAntGain < 0.0)
			for(int iAntClass = 0; iAntClass < nClass; ++iAntClass) {

			    if(iAntClass == iClass)
				/* By definition of an antagonistic class */
				continue;

			    NaPrintLog("### Antagonistic class %d ###\n", iAntClass+1);

			    /* Read the antagonistic input data */
			    if(dfInDS[iAntClass]->GoStartRecord()) {
				int	nAntSamples = 0;

				do {
				    for(j = 0; j < nn.descr.InputsNumber(); ++j)
					pIn[j] = dfInDS[iAntClass]->GetValue(j);

				    /* Pass through the net in forward direction */
				    nn.Function(pIn, pOut);

				    /* Compute sum of squared error */
				    //for(j = 0; j < nn.descr.nOutNeurons; ++j)
				    //    pLeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);

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

				    ++nAntSamples;
				    ++nSamples;

				}while(dfInDS[iAntClass]->GoNextRecord());

				NaPrintLog("  has %d antagonistic samples\n", nAntSamples);
			    }

			    /* Update the NN weights at the end of antagonistic course */
			    nnteacher->UpdateNN(fAntGain);
			}

		    ++iClass;

		}while(dfLeOut->GoNextRecord());

		NaPrintLog("Total %d samples\n", nSamples);
	    }

	    /* Testing epoch */
	    if(dfTeIn->GoStartRecord() && dfTeOut->GoStartRecord())
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

	    ///* Update the NN weights at the end of each epoch */
	    //nnteacher->UpdateNN();

	    /* Display epoch number, learning and testing errors */
	    printf("%d\t", iEpoch);

	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
		printf(" %g", pLeMSE[j]);

	    putchar('\t');

	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
		printf(" %g", pTeMSE[j]);

	    putchar('\n');

	    if(iEpoch > nMaxEpochs && nMaxEpochs > 0)
		bTerminate = true;

	}/* End of learning loop */

	if(argc == 5 || argc == 7)
	    nn.Save(argv[6]);
	else
	    nn.Save(argv[1]);
    }
    catch(NaException& ex){
	NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
	return 1;
    }
    return 0;
}
