/* NaTextIO.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NaTextIO.h"

#define IOBUF_SIZE	102400

//---------------------------------------------------------------------------
// Create a stream or simply read it
NaTextStreamFile::NaTextStreamFile (const char* fname,
                                    NaFileMode fm)
  : NaDataFile(fname, fm), fCurVal(NULL), nVar(0)
{
    switch(eFileMode){

    case fmReadOnly:
        fp = fopen(szFileName, "rb"); // rt
        break;
    case fmCreateEmpty:
        fp = fopen(szFileName, "wb"); // wt
        break;
    }

    if(NULL == fp)
        throw(na_cant_open_file);

    if(fmReadOnly == eFileMode)
      {
        try{
            GoStartRecord();
        }catch(...){}
      }

    setvbuf(fp, NULL, _IOFBF, IOBUF_SIZE);

    iCase = -1;
}

//---------------------------------------------------------------------------
// Close (and write) file
NaTextStreamFile::~NaTextStreamFile ()
{
  if(fmCreateEmpty == eFileMode){
    AppendRecord();
    fflush(fp);
  }
  fclose(fp);
  delete[] fCurVal;
}


//***********************************
// Per cell operations
//***********************************

//---------------------------------------------------------------------------
// Access to data cell in current record
void    NaTextStreamFile::SetValue (NaReal fVal, int iVar)
{
    if(-1 == iCase)
        throw(na_out_of_range);

    if(0 == nVar){
        // Number of values in record is not defined yet
        nVar = iVar + 1;
        fCurVal = new NaReal[nVar];
        int    i;
        for(i = 0; i < nVar; ++i){
            if(i == iVar){
                fCurVal[i] = fVal;
            }else{
                fCurVal[i] = 0.;
            }
        }
    }else if(iVar < nVar){
        // Setting variable among known ones
        fCurVal[iVar] = fVal;
    }else if(iVar >= nVar){
        // Setting variable beyond known ones
        NaReal      *pOld = fCurVal;
        int    nOld = nVar;

        nVar = iVar + 1;
        fCurVal = new NaReal[nVar];
        int    i;
        for(i = 0; i < nVar; ++i){
            if(i < nOld){
                fCurVal[i] = pOld[i];
            }else if(i == iVar){
                fCurVal[i] = fVal;
            }else{
                fCurVal[i] = 0.;
            }
        }
        delete[] pOld;
    }
}

//---------------------------------------------------------------------------
// Access to data cell in current record
NaReal  NaTextStreamFile::GetValue (int iVar) const
{
    if(-1 == iCase || iVar >= nVar || iVar < 0)
        throw(na_out_of_range);

    return fCurVal[iVar];
}


//***********************************
// Per record operations
//***********************************

//---------------------------------------------------------------------------
// Return number of records in the file
long    NaTextStreamFile::CountOfRecord ()
{
    long    n, pos;
    char    buf[LINE_BUF_SIZE+1];

    n = 0;
    pos = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    while(NULL != fgets(buf, LINE_BUF_SIZE, fp))
        ++n;
    fseek(fp, pos, SEEK_SET);

    return n;
}

//---------------------------------------------------------------------------
// Go to next record
// Return true if the next record exists
bool    NaTextStreamFile::GoNextRecord ()
{
    if(NULL == fgets(szBuf, LINE_BUF_SIZE + 1, fp)){
        if(feof(fp))
            return false;
        throw(na_read_error);
    }
    ++iCase;

    // Scan line
    char   *s = szBuf, *p = NULL;
    int    iVar = 0;
    while(true){
        NaReal  fVal = strtod(s, &p);
        if(p == s)
            break;
        s = p;
        SetValue(fVal, iVar++);
    }
    if(iVar < nVar){
        while(iVar < nVar){
            // Fill by zeros
            fCurVal[iVar++] = 0.;
        }
        // Read from line less values than expected
        throw(na_read_error);
    }

    return true;
}

//---------------------------------------------------------------------------
// Go to start record: reset cycle of file reading
// Return true if the next record exists
bool    NaTextStreamFile::GoStartRecord ()
{
    if(0 != fseek(fp, 0, SEEK_SET))
        throw(na_read_error);
    iCase = -1;
    return GoNextRecord();
}

//---------------------------------------------------------------------------
// Append one empty record at the end of the file
void    NaTextStreamFile::AppendRecord ()
{
    if(0 != fseek(fp, 0, SEEK_END))
        throw(na_write_error);

    if(-1 != iCase){
        // Append previous
        int    i;
        for(i = 0; i < nVar; ++i){
            fprintf(fp, " %g", fCurVal[i]);
        }
        fputc('\n', fp);
    }
    ++iCase;
    ++nRecords;
}


//***********************************
// Variable operations
//***********************************

//---------------------------------------------------------------------------
// Get list of variables
void    NaTextStreamFile::GetVarNameList (int& nVars, char**& sVars)
{
    nVars = nVar;

    sVars = new char*[nVars];
    if(NULL == sVars)
        throw(na_bad_alloc);

    int    i;
    for(i = 0; i < nVar; ++i){
        sVars[i] = new char[10 + strlen(NaVAR_NAME)];
        if(NULL == sVars[0])
            throw(na_bad_alloc);
        sprintf(sVars[i], "%s_%d", NaVAR_NAME, i + 1);
    }
}

//---------------------------------------------------------------------------
// Set name of the variable
void    NaTextStreamFile::SetVarName (int iVar, const char* szVarName)
{
    if(NULL == szVarName)
        throw(na_null_pointer);

    // Dummy - not implemented
}

//---------------------------------------------------------------------------
