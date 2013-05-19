/* NaNNCon.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaNNCon.h"

//---------------------------------------------------------------------------
// Setup dimensions of controller inputs
NaNNControllerUnit::NaNNControllerUnit (unsigned nSetPointDim,
                                        unsigned nErrorDim,
                                        unsigned nNoiseEstDim)
:   nSetPointDepth(nSetPointDim),
    nErrorDepth(nErrorDim),
    nNoiseEstDepth(nNoiseEstDim),
    NaUnit(1/* inputs */, 1/* output */, 0/* feedback */)
{
    if(nSetPointDim < 1)
        // Must be > 0
        throw(na_bad_value);
    pTotalInput = new NaReal[nSetPointDepth + nErrorDepth + nNoiseEstDepth];
}

//---------------------------------------------------------------------------
// Destoy the object
NaNNControllerUnit::~NaNNControllerUnit ()
{
    delete[] pTotalInput;
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void
NaNNControllerUnit::Reset ()
{
    unsigned    i;
    unsigned    inpdim = nSetPointDepth + nErrorDepth + nNoiseEstDepth;

    // Make zero pre-time error and noise estimation
    for(i = 0; i < inpdim; ++i)
        pTotalInput[i] = 0.0;

    // Check for assigned neural network kernel
    if(NULL == pNN){
        NaPrintLog("NN is not defined for NN controller!");
        throw(na_null_pointer);
    }

    // Check NN input dim and assigned for controller
    if(pNN->InputDim() != inpdim){
        NaPrintLog("NN(=%d) and controller(=%d) don't have "
                   "the same input dimension!", pNN->InputDim(), inpdim);
        throw(na_size_mismatch);
    }
}

//---------------------------------------------------------------------------
// ATTENTION!  It's guaranteed that this->Function() will be called
// only once for each time step.  Index of the time step and current
// time can be got from Timer().

// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p), where x=x(t) - setpoint
// Control error and noise estimation must be set externally
void
NaNNControllerUnit::Function (NaReal* x, NaReal* y)
{
    int     i;

    // If some values of set point are not really known (pretime period),
    // simply return 0 (not controller force at all)
    if(0 == Timer().CurrentIndex()){
        for(i = 0; i < (int)nSetPointDepth; ++i)
            pTotalInput[i] = x[0];
        *y = 0.0;
        return;
    }
    else // Simply assign current set point
        pTotalInput[0] = x[0];

#ifdef NNCON_DEBUG
    unsigned    j;

    NaPrintLog("@@@ NNController\n");

    // input vector is x..e..n
    for(j = 0; j < nSetPointDepth; ++j){
        NaPrintLog("    x[%u] = %g\n", j, pTotalInput[j]);
    }
    for(j = nSetPointDepth; j < nSetPointDepth + nErrorDepth; ++j){
        NaPrintLog("    e[%u] = %g\n", j, pTotalInput[j]);
    }
    for(j = nSetPointDepth + nErrorDepth;
        j < nSetPointDepth + nErrorDepth + nNoiseEstDepth;
        ++j){
        NaPrintLog("    n[%u] = %g\n", j, pTotalInput[j]);
    }
#endif // NNCON_DEBUG

    // Produce NN output
    pNN->Function(pTotalInput, y);

#ifdef NNCON_DEBUG
    NaPrintLog("    nn contr output = %g\n", *y);
#endif // NNCON_DEBUG

    // Shift backward
    for(i = nSetPointDepth - 1; i > 1; --i)
        pTotalInput[i] = pTotalInput[i - 1];

    for(i = nErrorDepth - 1; i > 1; --i)
        pTotalInput[nSetPointDepth + i] =
            pTotalInput[nSetPointDepth + i - 1];

    for(i = nNoiseEstDepth; i > 1; --i)
        pTotalInput[nSetPointDepth + nErrorDepth + i] =
            pTotalInput[nSetPointDepth + nErrorDepth + i - 1];
}

//---------------------------------------------------------------------------
// Set control error
void
NaNNControllerUnit::SetErrorInput (NaReal e)
{
    if(nErrorDepth > 0)
        pTotalInput[nSetPointDepth] = e;
}

//---------------------------------------------------------------------------
// Set noise estimation
void
NaNNControllerUnit::SetNoiseEstInput (NaReal n)
{
    if(nNoiseEstDepth > 0)
        pTotalInput[nSetPointDepth + nErrorDepth] = n;
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
