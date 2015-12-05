/* randunified.cpp */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <NaMath.h>

#define __NaSharedExternFunction
#include "randunified.h"


///-----------------------------------------------------------------------
/// Create local external function
NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaRandUnifiedFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaRandUnifiedFunc::NaRandUnifiedFunc ()
	: n_ddescr(0), ddescr(NULL)
{
}


///-----------------------------------------------------------------------
/// Add new dimension descriptor to the array
void
NaRandUnifiedFunc::ddescr_addh (const DimDescr& dd)
{
	DimDescr	*pNew = new DimDescr[++n_ddescr];
	if(n_ddescr > 0) {
		memcpy(pNew, ddescr, sizeof(DimDescr) * (n_ddescr - 1));
		delete[] ddescr;
		ddescr = pNew;
	}
	ddescr[n_ddescr - 1] = dd;
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options (numbers): Amin1 Amax1 Amaxstep1 [Amin2 Amax2 Amaxstep2 ...]
/// where:
///  - AminN     - low bound of output value
///  - AmaxN     - high bound of output value
///  - AmaxstepN - maximum random step (absolute value; 0.0 - no limit)
/// initial: Afirst1 [Afirst2 [Afirst3 ...]]
///  - AfirstN   - to start with (middle point between Amin and
///                Amax by default)
///  - N      - dimension index for multiple outputs
NaRandUnifiedFunc::NaRandUnifiedFunc (char* szOptions, NaVector& vInit)
{
    char	*szToken, *szRest, *szThis = strdup(szOptions);
    NaReal	fTest;
    long	nTest;

    DimDescr	dd;

    szToken = strtok(szThis, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	dd.Amin = fTest;

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	dd.Amax = fTest;

    if(dd.Amin >= dd.Amax) {
	NaReal Atmp = dd.Amin;
	dd.Amin = dd.Amax;
	dd.Amax = Atmp;
    }

    // Middle point by default
    dd.Afirst = (dd.Amax + dd.Amin) * 0.5;
    if(vInit.dim() > 0) {
	dd.Afirst = vInit[0];
	if(dd.Afirst < dd.Amin)
	    dd.Afirst = dd.Amin;
	if(dd.Afirst > dd.Amax)
	    dd.Afirst = dd.Amax;
    }

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	dd.Amaxstep = fabs(fTest);
    else
	dd.Amaxstep = (dd.Amax - dd.Amin) * 0.5;
    if(dd.Amaxstep <= 0.0)
      dd.Amaxstep = (dd.Amax - dd.Amin) * 0.5;

    ddescr_addh(dd);

    // Rest dimensions
    while(NULL != szToken) {

	szToken = strtok(NULL, " ");
	if(NULL == szToken) break;
	fTest = strtod(szToken, &szRest);
	if(szToken != szRest)
	    dd.Amin = fTest;

	szToken = strtok(NULL, " ");
	if(NULL == szToken) break;
	fTest = strtod(szToken, &szRest);
	if(szToken != szRest)
	    dd.Amax = fTest;

	szToken = strtok(NULL, " ");
	if(NULL == szToken) break;
	fTest = strtod(szToken, &szRest);
	if(szToken != szRest)
	    dd.Amaxstep = fTest;
	if(dd.Amaxstep <= 0.0)
	  dd.Amaxstep = (dd.Amax - dd.Amin) * 0.5;

	dd.Afirst = (dd.Amax + dd.Amin) * 0.5;

	if(vInit.dim() >= n_ddescr) {
	  dd.Afirst = vInit[n_ddescr];
	  if(dd.Afirst < dd.Amin)
	    dd.Afirst = dd.Amin;
	  if(dd.Afirst > dd.Amax)
	    dd.Afirst = dd.Amax;
	}

	ddescr_addh(dd);
    }

    for(unsigned i = 0; i < n_ddescr; ++i) {
	NaPrintLog("randunified: [%u] Amin=%g Amax=%g Amaxstep=%g Afirst=%g\n",
		   i+1, ddescr[i].Amin, ddescr[i].Amax, ddescr[i].Amaxstep, ddescr[i].Afirst);
    }

    Assign(1, n_ddescr);

    free(szThis);
}


///-----------------------------------------------------------------------
/// Destructor
NaRandUnifiedFunc::~NaRandUnifiedFunc ()
{
	delete[] ddescr;
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

    for(unsigned i = 0; i < n_ddescr; ++i) {
	ddescr[i].Aprev = ddescr[i].Afirst;
    }
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
    for(unsigned i = 0; i < n_ddescr; ++i) {

      NaReal Alow = ddescr[i].Aprev - ddescr[i].Amaxstep;
      NaReal Ahigh = ddescr[i].Aprev + ddescr[i].Amaxstep;

      if(Alow < ddescr[i].Amin) {
	Alow = ddescr[i].Amin;
	Ahigh = ddescr[i].Amin + 2*ddescr[i].Amaxstep;
      }
      if(Ahigh > ddescr[i].Amax) {
	Alow = ddescr[i].Amax - 2*ddescr[i].Amaxstep;
	Ahigh = ddescr[i].Amax;
      }
      ddescr[i].Aprev = rand_unified(Alow, Ahigh);

      *y = ddescr[i].Aprev;
    }
}
