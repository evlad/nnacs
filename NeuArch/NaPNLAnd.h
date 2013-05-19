//-*-C++-*-
/* NaPNLAnd.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNLAndH
#define NaPNLAndH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: logical "AND" operator
// Has two inputs: in1 and in2 and out output.
// +1(>0) - TRUE; -1(<=0) - FALSE

//---------------------------------------------------------------------------
class NaPNLogicalAND : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNLogicalAND (const char* szNodeName = "logicaland");


    ////////////////
    // Connectors //
    ////////////////

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

    // 3. Open output data (pure input nodes) and set their dimensions
    virtual void        open_output_data ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

};

//---------------------------------------------------------------------------
#endif
 
