/* dsin.cpp */
static char rcsid[] = "$Id$";

// Especially to get defined M_PI for MSVC
#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>


/***********************************************************************
 * Generate sine of given length, with given period and phase shift.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(3 != argc && 4 != argc)
    {
      fprintf(stderr, "Usage: dsin Length Period [PhaseShift]\n");
      return 1;
    }

  int		nLength = atoi(argv[1]);
  double	fPeriod = atof(argv[2]);
  double	fPhase = (4 == argc)? atof(argv[3]): 0.0;

  double	fFreq = 2 * M_PI / fPeriod;

  int	i;
  for(i = 0; i < nLength; ++i)
    {
      printf("%g\n", sin(fFreq * (i + fPhase)));
    }

  return 0;
}
