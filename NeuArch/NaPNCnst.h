//-*-C++-*-
/* NaPNCnst.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNCnstH
#define NaPNCnstH


#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: constant number generator
// Has the only output of adjustable dimension.

//---------------------------------------------------------------------------
class NaPNConstGen : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNConstGen (const char* szNodeName = "constgen");

    // Destroy node
    virtual ~NaPNConstGen ();

    ////////////////
    // Connectors //
    ////////////////

    // Output (mainstream)
    NaPetriCnOutput     out;


    ///////////////////
    // Node specific //
    ///////////////////

    // Set output dimension
    virtual void        set_out_dim (unsigned nDim);

    // Set generated value
    // !!Can be changed asynchronously!!
    virtual void        set_const_value (NaReal fConst);

    // Set generated value (vector)
    // !!Can be changed asynchronously!!
    virtual void        set_const_value (const NaReal* fConst);


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 1. Open input data (pure output nodes) and get their dimensions
    virtual void        open_input_data ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:

    // Output dimension
    unsigned            nOutDim;

    // Constant value to fill the output
    NaReal              *fConstVal;

};


//---------------------------------------------------------------------------
#endif
