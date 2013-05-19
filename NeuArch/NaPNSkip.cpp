/* NaPNSkip.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNSkip.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNSkip::NaPNSkip (const char* szNodeName)
  : NaPetriNode(szNodeName), nSkip(0),
    ////////////////
    // Connectors //
    ////////////////
    in(this, "in"),
    out(this, "out"),
    sync(this, "sync")
{
  // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNSkip::main_output_cn ()
{
  return &out;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set number of data portions to skip
void
NaPNSkip::set_skip_number (unsigned n)
{
  check_tunable();

  nSkip = n;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNSkip::relate_connectors ()
{
  out.data().new_dim(in.data().dim());
  sync.data().new_dim(1);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNSkip::verify ()
{
  return out.data().dim() == in.data().dim()
    && 1 == sync.data().dim();
}



//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNSkip::action ()
{
  out.data() = in.data();
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNSkip::post_action ()
{
  // Commit input anyway to get next data portion
  in.commit_data();

  if(activations() > nSkip)
    // Commit output only if given number of portions were skipped
    out.commit_data();

  if(activations() > nSkip)
    sync.data()[0] = 1;
  else
    sync.data()[0] = -1;

  // Commit synchronization
  sync.commit_data();
}
