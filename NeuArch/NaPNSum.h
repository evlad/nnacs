//-*-C++-*-
/* NaPNSum.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNSumH
#define NaPNSumH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: sum of two inputs
// Has two equal inputs and one output for resulting sum.

//---------------------------------------------------------------------------
class NaPNSum : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNSum (const char* szNodeName = "sum");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      main;

    // Another input
    NaPetriCnInput      aux;

    // Output (mainstream)
    NaPetriCnOutput     sum;


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();


    ///////////////////
    // Node specific //
    ///////////////////

    // Assign gain weights on input; ones by default
    virtual void        set_gain (const NaVector& vMainK,
				  const NaVector& vAuxK);


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:/* data */

    // Gain for main and auxiliary inputs
    NaVector		vMainGain, vAuxGain;

};


//---------------------------------------------------------------------------
#endif
 
