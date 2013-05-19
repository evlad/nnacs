/* NaBinrIO.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NaBinrIO.h"

/* Limit the value X by the range X1..X9 */
#define NaZone(x,x1,x9)	(x)<(x1)?(x1):((x)>(x9)?(x9):(x))

//---------------------------------------------------------------------------
// Create a stream or simply read it
NaBinaryStreamFile::NaBinaryStreamFile (const char* fname,
					NaFileMode fm,
					NaBinaryDataType bdt,
					int var_num)
  : NaDataFile(fname, fm), vCurLine(NULL), nVar(var_num), eDataType(bdt)
{
  if(0 == nVar && fmCreateEmpty == eFileMode)
    throw(na_bad_value);

  if(fmReadOnly == eFileMode)
    eDataType = bdtAuto;	// Determine later while reading header
  else if(fmCreateEmpty == eFileMode && bdtAuto == eDataType)
    eDataType = bdtReal8;

  nVersion[0] = 1;
  nVersion[1] = 0;

  switch(eFileMode)
    {
    case fmReadOnly:
      fp = fopen(szFileName, "r");
      break;
    case fmCreateEmpty:
      fp = fopen(szFileName, "w");
      break;
    }
  if(NULL == fp)
    throw(na_cant_open_file);

  switch(eFileMode)
    {
    case fmReadOnly:
      try{
	ReadHeader();
	GoStartRecord();
      }catch(...){}
      break;
    case fmCreateEmpty:
      try{
	WriteHeader();
      }catch(...){}
      break;
    }

  iCurLine = -1;
}


//---------------------------------------------------------------------------
// Close (and write) file
NaBinaryStreamFile::~NaBinaryStreamFile ()
{
  if(fmCreateEmpty == eFileMode)
    {
      AppendRecord();
      fflush(fp);
    }
  fclose(fp);
  delete[] (char*)vCurLine;
}


//***********************************
// Per cell operations
//***********************************

//---------------------------------------------------------------------------
// Access to data cell in current record
// Variables are numbered with 0 base
void
NaBinaryStreamFile::SetValue (NaReal fVal, int iVar)
{
  if(-1 == iCurLine || iVar >= nVar || iVar < 0)
    throw(na_out_of_range);

  switch(eDataType)
    {
    case bdtInteger1:
      ((char*)vCurLine)[iVar] = NaZone((int)fVal, -128, 127);
      break;
    case bdtInteger2:
      ((short*)vCurLine)[iVar] = NaZone((int)fVal, -32768, 32767);
      break;
    case bdtInteger4:
      ((long*)vCurLine)[iVar] = NaZone((long)fVal, -2147483647-1, 2147483647);
      break;
    case bdtReal4:	((float*)vCurLine)[iVar] = (float)fVal;	break;
    case bdtReal8:	((double*)vCurLine)[iVar] = (double)fVal;break;
    }
}


//---------------------------------------------------------------------------
// Access to data cell in current record
// Variables are numbered with 0 base
NaReal
NaBinaryStreamFile::GetValue (int iVar) const
{
  if(-1 == iCurLine || iVar >= nVar || iVar < 0)
    throw(na_out_of_range);

  NaReal	v = 0.0;

  switch(eDataType)
    {
    case bdtInteger1:	v = (int)((char*)vCurLine)[iVar];break;
    case bdtInteger2:	v = ((short*)vCurLine)[iVar];	break;
    case bdtInteger4:	v = ((long*)vCurLine)[iVar];	break;
    case bdtReal4:	v = ((float*)vCurLine)[iVar];	break;
    case bdtReal8:	v = ((double*)vCurLine)[iVar];	break;
    }

  return v;
}


//***********************************
// Per record operations
//***********************************

//---------------------------------------------------------------------------
// Go to next record
// Return true if the next record exists
bool
NaBinaryStreamFile::GoNextRecord ()
{
  if(nVar != fread(vCurLine, nDataSize, nVar, fp))
    {
      if(feof(fp))
	return false;
      throw(na_read_error);
    }
  ++iCurLine;

  return true;
}


//---------------------------------------------------------------------------
// Go to start record: reset cycle of file reading
// Return true if the next record exists
bool
NaBinaryStreamFile::GoStartRecord ()
{
  if(0 != fseek(fp, oBinStream, SEEK_SET))
    throw(na_read_error);
  iCurLine = -1;

  return GoNextRecord();
}


//---------------------------------------------------------------------------
// Append one empty record at the end of the file
void
NaBinaryStreamFile::AppendRecord ()
{
  if(0 != fseek(fp, 0, SEEK_END))
    throw(na_write_error);

  if(-1 != iCurLine)
    fwrite(vCurLine, nDataSize, nVar, fp);

  ++iCurLine;
  ++nRecords;
}


//---------------------------------------------------------------------------
// Return number of records in the file
long
NaBinaryStreamFile::CountOfRecord ()
{
  long    n, pos;

  n = 0;
  pos = ftell(fp);

  /* goto end of file */
  fseek(fp, 0, SEEK_END);
  n = (ftell(fp) - oBinStream) / (nDataSize * nVar);

  /* return to old position */
  fseek(fp, pos, SEEK_SET);

  return n;
}


//***********************************
// Variable operations
//***********************************

//---------------------------------------------------------------------------
// Get list of variables
void
NaBinaryStreamFile::GetVarNameList (int& nVars, char**& sVars)
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
// Variables are numbered with 0 base
void
NaBinaryStreamFile::SetVarName (int iVar, const char* szVarName)
{
    if(NULL == szVarName)
        throw(na_null_pointer);

    // Dummy - not implemented
}


//---------------------------------------------------------------------------
// Read file header and determine important parameters
void
NaBinaryStreamFile::ReadHeader ()
{
  char	buf[1024];

  /* [BinaryDataHeader]\n */
  if(NULL == fgets(buf, 1024, fp))
    goto BadHeaderSyntax;
  else if(strcmp(buf, NaIO_BINARY_HEADER))
    goto BadHeaderSyntax;

  /* Version= */
  if(NULL == fgets(buf, 1024, fp))
    goto BadHeaderSyntax;
  else if(strncmp(buf, NaIO_BIN_HDR_VERSION_KW,
		  strlen(NaIO_BIN_HDR_VERSION_KW)))
    goto BadHeaderSyntax;
  else
    sscanf(buf + strlen(NaIO_BIN_HDR_VERSION_KW), "%d.%d",
	   nVersion + 0, nVersion + 1);

  /* VarNum= */
  if(NULL == fgets(buf, 1024, fp))
    goto BadHeaderSyntax;
  else if(strncmp(buf, NaIO_BIN_HDR_VARNUM_KW,
		  strlen(NaIO_BIN_HDR_VARNUM_KW)))
    goto BadHeaderSyntax;
  else
    sscanf(buf + strlen(NaIO_BIN_HDR_VARNUM_KW), "%d", &nVar);

  /* VarFmt= */
  if(NULL == fgets(buf, 1024, fp))
    goto BadHeaderSyntax;
  else if(strncmp(buf, NaIO_BIN_HDR_VARFMT_KW,
		  strlen(NaIO_BIN_HDR_VARFMT_KW)))
    goto BadHeaderSyntax;
#define FIND_FORMAT(x)	!strncmp(buf + strlen(NaIO_BIN_HDR_VARFMT_KW), \
				x, strlen(x))
  else if(FIND_FORMAT(NaVARFMT_I1))
    eDataType = bdtInteger1;
  else if(FIND_FORMAT(NaVARFMT_I2))
    eDataType = bdtInteger2;
  else if(FIND_FORMAT(NaVARFMT_I4))
    eDataType = bdtInteger4;
  else if(FIND_FORMAT(NaVARFMT_R4))
    eDataType = bdtReal4;
  else if(FIND_FORMAT(NaVARFMT_R8))
    eDataType = bdtReal8;
#undef FIND_FORMAT
  else
    goto BadHeaderSyntax;

  /* [BinaryDataStream]\n */
  if(NULL == fgets(buf, 1024, fp))
    goto BadHeaderSyntax;
  else if(strcmp(buf, NaIO_BINARY_STREAM))
    goto BadHeaderSyntax;
  else
    oBinStream = ftell(fp);

  /* compute size of data element */
  if(eDataType == bdtInteger1)
    nDataSize = 1;
  else if(eDataType == bdtInteger2)
    nDataSize = 2;
  else if(eDataType == bdtInteger4 || eDataType == bdtReal4)
    nDataSize = 4;
  else if(eDataType == bdtReal8)
    nDataSize = 8;

  /* allocate buffer for a line */
  vCurLine = new char[nDataSize * nVar];

  return;

 BadHeaderSyntax:
  fclose(fp);
  fp = NULL;
  throw(na_read_error);
}


//---------------------------------------------------------------------------
// Write file header
void
NaBinaryStreamFile::WriteHeader ()
{
  fputs(NaIO_BINARY_HEADER, fp);
  fprintf(fp, "%s%d.%d\n",
	  NaIO_BIN_HDR_VERSION_KW, nVersion[0], nVersion[1]);
  fprintf(fp, "%s%d\n",
	  NaIO_BIN_HDR_VARNUM_KW, nVar);

  const char	*szFmt = "??";
  switch(eDataType)
    {
    case bdtInteger1:	szFmt = NaVARFMT_I1;	break;
    case bdtInteger2:	szFmt = NaVARFMT_I2;	break;
    case bdtInteger4:	szFmt = NaVARFMT_I4;	break;
    case bdtReal4:	szFmt = NaVARFMT_R4;	break;
    case bdtReal8:	szFmt = NaVARFMT_R8;	break;
    }
  fprintf(fp, "%s%s\n",
	  NaIO_BIN_HDR_VARFMT_KW, szFmt);

  fputs(NaIO_BINARY_STREAM, fp);
  oBinStream = ftell(fp);

  /* compute size of data element */
  if(eDataType == bdtInteger1)
    nDataSize = 1;
  else if(eDataType == bdtInteger2)
    nDataSize = 2;
  else if(eDataType == bdtInteger4 || eDataType == bdtReal4)
    nDataSize = 4;
  else if(eDataType == bdtReal8)
    nDataSize = 8;

  /* allocate buffer for a line */
  vCurLine = new char[nDataSize * nVar];
}
