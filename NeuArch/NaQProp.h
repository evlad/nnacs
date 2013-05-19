//-*-C++-*-
/* NaQProp.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaQPropH
#define NaQPropH
//---------------------------------------------------------------------------
#include <NaVector.h>
#include <NaMatrix.h>
#include <NaNDescr.h>
#include <NaNNUnit.h>
#include <NaStdBPE.h>

//---------------------------------------------------------------------------
// Quickprop algorithm is as described in "Back propagation family album" by
// Jondarr c g h 2 {heart} Gibb, Department of Computing Macquarie University
// NSW 2109, Technical Report C/TR96-05, August, 1996 Version 1.0
//
// It's said that the algorithm "...comes from Veitch and Holmes[92] and is a
// slight modification to that described by Fahlman[24], but is presented in
// full." (Appendix B, p.51)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Learning coefficients
class NaQuickPropParams : virtual public NaStdBackPropParams
{
public:

    NaQuickPropParams ();

    NaReal      mu;         // Maximal growth factor (1.75 .. 2.25 recomm.)

    // Assignment of the object
    NaQuickPropParams&    operator= (const NaQuickPropParams& r);

    // Maximal growth factor as an upper limit of weights to prevent
    // oscillations and dropping in local minima.
    virtual NaReal  MaxGrowthFactor (unsigned iLayer) const{
        return mu;
    }

    // Turno-off momentum (inertia coefficient)
    virtual NaReal  Momentum (unsigned iLayer) const{
        return 0.0;
    }

    virtual void    PrintLog () const;

};


//---------------------------------------------------------------------------
// Class for standard backpropagation learning environment
class NaQuickProp :
    virtual public NaQuickPropParams,
    virtual public NaStdBackProp
{
public:/* methods */

    NaQuickProp (NaNNUnit& rNN);
    virtual ~NaQuickProp ();

    // Reset computed changes
    virtual void    Reset ();

    // Update network parameters on the basis of computed changes
    virtual void    UpdateNN ();

    // Compute delta of exact w[i,j,k]
    virtual NaReal  DeltaWeight (unsigned iLayer, unsigned iNeuron,
                                 unsigned iInput);

    // Compute delta of exact b[i,j]
    virtual NaReal  DeltaBias (unsigned iLayer, unsigned iNeuron);

    // QuickProp step
    NaReal  QuickPropDelta (unsigned iLayer, unsigned iNeuron, NaReal dWprev,
                            NaReal fXinp, NaReal fXinp_prev);

public:/* data */

    // Previous step input
    NaVector    Xinp_prev[NaMAX_HIDDEN+1];

};


//---------------------------------------------------------------------------
#endif
