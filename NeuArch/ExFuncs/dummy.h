//-*-C++-*-
/* dummy.h */
//---------------------------------------------------------------------------
#ifndef __dummy_h
#define __dummy_h

#include <NaExFunc.h>


//---------------------------------------------------------------------------
// Class for external function for debuging.
class NaDummyFunc : public NaExternFunc
{
public:

  // Make empty (y=x) function
  NaDummyFunc ();

  // Make empty function with given options and initial vector
  // options: Anything
  // initial: Anything
  NaDummyFunc (char* szOptions, NaVector& vInit);

  // Destructor
  virtual ~NaDummyFunc ();

  // Reset operations, that must be done before new modelling
  // session will start.  It's guaranteed that this reset will be
  // called just after Timer().ResetTime().
  virtual void	Reset ();

  // ATTENTION!  It's guaranteed that this->Function() will be called
  // only once for each time step.  Index of the time step and current
  // time can be got from Timer().

  // Compute output on the basis of internal parameters,
  // stored state and external input: y=F(x,t,p)
  virtual void	Function (NaReal* x, NaReal* y);

};


//---------------------------------------------------------------------------
#endif // __deadzone_h
