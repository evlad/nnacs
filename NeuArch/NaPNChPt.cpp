/* NaPNChPt.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNChPt.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNCheckPoint::NaPNCheckPoint (const char* szNodeName)
: NaPNFileOutput(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  out(this, "out")
{
    // Trace on information stand is turned off by default
    bInfo = false;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Switch for information stand trace of values on the node
void
NaPNCheckPoint::set_info_trace (bool flag)
{
    check_tunable();

    bInfo = flag;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNCheckPoint::relate_connectors ()
{
    NaPNFileOutput::relate_connectors();

    out.data().new_dim(in.data().dim());
}


//---------------------------------------------------------------------------
// 4. Allocate resources for internal usage
void
NaPNCheckPoint::allocate_resources ()
{
    NaPNFileOutput::allocate_resources();
    
    if(bInfo){
        // Add new trace to the information stand
        idTrace = net()->istand.new_trace(name());
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNCheckPoint::verify ()
{
    return NaPNFileOutput::verify()
        && out.data().dim() == in.data().dim()
        && in.data().dim() >= 1;                    // Due to [0] for trace
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNCheckPoint::action ()
{
    NaPNFileOutput::action();

    // Pass data through the node
    out.data() = in.data();

    // Make trace
    if(bInfo){
        net()->istand.put_trace_data(idTrace, in.data()[0]);
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
