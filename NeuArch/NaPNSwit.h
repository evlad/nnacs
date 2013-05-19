//-*-C++-*-
/* NaPNSwit.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNSwitH
#define NaPNSwitH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: switcher 2->1
// Has two data inputs: in1 and in2 and one control one.
// Data output is set to in1 if control >0 and to in2 if control <0.

//---------------------------------------------------------------------------
class NaPNSwitcher : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNSwitcher (const char* szNodeName = "switcher");


    ////////////////
    // Connectors //
    ////////////////

    // Control input
    NaPetriCnInput      turn;

    // Input #1 (mainstream)
    NaPetriCnInput      in1;

    // Input #2
    NaPetriCnInput      in2;

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

    // 7. Do one step of node activity and return true if succeeded
    virtual bool        activate ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 9. Finish data processing by the node (if activate returned true)
    virtual void        post_action ();

};

//---------------------------------------------------------------------------
#endif
 
