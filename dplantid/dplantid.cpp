/* dplantid.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaConfig.h>
#include <NaTrFunc.h>
#include <NaDataIO.h>
#include <NaParams.h>
#include <kbdif.h>

#include "NaNNRPL.h"


/***********************************************************************
 * Train neural net plant identification model.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc < 2)
    {
      fprintf(stderr, "Usage: dplantid ParamFile [Extra name=value pairs]\n");
      return 1;
    }

  try{
    NaParams	par(argv[1], argc - 2, argv + 2);

    NaPrintLog("Run dplantid with %s\n", argv[1]);

    par.ListOfParamsToLog();

    // Read neural network from file
    NaNNUnit            au_nn;
    au_nn.Load(par("in_nnp_file"));

    // Additional log files
    NaDataFile  *nnllog = OpenOutputDataFile(par("trace_file"), bdtAuto, 8);

    nnllog->SetTitle("NN regression plant learning");

    nnllog->SetVarName(0, "Mean");
    nnllog->SetVarName(1, "StdDev");
    nnllog->SetVarName(2, "MSE");

    NaNNRegrPlantLearn	nnrol(NaTrainingAlgorithm, "nnpl");
    NaNNRegrPlantLearn	nnroe(NaEvaluationAlgorithm, "nnpe");

    // Configure nodes
    unsigned	*input_delays = au_nn.descr.InputDelays();
    unsigned	*output_delays = au_nn.descr.OutputDelays();

    int		nLearn, nList_in_u, nList_in_y, nList_nn_y, nList_tr_y;
    char	**szList_in_u = par("in_u", nList_in_u);
    char	**szList_in_y = par("in_y", nList_in_y);
    char	**szList_nn_y = par("nn_y", nList_nn_y);
    char	**szList_tr_y = par("tr_y", nList_tr_y);

    /* nLearn=MIN4(a,b,c,d) */
    nLearn = nList_in_u;
    if(nLearn > nList_in_y)
      nLearn = nList_in_y;
    if(nLearn > nList_nn_y)
      nLearn = nList_nn_y;
    if(nLearn > nList_tr_y)
      nLearn = nList_tr_y;

    NaPrintLog("Total %d learning data files\n", nLearn);

    int		nTest, nList_test_in_u, nList_test_in_y, nList_test_nn_y,
		nList_test_tr_y;
    char	**szList_test_in_u = par("test_in_u", nList_test_in_u);
    char	**szList_test_in_y = par("test_in_y", nList_test_in_y);
    char	**szList_test_nn_y = par("test_nn_y", nList_test_nn_y);
    char	**szList_test_tr_y = par("test_tr_y", nList_test_tr_y);

    /* nTest=MIN4(a,b,c,d) */
    nTest = nList_test_in_u;
    if(nTest > nList_test_in_y)
      nTest = nList_test_in_y;
    if(nTest > nList_test_nn_y)
      nTest = nList_test_nn_y;
    if(nTest > nList_test_tr_y)
      nTest = nList_test_tr_y;

    NaPrintLog("Total %d testing data files\n", nTest);

    nnrol.nnplant.set_transfer_func(&au_nn);
    nnrol.nnteacher.set_nn(&nnrol.nnplant);

    nnrol.delay_u.set_delay(au_nn.descr.nInputsRepeat, input_delays);
    nnrol.delay_y.set_delay(au_nn.descr.nOutputsRepeat, output_delays);

    unsigned	iDelay_u = nnrol.delay_u.get_max_delay();
    unsigned	iDelay_y = nnrol.delay_y.get_max_delay();
    unsigned	iSkip_u = 0, iSkip_y = 0;
    unsigned	iDelay_yt, iSkip_yt;

    // Skip u or y due to absent earlier values of y or u
    if(iDelay_y >= iDelay_u)
      {
	iSkip_u = iDelay_y - iDelay_u + 1;
      }
    else /* if(iDelay_u > iDelay_y) */
      {
	iSkip_y = iDelay_u - iDelay_y - 1;
      }
    iDelay_yt = iDelay_y + iSkip_y;
    iSkip_yt = 1 + iDelay_yt;

    nnrol.skip_u.set_skip_number(iSkip_u);
    nnroe.skip_u.set_skip_number(iSkip_u);

    nnrol.skip_y.set_skip_number(iSkip_y);
    nnroe.skip_y.set_skip_number(iSkip_y);

    // Additional delay for target value
    nnrol.skip_yt.set_skip_number(iSkip_yt);
    nnrol.delay_yt.set_delay(0);
    nnrol.delay_yt.add_delay(iDelay_yt);

    nnroe.skip_yt.set_skip_number(iSkip_yt);
    nnroe.delay_yt.set_delay(0);
    nnroe.delay_yt.add_delay(iDelay_yt);

    nnroe.nnplant.set_transfer_func(&au_nn);
    nnroe.delay_u.set_delay(au_nn.descr.nInputsRepeat, input_delays);
    nnroe.delay_y.set_delay(au_nn.descr.nOutputsRepeat, output_delays);

    NaPrintLog("delay_u=%d,  skip_u=%d\n", iDelay_u, iSkip_u);
    NaPrintLog("delay_y=%d,  skip_y=%d\n", iDelay_y, iSkip_y);
    NaPrintLog("delay_yt=%d,  skip_yt=%d\n", iDelay_yt, iSkip_yt);

    // Duplicate input y series at the same time ticks as nn_y
    nnrol.fetch_y.set_output(0);
    nnroe.fetch_y.set_output(0);

    // Configure learning parameters
    nnrol.nnteacher.lpar.eta = atof(par("eta"));
    nnrol.nnteacher.lpar.eta_output = atof(par("eta_output"));
    nnrol.nnteacher.lpar.alpha = atof(par("alpha"));

    // Number of successful descending MSE to make faster learning
    // rate (=0 means off)
    int		nAccelHits = atoi(par("accel_hits"));
    float	fMaxEta = atof(par("eta"));
    if(nAccelHits > 0)
      NaPrintLog("when learning will succeed for %d epochs"	\
		 " eta will be enlarged\n", nAccelHits);

    //ask_user_lpar(nnrol.nnteacher.lpar);
    //putchar('\n');

    // Teach the network iteratively
    NaPNEvent   pnev, pnev_test;
    int         iIter = 0, iEpoch = 0, iData, nHits = 0;

#if defined(__MSDOS__) || defined(__WIN32__)
    printf("Press 'q' or 'x' for exit\n");
#endif /* DOS & Win */

    //au_nn.Initialize();

    NaReal	fNormMSE, fNormTestMSE;
    NaReal	fPrevMSE = 0.0, fLastMSE = 0.0;
    NaReal	fPrevTestMSE = 0.0;
    int		nGrowingMSE = 0;
    NaNNUnit	rPrevNN(au_nn);
    bool	bTeach, bTeachLinkage = false, bTestLinkage = false;

    /* number of epochs when MSE on test set grows to finish the
       learning */
    int		nFinishOnGrowMSE = atoi(par("finish_on_grow"));

    /* number of epochs to stop learning anyway */
    int		nFinishOnMaxEpoch = atoi(par("finish_max_epoch"));

    /* absolute value of MSE to stop learning if reached */
    NaReal	fFinishOnReachMSE = atof(par("finish_on_value"));

    // Time chart
    //nnrol.net.time_chart(true);

    do{
      ++iIter;

      // teach pass
      for(iData = 0; iData < nLearn; ++iData)
	{
	  NaPrintLog("*** Teach pass: '%s' '%s' '%s' '%s' ***\n",
		     szList_in_u[iData],
		     szList_in_y[iData],
		     szList_tr_y[iData],
		     szList_nn_y[iData]);

	  nnrol.in_u.set_input_filename(szList_in_u[iData]);
	  nnrol.in_y.set_input_filename(szList_in_y[iData]);
	  nnrol.nn_y.set_output_filename(szList_nn_y[iData]);
	  nnrol.tr_y.set_output_filename(szList_tr_y[iData]);

	  if(!bTeachLinkage)
	    {
	      nnrol.link_net();
	      bTeachLinkage = true;
	    }
	  pnev = nnrol.run_net();

	  if(pneError == pnev || pneTerminate == pnev)
	    break;
	}

      if(pneError == pnev || pneTerminate == pnev)
	break;

      NaPrintLog("*** Teach passed ***\n");
      nnrol.statan.print_stat();

      fNormMSE = nnrol.statan.RMS[0] / nnrol.statan_y.RMS[0];

      printf("Iteration %-4d, MSE=%g (%g)", iIter, nnrol.statan.RMS[0],
	     fNormMSE);

      if(1 == iIter)
	{
	  fLastMSE = fPrevMSE = fNormMSE;
	  rPrevNN = au_nn;
	  nnrol.nnteacher.update_nn();
	  printf(" -> teach NN\n");
	  fflush(stdout);
	  bTeach = true;
	}
      else
	{
	  /* next passes */
	  if(fLastMSE < fNormMSE)
	    {
	      /* growing MSE on learning set */
	      nnrol.nnteacher.lpar.eta /= 2;
	      nnrol.nnteacher.lpar.eta_output /= 2;
	      nnrol.nnteacher.lpar.alpha /= 2;

	      nHits = 0;

	      NaPrintLog("Learning parameters: "\
			 "lrate=%g  lrate(out)=%g  momentum=%g\n",
			 nnrol.nnteacher.lpar.eta,
			 nnrol.nnteacher.lpar.eta_output,
			 nnrol.nnteacher.lpar.alpha);

	      au_nn = rPrevNN;
	      fLastMSE = fNormMSE;
	      nnrol.nnplant.set_transfer_func(&au_nn);
	      nnrol.nnteacher.reset_training();

	      printf(" -> repeat with (%g, %g, %g)\n",
		     nnrol.nnteacher.lpar.eta,
		     nnrol.nnteacher.lpar.eta_output,
		     nnrol.nnteacher.lpar.alpha);
	      fflush(stdout);

	      bTeach = false;
	    }
	  else
	    {
	      /* MSE became less */
	      fLastMSE = fPrevMSE = fNormMSE;
	      rPrevNN = au_nn;
	      nnrol.nnteacher.update_nn();
	      bTeach = true;

	      /* Try to accelerate learning */
	      ++nHits;
	      if(nAccelHits > 0 && nHits >= nAccelHits &&
		 nnrol.nnteacher.lpar.eta < fMaxEta)
		{
		  nnrol.nnteacher.lpar.eta *= 1.5;
		  nnrol.nnteacher.lpar.eta_output *= 1.5;
		  nnrol.nnteacher.lpar.alpha *= 1.5;

		  nHits = 0;

		  NaPrintLog("Learning parameters: "			\
			     "lrate=%g  lrate(out)=%g  momentum=%g\n",
			     nnrol.nnteacher.lpar.eta,
			     nnrol.nnteacher.lpar.eta_output,
			     nnrol.nnteacher.lpar.alpha);

		  printf(" -> teach NN and continue with (%g, %g, %g)\n",
			 nnrol.nnteacher.lpar.eta,
			 nnrol.nnteacher.lpar.eta_output,
			 nnrol.nnteacher.lpar.alpha);
		  fflush(stdout);
		}
	      else {
		printf(" -> teach NN\n");
		fflush(stdout);
	      }
	    }
	}

      if(bTeach)
	{
	  ++iEpoch;

	  nnllog->AppendRecord();
	  nnllog->SetValue(nnrol.statan.Mean[0], 0);
	  nnllog->SetValue(nnrol.statan.StdDev[0], 1);
	  nnllog->SetValue(nnrol.statan.RMS[0], 2);

	  // test pass
	  for(iData = 0; iData < nTest; ++iData)
	    {
	      NaPrintLog("*** Test pass: '%s' '%s' '%s' '%s' ***\n",
			 szList_test_in_u[iData],
			 szList_test_in_y[iData],
			 szList_test_tr_y[iData],
			 szList_test_nn_y[iData]);

	      nnroe.in_u.set_input_filename(szList_test_in_u[iData]);
	      nnroe.in_y.set_input_filename(szList_test_in_y[iData]);
	      nnroe.nn_y.set_output_filename(szList_test_nn_y[iData]);
	      nnroe.tr_y.set_output_filename(szList_test_tr_y[iData]);

	      if(!bTestLinkage)
		{
		  nnroe.link_net();
		  bTestLinkage = true;
		}
	      pnev_test = nnroe.run_net();

	      if(pneError == pnev_test || pneTerminate == pnev_test)
		break;
	    }

	  if(pneError == pnev_test || pneTerminate == pnev_test)
	    break;

	  fNormTestMSE = nnroe.statan.RMS[0] / nnroe.statan_y.RMS[0];

	  printf("          Test: MSE=%g (%g)", nnroe.statan.RMS[0],
		 nnroe.statan.RMS[0] / nnroe.statan_y.RMS[0]);

	  NaPrintLog("*** Test passed ***\n");
	  nnroe.statan.print_stat();

	  nnllog->SetValue(nnroe.statan.Mean[0], 3);
	  nnllog->SetValue(nnroe.statan.StdDev[0], 4);
	  nnllog->SetValue(nnroe.statan.RMS[0], 5);

	  nnllog->SetValue(fNormMSE, 6);
	  nnllog->SetValue(fNormTestMSE, 7);

	  if(fNormTestMSE < fFinishOnReachMSE)
	    {
	      NaPrintLog("Test MSE reached preset value %g -> stop\n",
			 fFinishOnReachMSE);
	      printf(" -> reached preset value %g -> stop\n",
		     fFinishOnReachMSE);
	      fflush(stdout);
	      break;
	    }
	  if(fPrevTestMSE < fNormTestMSE)
	    {
	      /* Start growing */
	      ++nGrowingMSE;
	      if(nGrowingMSE > nFinishOnGrowMSE && nFinishOnGrowMSE > 0)
		{
		  NaPrintLog("Test MSE was growing for %d epoch -> stop\n",
			     nFinishOnGrowMSE);
		  printf(" -> grew for %d epochs -> stop\n", nFinishOnGrowMSE);
		  fflush(stdout);
		  break;
		}
	      printf(" -> grows\n", nFinishOnGrowMSE);
	      fflush(stdout);
	    }
	  else
	    {
	      /* Reset counter */
	      nGrowingMSE = 0;
	      printf(" -> descents\n");
	      fflush(stdout);
	    }

	  if(nFinishOnMaxEpoch != 0 && iEpoch >= nFinishOnMaxEpoch)
	    {
	      NaPrintLog("Max number of epoch %d is reached -> stop\n",
			 nFinishOnMaxEpoch);
	      printf(" %d epochs is over -> stop\n", nFinishOnMaxEpoch);
	      fflush(stdout);
	      break;
	    }

	  fPrevTestMSE = fNormTestMSE;
	}

    }while(pneDead == pnev);

    NaPrintLog("IMPORTANT: net is dead due to ");
    switch(pnev){
    case pneTerminate:
      NaPrintLog("user break.\n");
      break;

    case pneHalted:
      NaPrintLog("internal halt.\n");
      break;

    case pneDead:
      NaPrintLog("data are exhausted.\n");
      break;

    case pneError:
      NaPrintLog("some error occured.\n");
      break;

    default:
      NaPrintLog("unknown reason.\n");
      break;
    }

    delete nnllog;

    au_nn.Save(par("out_nnp_file"));
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
    fprintf(stderr, "EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
