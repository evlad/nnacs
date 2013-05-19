/* NaPetCn.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "NaStrOps.h"
#include "NaExcept.h"
#include "NaPetNet.h"
#include "NaPetNod.h"
#include "NaPetCn.h"


//---------------------------------------------------------------------------
// Autoname facility counter
int     NaPetriConnector::iCnNumber = 0;


//---------------------------------------------------------------------------
// Link to the host node
NaPetriConnector::NaPetriConnector (NaPetriNode* pHost, const char* szCnName)
  : dfTrace(NULL)
{
    if(NULL == pHost)
        throw(na_null_pointer);

    pNode = pHost;

    if(NULL == szCnName)
        szName = autoname("conn", iCnNumber);
    else
        szName = newstr(szCnName);

    host()->add_cn(this);
}


//---------------------------------------------------------------------------
// Destroy the connector
NaPetriConnector::~NaPetriConnector ()
{
    host()->del_cn(this);

    delete[] szName;
}


//---------------------------------------------------------------------------
// Return pointer to host node
NaPetriNode*
NaPetriConnector::host () const
{
    return pNode;
}


//---------------------------------------------------------------------------
// Return name of the connector
const char*
NaPetriConnector::name () const
{
    return szName;
}


//---------------------------------------------------------------------------
// Return number of links of this connector
int
NaPetriConnector::links () const
{
    return pcaLinked.count();
}


//---------------------------------------------------------------------------
// Unlink the connector from this one and return true on success
bool
NaPetriConnector::unlink (NaPetriConnector* pLinked)
{
    int i;
    for(i = 0; i < pcaLinked.count(); ++i){
        if(pLinked == pcaLinked[i]){
            pcaLinked.remove(i);
            return true;
        }
    }
    return false;
}


//---------------------------------------------------------------------------
// Unlink all connectors from this one
void
NaPetriConnector::unlink ()
{
    pcaLinked.clean();
}


//---------------------------------------------------------------------------
// Called when dimension is already defined
void
NaPetriConnector::dim_ready ()
{
  // Check for env name with track file
  const char	*szNetName = host()->net()->name();
  const char	*szNodeName = host()->name();
  const char	szPrefix[] = "cn";
  const char	*szFileName;

  char	*szEnvName = new char[strlen(szPrefix) + 1
			     + strlen(szNetName) + 1
			     + strlen(szNodeName) + 1
			     + strlen(szName) + 1];

  /* The most exact */
  sprintf(szEnvName, "%s_%s_%s_%s", szPrefix, szNetName, szNodeName, szName);
  if(NULL != getenv(szEnvName)){
    szFileName = getenv(szEnvName);
    goto FILENAME_OK;
  }

  /* All connectors of the same net */
  sprintf(szEnvName, "%s_%s_%s", szPrefix, szNetName, szName);
  if(NULL != getenv(szEnvName)){
    szFileName = getenv(szEnvName);
    goto FILENAME_OK;
  }

  /* Exact connector of the given node (all nets) */
  sprintf(szEnvName, "%s_%s_%s", szPrefix, szNodeName, szName);
  if(NULL != getenv(szEnvName)){
    szFileName = getenv(szEnvName);
    goto FILENAME_OK;
  }

  /* Any connector of given name */
  sprintf(szEnvName, "%s_%s", szPrefix, szName);
  if(NULL != getenv(szEnvName)){
    szFileName = getenv(szEnvName);
    goto FILENAME_OK;
  }

  delete[] szEnvName;

  /* No env. variables were found */
  return;

 FILENAME_OK:

  dfTrace = OpenOutputDataFile(szFileName, bdtAuto, data().dim());

  delete[] szEnvName;

  if(NULL == dfTrace)
    throw(na_cant_open_file);
}


//---------------------------------------------------------------------------
// Called at network termination
void
NaPetriConnector::final ()
{
  if(NULL != dfTrace)
    delete dfTrace;

  dfTrace = NULL;
}


//---------------------------------------------------------------------------
// Complete data waiting period (after activate)
void
NaPetriConnector::commit_data ()
{
  if(NULL == dfTrace)
    return;

  unsigned    i;

  // Write the data passed trough the connector
  dfTrace->AppendRecord();
  for(i = 0; i < data().dim(); ++i){
    dfTrace->SetValue(data()[i], i);
  }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
