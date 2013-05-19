/* NaPNTime.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNTime.h"


//---------------------------------------------------------------------------
// Create generator node
NaPNTimer::NaPNTimer (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  time(this, "time"),
  samp(this, "samp")
{
    // nothing to do
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNTimer::relate_connectors ()
{
    time.data().new_dim(1);
    samp.data().new_dim(1);
}


//---------------------------------------------------------------------------
// 4. Allocate resources for internal usage
void
NaPNTimer::allocate_resources ()
{
    net()->set_timer(this);
    ResetTime();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTimer::action ()
{
    if(activations() != 0){
        GoNextTime();
    }
    time.data()[0] = CurrentTime();
    samp.data()[0] = CurrentIndex();
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNTimer::post_action ()
{
    NaPetriNode::post_action();
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
