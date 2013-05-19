/* saturat.cpp */
static char rcsid[] = "$Id: saturat.cpp,v 1.1 2003-07-24 06:26:35 vlad Exp $";

#include <stdlib.h>

#define __NaSharedExternFunction
#include "saturat.h"


//-----------------------------------------------------------------------
// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaSaturationFunc(szOptions, vInit);
}


//-----------------------------------------------------------------------
// Make empty (y=x) function
NaSaturationFunc::NaSaturationFunc ()
  : fGain(1.0), fLimit(0.0), fLimit2(0.0)
{
  // Nothing to do more
}


//-----------------------------------------------------------------------
// Make function with given options and initial vector
NaSaturationFunc::NaSaturationFunc (char* szOptions, NaVector& vInit)
  : fGain(1.0), fLimit(0.0), fLimit2(0.0)
{
  char		*szRest, *szLimit2, *szGain, *szLimit = szOptions;
  NaReal	fTest;

  fTest = strtod(szLimit, &szGain);
  if(szLimit != szGain)
    fLimit = fTest;

  fTest = strtod(szGain, &szLimit2);
  if(szGain != szLimit2)
    fGain = fTest;

  fTest = strtod(szLimit2, &szRest);
  if(szLimit2 != szRest)
    fLimit2 = fTest;
  else
    fLimit2 = - fLimit;
}


//-----------------------------------------------------------------------
// Destructor
NaSaturationFunc::~NaSaturationFunc ()
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaSaturationFunc::Reset ()
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
NaSaturationFunc::Function (NaReal* x, NaReal* y)
{
  if(NULL == x || NULL == y)
    return;

  *y = *x * fGain;

  if(fLimit > 0.0)
    {
      if(*y > fLimit)
	*y = fLimit;
      else if(*y < fLimit2)
	*y = fLimit2;
    }
}
