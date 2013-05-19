/* deadzone.cpp */
static char rcsid[] = "$Id: deadzone.cpp,v 1.1 2002-02-16 21:34:41 vlad Exp $";

#include <stdlib.h>

#define __NaSharedExternFunction
#include "deadzone.h"


//-----------------------------------------------------------------------
// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaDeadZoneFunc(szOptions, vInit);
}


//-----------------------------------------------------------------------
// Make empty (y=x) function
NaDeadZoneFunc::NaDeadZoneFunc ()
  : fGain(1.0), fHalfWidth(0.0)
{
  // Nothing to do more
}


//-----------------------------------------------------------------------
// Make function with given options and initial vector
NaDeadZoneFunc::NaDeadZoneFunc (char* szOptions,
				NaVector& vInit)
  : fGain(1.0), fHalfWidth(0.0)
{
  char		*szRest, *szGain, *szHalfWidth = szOptions;
  NaReal	fTest;

  fTest = strtod(szHalfWidth, &szGain);
  if(szHalfWidth != szGain)
    fHalfWidth = fTest;

  fTest = strtod(szGain, &szRest);
  if(szGain != szRest)
    fGain = fTest;
}


//-----------------------------------------------------------------------
// Destructor
NaDeadZoneFunc::~NaDeadZoneFunc ()
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaDeadZoneFunc::Reset ()
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
NaDeadZoneFunc::Function (NaReal* x, NaReal* y)
{
  if(NULL == x || NULL == y)
    return;

  if(*x > -fHalfWidth && *x < fHalfWidth){
    // Not sensitive area
    *y = 0.;
  }else if(*x <= -fHalfWidth){
    *y = (*x + fHalfWidth) * fGain;
  }else if(*x >= fHalfWidth){
    *y = (*x - fHalfWidth) * fGain;
  }
}
