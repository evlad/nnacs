/* polynomial.cpp */

#include <vector>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define __NaSharedExternFunction
#include "polynomial.h"


///-----------------------------------------------------------------------
/// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaPolynomialFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaPolynomialFunc::NaPolynomialFunc ()
{
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options: k[n] k[n-1] ... k[0]
///  -  n    - polynom order
///  -  k[i] - coefficients
/// initial: -
NaPolynomialFunc::NaPolynomialFunc (char* szOptions, NaVector& vInit)
{
    char	*szThis = strdup(szOptions), *szRest, *szToken;
    NaReal	fTest;

    for(szToken = strtok(szThis, " ");
	NULL != szToken;
	szToken = strtok(NULL, " ")) {
	// Check for proper floating point number
	fTest = strtod(szToken, &szRest);
	if(szRest == szToken)
	    continue;
	vfK.addl(fTest);
    }
    free(szThis);

    NaPrintLog("polynomial coefficients:");
    for(int i = vfK.count()-1; i >= 0; --i) {
	NaPrintLog(" k[%d]=%g", i, vfK[i]);
    }
    NaPrintLog("\n");
}


///-----------------------------------------------------------------------
/// Destructor
NaPolynomialFunc::~NaPolynomialFunc ()
{
}


///-----------------------------------------------------------------------
/// Reset operations, that must be done before new modelling
/// session will start.  It's guaranteed that this reset will be
/// called just after Timer().ResetTime().
void
NaPolynomialFunc::Reset ()
{
}


///-----------------------------------------------------------------------
/// Compute output on the basis of internal parameters,
/// stored state and external input: y=F(x,t,p)
void
NaPolynomialFunc::Function (NaReal* x, NaReal* y)
{
    NaReal	xi = 1;
    if(vfK.count() == 0)
	*y = *x;
    else {
	*y = vfK[0];
	NaReal xi = 1;
	for(int i = 1; i < vfK.count(); ++i) {
	    xi *= *x;
	    *y += vfK[i] * xi;
	}
    }
}
