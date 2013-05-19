//-*-C++-*-
/* NaParams.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaParamsH
#define NaParamsH


//---------------------------------------------------------------------------
// Class for list of parameters stored in file
// Format of file:
//  <file> ::= <line> <file> | <empty>
//  <line> ::= <comment-line> | <assign-line>
//  <assign-line> ::= <name> "=" <value>
//  <comment-line> ::= "#" <any-characters>

#define NaPARAM_LINE_MAX_LEN	1023

//---------------------------------------------------------------------------
class NaParams
{
public:

  // Open file with parameters
  NaParams (const char* szFileName,
	    /* Extra values in format argv[i]=" name = value " */
	    int argc = 0, char** argv = NULL,
	    /* Special char: comment; assignment */
	    const char szSpecChar[2] = "#=");

  virtual ~NaParams ();

  // Get parameter's value by his name
  char*		GetParam (const char* szParamName) const;
  char*		operator() (const char* szParamName) const{
    return GetParam(szParamName);
  }

  // Fetch parameter's value by his name
  char*&	FetchParam (const char* szParamName);
  char*&	operator[] (const char* szParamName){
    return FetchParam(szParamName);
  }

  // Check for parameter existence
  bool		CheckParam (const char* szParamName) const;

  // Get array of parameters listed in the line
  char**	GetListOfParams (const char* szParamName,
				 int& nList,
				 const char* szDelims = " ") const;
  char**	operator() (const char* szParamName,
			    int& nList,
			    const char* szDelims = " ") const{
    return GetListOfParams(szParamName, nList, szDelims);
  }

  // Put parameters to log file
  void		ListOfParamsToLog () const;

protected:

  char	spec[2];
  char	buf[NaPARAM_LINE_MAX_LEN+1];	// Line buffer

  struct item_t {
    char	*name;
    char	*value;
  }	*storage;

  int	stored_n;

  // Compare two items
  static int	stored_cmp (const void* p1, const void* p2);

  // Parse the line to name and value (new allocated) or return false
  // in case of bad suntax
  bool		parse_line (char* line, char*& name, char*& value);

};


//---------------------------------------------------------------------------
#endif
