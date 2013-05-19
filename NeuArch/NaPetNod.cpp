/* NaPetNod.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NaStrOps.h"
#include "NaPetNod.h"
#include "NaPCnInp.h"
#include "NaPCnOut.h"


//---------------------------------------------------------------------------
// Autoname facility counter
int     NaPetriNode::iNodeNumber = 0;


//---------------------------------------------------------------------------
// Create node for Petri network
NaPetriNode::NaPetriNode (const char* szNodeName)
{
    bHalt = false;
    bTunable = true;
    if(NULL == szNodeName)
        szName = autoname("node", iNodeNumber);
    else
        szName = newstr(szNodeName);

    /* setup verbose from program environment */
    char	szEnvName[1024];
    sprintf(szEnvName, "%s_verbose", szName);
    if(NULL == getenv(szEnvName))
      bVerbose = false;
    else
      bVerbose = !!atoi(getenv(szEnvName));
}


//---------------------------------------------------------------------------
// Destroy the node
NaPetriNode::~NaPetriNode ()
{
    int i;
    for(i = 0; i < connectors(); ++i){
        delete connector(i);
    }
    delete[] szName;
}


//---------------------------------------------------------------------------

/////////////////
// Information //
/////////////////

//---------------------------------------------------------------------------
// Return name of the connector
NaPetriNet*
NaPetriNode::net () const
{
    return pNet;
}


//---------------------------------------------------------------------------
// Return name of the connector
const char*
NaPetriNode::name () const
{
    return szName;
}


//---------------------------------------------------------------------------
// Return number of connectors
int
NaPetriNode::connectors () const
{
    return pcaPorts.count();
}


//---------------------------------------------------------------------------
// Return pointer to given connector
NaPetriConnector*
NaPetriNode::connector (int i) const
{
    return pcaPorts(i);
}


//---------------------------------------------------------------------------
// Request for external change of some important node parameters
// 'true' means the net is dead or node is not linked
// 'false' means the net is alive and node can't be changed this time
bool
NaPetriNode::tunable () const
{
    return bTunable;
}


//---------------------------------------------------------------------------
// Check whether node is tunable and generate exception
void
NaPetriNode::check_tunable () const
{
    if(!tunable())
        throw(na_not_tunable);
}


//---------------------------------------------------------------------------
// Get number of past node's activations (incremented after action() call)
unsigned
NaPetriNode::activations () const
{
    return nActivations;
}


//---------------------------------------------------------------------------
// Get number of past node calls (incremented after activate() call)
unsigned
NaPetriNode::calls () const
{
    return nCalls;
}


//---------------------------------------------------------------------------
// Check whether the node is waiting for data
bool
NaPetriNode::is_waiting () const
{
    int     iCn;
    for(iCn = 0; iCn < connectors(); ++iCn){
        if(connector(iCn)->is_waiting()){
            return true;
        }
    }
    return false;
}


//---------------------------------------------------------------------------
// Check whether the node is verbose
bool
NaPetriNode::is_verbose () const
{
    return bVerbose;
}


//---------------------------------------------------------------------------

////////////
// Orders //
////////////

//---------------------------------------------------------------------------
// Say 'be verbose while execution' or 'be silent'
void
NaPetriNode::verbose (bool bFlag)
{
    bVerbose = bFlag;
}


//---------------------------------------------------------------------------
// Say 'lets terminate the whole net'
void
NaPetriNode::halt ()
{
    bHalt = true;
}


//---------------------------------------------------------------------------
// Say 'the data on all connectors are ready'
void
NaPetriNode::commit_data ()
{
    int     iCn;
    for(iCn = 0; iCn < pcaPorts.count(); ++iCn){
        pcaPorts[iCn]->commit_data();
    }
}


//---------------------------------------------------------------------------
// Describe to the log node's state
void
NaPetriNode::describe ()
{
    NaPrintLog("node '%s': called %u times, activated %u times\n"
               "  currently is %s data\n"
               "  connectors %u:\n",
               name(), calls(), activations(),
               is_waiting()? "waiting for": "ready to process",
               connectors());

    int     iCn;
    for(iCn = 0; iCn < pcaPorts.count(); ++iCn){
        NaPrintLog("  #%d ", iCn + 1);
        pcaPorts[iCn]->describe();
        if(pckOutput == pcaPorts[iCn]->kind()){
            NaPetriCnOutput &cout = *(NaPetriCnOutput*)pcaPorts[iCn];
            int             iACn;
            for(iACn = 0; iACn < cout.links(); ++iACn){
                if(!cout.adjoint(iACn)->is_waiting()){
                    NaPrintLog("    ? %s.%s\n",
                               cout.adjoint(iACn)->host()->name(),
                               cout.adjoint(iACn)->name());
                }
            }
        }/* if */
        if(pckOutput == pcaPorts[iCn]->kind() ||
           0 != pcaPorts[iCn]->links()){
            pcaPorts[iCn]->data().print_contents();
        }
    }/* for */
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPetriNode::main_input_cn ()
{
    int                 iCn;
    NaPetriConnector*   pInCn = NULL;
    for(iCn = 0; iCn < connectors(); ++iCn){
        if(connector(iCn)->kind() == pckInput){
            if(NULL != pInCn){
                // Not the only input connector
                return NULL;
            }
            pInCn = connector(iCn);
        }
    }
    return pInCn;
}


//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPetriNode::main_output_cn ()
{
    int                 iCn;
    NaPetriConnector*   pOutCn = NULL;
    for(iCn = 0; iCn < connectors(); ++iCn){
        if(connector(iCn)->kind() == pckOutput){
            if(NULL != pOutCn){
                // Not the only input connector
                return NULL;
            }
            pOutCn = connector(iCn);
        }
    }
    return pOutCn;
}


//---------------------------------------------------------------------------
// Add the connector
void
NaPetriNode::add_cn (NaPetriConnector* pCn)
{
    int iCn;
    if(!ask_for_cn(pCn, &iCn)){
        pcaPorts.addh(pCn);
    }
}


//---------------------------------------------------------------------------
// Remove the connector
void
NaPetriNode::del_cn (NaPetriConnector* pCn)
{
    int iCn;
    if(ask_for_cn(pCn, &iCn)){
        pcaPorts.remove(iCn);
    }
}


//---------------------------------------------------------------------------
// Ask for connector's index or return -1
bool
NaPetriNode::ask_for_cn (NaPetriConnector* pCn, int* pIndex)
{
    int i;
    for(i = 0; i < connectors(); ++i){
        if(pCn == connector(i)){
            if(NULL != pIndex){
                *pIndex = i;
            }
            return true;
        }
    }
    return false;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 0. Called once when the node becomes part of Petri network
void
NaPetriNode::attend_network ()
{
    // N/A
}


//---------------------------------------------------------------------------
// 1. Open input data and get their dimensions
void
NaPetriNode::open_input_data ()
{
    // N/A
}


//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPetriNode::relate_connectors ()
{
    // N/A
}


//---------------------------------------------------------------------------
// 3. Open output data and set their dimensions
void
NaPetriNode::open_output_data ()
{
    // N/A
}


//---------------------------------------------------------------------------
// 4. Allocate resources for internal usage
void
NaPetriNode::allocate_resources ()
{
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPetriNode::verify ()
{
    return true;    // if not overridden
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPetriNode::initialize (bool& starter)
{
    // N/A
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPetriNode::activate ()
{
    // Default behaviour is to check all connectors and
    // return false on any of them are is_waiting()
    return !is_waiting();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPetriNode::action ()
{
    // N/A
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPetriNode::post_action ()
{
    // Default behaviour is to make new data be ready by means
    // of commit_data() for each connector
    commit_data();
}


//---------------------------------------------------------------------------
// 10. Deallocate resources and close external data
void
NaPetriNode::release_and_close ()
{
    // N/A
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
