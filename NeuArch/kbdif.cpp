/* kbdif.cpp */
static char rcsid[] = "$Id$";

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "kbdif.h"


//---------------------------------------------------------------------------
// Ask for string parameter
char*
ask_user_string (const char* szPrompt, const char* szDefault)
{
    char    *szResult;
    char    enter[1024] = "";
 repeat_enter:
    if(NULL != szDefault)
      printf("%s <%s>: ", szPrompt, szDefault);
    else
      printf("%s: ", szPrompt, szDefault);
    fgets(enter, sizeof(enter)-1, stdin);
    if(('\0' == enter[0] || '\n' == enter[0]) && NULL != szDefault)
      {
	szResult = new char[strlen(szDefault) + 1];
	strcpy(szResult, szDefault);
      }
    else if(('\0' == enter[0] || '\n' == enter[0]) && NULL == szDefault)
      {
	/* default value is not provided - repeat enter */
	goto repeat_enter;
      }
    else
      {
	if('\n' == enter[strlen(enter)-1])
	  enter[strlen(enter)-1] = '\0';
	szResult = new char[strlen(enter) + 1];
	strcpy(szResult, enter);
      }
    return szResult;
}


//---------------------------------------------------------------------------
// Ask for name parameter (space limited)
char*
ask_user_name (const char* szPrompt, const char* szDefault)
{
  char	*szResult = ask_user_string(szPrompt, szDefault);
  char	*s1, *s2, *szNewResult;

  // strip spaces
  s1 = szResult;
  while(isspace(*s1) && '\0' != *s1 && '\n' != *s1)
    ++s1;

  s2 = s1;
  while(!isspace(*s2) && '\0' != *s2 && '\n' != *s2)
    ++s2;
  *s2 = '\0';

  szNewResult = new char[strlen(s1) + 1];
  strcpy(szNewResult, s1);

  delete szResult;

  return szNewResult;
}


//---------------------------------------------------------------------------
// Ask for real parameter
NaReal
ask_user_real (const char* szPrompt, NaReal fDefault)
{
    char    enter[30] = "";
    printf("%s <%g>: ", szPrompt, fDefault);
    fgets(enter, sizeof(enter)-1, stdin);
    if('\0' == enter[0] || '\n' == enter[0])
        return fDefault;
    return strtod(enter, NULL);
}


//---------------------------------------------------------------------------
// Ask for integer parameter
int
ask_user_int (const char* szPrompt, int iDefault)
{
    char    enter[30] = "";
    printf("%s <%d>: ", szPrompt, iDefault);
    fgets(enter, sizeof(enter)-1, stdin);
    if('\0' == enter[0] || '\n' == enter[0])
        return iDefault;
    return strtol(enter, NULL, 10);
}


//---------------------------------------------------------------------------
// Ask for boolean parameter
bool
ask_user_bool (const char* szPrompt, bool bDefault)
{
    char    enter[30] = "";
    const char *szSelVals;
    if(bDefault){
        szSelVals = "<y>,n";
    }else{
        szSelVals = "y,<n>";
    }

    do{
        printf("%s (%s): ", szPrompt, szSelVals);
        fgets(enter, sizeof(enter)-1, stdin);
    }while('y' != enter[0] && 'n' != enter[0] &&
	   '\0' != enter[0] && '\n' != enter[0]);

    switch(enter[0]){
    case 'y':
        return true;
    case 'n':
        return false;
    }
    return bDefault;
}


//---------------------------------------------------------------------------
// Ask for learning parameters
void
ask_user_lpar (NaStdBackPropParams& lpar)
{
    lpar.eta = ask_user_real("Learning rate (hidden layers)", lpar.eta);
    lpar.eta_output = ask_user_real("Learning rate (output layer)",
                               lpar.eta_output);
    lpar.alpha = ask_user_real("Momentum (inertia)", lpar.alpha);

    NaPrintLog("Learning parameters: lrate=%g  lrate(out)=%g  momentum=%g\n",
                lpar.eta, lpar.eta_output, lpar.alpha);
}
