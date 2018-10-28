/* dsum.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>
#include <NaVector.h>

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

  int		i, n = argc - 1, nColumns = 0;
  NaDataFile	**dfSeries = new NaDataFile*[n];

  NaOpenLogFile("dsum.log");

  for(i = 0; i < n; ++i)
    {
      try{
	dfSeries[i] = OpenInputDataFile(argv[1+i]);
	if(NULL != dfSeries[i]) {
	  dfSeries[i]->GoStartRecord();

	  int	nVar;
	  char	**pszVarNames;
	  dfSeries[i]->GetVarNameList(nVar, pszVarNames);

	  nColumns = std::max(nColumns, nVar);
	  NaPrintLog("File '%s', variables: %d\n", argv[1+i], nVar);
	  for(int j = 0; j < nVar; ++j) {
	      NaPrintLog("  Var '%s'\n", pszVarNames[j]);
	      delete[] pszVarNames[j];
	  }
	  delete[] pszVarNames;
	}
      }
      catch(NaException& ex){
	NaPrintLog("Failed to open '%s' due to %s\n",
		   argv[1+i], NaExceptionMsg(ex));
	dfSeries[i] = NULL;
      }
    }

  try{
    NaVector	fSum(nColumns);
    bool	bEOF = false;
    do{
	fSum.init_zero();

	for(i = 0; i < n; ++i)
	    if(NULL != dfSeries[i]) {
		for(int j = 0; j < nColumns; ++j) {
		    try{
			fSum[j] += dfSeries[i]->GetValue(j);
		    }
		    catch(...) {
			// skip na_out_of_range
		    }
		}
		if(!dfSeries[i]->GoNextRecord())
		    bEOF = true;
	    }

	for(int j = 0; j < nColumns; ++j) {
	    if(j == nColumns - 1)
		printf("%g\n", fSum[j]);
	    else
		printf("%g\t", fSum[j]);
	}

    } while(!bEOF);

    for(i = 0; i < n; ++i)
      delete dfSeries[i];

    delete dfSeries;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  NaCloseLogFile();

  return 0;
}
