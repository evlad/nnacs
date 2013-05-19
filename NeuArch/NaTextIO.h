//-*-C++-*-
/* NaTextIO.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaTextIOH
#define NaTextIOH
//---------------------------------------------------------------------------

#include <stdio.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Class for simple text data file manipulation.
class NaTextStreamFile : public NaDataFile
{
public:

    // Create a stream or simply read it
    NaTextStreamFile (const char* fname,
                      NaFileMode fm = fmReadOnly);

    // Close (and write) file
    virtual ~NaTextStreamFile ();

    //***********************************
    // Per cell operations
    //***********************************

    // Access to data cell in current record
    // Variables are numbered with 0 base
    virtual void    SetValue (NaReal fVal, int iVar = 0);
    virtual NaReal  GetValue (int iVar = 0) const;

    //***********************************
    // Per record operations
    //***********************************

    // Go to next record
    // Return true if the next record exists
    virtual bool    GoNextRecord ();

    // Go to start record: reset cycle of file reading
    // Return true if the next record exists
    virtual bool    GoStartRecord ();

    // Append one empty record at the end of the file
    virtual void    AppendRecord ();

    // Return number of records in the file
    virtual long    CountOfRecord ();

    //***********************************
    // Variable operations
    //***********************************

    // Get list of variables
    virtual void    GetVarNameList (int& nVars, char**& sVars);

    // Set name of the variable
    // Variables are numbered with 0 base
    virtual void    SetVarName (int iVar, const char* szVarName);

protected:

    // File handle
    FILE        *fp;

    // Index of the current record or EOF if so
    long        iCase;

    // Preread current value [nVar]
    NaReal      *fCurVal;

    // Line buffer
#define LINE_BUF_SIZE   1024
    char        szBuf[LINE_BUF_SIZE+1];

    // Number of variables in the line
    int         nVar;

};


//---------------------------------------------------------------------------
#endif
