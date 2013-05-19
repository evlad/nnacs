/* randmeander.cpp */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <NaMath.h>

#define __NaSharedExternFunction
#include "randmeander.h"


///-----------------------------------------------------------------------
/// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaRandMeanderFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaRandMeanderFunc::NaRandMeanderFunc ()
    : Amin(0.0), Amax(0.0), Lconst(0), Lmin(0), Lmax(0),
      Lcounter(0), Aconst(0.0)
{
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options (numbers): Amin Amax Lconst
///  or
/// options (numbers): Amin Amax Lmin Lmax
/// where:
///  - Amin   - for uniform distribution of amplitudes
///  - Amax   - for uniform distribution of amplitudes
///  - Lmin   - for uniform distribution of length of constant amplitude
///  - Lmax   - for uniform distribution of length of constant amplitude
///  - Lconst - constant length of constant amplitude
/// initial: not used
NaRandMeanderFunc::NaRandMeanderFunc (char* szOptions, NaVector& vInit)
    : Amin(0.0), Amax(0.0), Lconst(0), Lmin(0), Lmax(0),
      Lcounter(0), Aconst(0.0)
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

    szToken = strtok(NULL, " ");
    nTest = strtol(szToken, &szRest, 0);
    if(szToken != szRest)
	Lmin = nTest;

    szToken = strtok(NULL, " ");
    if(NULL == szToken) {
	Lconst = Lmin;
	Lmin = 0;
    } else {
	nTest = strtol(szToken, &szRest, 0);
	if(szToken != szRest)
	    Lmax = nTest;
    }

    NaPrintLog("randmeander: Amin=%g Amax=%g Lmin=%d Lmax=%d Lconst=%d\n",
	       Amin, Amax, Lmin, Lmax, Lconst);

    free(szThis);
}


///-----------------------------------------------------------------------
/// Destructor
NaRandMeanderFunc::~NaRandMeanderFunc ()
{
}


///-----------------------------------------------------------------------
/// Reset operations, that must be done before new modelling
/// session will start.  It's guaranteed that this reset will be
/// called just after Timer().ResetTime().
void
NaRandMeanderFunc::Reset ()
{
    // See DRAND_SAFE to prevent dependent random series
    reset_rand();

    Lcounter = 0;
}


///-----------------------------------------------------------------------
/// Compute output on the basis of internal parameters,
/// stored state: y=F(t,p)  INPUT x IS NOT USED!
void
NaRandMeanderFunc::Function (NaReal* x, NaReal* y)
{
    if(NULL == x || NULL == y)
	return;

    if(Lcounter <= 0) {
	if(Lconst > 0) {
	    Lcounter = Lconst;
	} else {
	    Lcounter = rand_unified(Lmin, Lmax + 1);
	    if(Lcounter <= 0)
		Lcounter = 1;
	}
	Aconst = rand_unified(Amin, Amax);
    }
    --Lcounter;
    *y = Aconst;
}
