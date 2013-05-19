/* NaPNLAnd.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNLAnd.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNLogicalAND::NaPNLogicalAND (const char* szNodeName)
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
NaPNLogicalAND::main_input_cn ()
{
    return &in1;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 3. Open output data (pure input nodes) and set their dimensions
void
NaPNLogicalAND::open_output_data ()
{
  out.data().new_dim(1);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNLogicalAND::verify ()
{
  return out.data().dim() == 1 &&
    in1.data().dim() == 1 && in2.data().dim() == 1;
}

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNLogicalAND::action ()
{
  if(in1.data()[0] > 0 && in2.data()[0] > 0)
    out.data()[0] = 1;	/* TRUE */
  else
    out.data()[0] = -1;	/* FALSE */
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
