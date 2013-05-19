/* NaPNSwap.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNSwap.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNSwapper::NaPNSwapper (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  in2(this, "in2"),
  out(this, "out"),
  out2(this, "out2"),
  turn(this, "turn")
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
NaPNSwapper::main_input_cn ()
{
    return &in;
}


//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNSwapper::main_output_cn ()
{
    return &out;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNSwapper::relate_connectors ()
{
    out.data().new_dim(in.data().dim());
    out2.data().new_dim(in2.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNSwapper::verify ()
{
    return out.data().dim() == in.data().dim()
        && in2.data().dim() == in.data().dim()
        && out2.data().dim() == in2.data().dim()
        && turn.data().dim() == 1;
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNSwapper::activate ()
{
    return !turn.is_waiting();
    // && !in.is_waiting() && !out.is_waiting())
    //    || (!turn.is_waiting() && !in2.is_waiting() && !out2.is_waiting())
    //    || (!turn.is_waiting() && !in.is_waiting() && !out2.is_waiting())
    //    || (!turn.is_waiting() && !in2.is_waiting() && !out.is_waiting());
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNSwapper::action ()
{
    if(turn.data()[0] > 0){
        // Copy input to output
        if(!in.is_waiting() && !out.is_waiting()){
            out.data() = in.data();
        }
        if(!in2.is_waiting() && !out2.is_waiting()){
            out2.data() = in2.data();
        }
    }else{
        // Copy crossed input to output
        if(!in2.is_waiting() && !out.is_waiting()){
            out.data() = in2.data();
        }
        if(!in.is_waiting() && !out2.is_waiting()){
            out2.data() = in.data();
        }
    }
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNSwapper::post_action ()
{
    if(turn.data()[0] > 0){
        // Copy input to output
        if(!in.is_waiting() && !out.is_waiting()){
            out.commit_data();
            in.commit_data();
        }
        if(!in2.is_waiting() && !out2.is_waiting()){
            out2.commit_data();
            in2.commit_data();
        }
    }else{
        // Copy crossed input to output
        if(!in2.is_waiting() && !out.is_waiting()){
            out.commit_data();
            in2.commit_data();
        }
        if(!in.is_waiting() && !out2.is_waiting()){
            out2.commit_data();
            in.commit_data();
        }
    }
    turn.commit_data();
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
