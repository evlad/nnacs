/* NaTrainNN.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaExcept.h"
#include "NaTrainNN.h"


//---------------------------------------------------------------------------
NaTrainNN::NaTrainNN()
    : m_pNN(NULL), m_pDS(NULL)
{
}


//---------------------------------------------------------------------------
NaTrainNN::~NaTrainNN ()
{
    // nothing
}


//---------------------------------------------------------------------------
/// Setup for the given neural network
void
NaTrainNN::BeginWithNN (NaNNUnit* pNN)
{
    if(NULL == pNN)
	throw(na_null_pointer);
    m_pNN = pNN;
}


//---------------------------------------------------------------------------
/// Setup data set
/// @return \b false in case of dimension mismatch
bool
NaTrainNN::AssignDataSet (NaDataSet* pDS)
{
    if(NULL == pDS)
	throw(na_null_pointer);
    if(NULL == NN())
	throw(na_not_found);

    m_pDS = pDS;

    return NN()->InputDim() == DataSet()->GetInputSize()
	&& NN()->OutputDim() == DataSet()->GetOutputSize();
}


//---------------------------------------------------------------------------
/// Access to the currently assigned neural network.
NaNNUnit*
NaTrainNN::NN ()
{
    return m_pNN;
}

    
//---------------------------------------------------------------------------
/// Access to the currently assigned data set.
NaDataSet*
NaTrainNN::DataSet ()
{
    return m_pDS;
}
