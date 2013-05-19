/* NaQProp.cpp */
static char rcsid[] = "$Id$";
 //---------------------------------------------------------------------------
#include "NaLogFil.h"
#include "NaStdBPE.h"

#include "NaQProp.h"

//---------------------------------------------------------------------------
NaQuickPropParams::NaQuickPropParams ()
: mu(1.75)
{
    // Dummy
}

//---------------------------------------------------------------------------
// Assignment of the object
NaQuickPropParams&
NaQuickPropParams::operator= (const NaQuickPropParams& r)
{
    NaStdBackPropParams::operator=(r);
    mu = r.mu;
    return *this;
}

//---------------------------------------------------------------------------
void    NaQuickPropParams::PrintLog () const
{
    NaPrintLog("QuickProp parameters: learning rate %g (on output %g),  "
               "max growth factor %g\n", eta, eta_output, mu);
}

//---------------------------------------------------------------------------
NaQuickProp::NaQuickProp (NaNNUnit& rNN)
: NaStdBackProp(rNN)
{
    unsigned    /*iInput, iNeuron, */iLayer;

    // Assign dimensions
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){
        Xinp_prev[iLayer].new_dim(nd.Inputs(iLayer));
    }

    // Initalize all values by zero
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){
        Xinp_prev[iLayer].init_zero();
    }
}

//---------------------------------------------------------------------------
NaQuickProp::~NaQuickProp ()
{
    // Dummy
}

//---------------------------------------------------------------------------
// QuickProp step
NaReal          NaQuickProp::QuickPropDelta (unsigned iLayer,
                                             unsigned iNeuron,
                                             NaReal dWprev,
                                             NaReal fXinp,
                                             NaReal fXinp_prev)
{
    NaReal  dW = 0.0;
    NaReal  dEdW = fXinp * delta[iLayer][iNeuron];
    NaReal  dEdWprev = fXinp_prev * delta_prev[iLayer][iNeuron];
    NaReal  fLR = LearningRate(iLayer);
    NaReal  fMGF = MaxGrowthFactor(iLayer);
    NaReal  fRatio = fMGF / (1.0 + fMGF);

#ifdef QuickProp_DEBUG
    NaPrintLog("QuickPropDelta: (this=%p) LR=%g  MGF=%g\n"\
               "                dEdW(t)=%g  dEdW(t-1)=%g\n"\
               "                X(t)=%g  X(t-1)=%g\n"\
               "                dW(t-1)=%g",
               this, fLR, fMGF, dEdW, dEdWprev, fXinp, fXinp_prev, dWprev);
#endif // QuickProp_DEBUG

    // /* my */ is used only for preventing zero-division
    if(dWprev > 0){
        if(dEdW > 0){
            dW = - dEdW * fLR;
        }
        if(dEdW > fRatio * dEdWprev /* my */|| dEdWprev == dEdW){
            dW += fMGF * dWprev;
        }else{
            dW += dEdW * dWprev / (dEdWprev - dEdW);
        }
    }else if(dWprev < 0){
        if(dEdW < 0){
            dW = - dEdW * fLR;
        }
        if(dEdW < fRatio * dEdWprev /* my */|| dEdWprev == dEdW){
            dW += fMGF * dWprev;
        }else{
            dW += dEdW * dWprev / (dEdWprev - dEdW);
        }
    }else{
        dW = - dEdW * fLR;
    }

#ifdef QuickProp_DEBUG
    NaPrintLog(" --> dW(t)=%g\n", dW);
#endif // QuickProp_DEBUG

    return dW;
}

//---------------------------------------------------------------------------
// Compute delta of exact w[i,j,k]
NaReal  NaQuickProp::DeltaWeight (unsigned iLayer, unsigned iNeuron,
                                  unsigned iInput)
{
    return QuickPropDelta(iLayer, iNeuron, psWeight[iLayer][iNeuron][iInput],
                          nn().Xinp(iLayer)(iInput), Xinp_prev[iLayer][iInput]);
}

//---------------------------------------------------------------------------
// Compute delta of exact b[i,j]
NaReal  NaQuickProp::DeltaBias (unsigned iLayer, unsigned iNeuron)
{
    return QuickPropDelta(iLayer, iNeuron, psBias[iLayer][iNeuron],
                          /* bias! */ 1.0, 1.0);
}

//---------------------------------------------------------------------------
// Reset computed changes
void    NaQuickProp::Reset ()
{
  NaStdBackProp::Reset();
}

//---------------------------------------------------------------------------
// Update network parameters on the basis of computed changes
void    NaQuickProp::UpdateNN ()
{
    NaStdBackProp::UpdateNN();

    unsigned    iInput, /*iNeuron,*/ iLayer;

    // Store Xinp from previous step
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){
        for(iInput = 0; iInput < nd.Inputs(iLayer); ++iInput){
            Xinp_prev[iLayer][iInput] = nn().Xinp(iLayer)[iInput];
        }
    }
}

//---------------------------------------------------------------------------

