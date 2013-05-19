//-*-C++-*-
/* NaInvPnd.h */
/* $Id$ */
#ifndef __NaInvPnd_h
#define __NaInvPnd_h

#include <NaUnit.h>

/**
 ***********************************************************************
 * Inverted pendulum on the cart linearized system implementation.
 ***********************************************************************/
class NaInvPend : public NaUnit
{
public:

  /** Construct inverted pendulum model. */
  NaInvPend (NaReal M_, NaReal m_, NaReal b_,
	     NaReal l_, NaReal I_);

  /** Copying inverted pendulum model. */
  NaInvPend (const NaInvPend& rInvPend);

  /** Destroy the object */
  virtual	~NaInvPend ();


  /** Reset operations, that must be done before new modelling session
      will start.  It's guaranteed that this reset will be called just
      after Timer().ResetTime(). */
  virtual void	Reset ();

  /** Compute output on the basis of internal parameters, stored state
      and external input: y=F(u,t,p).
      y[0] => x[0] - position of the cart [m];
      y[2] => q[0] - angle of the pendulum [rad] */
  virtual void	Function (NaReal* u, NaReal* y);

  /** Print current state of the inverted pendulum. */
  virtual void	PrintLog () const;


  /** Calculated linearized state space matrices. */
  static void	CalcLinMatrices (NaReal M, NaReal m, NaReal b,
				 NaReal l, NaReal I,
				 NaReal A[4][4], NaReal B[4]);

protected:

  /* Parameters */
  NaReal	M;	/**< mass of the cart [kg] */
  NaReal	m;	/**< mass of the pendulum [kg] */
  NaReal	b;	/**< friction of the cart [N/m/sec] */
  NaReal	l;	/**< length of pendulum center of mass [m] */
  NaReal	I;	/**< inertia of the pendulum [kg*m^2] */

  /* Calculated parameters */
  NaReal	A[4][4], B[4];

  /* Internal state */
  NaReal	x[2];	/**< x[0] - position of the cart [m];
			     x[1] - velocity of the cart [m/sec] */
  NaReal	q[2];	/**< q[0] - angle of the pendulum [rad];
			     q[1] - anglevelocity of the pendulum [rad/sec] */
};


#endif /* NaInvPnd.h */
