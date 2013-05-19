/* NaParams.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "NaExcept.h"
#include "NaLogFil.h"
#include "NaParams.h"

#define COMMENT		0
#define ASSIGN		1


//---------------------------------------------------------------------------
// Open file with parameters
NaParams::NaParams (const char* szFileName,
		    /* Extra values in format argv[i]=" name = value " */
		    int argc, char** argv,
		    const char szSpecChar[2])
{
  if(NULL == szSpecChar)
    throw(na_null_pointer);

  spec[COMMENT] = szSpecChar[COMMENT];
  spec[ASSIGN] = szSpecChar[ASSIGN];

  FILE	*fp;
  stored_n = 0;

  if(NULL != szFileName)
    {
      // open special logfile derived from name of parameter
      char	*szPath = strdup(szFileName);
      char	*szExt = strstr(szPath, ".par");
      if(NULL != szExt && szExt[4] == '\0')
	{
	  // let's replace log file with new one but in current directory
	  NaCloseLogFile();
	  strcpy(szExt, ".log");
	  char	*szFile = strrchr(szPath, '/');
	  if(NULL == szFile)
	    /* local already */
	    szFile = szPath - 1;
	  NaOpenLogFile(szFile + 1);
	}
      free(szPath);

      // open file
      fp = fopen(szFileName, "rb"); // rt
      if(NULL == fp)
	throw(na_cant_open_file);

      // read file to get high estimation for number of lines in it
      while(NULL != fgets(buf, NaPARAM_LINE_MAX_LEN, fp))
	{
	  // skip leading spaces
	  char	*p;

	  // skip leading spaces before name
	  p = buf;
	  while(isspace(*p) && *p != '\0')
	    ++p;
	  if(*p != '\0' && *p != spec[COMMENT])
	    // non-empty line
	    ++stored_n;
	  // else
	  //   no name in this line due to empty line
	}

      // rewind file
      rewind(fp);
    }

  if(argc > 0 && argv != NULL)
    {
      // add lines for additional arguments in command line
      stored_n += argc;
    }

  // allocate storage
  if(0 == stored_n)
    {
      NaPrintLog("File '%s' does not contain parameters.\n",
		 szFileName);
      storage = NULL;
    }
  else
    {
      char	*name, *value;
      char	szAbsent[] = "?absent";
      storage = new item_t[stored_n];

      //int	item_i = 0;
      stored_n = 0;

      // read file to the storage
      while(NULL != fgets(buf, NaPARAM_LINE_MAX_LEN, fp))
	{
	  // parse line of text
	  if(!parse_line(buf, name, value))
	    {
	      if(NULL == name)
		// comment line
		continue;
	      // empty value
	      value = szAbsent;
	    }

	  NaPrintLog("name='%s' value='%s' ", name, value);

	  if(CheckParam(name))
	    {
	      // already exist -> replace value
	      char*&	value_ref = FetchParam(name);
	      delete[] value_ref;

	      value_ref = new char[strlen(value) + 1];
	      strcpy(value_ref, value);

	      NaPrintLog("REPLACE\n");
	    }
	  else
	    {
	      // not exist
	      storage[stored_n].name = new char[strlen(name) + 1];
	      strcpy(storage[stored_n].name, name);

	      storage[stored_n].value = new char[strlen(value) + 1];
	      strcpy(storage[stored_n].value, value);

	      // go to next command line argument and next item
	      ++stored_n;

	      // quick sort the storage to have proper order
	      qsort(storage, stored_n, sizeof(item_t), stored_cmp);

	      NaPrintLog("APPEND\n");
	    }
	}

      // parse additional arguments from command line
      int	i;
      for(i = 0; i < argc; ++i)
	{
	  // parse command line argument
	  if(!parse_line(argv[i], name, value))
	    {
	      if(NULL == name)
		// comment line
		continue;
	      // empty value
	      value = szAbsent;
	    }

	  NaPrintLog("cmd.line: name='%s' value='%s' ", name, value);

	  if(CheckParam(name))
	    {
	      // already exist -> replace value
	      char*&	value_ref = FetchParam(name);
	      delete[] value_ref;

	      value_ref = new char[strlen(value) + 1];
	      strcpy(value_ref, value);

	      NaPrintLog("REPLACE\n");
	    }
	  else
	    {
	      // not exist
	      storage[stored_n].name = new char[strlen(name) + 1];
	      strcpy(storage[stored_n].name, name);

	      storage[stored_n].value = new char[strlen(value) + 1];
	      strcpy(storage[stored_n].value, value);

	      // go to next command line argument and next item
	      ++stored_n;

	      // quick sort the storage to have proper order
	      qsort(storage, stored_n, sizeof(item_t), stored_cmp);

	      NaPrintLog("APPEND\n");
	    }
	}

      //stored_n = item_i;
    }

  fclose(fp);

  // quick sort the storage
  qsort(storage, stored_n, sizeof(item_t), stored_cmp);
}


//---------------------------------------------------------------------------
NaParams::~NaParams ()
{
  delete storage;
}


//---------------------------------------------------------------------------
// Get parameter's value by his name
char*
NaParams::GetParam (const char* szParamName) const
{
  item_t	it, *pit;
  it.name = (char*)szParamName;
  it.value = NULL;

  char	szNotFound[] = "?not found";
  char	*szParamValue = szNotFound;

  pit = (item_t*)bsearch(&it, storage, stored_n, sizeof(item_t), stored_cmp);
  if(NULL != pit)
    szParamValue = pit->value;

  NaPrintLog("Query for parameter '%s' gives value '%s'\n",
	     szParamName, szParamValue);

  return szParamValue;
}


//---------------------------------------------------------------------------
// Fetch parameter's value by his name
char*&
NaParams::FetchParam (const char* szParamName)
{
  item_t	it, *pit;
  it.name = (char*)szParamName;
  it.value = NULL;

  const char	*szParamValue = "?not found";

  pit = (item_t*)bsearch(&it, storage, stored_n, sizeof(item_t), stored_cmp);
  if(NULL != pit)
    return pit->value;

  throw(na_not_found);
}


// Check for parameter existence
bool
NaParams::CheckParam (const char* szParamName) const
{
  item_t	it, *pit;
  it.name = (char*)szParamName;
  it.value = NULL;

  pit = (item_t*)bsearch(&it, storage, stored_n, sizeof(item_t), stored_cmp);
  if(NULL != pit)
    return true;
  return false;
}


//---------------------------------------------------------------------------
// Get array of parameters listed in the line
char**
NaParams::GetListOfParams (const char* szParamName,
			   int& nList,
			   const char* szDelims) const
{
  item_t	it, *pit;
  it.name = (char*)szParamName;
  it.value = NULL;

  char	**szResult;
  const char	*szParamValue;

  pit = (item_t*)bsearch(&it, storage, stored_n, sizeof(item_t), stored_cmp);
  if(NULL == pit)
    {
      nList = 0;
      szParamValue = "?not found";
    }
  else
    {
      /* split value by delimiters */
      char	*szParVal, *p, *buf = new char[strlen(pit->value) + 1];
      strcpy(buf, pit->value);

      /* pass 1: count all parameters */
      for(p = buf, nList = 0;
	  NULL != strtok(p, szDelims);
	  p = NULL, ++nList)
	;

      if(nList > 0)
	{
	  int	i;

	  /* allocate array of pointers */
	  szResult = new char*[nList];

	  /* pass 2: storing arguments */
	  for(i = 0, p = buf;
	      i < nList;
	      ++i, p += strlen(p) + 1)
	    szResult[i] = p;
	}
      else
	{
	  nList = 0;
	  szParamValue = "?empty list";
	}
    }

  if(0 == nList)
    {
      szResult = new char*[nList];
      szResult[0] = new char[strlen(szParamValue) + 1];
      strcpy(szResult[0], szParamValue);
      nList = 1;
    }

  return szResult;
}


//---------------------------------------------------------------------------
// Put parameters to log file
void
NaParams::ListOfParamsToLog () const
{
  int	i;
  NaPrintLog("----------------------------->8----------------------------\n");
  for(i = 0; i < stored_n; ++i)
    NaPrintLog("%s=%s\n", storage[i].name, storage[i].value);
  NaPrintLog("-----------------------------8<----------------------------\n");
}


//---------------------------------------------------------------------------
// Parse the line to name and value or return false in case of bad syntax
bool
NaParams::parse_line (char* line, char*& name, char*& value)
{
  // skip leading spaces before name
  name = line;
  while(isspace(*name) && *name != '\0')
    ++name;
  if(*name == '\0' || *name == spec[COMMENT])
    {
      // no name in this line due to empty or comment line
      name = NULL;
      return false;
    }

  // find end of the name
  value = name;
  while(!isspace(*value) && *value != '\0' &&
	*value != spec[ASSIGN])
    ++value;

  if(*value == spec[ASSIGN])
    {
      // assignment is found just after the name
      *value = '\0';	// end of the name
      ++value;	// start of the value
    }
  else
    {
      // assignment still is not found
      *value = '\0';	// end of the name
      ++value;	// start of the region to find value

      if(NULL == (value = strchr(value, spec[ASSIGN])))
	// no assignment char in the line
	return false;
      else
	++value;
    }

  // skip leading spaces before value
  while(isspace((unsigned char)*value) && *value != '\0')
    ++value;

  // skip final spaces after value
  char	*eov = value + strlen(value);

  // twice check due to MS-DOS \r\n 
  if('\n' == eov[-1] || '\r' == eov[-1])
    --eov;
  if('\n' == eov[-1] || '\r' == eov[-1])
    --eov;
  *eov = '\0';

  // skip final spaces after value
  while(isspace((unsigned char)*eov))
    --eov;

  *eov = '\0';

  return true;
}


//---------------------------------------------------------------------------
// Compare two items
int
NaParams::stored_cmp (const void* p1, const void* p2)
{
  const item_t	*it1 = (item_t*)p1, *it2 = (item_t*)p2;
  return strcmp(it1->name, it2->name);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)

#if 0
/***********************************************************************
 * Test of NaParams class.
 ***********************************************************************/
#include <stdio.h>

#include <NaLogFil.h>
#include <NaExcept.h>

#include <NaParams.h>


int main (int argc, char* argv[])
{
  if(2 != argc)
    {
      fprintf(stderr, "Usage: testpar ParamFile\n");
      return 1;
    }

  try{
    NaParams	par(argv[1]);
    char	*names[] = {
      "parA", "parB", "parC", "parD", "parE", "parF", "par?"
    };
    int	i;

    for(i = 0; i < sizeof(names)/sizeof(names[0]); ++i)
      printf("name='%s' -> value='%s'\n", names[i], par(names[i]));

    printf("****\n");
    for(i = 0; i < sizeof(names)/sizeof(names[0]); ++i)
      printf("name='%s' -> check=%d\n", names[i], par.CheckParam(names[i]));

    printf("++++\n");
  }
  catch(NaException& exCode){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(exCode));
  }

  return 0;
}
#endif
