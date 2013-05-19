//-*-C++-*-
/* luft.h */
/* $Id: luft.h,v 1.1 2003-07-24 09:30:18 vlad Exp $ */
//---------------------------------------------------------------------------
#ifndef __luft_h
#define __luft_h

#include <NaExFunc.h>


//---------------------------------------------------------------------------
// Class for external function which implements luft area
//            ^
//            |
//           /+--<---/ upward
//          /-+-<---/
// downward/  |    / Gain
// -------/---+---/------->
//       /    |  /
//      /--->-+-/
//     /--->--+/
//    /       /<-->
//            | HalfWidth
//
class NaLuftAreaFunc : public NaExternFunc
{
public:

  // Make empty (y=x) function
  NaLuftAreaFunc ();

  // Make function with given options and initial vector
  // options: HalfWidth Gain
  //          HalfWidth - floating point number
  //          Gain      - floating point number
  // initial: X0 Y0  - floating point number
  NaLuftAreaFunc (char* szOptions, NaVector& vInit);

  // Destructor
  virtual ~NaLuftAreaFunc ();

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

  NaReal	fHalfWidth;	// half width of luft area
  NaReal	fGain;		// gain coefficient y=k*x

  NaReal	fPrevX;		// x at previous time sample (0 by default)
  NaReal	fPrevY;		// y at previous time sample (0 by default)
  NaReal	fX0, fY0;	// initial x and y values
};


//---------------------------------------------------------------------------
#endif // __luft_h
