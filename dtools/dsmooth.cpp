/* dsmooth.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaDataIO.h>


/***********************************************************************
 * Read discrete signal, smooth it by mean value and print to output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 3)
    {
      fprintf(stderr, "Error: need arguments\n");
      fprintf(stderr, "Usage: dsmooth SmoothBase SignalSeries\n");
      return 1;
    }

  NaOpenLogFile("dsmooth.log");

  try{
    int		nBase = atoi(argv[1]);
    NaDataFile	*dfSeries = OpenInputDataFile(argv[2]);
    if(NULL == dfSeries)
      throw(na_cant_open_file);

    NaReal	fSum, *fBaseAr = new NaReal[nBase];
    int		nOutput = 0, nBaseAr = 0;
    bool	bNotEOF;

    for(bNotEOF = dfSeries->GoStartRecord();
	bNotEOF;
	bNotEOF = dfSeries->GoNextRecord())
      {
	int	i;
	NaReal	fSum = 0.0, fAvg;

	if(nBaseAr >= nBase){
	  /* shift values in buffer */
	  memmove(fBaseAr, fBaseAr+1, (nBaseAr-1) * sizeof(NaReal));
	}else{
	  ++nBaseAr;
	}
	/* put to the last slot */
	fBaseAr[nBaseAr-1] = dfSeries->GetValue();

	/* do smoothing */
	if(nBaseAr <= 2){
	  /* simple average */
	  fSum = 0.0;
	  for(i = 0; i < nBaseAr; ++i){
	    fSum += fBaseAr[i];
	  }
	  fAvg = fSum / nBaseAr;

	  if(0 == nOutput){
	    while(nOutput < nBaseAr){
	      printf("%g\n", fAvg);
	      ++nOutput;
	    }
	  }else{
	    printf("%g\n", fAvg);
	    ++nOutput;
	  }
	}else /* if(nBaseAr > 2) */{
	  /* strip minimum and maximum */

	  /* find index of minimum and maxtimum */
	  int	iMin = 0, iMax = 0;
	  for(i = 1; i < nBaseAr; ++i){
	    if(fBaseAr[i] < fBaseAr[iMin]){
	      iMin = i;
	    }
	    if(fBaseAr[i] > fBaseAr[iMax]){
	      iMax = i;
	    }
	  }

	  /* simple average on the rest of numbers */
	  fSum = 0.0;
	  for(i = 0; i < nBaseAr; ++i){
	    if(i != iMin && i != iMax){
	      fSum += fBaseAr[i];
	    }
	  }
	  fAvg = fSum / (nBaseAr - 2);

	  if(0 == nOutput){
	    while(nOutput < nBaseAr){
	      printf("%g\n", fAvg);
	      ++nOutput;
	    }
	  }else{
	    printf("%g\n", fAvg);
	    ++nOutput;
	  }
	}
      }/* end of file */

    delete[] fBaseAr;
    delete dfSeries;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
