/* NaPNSum.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNSum.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNSum::NaPNSum (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  main(this, "main"),
  aux(this, "aux"),
  sum(this, "sum")
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
NaPNSum::main_input_cn ()
{
    return &main;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign gain weights on input; ones by default
void
NaPNSum::set_gain (const NaVector& vMainK, const NaVector& vAuxK)
{
  vMainGain = vMainK;
  vAuxGain = vAuxK;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNSum::relate_connectors ()
{
    sum.data().new_dim(main.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNSum::verify ()
{
  /* check gain dimension */
  bool	bGoodGain =
    (aux.data().dim() == vAuxGain.dim() && 0 != vAuxGain.dim() &&
     main.data().dim() == vMainGain.dim() && 0 != vMainGain.dim()) ||
    (0 == vAuxGain.dim() && 0 == vMainGain.dim());

  if(!bGoodGain)
    NaPrintLog("Node '%s' has mismatched gain dimension\n", name());

  return bGoodGain
    && main.data().dim() == aux.data().dim()
    && main.data().dim() == sum.data().dim();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNSum::action ()
{
    unsigned    i;

    if(0 == vMainGain.dim() || 0 == vAuxGain.dim())
      {
	/* vMainGain[i]=1 and vAuxGain[i]=1 by default */
	for(i = 0; i < sum.data().dim(); ++i)
	  sum.data()[i] = main.data()[i] + aux.data()[i];
      }
    else
      {
	for(i = 0; i < sum.data().dim(); ++i)
	  sum.data()[i] =
	    vMainGain[i] * main.data()[i]
	    + vAuxGain[i] * aux.data()[i];
      }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
 
