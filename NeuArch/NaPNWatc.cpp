/* NaPNWatc.cpp */
static char rcsid[] = "$Id$";

#include "NaPNWatc.h"

//---------------------------------------------------------------------------
// Create node for Petri network
NaPNWatcher::NaPNWatcher (const char* szNodeName)
  : NaPetriNode(szNodeName),
    ////////////////
    // Connectors //
    ////////////////
    events(this, "events"),
    // Other data
    m_pExtFunc(NULL), m_pUserData(NULL)
{
  // Nothing to do
}


///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNWatcher::main_input_cn ()
{
  return &events;
}


///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Attach external watcher function
void
NaPNWatcher::attach_function (ExternalFunc pExtFunc, void* pUserData)
{
  m_pExtFunc = pExtFunc;
  m_pUserData = pUserData;
}


//---------------------------------------------------------------------------
// Built-in watcher function.  Called before attached function.
// Returns \b true to allow call external functions to process
// data events and \b false otherwise.  Returns \b true by default.
bool
NaPNWatcher::watcher (const NaVector& rEvents)
{
  return true;
}



///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNWatcher::action ()
{
  if(watcher(events.data()))
    if(NULL != m_pExtFunc)
      if(!(*m_pExtFunc)(m_pUserData, events.data(), net()->timer()))
	halt();
}
