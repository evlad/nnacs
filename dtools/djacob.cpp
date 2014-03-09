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
 * Jacobian(k)=Delta y(k)/Delta u(k)
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 3)
    {
      fprintf(stderr,
	      "Error: need 2 arguments\n"				\
	      "Usage: DJACOB_DELAY=n djacob u-Series y-Series\n"	\
	      "  Output contains du dy J=dy/du, where\n"		\
	      "  dy=y(k+1)-y(k), du=u(k+1-n)-u(k-n)\n");
      return 1;
    }

  char	*u_file = argv[1];
  char	*y_file = argv[2];

  NaOpenLogFile("djacob.log");

  try{
    NaDataFile	*dfU = OpenInputDataFile(u_file);
    NaDataFile	*dfY = OpenInputDataFile(y_file);

    if(NULL == dfU)
      {
	fprintf(stderr, "Error: can't open file '%s'\n", u_file);
	throw(na_cant_open_file);
      }

    if(NULL == dfY)
      {
	fprintf(stderr, "Error: can't open file '%s'\n", y_file);
	throw(na_cant_open_file);
      }

    dfU->GoStartRecord();
    dfY->GoStartRecord();

    char *p = getenv("DJACOB_DELAY");
    if(NULL != p) {
	int	i, n = atoi(p);
	for(i = 0; i < n; ++i)
	    // Make/avoid delay between u(k) and y(k) series */
	    dfY->GoNextRecord();
    }

    NaReal	fUprev = dfU->GetValue(),
		fYprev = dfY->GetValue();

    while(dfU->GoNextRecord() && dfY->GoNextRecord())
      {
	NaReal	fU = dfU->GetValue(),
		fY = dfY->GetValue();

	printf("%g\t%g\t%g\n",
	       fU - fUprev, fY - fYprev,
	       (fY - fYprev) / (fU - fUprev));

	fUprev = fU;
	fYprev = fY;
      }

    delete dfY;
    delete dfU;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
