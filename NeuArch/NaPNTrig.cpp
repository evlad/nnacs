/* NaPNTrig.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNTrig.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNTrigger::NaPNTrigger (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  turn(this, "turn"),
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
NaPNTrigger::main_input_cn ()
{
    return &in;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNTrigger::relate_connectors ()
{
    out.data().new_dim(in.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNTrigger::verify ()
{
    return out.data().dim() == in.data().dim()
        && turn.data().dim() == 1;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTrigger::action ()
{
    if(turn.data()[0] > 0){
        // Copy input to output
        out.data() = in.data();
    }
    // else
    //   Skip input data
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNTrigger::post_action ()
{
    if(turn.data()[0] > 0){
        // Pass input to output
        out.commit_data();
    }
    // else
    //   Skip input data

    // Commit inputs anyway
    turn.commit_data();
    in.commit_data();
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
