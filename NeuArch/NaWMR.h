//-*-C++-*-
/* NaWMR.h */
/* $Id$ */
#ifndef __NaWMR_h
#define __NaWMR_h

#include <NaUnit.h>
#include <NaParams.h>


/**
 ***********************************************************************
 * Wheeled mobile robot: two wheels with independent motors and one
 * passive wheel.
 ***********************************************************************/
class NaWMR : public NaUnit
{
public:

  NaWMR ();

  enum {
    left_volts = 0,
    right_volts,
    __input_dim
  };
  enum {
    x_coord = 0,
    y_coord,
    angle,
    angle_vel,
    dir_vel,
    left_torque,
    right_torque,
    __state_dim
  };

  /** Current state (see y[] description) */
  NaReal	cs[7];

  struct Parameters {
    NaReal	l;	/**< distance between motor wheels, m */
    NaReal	b;	/**< distance between motor wheels axis and
			    center of mass, m */
    NaReal	r;	/**< radius of the wheel, m */
    NaReal	m;	/**< mass of the robot, kg */
    NaReal	iz;	/**< moment of inertia of the robot around
			   vertical axis, kg*m^2 */
    NaReal	R;	/**< electric resistance, Ohm */
    NaReal	L;	/**< induction, Henry */
    NaReal	Kw;	/**< coefficient 1 */
    NaReal	Km;	/**< coefficient 2 */
    NaReal	i;	/**< reductor ratio */
  }	par;

  /** Set parameters of the mobile robot */
  void	SetParameters (const NaParams& params);

  /** Reset operations, that must be done before new modelling session
      will start.  All state variables became zero.  They can be
      changed jsut after Reset().  It's guaranteed that this reset
      will be called just after Timer().ResetTime(). */
  virtual void	Reset ();

  /** Compute output on the basis of internal parameters, stored state
      and external input: y=F(x,t,p).

      x[0] - left DC motor voltage
      x[1] - right DC motor voltage

      y[0] - x coordinate
      y[1] - y coordinate
      y[2] - directional angle
      y[3] - angle velocity
      y[4] - direct velocity
      y[5] - left wheel torque
      y[6] - right wheel torque

      It's guaranteed that this->Function() will be called only once
      for each time step.  Index of the time step and current time can
      be got from Timer(). */
  virtual void	Function (NaReal* x, NaReal* y);

};


#endif /* NaWMR.h */
