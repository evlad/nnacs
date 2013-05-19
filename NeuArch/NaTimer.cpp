/* NaTimer.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include "NaTimer.h"

/*************************************************************
 Main timer.
**************************************************************/
NaTimer TheTimer;


//---------------------------------------------------------------------------
NaTimer::NaTimer ()
{
    // Current index of time
    Itime = 0;

    // Time sampling rate
    Tdelta = 1.0;
}


//---------------------------------------------------------------------------
// Set new sampling rate
void
NaTimer::SetSamplingRate (NaReal sr)
{
    Tdelta = sr;
    ResetTime();
}

//---------------------------------------------------------------------------
// Get current sampling rate
NaReal
NaTimer::GetSamplingRate () const
{
    return Tdelta;
}

//---------------------------------------------------------------------------
// Reset time/index counter to 0
void
NaTimer::ResetTime ()
{
    Itime = 0;
}

//---------------------------------------------------------------------------
// Go to next time/index
void
NaTimer::GoNextTime ()
{
    ++Itime;
}

//---------------------------------------------------------------------------
// Return current time
NaReal
NaTimer::CurrentTime () const
{
    return Tdelta * Itime;
}

//---------------------------------------------------------------------------
// Return current index
int
NaTimer::CurrentIndex () const
{
    return Itime;
}

//---------------------------------------------------------------------------

