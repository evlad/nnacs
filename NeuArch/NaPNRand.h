//-*-C++-*-
/* NaPNRand.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNRandH
#define NaPNRandH

#include <NaPetri.h>
#include <NaRandom.h>
#include <NaPNGen.h>


//---------------------------------------------------------------------------
// Applied Petri net node: random generator unit.
// Has no inputs and the only output

//---------------------------------------------------------------------------
class NaPNRandomGen : public NaPNGenerator
{
public:

  // Create node for Petri network
  NaPNRandomGen (const char* szNodeName = "randomgen");

  // Destroy node
  virtual ~NaPNRandomGen ();

  ////////////////
  // Connectors //
  ////////////////

  /* inherited */


  ///////////////////
  // Node specific //
  ///////////////////

  // Assign new generator function y=f() (overloaded NaPNGenerator's)
  virtual void        set_generator_func (NaUnit* pFunc);


  // Setup random generation with normal (Gauss) distribution
  // with given output dimension
  virtual void        set_gauss_distrib (const NaReal* fMean,
					 const NaReal* fStdDev);

  // Setup random generation with uniform distribution
  // with given output dimension
  virtual void        set_uniform_distrib (const NaReal* fMin,
					   const NaReal* fMax);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 5. Verification to be sure all is OK (true)
  virtual bool        verify ();

  // 6. Initialize node activity and setup starter flag if needed
  virtual void        initialize (bool& starter);

  // 8. True action of the node (if activate returned true)
  virtual void        action ();


protected:/* data */

  // Array of RandomSequence units
  NaRandomSequence	*pRandGenAr;

  // Array of random numbers
  NaReal		*pRandNums;

};


//---------------------------------------------------------------------------
#endif
