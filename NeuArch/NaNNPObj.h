//-*-C++-*-
/* NaNNPObj.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaNNPObjH
#define NaNNPObjH

#include <NaUnit.h>
#include <NaNNUnit.h>

//---------------------------------------------------------------------------
// NN predictive model of object.  Unit for runtime processing.
// Unit has (1;1;0) interface and delayed lines for previous real object
// output and control signal.
class NaNNPredictObjectUnit : public NaUnit
{
public:

    // Setup dimensions of object inputs
    // NaUnit anyway has (1:1:0) i-o-fb schema
    NaNNPredictObjectUnit (unsigned nControlDim,
                           unsigned nOutputDim);

    // Destoy the object
    virtual ~NaNNPredictObjectUnit ();

    // Reset operations, that must be done before new modelling
    // session will start.  It's guaranteed that this reset will be
    // called just after Timer().ResetTime().
    virtual void    Reset ();

    // ATTENTION!  It's guaranteed that this->Function() will be called
    // only once for each time step.  Index of the time step and current
    // time can be got from Timer().

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p), where x - actual
    // control signal.  Previous real object output is set by special
    // function.
    virtual void    Function (NaReal* x, NaReal* y);

    // Set real object output
    void            SetObjectOutput (NaReal y);

    // Base neural network
    NaNNUnit        *pNN;

private:

    // Input components' delay depth
    unsigned        nControlDepth, nObjectDepth;

    // Temporal vector for composed input
    NaReal          *pTotalInput;

};


//---------------------------------------------------------------------------
#endif
