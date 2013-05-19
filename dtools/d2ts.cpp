/* d2ts.cpp */
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
#include <NaDataIO.h>


/***********************************************************************
 * Supply discrete data series with time ticks.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 4 && argc != 5)
    {
      fprintf(stderr, "Error: need 3 or 4 arguments\n");
      printf("Usage: d2ts InSeries OutSeries SamplingRate [StartTime]\n");
      return 1;
    }

  char	*in_file = argv[1];
  char	*out_file = argv[2];
  float	dt = atof(argv[3]);
  float	t0 = 0.;
  if(argc == 5)
    t0 = atof(argv[4]);

  NaOpenLogFile("d2ts.log");

  try{
    NaDataFile	*dfIn = OpenInputDataFile(in_file);
    NaDataFile	*dfOut = OpenOutputDataFile(out_file, bdtAuto, 1);
    if(NULL == dfOut) {
	fprintf(stderr, "Failed to create '%s'; maybe, unknown extension?\n",
		out_file);
	return 2;
    }
    NaReal	t;

    dfIn->GoStartRecord();
    t = t0;
    do{
      NaReal	fIn, fOut;
      fIn = dfIn->GetValue();
      fOut = fIn;

      dfOut->AppendRecord();
      dfOut->SetValue(t, 0);
      dfOut->SetValue(fOut, 1);

      t += dt;
    }while(dfIn->GoNextRecord());

    delete dfOut;
    delete dfIn;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
