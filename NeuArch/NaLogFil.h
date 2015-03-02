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
PNNA_API void    NaOpenLogFile (const char* szFile);

// Close log file
PNNA_API void    NaCloseLogFile ();

// Turn on/off logging
PNNA_API void    NaSwitchLogFile (bool state);

// Prints a message to log file if it's open
PNNA_API void    NaPrintLog (const char* fmt, ...);


//---------------------------------------------------------------------------
class PNNA_API NaLogging
{
public:

    // Print log without indentation
    virtual void    PrintLog () const;

    // Print log with indentation
    //virtual void    PrintLog (const char* szIndent) const;
    
};


//---------------------------------------------------------------------------
#endif
