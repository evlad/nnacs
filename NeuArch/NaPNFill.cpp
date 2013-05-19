/* NaPNFill.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNFill.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNFill::NaPNFill (const char* szNodeName)
  : NaPetriNode(szNodeName), nFill(0),
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
NaPNFill::main_output_cn ()
{
  return &out;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set number of data portions to fill zeros by default
void
NaPNFill::set_fill_number (unsigned n)
{
  check_tunable();

  nFill = n;

  // dim=0 means fill by zeros
  vFill.new_dim(0);
}


//---------------------------------------------------------------------------
// Set number of data portions to fill
void
NaPNFill::set_fill_number (unsigned n, NaVector& vect)
{
  check_tunable();

  nFill = n;
  vFill = vect;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNFill::relate_connectors ()
{
  out.data().new_dim(in.data().dim());
  sync.data().new_dim(1);

  if(0 == vFill.dim()){
    vFill.new_dim(in.data().dim());
    vFill.init_zero();
  }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNFill::verify ()
{
  if(vFill.dim() != in.data().dim()){
    NaPrintLog("%s.%s: fill vector has mismatched dimension: %u!=%u\n",
	       vFill.dim(), in.data().dim());
  }

  return out.data().dim() == in.data().dim()
    && vFill.dim() == in.data().dim()
    && 1 == sync.data().dim();
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNFill::activate ()
{
  // Don't fill more than nFill times
  if(activations() >= nFill)
    return NaPetriNode::activate();

  // Take care only about output connectors
  return !sync.is_waiting() && !out.is_waiting();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNFill::action ()
{
  if(activations() >= nFill)
    // Simple transfer
    out.data() = in.data();
  else // if(activations() < nFill)
    // Fill output
    out.data() = vFill;
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNFill::post_action ()
{
  // <= due to activations() already ++
  if(activations() > nFill){
    // Case of simple transfer
    
    // Commit input to get next data portion
    in.commit_data();
  }

  // Case of filling and simple transfer
  out.commit_data();

  if(activations() > nFill)
    sync.data()[0] = 1;
  else
    sync.data()[0] = -1;

  // Commit synchronization
  sync.commit_data();
}
