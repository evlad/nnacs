/* dwtest2.cpp */
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
 * Gets two time series: S(i) and N(i).  Creates
 * O(i)=DTF1*DTF2(S(i)+N(i)) and computes MSE of O(i)-S(i).
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 6 && argc != 7)
    {
      fprintf(stderr, "Error: need 5 or 6 arguments\n");
      printf("Usage: dwtest2 ContrTrFunc PlantTrFunc SignalSeries NoiseSeries"\
	     " OutSeries [InSeries]\n");
      return 1;
    }

  char	*dtf1_file = argv[1];
  char	*dtf2_file = argv[2];
  char	*signal_file = argv[3];
  char	*noise_file = argv[4];
  char	*out_file = argv[5];
  char	*in_file = NULL;

  if(argc == 7)
    in_file = argv[6];

  NaOpenLogFile("dwtest.log");

  try{
    int			n;
    NaReal		e2, e;
    NaTransFunc		dtf1, dtf2;
    NaConfigPart	*conf1_list[] = { &dtf1 };
    NaConfigPart	*conf2_list[] = { &dtf2 };
    NaConfigFile	conf1_file(";NeuCon transfer", 1, 0);
    NaConfigFile	conf2_file(";NeuCon transfer", 1, 0);
    conf1_file.AddPartitions(NaNUMBER(conf1_list), conf1_list);
    conf2_file.AddPartitions(NaNUMBER(conf2_list), conf2_list);
    conf1_file.LoadFromFile(dtf1_file);
    conf2_file.LoadFromFile(dtf2_file);

    NaDataFile	*dfSignal = OpenInputDataFile(signal_file);
    NaDataFile	*dfNoise = OpenInputDataFile(noise_file);
    NaDataFile	*dfOut = OpenOutputDataFile(out_file, bdtAuto, 1);
    NaDataFile	*dfIn = NULL;

    if(NULL != in_file)
      dfIn = OpenOutputDataFile(in_file, bdtAuto, 1);

    dfSignal->GoStartRecord();
    dfNoise->GoStartRecord();
    dtf1.Reset();
    dtf2.Reset();
    n = 0;
    e = 0.;
    e2 = 0.;
    do{
      ++n;
      NaReal	fSignal, fNoise, fIn, fOut, fTmp;
      fSignal = dfSignal->GetValue();
      fNoise = dfNoise->GetValue();

      fIn = fSignal + fNoise - e;
      dtf1.Function(&fIn, &fTmp);
      dtf2.Function(&fTmp, &fOut);

      if(NULL != dfIn)
	{
	  dfIn->AppendRecord();
	  dfIn->SetValue(fIn);
	}

      dfOut->AppendRecord();
      dfOut->SetValue(fOut);

      e = fOut;
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
