/* NaPNFOut.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <string.h>

#include "NaPNFOut.h"


//---------------------------------------------------------------------------
// Create node
NaPNFileOutput::NaPNFileOutput (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in")
{
    szDataFName = NULL;
    pDataF = NULL;
}


//---------------------------------------------------------------------------
// Destroy the node
NaPNFileOutput::~NaPNFileOutput ()
{
    delete[] szDataFName;
    delete pDataF;
}

//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new filename as a data destination (before open_input_data()
// and after release_and_close())
void
NaPNFileOutput::set_output_filename (const char* szFileName)
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
// 3. Open output data (pure input nodes) and set their dimensions
void
NaPNFileOutput::open_output_data ()
{
    if(NULL ==szDataFName)
        throw(na_null_pointer);

    delete pDataF;

    pDataF = OpenOutputDataFile(szDataFName, bdtAuto, in.data().dim());

    if(NULL != pDataF){
        unsigned    i;
        char        *vname = new char[strlen(name()) + 10];

        // Setup dimension of file data and variable titles
        for(i = 0; i < in.data().dim(); ++i){
            sprintf(vname, "%s%u", name(), i + 1);
            pDataF->SetVarName(i, vname);
        }

        delete[] vname;
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNFileOutput::verify ()
{
    if(NULL == pDataF){
        NaPrintLog("Node '%s': data destination is not defined.\n", name());
        return false;
    }
    return true;    // if not overridden
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNFileOutput::activate ()
{
    // Check data is read by data destination connectors
    if(!NaPetriNode::activate())
        // Data must be read before them will processed
        return false;

    // Add yet another data record to the file
    pDataF->AppendRecord();

    return true;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNFileOutput::action ()
{
    unsigned    i;

    // Write the data
    for(i = 0; i < in.data().dim(); ++i){
        pDataF->SetValue(in.data()[i], i);
    }
}


//---------------------------------------------------------------------------
// 10. Deallocate resources and close external data
void
NaPNFileOutput::release_and_close ()
{
    delete pDataF;  pDataF = NULL;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
