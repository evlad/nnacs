/* djacob.cpp */
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
 * Read input and observation of some system and compute discrete
 * jacobian estimation.  Output contains Delta u(k), Delta y(k) and
 * Jacobian(k)=Delta u(k)/Delta y(k)
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 3)
    {
      fprintf(stderr, "Error: need 2 arguments\n");
      fprintf(stderr, "Usage: djacob u-Series y-Series\n");
      fprintf(stderr, "  Output contains du dy J=dy/du\n");
      return 1;
    }

  char	*in_file = argv[1];
  char	*out_file = argv[2];

  NaOpenLogFile("djacob.log");

  try{
    NaDataFile	*dfIn = OpenInputDataFile(in_file);
    NaDataFile	*dfOut = OpenInputDataFile(out_file);

    if(NULL == dfIn)
      {
	fprintf(stderr, "Error: can't open file '%s'\n", in_file);
	throw(na_cant_open_file);
      }

    if(NULL == dfOut)
      {
	fprintf(stderr, "Error: can't open file '%s'\n", out_file);
	throw(na_cant_open_file);
      }

    dfIn->GoStartRecord();
    dfOut->GoStartRecord();

    NaReal	fInPrev = dfIn->GetValue(),
		fOutPrev = dfOut->GetValue();

    while(dfIn->GoNextRecord() && dfOut->GoNextRecord())
      {
	NaReal	fIn = dfIn->GetValue(),
		fOut = dfOut->GetValue();

	printf("%g\t%g\t%g\n",
	       fIn - fInPrev, fOut - fOutPrev,
	       (fOut - fOutPrev) / (fIn - fInPrev));

	fInPrev = fIn;
	fOutPrev = fOut;
      }

    delete dfOut;
    delete dfIn;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
