/* dsum.cpp */
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
 * Read discrete signals and prints sum to output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc == 1)
    {
      fprintf(stderr, "Error: need arguments\n");
      fprintf(stderr, "Usage: dsum SignalSeries1 [SignalSeries2 ...]\n");
      return 1;
    }

  int		i, n = argc - 1;
  NaDataFile	**dfSeries = new NaDataFile*[n];

  NaOpenLogFile("dsum.log");

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
    NaReal	fSum;
    bool	bEOF = false;
    do{
      fSum = 0.0;

      for(i = 0; i < n; ++i)
	if(NULL != dfSeries[i])
	  {
	    fSum += dfSeries[i]->GetValue();
	    if(!dfSeries[i]->GoNextRecord())
	      bEOF = true;
	  }

      printf("%g\n", fSum);

    }while(!bEOF);

    for(i = 0; i < n; ++i)
      delete dfSeries[i];

    delete dfSeries;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
