/* NaPNBu21.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNBu21.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNBus2i1o::NaPNBus2i1o (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in1(this, "in1"),
  in2(this, "in2"),
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
NaPNBus2i1o::main_input_cn ()
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
NaPNBus2i1o::relate_connectors ()
{
    out.data().new_dim(in1.data().dim() + in2.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNBus2i1o::verify ()
{
    return out.data().dim() == in1.data().dim() + in2.data().dim();
}

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNBus2i1o::action ()
{
    unsigned    i;
    for(i = 0; i < in1.data().dim(); ++i){
        out.data()[i] = in1.data()[i];
    }
    for(i = 0; i < in2.data().dim(); ++i){
        out.data()[in1.data().dim() + i] = in2.data()[i];
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
