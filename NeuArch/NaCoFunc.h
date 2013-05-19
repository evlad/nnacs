//-*-C++-*-
/* NaCoFunc.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaCoFuncH
#define NaCoFuncH

#include <stdarg.h>

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaConfig.h>
#include <NaDynAr.h>
#include <NaVector.h>
#include <NaLogFil.h>


// Name of the type for config file
#define NaTYPE_CombinedFunc	"CombinedFunction"

//---------------------------------------------------------------------------
// Class for combined function (sequence of NaUnits)
class NaCombinedFunc : public NaUnit, public NaConfigPart
{
public:

  // Registrar for the NaCombinedFunc
  static NaConfigPart*	NaRegCombinedFunc ();

  // Make empty combined function
  NaCombinedFunc ();

  // Destructor
  virtual ~NaCombinedFunc ();

  // Print self
  virtual void	PrintLog () const;

  // Print self with indentation
  virtual void	PrintLog (const char* szIndent) const;

  // Is empty
  virtual bool	Empty () const;

  // Make empty
  virtual void	Clean ();


  //***********************************************************************
  // Unit part
  //***********************************************************************

  // Reset operations, that must be done before new modelling
  // will start
  virtual void	Reset ();

  // Compute output on the basis of internal parameters,
  // stored state and external input: y=F(x,t,p)
  virtual void	Function (NaReal* x, NaReal* y);

  //***********************************************************************
  // Store and retrieve configuration data
  //***********************************************************************

  // Store configuration data in internal order to given stream
  virtual void	Save (NaDataStream& ds);

  // Retrieve configuration data in internal order from given stream
  virtual void	Load (NaDataStream& ds);

  // Store sequence of functions to file
  virtual void	Save (const char* szFileName);

  // Read sequence of functions from given configuration file.  Smart
  // enough to take care of transfer (.tf) and combined functions (.cof)
  virtual void	Load (const char* szFileName);

protected:

  // Array of partitions
  unsigned	nParts;
  NaConfigPart	**pParts;

  // Configuration file
  NaConfigFile	conf_file;

private:

  // Time sample
  int		iTime;

};


//---------------------------------------------------------------------------
#endif
