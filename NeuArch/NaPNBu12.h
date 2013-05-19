//-*-C++-*-
/* NaPNBu12.h */
/* $Id: NaPNBu12.h 418 2010-01-09 19:01:41Z efox $ */
//---------------------------------------------------------------------------
#ifndef NaPNBu12H
#define NaPNBu12H

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: bus former 1->2.
// Has one input in and two outputs: out1 and out2.
// in.dim()=out1.dim()+out2.dim() in required proportion

//---------------------------------------------------------------------------
class NaPNBus1i2o : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNBus1i2o (const char* szNodeName = "bus1i2o");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      in;

    // Output #1
    NaPetriCnOutput     out1;

    // Output #2
    NaPetriCnOutput     out2;


    ///////////////////
    // Node specific //
    ///////////////////

    /// Set output connector proportion: n1/n2
    /// If n1=n2 then out1=out2[+1]
    /// If n1=0 then out2 has dim=n2
    /// If n2=0 then out1 has dim=n1
    void	set_out_dim_proportion (int n1, int n2);

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

protected:

    /// Output dimension proportion
    int			nOutDimProp1, nOutDimProp2;

};

//---------------------------------------------------------------------------
#endif
 
