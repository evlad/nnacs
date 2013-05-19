//-*-C++-*-
/* NaStaIO.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaStaIOH
#define NaStaIOH
//---------------------------------------------------------------------------

#include <stadev.h>

#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Class for STATISTICA data file manipulation as a set of variables
class NaStatisticaFile : public NaDataFile
{
public:

    // Create a stream or simply read it
    NaStatisticaFile (const char* fname,
                      NaFileMode fm = fmReadOnly);

    // Close (and write) file
    virtual ~NaStatisticaFile ();

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

    //***********************************
    // Common description of the data
    //***********************************

    // Titles: 0,1,2

    // Set title string; many titles are supported
    virtual void    SetTitle (const char* szTitle, int iTitleNo = 0);

    // Get title string; many titles are supported
    virtual const char* GetTitle (int iTitleNo = 0);

protected:

    // File handle
    HSTAFILE    hFile;

    // Index of the current record - case
    long        iCase;

    // File header
    char        *szTitle;
};


//---------------------------------------------------------------------------
#endif
