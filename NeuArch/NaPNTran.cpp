/* NaPNTran.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPNTran.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNTransfer::NaPNTransfer (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  x(this, "x"),
  y(this, "y"),
  time(this, "time")
{
    pUnit = NULL;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new transfer function y=f(x)
void
NaPNTransfer::set_transfer_func (NaUnit* pFunc)
{
    check_tunable();

    if(NULL == pFunc)
        throw(na_null_pointer);

    pUnit = pFunc;
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNTransfer::main_input_cn ()
{
    return &x;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNTransfer::relate_connectors ()
{
    if(NULL != pUnit){
        x.data().new_dim(pUnit->InputDim());
        y.data().new_dim(pUnit->OutputDim());
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNTransfer::verify ()
{
    if(NULL == pUnit){
        NaPrintLog("Unit is not defined for transfer node '%s'.\n",
                   name());
        return false;
    }else if(time.links() > 0){
        return pUnit->InputDim() == x.data().dim()
            && pUnit->OutputDim() == y.data().dim()
            && 1 == time.data().dim();
    }else{
        return pUnit->InputDim() == x.data().dim()
            && pUnit->OutputDim() == y.data().dim();
    }
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNTransfer::initialize (bool& starter)
{
    pUnit->Reset();
    pUnit->SetupTimer(&net()->timer());
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTransfer::action ()
{
    pUnit->Function(&x.data()[0], &y.data()[0]);
    //NaPrintLog("node %s: time=%g\n", name(), net()->timer().CurrentTime());
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
