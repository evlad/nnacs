/* NaConfig.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* !_GNU_SOURCE */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//#include <unistd.h>
#include <ctype.h>

//#define CONFIG_DEBUG
#include "NaLogFil.h"
#include "NaConfig.h"

//===========================================================================
// Class NaDataStream
// Stream of data object
//===========================================================================

//---------------------------------------------------------------------------
NaDataStream::NaDataStream (const unsigned pVersion[])
: szFormatBuf(NULL), pVer(pVersion)
{
    // Prepare buffer
    FormatBuffer();

    if(NULL == pVersion){
        throw(na_null_pointer);
    }
}


//---------------------------------------------------------------------------
NaDataStream::~NaDataStream ()
{
    delete[] szFormatBuf;
}


//---------------------------------------------------------------------------
// Version info
unsigned
NaDataStream::GetVersion (int id) const
{
    if(NaMajorVerNo == id || NaMinorVerNo == id){
        return pVer[id];
    }
    return 0;
}


//***********************************************************************
// Derived operations
//***********************************************************************

//---------------------------------------------------------------------------
// Put comment (or empty line)
void
NaDataStream::PutComment (const char* szComment)
{
    PutData(NULL, szComment);
}

//---------------------------------------------------------------------------
// Put formatted string with comment (see *printf)
void
NaDataStream::PutF (const char* szComment, const char* szFormat, ...)
{
    if(NULL == szFormatBuf)
        throw(na_bad_value);

    va_list argptr;

    va_start(argptr, szFormat);
    vsprintf(szFormatBuf, szFormat, argptr);
    PutData(szFormatBuf, szComment);

    va_end(argptr);
}

//---------------------------------------------------------------------------
// Reserve buffer for formatted data (in bytes)
void
NaDataStream::FormatBuffer (unsigned nSize)
{
    delete[] szFormatBuf;
    szFormatBuf = new char[nSize];
}

//---------------------------------------------------------------------------
// Get formatted string (see *scanf)
void
NaDataStream::GetF (const char* szFormat, ...)
{
    va_list argptr;

    va_start(argptr, szFormat);
    vsscanf(GetData(), szFormat, argptr);

    va_end(argptr);
}


//===========================================================================
// Class NaConfigPart
// Configuration file partition object.  Abstract class.
// Has generic name and instance name.
//===========================================================================

//---------------------------------------------------------------------------
// Initialize configuration partition with type name - one word
// without spaces, punctuation and special marks
NaConfigPart::NaConfigPart (const char* szType)
{
    if(NULL == szType)
        throw(na_null_pointer);
    szTypeName = new char[strlen(szType) + 1];
    strcpy(szTypeName, szType);

    szInstanceName = NULL;

    pSelfData = NULL;
}

//---------------------------------------------------------------------------
// Initialize configuration partition as a copy of another
NaConfigPart::NaConfigPart (const NaConfigPart& rCP)
{
    const char  *szType = rCP.GetType();
    const char  *szInstance = rCP.GetInstance();

    szTypeName = new char[strlen(szType) + 1];
    strcpy(szTypeName, szType);

    szInstanceName = NULL;
    SetInstance(szInstance);

    pSelfData = NULL;
}

//---------------------------------------------------------------------------
// Destroy local data of the configuation partition
NaConfigPart::~NaConfigPart ()
{
    delete[] szTypeName;
    delete[] szInstanceName;
}

//---------------------------------------------------------------------------
// Assign name to instance (NULL by default) - one word
// without spaces, punctuation and special marks
void
NaConfigPart::SetInstance (const char* szInstance)
{
    if(NULL == szInstance){
        delete[] szInstanceName;
        szInstanceName = NULL;
    }
    else{
        szInstanceName = new char[strlen(szInstance) + 1];
        strcpy(szInstanceName, szInstance);
    }
}

//---------------------------------------------------------------------------
// Get name of the type
const char*
NaConfigPart::GetType () const
{
    return szTypeName;
}

//---------------------------------------------------------------------------
// Get name of the instance (NULL by default)
const char*
NaConfigPart::GetInstance () const
{
    return szInstanceName;
}


//===========================================================================
// Class NaConfigFile
// Configuration file object.
// Has common methdos to read and write text configuration.  Consists of
// many partitions.
//===========================================================================

//---------------------------------------------------------------------------
// Common identification: magic string at the beginning of the file
// and default file name extension.
// Plus space-separated marks for start of partition title, end of it
// and for comment starting.
NaConfigFile::NaConfigFile (const char* szMagic,
                            unsigned nMajorVerNo,
                            unsigned nMinorVerNo,
                            const char* szFileExt,
                            const char* szMarks)
: NaDataStream(nFileVer), nPartList(0), pPartList(NULL), fp(NULL)
{
    if(NULL == szMagic || NULL == szFileExt || NULL == szMarks)
        throw(na_null_pointer);

    szMagicString = new char[strlen(szMagic) + 1];
    strcpy(szMagicString, szMagic);

    szFileExtString = new char[strlen(szFileExt) + 1];
    strcpy(szFileExtString, szFileExt);

    szMarksBuf = new char[strlen(szMarks) + 1];
    strcpy(szMarksBuf, szMarks);

    szTitleStartMark = szMarksBuf;
    szTitleEndMark = 1 + strchr(szTitleStartMark, ' ');
    szCommentMark = 1 + strchr(szTitleEndMark, ' ');

    szTitleEndMark[-1] = '\0';
    szCommentMark[-1] = '\0';

    nFileVer[NaMajorVerNo] = nVer[NaMajorVerNo] = nMajorVerNo;
    nFileVer[NaMinorVerNo] = nVer[NaMinorVerNo] = nMinorVerNo;
}

//---------------------------------------------------------------------------
NaConfigFile::~NaConfigFile ()
{
    delete[] szMagicString;
    delete[] szFileExtString;
    delete[] szMarksBuf;
}


//***********************************************************************
// Self-identification
//***********************************************************************

//---------------------------------------------------------------------------
// Self-identification by magic
const char*
NaConfigFile::Magic () const
{
    return szMagicString;
}

//---------------------------------------------------------------------------
// Self-identification by file extension
const char*
NaConfigFile::FileExt () const
{
    return szFileExtString;
}

//---------------------------------------------------------------------------
// Version info
unsigned
NaConfigFile::Version (int id) const
{
    if(NaMajorVerNo == id || NaMinorVerNo == id){
        return nVer[id];
    }
    return 0;
}


//***********************************************************************
// Used marks
//***********************************************************************

//---------------------------------------------------------------------------
// Title start mark string used in file
const char*
NaConfigFile::TitleStartMark () const
{
    return szTitleStartMark;
}

//---------------------------------------------------------------------------
// Title end mark string used in file
const char*
NaConfigFile::TitleEndMark () const
{
    return szTitleEndMark;
}

//---------------------------------------------------------------------------
// Comment mark string used in file
const char*
NaConfigFile::CommentMark () const
{
    return szCommentMark;
}


//***********************************************************************
// Registrar for dynamic partition attachment
//***********************************************************************

//---------------------------------------------------------------------------
// Add registrator for given partition type (create it dynamically)
void
NaConfigFile::AddRegistrar (const char* szPartType, NaCPRegistrar pRegFunc)
{
  NaCPRegItem	it;

  if(NULL == szPartType || NULL == pRegFunc)
    throw(na_null_pointer);

  if(NULL != FindRegistrar(szPartType))
    /* already exist */
    return;

  it.pRegFunc = pRegFunc;
  it.szTypeName = new char[strlen(szPartType) + 1];
  strcpy(it.szTypeName, szPartType);

  arReg.addh(it);
}


//---------------------------------------------------------------------------
// Find registrar for given partition type (create it dynamically)
NaCPRegistrar
NaConfigFile::FindRegistrar (const char* szPartType)
{
  NaCPRegistrar	pFunc = NULL;
  int		i;

  if(NULL == szPartType)
    throw(na_null_pointer);

  for(i = 0; i < arReg.count(); ++i){
    if(!strcmp(arReg[i].szTypeName, szPartType)){
      pFunc = arReg[i].pRegFunc;
      break;
    }
  }

  return pFunc;
}


//***********************************************************************
// Partition attachment/detachment operations
//***********************************************************************

//---------------------------------------------------------------------------
// Add partitions (created externally)
void
NaConfigFile::AddPartitions (unsigned nParts, NaConfigPart* pParts[])
{
    NaConfigPart    **pNewParts;

    if(0 == nParts)
        return;
    if(NULL == pParts)
        throw(na_null_pointer);

    pNewParts = new NaConfigPart*[nPartList + nParts];
    if(0 != nPartList){
        memcpy(pNewParts, pPartList, sizeof(NaConfigPart*) * nPartList);
        delete[] pPartList;
    }
    memcpy(pNewParts + nPartList, pParts, sizeof(NaConfigPart*) * nParts);

    nPartList += nParts;
    pPartList = pNewParts;
}

//---------------------------------------------------------------------------
// Clear list of partitions
void
NaConfigFile::RemovePartitions ()
{
    if(nPartList != 0){
        nPartList = 0;
        delete[] pPartList;
    }
}


//***********************************************************************
// The whole file operations
//***********************************************************************

//---------------------------------------------------------------------------
// Save partitions' data with overwriting existant file
void
NaConfigFile::SaveToFile (const char* szFilePath)
{
    if(NULL == szFilePath)
        throw(na_null_pointer);

    fp = fopen(szFilePath, "wt");
    if(NULL == fp)
        throw(na_cant_open_file);

    NaPrintLog("Open file '%s' for saving configuration data\n", szFilePath);

    PutString(Magic());
    PutString(" ");

    char    szVersion[40];
    sprintf(szVersion, "%u.%u", Version(NaMajorVerNo), Version(NaMinorVerNo));
    PutString(szVersion);

    PutString("\n");

    for(unsigned iPart = 0; iPart < nPartList; ++iPart){
        if(NULL != pPartList[iPart]){
            try{
                const char  *szInstance = (pPartList[iPart]->GetInstance())
                    ? pPartList[iPart]->GetInstance(): "is undefined";

                NaPrintLog("Saving %s (instance %s)\n",
                           pPartList[iPart]->GetType(), szInstance);

                PutString(TitleStartMark());
                PutString(pPartList[iPart]->GetType());
                if(NULL != pPartList[iPart]->GetInstance()){
                    PutString(" ");
                    PutString(pPartList[iPart]->GetInstance());
                }
                PutString(TitleEndMark());
                PutString("\n");

                pPartList[iPart]->Save(*this);
            }
            catch(NaException exCode){
                NaPrintLog("Failed while saving: %s\n",
                           NaExceptionMsg(exCode));
            }
        }
    }/* for each instance */

    NaPrintLog("Close file '%s'\n", szFilePath);
    fclose(fp);
}

//---------------------------------------------------------------------------
// Load partitions' data from file
void
NaConfigFile::LoadFromFile (const char* szFilePath)
{
    if(NULL == szFilePath)
        throw(na_null_pointer);

    fp = fopen(szFilePath, "rb");
    if(NULL == fp)
        throw(na_cant_open_file);

    NaPrintLog("Open file '%s' for loading configuration data\n", szFilePath);
    bUndo = false;

    // Load magic line
    if(NULL == fgets(szLineBuf, MaxConfigFileLine, fp))
        goto CloseFile;
    if(0 != strncmp(szLineBuf, Magic(), strlen(Magic()))){
        NaPrintLog("Magic does not match.\n");
        goto CloseFile;
    }

    // Parse version of file
    sscanf(szLineBuf + strlen(Magic()), "%u.%u",
           nFileVer + NaMajorVerNo, nFileVer + NaMinorVerNo);

    if(nVer[NaMajorVerNo] == nFileVer[NaMajorVerNo] &&
       nVer[NaMinorVerNo] > nFileVer[NaMinorVerNo] ||
       nVer[NaMajorVerNo] > nFileVer[NaMajorVerNo]){
        // Old version file
        NaPrintLog("Old file format version %u.%u is detected: ok.\n",
                   nFileVer[NaMajorVerNo], nFileVer[NaMinorVerNo]);
    }else if(nVer[NaMajorVerNo] == nFileVer[NaMajorVerNo] &&
             nVer[NaMinorVerNo] < nFileVer[NaMinorVerNo] ||
             nVer[NaMajorVerNo] < nFileVer[NaMajorVerNo]){
        // New version file
        NaPrintLog("Future file format version %u.%u is detected: failed.\n",
                   nFileVer[NaMajorVerNo], nFileVer[NaMinorVerNo]);
        goto CloseFile;
    } else {
	NaPrintLog("File format version %u.%u is detected: ok.\n",
                   nFileVer[NaMajorVerNo], nFileVer[NaMinorVerNo]);
    }

    NaConfigLineKind    eLineKind;

    // Trying to load partitions
    while(clkEOF != (eLineKind = GetLine())){

        if(clkPartitionTitle != eLineKind)
            continue;

        // Partition title is found
        char    *szType, *szInstance;

#ifdef CONFIG_DEBUG
        NaPrintLog("==> %s\n", szLineBuf);
#endif /* CONFIG_DEBUG */
        ParseTitle(szLineBuf, szType, szInstance);
#ifdef CONFIG_DEBUG
        NaPrintLog("Partition %s is found in file (instance %s).\n",
                   szType, szInstance? szInstance: "isn't defined");
#endif /* CONFIG_DEBUG */

	NaCPRegistrar	pRegFunc = FindRegistrar(szType);

	// Check for partition type registrar
	if(NULL != pRegFunc){
	  // Create partition dynamically...
	  NaConfigPart	*cp = (*pRegFunc)();

	  // ...set instance...
	  if(NULL != szInstance)
	    cp->SetInstance(szInstance);

	  // ...and add it to list
	  AddPartitions(1, &cp);

#ifdef CONFIG_DEBUG
	  NaPrintLog("Partition %s (instance %s) is registered "\
		     "in list dynamically.\n",
		     szType, szInstance? szInstance: "isn't defined");
#endif /* CONFIG_DEBUG */
	}

        // Store file position
        long    iPos = ftell(fp);

        // Find for the partitions of type found
        for(unsigned iPart = 0; iPart < nPartList; ++iPart){
            if(NULL != pPartList[iPart]){
#ifdef CONFIG_DEBUG
                NaPrintLog("#%d: Type=%s, Instance=%s --> ", iPart,
                           pPartList[iPart]->GetType(),
                           NULL == pPartList[iPart]->GetInstance()
                           ?"(nil)":pPartList[iPart]->GetInstance());
#endif /* CONFIG_DEBUG */
                if(strcmp(szType, pPartList[iPart]->GetType())){
                    // Different types
#ifdef CONFIG_DEBUG
                    NaPrintLog("reject due to different types\n");
                    NaPrintLog("\"%s\" != \"%s\"\n",
			       szType, pPartList[iPart]->GetType());
#endif /* CONFIG_DEBUG */
                    continue;
                }
                if(NULL != szInstance &&
                   NULL != pPartList[iPart]->GetInstance()){
                    //if(NULL == pPartList[iPart]->GetInstance()){
                    //    NaPrintLog("reject due to undefined instance\n");
                    //    continue;
                    //}
                    if(strcmp(szInstance, pPartList[iPart]->GetInstance())){
                        // Different instances
#ifdef CONFIG_DEBUG
                        NaPrintLog("reject due to different instance\n");
#endif /* CONFIG_DEBUG */
                        continue;
                    }
                }
                else if(NULL == szInstance &&
                        NULL != pPartList[iPart]->GetInstance()){
#ifdef CONFIG_DEBUG
                    NaPrintLog("reject due to different instance\n");
#endif /* CONFIG_DEBUG */
                    continue;
                }

#ifdef CONFIG_DEBUG
                NaPrintLog("accept\n");
#endif /* CONFIG_DEBUG */

                // Restore file position
                fseek(fp, iPos, SEEK_SET);

		// Assign partition instance
		if(NULL != szInstance)
		  pPartList[iPart]->SetInstance(szInstance);

#ifdef CONFIG_DEBUG
                NaPrintLog("Loading configuration for instance %s.\n",
                           (NULL == szInstance)? "(nil)": szInstance);
#endif /* CONFIG_DEBUG */
                try{
		    pPartList[iPart]->Load(*this);
                }
                catch(NaException exCode){
                    NaPrintLog("Failed while loading: %s\n",
                               NaExceptionMsg(exCode));
                }
            }
        }/* for each instance */
    }/* until end of file */

CloseFile:
#ifdef CONFIG_DEBUG
    NaPrintLog("Close file '%s'\n", szFilePath);
#endif /* CONFIG_DEBUG */
    fclose(fp);

    /* restore own version */
    nFileVer[NaMajorVerNo] = nVer[NaMajorVerNo];
    nFileVer[NaMinorVerNo] = nVer[NaMinorVerNo];
}


//***********************************************************************
// Inside file operations while saving
//***********************************************************************

//---------------------------------------------------------------------------
// Put string to file
void
NaConfigFile::PutString (const char* szBuf)
{
    if(NULL == fp || NULL == szBuf)
        return;

    fputs(szBuf, fp);
}

//---------------------------------------------------------------------------
// Put title of the partition
// PartType and PartInstance must be words without spaces!
void
NaConfigFile::PutPartTitle (const char* szPartType,
                            const char* szPartInstance)
{
    if(NULL == fp)
        return;
    if(NULL == szPartType)
        throw(na_null_pointer);

    PutString(TitleStartMark());
    PutString(szPartType);
    if(NULL != szPartInstance){
        PutString(" ");
        PutString(szPartInstance);
    }
    PutString(TitleEndMark());
    PutString("\n");
}

//---------------------------------------------------------------------------
// Put string with ending comment
void
NaConfigFile::PutData (const char* szData, const char* szComment)
{
    if(NULL == fp)
        return;
    if(NULL != szData){
        PutString(szData);
    }
    if(NULL != szData && NULL != szComment){
        PutString("\t ");
    }
    if(NULL != szComment){
        PutString(CommentMark());
        PutString(" ");
        PutString(szComment);
    }
    PutString("\n");
}


//***********************************************************************
// Inside file operations while loading
//***********************************************************************

//---------------------------------------------------------------------------
// Undo the whole line reading from file (only one step of undo is provided)
void
NaConfigFile::UndoLine ()
{
  if(bUndo){
    // Only one step of undo can be performed
    throw(na_not_implemented);
  }else{
    bUndo = true;
  }
}


//---------------------------------------------------------------------------
// Get the whole line from file and returns type of the line
NaConfigLineKind
NaConfigFile::GetLine ()
{
    NaConfigLineKind    eLineKind;

    if(false == bUndo){
      // it's needed to read line from file
      char    szBuf[MaxConfigFileLine];

      if(NULL == fgets(szBuf, MaxConfigFileLine, fp)){
        /* error or end of file */
        eLineKind = clkEOF;
        szLineBuf[0] = '\0';

	return eLineKind;
      }
      else{
        if(szBuf[strlen(szBuf) - 1] == '\n'){
            szBuf[strlen(szBuf) - 1] = '\0';
        }
        else if(strlen(szBuf) >= MaxConfigFileLine - 1){
	  /* possibly only head of line was read from file... */
	  char    szHead[25];
	  strncpy(szHead, szBuf, 24);
	  NaPrintLog("Very long line '%s...' encountered!  Be careful!\n",
		     szHead);
        }

        /* normal line processing */
        strcpy(szLineBuf, szBuf);
      }
    }else{
      // szLineBuf has already contents of the line
      bUndo = false;
    }

    /* check for empty line */
    unsigned i = 0;
    //buggy!? while(isspace(szBuf[i]))
    while(' ' == szLineBuf[i] || '\t' == szLineBuf[i])
      ++i;
    if(szLineBuf[i] == '\0')
      // empty line
      eLineKind = clkComment;
    else if(0 == strncmp(szLineBuf, TitleStartMark(), strlen(TitleStartMark())))
      eLineKind = clkPartitionTitle;
    else if(0 == strncmp(szLineBuf, CommentMark(), strlen(CommentMark())))
      eLineKind = clkComment;
    else
      eLineKind = clkData;

    return eLineKind;
}

//---------------------------------------------------------------------------
// Extract partition type from the line
void
NaConfigFile::ParseTitle (const char* szLine,
                          char*& szType, char*& szInstance)
{
// Macro doesn't work properly under Borland C++Builder 3.0
#undef isspace
    if(NULL == szLine)
        throw(na_null_pointer);

    unsigned    i = 0;

    const char  *szTitleStartMark = TitleStartMark();
    const char  *szTitleEndMark = TitleEndMark();

    /* skip title start */
    while(0 != strncmp(szLine + i, szTitleStartMark,
                       strlen(szTitleStartMark)))
        ++i;
    i += strlen(szTitleStartMark);

    /* skip spaces */
    while(isspace(szLine[i]))
        ++i;

    /* partition type start */
    unsigned    iTypeStart = i, iTypeLen;
    while(!isspace(szLine[i]) &&
          0 != strncmp(szLine + i, szTitleEndMark, strlen(szTitleEndMark)))
        ++i;
    iTypeLen = i - iTypeStart;

    szType = new char[iTypeLen + 1];
    strncpy(szType, szLine + iTypeStart, iTypeLen);
    szType[iTypeLen] = '\0';

    if(0 == strncmp(szLine + i, szTitleEndMark, strlen(szTitleEndMark))){
        /* empty instance */
        szInstance = NULL;
        return;
    }

    /* skip spaces */
    while(isspace(szLine[i]))
        ++i;

    if(0 == strncmp(szLine + i, szTitleEndMark, strlen(szTitleEndMark))){
        /* empty instance */
        szInstance = NULL;
        return;
    }

    /* partition instance start */
    unsigned    iInstanceStart = i, iInstanceLen;
    while(!isspace(szLine[i]) &&
          0 != strncmp(szLine + i, szTitleEndMark, strlen(szTitleEndMark)))
        ++i;
    iInstanceLen = i - iInstanceStart;

    szInstance = new char[iInstanceLen + 1];
    strncpy(szInstance, szLine + iInstanceStart, iInstanceLen);
    szInstance[iInstanceLen] = '\0';
}

//---------------------------------------------------------------------------
// Get data string (next call changes previous data)
char*
NaConfigFile::GetData ()
{
    NaConfigLineKind    eLineKind;

    do{
        eLineKind = GetLine();
#ifdef CONFIG_DEBUG
        NaPrintLog(">>>>> %s\n", szLineBuf);
#endif /* CONFIG_DEBUG */

        switch(eLineKind){

        case clkPartitionTitle:
	  UndoLine();
#ifdef CONFIG_DEBUG
	  NaPrintLog(">>> undo line due to new partition starts here\n");
#endif /* CONFIG_DEBUG */
	  throw(na_end_of_partition);

        case clkData:
	  return szLineBuf;
        }
    }while(eLineKind != clkEOF);

    throw(na_end_of_file);

    /* make compiler happy */
    return NULL;
}

//---------------------------------------------------------------------------
