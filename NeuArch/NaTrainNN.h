//-*-C++-*-
/* NaTrainNN.h */
//---------------------------------------------------------------------------
#ifndef __NaTrainNN_h
#define __NaTrainNN_h

#include <NaExcept.h>
#include <NaDataSet.h>
#include <NaNNUnit.h>


//---------------------------------------------------------------------------
/// Base class for supervised learning og neural network.
class PNNA_API NaTrainNN
{
  public:

    NaTrainNN ();
    virtual ~NaTrainNN ();

    /// Setup for the given neural network
    virtual void	BeginWithNN (NaNNUnit* pNN);

    /// Setup data set
    /// @return \b false in case of dimension mismatch
    virtual bool	AssignDataSet (NaDataSet* pDS);

    /// Make one training step and calculate final MSE.
    /// @returns \b true if the next step is reasonable and \b false
    /// if local minimum or other problem was encountered, which
    /// prevents further learning
    virtual bool	TrainingStep (NaVector& vFinalMSE) = 0;

    /// Access to the currently assigned neural network.
    virtual NaNNUnit*	NN ();
    
    /// Access to the currently assigned data set.
    virtual NaDataSet*	DataSet ();

  protected:

    NaNNUnit	*m_pNN;
    NaDataSet	*m_pDS;

};

#endif // __NaTrainNN_h
