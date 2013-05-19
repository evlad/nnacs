//-*-C++-*-
/* NaNNCon.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaNNConH
#define NaNNConH

#include <NaUnit.h>
#include <NaNNUnit.h>

//---------------------------------------------------------------------------
// NN controller unit for runtime processing.
// Unit has (1;1;0) interface and delayed lines for setpoint,
// control error (opt) and noise estimation (opt).
class NaNNControllerUnit : public NaUnit
{
public:

    // Setup dimensions of controller inputs
    // NaUnit anyway has (1:1:0) i-o-fb schema
    NaNNControllerUnit (unsigned nSetPointDim,
                        unsigned nErrorDim,
                        unsigned nNoiseEstDim);

    // Destoy the object
    virtual ~NaNNControllerUnit ();

    // Reset operations, that must be done before new modelling
    // session will start.  It's guaranteed that this reset will be
    // called just after Timer().ResetTime().
    virtual void    Reset ();

    // ATTENTION!  It's guaranteed that this->Function() will be called
    // only once for each time step.  Index of the time step and current
    // time can be got from Timer().

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p), where x - setpoint
    // Control error and noise estimation must be set externally
    virtual void    Function (NaReal* x, NaReal* y);

    // Set control error
    void            SetErrorInput (NaReal e);

    // Set noise estimation
    void            SetNoiseEstInput (NaReal n);

    // Base neural network
    NaNNUnit        *pNN;

private:

    // Input components' delay depth
    unsigned        nSetPointDepth, nErrorDepth, nNoiseEstDepth;

    // Temporal vector for composed input
    NaReal          *pTotalInput;

};

//---------------------------------------------------------------------------
#endif
