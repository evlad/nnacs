//-*-C++-*-
/* saturat.h */
/* $Id: saturat.h,v 1.1 2003-07-24 06:26:35 vlad Exp $ */
//---------------------------------------------------------------------------
#ifndef __saturat_h
#define __saturat_h

#include <NaExFunc.h>


//---------------------------------------------------------------------------
// Class for external function which implements saturation with linear part
//
// Symmetric Limit:
//            ^
//            |    --------
//            |  /       ^ Limit
//            |/ Gain    |
// -----------+------------>
//           /|
//         /  |
// -------    |
//
//
// Assymmetric Limit:
//            ^
//            |    --------
//            |  /       ^ Limit
//            |/ Gain    |
// -----------+------------>
//  |        /|
// ---------  |
//  ^ Limit2
//
class NaSaturationFunc : public NaExternFunc
{
public:

  // Make empty (y=x) function
  NaSaturationFunc ();

  // Make function with given options and initial vector
  // options: Limit Gain [Limit2]
  //          Limit     - floating point number (0 means infinity)
  //          Gain      - floating point number
  //          Limit2    - floating point number (absence means symmetic)
  // initial: -
  NaSaturationFunc (char* szOptions, NaVector& vInit);

  // Destructor
  virtual ~NaSaturationFunc ();

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

protected:

  NaReal	fLimit;		// saturation threshold (0 means infinity)
  NaReal	fGain;		// gain coefficient y=k*x
  NaReal	fLimit2;	// negative saturation threshold (-fLimit)
};


//---------------------------------------------------------------------------
#endif // __saturat_h
