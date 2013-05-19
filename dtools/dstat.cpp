/* dstat.cpp */
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
 * Read discrete signals and prepare mean and std.dev. series on output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc == 1)
    {
      fprintf(stderr, "Error: need arguments.\n");
      fprintf(stderr,
	      "Usage: dstat [ColNum] SignalSeries1 [SignalSeries2 ...]\n");
      fprintf(stderr,
	      " DSTAT_EOF=t - stop at EOF in any series\n"\
	      " DSTAT_EOF=f - continue generating resulting series (default)\n");
      fprintf(stderr,
	      "Output: _        2        ____\n"\
	      "        x sd n Sx Sx S|x| S|x|\n");
      return 1;
    }

  char		*p, **args = argv + 1;
  int		i, argn = argc - 1, iCol;
  NaDataFile	**dfSeries = new NaDataFile*[argn];
  bool		bStopEOF = false;
  char		*szStopEOF = getenv("DSTAT_EOF");

  if(NULL != szStopEOF)
    {
      if('t' == szStopEOF[0])
	bStopEOF = true;
      if('f' == szStopEOF[0])
	bStopEOF = false;
    }

  NaOpenLogFile("dstat.log");

  iCol = strtol(argv[1], &p, 10);
  if(argv[1] == p)
    iCol = 0;	/* default column */
  else
    {
      --iCol;	/* index instead of number */
      args = argv + 2;
      --argn;
    }

  for(i = 0; i < argn; ++i)
    {
      try{
	dfSeries[i] = OpenInputDataFile(args[i]);
	if(NULL != dfSeries[i])
	  dfSeries[i]->GoStartRecord();
      }
      catch(NaException& ex){
	NaPrintLog("Failed to open '%s' due to %s\n",
		   args[i], NaExceptionMsg(ex));
	dfSeries[i] = NULL;
      }
    }

  try{
    unsigned	nVal;
    NaReal	fVal, fSum, fSumSq, fSumAbs;
    bool	bEOF = false;
    nVal = 0;
    fSum = 0.0;
    fSumSq = 0.0;
    fSumAbs = 0.0;

    do{
      for(i = 0; i < argn; ++i)
	if(NULL != dfSeries[i])
	  {
	    ++nVal;
	    fVal = dfSeries[i]->GetValue(iCol);
	    fSum += fVal;
	    fSumSq += fVal * fVal;
	    fSumAbs += fabs(fVal);
	    if(!dfSeries[i]->GoNextRecord())
	      {
		if(bStopEOF)
		  bEOF = true;

		delete dfSeries[i];
		dfSeries[i]= NULL;
	      }
	  }

      if(0 == nVal)
	bEOF = true;
      else if(1 == nVal)
	/*       _        2        ____
	 *       x sd n Sx Sx S|x| S|x| */
	printf("%g %g %d %g %g %g %g\n",
	       fSum, 0.0, nVal, fSumSq, fSum, fSumAbs, fSumAbs/nVal);
      else /* if(1 < nVal) */
	/*       _        2        ____
	 *       x sd n Sx Sx S|x| S|x| */
	printf("%g %g %d %g %g %g %g\n",
	       fSum/nVal, sqrt(fSumSq/nVal - (fSum*fSum)/(nVal*nVal)),
	       nVal, fSumSq, fSum, fSumAbs, fSumAbs/nVal);

    }while(!bEOF);

    for(i = 0; i < argn; ++i)
      delete dfSeries[i];

    delete dfSeries;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
