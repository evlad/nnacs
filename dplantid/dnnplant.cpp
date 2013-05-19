/* dnnplant.cpp */
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
  if(5 != argc)
    {
      fprintf(stderr, "Usage: dnnplant NNP-File u-File y-File y_nn-File\n");
      return 1;
    }

  NaOpenLogFile("dnnplant.log");

  try{
    // Neural network description
    NaNeuralNetDescr	nn_descr;

    // Read neural network from file
    NaNNUnit		au_nn(nn_descr);
    au_nn.Load(argv[1]);

    NaNNRegrPlantLearn	nnroe(NaEvaluationAlgorithm);	// test

    // Configure nodes
    nnroe.nnplant.set_transfer_func(&au_nn);

    unsigned	*input_delays = au_nn.descr.InputDelays();
    unsigned	*output_delays = au_nn.descr.OutputDelays();

    nnroe.delay_u.set_delay(au_nn.descr.nInputsRepeat, input_delays);
    nnroe.delay_y.set_delay(au_nn.descr.nOutputsRepeat, output_delays);

    unsigned	iDelay_u = nnroe.delay_u.get_max_delay();
    unsigned	iDelay_y = nnroe.delay_y.get_max_delay();
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

    nnroe.skip_u.set_skip_number(iSkip_u);

    nnroe.skip_y.set_skip_number(iSkip_y);

    // Additional delay for target value
    nnroe.skip_yt.set_skip_number(iSkip_yt);
    nnroe.delay_yt.set_delay(0);
    nnroe.delay_yt.add_delay(iDelay_yt);

    /*
    printf("delay_u=%d,  skip_u=%d\n", iDelay_u, iSkip_u);
    printf("delay_y=%d,  skip_y=%d\n", iDelay_y, iSkip_y);
    printf("delay_yt=%d,  skip_yt=%d\n", iDelay_yt, iSkip_yt);
    */

    // Duplicate input y series at the same time ticks as nn_y
    nnroe.fetch_y.set_output(0);

    nnroe.in_u.set_input_filename(argv[2]);
    nnroe.in_y.set_input_filename(argv[3]);
    nnroe.nn_y.set_output_filename(argv[4]);
    nnroe.tr_y.set_output_filename("/tmp/tr_y.dat");

    NaPNEvent   pnev;

    try{
      nnroe.link_net();
    }
    catch(NaException& ex){
      NaPrintLog("EXCEPTION at linkage phase: %s\n", NaExceptionMsg(ex));
      throw(ex);
    }

    try{
      pnev = nnroe.run_net();

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
    }
    catch(NaException& ex){
      NaPrintLog("EXCEPTION at runtime phase: %s\n", NaExceptionMsg(ex));
      throw(ex);
    }

    printf("MSE=%g (%g)\n", nnroe.statan.RMS[0],
	   nnroe.statan.RMS[0] / nnroe.statan_y.RMS[0]);

    nnroe.statan.print_stat();

  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
