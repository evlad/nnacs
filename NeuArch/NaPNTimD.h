//-*-C++-*-
/* NaPNTimD.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNTimDH
#define NaPNTimDH

#include <NaPetri.h>
#include <NaTimer.h>


//---------------------------------------------------------------------------
// Applied Petri net node: time dependence
// For time dependent objects implement relation with timer.

//---------------------------------------------------------------------------
class NaPNTimeDepend : public NaPetriNode, public NaTimer
{
public:

    // Create node for Petri network
    NaPNTimeDepend (const char* szNodeName = "timedep");


    ////////////////
    // Connectors //
    ////////////////

    // Input timer
    NaPetriCnInput      time;


    ///////////////////
    // Node specific //
    ///////////////////

    // Get timer object (linked with 'time' connector)
    virtual NaTimer&    timer () const;


    /////////////////////
    // Timer functions //
    /////////////////////

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


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:

  // Current time
  NaReal		fCurTime;

};


//---------------------------------------------------------------------------
#endif
 
