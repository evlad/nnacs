/* NaPCnInp.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPCnInp.h"
#include "NaPCnOut.h"


//---------------------------------------------------------------------------
// Link to the host node
NaPetriCnInput::NaPetriCnInput (NaPetriNode* pHost, const char* szCnName)
: NaPetriConnector(pHost, szCnName)
{
    // Nothing to do
}


//---------------------------------------------------------------------------
// Destroy the connector
NaPetriCnInput::~NaPetriCnInput ()
{
    // N/A
}


//---------------------------------------------------------------------------
// Return the adjoint connector
NaPetriCnOutput*
NaPetriCnInput::adjoint ()
{
    if(0 == links()){
        return NULL;
    }
    return (NaPetriCnOutput*)pcaLinked[0];
}


//---------------------------------------------------------------------------

////////////////
// Overridden //
////////////////

//---------------------------------------------------------------------------
// Initialize the connector on the start of network life
void
NaPetriCnInput::init ()
{
    // The same as in adjoint "output" for to be passive in the first
    // activation cycle
    idData = 0;
}


//---------------------------------------------------------------------------
// Return type of the connector
NaPCnKind
NaPetriCnInput::kind ()
{
    return pckInput;
}


//---------------------------------------------------------------------------
// Return adjoint data vector
NaVector&
NaPetriCnInput::data ()
{
  static NaVector	vEmpty;
  if(NULL == adjoint())
    return vEmpty;
  return adjoint()->data();
}


//---------------------------------------------------------------------------
// Return true if waits for new data from adjoint connector
bool
NaPetriCnInput::is_waiting ()
{
    if(0 == links()){
        // Always ready
        return false;
    }else if(adjoint()->idData == idData){
        return true;
    }
    // New data are ready
    return false;
}


//---------------------------------------------------------------------------
// Complete data waiting period (after activate)
void
NaPetriCnInput::commit_data ()
{
    if(links() > 0){
        idData = adjoint()->idData;     // Mark own data as old
        ++adjoint()->nPrCnt;            // One more processed connector

	NaPetriConnector::commit_data();
    }
}


//---------------------------------------------------------------------------
// Link the connector with another one and return true on success
bool
NaPetriCnInput::link (NaPetriConnector* pLinked)
{
    if(NULL == pLinked)
        throw(na_null_pointer);

    if(pckOutput == pLinked->kind()){
        if(links() == 0){
            pcaLinked.addh(pLinked);
        }else{
            pcaLinked[0] = pLinked;
        }
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
// Describe to the log connector's state
void
NaPetriCnInput::describe ()
{
    NaPrintLog("input connector '%s': ", name());
    if(NULL == host()){
        NaPrintLog("no host node\n");
    }else if(0 == links()){
        NaPrintLog("no links\n");
    }else{
        NaPrintLog("data_id=%d(%d)  data_dim=%u\n",
                   idData, adjoint()->idData, data().dim());
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
