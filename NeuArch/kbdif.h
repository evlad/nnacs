//-*-C++-*-
/* kbdif.h */
/* $Id$ */
#ifndef __kbdif_h
#define __kbdif_h

#include <NaGenerl.h>
#include <NaStdBPE.h>


// Ask for real parameter
PNNA_API NaReal	ask_user_real (const char* szPrompt, NaReal fDefault);

// Ask for integer parameter
PNNA_API int	ask_user_int (const char* szPrompt, int iDefault);

// Ask for boolean parameter
PNNA_API bool	ask_user_bool (const char* szPrompt, bool bDefault);

// Ask for string parameter
PNNA_API char*	ask_user_string (const char* szPrompt, const char* szDefault = NULL);

// Ask for name parameter (space limited)
PNNA_API char*	ask_user_name (const char* szPrompt, const char* szDefault = NULL);

// Ask for learning parameters
PNNA_API void	ask_user_lpar (NaStdBackPropParams& lpar);


#endif /* kbdif.h */
