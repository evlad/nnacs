/* NaPNRand.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPNRand.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNRandomGen::NaPNRandomGen (const char* szNodeName)
: NaPNGenerator(szNodeName),
  pRandGen(NULL),
  pRandNums(NULL)
  ////////////////
  // Connectors //
  ////////////////
  /* inherited */
{
  // dummy
}


//---------------------------------------------------------------------------
// Destroy node
NaPNRandomGen::~NaPNRandomGen ()
{
  delete pRandGen;
  delete[] pRandNums;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new generator function y=f(x) (overloaded NaPNGenerator's)
void
NaPNRandomGen::set_generator_func (NaUnit* pFunc)
{
  NaPNGenerator::set_generator_func(pFunc);

  // In case of different input dimension let's reinitialize all
  // distrubution parameters
  if(NULL != pRandGen && pUnit->InputDim() != pRandGen->OutputDim() ||
     NULL == pRandGen)
    {
      // Destroy previous RandomSequence object
      delete pRandGen;

      // Destroy previous buffer for random numbers
      delete[] pRandNums;

      // Create new set of RandomSequence objects
      pRandGen = new NaRandomSequence(pUnit->InputDim());

      // Create new buffer for random numbers
      pRandNums = new NaReal[pUnit->InputDim()];

      // Setup distribution parameters
      for(unsigned i = 0; i < pUnit->InputDim(); ++i)
	{
	  // Gaussian distribution by default
	  pRandGen->SetDistribution(rdGaussNormal, i);
	  pRandGen->SetUniformParams(0.0, 1.0, i);
	}
    }
}

//---------------------------------------------------------------------------
// Setup random generation with normal (Gauss) distribution
// with given output dimension
void
NaPNRandomGen::set_gauss_distrib (const NaReal* fMean, const NaReal* fStdDev)
{
  check_tunable();

  if(NULL == pUnit || NULL == fMean || NULL == fStdDev)
    throw(na_null_pointer);

  // Setup distribution parameters
  for(unsigned i = 0; i < pUnit->InputDim(); ++i)
    {
      pRandGen->SetDistribution(rdGaussNormal, i);
      pRandGen->SetGaussianParams(fMean[i], fStdDev[i], i);
    }
}


//---------------------------------------------------------------------------
// The same parameters for the whole dimension
void
NaPNRandomGen::set_gauss_distrib (NaReal fMean, NaReal fStdDev)
{
  check_tunable();

  if(NULL == pUnit)
    throw(na_null_pointer);

  // Setup distribution parameters
  for(unsigned i = 0; i < pUnit->InputDim(); ++i)
    {
      pRandGen->SetDistribution(rdGaussNormal, i);
      pRandGen->SetGaussianParams(fMean, fStdDev, i);
    }
}


//---------------------------------------------------------------------------
// Setup random generation with uniform distribution
// with given output dimension
void
NaPNRandomGen::set_uniform_distrib (const NaReal* fMin, const NaReal* fMax)
{
  check_tunable();

  if(NULL == pUnit || NULL == fMin || NULL == fMax)
    throw(na_null_pointer);

  // Setup distribution parameters
  for(unsigned i = 0; i < pUnit->InputDim(); ++i)
    {
      pRandGen->SetDistribution(rdUniform, i);
      pRandGen->SetUniformParams(fMin[i], fMax[i], i);
    }
}


//---------------------------------------------------------------------------
// The same parameters for the whole dimension
void
NaPNRandomGen::set_uniform_distrib (NaReal fMin, NaReal fMax)
{
  check_tunable();

  if(NULL == pUnit)
    throw(na_null_pointer);

  // Setup distribution parameters
  for(unsigned i = 0; i < pUnit->InputDim(); ++i)
    {
      pRandGen->SetDistribution(rdUniform, i);
      pRandGen->SetUniformParams(fMin, fMax, i);
    }
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNRandomGen::verify ()
{
  if(!NaPNGenerator::verify())
    return false;

  return (NULL != pRandGen) && (NULL != pRandNums);
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNRandomGen::initialize (bool& starter)
{
  pRandGen->Reset();
  NaPNGenerator::initialize(starter);
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNRandomGen::action ()
{
  NaReal	dummy;

  unsigned	i;
  pRandGen->Function(&dummy, pRandNums);
  pUnit->Function(pRandNums, &y.data()[0]);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
