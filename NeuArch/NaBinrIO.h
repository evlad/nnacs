//-*-C++-*-
/* NaBinrIO.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaBinrIOH
#define NaBinrIOH
//---------------------------------------------------------------------------

#include <stdio.h>
#include <NaDataIO.h>

/* Structure of the file:

   [BinaryDataHeader]
   Version=1.0
   VarNum=###
   VarFmt=??
   [BinaryDataStream]
   binary data with little-endian byte order
*/

// Macro
#define NaIO_BINARY_HEADER	NaIO_BINARY_MAGIC
#define NaIO_BINARY_STREAM	"[BinaryDataStream]\n"
#define NaIO_BIN_HDR_VERSION_KW	"Version="
#define NaIO_BIN_HDR_VARNUM_KW	"VarNum="
#define NaIO_BIN_HDR_VARFMT_KW	"VarFmt="

//---------------------------------------------------------------------------
// Class for simple binary data file manipulation
class NaBinaryStreamFile : public NaDataFile
{
public:

    // Create a stream or simply read it
    NaBinaryStreamFile (const char* fname,
			NaFileMode fm = fmReadOnly,
			NaBinaryDataType bdt = bdtAuto,
			int var_num = 0);

    // Close (and write) file
    virtual ~NaBinaryStreamFile ();

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
    FILE	*fp;

    // Index of the current record or EOF if so
    long	iCurLine;

    // Preread current line of values [nVar]
    void	*vCurLine;

    // Number of variables in the line
    int		nVar;

    // Actual data type
    NaBinaryDataType	eDataType;
    size_t	nDataSize;

    // Offset to the 1st byte of binary data stream
    long	oBinStream;

    // Version (major.minor)
    int		nVersion[2];

    // Read/write file header
    void	ReadHeader ();
    void	WriteHeader ();

};


//---------------------------------------------------------------------------
#endif
