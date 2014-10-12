/* NaPNBu31.cpp */
//---------------------------------------------------------------------------

#include "NaPNBu31.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNBus3i1o::NaPNBus3i1o (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in1(this, "in1"),
  in2(this, "in2"),
  in3(this, "in3"),
  out(this, "out")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNBus3i1o::main_input_cn ()
{
    return &in1;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNBus3i1o::relate_connectors ()
{
    out.data().new_dim(in1.data().dim() + in2.data().dim() + in3.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNBus3i1o::verify ()
{
    return out.data().dim() == in1.data().dim() + in2.data().dim() + in3.data().dim();
}

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNBus3i1o::action ()
{
    unsigned    i, n0;
    n0 = 0;
    for(i = 0; i < in1.data().dim(); ++i){
        out.data()[n0 + i] = in1.data()[i];
    }
    n0 += i;
    for(i = 0; i < in2.data().dim(); ++i){
        out.data()[n0 + i] = in2.data()[i];
    }
    n0 += i;
    for(i = 0; i < in3.data().dim(); ++i){
        out.data()[n0 + i] = in3.data()[i];
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
