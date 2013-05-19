/* NaPlotIO.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <string.h>

#include "NaPlotIO.h"

//---------------------------------------------------------------------------
// Create a stream or simply read it
NaDPlotFile::NaDPlotFile (const char* fname, NaFileMode fm)
: NaDataFile(fname, fm)
{
    int i;
    for(i = 0; i < NaDPLOT_TITLE_NUM; ++i){
        szTitles[i] = new char[1];
        szTitles[i][0] = '\0';
    }

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
        ReadDPlotFile();

    iCase = -1;
}

//---------------------------------------------------------------------------
// Close (and write) file
NaDPlotFile::~NaDPlotFile ()
{
    if(fmCreateEmpty == eFileMode)
        WriteDPlotFile();
    fclose(fp);
}

//---------------------------------------------------------------------------
// Read file
void
NaDPlotFile::ReadDPlotFile ()
{
    throw(na_not_implemented);
}

//---------------------------------------------------------------------------
// Write file
void
NaDPlotFile::WriteDPlotFile ()
{
    // Magic
    fprintf(fp, "%s\n", NaIO_DPLOT_MAGIC);

    // Data format description, number of series
    fprintf(fp, "dx\n%u\n", aVars.count());

    if(0 == aVars.count()){
        return;
    }

    int iVar;
    for(iVar = 0; iVar < aVars.count(); ++iVar){
        int     nBy5, nRest, iSer;
        nBy5 = CountOfRecord() / 5;
        nRest = CountOfRecord() - 5 * nBy5;

        // Number of values in the series
        fprintf(fp, "%lu\n", CountOfRecord());
        // Values in the series
        for(iSer = 0; iSer < nBy5; ++iSer){
            fprintf(fp, "%g,%g,%g,%g,%g\n", aVars[iVar][5*iSer+0],
                    aVars[iVar][5*iSer+1], aVars[iVar][5*iSer+2],
                    aVars[iVar][5*iSer+3], aVars[iVar][5*iSer+4]);
        }
        if(nRest != 0){
            for(iSer = 0; iSer < nRest - 1; ++iSer){
                fprintf(fp, "%g,", aVars[iVar][5 * nBy5 + iSer]);
            }
            fprintf(fp, "%g\n", aVars[iVar][5 * nBy5 + iSer]);
        }
        // Argument range for the series
        fprintf(fp, "0,%ld\n", CountOfRecord() - 1);
        // Type of curve -- solid line without knots
        fputs(" 1    0\n", fp);
        // Name of the series
        fprintf(fp, "%s\n\n", aVarNames[iVar]);
    }

    // Titles
    fprintf(fp, "%s\n", GetTitle(0));
    fprintf(fp, "%s\n", GetTitle(1));

    fprintf(fp, "\n\n");

    fprintf(fp, "     1\n0.0,0.0\nDate\n           1\nGrid Type\n01\n");

    // For each series
    fprintf(fp, "LineWidths\n");
    for(iVar = 0; iVar < aVars.count(); ++iVar){
        fprintf(fp, " 10");
    }
    fprintf(fp, "\n");

    fprintf(fp, "PointSizes\n 7\n 10 18 14 14 14 11 10\n");

    fprintf(fp, "SymbolSizes\n");
    for(iVar = 0; iVar < aVars.count(); ++iVar){
        if(0 == iVar)
            fprintf(fp, " %3d", 125);
        else
            fprintf(fp, " %3d", 0);
    }
    fprintf(fp, "\n");

    if('\0' != GetTitle(2)[0])
        fprintf(fp, "Title3\n%s\n", GetTitle(2));

    // End of file
    fprintf(fp, "Stop\n");
}


//***********************************
// Per cell operations
//***********************************

//---------------------------------------------------------------------------
// Access to data cell in current record
void    NaDPlotFile::SetValue (NaReal fVal, int iVar)
{
    if(-1 == iCase || iVar >= aVars.count())
        throw(na_out_of_range);

    aVars[iVar][iCase] = fVal;
}

//---------------------------------------------------------------------------
// Access to data cell in current record
NaReal  NaDPlotFile::GetValue (int iVar) const
{
    if(-1 == iCase || iVar >= aVars.count())
        throw(na_out_of_range);

    return aVars(iVar)(iCase);
}


//***********************************
// Per record operations
//***********************************

//---------------------------------------------------------------------------
// Return number of records in the file
long    NaDPlotFile::CountOfRecord ()
{
    if(aVars.count() == 0)
        return 0;
    return aVars[0].dim();
}

//---------------------------------------------------------------------------
// Go to next record
// Return true if the next record exists
bool    NaDPlotFile::GoNextRecord ()
{
    if(0 == aVars.count())
        return false;
    if(iCase >= CountOfRecord())
        return false;

    ++iCase;
    return true;
}

//---------------------------------------------------------------------------
// Go to start record: reset cycle of file reading
// Return true if the next record exists
bool    NaDPlotFile::GoStartRecord ()
{
    iCase = -1;
    return GoNextRecord();
}

//---------------------------------------------------------------------------
// Append one empty record at the end of the file
void    NaDPlotFile::AppendRecord ()
{
    ++iCase;
    ++nRecords;
    int iVar;
    for(iVar = 0; iVar < aVars.count(); ++iVar){
        aVars[iVar].new_dim(nRecords);
    }
}


//***********************************
// Variable operations
//***********************************

//---------------------------------------------------------------------------
// Get list of variables
void    NaDPlotFile::GetVarNameList (int& nVars, char**& sVars)
{
    nVars = aVars.count();

    if(0 == nVars)
        return;

    sVars = new char*[nVars];
    if(NULL == sVars)
        throw(na_bad_alloc);

    int iVar;
    for(iVar = 0; iVar < nVars; ++iVar){
        sVars[iVar] = new char[1 + strlen(aVarNames[iVar])];
        if(NULL == sVars[iVar])
            throw(na_bad_alloc);
        strcpy(sVars[iVar], aVarNames[iVar]);
    }
}

//---------------------------------------------------------------------------
// Set name of the variable
void    NaDPlotFile::SetVarName (int iVar, const char* szVarName)
{
    NaVector    vect(CountOfRecord());
    char        *szName;

    while(iVar >= aVars.count()){
        // Vector of series
        aVars.addh(vect);
        // Name of series VAR#
        aVarNames.addh("");
        szName = new char[1 + 20 + strlen(NaVAR_PREFIX)];
        sprintf(szName, "%s%d", NaVAR_PREFIX, aVars.count());
        aVarNames[aVars.count() - 1] = szName;
    }

    if(NULL != szVarName){
        szName = new char[1 + strlen(szVarName)];
        strcpy(szName, szVarName);
        aVarNames[iVar] = szName;
    }
}

//***********************************
// Common description of the data
//***********************************

//---------------------------------------------------------------------------
// Set title string; many titles are supported
void    NaDPlotFile::SetTitle (const char* szTitle, int iTitleNo)
{
    if(iTitleNo < 0 || iTitleNo >= NaDPLOT_TITLE_NUM)
        // Not so many titles are supported
        return;

    if(NULL == szTitle)
        throw(na_null_pointer);

    delete[] szTitles[iTitleNo];
    szTitles[iTitleNo] = new char[1 + strlen(szTitle)];
    strcpy(szTitles[iTitleNo], szTitle);
}

//---------------------------------------------------------------------------
// Get title string; many titles are supported
const char* NaDPlotFile::GetTitle (int iTitleNo)
{
    if(iTitleNo < 0 || iTitleNo >= NaDPLOT_TITLE_NUM)
        // Not so many titles are supported
        return NULL;
    return szTitles[iTitleNo];
}

//---------------------------------------------------------------------------

