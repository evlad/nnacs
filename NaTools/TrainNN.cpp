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
        NaDataFile	*dfLeIn = OpenInputDataFile(argv[2]);
        NaDataFile	*dfLeOut = OpenInputDataFile(argv[3]);
        NaDataFile	*dfTeIn = dfLeIn, *dfTeOut = dfLeOut;

        if(argc > 4 && strcmp(argv[4], "-"))
            dfTeIn = OpenInputDataFile(argv[4]);
        if(argc > 5 && strcmp(argv[5], "-"))
            dfTeOut = OpenInputDataFile(argv[5]);
        NaNNUnit            nn;
        nn.Load(argv[1]);
        NaLM bpe(nn);
        NaLM *nnteacher=&bpe;
        NaQuickProp		qpe(nn);
        NaLM                lm(nn);

        bool                bLMCycle = false;
        //  char		*szQPropMu = getenv("QPROP_MU");
        //  if(NULL != szQPropMu)
        //  {
        //      qpe.mu = atof(szQPropMu);
//            nnteacher = &qpe;
        //   }
        /* Prepare buffers for input, target and output data */
        NaReal	*pIn = new NaReal[nn.descr.InputsNumber()];// вход
        NaReal	*pTar = new NaReal[nn.descr.nOutNeurons];// target
        NaReal	*pOut = new NaReal[nn.descr.nOutNeurons]; // out
        NaReal	*pLeMSE = new NaReal[nn.descr.nOutNeurons];// буффер выходной выборки обучения
        NaReal	*pTeMSE = new NaReal[nn.descr.nOutNeurons];// буффер выходной выборки теста

        /* Let's start the learning */
        int	iEpoch = 0, iLayer, j;

	// Size of base to calculate jacobian for LM
	// Should be equal to size of the training set
        char		*szLMBaseSize = getenv("LM_NSAMPLES");
        if(NULL != szLMBaseSize)
        {

            nnteacher->SetNSamples_LM(atoi(szLMBaseSize));
            bLMCycle = true;
            NaReal ALPHA_LM=0.001;
            NaReal E_LM_OLD;
            while(!bTerminate&bLMCycle)
            {
                ++iEpoch;
                /* Reset error counters */
                for(j = 0; j < nn.descr.nOutNeurons; ++j)
                    pLeMSE[j] = pTeMSE[j] = 0.0;

                /* Learning epoch */
                // прохождение по всей выборке
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
                        //проходим через сеть в прямом направлении
			// Forward propagation of the data through NN
                        nn.Function(pIn, pOut);

                        /* Compute sum of squared error */
                        for(j = 0; j < nn.descr.nOutNeurons; ++j)
                        {
                            pLeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);
                        }

                        //
                        nnteacher->SetOutputError(nSamples, pTar, pOut);
                        nnteacher->SetTrainingPair(nSamples, pIn, pTar, pOut);
                        ++nSamples;
                    }
                    while(dfLeIn->GoNextRecord() && dfLeOut->GoNextRecord());
                    //
                    /* Compute mean squared error */
                    /* Reset error counters */
                    E_LM_OLD = 0;
                    for(j = 0; j < nn.descr.nOutNeurons; ++j)
                    {

                        pLeMSE[j] /= nSamples;
                        E_LM_OLD += pLeMSE[j];
                    }
                }// прохождение по всей выборке

		// посчитали якобиан и ошибку
		// Jacobian and error are ready

                do
                {
                    /* Reset error counters */
                    for(j = 0; j < nn.descr.nOutNeurons; ++j)
                        pLeMSE[j] = pTeMSE[j] = 0.0;

                    nnteacher->CalcWeightAdj(ALPHA_LM);
                    nnteacher->UpdateNN_LM();

                    //считаем новую ошибку
                    // прохождение по всей выборке
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
                            //проходим через сеть в прямом направлении
                            nn.Function(pIn, pOut);
                            /* Compute sum of squared error */
                            for(j = 0; j < nn.descr.nOutNeurons; ++j)
                                pLeMSE[j] += (pTar[j] - pOut[j]) * (pTar[j] - pOut[j]);
                            //
                            ++nSamples;
                        }
                        while(dfLeIn->GoNextRecord() && dfLeOut->GoNextRecord());
                        //
                        /* Compute mean squared error */
                        /* Reset error counters */
                        E_LM_NEW=0;
                        for(j = 0; j < nn.descr.nOutNeurons; ++j)
                        {

                            pLeMSE[j] /= nSamples;
                            E_LM_NEW+=pLeMSE[j];
                        }
                    }// прохождение по всей выборке
                    //посчитали новую ошибку
                    if(E_LM_NEW>E_LM_OLD)
                    {
                        ALPHA_LM*=10;
                        nnteacher->DegradeNN_LM();
                        if(ALPHA_LM>1000000)
                        {
                            bTerminate=true;
                            printf("\n local minimim\n");
                            break;
                        }
                    }
                }
                while(E_LM_NEW>E_LM_OLD);
///////////////////////////////////////////////////////////////////////
                if(bTerminate==true)
                {
                    break;
                }
                ALPHA_LM/=10;
                nnteacher->Init_Zero_LM();
//
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
//
                /* Display epoch number, learning and testing errors */
                printf("%d\t", iEpoch);
                for(j = 0; j < nn.descr.nOutNeurons; ++j)
                    printf(" %g", pLeMSE[j]);
                putchar('\t');
                printf("TIME=");
                printf("%g",(clock()-start_time)/1000);
                putchar(' ');
                printf("ALPHA=");
                printf("%g",ALPHA_LM);
                TIME_NN <<"("<<((clock()-start_time)/1000)<<";"<<E_LM_NEW<<")\n";
                putchar('\t');
                for(j = 0; j < nn.descr.nOutNeurons; ++j)
                    printf(" %g", pTeMSE[j]);
                if(iEpoch > nMaxEpochs)
                    bTerminate = true;
                putchar('\n');
            }
        }
        while(!bTerminate&!bLMCycle)
        {

            ++iEpoch;

            /* Reset error counters */
            for(j = 0; j < nn.descr.nOutNeurons; ++j)
                pLeMSE[j] = pTeMSE[j] = 0.0;

            /* Learning epoch */
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

        }/* End of learning loop */

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
