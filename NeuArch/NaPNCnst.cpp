/* NaPNCnst.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNCnst.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNConstGen::NaPNConstGen (const char* szNodeName)
: NaPetriNode(szNodeName),
  nOutDim(0),
  fConstVal(NULL),
  ////////////////
  // Connectors //
  ////////////////
  out(this, "out")
{
    set_out_dim(1);
    set_const_value(0.0);
}


//---------------------------------------------------------------------------
// Destroy node
NaPNConstGen::~NaPNConstGen ()
{
    delete[] fConstVal;
}

//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set output dimension
void
NaPNConstGen::set_out_dim (unsigned nDim)
{
    check_tunable();

    if(0 == nDim)
        throw(na_bad_value);

    nOutDim = nDim;

    delete[] fConstVal;
    fConstVal = new NaReal[nOutDim];
}


//---------------------------------------------------------------------------
// Set generated value
// !!Can be changed asynchronously!!
void
NaPNConstGen::set_const_value (NaReal fConst)
{
    //!!!check_tunable();

    unsigned    i;
    for(i = 0; i < nOutDim; ++i){
        fConstVal[i] = fConst;
    }
}


//---------------------------------------------------------------------------
// Set generated value (vector)
// !!Can be changed asynchronously!!
void
NaPNConstGen::set_const_value (const NaReal* fConst)
{
    //!!!check_tunable();

    if(NULL == fConst)
        throw(na_null_pointer);

    unsigned    i;
    for(i = 0; i < nOutDim; ++i){
        fConstVal[i] = fConst[i];
    }
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 1. Open input data (pure output nodes) and get their dimensions
void
NaPNConstGen::open_input_data ()
{
    out.data().new_dim(nOutDim);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNConstGen::verify ()
{
    return out.data().dim() == nOutDim;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNConstGen::action ()
{
    out.data() = fConstVal;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
