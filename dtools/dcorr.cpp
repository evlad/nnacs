/* dcorr.cpp */
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
 * Read one or two discrete signal series and compute autocorrelation
 * or correlation series.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 3 && argc != 4)
    {
      fprintf(stderr, "Error: need 2 or 3 arguments\n");
      fprintf(stderr, "Usage: dcorr Length SignalSeries1 [SignalSeries2]\n");
      return 1;
    }

  int	length = atoi(argv[1]);
  char	*signal1_file = argv[2];
  char	*signal2_file = (argc == 4)? argv[3]: argv[2];

  NaOpenLogFile("dcorr.log");

  try{
    NaDataFile	*dfSignal1 = OpenInputDataFile(signal1_file);
    NaDataFile	*dfSignal2 = OpenInputDataFile(signal2_file);
    int		i, j, n1, n2, nSamples = 0;
    NaReal	*fSeries1, *fSeries2;
    NaReal	fMean1 = 0., fMean2 = 0.;

    if(0 == dfSignal1->CountOfRecord() ||
       0 == dfSignal2->CountOfRecord())
      throw(na_size_mismatch);

    /* allocate arrays for fast computation */
    n1 = dfSignal1->CountOfRecord();
    fSeries1 = new NaReal[n1];
    if(!strcmp(signal1_file, signal2_file))
      {
	n2 = n1;
	fSeries2 = fSeries1;
      }
    else
      {
	n2 = dfSignal2->CountOfRecord();
	fSeries2 = new NaReal[n2];
      }

    /* read series to the array and compute mean value by the way */
    for(dfSignal1->GoStartRecord(), i = 0; i < n1;
	dfSignal1->GoNextRecord(), ++i)
      {
	fSeries1[i] = dfSignal1->GetValue();
	fMean1 += fSeries1[i];
      }
    fMean1 /= n1;

    for(dfSignal2->GoStartRecord(), i = 0; i < n2;
	dfSignal2->GoNextRecord(), ++i)
      {
	fSeries2[i] = dfSignal2->GetValue();
	fMean2 += fSeries2[i];
      }
    fMean2 /= n2;

    nSamples = n1;
    if(nSamples < n2)
      nSamples = n2;

    /* mutual correlation */
    for(j = 0; j < length; ++j)
      {
	NaReal	fCOR = 0.;

	/* compose sum */
	for(i = 0; i < nSamples - j; ++i)
	  {
	    fCOR += (fSeries1[i] - fMean1) * (fSeries2[i + j] - fMean2);
	  }

	fCOR /= nSamples - j;

	printf("%g\n", fCOR);
      }

    if(fSeries1 != fSeries2)
      delete fSeries2;
    delete fSeries1;

    delete dfSignal2;
    delete dfSignal1;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
