/* NaPCnOut.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaLogFil.h"
#include "NaPCnOut.h"
#include "NaPCnInp.h"
#include "NaPetNod.h"


//---------------------------------------------------------------------------
// Link to the host node
NaPetriCnOutput::NaPetriCnOutput (NaPetriNode* pHost, const char* szCnName)
  : NaPetriConnector(pHost, szCnName), bStarter(false)
{
    // Nothing to do
}


//---------------------------------------------------------------------------
// Destroy the connector
NaPetriCnOutput::~NaPetriCnOutput ()
{
    // N/A
}


//---------------------------------------------------------------------------
// Return given adjoint connector {iCn=0..links()-1}
NaPetriCnInput*
NaPetriCnOutput::adjoint (int iCn)
{
    if(iCn < 0 || iCn >= links()){
        return NULL;
    }
    return (NaPetriCnInput*)pcaLinked[iCn];
}


//---------------------------------------------------------------------------

////////////////
//  Specific  //
////////////////

//---------------------------------------------------------------------------
// Say 'generate initial impulse in network'
void
NaPetriCnOutput::set_starter (const NaVector& rInitial)
{
  bStarter = true;
  rData0 = rInitial;
}


//---------------------------------------------------------------------------

////////////////
// Overridden //
////////////////

//---------------------------------------------------------------------------
// Initialize the connector on the start of network life
void
NaPetriCnOutput::init ()
{
    // The same as in adjoint "input" for to be passive in the first
    // activation cycle
    idData = 0;

    // No adjoint connectors which processed the data
    nPrCnt = links();

    // Initial assignment and commit data
    if(bStarter)
      {
	NaPrintLog("node '%s', connector '%s' is << STARTER >>\n",
		   host()->name(), name());

	rData = rData0;
	commit_data();
      }
}


//---------------------------------------------------------------------------
// Return type of the connector
NaPCnKind
NaPetriCnOutput::kind ()
{
    return pckOutput;
}


//---------------------------------------------------------------------------
// Return data vector
NaVector&
NaPetriCnOutput::data ()
{
    return rData;
}


//---------------------------------------------------------------------------
// Return true if waits for reading own data by adjoint connectors
bool
NaPetriCnOutput::is_waiting ()
{
    //!// Allow to increase nPrCnt more than links number, so one-node
    //!// loop become accepted
    //!// if(nPrCnt >= links()){
    if(nPrCnt == links()){
        // Data can be released
        return false;
    }
    return true;
}


//---------------------------------------------------------------------------
// Complete data waiting period (after activate)
void
NaPetriCnOutput::commit_data ()
{
    nPrCnt = 0;                 // No processed adjoint connectors
    ++idData;                   // New data identifier

    NaPetriConnector::commit_data();
}


//---------------------------------------------------------------------------
// Link the connector with another one and return true on success
bool
NaPetriCnOutput::link (NaPetriConnector* pLinked)
{
    if(NULL == pLinked)
        throw(na_null_pointer);

    // Don't add the same connector twice
    int i;
    for(i = 0; i < pcaLinked.count(); ++i){
        if(pLinked == pcaLinked[i]){
            // Found!
            NaPrintLog("LINK FAILED: trying to bind %s.%s to %s.%s twice\n",
                       pLinked->host(), pLinked->name(), host(), name());
            return false;
        }
    }

    // Not found
    if(pckInput == pLinked->kind()){
        pcaLinked.addh(pLinked);
        ++nPrCnt;
        return true;
    }

    // Bad kind
    NaPrintLog("LINK FAILED: bad kind binding %s.%s to %s.%s\n",
               pLinked->host(), pLinked->name(), host(), name());
    return false;
}


//---------------------------------------------------------------------------
// Unlink the connector from this one
bool
NaPetriCnOutput::unlink (NaPetriConnector* pLinked)
{
    if(NaPetriConnector::unlink(pLinked)){
        --nPrCnt;
        return true;
    }
    return false;
}


//---------------------------------------------------------------------------
// Unlink all connectors from this one
void
NaPetriCnOutput::unlink ()
{
    NaPetriConnector::unlink();
    ///nPrCnt = 0;
}


//---------------------------------------------------------------------------
// Describe to the log connector's state
void
NaPetriCnOutput::describe ()
{
    NaPrintLog("output%s connector '%s': ",
	       bStarter?" << STARTER >>": "", name());
    if(NULL == host()){
        NaPrintLog("no host node\n");
    //}else if(0 == links()){
    //    NaPrintLog("no links\n");
    }else{
        NaPrintLog("links=%u  pr.cnt=%u  data_id=%d  data_dim=%u\n",
                   links(), nPrCnt, idData, data().dim());
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
