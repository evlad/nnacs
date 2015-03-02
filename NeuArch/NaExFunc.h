//-*-C++-*-
/* NaExFunc.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaExFuncH
#define NaExFuncH

#include <NaUnit.h>
#include <NaVector.h>


//---------------------------------------------------------------------------
// Class for external function definition (mainly for shared objects)
class PNNA_API NaExternFunc : public NaUnit
{
public:

  // Make empty (y=x) function
  NaExternFunc ();

  // Make function with given options and initial vector
  NaExternFunc (char* szOptions, NaVector& vInit);

  // Destructor
  virtual ~NaExternFunc ();

};


// Function prototype for exact external function object creation.
// The function is not defined in library, but its prototype is known.
typedef NaExternFunc*	(*NaCreateExternFuncProto)(char* szOptions,
						   NaVector& vInit);


#ifdef __NaSharedExternFunction

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#  define EXFUNC_EXPORTED __declspec(dllexport)
#else
#  define EXFUNC_EXPORTED
#endif

EXFUNC_EXPORTED NaExternFunc*	NaCreateExternFunc (char* szOptions,
						    NaVector& vInit);

#ifdef __cplusplus
};
#endif

#else /* in the library */

#define NaCreateExternFunc	"NaCreateExternFunc"

#endif /* __NaSharedExternFunction */

//---------------------------------------------------------------------------
#endif
