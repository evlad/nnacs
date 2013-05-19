/* NaPNFIn.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <string.h>

#include "NaPNFIn.h"


//---------------------------------------------------------------------------
// Create node
NaPNFileInput::NaPNFileInput (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  out(this, "out")
{
    szDataFName = NULL;
    pDataF = NULL;
}


//---------------------------------------------------------------------------
// Destroy the node
NaPNFileInput::~NaPNFileInput ()
{
    delete[] szDataFName;
    delete pDataF;
}

//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new filename as a data source (before open_input_data() and
// after release_and_close())
void
NaPNFileInput::set_input_filename (const char* szFileName)
{
    check_tunable();

    if(NULL == szFileName)
        throw(na_null_pointer);

    delete[] szDataFName;  szDataFName = NULL;

    szDataFName = new char[strlen(szFileName) + 1];
    strcpy(szDataFName, szFileName);
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 1. Open input data and get their dimensions
void
NaPNFileInput::open_input_data ()
{
    // Assign data dimension
    int     nDim = 1;
    char    **szVarName;

    if(NULL == szDataFName)
        throw(na_null_pointer);

    delete pDataF;

    pDataF = OpenInputDataFile(szDataFName);

    if(NULL != pDataF){
        pDataF->GetVarNameList(nDim, szVarName);
    }

    out.data().new_dim(nDim);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNFileInput::verify ()
{
    if(NULL == pDataF){
        NaPrintLog("Node '%s': data source is not defined.\n", name());
        return false;
    }

    // First call to check whether data file is not empty
    if(false == pDataF->GoStartRecord()){
        NaPrintLog("Node '%s': source data file '%s' is empty.\n",
                   name(), szDataFName);
        return false;
    }

    return true;    // if not overridden
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNFileInput::activate ()
{
    // Check data is read by data destination connectors
    if(!NaPetriNode::activate())
        // Data must be stored until all destination connectors
        // would read them
        return false;

    // Return 'true' if there are data for reading or 'false' otherwise
    if(0 == activations())
        return pDataF->GoStartRecord();
    return pDataF->GoNextRecord();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNFileInput::action ()
{
    unsigned    i;

    // Read the data
    for(i = 0; i < out.data().dim(); ++i){
        out.data()[i] = pDataF->GetValue(i);
    }
}


//---------------------------------------------------------------------------
// 10. Deallocate resources and close external data
void
NaPNFileInput::release_and_close ()
{
    delete pDataF;  pDataF = NULL;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
 
