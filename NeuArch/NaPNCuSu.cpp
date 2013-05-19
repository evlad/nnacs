/* NaPNCuSu.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

// Especially to get defined M_PI for MSVC
#define _USE_MATH_DEFINES

#include <math.h>

#include "NaPNCuSu.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNCuSum::NaPNCuSum (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  x(this, "x"),
  d(this, "d"),
  sum(this, "sum")
{
  // Nothing to do
  fATAD = 0.0;
}


//---------------------------------------------------------------------------
// Destroy the node
NaPNCuSum::~NaPNCuSum ()
{
  // Nothing to do
}


//---------------------------------------------------------------------------
// Setup parameters
void
NaPNCuSum::setup (NaReal sigma0, NaReal sigma1,
		  NaReal h_sol, NaReal k_const)
{
  fSigma[0] = sigma0;
  fSigma[1] = sigma1;
  fK = k_const;
  fTopVal = h_sol;
  fS = 0.0;
  fStartTime = 0.0;
  fPrevTA = -1.0;
}


//---------------------------------------------------------------------------
// Make smart detection considering known average time of alarm
// delay.  Actual detection is solved to be the second during given
// time range from the first one.
void
NaPNCuSum::set_smart_detection (NaReal atad)
{
  fATAD = atad;
  NaPrintLog("'%s' fATAD=%g\n", name(), fATAD);
}


//---------------------------------------------------------------------------
// Setup time of turning on the node in activity (time=0 by default)
void
NaPNCuSum::turn_on_at (NaReal start_time)
{
  fStartTime = start_time;
}


//---------------------------------------------------------------------------
// Gaussian distribution
NaReal
NaPNCuSum::gaussian_distrib (NaReal sigma, NaReal Xt)
{
  return exp(-Xt * Xt / 2 * sigma * sigma) / (sigma * sqrt(2 * M_PI));
}


//---------------------------------------------------------------------------
// Imaging point S(t) computation on the basis of S(t-1)
NaReal
NaPNCuSum::imaging_point (NaReal Sprev, NaReal Xt)
{
  NaReal	w0 = gaussian_distrib(fSigma[0], Xt);
  NaReal	w1 = gaussian_distrib(fSigma[1], Xt);
  NaReal	zt = log(w1 / w0);

  /*NaPrintLog("%g\t%g\t%g\t%g\t%g\n",
    Sprev, Sprev + zt - fK, w0, w1, zt);*/

  //return Sprev + zt - fK;
  NaReal	sq1 = fSigma[1] * fSigma[1];
  NaReal	sq0 = fSigma[0] * fSigma[0];

  //NaPrintLog("%g\t%g\n", zt, (-0.5*(1/sq1-1/sq0)*Xt*Xt - 0.5*log(sq1/sq0)));

  return Sprev + (-0.5*(1/sq1-1/sq0)*Xt*Xt - 0.5*log(sq1/sq0)) - fK;
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNCuSum::main_input_cn ()
{
  return &x;
}


//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNCuSum::main_output_cn ()
{
  return &d;
}

//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNCuSum::relate_connectors ()
{
  d.data().new_dim(1);
  sum.data().new_dim(1);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNCuSum::verify ()
{
  return 1 == x.data().dim()
    && 1 == d.data().dim()
    && 1 == sum.data().dim();
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNCuSum::initialize (bool& starter)
{
  fS = 0.0;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNCuSum::action ()
{
  if(fStartTime > net()->timer().CurrentTime())
    {
      // "turned-off" mode to wait for predefined moment
      sum.data()[0]  = 0.0;
      d.data()[0] = 0;
      return;
    }

  fS = imaging_point(fS, x.data()[0]);
  if(fS < 0.0)
    fS = 0.0;

  sum.data()[0]  = fS;
  /*
  NaPrintLog("'%s' -> check %g > %g (at %d) fATAD=%g\n",
	     name(), fS, fTopVal, net()->timer().CurrentIndex(), fATAD);
  */
  // compute detection signal here
  if(fS > fTopVal)
    {
      if(fATAD <= 0.0)
	{
	  /* unconditional alarm */
	  d.data()[0] = 1;
	  /*
	  NaPrintLog("'%s' -> unconditional alarm (at %d)\n",
		     name(), net()->timer().CurrentIndex());
	  */
	}
      else
	{
	  if(fPrevTA < 0)
	    {
	      /* first alarm: let's check it */
	      fPrevTA = net()->timer().CurrentTime();
	      d.data()[0] = 0;
	      /*
	      NaPrintLog("'%s' -> first alarm (at %d)\n",
			 name(), net()->timer().CurrentIndex());
	      */
	    }
	  else if(net()->timer().CurrentTime() - fPrevTA <= fATAD)
	    {
	      /* second alarm: fits average time of alarm delay */
	      fPrevTA = -1.0;
	      d.data()[0] = 1;
	      /*
	      NaPrintLog("'%s' -> second alarm (at %d) -> YES\n",
			 name(), net()->timer().CurrentIndex());
	      */
	    }
	  else if(net()->timer().CurrentTime() - fPrevTA > fATAD)
	    {
	      /* second alarm: out of average time of alarm delay */
	      fPrevTA = net()->timer().CurrentTime();
	      d.data()[0] = 0;
	      /*
	      NaPrintLog("'%s' -> second alarm (at %d) -> NO\n",
			 name(), net()->timer().CurrentIndex());
	      */
	    }
	}
      /* reset sum to initial one */
      fS = 0.0;

      /* show imaging point as threshold value */
      sum.data()[0]  = fTopVal;
    }
  else
    d.data()[0] = 0;
}
