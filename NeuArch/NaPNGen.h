//-*-C++-*-
/* NaPNGen.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNGenH
#define NaPNGenH

#include <NaPetri.h>
#include <NaUnit.h>


//---------------------------------------------------------------------------
// Applied Petri net node: generator unit.
// Has no inputs and the only output

//---------------------------------------------------------------------------
class NaPNGenerator : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNGenerator (const char* szNodeName = "generator");


    ////////////////
    // Connectors //
    ////////////////

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

    // Assign new generator function y=f()
    virtual void        set_generator_func (NaUnit* pFunc);


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
 
