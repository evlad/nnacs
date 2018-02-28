/* TrainNN.cpp */
static char rcsid[] = "$Id$";
using namespace std;
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <NaExcept.h>
#include <NaDataIO.h>
#include <NaStdBPE.h>
#include <NaQProp.h>
#include <NaDataSet.h>
#include <NaLM.h>
#include <ctime>
#include <fstream>
#include <iostream>
// флаг завершения программы
bool	bTerminate = false;
/* Signal handler */
void
handle_sigint (int signo)
{
    if(SIGINT == signo)
        bTerminate = true;
}


bool
ReadDataSetFrom2Files (NaDataSet& ds, NaDataFile* dfIn, NaDataFile* dfOut)
{
    if(!dfIn->GoStartRecord() || !dfOut->GoStartRecord())
	return false;

    NaReal	*pIn = new NaReal[ds.GetInputSize()];
    NaReal	*pOut = new NaReal[ds.GetOutputSize()];
    unsigned	j;

    do {
	/* Read the learning pair */
	for(j = 0; j < ds.GetInputSize(); ++j)
	    pIn[j] = dfIn->GetValue(j);
	for(j = 0; j < ds.GetOutputSize(); ++j)
	    pOut[j] = dfOut->GetValue(j);

	ds.AddPair(pIn, pOut);
    }
    while(dfIn->GoNextRecord() && dfOut->GoNextRecord());

    delete[] pIn;
    delete[] pOut;

    return true;
}


/// Calculate MSE for the output of the unit and the desired output
/// provided in ds.out.  Calculated actual output of the unit is
/// stored in optional pDSOut array of vector.
/// \return MSE
NaVector
CalcOutputMSE (NaUnit& unit, NaDataSet& ds, NaDynAr<NaVector>* pDSOut = NULL)
{
  NaVector	vMSE(unit.InputDim());
  NaVector	vOut(unit.OutputDim());

  vMSE.init_zero();

  // Iterate through the whole dataset
  for(unsigned i = 0; i < ds.NumberOfPairs(); ++i)
    {
      NaVector	&vIn = ds.FetchInput(i);
      NaVector	&vTar = ds.FetchOutput(i);

      // Forward propagation of the data through NN
      unit.Function(&vIn[0], &vOut[0]);
      if(NULL != pDSOut)
	pDSOut->addh(vOut);

      // Compute sum of squared error
      for(unsigned j = 0; j < unit.OutputDim(); ++j)
	vMSE[j] += (vTar[j] - vOut[j]) * (vTar[j] - vOut[j]);
    }

  return vMSE;
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
    NaReal start_time=clock();
    ofstream TIME_NN;
    ofstream ERROR_NN;
    double E_LM_NEW;
    TIME_NN.open("TIME_NN.txt");
    if(argc < 4)
    {
        printf("Usage: %s NN-file LearnIn LearnOut [TestIn TestOut [NN-out]]\n",
               argv[0]);
        return 1;
    }

    NaOpenLogFile("TrainNN.log");

    /* Let's handle SIGINT(2) */
    signal(SIGINT, handle_sigint);

    int	nMaxEpochs = -1;	// unlimited
    char	*szMaxEpochs = getenv("MAX_EPOCHS");
    if(NULL != szMaxEpochs)
    {
        nMaxEpochs = atoi(szMaxEpochs);
    }

    try
    {
        NaNNUnit            nn;
        nn.Load(argv[1]);

	NaDataSet	dsLe(nn.descr.InputsNumber(), nn.descr.nOutNeurons);
	NaDataSet	dsTe(nn.descr.InputsNumber(), nn.descr.nOutNeurons);

	{
	  NaDataFile	*dfLeIn = OpenInputDataFile(argv[2]);
	  NaDataFile	*dfLeOut = OpenInputDataFile(argv[3]);
	  NaDataFile	*dfTeIn = dfLeIn, *dfTeOut = dfLeOut;

	  if(argc > 4 && strcmp(argv[4], "-"))
            dfTeIn = OpenInputDataFile(argv[4]);
	  if(argc > 5 && strcmp(argv[5], "-"))
            dfTeOut = OpenInputDataFile(argv[5]);

	  if(!ReadDataSetFrom2Files(dsLe, dfLeIn, dfLeOut)) {
	    NaPrintLog("Failed to read training set");
	    throw(na_read_error);
	  }
	  if(!ReadDataSetFrom2Files(dsTe, dfTeIn, dfTeOut)) {
	    NaPrintLog("Failed to read test set");
	    throw(na_read_error);
	  }

	  if(dfTeIn != dfLeIn)
	    delete dfTeIn;
	  if(dfTeOut != dfLeOut)
	    delete dfTeOut;
	  delete dfLeIn;
	  delete dfLeOut;
	}

        NaLM bpe(nn);
        NaLM *nnteacher=&bpe;
        NaQuickProp	qpe(nn);
        NaLM		lm(nn);

        bool		bLMCycle = false;
        //  char		*szQPropMu = getenv("QPROP_MU");
        //  if(NULL != szQPropMu)
        //  {
        //      qpe.mu = atof(szQPropMu);
//            nnteacher = &qpe;
        //   }
        /* Prepare buffers for input, target and output data */
        //NaReal	*pIn = new NaReal[nn.descr.InputsNumber()];// вход
        //NaReal	*pTar = new NaReal[nn.descr.nOutNeurons];// target
        NaReal	*pOut = new NaReal[nn.descr.nOutNeurons]; // out

	NaVector vLeMSE, vTeMSE;
        NaReal	*pLeMSE = new NaReal[nn.descr.nOutNeurons];// буффер выходной выборки обучения
        NaReal	*pTeMSE = new NaReal[nn.descr.nOutNeurons];// буффер выходной выборки теста

        /* Let's start the learning */
        int	iEpoch = 0, j;

	// Size of base to calculate jacobian for LM
	// Should be equal to size of the training set
        char		*szLMBaseSize = getenv("LM_NSAMPLES");
        if(NULL != szLMBaseSize)
        {
	  unsigned	nLMbase = atoi(szLMBaseSize);

	  if(nLMbase != dsLe.NumberOfPairs()) {
	    NaPrintLog("Error: LM_NSAMPLES=%u but should be equal to traing set size %u\n",
		       nLMbase, dsLe.NumberOfPairs());
	    throw(na_bad_value);
	  }
	  nnteacher->SetNSamples_LM(nLMbase);
	  bLMCycle = true;
	  NaReal ALPHA_LM=0.001;
	  NaReal E_LM_OLD;
	  while(!bTerminate & bLMCycle)
            {
	      ++iEpoch;

	      NaDynAr<NaVector>	dvLeOut;
	      NaVector vLeMSE = CalcOutputMSE(nn, dsLe, &dvLeOut);
	      E_LM_OLD = vLeMSE.sum() / dsLe.NumberOfPairs();

	      /* Learning epoch */
	      // прохождение по всей выборке
	      for(unsigned i = 0; i < dsLe.NumberOfPairs(); ++i)
		{
		  nnteacher->SetOutputError(i, &dsLe.FetchOutput(i)[0], &dvLeOut[i][0]);
		  nnteacher->SetTrainingPair(i, &dsLe.FetchInput(i)[0],
					     &dsLe.FetchOutput(i)[0], &dvLeOut[i][0]);
		}

	      // посчитали якобиан и ошибку
	      // Jacobian and error are ready

	      do
		{
		  /* Reset error counters */
		  nnteacher->CalcWeightAdj(ALPHA_LM);
		  nnteacher->UpdateNN_LM();

		  //считаем новую ошибку
		  // прохождение по всей выборке
		  vLeMSE = CalcOutputMSE(nn, dsLe);
		  E_LM_NEW = vLeMSE.sum() / dsLe.NumberOfPairs();

		  if(E_LM_NEW > E_LM_OLD)
		    {
		      ALPHA_LM*=10;
		      nnteacher->DegradeNN_LM();
		      if(ALPHA_LM > 1000000)
			{
			  bTerminate = true;
			  printf("\n local minimim\n");
			  break;
			}
		    }
		}
	      while(E_LM_NEW > E_LM_OLD);
	      ///////////////////////////////////////////////////////////////////////

	      if(bTerminate==true)
		{
		  break;
		}

	      ALPHA_LM /= 10;
	      nnteacher->Init_Zero_LM();

	      vTeMSE = CalcOutputMSE(nn, dsTe);

	      /* Display epoch number, learning and testing errors */
	      printf("%d\t", iEpoch);
	      for(j = 0; j < nn.descr.nOutNeurons; ++j)
		printf(" %g", vLeMSE[j]);
	      putchar('\t');
	      printf("TIME=");
	      printf("%g",(clock()-start_time)/1000);
	      putchar(' ');
	      printf("ALPHA=");
	      printf("%g",ALPHA_LM);
	      TIME_NN <<"("<<((clock()-start_time)/1000)<<";"<<E_LM_NEW<<")\n";
	      putchar('\t');
	      for(j = 0; j < nn.descr.nOutNeurons; ++j)
		printf(" %g", vTeMSE[j]);
	      if(iEpoch > nMaxEpochs)
		bTerminate = true;
	      putchar('\n');
	    }

	} // End of training by LM method
	else {
	  // Begin Backpropagations of error
	  while(!bTerminate&!bLMCycle) {

	    ++iEpoch;

	    /* Reset error counters */
	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
	      pLeMSE[j] = pTeMSE[j] = 0.0;

	    /* Learning epoch */

	    NaDynAr<NaVector>	dvLeOut;
	    NaVector vLeMSE = CalcOutputMSE(nn, dsLe, &dvLeOut);
#if 0
	    E_LM_OLD = vLeMSE.sum() / dsLe.NumberOfPairs();

	    BackPropagationOfError

	      /* Learning epoch */
	      // прохождение по всей выборке
	      for(unsigned i = 0; i < dsLe.NumberOfPairs(); ++i)
		{
		  nnteacher->SetOutputError(i, &dsLe.FetchOutput(i)[0], &dvLeOut[i][0]);
		  nnteacher->SetTrainingPair(i, &dsLe.FetchInput(i)[0],
					     &dsLe.FetchOutput(i)[0], &dvLeOut[i][0]);
		}
	    
	    if(dfLeIn->GoStartRecord() && dfLeOut->GoStartRecord())
	      {
		int	nSamples = 0;

		do
		  {
		    /* Read the learning pair */
		    for(j = 0; j < nn.descr.InputsNumber(); ++j)
		      pIn[j] = dfLeIn->GetValue(j);
		    for(j = 0; j < nn.descr.nOutNeurons; ++j)
		      pTar[j] = dfLeOut->GetValue(j);

		    /* Pass through the net in forward direction */
		    //проходим через сеть в прямом направлении
		    nn.Function(pIn, pOut);
		    //

		    // Use bLMCycle
		    if(bLMCycle)
		      NaPrintLog("LM cycle\n");

		    /* Compute sum of squared error */
		    for(j = 0; j < nn.descr.nOutNeurons; ++j)
		      pLeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);
		    //
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

		    ++nSamples;

		  }
		while(dfLeIn->GoNextRecord() && dfLeOut->GoNextRecord());

		/* Compute mean squared error */
		E_LM_NEW=0;
		for(j = 0; j < nn.descr.nOutNeurons; ++j)
		  {
		    pLeMSE[j] /= nSamples;
		    E_LM_NEW+=pLeMSE[j];
		  }
	      }

	    /* Testing epoch */
	    if(dfTeIn->GoStartRecord() && dfTeOut->GoStartRecord())
	      {
		int	nSamples = 0;

		do
		  {
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
		  }
		while(dfTeIn->GoNextRecord() && dfTeOut->GoNextRecord());

		/* Compute mean squared error */
		for(j = 0; j < nn.descr.nOutNeurons; ++j)
		  pTeMSE[j] /= nSamples;
	      }

	    /* Update the NN weights at the end of each epoch */
	    nnteacher->UpdateNN();

	    /* Display epoch number, learning and testing errors */
	    printf("%d\t", iEpoch);

	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
	      printf(" %g", pLeMSE[j]);
	    TIME_NN <<"("<<((clock()-start_time)/1000)<<";"<<E_LM_NEW<<")\n";
	    putchar('\t');
	    printf("%g",(clock()-start_time)/1000);

	    for(j = 0; j < nn.descr.nOutNeurons; ++j)
	      printf(" %g", pTeMSE[j]);

	    putchar('\n');

	    if(iEpoch > nMaxEpochs)
	      bTerminate = true;
#endif

	  }/* End of learning loop */
	} // End of the BPE mode

	if(argc > 6)
	  nn.Save(argv[6]);
	else
	  nn.Save(argv[1]);
    }
    catch(NaException& ex)
      {
        NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
      }
    ERROR_NN.close();
    TIME_NN.close();
}
