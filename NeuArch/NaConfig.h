//-*-C++-*-
/* NaConfig.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaConfigH
#define NaConfigH
//---------------------------------------------------------------------------

#include <NaExcept.h>
#include <NaDynAr.h>

// Very long string length
#define MaxConfigFileLine           1024

// Version ids:
#define NaMajorVerNo                0
#define NaMinorVerNo                1


//---------------------------------------------------------------------------
// Classification of the lines in
enum NaConfigLineKind {
    clkPartitionTitle = 0,
    clkComment,             // and empty line too
    clkData,
    clkEOF,                 // End of file
    __cltNumber             // special meaning
};


//---------------------------------------------------------------------------
// Stream of data object
class NaDataStream
{
public:

    NaDataStream (const unsigned pVersion[]);
    virtual ~NaDataStream ();

    // Version info
    unsigned        GetVersion (int id) const;
    
    //***********************************************************************
    // Generic abstract operations: store and retrieve data
    //***********************************************************************

    // Put string with ending comment
    virtual void    PutData (const char* szData,
                             const char* szComment = NULL) = 0;

    // Get data string (next call changes previous data buffer)
    virtual char*   GetData () = 0;


    //***********************************************************************
    // Derived operations
    //***********************************************************************

    // Put comment (or empty line)
    void        PutComment (const char* szComment = NULL);

    // Put formatted string with comment (see *printf)
    void        PutF (const char* szComment, const char* szFormat, ...);

    // Reserve buffer for formatted data (in bytes)
    void        FormatBuffer (unsigned nSize = MaxConfigFileLine);

    // Get formatted string (see *scanf)
    void        GetF (const char* szFormat, ...);

private:

    // Buffer for format operation (PutF only)
    char        *szFormatBuf;

    // Version of the file format
    const unsigned  *pVer;    

};


//---------------------------------------------------------------------------
// Configuration file partition object.  Abstract class.
// Has generic name and instance name.
class NaConfigPart
{
public:/* methods */

    // Initialize configuration partition with type name - one word
    // without spaces, punctuation and special marks
    NaConfigPart (const char* szType);

    // Initialize configuration partition as a copy of another
    NaConfigPart (const NaConfigPart& rCP);

    // Destroy local data of the configuation partition
    virtual ~NaConfigPart ();

    // Get name of the type
    const char* GetType () const;

    // Assign name to instance (NULL by default) - one word
    // without spaces, punctuation and special marks
    void        SetInstance (const char* szInstance);

    // Get name of the instance (NULL by default)
    const char* GetInstance () const;

    //***********************************************************************
    // Abstract operations: store and retrieve data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds) = 0;

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds) = 0;

private:/* data */

    // Storage
    char    *szTypeName;        // Type of the partition
    char    *szInstanceName;    // Instance of the partition
    
public:/* data */

    // Data reference that can be accessed knowing the type
    void    *pSelfData;
};


// Registrar function type
typedef NaConfigPart* (*NaCPRegistrar)();

// Registrar storage item
typedef struct {
  char		*szTypeName;
  NaCPRegistrar	pRegFunc;
}	NaCPRegItem;

// Registrar storage array
typedef NaDynAr<NaCPRegItem>	NaCPRegArray;


//---------------------------------------------------------------------------
// Configuration file object.
// Has common methdos to read and write text configuration.  Consists of
// many partitions.

class NaConfigFile : public NaDataStream
{
public:

    // Common identification: magic string at the beginning of the file
    // and default file name extension.
    // Plus space-separated marks for start of partition title, end of it
    // and for comment starting.
    NaConfigFile (const char* szMagic,
                  unsigned nMajorVerNo,
                  unsigned nMinorVerNo,
                  const char* szFileExt = ".cna",
                  const char* szMarks = "[ ] ;");

    virtual ~NaConfigFile ();

    //***********************************************************************
    // Self-identification
    //***********************************************************************

    // Version info
    unsigned        Version (int id) const;

    // Self-identification by magic
    const char*     Magic () const;

    // Self-identification by file extension
    const char*     FileExt () const;


    //***********************************************************************
    // Used marks
    //***********************************************************************

    // Title start mark string used in file
    const char*     TitleStartMark () const;

    // Title end mark string used in file
    const char*     TitleEndMark () const;

    // Comment mark string used in file
    const char*     CommentMark () const;


    //***********************************************************************
    // Registrar for dynamic partition attachment
    //***********************************************************************

    // Add registrar for given partition type (create it dynamically)
    void            AddRegistrar (const char* szPartType,
				  NaCPRegistrar pRegFunc);

    // Find registrar for given partition type (create it dynamically)
    NaCPRegistrar   FindRegistrar (const char* szPartType);

    //***********************************************************************
    // Partition attachment/detachment operations
    //***********************************************************************

    // Add partitions (created externally)
    void        AddPartitions (unsigned nParts, NaConfigPart* pParts[]);

    // Clear list of partitions
    void        RemovePartitions ();


    //***********************************************************************
    // The whole file operations
    //***********************************************************************

    // Save partitions' data with overwriting existant file
    void        SaveToFile (const char* szFilePath);

    // Load partitions' data from file
    void        LoadFromFile (const char* szFilePath);


    //***********************************************************************
    // Inside file operations while saving
    //***********************************************************************

    // Put string to file
    void        PutString (const char* szBuf);

    // Put title of the partition
    // PartType and PartInstance must be words without spaces!
    void        PutPartTitle (const char* szPartType,
                              const char* szPartInstance);

    // Put string with ending comment
    virtual void    PutData (const char* szData,
                             const char* szComment = NULL);


    //***********************************************************************
    // Inside file operations while loading
    //***********************************************************************

    // Extract partition type from the line
    void        ParseTitle (const char* szLine,
                            char*& szType, char*& szInstance);

    // Get data string (next call changes previous data)
    virtual char*   GetData ();

private:

    // Get the whole line from file, put it to szLineBuf and returns
    // type of the line
    NaConfigLineKind    GetLine ();

    // Undo the whole szLineBuf reading from file (only one step of
    // undo is provided)
    void		UndoLine ();


    // Delimiters and special strings
    char    *szMagicString;     // Magic string in the beginning of the file
    char    *szFileExtString;   // Filename extension

    char    *szMarksBuf;        // Common memory buffer for next marks:
    char    *szTitleStartMark;  // Start of the partition title
    char    *szTitleEndMark;    // Finish of the partition title
    char    *szCommentMark;     // Start of the comment until end of line

    // Version info: [0] - major, [1] - minor
    unsigned        nVer[2];    // own class version
    unsigned        nFileVer[2];// actual file version

    // Storage for partitions
    unsigned        nPartList;
    NaConfigPart    **pPartList;

    // Storage for registrars
    NaCPRegArray    arReg;

    // During save/load operations
    FILE    *fp;
    char    szLineBuf[MaxConfigFileLine + 1];
    bool    bUndo;	// just one step of undo can be performed

};


//---------------------------------------------------------------------------
#endif
