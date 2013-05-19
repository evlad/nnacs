/* NaCuFunc.cpp */
static char rcsid[] = "$Id$";

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifndef WIN32
#include <dlfcn.h>
#endif

#include "NaExFunc.h"
#include "NaCuFunc.h"


//-----------------------------------------------------------------------
// Registrar for the NaCustomFunc
NaConfigPart*
NaCustomFunc::NaRegCustomFunc ()
{
  return new NaCustomFunc();
}


//-----------------------------------------------------------------------
// Make custom function, given by shared object file
NaCustomFunc::NaCustomFunc ()
  : NaConfigPart(NaTYPE_CustomFunc), so(NULL), exfunc(NULL),
    szFile(NULL), szOptions(NULL)
{
  // Nothing to do
}


//-----------------------------------------------------------------------
// Destructor
NaCustomFunc::~NaCustomFunc ()
{
  Clean();
}


//-----------------------------------------------------------------------
// Print self
void
NaCustomFunc::PrintLog () const
{
  PrintLog(NULL);
}


//-----------------------------------------------------------------------
// Print self with indentation
void
NaCustomFunc::PrintLog (const char* szIndent) const
{
  if(Empty())
    return;

  const char	*indent = (NULL == szIndent)? "": szIndent;
  NaPrintLog("%sCustom function %s:\n", indent,
	     (NULL == GetInstance())? "": GetInstance());
  NaPrintLog("%s  file    %s\n", indent, szFile);
  NaPrintLog("%s  options %s\n", indent, NULL==szOptions?"(nil)":szOptions);
  NaPrintLog("%s  initial");

  unsigned	i;
  for(i = 0; i < vInitial.dim(); ++i){
    NaPrintLog(" %g", vInitial(i));
  }
  NaPrintLog("\n");

  NaPrintLog("%s  shared object     %p\n", so);
  NaPrintLog("%s  external function %p\n", exfunc);
}


//-----------------------------------------------------------------------
// Is empty
bool
NaCustomFunc::Empty () const
{
  return NULL == szFile;
}


//-----------------------------------------------------------------------
// Make empty
void
NaCustomFunc::Clean ()
{
  if(NULL != szFile){
    delete[] szFile;
    szFile = NULL;
  }
  if(NULL != szOptions){
    delete[] szOptions;
    szOptions = NULL;
  }
  vInitial.new_dim(0);

  if(NULL != exfunc){
    delete exfunc;
    exfunc = NULL;
  }
  if(NULL != so){
#ifdef WIN32
    FreeLibrary(so);
#else
    dlclose(so);
#endif
    so = NULL;
  }
}


//***********************************************************************
// Unit part
//***********************************************************************

//-----------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void
NaCustomFunc::Reset ()
{
  if(NULL == exfunc){
    NaPrintLog("Failed to Reset(): external function is not linked!\n");
    return;
  }

  exfunc->Reset();
}


//-----------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaCustomFunc::Function (NaReal* x, NaReal* y)
{
  if(NULL == exfunc){
    NaPrintLog("Dummy Function(): external function is not linked!\n");
    if(NULL != y && NULL != x)
      *y  = *x;
    return;
  }

  exfunc->Function(x, y);
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//-----------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaCustomFunc::Save (NaDataStream& ds)
{
  if(NULL != szFile)
    ds.PutF(NULL, "file    %s", szFile);
  if(NULL != szOptions)
    ds.PutF(NULL, "options %s", szOptions);

  switch(vInitial.dim()){
  case 0:
    // nothing
    break;

  case 1:
    // economic mode
    ds.PutF(NULL, "initial %g", vInitial[0]);
    break;

  case 2:
    // economic mode
    ds.PutF(NULL, "initial %g %g", vInitial[0], vInitial[1]);
    break;

  default:
    // expensive mode
    {
      char	*buffer = new char[8 + vInitial.dim() * 30];
      unsigned	i;
      strcpy(buffer, "initial");
      for(i = 0; i < vInitial.dim(); ++i){
	sprintf(buffer + strlen(buffer), " %g", vInitial[i]);
      }
      ds.PutF(NULL, "%s", buffer);
      delete[] buffer;
    }
    break;
  }
}


//-----------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaCustomFunc::Load (NaDataStream& ds)
{
  // Remove all previous functions
  Clean();

  // Read line-by-line
  while(true){
    try{
      char	*szKey, *szArg, *s = ds.GetData();

      szKey = strtok(s, " ");
      szArg = s + strlen(s) + 1;

      if(!strcmp(szKey, NaCF_file)){
	char	*filename = strtok(szArg, " ");

	// Replace file instead of old one
	if(NULL != szFile)
	  delete[] szFile;
	szFile = new char[strlen(filename) + 1];
	strcpy(szFile, filename);
      }else if(!strcmp(szKey, NaCF_options)){
	// Concatenate options
	if(NULL == szOptions){
	  szOptions = new char[strlen(szArg) + 1];
	  strcpy(szOptions, szArg);
	}else{
	  char	*szPrevOpt = szOptions;
	  szOptions = new char[strlen(szPrevOpt) + strlen(szArg) + 2];
	  sprintf(szOptions, "%s %s", szPrevOpt, szArg);
	}
      }else if(!strcmp(szKey, NaCF_initial)){
	char	*number;
	for(number = strtok(szArg, " ");
	    NULL != number;
	    number = strtok(NULL, " ")){
	  vInitial.new_dim(vInitial.dim() + 1);
	  vInitial[vInitial.dim() - 1] = atof(number);
	}
      }
    }catch(NaException exCode){
      if(na_end_of_partition == exCode ||
	 na_end_of_file == exCode)
	break;
      NaPrintLog("Exception %s while NaCustomFunc::Load(ds)\n",
		 NaExceptionMsg(exCode));
    }
  }

  if(NULL == szFile){
    NaPrintLog("Shared object file is not defined for %s (instance %s).\n",
	       GetType(),
	       (NULL==GetInstance())? "is not defined": GetInstance());
    return;
  }

  // Try to load shared object file
  if(NULL != getenv(NaEXFUNC_DIR_ENV)){
    char	*filepath = new char[strlen(getenv(NaEXFUNC_DIR_ENV))
				    + strlen(szFile)
				    + strlen(NaEXFUNC_DIR_SEP)
				    + strlen(NaEXFUNC_FILE_EXT) + 1];
    sprintf(filepath, "%s%s%s%s", getenv(NaEXFUNC_DIR_ENV),
	    NaEXFUNC_DIR_SEP, szFile, NaEXFUNC_FILE_EXT);
    NaPrintLog("Loading '%s' ...\n", filepath);
#ifdef WIN32
    so = LoadLibrary(filepath);
    if(NULL == so)
      NaPrintLog("Can't find shared object '%s': 0x%08x\n",
		 filepath, GetLastError());
#else
    so = dlopen(filepath, RTLD_LAZY);
    if(NULL == so)
      NaPrintLog("Can't find shared object '%s': %s\n",
		 filepath, dlerror());
#endif
    delete[] filepath;
  }

  if(NULL == so){
    NaPrintLog("Loading '%s' ...\n", szFile);
#ifdef WIN32
    so = LoadLibrary(szFile);
#else
    so = dlopen(szFile, RTLD_LAZY);
#endif
	}
  if(NULL == so){
#ifdef WIN32
    NaPrintLog("Failed to open '%s': 0x%08x\n", szFile, GetLastError());
#else
    NaPrintLog("Failed to open '%s': %s\n", szFile, dlerror());
#endif
    return;
  }

  NaCreateExternFuncProto	create =
#ifdef WIN32
	(NaCreateExternFuncProto)GetProcAddress(so, NaCreateExternFunc);
#else
	(NaCreateExternFuncProto)dlsym(so, NaCreateExternFunc);
#endif
	if(NULL == create){
#ifdef WIN32
		NaPrintLog("Bad external function object: 0x%08x\n", GetLastError());
		FreeLibrary(so);
#else
		NaPrintLog("Bad external function object: %s\n", dlerror());
		dlclose(so);
#endif
		so = NULL;
    return;
  }

  exfunc = (*create)(szOptions, vInitial);
}
