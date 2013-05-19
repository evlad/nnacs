//-*-C++-*-
/* NaPlotIO.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPlotIOH
#define NaPlotIOH
//---------------------------------------------------------------------------
// Example of the *.grf file:

/*
DPLOT/W v1.2
dx
3  -- number of series
6  -- number points in the 1st series
0,0.116994,0.0196264,0.0843889,0.151711  -- not more than 5 values per row
-0.140386
0,500  -- argument for values of the 1st series
 1    0  -- solid line without knots
Вход объекта  -- name of the 1st series

6  -- number points in the 2nd series
0,1.72694E-005,5.16658E-005,0.00011247,0.000217104
0.00395331
0,500  -- argument for values of the 2nd series
 1    0  -- solid line without knots
Выход объекта  -- name of the 2nd series

5  -- number points in the 3rd series
1.72694E-005,5.16658E-005,0.00011247,0.0198901,0.0202716
0,499  -- argument for values of the 3rd series
 1    1  -- solid line with knots
Выход ИНС  -- name of the 3rd series

Модель предсказания объекта №1  -- Title #1
NN{6,5,3,1} in=x(i), x(i-1), x(i-2), y(i), y(i-1), y(i-2)  -- Title #2


     1
0.8004896,-3.625
Date  -- print date
           1
Grid Type
01
LineWidths
 10 10 10  -- for each series
Multiple Axes
 2  -- two different vertical axis/scales
 1 2 2  -- 1 axis - 1st serie; 2 axis - 2nd & 3rd series
PointSizes
 7  -- number of next values
 10 18 14 14 14 11 10  -- values ?
SymbolSizes
 125   0   0  -- for each series
Title3
Полная выборка.  Дата: 26.10.99  -- Title #3
Stop
*/

//---------------------------------------------------------------------------

#include <stdio.h>
#include <NaDataIO.h>
#include <NaVector.h>
#include <NaDynAr.h>

// Default name of the only variable
#define NaVAR_PREFIX    "VAR"

// Supported number of titles
#define NaDPLOT_TITLE_NUM   3


//---------------------------------------------------------------------------
// Class for DPlot drawing data file writing (many variables are allowed)
// Stream of the only variable values
class NaDPlotFile : public NaDataFile
{
public:

    // Create a stream (read mode is not allowed)
    NaDPlotFile (const char* fname,
                 NaFileMode fm = fmReadOnly);

    // Close (and write) file
    virtual ~NaDPlotFile ();

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

    // Read file
    void        ReadDPlotFile ();

    // Write file
    void        WriteDPlotFile ();

    // File handle
    FILE        *fp;

    // Index of the current record or EOF if so
    long        iCase;

    // Variables in the file
    NaDynAr<NaVector>   aVars;      // series themselves
    NaDynAr<char*>      aVarNames;  // their legend

    // Titles
    char        *szTitles[NaDPLOT_TITLE_NUM];

};


//---------------------------------------------------------------------------
#endif
 
