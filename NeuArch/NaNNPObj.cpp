/* NaNNPObj.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaNNPObj.h"


//---------------------------------------------------------------------------
// Setup dimensions of object inputs
// NaUnit anyway has (1:1:0) i-o-fb schema
NaNNPredictObjectUnit::NaNNPredictObjectUnit (unsigned nControlDim,
                                              unsigned nOutputDim)
:   nControlDepth(nControlDim),
    nObjectDepth(nOutputDim),
    NaUnit(1/* inputs */, 1/* outputs */, 0/* feedback */)
{
    if(nControlDim < 1)
        // Must be > 0
        throw(na_bad_value);
    pTotalInput = new NaReal[nControlDepth + nObjectDepth];
}

//---------------------------------------------------------------------------
// Destoy the object
NaNNPredictObjectUnit::~NaNNPredictObjectUnit ()
{
    delete[] pTotalInput;
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaNNPredictObjectUnit::Reset ()
{
    unsigned    i;
    unsigned    inpdim = nControlDepth + nObjectDepth;

    // Make zero pre-time error and noise estimation
    for(i = 0; i < inpdim; ++i)
        pTotalInput[i] = 0.0;

    // Check for assigned neural network kernel
    if(NULL == pNN){
        NaPrintLog("NN is not defined for NN predictive object!");
        throw(na_null_pointer);
    }

    // Check NN input dim and assigned for controller
    if(pNN->InputDim() != inpdim){
        NaPrintLog("NN and predictive object don't have "
                   "the same input dimension!");
        throw(na_size_mismatch);
    }
}

//---------------------------------------------------------------------------
// ATTENTION!  It's guaranteed that this->Function() will be called
// only once for each time step.  Index of the time step and current
// time can be got from Timer().

// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p), where x - actual
// control signal.  Previous real object output is set by special
// function.
void
NaNNPredictObjectUnit::Function (NaReal* x, NaReal* y)
{
    int     i;

    // If some values of set point are not really known (pretime period),
    // propagate real object one
    if(0 == Timer().CurrentIndex())
        for(i = 0; i < (int)nControlDepth; ++i)
            pTotalInput[i] = (nControlDepth > 0)
                ? pTotalInput[nControlDepth] : 0.0;
    else // Simply assign current control signal
        pTotalInput[0] = x[0];

    // Produce NN output
    pNN->Function(pTotalInput, y);

    // Shift backward
    for(i = nControlDepth - 1; i > 1; --i)
        pTotalInput[i] = pTotalInput[i - 1];

    for(i = nObjectDepth - 1; i > 1; --i)
        pTotalInput[nControlDepth + i] =
            pTotalInput[nControlDepth + i - 1];
}

//---------------------------------------------------------------------------
// Set real object output
void
NaNNPredictObjectUnit::SetObjectOutput (NaReal y)
{
    if(nControlDepth > 0)
        pTotalInput[nControlDepth] = y;
}

//---------------------------------------------------------------------------

