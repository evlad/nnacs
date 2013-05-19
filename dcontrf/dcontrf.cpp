/* dcontrf.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#pragma hdrstop
#ifndef unix
//#include <condefs.h>
#endif /* unix */

//---------------------------------------------------------------------------
// Implementation of the phase #3 of neural network control paradigm (NNCP).
// NNCP - neural network control paradigm. (C)opyright by Eliseev Vladimir
//---------------------------------------------------------------------------
// Phase #3: NN optimal controller (in Wiener filter terms) training using
//           NN identifier for error computation.  Input data - r(t), n(t).
//           + prelearned regression NN plant model.
//---------------------------------------------------------------------------

#include <vector>

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaConfig.h>
#include <NaCoFunc.h>
#include <NaNNUnit.h>
#include <NaNNLrn.h>
#include <NaDataIO.h>
#include <NaParams.h>
#include <kbdif.h>

#include "NaNNOCL.h"


//---------------------------------------------------------------------------
NaReal	fPrevMSEc = 0.0, fPrevMSEi = 0.0;
int	nMaxEpochs = 0;

void
PrintLog (int iAct, void* pData)
{
  NaNNOptimContrLearn	&nnocl = *(NaNNOptimContrLearn*)pData;

  printf("%4d: Control MSE=%7.4f delta=%+9.7f"\
	 "  Ident.MSE=%7.4f delta=%+9.7f\n", iAct,
	 nnocl.cerrstat.RMS[0], nnocl.cerrstat.RMS[0] - fPrevMSEc,
	 nnocl.iderrstat.RMS[0], nnocl.iderrstat.RMS[0] - fPrevMSEi);
  fflush(stdout);

  fPrevMSEc = nnocl.cerrstat.RMS[0];
  fPrevMSEi = nnocl.iderrstat.RMS[0];

  if(nMaxEpochs > 0 && iAct >= nMaxEpochs) {
      printf("Maximum number of epochs %d was reached\n", nMaxEpochs);
      fflush(stdout);
      NaPrintLog("Maximum number of epochs %d was reached\n", nMaxEpochs);
      /* arbitrary node may be halted to stop the whole network */
      nnocl.plant.halt();
  }
}


//---------------------------------------------------------------------------
// Copy neural network from nnplant2 (NN-P adoption) to nnplant (NN-C
// adoption).
void
PushNNP (int iAct, void* pData)
{
  NaNNOptimContrLearn	*self = (NaNNOptimContrLearn*)pData;
  NaNNUnit	*pNNP = self->nnplant.get_nn_unit();
  NaNNUnit	*pNNP2 = self->nnplant2.get_nn_unit();

  /* just copy oneNN to another */
  *pNNP = *pNNP2;
}


//---------------------------------------------------------------------------
void
ParseHaltCond (NaPNStatistics& pnstat, char* parvalue)
{
  char	*token;

  for(token = strtok(parvalue, ";");
      NULL != token;
      token = strtok(NULL, ";"))
    {
      int	sign;
      char	*stend;

      char	*stname = token;

      // skip leading spaces
      while(*stname != '\0')
	{
	  if(!isspace(*stname))
	    break;
	  ++stname;
	}

      stend = stname;

      // find end of statistics name
      while(*stend != '\0')
	{
	  if(!isalpha(*stend))
	    break;
	  ++stend;
	}

      char	*op = stend;

      // skip leading spaces
      while(*op != '\0')
	{
	  if(!isspace(*op))
	    break;
	  ++op;
	}

      switch(*op)
	{
	case '<':
	  sign = LESS_THAN;
	  break;
	case '=':
	  sign = EQUAL_TO;
	  break;
	case '>':
	  sign = GREATER_THAN;
	  break;
	default:
	  // skip 
	  NaPrintLog("Unknown condition operator '%c' -> skip token '%s'\n",
		     *op, token);
	  continue;
	}

      *stend = '\0';

      int	stat_id = NaStatTextToId(stname);
      NaReal	value = atof(1 + op);

      NaPrintLog("Halt rule is: %s %c %g\n",
		 NaStatIdToText(stat_id),
		 (sign < 0)?'<':((sign > 0)?'>':'='),
		 value);

      pnstat.halt_condition(stat_id, sign, value);
    }
}


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
  if(argc < 2)
    {
      fprintf(stderr, "Usage: dcontrf ParamFile [Extra name=value pairs]\n");
      return 1;
    }

  try{
    NaParams	par(argv[1], argc - 2, argv + 2);

    NaPrintLog("Run dcontrf with %s\n", argv[1]);

    par.ListOfParamsToLog();

    /*************************************************************/
    enum {
      stream_mode,
      file_mode
    }	inp_data_mode;

    if(!strcmp("stream", par("input_kind")))
      {
	NaPrintLog("Using stream reference and noise (transfer functions)\n");
	inp_data_mode = stream_mode;
      }
    else if(!strcmp("file", par("input_kind")))
      {
	NaPrintLog("Using limited reference and noise series (data files)\n");
	inp_data_mode = file_mode;
      }
    else
      {
	NaPrintLog("Bad or undefined input_kind value in .par file\n");
	return 0;
      }

    /*************************************************************/
    // Applied units
    NaCombinedFunc	refer_tf;
    NaCombinedFunc	noise_tf;
    NaCombinedFunc	au_linplant;
    NaCombinedFunc	au_tradcontr;

    // Load plant
    au_linplant.Load(par("linplant_tf"));

    // Read neural network from file
    NaNNUnit            au_nnc, au_nnp;

    //au_nnc.SetInstance("Plant");
    au_nnp.Load(par("in_nnp_file"));

    // Get NNP delays
    unsigned	*input_delays = au_nnp.descr.InputDelays();
    unsigned	*output_delays = au_nnp.descr.OutputDelays();

    // Interpret NN-C structure
    NaControllerKind	ckind;
#if 0
    // Default rule
    if(au_nnc.descr.nInputsRepeat > 1)
      ckind = NaNeuralContrDelayedE;
    else
      ckind = NaNeuralContrER;
#endif
    // Explicit rule
    if(!strcmp(par("nnc_mode"), "tradcontr"))
      ckind = NaLinearContr;
    else if(!strcmp(par("nnc_mode"), "e+r"))
      ckind = NaNeuralContrER;
    else if(!strcmp(par("nnc_mode"), "e+de"))
      ckind = NaNeuralContrEdE;
    else if(!strcmp(par("nnc_mode"), "e+e+..."))
      ckind = NaNeuralContrDelayedE;

    if(NaLinearContr == ckind)
      // Load traditional controller
      au_tradcontr.Load(par("tradcontr"));
    else
      // Load NNC from file
      au_nnc.Load(par("in_nnc_file"));

    // Load input data
    int	len;
    switch(inp_data_mode)
      {
      case stream_mode:
	refer_tf.Load(par("refer_tf"));
	noise_tf.Load(par("noise_tf"));
	len = atoi(par("stream_len"));
	break;
      case file_mode:
	len = 0;
	break;
      }

    NaNNOptimContrLearn     nnocl(len, ckind, "nncfl");

    // Configure nodes
    nnocl.delay_u.set_delay(au_nnp.descr.nInputsRepeat, input_delays);
    nnocl.delay_y.set_delay(au_nnp.descr.nOutputsRepeat, output_delays);

    unsigned	iDelay_u = nnocl.delay_u.get_max_delay();
    unsigned	iDelay_y = nnocl.delay_y.get_max_delay();
    unsigned	iSkip_u = 0, iSkip_y = 0;
    unsigned	iDelay_e, iSkip_e;

    // Skip u or y due to absent earlier values of y or u
    if(iDelay_y >= iDelay_u)
      {
	iSkip_u = iDelay_y - iDelay_u + 1;
      }
    else /* if(iDelay_u > iDelay_y) */
      {
	iSkip_y = iDelay_u - iDelay_y - 1;
      }
    iDelay_e = iDelay_y + iSkip_y;
    iSkip_e = 1 + iDelay_e;

    nnocl.skip_u.set_skip_number(iSkip_u);
    nnocl.skip_y.set_skip_number(iSkip_y);

    // Additional delay for target value
    nnocl.skip_e.set_skip_number(1 + iSkip_e);

    nnocl.skip_ny.set_skip_number(1 + iSkip_e);
    nnocl.fill_nn_y.set_fill_number(iSkip_e);

    NaPrintLog("delay_u=%d,  skip_u=%d\n", iDelay_u, iSkip_u);
    NaPrintLog("delay_y=%d,  skip_y=%d\n", iDelay_y, iSkip_y);
    NaPrintLog("delay_e=%d,  skip_e=%d\n", iDelay_e, iSkip_e);

    nnocl.plant.set_transfer_func(&au_linplant);

    if(NaLinearContr == ckind)
      nnocl.tradcontr.set_transfer_func(&au_tradcontr);
    else
      {
	nnocl.nncontr.set_nn_unit(&au_nnc);
	nnocl.nncteacher.set_nn(&nnocl.nncontr, iSkip_e - 1);
      }

    nnocl.nnplant.set_nn_unit(&au_nnp);
    //au_nnp.PrintLog();

    /* Special neural network to be tuned by identification error */
    NaNNUnit	au_nnp2(au_nnp);
    //au_nnp2.PrintLog();
    nnocl.nnplant2.set_nn_unit(&au_nnp2);

    // NN-P is used here to backpropagate control error to teach the
    // NN controller
    nnocl.errbackprop.set_nn(&nnocl.nnplant);

    // NN-P is used here to teach the NN-P itself according to
    // identification error
    nnocl.nnpteacher.set_nn(&nnocl.nnplant2);
    //nnocl.nnpteacher.set_nn(&nnocl.nnplant2, iSkip_e);

    // u[?] - actual controller force
    if(!par.CheckParam("errfetch_output"))
      {
	int	iErrFetch = iDelay_u;	// value of u delay by default
	NaPrintLog("default errfetch_output=%d\n", iErrFetch);
	nnocl.errfetch.set_output(iErrFetch);
      }
    else
      {
	char	*buf = strdup(par("errfetch_output"));
	char	*token = strtok(buf, " ,;");
	std::vector<int>	viErrFetch;
	std::vector<NaReal>	vfSumWeights;
	while(NULL != token)
	  {
	    viErrFetch.push_back(atoi(token));
	    token = strtok(NULL, " ,;");
	  }
	free(buf);  buf = NULL;

	int	nOutDim = viErrFetch.size();
	unsigned	*piOutMap = new unsigned[nOutDim];
	NaPrintLog("errfetch_output=");
	for(int i = 0; i < nOutDim; ++i)
	  {
	    piOutMap[i] = viErrFetch[i];
	    NaPrintLog("%d ", piOutMap[i]);
	  }
	NaPrintLog("\n");
	nnocl.errfetch.set_output(nOutDim, piOutMap);
	delete[] piOutMap;  piOutMap = NULL;

	if(par.CheckParam("errfetch_sumweights"))
	  {
	    buf = strdup(par("errfetch_sumweights"));
	    token = strtok(buf, " ,;");
	    while(NULL != token)
	      {
		vfSumWeights.push_back(atoi(token));
		token = strtok(NULL, " ,;");
	      }
	    free(buf);

	    NaReal	*pfSumWeights = new NaReal[nOutDim];
	    NaPrintLog("errfetch_sumweights=");
	    for(int i = 0; i < nOutDim; ++i)
	      {
		if(i >= vfSumWeights.size())
		  pfSumWeights[i] = 1.0;
		else
		  pfSumWeights[i] = vfSumWeights[i];
		NaPrintLog("%g ", pfSumWeights[i]);
	      }
	    NaPrintLog("\n");
	    nnocl.errfetch.set_sum_weights(pfSumWeights);
	    delete[] pfSumWeights;  pfSumWeights = NULL;
	  }
      }

    // Setpoint and noise
    NaReal	fMean = 0.0, fStdDev = 1.0;

    // Log files with statistics (file_mode)
    NaDataFile	*dfCErr = NULL, *dfIdErr = NULL;

    nnocl.c_in.set_output_filename(par("c_in"));
    nnocl.p_in.set_output_filename(par("p_in"));

    switch(inp_data_mode)
      {
      case stream_mode:
	nnocl.setpnt_gen.set_generator_func(&refer_tf);
	nnocl.setpnt_gen.set_gauss_distrib(&fMean, &fStdDev);

	nnocl.noise_gen.set_generator_func(&noise_tf);
	nnocl.noise_gen.set_gauss_distrib(&fMean, &fStdDev);

	nnocl.setpnt_out.set_output_filename(par("out_r"));
	NaPrintLog("Writing reference signal to '%s' file.\n", par("out_r"));

	nnocl.noise_out.set_output_filename(par("out_n"));
	NaPrintLog("Writing pure noise signal to '%s' file.\n", par("out_n"));
	break;

      case file_mode:
	nnocl.setpnt_inp.set_input_filename(par("in_r"));
	nnocl.noise_inp.set_input_filename(par("in_n"));

#if 0
	dfCErr = OpenOutputDataFile(par("cerr_trace_file"),
				    bdtAuto, NaSI_number);
	dfIdErr = OpenOutputDataFile(par("iderr_trace_file"),
				     bdtAuto, NaSI_number);
#endif
	// Need only the last data portion; all previous must be skipped
	nnocl.cerr_qout.set_queue_limit(0);
	nnocl.iderr_qout.set_queue_limit(0);
	break;
      }

    if(par.CheckParam("cerr_trace_file")) {
	NaPrintLog("Writing control error statistics to '%s' file.\n",
		   par("cerr_trace_file"));
	nnocl.cerr_fout.set_output_filename(par("cerr_trace_file"));
	if(par.CheckParam("cerr_trace_contents")) {
	    nnocl.cerrstat.configure_output(par("cerr_trace_contents"));
	}
	if(par.CheckParam("cerr_avg_base")) {
	    nnocl.cerrstat.set_floating_gap(atoi(par("cerr_avg_base")));
	}
    } else {
	nnocl.cerr_fout.set_output_filename(DEV_NULL);
    }

    if(par.CheckParam("iderr_trace_file")) {
	NaPrintLog("Writing identification error statistics to '%s' file.\n",
		   par("iderr_trace_file"));
	nnocl.iderr_fout.set_output_filename(par("iderr_trace_file"));
	if(par.CheckParam("iderr_trace_contents")) {
	    nnocl.iderrstat.configure_output(par("iderr_trace_contents"));
	}
	if(par.CheckParam("iderr_avg_base")) {
	    nnocl.cerrstat.set_floating_gap(atoi(par("iderr_avg_base")));
	}
    } else {
	nnocl.iderr_fout.set_output_filename(DEV_NULL);
    }

    nnocl.nn_u.set_output_filename(par("out_u"));
    NaPrintLog("Writing NNC control force to '%s' file.\n", par("out_u"));

    nnocl.nn_y.set_output_filename(par("out_nn_y"));
    NaPrintLog("Writing NNP identification output to '%s' file.\n",
	       par("out_nn_y"));

    nnocl.on_y.set_output_filename(par("out_ny"));
    NaPrintLog("Writing plant + noise observation output to '%s' file.\n",
	       par("out_ny"));

    switch(ckind)
      {
      case NaNeuralContrDelayedE:
      case NaNeuralContrEdE:
	// Nothing special
	break;
      case NaNeuralContrER:
	nnocl.delay_c.set_delay(au_nnc.descr.nInputsRepeat - 1);
	nnocl.delay_c.set_sleep_value(0.0);
	break;
      }

    // Link the network
    nnocl.link_net();

    // If error changed for less than given value consider stop learning
    // Finish error decrease
    NaReal  fErrPrec = atof(par("finish_decrease"));

    // If error started grows, skip checking MSE given number of epochs
    // Skip growing error up to given epochs
    int     nSkipped = 0, nSkipGrowErr = atoi(par("skip_growing"));

    // Maximum number of growing error slopes
    int     nNumGrowErr = atoi(par("finish_on_grow"));

    // Configure learning parameters
    nnocl.nncteacher.lpar.eta = atof(par("eta"));
    nnocl.nncteacher.lpar.eta_output = atof(par("eta_output"));
    nnocl.nncteacher.lpar.alpha = atof(par("alpha"));

    nnocl.nnpteacher.lpar.eta = atof(par("p_eta"));
    nnocl.nnpteacher.lpar.eta_output = atof(par("p_eta_output"));
    nnocl.nnpteacher.lpar.alpha = atof(par("p_alpha"));

    // Teach the network iteratively
    NaPNEvent   pnev;
    int         iIter = 0;

#if defined(__MSDOS__) || defined(__WIN32__)
    printf("Press 'q' or 'x' for exit\n");
#endif /* DOS & Win */

    NaReal	fPrevMSE = 0.0;
    int		nnc_auf = 0;
    int		nnp_auf = 0;
    bool	bLRbyAUF = false;

    nMaxEpochs = atoi(par("max_epochs"));

    bLRbyAUF = !!atoi(par("eta_scale_by_auf"));

    nnc_auf = atoi(par("nnc_auf"));
    nnp_auf = atoi(par("nnp_auf"));

    if(nnp_auf > 0)
      NaPrintLog("NNP training is %s\n", (nnp_auf > 0)? "ON": "OFF");

    if(stream_mode == inp_data_mode ||
       file_mode == inp_data_mode && (nnc_auf > 0 || nnp_auf > 0))
      {
	// Set autoupdate facility
	if(nnc_auf > 0) {
	  nnocl.nncteacher.set_auto_update_freq(nnc_auf);
	  NaPrintLog("Autoupdate frequency for NNC is %d\n", nnc_auf);

	  if(bLRbyAUF)
	    {
	      /* Scale down learning rate by length of auto-update
		 frequency */
	      nnocl.nncteacher.lpar.eta /= nnc_auf;
	      nnocl.nncteacher.lpar.eta_output /= nnc_auf;
	  }
	}

	if(nnp_auf > 0) {
	    nnocl.nnpteacher.set_auto_update_freq(nnp_auf);
	    NaPrintLog("Autoupdate frequency for NNP is %d\n", nnp_auf);

	    if(bLRbyAUF)
	      {
		/* Scale down learning rate by length of auto-update
		   frequency */
		nnocl.nnpteacher.lpar.eta /= nnp_auf;
		nnocl.nnpteacher.lpar.eta_output /= nnp_auf;
	      }
	}

	ParseHaltCond(nnocl.cerrstat, par("finish_cerr_cond"));
	ParseHaltCond(nnocl.iderrstat, par("finish_iderr_cond"));

	if(nnc_auf > 0 || nnp_auf > 0) {
	  nnocl.nncteacher.set_auto_update_proc(PrintLog, &nnocl);
	}
	if(nnc_auf > 0 && nnp_auf > 0) {
	  nnocl.nnpteacher.set_auto_update_proc(PushNNP, &nnocl);
	}

	pnev = nnocl.run_net();

	if(pneError != pnev)
	  {
	    nnocl.iderrstat.print_stat();
	    nnocl.cerrstat.print_stat();
	  }
      }
    else /* Update NN-C at the end of iteration */
      {
	do{
	  ++iIter;
	  NaPrintLog("__Iteration_%d__\n", iIter);

	  pnev = nnocl.run_net();

	  // Store error statistics
	  int	i;
	  NaReal	buf[NaSI_number] = {10., 20., 30.};

	  if(NULL != dfCErr) {
	      dfCErr->AppendRecord();
	      nnocl.cerr_qout.get_data(buf);
	      for(i = 0; i < NaSI_number; ++i)
		  dfCErr->SetValue(buf[i], i);
	  }

	  if(NULL != dfIdErr) {
	      dfIdErr->AppendRecord();
	      nnocl.iderr_qout.get_data(buf);
	      for(i = 0; i < NaSI_number; ++i)
		  dfIdErr->SetValue(buf[i], i);
	  }

	  if(pneError == pnev || pneTerminate == pnev || pneHalted == pnev)
	    break;

	  PrintLog(iIter, &nnocl);

	  if(nMaxEpochs > 0 && iIter >= nMaxEpochs){
	    // Limited number of epochs
	    nnocl.nncteacher.update_nn();
	    if(nnp_auf > 0)
		nnocl.nnpteacher.update_nn();
	    break;
	  }

	  if(fPrevMSE != 0.0 && fPrevMSE < nnocl.cerrstat.RMS[0]){
	    if(nSkipped < nSkipGrowErr && nNumGrowErr > 0){
	      --nNumGrowErr;
	      ++nSkipped;
	      printf("Error grows by %g; continue\n",
		     nnocl.cerrstat.RMS[0] - fPrevMSE);
	      fflush(stdout);
	    }else{
	      nSkipped = 0;
	      if(ask_user_bool("MSE started to grow; stop learning?", true)){
		pnev = pneTerminate;
		break;
	      }
	      if(ask_user_bool("Would you like to change learning parameters?",
			       false)){
		ask_user_lpar(nnocl.nncteacher.lpar);
	      }
	    }
	  }else{
	    nSkipped = 0;

	    if(fPrevMSE != 0.0 &&
	       fabs(fPrevMSE - nnocl.cerrstat.RMS[0]) < fErrPrec){
	      if(ask_user_bool("MSE decrease is very small; stop learning?",
			       true)){
		pnev = pneTerminate;
		break;
	      }
	    }
	  }

	  fPrevMSE = nnocl.cerrstat.RMS[0];

	  nnocl.nncteacher.update_nn();

	}while(pneDead == pnev);
      }

    const char	*szFirst = "IMPORTANT: net is dead due to";
    const char	*szSecond = NULL;
    switch(pnev){
    case pneTerminate:
      szSecond = "user break.";
      break;

    case pneHalted:
      szSecond = "internal halt.";
      break;

    case pneDead:
      szSecond = "data are exhausted.";
      break;

    case pneError:
      szSecond = "some error occured.";
      break;

    default:
      if(nMaxEpochs > 0 && iIter >= nMaxEpochs)
	szSecond = "maximum number of epochs was reached.";
      else
	szSecond = "unknown reason.";
      break;
    }

    NaPrintLog("%s %s\n", szFirst, szSecond);
    printf("%s %s\n", szFirst, szSecond);
    fflush(stdout);

    delete dfCErr;
    delete dfIdErr;

    if(nnp_auf > 0)
      au_nnp.Save(par("out_nnp_file"));
    if(NaLinearContr != ckind)
      au_nnc.Save(par("out_nnc_file"));
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
    fprintf(stderr, "EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
