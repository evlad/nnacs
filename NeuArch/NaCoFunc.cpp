/* NaCoFunc.cpp */
static char rcsid[] = "$Id$";

#include <string.h>
#include <stdlib.h>

#include "NaTrFunc.h"
#include "NaCuFunc.h"

#include "NaCoFunc.h"


#define NaInfinity	(-1)


//-----------------------------------------------------------------------
// Reference to unit with time range of it's activity
struct NaTimedUnit
{
  NaUnit	*pUnit;		// reference to computational unit
  int		nTimeRange[2];	// -1 means +oo (Infinity)
};


//-----------------------------------------------------------------------
// Registrar for the NaCombinedFunc
NaConfigPart*
NaCombinedFunc::NaRegCombinedFunc ()
{
  return new NaCombinedFunc();
}


//-----------------------------------------------------------------------
// Make empty combined function
NaCombinedFunc::NaCombinedFunc ()
  : NaConfigPart(NaTYPE_CombinedFunc),
    nParts(0), pParts(NULL), iTime(0),
    conf_file(";NeuCon combined function", 1, 1, ".cof")
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Destructor
NaCombinedFunc::~NaCombinedFunc ()
{
  Clean();
}


//-----------------------------------------------------------------------
// Print self
void
NaCombinedFunc::PrintLog () const
{
  PrintLog(NULL);
}


//-----------------------------------------------------------------------
// Print self with indentation
void
NaCombinedFunc::PrintLog (const char* szIndent) const
{
  unsigned	i;
  const char	*indent = (NULL == szIndent)? "": szIndent;

  NaPrintLog("%sCombined function %s:\n", indent,
	     (NULL == GetInstance())? "": GetInstance());
  for(i = 0; i < nParts; ++i){
    NaTimedUnit	*tu = (NaTimedUnit*)pParts[i]->pSelfData;
    if(0 == tu->nTimeRange[0] && NaInfinity == tu->nTimeRange[1])
      NaPrintLog("%s  %s %s\n", indent,
		 pParts[i]->GetType(), pParts[i]->GetInstance());
    else if(NaInfinity == tu->nTimeRange[1])
      NaPrintLog("%s  %s %s  [%d..+oo]\n", indent,
		 pParts[i]->GetType(), pParts[i]->GetInstance(),
		 tu->nTimeRange[0]);
    else
      NaPrintLog("%s  %s %s  [%d..%d]\n", indent,
		 pParts[i]->GetType(), pParts[i]->GetInstance(),
		 tu->nTimeRange[0], tu->nTimeRange[1]);
  }
}


//-----------------------------------------------------------------------
// Is empty
bool
NaCombinedFunc::Empty () const
{
  return 0 == nParts;
}


//-----------------------------------------------------------------------
// Make empty
void
NaCombinedFunc::Clean ()
{
  unsigned	i;
  for(i = 0; i < nParts; ++i){
    NaTimedUnit	*tu = (NaTimedUnit*)pParts[i]->pSelfData;
    delete tu;
  }

  nParts = 0;
  delete[] pParts;
}


//***********************************************************************
// Unit part
//***********************************************************************

//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void
NaCombinedFunc::Reset ()
{
  unsigned	i;
  for(i = 0; i < nParts; ++i){
    NaTimedUnit	*tu = (NaTimedUnit*)pParts[i]->pSelfData;
    tu->pUnit->Reset();
  }
  iTime = 0;
}


//-----------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaCombinedFunc::Function (NaReal* x, NaReal* y)
{
  unsigned	i;
  NaReal	tmp = *x;

  for(i = 0; i < nParts; ++i){
    NaTimedUnit	*tu = (NaTimedUnit*)pParts[i]->pSelfData;

    if(iTime >= tu->nTimeRange[0] &&
       (NaInfinity == tu->nTimeRange[1] ||
	iTime < tu->nTimeRange[1])){
      // Activate function only in case of 1) eternal time range or 2)
      // current time index is in time range of the unit
      tu->pUnit->Function(&tmp, y);
      tmp = *y;
    }
  }
  *y = tmp;

  ++iTime;
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//-----------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaCombinedFunc::Save (NaDataStream& ds)
{
  unsigned	i;
  for(i = 0; i < nParts; ++i){
    ds.PutF(NULL, "%s %s", pParts[i]->GetType(), pParts[i]->GetInstance());
  }
}


//-----------------------------------------------------------------------
// Structured line of combined function
struct item_t {
  char	*szType;
  char	*szInstance;
  int	nTimeRange[2];
};


//-----------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaCombinedFunc::Load (NaDataStream& ds)
{
  // Remove all previous functions
  Clean();

  item_t	item;
  NaDynAr<item_t>	items;

  // Read line-by-line
  while(true){
    try{
      char	*szType, *szInstance, *szStartTime, *szEndTime;

      szType = strtok(ds.GetData(), " ");
      szInstance = strtok(NULL, " ");
      szStartTime = strtok(NULL, " ");
      szEndTime = strtok(NULL, " ");

      if(NULL == szType || NULL == szInstance)
	continue;

      if(strcmp(szType, NaTYPE_CustomFunc) &&
	 strcmp(szType, NaTYPE_TransFunc)){
	NaPrintLog("Function type '%s' is not defined -> skip it.\n", szType);
	continue;
      }

      item.szType = new char[strlen(szType) + 1];
      item.szInstance = new char[strlen(szInstance) + 1];

      if(NULL != szStartTime && NULL != szEndTime)
	{
	  item.nTimeRange[0] = atoi(szStartTime);
	  item.nTimeRange[1] = atoi(szEndTime);
	}
      else if(NULL != szStartTime && NULL == szEndTime)
	{
	  item.nTimeRange[0] = atoi(szStartTime);
	  item.nTimeRange[1] = NaInfinity;
	}
      else // time range is not defined, so define eternal time range
	{
	  item.nTimeRange[0] = 0;
	  item.nTimeRange[1] = NaInfinity;
	}

      strcpy(item.szType, szType);
      strcpy(item.szInstance, szInstance);

      items.addh(item);

    }catch(NaException exCode){
      if(na_end_of_partition == exCode ||
	 na_end_of_file == exCode)
	break;
      NaPrintLog("Exception %s while NaCombinedFunc::Load(ds)\n",
		 NaExceptionMsg(exCode));
    }
  }
  // EOF or end of partition is reached

  // Lets make array of partitions
  nParts = items.count();
  pParts = new NaConfigPart*[nParts];

  unsigned	i;
  for(i = 0; i < nParts; ++i){
    NaTimedUnit	*tu = new NaTimedUnit;

    tu->nTimeRange[0] = items[i].nTimeRange[0];
    tu->nTimeRange[1] = items[i].nTimeRange[1];

    if(!strcmp(items[i].szType, NaTYPE_TransFunc)){
      NaTransFunc	*p = new NaTransFunc;
      pParts[i] = p;
      tu->pUnit = p;
      pParts[i]->pSelfData = tu;
    }else if(!strcmp(items[i].szType, NaTYPE_CustomFunc)){
      NaCustomFunc	*p = new NaCustomFunc;
      pParts[i] = p;
      tu->pUnit = p;
      pParts[i]->pSelfData = tu;
    }
    pParts[i]->SetInstance(items[i].szInstance);

    delete[] items[i].szType;
    delete[] items[i].szInstance;
  }

  // Register all expected partitions
  conf_file.AddPartitions(nParts, pParts);
}


//-----------------------------------------------------------------------
// Store sequence of functions to file
void
NaCombinedFunc::Save (const char* szFileName)
{
  conf_file.SaveToFile(szFileName);
}


//-----------------------------------------------------------------------
// Read sequence of functions from given configuration file.  Smart
// enough to take care of transfer (.tf) and combined functions (.cof)
void
NaCombinedFunc::Load (const char* szFileName)
{
  if(NULL == szFileName)
    throw(na_null_pointer);
  if(strlen(szFileName) >= 3){
    if(!strcmp(szFileName + strlen(szFileName) - 3, ".tf")){
      // Old transfer function file
      Clean();

      nParts = 1;
      pParts = new NaConfigPart*[nParts];

      NaTransFunc	*p = new NaTransFunc;
      pParts[0] = p;

      // Load the transfer function
      p->Load(szFileName);

      // Just to have the save facility
      conf_file.RemovePartitions();
      conf_file.AddPartitions(nParts, pParts);

      NaTimedUnit	*tu = new NaTimedUnit;

      tu->nTimeRange[0] = 0;
      tu->nTimeRange[1] = NaInfinity;
      tu->pUnit = p;

      pParts[0]->pSelfData = tu;

      return;
    }
  }

  // Combined function file
  NaConfigPart	*conf_list[] = { this };
  conf_file.RemovePartitions();
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.LoadFromFile(szFileName);
}
