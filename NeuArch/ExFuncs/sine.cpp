/* sine.cpp */
static char rcsid[] = "$Id: sine.cpp,v 1.1 2006-03-27 18:44:13 evlad Exp $";

#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define __NaSharedExternFunction
#include "sine.h"


//-----------------------------------------------------------------------
// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaSineFunc(szOptions, vInit);
}


//-----------------------------------------------------------------------
// Make empty (y=x) function
NaSineFunc::NaSineFunc ()
  : bTime(false), fK(1.0), fF(1.0), fP(0.0), fO(0.0)
{
  // Nothing to do more
}


//-----------------------------------------------------------------------
// Make function with given options and initial vector
NaSineFunc::NaSineFunc (char* szOptions, NaVector& vInit)
  : bTime(false), fK(1.0), fF(1.0), fP(0.0), fO(0.0)
{
  const char	*szTime = "time_arg";
  char		*szK, *szF, *szP, *szO, *szRest;
  NaReal	fTest;

  if(!strncmp(szOptions, szTime, strlen(szTime)))
    {
      szRest = szOptions + strlen(szTime);
      if('\0' == *szRest) {
	szK = szRest;
	bTime = true;
      }
      else if(isspace(*szRest)) {
	szK = szRest + 1;
	bTime = true;
      }
      else
	szK = szOptions;
    }
  else
    szK = szOptions;

  fTest = strtod(szK, &szF);
  if(szK != szF)
    fK = fTest;

  fTest = strtod(szF, &szP);
  if(szF != szP)
    fF = fTest;

  fTest = strtod(szP, &szO);
  if(szP != szO)
    fP = fTest;

  fTest = strtod(szO, &szRest);
  if(szO != szRest)
    fO = fTest;
}


//-----------------------------------------------------------------------
// Destructor
NaSineFunc::~NaSineFunc ()
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaSineFunc::Reset ()
{
}


//-----------------------------------------------------------------------
// ATTENTION!  It's guaranteed that this->Function() will be called
// only once for each time step.  Index of the time step and current
// time can be got from Timer().
//-----------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaSineFunc::Function (NaReal* x, NaReal* y)
{
  if(NULL == y || (!bTime && NULL == x))
    return;

  NaReal	fArg = bTime? Timer().CurrentTime(): *x;

  *y = fK * sin(fF * fArg + fP) + fO;
}
