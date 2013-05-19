/* NaInvPnd.cpp */
static char rcsid[] = "$Id$";

#include <stdio.h>

#include "NaMath.h"
#include "NaInvPnd.h"


/**
 ***********************************************************************
 * Construct inverted pendulum model.
 ***********************************************************************/
NaInvPend::NaInvPend (NaReal M_, NaReal m_, NaReal b_,
		      NaReal l_, NaReal I_)
  : NaUnit(1, 4, 0),
    M(M_), m(m_), b(b_), l(l_), I(I_)
{
  CalcLinMatrices(M, m, b, l, I, A, B);
}


/**
 ***********************************************************************
 * Copying inverted pendulum model.
 ***********************************************************************/
NaInvPend::NaInvPend (const NaInvPend& rInvPend)
  : NaUnit(rInvPend),
    M(rInvPend.M), m(rInvPend.m), b(rInvPend.b), l(rInvPend.l), I(rInvPend.I)
{
  CalcLinMatrices(M, m, b, l, I, A, B);
}


/**
 ***********************************************************************
 * Destroy the object.
 ***********************************************************************/
NaInvPend::~NaInvPend ()
{
}


/**
 ***********************************************************************
 * Reset operations, that must be done before new modelling session
 * will start.  It's guaranteed that this reset will be called just
 * after Timer().ResetTime().
 ***********************************************************************/
void
NaInvPend::Reset ()
{
  x[0] = x[1] = 0.0;
  q[0] = q[1] = 0.0;
}


/**
 ***********************************************************************
 * Compute output on the basis of internal parameters, stored state
 * and external input: y=F(u,t,p).
 *
 * y[0] => x[0] - position of the cart [m];
 * y[1] => q[0] - angle of the pendulum [rad];
 ***********************************************************************/
void
NaInvPend::Function (NaReal* u, NaReal* y)
{
  NaReal	yp[4];	/* previous state */

  yp[0] = x[0];
  yp[1] = x[1];
  yp[2] = q[0];
  yp[3] = q[1];

  for(int i = 0; i < 4; ++i)
    {
      y[i] = u[0] * B[i];
      for(int j = 0; j < 4; ++j)
	{
	  y[i] += A[i][j] * yp[j];
	}
    }

  x[0] = y[0];
  x[1] = y[1];
  q[0] = y[2];
  q[1] = y[3];
}


/**
 ***********************************************************************
 * Print current state of the inverted pendulum.
 ***********************************************************************/
void
NaInvPend::PrintLog () const
{
  NaPrintLog("NaInvPend(this=%p): t=%g x=%gm v=%gm/s q=%gdeg w=%gdeg/s\n",
	     this, Timer().CurrentTime(),
	     x[0], x[1], q[0]*M_PI/180, q[1]*M_PI/180);
}


/**
 ***********************************************************************
 * Calculated linearized state space matrices.
 ***********************************************************************/
void
NaInvPend::CalcLinMatrices (NaReal M, NaReal m, NaReal b,
			    NaReal l, NaReal I,
			    NaReal A[4][4], NaReal B[4])
{
  NaReal	g = 9.8;
  NaReal	p = I*(m+M)+M*m*l*l;
  A[0][0] = 0;
  A[0][1] = 1;
  A[0][2] = 0;
  A[0][3] = 0;

  A[1][0] = 0;
  A[1][1] = -(I+m*l*l)*b/p;
  A[1][2] = (m*m*g*l*l)/p;
  A[1][3] = 0;

  A[2][0] = 0;
  A[2][1] = 0;
  A[2][2] = 0;
  A[2][3] = 1;

  A[3][0] = 0;
  A[3][1] = -(m*l*b)/p;
  A[3][2] = m*g*l*(M+m)/p;
  A[3][3] = 0;

  NaPrintLog("A:");
  for(int i = 0; i < 4; ++i)
    NaPrintLog("\t%7.4f\t%7.4f\t%7.4f\t%7.4f\n",
	       A[i][0], A[i][1], A[i][2], A[i][3]);

  B[0] = 0;
  B[1] = (I+m*l*l)/p;
  B[2] = 0;
  B[3] = m*l/p;

  NaPrintLog("B:\t%7.4f\t%7.4f\t%7.4f\t%7.4f\n",
	     B[0], B[1], B[2], B[3]);
}
