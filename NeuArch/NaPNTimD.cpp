/* NaPNTimD.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <string.h>

#include "NaPNTimD.h"
#include "NaPNTime.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNTimeDepend::NaPNTimeDepend (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  time(this, "time")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Get timer object (linked with 'time' connector)
NaTimer&
NaPNTimeDepend::timer () const
{
    NaPNTimer   *pPNTimer = (NaPNTimer*)time.host();
    return *pPNTimer;
}


//---------------------------------------------------------------------------

/////////////////////
// Timer functions //
/////////////////////

//---------------------------------------------------------------------------
// Set new sampling rate
void
NaPNTimeDepend::SetSamplingRate (NaReal sr)
{
    timer().SetSamplingRate(sr);
}


//---------------------------------------------------------------------------
// Get current sampling rate
NaReal
NaPNTimeDepend::GetSamplingRate () const
{
    return timer().GetSamplingRate();
}


//---------------------------------------------------------------------------
// Reset time/index counter to 0
void
NaPNTimeDepend::ResetTime ()
{
    timer().ResetTime();
}


//---------------------------------------------------------------------------
// Go to next time/index
void
NaPNTimeDepend::GoNextTime ()
{
    timer().GoNextTime();
}


//---------------------------------------------------------------------------
// Return current time
NaReal
NaPNTimeDepend::CurrentTime () const
{
    return fCurTime;
}


//---------------------------------------------------------------------------
// Return current index
int
NaPNTimeDepend::CurrentIndex () const
{
    return (int)(CurrentTime() / GetSamplingRate());
}



//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNTimeDepend::verify ()
{
    if(strcmp(time.adjoint()->name(), "time")){
        NaPrintLog("Time dependent PN object '%s' must be linked "
                   "with NaPNTimer.\n", name());
        return false;
    }
    return 1 == time.data().dim();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTimeDepend::action ()
{
  fCurTime = time.data()(0);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
