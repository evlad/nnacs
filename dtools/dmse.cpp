/* dmse.cpp */
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
 * Read discrete signal and observed output and compute continous mean
 * squared error signal.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 2 && argc != 3)
    {
      fprintf(stderr, "Error: need 1 or 2 arguments\n");
      fprintf(stderr,
	      "Usage: DMSE_DELAY=Delay dmse SignalSeries [ObservSeries]\n");
      return 1;
    }

  char	*szSignalFile = argv[1];
  char	*szObservedFile = (argc == 2) ? NULL : argv[2];

  NaOpenLogFile("dmse.log");

  try{
    NaDataFile	*dfSignal = OpenInputDataFile(szSignalFile);
    NaDataFile	*dfObserved =
      (szObservedFile != NULL) ? OpenInputDataFile(szObservedFile) : NULL;

    if(NULL == dfSignal || (NULL != szObservedFile
			    && NULL == dfObserved))
      throw(na_cant_open_file);

    NaReal	fMSE = 0.;
    unsigned	nSamples = 0;

    dfSignal->GoStartRecord();
    if(dfObserved != NULL)
      dfObserved->GoStartRecord();

    if(dfObserved != NULL)
      {
	char *p = getenv("DMSE_DELAY");
	if(NULL != p)
	  {
	    int	i, n = atoi(p);
	    for(i = 0; i < n; ++i)
	      /* avoid delay between signal and observation */
	      dfObserved->GoNextRecord();
	  }
      }

    do{
      NaReal	fSignal, fObserved;
      fSignal = dfSignal->GetValue();
      if(dfObserved != NULL)
	fObserved = dfObserved->GetValue();
      else
	fObserved = 0.;

      fMSE += (fSignal - fObserved) * (fSignal - fObserved);
      ++nSamples;

      printf("%d\t%g\n", nSamples, fMSE / nSamples);

      if(NULL != dfObserved)
	if(!dfObserved->GoNextRecord())
	  break;
    }while(dfSignal->GoNextRecord());

    if(NULL != dfObserved)
      delete dfObserved;
    delete dfSignal;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
