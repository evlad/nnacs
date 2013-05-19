/* NaPNAct.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPNAct.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNActor::NaPNActor (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  sync(this, "sync")
{
    pActionProc = NULL;
    pUserData = NULL;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Setup some external procedure and its argument to call
// on data ready
void
NaPNActor::set_action (NaActionProc ap, void* ud)
{
    pActionProc = ap;
    pUserData = ud;
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNActor::main_input_cn ()
{
    return &sync;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNActor::action ()
{
    if(NULL != pActionProc){
        (*pActionProc)(pUserData);
    }else if(is_verbose()){
        NaPrintLog("%s.%s: NULL action proc\n", net(), name());
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
