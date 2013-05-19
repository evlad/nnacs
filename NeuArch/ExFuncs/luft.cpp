/* luft.cpp */
static char rcsid[] = "$Id: luft.cpp,v 1.1 2003-07-24 09:30:18 vlad Exp $";

#include <stdlib.h>

#define __NaSharedExternFunction
#include "luft.h"


//-----------------------------------------------------------------------
// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaLuftAreaFunc(szOptions, vInit);
}


//-----------------------------------------------------------------------
// Make empty (y=x) function
NaLuftAreaFunc::NaLuftAreaFunc ()
  : fGain(1.0), fHalfWidth(0.0)
{
  // Nothing to do more
}


//-----------------------------------------------------------------------
// Make function with given options and initial vector
NaLuftAreaFunc::NaLuftAreaFunc (char* szOptions, NaVector& vInit)
  : fGain(1.0), fHalfWidth(0.0), fX0(0.0), fY0(0.0)
{
  char		*szRest, *szGain, *szHalfWidth = szOptions;
  NaReal	fTest;

  fTest = strtod(szHalfWidth, &szGain);
  if(szHalfWidth != szGain)
    fHalfWidth = fTest;

  fTest = strtod(szGain, &szRest);
  if(szGain != szRest)
    fGain = fTest;

  if(vInit.dim() > 0)
    fX0 = vInit(0);
  if(vInit.dim() > 1)
    fY0 = vInit(1);
}


//-----------------------------------------------------------------------
// Destructor
NaLuftAreaFunc::~NaLuftAreaFunc ()
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaLuftAreaFunc::Reset ()
{
  fPrevX = fX0;
  fPrevY = fY0;
}


//-----------------------------------------------------------------------
// ATTENTION!  It's guaranteed that this->Function() will be called
// only once for each time step.  Index of the time step and current
// time can be got from Timer().
//-----------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaLuftAreaFunc::Function (NaReal* x, NaReal* y)
{
  if(NULL == x || NULL == y)
    return;

  if(fHalfWidth > 0.0 && fGain != 0.0){
    if(*x > fPrevY / fGain + fHalfWidth){
      /* Upward part */
      *y = *x * fGain - fHalfWidth;
    }else if(*x < fPrevY / fGain - fHalfWidth){
      /* Downward part */
      *y = *x * fGain + fHalfWidth;
    }else{
      /* Horizontal part */
      *y = fPrevY;
    }
  }

  fPrevX = *x;
  fPrevY = *y;
}
