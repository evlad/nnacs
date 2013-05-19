//-*-C++-*-
/* kbdif.h */
/* $Id$ */
#ifndef __kbdif_h
#define __kbdif_h

#include <NaGenerl.h>
#include <NaStdBPE.h>


// Ask for real parameter
NaReal	ask_user_real (const char* szPrompt, NaReal fDefault);

// Ask for integer parameter
int	ask_user_int (const char* szPrompt, int iDefault);

// Ask for boolean parameter
bool	ask_user_bool (const char* szPrompt, bool bDefault);

// Ask for string parameter
char*	ask_user_string (const char* szPrompt, const char* szDefault = NULL);

// Ask for name parameter (space limited)
char*	ask_user_name (const char* szPrompt, const char* szDefault = NULL);

// Ask for learning parameters
void	ask_user_lpar (NaStdBackPropParams& lpar);


#endif /* kbdif.h */
