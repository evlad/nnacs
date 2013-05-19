//-*-C++-*-
/* sine.h */
/* $Id: sine.h,v 1.1 2006-03-27 18:44:13 evlad Exp $ */
#ifndef __sine_h
#define __sine_h

#include <NaExFunc.h>

//---------------------------------------------------------------------------
// Class for external function which calculates K*sin(F*x+P)+O for input
//            ^ y
//            |
// _     _    |_     _     _
//  \   / \   / \   / \   /
//   \_/   \_/|  \_/   \_/
// -----------+------------>
//            |            x
//
class NaSineFunc : public NaExternFunc
{
public:

  // Make empty (y=x) function
  NaSineFunc ();

  // Make function with given options and initial vector
  // options: [time] [K [F [P [O]]]]
  //          time - optional string which means that t used instead of x
  //          K - gain floating point number (1 by default)
  //          F - frequency floating point number (1 by default)
  //          P - phase shift floating point number (0 by default)
  //          O - resulting offset floating point number (0 by default)
  // initial: - not required
  NaSineFunc (char* szOptions, NaVector& vInit);

  // Destructor
  virtual ~NaSineFunc ();

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

  bool		bTime;	// use time instead of input then the function
			// becomes generator
  NaReal	fK;	// gain floating point number
  NaReal	fF;	// frequency floating point number
  NaReal	fP;	// phase shift floating point number
  NaReal	fO;	// resulting offset floating point number

};


#endif /* sine.h */
