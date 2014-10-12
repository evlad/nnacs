//-*-C++-*-
/* NaPNBu31.h */
//---------------------------------------------------------------------------
#ifndef NaPNBu31H
#define NaPNBu31H

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: bus former 2->1.
// Has three inputs: in1, in2, in3 and out output.
//   in1.dim()+in2.dim()+in3.dim()=out.dim()

//---------------------------------------------------------------------------
class NaPNBus3i1o : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNBus3i1o (const char* szNodeName = "bus3i1o");


    ////////////////
    // Connectors //
    ////////////////

    // Input #1 (mainstream)
    NaPetriCnInput      in1;

    // Input #2
    NaPetriCnInput      in2;

    // Input #3
    NaPetriCnInput      in3;

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
 
