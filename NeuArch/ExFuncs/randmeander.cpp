/* randmeander.cpp */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <NaMath.h>

#define __NaSharedExternFunction
#include "randmeander.h"


///-----------------------------------------------------------------------
/// Create local external function
NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaRandMeanderFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaRandMeanderFunc::NaRandMeanderFunc ()
{
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options (numbers): Amin Amax Lconst
///  or
/// options (numbers): Amin1 Amax1 Lmin1 Lmax1 [Amin2 Amax2 Lmin2 Lmax2 ...]
/// where:
///  - AminN  - for uniform distribution of amplitudes
///  - AmaxN  - for uniform distribution of amplitudes
///  - LminN  - for uniform distribution of length of constant amplitude
///  - LmaxN  - for uniform distribution of length of constant amplitude
///  - Lconst - constant length of constant amplitude (eq. LminN==LmaxN)
///  - N      - dimension index for multiple outputs
/// initial: not used
NaRandMeanderFunc::NaRandMeanderFunc (char* szOptions, NaVector& vInit)
{
    char	*szToken, *szRest, *szThis = strdup(szOptions);
    NaReal	fTest;
    long	nTest;

    DimDescr	dd;

    szToken = strtok(szThis, " ");
    if(NULL != szToken) {
	fTest = strtod(szToken, &szRest);
	if(szToken != szRest)
	    dd.Amin = fTest;
    }

    szToken = strtok(NULL, " ");
    if(NULL != szToken) {
	fTest = strtod(szToken, &szRest);
	if(szToken != szRest)
	    dd.Amax = fTest;
    }

    szToken = strtok(NULL, " ");
    if(NULL != szToken) {
	nTest = strtol(szToken, &szRest, 0);
	if(szToken != szRest)
	    dd.Lmin = nTest;
    }

    szToken = strtok(NULL, " ");
    if(NULL == szToken) {
	dd.Lmax = dd.Lmin;
    } else {
	nTest = strtol(szToken, &szRest, 0);
	if(szToken != szRest)
	    dd.Lmax = nTest;
    }

    ddescr.addh(dd);

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
	nTest = strtol(szToken, &szRest, 0);
	if(szToken != szRest)
	    dd.Lmin = nTest;

	szToken = strtok(NULL, " ");
	if(NULL == szToken) break;
	nTest = strtol(szToken, &szRest, 0);
	if(szToken != szRest)
	    dd.Lmax = nTest;

	ddescr.addh(dd);
    }

    for(unsigned i = 0; i < ddescr.count(); ++i) {
	NaPrintLog("randmeander: [%u] Amin=%g Amax=%g Lmin=%d Lmax=%d\n",
		   i+1, ddescr[i].Amin, ddescr[i].Amax, ddescr[i].Lmin, ddescr[i].Lmax);
    }

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

    for(unsigned i = 0; i < ddescr.count(); ++i) {
	ddescr[i].Lcounter = 0;
    }
}


///-----------------------------------------------------------------------
/// Compute output on the basis of internal parameters,
/// stored state: y=F(t,p)  INPUT x IS NOT USED!
void
NaRandMeanderFunc::Function (NaReal* x, NaReal* y)
{
    if(NULL == y)
	return;

    for(unsigned i = 0; i < ddescr.count(); ++i) {
	if(ddescr[i].Lcounter <= 0) {
	    if(ddescr[i].Lmin == ddescr[i].Lmax) {
		ddescr[i].Lcounter = ddescr[i].Lmin;
	    } else {
		ddescr[i].Lcounter = rand_unified(ddescr[i].Lmin, ddescr[i].Lmax + 1);
		if(ddescr[i].Lcounter <= 0)
		    ddescr[i].Lcounter = 1;
	    }
	    ddescr[i].Aconst = rand_unified(ddescr[i].Amin, ddescr[i].Amax);
	}
	--ddescr[i].Lcounter;
	y[i] = ddescr[i].Aconst;
    }
}
