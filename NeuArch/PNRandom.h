//-*-C++-*-
/* PNRandom.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef PNRandomH
#define PNRandomH

#include <NaPetri.h>

//---------------------------------------------------------------------------
// Random number generator

class NaPNRandomGen : public NaPetriNode
{
public:

    // Construct the random number generator
    NaPNRandomGen (const char* szNodeName = "randgen");


    /////////////////////
    // Preset specific //
    /////////////////////

    // Setup dimension of the output connector 'rand'
    void    set_rand_dim (unsigned nDim);

    
    ////////////////
    // Connectors //
    ////////////////

    // Random number
    NaPetriCnOutput     rand;


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 8. True action of the node (if activate returned true)
    virtual void        action ();
    
};


//---------------------------------------------------------------------------
#endif
