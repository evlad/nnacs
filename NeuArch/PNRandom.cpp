/* PNRandom.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdlib.h>

#include "PNRandom.h"


//---------------------------------------------------------------------------
NaPNRandomGen::NaPNRandomGen (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  rand(this, "rand")
{
    // nothing more
}


//---------------------------------------------------------------------------

/////////////////////
// Preset specific //
/////////////////////

//---------------------------------------------------------------------------
// Setup dimension of the output connector 'rand'
void
NaPNRandomGen::set_rand_dim (unsigned nDim)
{
    check_tunable();
    
    rand.data().new_dim(nDim);
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNRandomGen::action ()
{
    NaVector    &vect = rand.data();
    unsigned    i;

    for(i = 0; i < vect.dim(); ++i){
        // int[0..10000] -> float[0..1]
        vect[i] = 0.0001 * random(10001);
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
