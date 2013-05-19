/* dummy.cpp */
static char rcsid[] = "$Id$";

#include <stdlib.h>

#define __NaSharedExternFunction
#include "dummy.h"


//-----------------------------------------------------------------------
// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaDummyFunc(szOptions, vInit);
}


//-----------------------------------------------------------------------
// Make empty (y=x) function
NaDummyFunc::NaDummyFunc ()
{
  // Nothing to do more
}


//-----------------------------------------------------------------------
// Make empty function with given options and initial vector
// options: Anything
// initial: Anything
NaDummyFunc::NaDummyFunc (char* szOptions,
			  NaVector& vInit)
{
    NaPrintLog("dummy: options='%s'\n", szOptions);
    NaPrintLog("dummy: initial:");
    for(int i = 0; i < vInit.dim(); ++i) {
	NaPrintLog(" %g", vInit[i]);
    }
    NaPrintLog("\n");
}


//-----------------------------------------------------------------------
// Destructor
NaDummyFunc::~NaDummyFunc ()
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaDummyFunc::Reset ()
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// ATTENTION!  It's guaranteed that this->Function() will be called
// only once for each time step.  Index of the time step and current
// time can be got from Timer().
//-----------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaDummyFunc::Function (NaReal* x, NaReal* y)
{
  if(NULL == x || NULL == y)
    return;

  *y = *x;
}
