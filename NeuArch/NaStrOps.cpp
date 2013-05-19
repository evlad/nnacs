/* NaStrOps.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include "NaStrOps.h"


//---------------------------------------------------------------------------
// Allocate new string and copy its from given in argument
char*
newstr (const char* s)
{
    char    *dup;

    if(NULL == s){
        s = "";
    }

    dup = new char[strlen(s) + 1];
    strcpy(dup, s);

    return dup;
}


//---------------------------------------------------------------------------
// Autoname facility
char*
autoname (const char* root, int& iCounter)
{
    char    *aname;

    if(NULL == root)
        root = "name";

    aname = new char[strlen(root) + 20];
    sprintf(aname, "%s%d", root, ++iCounter);

    return aname;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
