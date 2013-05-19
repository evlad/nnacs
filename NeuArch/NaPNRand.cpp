/* NaPNRand.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaPNRand.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNRandomGen::NaPNRandomGen (const char* szNodeName)
: NaPNGenerator(szNodeName),
  pRandGenAr(NULL),
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
  delete[] pRandGenAr;
  delete[] pRandNums;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new generator function y=f() (overloaded NaPNGenerator's)
void
NaPNRandomGen::set_generator_func (NaUnit* pFunc)
{
  NaPNGenerator::set_generator_func(pFunc);

  // Destroy previous RandomSequence objects
  delete[] pRandGenAr;
  pRandGenAr = NULL;

  // Destroy previous buffer for random numbers
  delete[] pRandNums;

  // Create new set of RandomSequence objects
  pRandGenAr = new NaRandomSequence[pUnit->OutputDim()];

  // Create new buffer for random numbers
  pRandNums = new NaReal[pUnit->OutputDim()];

  // Setup distribution parameters
  unsigned    i;
  for(i = 0; i < pUnit->OutputDim(); ++i)
    {
      // Gaussian distribution by default
      pRandGenAr[i].SetDistribution(rdGaussNormal);
      pRandGenAr[i].SetUniformParams(0.0, 1.0);
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
  unsigned    i;
  for(i = 0; i < pUnit->OutputDim(); ++i)
    {
      pRandGenAr[i].SetDistribution(rdGaussNormal);
      pRandGenAr[i].SetGaussianParams(fMean[i], fStdDev[i]);
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
  unsigned    i;
  for(i = 0; i < pUnit->OutputDim(); ++i)
    {
      pRandGenAr[i].SetDistribution(rdUniform);
      pRandGenAr[i].SetUniformParams(fMin[i], fMax[i]);
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

  return (NULL != pRandGenAr) && (NULL != pRandNums);
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNRandomGen::initialize (bool& starter)
{
  NaPNGenerator::initialize(starter);

  unsigned    i;
  for(i = 0; i < pUnit->OutputDim(); ++i)
    pRandGenAr[i].Reset();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNRandomGen::action ()
{
  NaReal	dummy;

  unsigned	i;
  for(i = 0; i < pUnit->OutputDim(); ++i)
    {
      pRandGenAr[i].Function(&dummy, pRandNums + i);
      pUnit->Function(pRandNums + i, &y.data()[i]);
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
