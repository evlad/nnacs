/* NaPNDerv.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPNDerv.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNDerivative::NaPNDerivative (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  x(this, "x"),
  dx(this, "dx")
{
  // Nothing
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNDerivative::relate_connectors ()
{
  dx.data().new_dim(x.data().dim());
}


//---------------------------------------------------------------------------
// 4. Allocate resources for internal usage
void
NaPNDerivative::allocate_resources ()
{
  vPrev.new_dim(x.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNDerivative::verify ()
{
  return x.data().dim() == dx.data().dim();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNDerivative::action ()
{
  if(activations() > 0){
    // Not for the first time
    dx.data() = x.data();	// x(k)
    vPrev.multiply(-1.0);	// -x(k-1)
    dx.data().add(vPrev);	// dx(k) = x(k) - x(k-1)
  }else{
    // For the first time
    dx.data().init_zero();
  }
  vPrev = x.data();
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
