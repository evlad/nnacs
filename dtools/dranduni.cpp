/* dranduni.cpp */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaMath.h>
#include <NaConfig.h>
#include <NaTrFunc.h>
#include <NaDataIO.h>


/***********************************************************************
 * Generates randomized series with unified distribution in given
 * range from min to max.  Puts the series to output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 4)
    {
      fprintf(stderr,
	      "Usage: drand Length Min Max\n"\
	      "DRAND_SAFE=something to avoid the same seed in srand()\n");
      return 1;
    }

  NaOpenLogFile("dranduni.log");

  int		length = atol(argv[1]);
  double	min = atof(argv[2]);
  double	max = atof(argv[3]);

  try{
    int		i;

    // See DRAND_SAFE to prevent dependent random series
    reset_rand();

    for(i = 0; i < length; ++i)
      {
	NaReal	fRand;
	fRand = rand_unified(min, max);
	printf("%g\n", fRand);
      }
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
