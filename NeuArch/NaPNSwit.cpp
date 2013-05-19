/* NaPNSwit.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNSwit.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNSwitcher::NaPNSwitcher (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in1(this, "in1"),
  in2(this, "in2"),
  out(this, "out"),
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
NaPNSwitcher::main_input_cn ()
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
NaPNSwitcher::relate_connectors ()
{
    out.data().new_dim(in1.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNSwitcher::verify ()
{
    return out.data().dim() == in1.data().dim()
        && out.data().dim() == in2.data().dim()
        && turn.data().dim() == 1;
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNSwitcher::activate ()
{
  /*NaPrintLog("\tturn=%d\n\tin1=%d\n\tin2=%d\n\tout=%d\n",
	     turn.is_waiting(), in1.is_waiting(),
	     in2.is_waiting(), out.is_waiting());*/

  if(turn.is_waiting())
    return false;
  
  if(turn.data()[0] > 0)
    return !in1.is_waiting() && !out.is_waiting();
  /* else */
  return !in2.is_waiting() && !out.is_waiting();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNSwitcher::action ()
{
    if(turn.data()[0] > 0){
        // Copy input to output
        if(!in1.is_waiting() && !out.is_waiting()){
            out.data() = in1.data();
        }
    }else{
        // Copy crossed input to output
        if(!in2.is_waiting() && !out.is_waiting()){
            out.data() = in2.data();
        }
    }
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNSwitcher::post_action ()
{
    if(turn.data()[0] > 0){
        // Copy input to output
        if(!in1.is_waiting() && !out.is_waiting()){
            in1.commit_data();
            out.commit_data();
            turn.commit_data();
        }
        if(!in2.is_waiting()){
            in2.commit_data();
        }
    }else{
        // Copy crossed input to output
        if(!in2.is_waiting() && !out.is_waiting()){
            in2.commit_data();
            out.commit_data();
            turn.commit_data();
        }
        if(!in1.is_waiting()){
            in1.commit_data();
        }
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
