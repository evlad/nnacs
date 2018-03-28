/* NaDataSet.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <algorithm>    // std::min

#include "NaExcept.h"
#include "NaDataSet.h"

//---------------------------------------------------------------------------
/// Create data set with given number of input and output items in
/// vectors.  Zero size does not allowed and causes na_bad_value
/// exception.
NaDataSet::NaDataSet (unsigned nInSize, unsigned nOutSize)
    : m_nInSize(nInSize), m_nOutSize(nOutSize)
{
    if(0 == nInSize || 0 == nOutSize)
	throw(na_bad_value);
}


//---------------------------------------------------------------------------
/// Create copy of the data set.
NaDataSet::NaDataSet (const NaDataSet& rDS)
    : m_nInSize(rDS.m_nInSize), m_nOutSize(rDS.m_nOutSize),
      m_vIn(rDS.m_vIn), m_vOut(rDS.m_vOut)
{
}


//---------------------------------------------------------------------------
/// Release all memory.
NaDataSet::~NaDataSet ()
{
}


//---------------------------------------------------------------------------
/// Reset to empty data set and change size of vectors.  0 size
/// means the same size of the vector.
void
NaDataSet::Reset (unsigned nInSize, unsigned nOutSize)
{
    m_vIn.clean();
    m_vOut.clean();
}


//---------------------------------------------------------------------------
/// Add pair of input and output.
void
NaDataSet::AddPair (const NaReal* pInVector, const NaReal* pOutVector)
{
    m_vIn.addh(new NaVector(m_nInSize, pInVector));
    m_vOut.addh(new NaVector(m_nOutSize, pOutVector));
}


//---------------------------------------------------------------------------
/// Return number of pairs (input,output) in the data set
unsigned
NaDataSet::NumberOfPairs () const
{
    return std::min(m_vIn.count(), m_vOut.count());
}


//---------------------------------------------------------------------------
/// Get vector of the input for modification.  Throws
/// na_out_of_range in case of wrong index).
NaVector&
NaDataSet::FetchInput (unsigned iPair)
{
    if(iPair >= NumberOfPairs())
	throw(na_out_of_range);
    return m_vIn[iPair];
}


//---------------------------------------------------------------------------
/// Get vector of the input.  Throws na_out_of_range in case of
/// wrong index).
const NaVector&
NaDataSet::GetInput (unsigned iPair) const
{
    if(iPair >= NumberOfPairs())
	throw(na_out_of_range);
    return m_vIn(iPair);
}


//---------------------------------------------------------------------------
/// Get size of the input vector.
unsigned
NaDataSet::GetInputSize () const
{
    return m_nInSize;
}


//---------------------------------------------------------------------------
/// Get vector of the output for modification.  Throws
/// na_out_of_range in case of wrong index).
NaVector&
NaDataSet::FetchOutput (unsigned iPair)
{
    if(iPair >= NumberOfPairs())
	throw(na_out_of_range);
    return m_vOut[iPair];
}


//---------------------------------------------------------------------------
/// Get vector of the output.  Throws na_out_of_range in case of wrong
/// index).
const NaVector&
NaDataSet::GetOutput (unsigned iPair) const
{
    if(iPair >= NumberOfPairs())
	throw(na_out_of_range);
    return m_vOut(iPair);
}


//---------------------------------------------------------------------------
/// Get size of the output vector.
unsigned
NaDataSet::GetOutputSize () const
{
    return m_nOutSize;
}


//---------------------------------------------------------------------------
/// Calculate MSE for the output of the unit (may be NN also) and
/// the desired output provided in the data set.  Calculated actual
/// output of the unit is stored in optional pDSOut array of vector.
/// @return MSE
NaVector
NaDataSet::CalcOutputMSE (NaUnit& unit, NaDynAr<NaVector>* pDSOut)
{
  NaVector	vMSE(unit.InputDim());
  NaVector	vOut(unit.OutputDim());

  vMSE.init_zero();

  // Iterate through the whole dataset
  for(unsigned i = 0; i < NumberOfPairs(); ++i)
    {
      NaVector	&vIn = m_vIn[i];
      NaVector	&vTar = m_vOut[i];

      // Forward propagation of the data through NN
      unit.Function(&vIn[0], &vOut[0]);
      if(NULL != pDSOut)
	pDSOut->addh(vOut);

      // Compute sum of squared error
      for(unsigned j = 0; j < unit.OutputDim(); ++j)
	vMSE[j] += (vTar[j] - vOut[j]) * (vTar[j] - vOut[j]);
    }

  return vMSE;
}
