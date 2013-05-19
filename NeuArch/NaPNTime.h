//-*-C++-*-
/* NaPNTime.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNTimeH
#define NaPNTimeH

#include <NaPetri.h>
#include <NaTimer.h>


//---------------------------------------------------------------------------
// Applied Petri net node: timer
// Has no input and the only output.  Reads given file value-by-value.

//---------------------------------------------------------------------------
class NaPNTimer : public NaPetriNode, public NaTimer
{
public:

    // Create generator node
    NaPNTimer (const char* szNodeName = "timer");


    ////////////////
    // Connectors //
    ////////////////

    // Non-obligatory stream of time stamps: 0, dt, 2*dt, ...
    NaPetriCnOutput     time;

    // Non-obligatory stream of sample stamps: 0, 1, 2, ...
    NaPetriCnOutput     samp;


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 4. Allocate resources for internal usage
    virtual void        allocate_resources ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 9. Finish data processing by the node (if activate returned true)
    virtual void        post_action ();

};


//---------------------------------------------------------------------------
#endif
