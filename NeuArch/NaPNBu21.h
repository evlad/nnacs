//-*-C++-*-
/* NaPNBu21.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNBu21H
#define NaPNBu21H

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: bus former 2->1.
// Has two inputs: in1 and in2 and out output.  in1.dim()+in2.dim()=out.dim()

//---------------------------------------------------------------------------
class NaPNBus2i1o : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNBus2i1o (const char* szNodeName = "bus2i1o");


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

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

};

//---------------------------------------------------------------------------
#endif
 
