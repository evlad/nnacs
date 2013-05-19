/* NaDataIO.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <string.h>

#ifdef unix
#define stricmp		strcasecmp
#endif /* unix */

#include <stdio.h>

#include "NaLogFil.h"
#include "NaDataIO.h"

//---------------------------------------------------------------------------
// Create a stream or simply read it
NaDataFile::NaDataFile (const char* fname,
                        NaFileMode fm)
{
    if(NULL == fname)
        throw(na_null_pointer);
	if(!strcmp(fname, "/dev/null")) {
		szFileName = new char[strlen(DEV_NULL) + 1];
		strcpy(szFileName, DEV_NULL);
	} else {
		szFileName = new char[strlen(fname) + 1];
		strcpy(szFileName, fname);
	}
    eFileMode = fm;
    nRecords = 0;
}

//---------------------------------------------------------------------------
// Close (and write) file
NaDataFile::~NaDataFile ()
{
    delete[] szFileName;
}

//---------------------------------------------------------------------------
// Return number of records in the file
long    NaDataFile::CountOfRecord ()
{
    long    n = 0;
    switch(eFileMode){
    case fmReadOnly:
        n = 0;
        if(GoStartRecord())
            while(GoNextRecord())
                ++n;
        break;

    case fmCreateEmpty:
        n = nRecords;
        break;
    }
    return n;
}

//***********************************
// Common description of the data
//***********************************

//---------------------------------------------------------------------------
// Set title string; many titles are supported
void    NaDataFile::SetTitle (const char* szTitle, int iTitleNo)
{
    // Dummy
}

//---------------------------------------------------------------------------
// Get title string; many titles are supported
const char* NaDataFile::GetTitle (int iTitleNo)
{
    return NULL;
}

//***********************************
// File format determiner
//***********************************

//---------------------------------------------------------------------------
// Guess file format by filename (see .EXT)
NaFileFormat NaDataFile::GuessFileFormatByName (const char* szFName)
{
    if(NULL == szFName)
        throw(na_null_pointer);

    // Text output in case of character device or FIFO
    struct stat	st;
    if(-1 != stat(szFName, &st)){
#ifdef WIN32
	  if(!!(st.st_mode & _S_IFCHR) || !stricmp(szFName, "NUL"))
#else
      if(S_ISFIFO(st.st_mode) || S_ISCHR(st.st_mode))
#endif
		return ffTextStream;
#ifdef WIN32
	  if(!(st.st_mode & _S_IFREG))
#else
      if(S_ISBLK(st.st_mode) || S_ISDIR(st.st_mode))
#endif
		return ffUnknown;
    }
    /* else
       don't user stat() results in case of it's error */

    int len = strlen(szFName);
    if(len >= 4){
        if(!stricmp(szFName + len - 4, NaIO_TEXT_STREAM_EXT))
            return ffTextStream;
        else if(!stricmp(szFName + len - 4, NaIO_STATISTICA_EXT))
            return ffStatistica;
        else if(!stricmp(szFName + len - 4, NaIO_BINARY_STREAM_EXT))
            return ffBinaryStream;
        else if(!stricmp(szFName + len - 4, NaIO_DPLOT_EXT))
            return ffDPlot;
    }
    return ffUnknown;
}


//---------------------------------------------------------------------------
// Guess file format by quick file observe (read magic)
NaFileFormat NaDataFile::GuessFileFormatByMagic (const char* szFName)
{
    NaFileFormat    guess; // = ffUnknown;

    if(NULL == szFName)
        throw(na_null_pointer);

    FILE    *fp = fopen(szFName, "rb");
    if(NULL == fp)
        throw(na_cant_open_file);

    // Check for...

    // STATISTICA data file
    {
        unsigned    magic_len = strlen(NaIO_STATISTICA_MAGIC);
        char        buf[1 + sizeof(NaIO_STATISTICA_MAGIC)];
        fseek(fp, 0, SEEK_SET);
        if(magic_len == fread(buf, 1, magic_len, fp)){
            buf[magic_len] = '\0';
            if(!strcmp(buf, NaIO_STATISTICA_MAGIC)){
                guess = ffStatistica;
                goto END;
            }
        }
    }

    // DPLOT data file
    {
        unsigned    magic_len = strlen(NaIO_DPLOT_MAGIC);
        char        buf[1 + sizeof(NaIO_DPLOT_MAGIC)];
        fseek(fp, 0, SEEK_SET);
        if(magic_len == fread(buf, 1, magic_len, fp)){
            buf[magic_len] = '\0';
            if(!strcmp(buf, NaIO_DPLOT_MAGIC)){
                guess = ffDPlot;
                goto END;
            }
        }
    }

    // NeuArch binary data stream
    {
        unsigned    magic_len = strlen(NaIO_BINARY_MAGIC);
        char        buf[1 + sizeof(NaIO_BINARY_MAGIC)];
        fseek(fp, 0, SEEK_SET);
        if(magic_len == fread(buf, 1, magic_len, fp)){
            buf[magic_len] = '\0';
            if(!strcmp(buf, NaIO_BINARY_MAGIC)){
                guess = ffBinaryStream;
                goto END;
            }
        }
    }

    // Other data file
    guess = ffUnknown;

END:
    fclose(fp);
    return guess;
}


//---------------------------------------------------------------------------
// Create object (NaDataFile descendant) for reading given data file
NaDataFile* OpenInputDataFile (const char* szPath)
{
    NaDataFile  *pDF;
    NaFileFormat    guess; // = ffUnknown;

    try{
        guess = NaDataFile::GuessFileFormatByMagic(szPath);
    }catch(...){
        // If some errors occured try to check type of file by name
        guess = ffUnknown;
    }

    if(guess == ffUnknown)
        guess = NaDataFile::GuessFileFormatByName(szPath);

    switch(guess){

    case ffStatistica:
#ifdef _STADEV_H
        pDF = new NaStatisticaFile(szPath, fmReadOnly);
#endif /* _STADEV_H */
        NaPrintLog("STATISTICA input data file '%s'\n",
                   szPath);
#ifndef _STADEV_H
	throw(na_not_implemented);
#endif /* _STADEV_H */
        break;

    case ffTextStream:
        pDF = new NaTextStreamFile(szPath, fmReadOnly);
        NaPrintLog("Text stream input data file '%s'\n",
                   szPath);
        break;

    case ffBinaryStream:
        pDF = new NaBinaryStreamFile(szPath, fmReadOnly);
        NaPrintLog("Binary stream input data file '%s'\n",
                   szPath);
        break;

    case ffDPlot:
        pDF = new NaDPlotFile(szPath, fmReadOnly);
        NaPrintLog("DPlot input data file '%s'\n",
                   szPath);
        break;

    default:
        NaPrintLog("Can't determine type of input data file '%s'\n",
                   szPath);
        pDF = NULL;
        break;
    }
    return pDF;
}


//---------------------------------------------------------------------------
// Create object (NaDataFile descendant) for writing given data file
NaDataFile* OpenOutputDataFile (const char* szPath,
				NaBinaryDataType bdt,
				int var_num)
{
    NaDataFile  *pDF;
    switch(NaDataFile::GuessFileFormatByName(szPath)){

    case ffStatistica:
#ifdef _STADEV_H
        pDF = new NaStatisticaFile(szPath, fmCreateEmpty);
#endif /* _STADEV_H */
        NaPrintLog("STATISTICA output data file '%s'\n",
                   szPath);
#ifndef _STADEV_H
	throw(na_not_implemented);
#endif /* _STADEV_H */
        break;

    case ffTextStream:
        pDF = new NaTextStreamFile(szPath, fmCreateEmpty);
        NaPrintLog("Text stream output data file '%s'\n",
                   szPath);
        break;

    case ffBinaryStream:
        pDF = new NaBinaryStreamFile(szPath, fmCreateEmpty, bdt, var_num);
        NaPrintLog("Binary stream input data file '%s' (%d variables)\n",
                   szPath, var_num);
        break;

    case ffDPlot:
        pDF = new NaDPlotFile(szPath, fmCreateEmpty);
        NaPrintLog("DPlot output data file '%s'\n",
                   szPath);
        break;

    default:
        NaPrintLog("Can't determine type of output data file '%s'\n",
                   szPath);
        pDF = NULL;
        break;
    }
    return pDF;
}

//---------------------------------------------------------------------------

