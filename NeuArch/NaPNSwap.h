//-*-C++-*-
/* NaPNSwap.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNSwapH
#define NaPNSwapH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: two input to two output dynamic swapper.
// in->out and in2->out2 when turn is positive
// in->out2 and in2->out when turn is negative

//---------------------------------------------------------------------------
class NaPNSwapper : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNSwapper (const char* szNodeName = "swapper");


    ////////////////
    // Connectors //
    ////////////////

    // Control input
    NaPetriCnInput      turn;

    // Input (mainstream)
    NaPetriCnInput      in;

    // Another input
    NaPetriCnInput      in2;

    // Output (mainstream)
    NaPetriCnOutput     out;

    // Another output
    NaPetriCnOutput     out2;


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();

    // Return mainstream output connector (the only output or NULL)
    virtual NaPetriConnector*   main_output_cn ();


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
 
