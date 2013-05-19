/* drandmea.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include <NaMath.h>
#include <NaConfig.h>
#include <NaTrFunc.h>
#include <NaDataIO.h>


/***********************************************************************
 * Generates series with quasi-meander with randomized amplitude and
 * given or randomized half period.  Puts the series to output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 5 && argc != 6)
    {
      fprintf(stderr,
	      "Usage: drandmea Length HalfPeriod MinAmpl MaxAmpl\n"\
	      "   or  drandmea Length MinHalfPer MaxHalfPer MinAmpl MaxAmpl\n"\
	      "DRAND_SAFE=something to avoid the same seed in srand()\n");
      return 1;
    }

  double	fMinAmpl, fMaxAmpl;
  int		nLength, nHalfPeriod, nMinHalfPer, nMaxHalfPer;

  nLength = atoi(argv[1]);
  if(nLength <= 0)
    {
      fprintf(stderr, "Length must be positive\n");
      return 2;
    }

  if(argc == 5)
    {
      nHalfPeriod = atoi(argv[2]);
      if(nHalfPeriod <= 0)
	{
	  fprintf(stderr, "HalfPeriod must be positive\n");
	  return 2;
	}
      fMinAmpl = atof(argv[3]);
      fMaxAmpl = atof(argv[4]);
    }
  else
    {
      nMinHalfPer = atoi(argv[2]);
      nMaxHalfPer = atoi(argv[3]);
      if(nMinHalfPer <= 0 || nMaxHalfPer <= 0)
	{
	  fprintf(stderr, "MinHalfPer and nMaxHalfPer must be positive\n");
	  return 2;
	}
      fMinAmpl = atof(argv[4]);
      fMaxAmpl = atof(argv[5]);
    }

  // See DRAND_SAFE to prevent dependent random series
  reset_rand();

  NaReal	fAmpl = rand_unified(fMinAmpl, fMaxAmpl);
  if(argc == 6)
    {
      nHalfPeriod = rand_unified(nMinHalfPer, nMaxHalfPer + 1);
      if(nHalfPeriod <= 0)
	nHalfPeriod = 1;
    }

  int		i = 0, j = 0;
  while(i < nLength)
    {
      if(j < nHalfPeriod)
	{
	  printf("%g\n", fAmpl);
	  ++j;
	}
      else
	{
	  fAmpl = rand_unified(fMinAmpl, fMaxAmpl);
	  if(argc == 6)
	    {
	      nHalfPeriod = rand_unified(nMinHalfPer, nMaxHalfPer + 1);
	      if(nHalfPeriod <= 0)
		nHalfPeriod = 1;
	    }
	  j = 0;
	  continue;
	}
      ++i;
    }

  return 0;
}
