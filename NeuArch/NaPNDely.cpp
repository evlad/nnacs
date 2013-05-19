/* NaPNDely.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNDely.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNDelay::NaPNDelay (const char* szNodeName)
: NaPetriNode(szNodeName), piOutMap(NULL), nOutDim(0),
  nMaxLag(0), nMinLag(0), bSleepValue(false), fSleepValue(0.0),
  bSleepValue1st(false),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  dout(this, "dout"),
  sync(this, "sync")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Print actual status
void
NaPNDelay::print_status ()
{
  int	i, j;

  /* Print delay information anyway */
  NaPrintLog("NaPNDelay (%p, %s.%s):", this, net()->name(), name());
  NaPrintLog("  nMaxLag=%u,  nMinLag=%u\n", nMaxLag, nMinLag);
  NaPrintLog("  nActiveSleep=%u\n", nActiveSleep);
  NaPrintLog("  bAwaken=%s\n", bAwaken?"true":"false");
  if(!bSleepValue)
      NaPrintLog("  bSleepValue=false\n");
  else if(bSleepValue1st)
      NaPrintLog("  bSleepValue=true,  fSleepValue=(First)\n");
  else
      NaPrintLog("  bSleepValue=true,  fSleepValue=%g\n", fSleepValue);
  NaPrintLog("  delays map:");
  for(i = 0; i < nOutDim; ++i)
    NaPrintLog(" %u", piOutMap[i]);
  NaPrintLog("\n");

  if(!tunable()){
    /* Additional runtime info */
    NaPrintLog("  delayed values in buffer:\n");
    for(j = 0; j < vBuffer.dim() / in.data().dim(); ++j){
      for(i = 0; i < nOutDim; ++i)
	if(piOutMap[i] == j)
	  break;
      if(i < nOutDim)
	NaPrintLog("  [%d] ->", piOutMap[i]);
      else
	NaPrintLog("      ->");

      for(i = 0; i < in.data().dim(); ++i)
	NaPrintLog(" %g", vBuffer[j * in.data().dim() + i]);
      NaPrintLog("\n");
    }
  }
}


//---------------------------------------------------------------------------
// Get maximum lag
unsigned
NaPNDelay::get_max_delay () const
{
  return nMaxLag;
}


//---------------------------------------------------------------------------
// Get minimum lag
unsigned
NaPNDelay::get_min_delay () const
{
  return nMinLag;
}


//---------------------------------------------------------------------------
// Add delay
void
NaPNDelay::add_delay (unsigned iShift)
{
    check_tunable();

    if(NULL == piOutMap || 0 == nOutDim)
      throw(na_null_pointer);

    unsigned	i;
    for(i = 0; i < nOutDim; ++i)
      piOutMap[i] += iShift;
    nMaxLag += iShift;
    nMinLag += iShift;
}


//---------------------------------------------------------------------------
// Set delay (0 - no delay, 1 - one delayed step, etc)
void
NaPNDelay::set_delay (unsigned nSamples)
{
    check_tunable();

    nMinLag = 0;
    nMaxLag = nSamples;
    nOutDim = 1 + nSamples;
    piOutMap = new unsigned[nOutDim];
    unsigned	i;
    for(i = 0; i < nOutDim; ++i)
      piOutMap[i] = i;
}


//---------------------------------------------------------------------------
// Set delay encoded in piMap[nDim]
void
NaPNDelay::set_delay (unsigned nDim, unsigned* piMap)
{
  check_tunable();

  nOutDim = nDim;
  delete piOutMap;

  if(0 == nOutDim)
    throw(na_bad_value);
  else if(NULL == piMap)
    throw(na_null_pointer);

  piOutMap = new unsigned[nOutDim];
  nMaxLag = 0;
  nMinLag = 0;
  unsigned	i;
  for(i = 0; i < nOutDim; ++i)
    {
      piOutMap[i] = piMap[i];
      if(piOutMap[i] > nMaxLag)
	nMaxLag = piOutMap[i];

      if(0 == i)
	nMinLag = piOutMap[i];
      if(piOutMap[i] < nMinLag)
	nMinLag = piOutMap[i];
    }
}


//---------------------------------------------------------------------------
// Set value to substitute output in sleep time
void
NaPNDelay::set_sleep_value (NaReal fValue)
{
  check_tunable();

  bSleepValue = true; // don't sleep for this case
  fSleepValue = fValue;
}


//---------------------------------------------------------------------------
// Set the 1st value to substitute output in sleep time
void
NaPNDelay::set_sleep_value_1st ()
{
  check_tunable();

  bSleepValue = true;    // don't sleep for this case
  bSleepValue1st = true; // don't sleep for this case
}


//---------------------------------------------------------------------------
// Return true is passive sleep time is over and false otherwise
bool
NaPNDelay::awake ()
{
    return nActiveSleep != 0 || activations() >= nMaxLag;
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNDelay::main_output_cn ()
{
    return &dout;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNDelay::relate_connectors ()
{
    // Get nMaxLag times input vector
    dout.data().new_dim(in.data().dim() * nOutDim);

    // Just for synchronization...
    sync.data().new_dim(1);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNDelay::verify ()
{
    return ((sync.links() > 0)? 1 == sync.data().dim(): true)
        && dout.data().dim() == in.data().dim() * nOutDim;
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNDelay::initialize (bool& starter)
{
  if(bSleepValue)
    nActiveSleep = nMaxLag + 1;
  else
    nActiveSleep = 0;
  vBuffer.new_dim((1 + nMaxLag) * in.data().dim());
  vBuffer.init_value(fSleepValue);
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNDelay::activate ()
{
  // For action() and post_action() only
  bAwaken = awake();

  if(bAwaken && is_verbose())
    {
      unsigned	i;
      NaPrintLog("node '%s' is awaken: activations=%d, delay=",
		 name(), activations());
      for(i = 0; i < nOutDim; ++i)
	if(i + 1 == nOutDim)
	  NaPrintLog("%d\n", piOutMap[i]);
	else
	  NaPrintLog("%d,", piOutMap[i]);
    }

  // Does not depend on bAwaken state and output state, so 
  return !in.is_waiting() &&
    (!bAwaken || (bAwaken && !dout.is_waiting()));
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNDelay::action ()
{
    // Portion size
    unsigned	nSize = in.data().dim();

    // Make free space in buffer area
    vBuffer.shift(nSize);

    // Copy input data to buffer
    unsigned    i, j;
    for(i = 0; i < nSize; ++i)
      {
        vBuffer[i] = in.data()[i];
      }

    // If 1st input should be used for all delays
    if(bSleepValue && bSleepValue1st && 0 == activations()) {
	for(j = 1; j <= nMaxLag; ++j)
	    for(i = 0; i < nSize; ++i)
		vBuffer[i + nSize * j] = vBuffer[i];
    }

    // Copy selected portions of stored data to output
    for(j = 0; j < nOutDim; ++j)
      for(i = 0; i < nSize; ++i)
	{
	  dout.data()[j * nSize + i] = vBuffer[piOutMap[j] * nSize + i];
	}

    // Sleep/awaken related nodes
    if(bAwaken){
        sync.data()[0] = 1.0;   // Let linked nodes to deliver data
    }else{
        sync.data()[0] = -1.0;  // Force to be passive linked nodes
    }
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNDelay::post_action ()
{
    if(bAwaken){
        // Output data are ready, so deliver them
        dout.commit_data();
    }

    // Welcome for new data
    in.commit_data();

    // Manage linked nodes
    sync.commit_data();

    // Decrement number of active sleeping ticks
    if(nActiveSleep != 0){
      --nActiveSleep;
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
