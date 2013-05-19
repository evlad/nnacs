//-*-C++-*-
/* NaLogFil.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaLogFilH
#define NaLogFilH
//---------------------------------------------------------------------------

#include <stdio.h>
#include "NaGenerl.h"

//extern FILE *fpNaLog;

// Open log file
void    NaOpenLogFile (const char* szFile);

// Close log file
void    NaCloseLogFile ();

// Turn on/off logging
void    NaSwitchLogFile (bool state);

// Prints a message to log file if it's open
void    NaPrintLog (const char* fmt, ...);


//---------------------------------------------------------------------------
class NaLogging
{
public:

    // Print log without indentation
    virtual void    PrintLog () const;

    // Print log with indentation
    //virtual void    PrintLog (const char* szIndent) const;
    
};


//---------------------------------------------------------------------------
#endif
