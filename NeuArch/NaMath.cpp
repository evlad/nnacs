/* NaMath.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <math.h>
#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#include "NaMath.h"
#include "NaLogFil.h"


//---------------------------------------------------------------------------

//////////////////////////////
// Random number generation //
//////////////////////////////

//---------------------------------------------------------------------------
// Setup random generator from the system timer
extern "C" void
reset_rand ()
{
#if 0
  char	*p = getenv("DRAND_SAFE");
  if(NULL != p)
    if(strlen(p) > 0)
      {
		/* wait a bit to prevent random number generator
			initialization with the same seed (time(NULL)) */
		sleep(1);
      }

  /* very standardized but very slow way */
  time_t	tTime = time(NULL);
  srand(tTime);
#endif
#ifdef WIN32
  srand(time(NULL) + clock() * CLOCKS_PER_SEC);
#else
  struct timeval	tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec);
#endif
}


//---------------------------------------------------------------------------
// Unified destributed random number
extern "C" NaReal
rand_unified (NaReal fMin, NaReal fMax)
{
    NaReal v = fMin + (fMax - fMin) * rand() / (NaReal)RAND_MAX;
    return v;
}


//---------------------------------------------------------------------------
// Gaussian normal random number distribution
// Marsaglia-Bray algorithm
extern "C" NaReal
rand_gaussian (NaReal fMean, NaReal fStdDev)
{
    NaReal  u1, u2, s2;

    do{
        u1 = rand_unified(-1, 1);
        u2 = rand_unified(-1, 1);
        s2 = u1 * u1 + u2 * u2;
    }while(s2 >= 1);

    return sqrt(-2 * log(s2)/s2) * u1 * fStdDev + fMean;
}


/*
function RandG(Mean, StdDev: Extended): Extended;
{ Marsaglia-Bray algorithm }
var
  U1, S2: Extended;
begin
  repeat
    U1 := 2*Random - 1;
    S2 := Sqr(U1) + Sqr(2*Random-1);
  until S2 < 1;
  Result := Sqrt(-2*Ln(S2)/S2) * U1 * StdDev + Mean;
end;
*/

/*
See wikipedia.org

#include <stdint.h>
 
#define PHI 0x9e3779b9
 
static uint32_t Q[4096], c = 362436;
 
void init_rand(uint32_t x)
{
	int i;
 
	Q[0] = x;
	Q[1] = x + PHI;
	Q[2] = x + PHI + PHI;
 
	for (i = 3; i < 4096; i++)
		Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
}
 
uint32_t rand_cmwc(void) {
        static uint32_t i = 4095;
        uint64_t t;
 
        i = (i + 1) & 4095;
        t = (18705ULL * Q[i]) + c;
        c = t >> 32;
        Q[i] = 0xfffffffe - t;
 
        return Q[i];
}
*/


/// Correlate two vectors of length with given width xc[2*width+1]
extern "C" void
xcfunction (NaReal* a, NaReal* b, unsigned len, NaReal* xc, unsigned width)
{
    unsigned i, j;
    for(j = 0; j <= 2 * width; ++j)
	xc[j] = 0.0;

    for(i = width; i < len - width; ++i) {
	
    }

}


/** Cross correlation function calculation between F1 and F2 with
    result vector placed to B.

    Input:
    - F1[n] - first vector;
    - F2[n] - second vector;
    - n - number of samples in F1 and F2;
    - ip - maximum shift (recommended value: 0 < ip <= n/2);
    - ind1 - type of CCF: 0-simple CCF; 1-normalized CCF;
    - ind2 - balancing input vectors to average zero level: 0-off, 1-on;

    Output:
    - B[2*ip+1] - resulting CCF;

    
    - ierr - error code; 0 means OK.
*/
extern "C" int
ccf1c (NaReal F1[], NaReal F2[], int n, NaReal B[], int ip, int ind1, int ind2)
{
    int	imax, ierr, i, j, k;
    NaReal sum, average1, average2, b0, r01, r02, zero = 1e-6;

    // Absent data
    if(NULL == F1 || NULL == F2 || NULL == B)
      return 1;

    // Illegal dimension
    if(n <= 0 || ip <= 0)
      return 2;

    // Wrong dimension
    if(2*ip + 1 >= n)
      return 3;
	
    if (ind2 != 0) {
	average1 = 0;
	average2 = 0;

	for(j = 0; j < n; ++j) {
	    average1 += F1[j];
	    average2 += F2[j];
	}
	average1 /= n;
	average2 /= n;

	// Minus average
	for(j = 0; j < n; ++j) {
	    F1[j] -= average1;
	    F2[j] -= average2;
	}
    }

    if (ind1 == 0) {
	for(k = 1; k <= ip; ++k) {
	    sum = 0;
	    for(i = 0; i < n-k; ++i)
		sum += F1[i+k] * F2[i];

	    B[ip-k] = sum / (n-k);
	}

	for(k = 0; k <= ip; ++k) {
	    sum = 0;
	    for(i = 0; i < n-k; ++i)
		sum += F1[i] * F2[i+k];

	    B[ip+k] = sum / (n-k);
	}
    }
    else {
      // Normalized CCF
      for(k = 0; k < ip; ++k) {
	sum = 0.0;
	r01 = 0.0;
	r02 = 0.0;

	for(i = 0; i < n-k; +i) {
	  sum = sum + F1[i+k] * F2[i];
	  r01 += F1[i+k] * F1[i+k];
	  r02 += F2[i] * F2[i];
	}

	b0 = sqrt(r01*r02);
	if(b0 < zero) b0 = zero;

	B[ip-k] = sum / b0;
      }

      for(k = 0; k <= ip; ++k) {
	sum = 0.0;
	r01 = 0.0;
	r02 = 0.0;

	for(i = 0; i < n-k; +i) {
	  sum = sum + F1[i] * F2[i+k];
	  r01 += F1[i] * F1[i];
	  r02 += F2[i+k] * F2[i+k];
	}

	b0 = sqrt(r01*r02);
	if(b0 < zero) b0 = zero;

	B[ip+k] = sum / b0;
      }
    }

    return 0;
}
