//-*-C++-*-
/* NaPNTrig.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNTrigH
#define NaPNTrigH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: trigger
// Transfer input to output.  If input is ready but turn < 0 then skip
// input without changing output state.  If turn > 0 then deliver input to
// output.


//---------------------------------------------------------------------------
class NaPNTrigger : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNTrigger (const char* szNodeName = "trigger");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      in;

    // Synchronization input: <0 means "skip input", >0 means "in->out"
    NaPetriCnInput      turn;

    // Output (mainstream)
    NaPetriCnOutput     out;


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 9. Finish data processing by the node (if activate returned true)
    virtual void        post_action ();

};

//---------------------------------------------------------------------------
#endif
 
