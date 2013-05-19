/* dwtest.cpp */
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


/***********************************************************************
 * Gets two time series: S(i) and N(i).  Creates O(i)=DTF(S(i)+N(i))
 * and computes MSE of O(i)-S(i).
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 5 && argc != 6)
    {
      fprintf(stderr, "Error: need 4 or 5 arguments\n");
      printf("Usage: dwtest DiscrTrFunc SignalSeries NoiseSeries"\
	     " OutSeries [InSeries]\n");
      return 1;
    }

  char	*dtf_file = argv[1];
  char	*signal_file = argv[2];
  char	*noise_file = argv[3];
  char	*out_file = argv[4];
  char	*in_file = NULL;

  if(argc == 6)
    in_file = argv[5];

  NaOpenLogFile("dwtest.log");

  try{
    int			n;
    NaReal		e2;
    NaTransFunc		dtf;
    NaConfigPart	*conf_list[] = { &dtf };
    NaConfigFile	conf_file(";NeuCon transfer", 1, 0);
    conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
    conf_file.LoadFromFile(dtf_file);

    NaDataFile	*dfSignal = OpenInputDataFile(signal_file);
    NaDataFile	*dfNoise = OpenInputDataFile(noise_file);
    NaDataFile	*dfOut = OpenOutputDataFile(out_file, bdtAuto, 1);
    NaDataFile	*dfIn = NULL;

    if(NULL != in_file)
      dfIn = OpenOutputDataFile(in_file, bdtAuto, 1);

    dfSignal->GoStartRecord();
    dfNoise->GoStartRecord();
    dtf.Reset();
    n = 0;
    e2 = 0.;
    do{
      ++n;
      NaReal	fSignal, fNoise, fIn, fOut;
      fSignal = dfSignal->GetValue();
      fNoise = dfNoise->GetValue();

      fIn = fSignal + fNoise;
      dtf.Function(&fIn, &fOut);

      if(NULL != dfIn)
	{
	  dfIn->AppendRecord();
	  dfIn->SetValue(fIn);
	}

      dfOut->AppendRecord();
      dfOut->SetValue(fOut);

      e2 += (fOut - fSignal) * (fOut - fSignal);
    }while(dfSignal->GoNextRecord() && dfNoise->GoNextRecord());

    printf("Series length=%d samples\n", n);
    printf("Mean squared error=%g\n", e2/n);

    delete dfIn;
    delete dfOut;
    delete dfNoise;
    delete dfSignal;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
