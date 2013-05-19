//-*-C++-*-
/* NaStrOps.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaStrOpsH
#define NaStrOpsH

//---------------------------------------------------------------------------
// General string operations


// Allocate new string and copy its from given in argument
char*       newstr (const char* s);

// Autoname facility
char*       autoname (const char* root, int& iCounter);

//---------------------------------------------------------------------------
#endif
