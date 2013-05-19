/* NaPNGen.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPNGen.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNGenerator::NaPNGenerator (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  time(this, "time"),
  y(this, "y")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new generator function y=f()
void
NaPNGenerator::set_generator_func (NaUnit* pFunc)
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
NaPNGenerator::main_input_cn ()
{
    return NULL;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNGenerator::relate_connectors ()
{
    if(NULL != pUnit){
        y.data().new_dim(pUnit->OutputDim());
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNGenerator::verify ()
{
    if(NULL == pUnit){
        NaPrintLog("Unit is not defined for generator node '%s'.\n",
                   name());
        return false;
    }else if(time.links() > 0){
        return pUnit->OutputDim() == y.data().dim()
            && 1 == time.data().dim();
    }else{
        return pUnit->OutputDim() == y.data().dim();
    }
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNGenerator::initialize (bool& starter)
{
    pUnit->Reset();
    pUnit->SetupTimer(&net()->timer());
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNGenerator::action ()
{
    NaReal  dummy;
    pUnit->Function(&dummy, &y.data()[0]);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
