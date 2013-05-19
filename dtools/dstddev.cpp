/* dstddev.cpp */
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
 * Read discrete signals and prints standard deviation value of them to
 * output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc == 1)
    {
      fprintf(stderr, "Error: need arguments\n");
      fprintf(stderr, "Usage: DSTDDEV_DIM=m dstddev SignalSeries1 "\
	      "[SignalSeries2 ...]\n");
      return 1;
    }

  int		i, j, n = argc - 1, n_act, m = 1;
  NaDataFile	**dfSeries = new NaDataFile*[n];

  NaOpenLogFile("dstddev.log");

  if(NULL != getenv("DSTDDEV_DIM"))
    if(0 != strlen(getenv("DSTDDEV_DIM")))
      m = atoi(getenv("DSTDDEV_DIM"));

  NaPrintLog("Dimension of input signals is %d\n.", m);

  for(i = 0; i < n; ++i)
    {
      try{
	dfSeries[i] = OpenInputDataFile(argv[1+i]);
	if(NULL != dfSeries[i])
	  dfSeries[i]->GoStartRecord();
      }
      catch(NaException& ex){
	NaPrintLog("Failed to open '%s' due to %s\n",
		   argv[1+i], NaExceptionMsg(ex));
	dfSeries[i] = NULL;
      }
    }

  try{
    NaReal	*fSum = new NaReal[m];
    NaReal	*fSum2 = new NaReal[m];
    do{
      n_act = 0;

      for(j = 0; j < m; ++j)
	{
	  fSum[j] = 0.0;
	  fSum2[j] = 0.0;
	}

      /* for each input stream */
      for(i = 0; i < n; ++i)
	if(NULL != dfSeries[i])
	  {
	    /* read all values (dimensions) of the stream */
	    for(j = 0; j < m; ++j)
	      {
		NaReal	fVal = dfSeries[i]->GetValue(j);
		fSum[j] += fVal;
		fSum2[j] += fVal * fVal;
	      }

	    /* one more item from stream */
	    ++n_act;

	    /* check for EOF of the stream */
	    if(!dfSeries[i]->GoNextRecord())
	      {
		delete dfSeries[i];
		dfSeries[i] = NULL;
	      }
	  }

      /* compute and printout the mean value */
      if(n_act > 0)
	for(j = 0; j < m; ++j)
	  {
	    NaReal	fStdDev, fMean;

	    fMean = fSum[j] / n_act;
	    fStdDev = sqrt(fSum2[j] / n_act - fMean * fMean);

	    if(j == m - 1)
	      printf("%g\n", fStdDev);
	    else
	      printf("%g\t", fStdDev);
	  }

    }while(n_act > 0);

    for(i = 0; i < n; ++i)
      delete dfSeries[i];

    delete dfSeries;
    delete fSum;
    delete fSum2;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
