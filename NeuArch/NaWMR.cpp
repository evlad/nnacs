/* NaWMR.cpp */
static char rcsid[] = "$Id$";

#include <math.h>

#include "NaWMR.h"


NaWMR::NaWMR ()
  : NaUnit(__input_dim/* inputs */, __state_dim/* outputs */)
{
}


/** Set parameters of the mobile robot */
void
NaWMR::SetParameters (const NaParams& params)
{
  par.l = atof(params("base_length"));	/**< distance between motor wheels */
  par.b = atof(params("mass_offset"));	/**< distance between wheels axis and
					   center of mass */
  par.r = atof(params("wheel_radius"));	/**< radius of the wheel */
  par.m = atof(params("total_mass"));	/**< mass of the robot */
  par.iz = atof(params("inertia_z"));	/**< moment of inertia of the robot
					   around vertical axis */
  par.R = atof(params("motor_resist"));	/**< electic resistance */
  par.L = atof(params("motor_induct"));	/**< induction */
  par.Kw = atof(params("motor_Kw"));	/**< coefficient 1 */
  par.Km = atof(params("motor_Km"));	/**< coefficient 2 */
  par.i = atof(params("reductor_ratio"));/**< reductor ratio */
}


/**
 ***********************************************************************
 * Reset operations, that must be done before new modelling session
 * will start.  It's guaranteed that this reset will be called just
 * after Timer().ResetTime().
 ***********************************************************************/
void
NaWMR::Reset ()
{
  for(int i = 0; i < __state_dim; ++i)
    cs[i] = 0.0;
}


/**
 ***********************************************************************
 * Compute output on the basis of internal parameters, stored state
 * and external input: y=F(x,t,p).  It's guaranteed that
 * this->Function() will be called only once for each time step.
 * Index of the time step and current time can be got from Timer().
 ***********************************************************************/
void
NaWMR::Function (NaReal* x, NaReal* y)
{
  if(NULL == x || NULL == y)
    return;

#if 1
#define NEXT(id, next)	y[id] = cs[id] + dt * (next)

  NaReal	dt = Timer().GetSamplingRate();

  NEXT(x_coord,
       cs[dir_vel] * cos(cs[angle]));
  NEXT(y_coord,
       cs[dir_vel] * sin(cs[angle]));
  NEXT(angle,
       cs[angle_vel]);
  NEXT(angle_vel,
       par.b * par.m * cs[angle_vel] * cs[dir_vel]/par.iz
       + par.l * (cs[right_torque] - cs[left_torque])/(2 * par.r * par.iz));
  NEXT(dir_vel,
       - par.b * cs[angle_vel] * cs[angle_vel]
       + (cs[right_torque] + cs[left_torque])/(par.m * par.r));

  NaReal	a = par.Km * par.Kw * par.i / par.l/* or par.r ??? some m */;
  NaReal	b = cs[angle_vel] * par.l / 2;
  NEXT(left_torque,
       (- par.R * cs[left_torque]
	- a * (cs[dir_vel] - b)
	+ par.Km * x[left_volts]) / par.L);
  NEXT(right_torque,
       (- par.R * cs[right_torque]
	- a * (cs[dir_vel] + b)
	+ par.Km * x[right_volts]) / par.L);

#undef NEXT

#else

  y[x_coord] = cs[x_coord] + dt * (cs[dir_vel] * cos(cs[angle]));
  y[y_coord] = cs[y_coord] + dt * (cs[dir_vel] * sin(cs[angle]));
  y[angle] = cs[angle] + dt * cs[angle_vel];
  y[angle_vel] = cs[angle_vel]
    + dt * (par.b * par.m * cs[angle_vel] * cs[dir_vel]/par.iz
	    + (cs[right_torque] - cs[left_torque])/(2 * par.r * par.iz));
  y[dir_vel] = cs[dir_vel]
    + dt * (-par.b * cs[angle_vel] * cs[angle_vel]
	    + (cs[right_torque] + cs[left_torque])/(par.m * par.r));

  NaReal	a = par.Km * par.Kw * par.i;
  NaReal	b = cs[angle_vel] * par.l / 2;
  y[left_torque] =
    cs[left_torque] + dt * (- par.R * cs[left_torque]
			    - a * (cs[dir_vel] - b)
			    + par.Km * x[left_volts]) / par.L;
  y[right_torque] =
    cs[right_torque] + dt * (- par.R * cs[right_torque]
			     - a * (cs[dir_vel] + b)
			     + par.Km * x[right_volts]) / par.L;
#endif

  for(int i = 0; i < __state_dim; ++i)
    cs[i] = y[i];
}
