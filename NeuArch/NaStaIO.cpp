/* NaStaIO.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <string.h>

#include "NaStaIO.h"


//---------------------------------------------------------------------------
// Create a stream or simply read it
NaStatisticaFile::NaStatisticaFile (const char* fname,
                                    NaFileMode fm)
: NaDataFile(fname, fm), szTitle(NULL)
{
    switch(eFileMode){

    case fmReadOnly:
        hFile = StaOpenFile(szFileName);
        break;
    case fmCreateEmpty:
        hFile = StaCreateFile(1, 1, szFileName);
        break;
    }
    if(0 == hFile)
        throw(na_cant_open_file);
    
    iCase = -1;
}

//---------------------------------------------------------------------------
// Close (and write) file
NaStatisticaFile::~NaStatisticaFile ()
{
    delete[] szTitle;
    StaCloseFile(hFile);
}

//***********************************
// Per cell operations
//***********************************

//---------------------------------------------------------------------------
// Access to data cell in current record
void    NaStatisticaFile::SetValue (NaReal fVal, int iVar)
{
    if(-1 == iCase)
        throw(na_out_of_range);

    HRES    rc = StaSetData(hFile, (short)(iVar + 1), iCase + 1, fVal);
    if(RES_ERR == rc)
        throw(na_write_error);
}

//---------------------------------------------------------------------------
// Access to data cell in current record
NaReal  NaStatisticaFile::GetValue (int iVar) const
{
    if(-1 == iCase)
        throw(na_out_of_range);

    double  fVal = 0.;
    HRES    rc = StaGetData(hFile, (short)(iVar + 1),
                            iCase + 1, (double FAR*)&fVal);
    if(RES_ERR == rc)
        throw(na_read_error);

    return fVal;
}


//***********************************
// Per record operations
//***********************************

//---------------------------------------------------------------------------
// Return number of records in the file
long    NaStatisticaFile::CountOfRecord ()
{
    long    cases = StaGetNCases(hFile);
    if(RES_ERR == cases)
        return 0;
    return cases;
}

//---------------------------------------------------------------------------
// Go to next record
// Return true if the next record exists
bool    NaStatisticaFile::GoNextRecord ()
{
    long    cases = StaGetNCases(hFile);
    if(RES_ERR == cases)
        return false;
    if(iCase + 1 >= cases)
        return false;
    ++iCase;
    return true;
}

//---------------------------------------------------------------------------
// Go to start record: reset cycle of file reading
// Return true if the next record exists
bool    NaStatisticaFile::GoStartRecord ()
{
    long    cases = StaGetNCases(hFile);
    if(cases <= 0){
        iCase = -1;
        return false;
    }
    iCase = 0;
    return true;
}

//---------------------------------------------------------------------------
// Append one empty record at the end of the file
void    NaStatisticaFile::AppendRecord ()
{
    long    cases = StaGetNCases(hFile);
    if(RES_ERR == cases)
        throw(na_read_error);

    if(-1 != iCase){
        HRES    rc = StaAddCases(hFile, cases, 1);
        if(RES_ERR == rc)
            throw(na_write_error);
    }
    iCase = cases - 1;
    ++nRecords;
}


//***********************************
// Variable operations
//***********************************

//---------------------------------------------------------------------------
// Get list of variables
void    NaStatisticaFile::GetVarNameList (int& nVars, char**& sVars)
{
    nVars = StaGetNVars(hFile);
    if(RES_ERR == nVars)
        throw(na_read_error);

    sVars = new char*[nVars];
    if(NULL == sVars)
        throw(na_bad_alloc);

    for(int iVar = 0; iVar < nVars; ++iVar){
        sVars[iVar] = new char[STAMAX_VARNAMELEN];
        if(NULL == sVars[iVar])
            throw(na_bad_alloc);
        HRES    rc = StaGetVarName(hFile, (short)(iVar + 1),
                                   (LPSTR)sVars[iVar]);
        if(RES_ERR == rc)
            throw(na_read_error);
    }
}

//---------------------------------------------------------------------------
// Set name of the variable
void    NaStatisticaFile::SetVarName (int iVar, const char* szVarName)
{
    if(NULL == szVarName)
        throw(na_null_pointer);
    HRES    rc = StaSetVarName(hFile, (short)(iVar + 1),
                               (LPCSTR)szVarName);
    if(RES_ERR == rc)
        throw(na_write_error);
}

//---------------------------------------------------------------------------

//***********************************
// Common description of the data
//***********************************

//---------------------------------------------------------------------------
// Set title string; many titles are supported
void    NaStatisticaFile::SetTitle (const char* szText, int iTitleNo)
{
    if(iTitleNo != 0)
        // Not so many titles are supported
        return;

    if(NULL == szText)
        throw(na_null_pointer);

    delete[] szTitle;
    szTitle = new char[1 + strlen(szText)];
    strcpy(szTitle, szText);

    StaSetFileHeader(hFile, szTitle);
}

//---------------------------------------------------------------------------
// Get title string; many titles are supported
const char* NaStatisticaFile::GetTitle (int iTitleNo)
{
    if(iTitleNo != 0)
        // Not so many titles are supported
        return NULL;

    if(NULL == szTitle){
        szTitle = new char[STAMAX_HEADERLEN+1];
        HRES    rc = StaGetFileHeader(hFile, szTitle, STAMAX_HEADERLEN+1);
        if(RES_ERR == rc){
            delete[] szTitle;
            szTitle = NULL;
        }
    }
    return szTitle;
}

//---------------------------------------------------------------------------

