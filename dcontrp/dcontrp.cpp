/* dcontrp.cpp */
static char rcsid[] = "$Id$";

//---------------------------------------------------------------------------
// Implementation of the phase #1 of neural network control paradigm (NNCP).
// NNCP - neural network control paradigm. (C)opyright by Eliseev Vladimir
//---------------------------------------------------------------------------
// Phase #1: preliminary NN controller learning on the basis of obtained at
//           phase #0 series (u(t),e(t),x(t)).
//---------------------------------------------------------------------------

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaConfig.h>
#include <NaTrFunc.h>
#include <NaDataIO.h>
#include <NaParams.h>
#include <NaNNUnit.h>
#include <kbdif.h>

#include "NaNNCPL.h"


//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
  if(argc < 2)
    {
      fprintf(stderr, "Usage: dcontrp ParamFile [Extra name=value pairs]\n");
      return 1;
    }

  try{
    NaParams	par(argv[1], argc - 2, argv + 2);

    NaPrintLog("Run dcontrp with %s\n", argv[1]);

    par.ListOfParamsToLog();

    // Read neural network from file
    NaNNUnit            au_nnc;
    au_nnc.Load(par("in_nnc_file"));

    // Interpret NN-C structure
    NaControllerKind	ckind;
    // Default rule
    if(au_nnc.descr.nInputsRepeat > 1)
      ckind = NaNeuralContrDelayedE;
    else
      ckind = NaNeuralContrER;
    // Explicit rule
    const char* nnc_mode = par("nnc_mode");
    if(!strcmp(nnc_mode, "e+r") ||
       !strcmp(nnc_mode, "r+e"))
      ckind = NaNeuralContrER;
    else if(!strcmp(nnc_mode, "e+de"))
      ckind = NaNeuralContrEdE;
    else if(!strcmp(nnc_mode, "e+e+..."))
      ckind = NaNeuralContrDelayedE;
    else if(!strcmp(nnc_mode, "r+e+e+..."))
      ckind = NaNeuralContrRDelayedE;

    // Additional log files
    NaDataFile  *nnllog = OpenOutputDataFile(par("trace_file"), bdtAuto, 8);

    nnllog->SetTitle("NN controller preliminary learning (error)");

    nnllog->SetVarName(0, "Mean");
    nnllog->SetVarName(1, "StdDev");
    nnllog->SetVarName(2, "MSE");

    NaNNContrPreLearn	nncpl(NaTrainingAlgorithm, ckind, "nncpl");
    NaNNContrPreLearn	nncpe(NaEvaluationAlgorithm, ckind, "nncpe");

    // Configure nodes
    nncpl.nncontr.set_transfer_func(&au_nnc);
    nncpl.nnteacher.set_nn(&nncpl.nncontr);

    nncpl.in_r.set_input_filename(par("in_r"));
    nncpe.in_r.set_input_filename(par("test_in_r"));

    switch(ckind)
      {
      case NaNeuralContrER:
      case NaNeuralContrEdE:
	// Nothing special
	break;

      case NaNeuralContrDelayedE:
	nncpl.delay.set_delay(au_nnc.descr.nInputsRepeat - 1);
	nncpl.delay.set_sleep_value(0.0);
	nncpe.delay.set_delay(au_nnc.descr.nInputsRepeat - 1);
	nncpe.delay.set_sleep_value(0.0);
	break;

      case NaNeuralContrRDelayedE:
	nncpl.delay.set_delay(au_nnc.descr.nInputsRepeat - 2);
	nncpl.delay.set_sleep_value(0.0);
	nncpe.delay.set_delay(au_nnc.descr.nInputsRepeat - 2);
	nncpe.delay.set_sleep_value(0.0);
	break;
      }

    nncpl.in_e.set_input_filename(par("in_e"));
    nncpl.in_u.set_input_filename(par("in_u"));
    nncpl.nn_u.set_output_filename(par("nn_u"));

    nncpe.nncontr.set_transfer_func(&au_nnc);
    nncpe.in_e.set_input_filename(par("test_in_e"));
    nncpe.in_u.set_input_filename(par("test_in_u"));
    nncpe.nn_u.set_output_filename(par("test_nn_u"));

    // Link the network
    nncpl.link_net();
    nncpe.link_net();

    // Configure learning parameters
    nncpl.nnteacher.lpar.eta = atof(par("eta"));
    nncpl.nnteacher.lpar.eta_output = atof(par("eta_output"));
    nncpl.nnteacher.lpar.alpha = atof(par("alpha"));

    //ask_user_lpar(nncpl.nnteacher.lpar);
    //putchar('\n');

    // If error changed for less than given value consider stop learning
    NaReal	fFinishOnDecrease = atof(par("finish_decrease"));

    /* number of epochs when MSE on test set grows to finish the
       learning */
    int		nFinishOnGrowMSE = atoi(par("finish_on_grow"));

    /* number of epochs to stop learning anyway */
    int		nFinishOnMaxEpoch = atoi(par("finish_max_epoch"));

    /* absolute value of MSE to stop learning if reached */
    NaReal	fFinishOnReachMSE = atof(par("finish_on_value"));

    // If mean value of MSE don't change sign more than given iterations
    // consider start part of learning is over
    int     nMaxConstSignIters = 5, nConstSignIters = 0;

    //nMaxConstSignIters =
    //  ask_user("Number of start iters with const sign of mean",
    //	       nMaxConstSignIters);

    // Teach the network iteratively
    NaPNEvent   pnev, pnev_test;
    int         iIter = 0, iEpoch = 0;

#if defined(__MSDOS__) || defined(__WIN32__)
    printf("Press 'q' or 'x' for exit\n");
#endif /* DOS & Win */

    //au_nnc.Initialize();

    NaReal	fNormMSE, fNormTestMSE;
    NaReal	fPrevMSE = 0.0, fLastMSE = 0.0;
    NaReal	fPrevTestMSE = 0.0;
    int		nGrowingMSE = 0;
    int		nDescentingMSE = 0;
    NaNNUnit	rPrevNN(au_nnc);
    bool	bTeach;

    do{
      ++iIter;

      // teach pass
      pnev = nncpl.run_net();

      if(pneError == pnev || pneTerminate == pnev)
	break;

      NaPrintLog("*** Teach pass ***\n");
      nncpl.statan.print_stat();

      if(NaNeuralContrER == ckind)
	fNormMSE = nncpl.statan.RMS[0] / nncpl.statan_u.RMS[0];
      else
	fNormMSE = nncpl.statan.RMS[0];

      printf("Iteration %-4d, MSE=%g (%g)", iIter, nncpl.statan.RMS[0],
	     fNormMSE);

      if(1 == iIter)
	{
	  fLastMSE = fPrevMSE = fNormMSE;
	  rPrevNN = au_nnc;
	  nncpl.nnteacher.update_nn();
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
	      nncpl.nnteacher.lpar.eta /= 2;
	      nncpl.nnteacher.lpar.eta_output /= 2;
	      nncpl.nnteacher.lpar.alpha /= 2;

	      NaPrintLog("Learning parameters: "
			 "lrate=%g  lrate(out)=%g  momentum=%g\n",
			 nncpl.nnteacher.lpar.eta,
			 nncpl.nnteacher.lpar.eta_output,
			 nncpl.nnteacher.lpar.alpha);

	      au_nnc = rPrevNN;
	      fLastMSE = fNormMSE;
	      nncpl.nncontr.set_transfer_func(&au_nnc);
	      nncpl.nnteacher.reset_training();

	      printf(" -> repeat with (%g, %g, %g)\n",
		     nncpl.nnteacher.lpar.eta,
		     nncpl.nnteacher.lpar.eta_output,
		     nncpl.nnteacher.lpar.alpha);
	      fflush(stdout);

	      bTeach = false;
	      nDescentingMSE = 0;
	    }
	  else
	    {
	      /* MSE became less */
	      fLastMSE = fPrevMSE = fNormMSE;
	      rPrevNN = au_nnc;
	      nncpl.nnteacher.update_nn();

	      /* continuous descenting MSE on learning set */
	      if(nDescentingMSE > 10) {
		  double coef = 1.10; // +10% every 10 turns
		  nncpl.nnteacher.lpar.eta *= coef;
		  nncpl.nnteacher.lpar.eta_output *= coef;
		  nncpl.nnteacher.lpar.alpha *= coef;

		  NaPrintLog("Learning parameters: "
			     "lrate=%g  lrate(out)=%g  momentum=%g\n",
			     nncpl.nnteacher.lpar.eta,
			     nncpl.nnteacher.lpar.eta_output,
			     nncpl.nnteacher.lpar.alpha);

		  nDescentingMSE = 0;
		  printf(" -> teach NN and apply (%g, %g, %g)\n",
			 nncpl.nnteacher.lpar.eta,
			 nncpl.nnteacher.lpar.eta_output,
			 nncpl.nnteacher.lpar.alpha);
		  fflush(stdout);
	      } else {
		  ++nDescentingMSE;
		  printf(" -> teach NN\n");
		  fflush(stdout);
	      }
	      bTeach = true;
	    }
	}

      if(bTeach)
	{
	  ++iEpoch;

	  nnllog->AppendRecord();
	  nnllog->SetValue(nncpl.statan.Mean[0], 0);
	  nnllog->SetValue(nncpl.statan.StdDev[0], 1);
	  nnllog->SetValue(nncpl.statan.RMS[0], 2);

	  // test pass
	  pnev_test = nncpe.run_net();

	  if(pneError == pnev_test || pneTerminate == pnev_test)
	    break;

	  if(NaNeuralContrER == ckind)
	    fNormTestMSE = nncpe.statan.RMS[0] / nncpe.statan_u.RMS[0];
	  else
	    fNormTestMSE = nncpe.statan.RMS[0];

	  printf("          Test: MSE=%g (%g)", nncpe.statan.RMS[0],
		 fNormTestMSE);

	  NaPrintLog("*** Test pass ***\n");
	  nncpe.statan.print_stat();

	  nnllog->SetValue(nncpe.statan.Mean[0], 3);
	  nnllog->SetValue(nncpe.statan.StdDev[0], 4);
	  nnllog->SetValue(nncpe.statan.RMS[0], 5);

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
	  if(fFinishOnDecrease != 0 &&
	     fPrevTestMSE - fNormTestMSE > 0 &&
	     fPrevTestMSE - fNormTestMSE <= fFinishOnDecrease)
	    {
	      NaPrintLog("Test MSE became lower: %g -> %g = %g\n",
			 fPrevTestMSE, fNormTestMSE,
			 fPrevTestMSE - fNormTestMSE);
	      NaPrintLog("Test MSE decrease less than predefined %g -> stop\n",
			 fFinishOnDecrease);
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

    au_nnc.Save(par("out_nnc_file"));
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
