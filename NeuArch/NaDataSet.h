//-*-C++-*-
/* NaDataSet.h */
//---------------------------------------------------------------------------
#ifndef __NaDataSet_h
#define __NaDataSet_h

#include <NaDynAr.h>
#include <NaVector.h>
#include <NaUnit.h>
#include <NaExcept.h>


//---------------------------------------------------------------------------
/// Data set for supervised learning method implementation.
class PNNA_API NaDataSet
{
 public:

  /// Create data set with given number of input and output items in
  /// vectors.  Zero size does not allowed and causes na_bad_value
  /// exception.
  NaDataSet (unsigned nInSize, unsigned nOutSize);

  /// Create copy of the data set.
  NaDataSet (const NaDataSet& rDS);

  /// Release all memory.
  virtual ~NaDataSet ();

  /// Reset to empty data set and change size of vectors.  0 size
  /// means the same size of the vector.
  void		Reset (unsigned nInSize = 0, unsigned nOutSize = 0);

  /// Add pair of input and output
  void		AddPair (const NaReal* pInVector, const NaReal* pOutVector);

  /// Return number of pairs (input,output) in the data set
  unsigned	NumberOfPairs () const;

  /// Get vector of the input for modification.  Throws
  /// na_out_of_range in case of wrong index).
  NaVector&	FetchInput (unsigned iPair);

  /// Get vector of the input.  Throws na_out_of_range in case of
  /// wrong index).
  const NaVector&	GetInput (unsigned iPair) const;

  /// Get size of the input vector.
  unsigned	GetInputSize () const;

  /// Get vector of the output for modification.  Throws
  /// na_out_of_range in case of wrong index).
  NaVector&	FetchOutput (unsigned iPair);

  /// Get vector of the output.  Throws na_out_of_range in case of
  /// wrong index).
  const NaVector&	GetOutput (unsigned iPair) const;

  /// Get size of the output vector.
  unsigned	GetOutputSize () const;

  /// Calculate MSE for the output of the unit (may be NN also) and
  /// the desired output provided in the data set.  Calculated actual
  /// output of the unit is stored in optional pDSOut array of vector.
  /// @return MSE
  NaVector	CalcOutputMSE (NaUnit& unit, NaDynAr<NaVector>* pDSOut = NULL);

 private:

  /// Size of input vector
  unsigned	m_nInSize;

  /// Input vectors
  NaDynAr<NaVector>	m_vIn;

  /// Size of output vector
  unsigned	m_nOutSize;

  /// Output vectors
  NaDynAr<NaVector>	m_vOut;

};

#endif // __NaDataSet_h
