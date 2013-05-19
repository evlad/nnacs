/* NaPNFetc.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNFetc.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNFetcher::NaPNFetcher (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  out(this, "out")
{
    piOutMap = NULL;	// Inputs in output vector
    nOutDim = -1;	// Output dimension
    pfSumWeights = NULL;// Do not apply summation
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set output dimension and position of input
void
NaPNFetcher::set_output (unsigned iPos, int nDim)
{
  check_tunable();

  nOutDim = nDim;
  delete[] piOutMap;

  if(nOutDim < 0)
    piOutMap = NULL;
  else
    {
      int	i;
      piOutMap = new unsigned[nOutDim];
      for(i = 0; i < nOutDim; ++i)
	piOutMap[i] = iPos + i;
    }
}


//---------------------------------------------------------------------------
// Set output dimension and positions of input (0,1...)
void
NaPNFetcher::set_output (int nDim, const unsigned* piMap)
{
  check_tunable();

  nOutDim = nDim;
  delete[] piOutMap;

  if(nOutDim < 0)
    piOutMap = NULL;
  else if(NULL == piMap)
    throw(na_null_pointer);
  else
    {
      int	i;
      piOutMap = new unsigned[nOutDim];
      for(i = 0; i < nOutDim; ++i)
	piOutMap[i] = piMap[i];
    }
}


//---------------------------------------------------------------------------
// Setup sum of inputs with given weights; if no weights are given
// then sum with weight 1.0
void
NaPNFetcher::set_sum_weights (const NaReal* pfWeights)
{
  check_tunable();

  if(nOutDim < 0)
    {
      delete[] pfSumWeights;
      pfSumWeights = NULL;
    }
  else
    {
      int	i;
      pfSumWeights = new NaReal[nOutDim];
      if(NULL == pfWeights)
	for(i = 0; i < nOutDim; ++i)
	  pfSumWeights[i] = 1.0;
      else
	for(i = 0; i < nOutDim; ++i)
	  pfSumWeights[i] = pfWeights[i];
    }
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNFetcher::relate_connectors ()
{
    if(nOutDim >= 0){
      if(NULL == pfSumWeights)
        out.data().new_dim(nOutDim);
      else /* sum means scalar */
        out.data().new_dim(1);
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNFetcher::verify ()
{
  if(nOutDim < 0){
    NaPrintLog("VERIFY FAILED: output dimension is not set!\n");
    return false;
  }

  if(NULL == pfSumWeights)
    {
      int	i;
      unsigned	iMaxPos = 0;
      for(i = 0; i < nOutDim; ++i){
	if(iMaxPos < piOutMap[i])
	  iMaxPos = piOutMap[i];
      }

      if((unsigned)iMaxPos >= in.data().dim()){
	NaPrintLog("VERIFY FAILED: some output positions are out "\
		   "of input range!\n");
	return false;
      }
    }

  return true;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNFetcher::action ()
{
  if(NULL == pfSumWeights)
    {
      int	i;
      for(i = 0; i < nOutDim; ++i){
	out.data()[i] = in.data()[piOutMap[i]];
      }
    }
  else
    {
      NaReal	fSum = 0.0;
      int	i;
      for(i = 0; i < nOutDim; ++i){
	fSum += pfSumWeights[i] * in.data()[piOutMap[i]];
      }
      out.data()[0] = fSum;
    }
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
 
