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
class NaExternFunc : public NaUnit
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

extern "C" NaExternFunc*	NaCreateExternFunc (char* szOptions,
						    NaVector& vInit);

#else /* in the library */

#define NaCreateExternFunc	"NaCreateExternFunc"

#endif /* __NaSharedExternFunction */

//---------------------------------------------------------------------------
#endif
