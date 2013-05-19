/* NaPID.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include "NaPID.h"


//---------------------------------------------------------------------------
// Create default gain PID controller
NaPID::NaPID ()
: NaConfigPart("PIDController")
{
    Kp = 1.0;
    Ti = 0.0;
    Td = 0.0;
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaPID::Save (NaDataStream& ds)
{
    ds.PutF("Proportional gain", "%g", Kp);
    ds.PutF("Integration time", "%g", Ti);
    ds.PutF("Derivation time", "%g", Td);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaPID::Load (NaDataStream& ds)
{
    ds.GetF("%lg", &Kp);
    ds.GetF("%lg", &Ti);
    ds.GetF("%lg", &Td);
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void    NaPID::Reset ()
{
    Xsum = 0.0;
    Xprev = 0.0;
}


//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void    NaPID::Function (NaReal* x, NaReal* y)
{
    NaReal  &X = (*x), &Y = (*y);
    NaReal  dT = Timer().GetSamplingRate();
    NaReal  Integr = 0.0;

    Xsum += X;

    // Protect from devision by zero
    if(Ti != 0.0)
        Integr = dT * Xsum / Ti;

    if(Timer().CurrentIndex() == 0)
        // At Time=0 dx/dt = 0
        Y = Kp * (X + Integr);
    else
        // Later - normal derivation
        Y = Kp * (X + Integr + Td * (X - Xprev) / dT);

    Xprev = X;
}

//---------------------------------------------------------------------------

