/* randunified.cpp */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <NaMath.h>

#define __NaSharedExternFunction
#include "randunified.h"


///-----------------------------------------------------------------------
/// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaRandUnifiedFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaRandUnifiedFunc::NaRandUnifiedFunc ()
    : Amin(0.0), Amax(0.0), Amaxstep(0), Aprev(0)
{
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options (numbers): Amin Amax Amaxstep
/// where:
///  - Amin     - low bound of output value
///  - Amax     - high bound of output value
///  - Amaxstep - maximum random step (absolute value)
/// initial: [Afirst]
///  - Afirst   - to start with (middle point between Amin and
///               Amax by default)
NaRandUnifiedFunc::NaRandUnifiedFunc (char* szOptions, NaVector& vInit)
    : Amin(0.0), Amax(0.0), Amaxstep(0.0), Afirst(0.0), Aprev(0.0)
{
    char	*szToken, *szRest, *szThis = strdup(szOptions);
    NaReal	fTest;
    long	nTest;

    szToken = strtok(szThis, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	Amin = fTest;

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	Amax = fTest;

    if(Amin >= Amax) {
	NaReal Atmp = Amin;
	Amin = Amax;
	Amax = Atmp;
    }

    // Middle point by default
    Afirst = (Amax + Amin) * 0.5;

    if(vInit.dim() > 0) {
	Afirst = vInit[0];
	if(Afirst < Amin)
	    Afirst = Amin;
	if(Afirst > Amax)
	    Afirst = Amax;
    }

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	Amaxstep = fabs(fTest);
    else
	Amaxstep = (Amax - Amin) * 0.5;

    NaPrintLog("randunified: Amin=%g Amax=%g Amaxstep=%g Afirst=%g\n",
	       Amin, Amax, Amaxstep, Afirst);

    free(szThis);
}


///-----------------------------------------------------------------------
/// Destructor
NaRandUnifiedFunc::~NaRandUnifiedFunc ()
{
}


///-----------------------------------------------------------------------
/// Reset operations, that must be done before new modelling
/// session will start.  It's guaranteed that this reset will be
/// called just after Timer().ResetTime().
void
NaRandUnifiedFunc::Reset ()
{
    // See DRAND_SAFE to prevent dependent random series
    reset_rand();

    Aprev = Afirst;
}


///-----------------------------------------------------------------------
/// Compute output on the basis of internal parameters,
/// stored state: y=F(t,p)  INPUT x IS NOT USED!
void
NaRandUnifiedFunc::Function (NaReal* x, NaReal* y)
{
    if(NULL == x || NULL == y)
	return;

    // Current limits for random step
    NaReal Alow = Aprev - Amaxstep;
    NaReal Ahigh = Aprev + Amaxstep;

    if(Alow < Amin) {
	Alow = Amin;
	Ahigh = Amin + 2*Amaxstep;
    }
    if(Ahigh > Amax) {
	Alow = Amax - 2*Amaxstep;
	Ahigh = Amax;
    }
    Aprev = rand_unified(Alow, Ahigh);

    *y = Aprev;
}
