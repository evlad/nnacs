//-*-C++-*-
/* NaPNTran.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNTranH
#define NaPNTranH

#include <NaPetri.h>
#include <NaUnit.h>


//---------------------------------------------------------------------------
// Applied Petri net node: transfer unit.
// Has one input and one output

//---------------------------------------------------------------------------
class NaPNTransfer : public NaPetriNode, public NaTimer
{
public:

    // Create node for Petri network
    NaPNTransfer (const char* szNodeName = "transfer");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      x;

    // Input time stream
    NaPetriCnInput      time;

    // Output (mainstream)
    NaPetriCnOutput     y;


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();


    ///////////////////
    // Node specific //
    ///////////////////

    // Assign new transfer function y=f(x)
    virtual void        set_transfer_func (NaUnit* pFunc);


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 6. Initialize node activity and setup starter flag if needed
    virtual void        initialize (bool& starter);

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:/* data */

    // Unit as a transfer function y=f(x)
    NaUnit              *pUnit;
    
};


//---------------------------------------------------------------------------
#endif
 
