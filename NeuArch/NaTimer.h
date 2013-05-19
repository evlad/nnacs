//-*-C++-*-
/* NaTimer.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaTimerH
#define NaTimerH
//---------------------------------------------------------------------------

#include <NaGenerl.h>

// Time samples counter
class NaTimer
{
public:

    NaTimer ();

    // Set new sampling rate
    virtual void    SetSamplingRate (NaReal sr);

    // Get current sampling rate
    virtual NaReal  GetSamplingRate () const;

    // Reset time/index counter to 0
    virtual void    ResetTime ();

    // Go to next time/index
    virtual void    GoNextTime ();

    // Return current time/index
    virtual NaReal  CurrentTime () const;
    virtual int     CurrentIndex () const;

private:

    int             Itime;          // current index of time
    NaReal          Tdelta;         // time sampling rate
};

//---------------------------------------------------------------------------

/*************************************************************
 Main (default) timer.
**************************************************************/
extern NaTimer      TheTimer;


//---------------------------------------------------------------------------
#endif
