/* NaPNQIn.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNQIn.h"


//---------------------------------------------------------------------------
// Create generator node
NaPNQueueInput::NaPNQueueInput (const char* szNodeName)
  : NaPetriNode(szNodeName), nDataSize(1),
    ////////////////
    // Connectors //
    ////////////////
    out(this, "out")
{
  // Nothing
}


//---------------------------------------------------------------------------
// Destroy the node
NaPNQueueInput::~NaPNQueueInput ()
{
  // Nothing
}

//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set dimension of input data
void
NaPNQueueInput::set_data_dim (unsigned n)
{
  check_tunable();

  if(n <= 0)
    throw(na_bad_value);

  nDataSize = n;
}


//---------------------------------------------------------------------------
// Put data to the queue
void
NaPNQueueInput::put_data (const NaReal* pPortion)
{
  if(NULL == pPortion)
    throw(na_null_pointer);

  unsigned	i;

  vQueue.new_dim(vQueue.dim() + nDataSize);

  for(i = 0; i < nDataSize; ++i){
    vQueue[vQueue.dim() + i - nDataSize] = pPortion[i];
  }
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 1. Open input data (pure output nodes) and get their dimensions
void
NaPNQueueInput::open_input_data ()
{
  out.data().new_dim(nDataSize);
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNQueueInput::initialize (bool& starter)
{
  // Make empty queue
  vQueue.new_dim(0);
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNQueueInput::activate ()
{
  // Check the queue is not empty
  return (vQueue.dim() > 0);
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNQueueInput::action ()
{
  unsigned	i;

  // Get data from the queue
  for(i = 0; i < nDataSize; ++i){
    out.data()[i] = vQueue(i);
  }

  // Remove data portion
  vQueue.shift(- (int)nDataSize);
  vQueue.new_dim(vQueue.dim() - nDataSize);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
