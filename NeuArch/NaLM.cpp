/* NaStdBPE.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include "NaLogFil.h"
#include "NaLM.h"
#define __matrix_h
// Initialize the object on the basic of preset nd
void
NaLM::Init ()
{
    unsigned    /*iInput, iNeuron, */iLayer;
    I_LM=0;
    // Assign dimensions
    //назначение размеров
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
    {
        psWeight[iLayer].new_dim(nd.Neurons(iLayer), nd.Inputs(iLayer));
        psBias[iLayer].new_dim(nd.Neurons(iLayer));
        dWeight[iLayer].new_dim(nd.Neurons(iLayer), nd.Inputs(iLayer));
        dBias[iLayer].new_dim(nd.Neurons(iLayer));
        delta[iLayer].new_dim(nd.Neurons(iLayer));
        delta_prev[iLayer].new_dim(nd.Neurons(iLayer));
        //
        I_LM+=nd.Inputs(iLayer)*nd.Neurons(iLayer)+nd.Neurons(iLayer);
        COR_LM[iLayer].new_dim(nd.Neurons(iLayer));
    }
    dW_LM.new_dim(I_LM);
    dW_LM.init_zero();
    // Initalize all values by zero
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
    {
        psWeight[iLayer].init_zero();
        psBias[iLayer].init_zero();
        dWeight[iLayer].init_zero();
        dBias[iLayer].init_zero();
        delta[iLayer].init_zero();
        delta_prev[iLayer].init_zero();
//LM
        COR_LM[iLayer].init_zero();
    }
}
void NaLM::SetNSamples_LM(int n)
{
    nSamples_LM=n;
    J_LM.new_dim(nSamples_LM*nd.nOutNeurons,I_LM);
    Errors_LM.new_dim(nSamples_LM*nd.nOutNeurons);
    J_LM.init_zero();
    Errors_LM.init_zero();
}
void NaLM::GetErrors(unsigned nSamples,NaReal *pTar,NaReal *pOut)
{
    for(unsigned i=0; i<nd.nOutNeurons; ++i)
    {
        Errors_LM[nSamples*nd.nOutNeurons+i]=(pTar[i] - pOut[i]);
    }
}
void NaLM::Get_COR(NaReal *pTar,NaReal *pOut,unsigned nOUT)
{
    // вычисление  поправок для выходного слоя
    for(unsigned iNeuron=0; iNeuron<nd.nOutNeurons; ++iNeuron)
    {
        if(iNeuron!=nOUT)
        {
            COR_LM[nd.OutputLayer()][iNeuron]=0;

        }
        else
        {
            if(nd.eLastActFunc== afkLinear)
            {


                COR_LM[nd.OutputLayer()][iNeuron]=-nn().DerivActFunc(nd.OutputLayer(), nn().Znet[nd.OutputLayer()][iNeuron]);
            }
            else
            {
                COR_LM[nd.OutputLayer()][iNeuron]=-2*nn().DerivActFunc(nd.OutputLayer(), nn().Znet[nd.OutputLayer()][iNeuron]);
            }
        }
    }
    //вычисление поправок скрытых слоев
    for(int iLayer=nd.OutputLayer()-1; iLayer>=0; --iLayer)
    {
        for(unsigned iNeuron=0; iNeuron<nd.Neurons(iLayer); ++iNeuron)
        {
            if(iLayer==nd.OutputLayer()-1)// поправки для последнего скрытого слоя
            {
                NaReal buf=0;
                for(unsigned iNeuronOut=0; iNeuronOut<nd.nOutNeurons; ++iNeuronOut)
                {
                    buf+=COR_LM[nd.OutputLayer()][iNeuronOut]*nn().weight[nd.OutputLayer()](iNeuronOut,iNeuron);
                }
                COR_LM[iLayer][iNeuron]=nn().DerivActFunc(iLayer, nn().Znet[iLayer][iNeuron])*buf;
            }
            else // поправки доя остальных скрытых слоев
            {
                NaReal buf=0;
                for(unsigned iNeuronPrev=0; iNeuronPrev<nd.Neurons(iLayer+1); ++iNeuronPrev)
                {
                    buf+=COR_LM[iLayer+1][iNeuronPrev]*nn().weight[iLayer+1](iNeuronPrev,iNeuron);
                }
                COR_LM[iLayer][iNeuron]=nn().DerivActFunc(iLayer, nn().Znet[iLayer][iNeuron])*buf;

            }
        }
    }
}

void NaLM::GetJ(unsigned i,NaReal *pIn,NaReal *pTar,NaReal *pOut)// заполнение матрицы якобиана построчно
{
    for(unsigned k=0; k<nd.nOutNeurons; ++k)
    {
        Get_COR(pTar,pOut,k);
        unsigned I_J=0;
        for(int iLayer=nd.OutputLayer(); iLayer>=0; --iLayer)
        {
            if(iLayer!=0)
            {
                for(unsigned iNeuron=0; iNeuron<nn().Neurons(iLayer); ++iNeuron)
                {
                    J_LM[i*nd.nOutNeurons+k][I_J]=COR_LM[iLayer][iNeuron];
                    I_J+=1;
                    for(unsigned iNeuron_prev=0; iNeuron_prev<nn().Neurons(iLayer-1); ++iNeuron_prev)
                    {
                        J_LM[i*nd.nOutNeurons+k][I_J]=COR_LM[iLayer][iNeuron]*nn().Yout[iLayer-1][iNeuron_prev];
                        I_J+=1;
                    }
                }
            }
            else
            {
                for(unsigned iNeuron=0; iNeuron<nn().Neurons(iLayer); ++iNeuron)
                {
                    J_LM[i*nd.nOutNeurons+k][I_J]=COR_LM[iLayer][iNeuron];
                    I_J+=1;
                    for(unsigned iInput=0; iInput<nn().Inputs(iLayer); ++iInput)
                    {
                        J_LM[i*nd.nOutNeurons+k][I_J]=COR_LM[iLayer][iNeuron]*pIn[iInput];
                        I_J+=1;
                    }
                }

            }
        }
        for(unsigned iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
        {
            COR_LM[iLayer].init_zero();
        }
    }
}
void NaLM::Init_Zero_LM()
{
    J_LM.init_zero();
    Errors_LM.init_zero();
    for(unsigned iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
    {
        COR_LM[iLayer].init_zero();
    }
    dW_LM.init_zero();
}
void NaLM::Get_dW(NaReal ALPHA_LM)
{
    NaMatrix JT_LM;
    JT_LM.new_dim(I_LM,nSamples_LM*nd.nOutNeurons);
    J_LM.trans(JT_LM);
    NaMatrix JTJ_LM;
    JTJ_LM.new_dim(I_LM,I_LM);
    JT_LM.multiply(J_LM,JTJ_LM);
    for(unsigned i=0; i<I_LM; ++i)
    {
        JTJ_LM[i][i]+=ALPHA_LM;
    }
    NaMatrix JTJ_INV_LM;
    JTJ_INV_LM.new_dim(I_LM,I_LM);
    JTJ_LM.inv(JTJ_INV_LM);
    NaMatrix TOTAL_LM;
    TOTAL_LM.new_dim(I_LM,nSamples_LM*nd.nOutNeurons);
    J_LM.trans(JT_LM);
    JTJ_INV_LM.multiply(JT_LM,TOTAL_LM);
    TOTAL_LM.multiply(Errors_LM,dW_LM);
}
void NaLM::UpdateNN_LM()
{
    unsigned I_J=0;
    for(int iLayer=nd.OutputLayer(); iLayer>=0; --iLayer)
    {
        if(iLayer!=0)
        {
            for(unsigned iNeuron=0; iNeuron<nn().Neurons(iLayer); ++iNeuron)
            {
                nn().bias[iLayer][iNeuron]-=dW_LM[I_J];
                I_J+=1;
                for(unsigned iNeuron_prev=0; iNeuron_prev<nn().Neurons(iLayer-1); ++iNeuron_prev)
                {
                    nn().weight[iLayer][iNeuron][iNeuron_prev]-=dW_LM[I_J];
                    I_J+=1;
                }
            }
        }
        else
        {
            for(unsigned iNeuron=0; iNeuron<nn().Neurons(iLayer); ++iNeuron)
            {
                nn().bias[iLayer][iNeuron]-=dW_LM[I_J];
                I_J+=1;
                for(unsigned iInput=0; iInput<nd.nInputsNumber; ++iInput)
                {
                    nn().weight[iLayer][iNeuron][iInput]-=dW_LM[I_J];
                    I_J+=1;
                }
            }

        }
    }
}
void NaLM::DegradeNN_LM()
{
    unsigned I_J=0;
    for(int iLayer=nd.OutputLayer(); iLayer>=0; --iLayer)
    {
        if(iLayer!=0)
        {
            for(unsigned iNeuron=0; iNeuron<nn().Neurons(iLayer); ++iNeuron)
            {
                nn().bias[iLayer][iNeuron]+=dW_LM[I_J];
                I_J+=1;
                for(unsigned iNeuron_prev=0; iNeuron_prev<nn().Neurons(iLayer-1); ++iNeuron_prev)
                {
                    nn().weight[iLayer][iNeuron][iNeuron_prev]+=dW_LM[I_J];
                    I_J+=1;
                }
            }
        }
        else
        {
            for(unsigned iNeuron=0; iNeuron<nn().Neurons(iLayer); ++iNeuron)
            {
                nn().bias[iLayer][iNeuron]+=dW_LM[I_J];
                I_J+=1;
                for(unsigned iInput=0; iInput<nd.nInputsNumber; ++iInput)
                {
                    nn().weight[iLayer][iNeuron][iInput]+=dW_LM[I_J];
                    I_J+=1;
                }
            }

        }
    }
}
//-------------------------------------------------------------------------
// Prepare object to work with given neural network architecture.
// Most operations are disabled until attaching neural network data.
NaLM::NaLM (NaNeuralNetDescr& rND)
    : nd(rND), nDebugLvl(0)
{
    Init();
}
//---------------------------------------------------------------------------
// Prepare object to work with given neural network passed by
// reference.  Actually a pointer to the NN object will be stored.
NaLM::NaLM (NaNNUnit& rNN)
    : nd(rNN.descr), nDebugLvl(0)
{
    Init();
    AttachNN(&rNN);
}
//---------------------------------------------------------------------------
NaLM::~NaLM ()
{
    // Dummy
}

//---------------------------------------------------------------------------
// Return reference to the neural network architecture
const NaNeuralNetDescr&
NaLM::Arch () const
{
    return nd;
}


//---------------------------------------------------------------------------
// Return pointer to the current neural network data or NULL if no
// network data is attached
NaNNUnit*
NaLM::NN () const
{
    if(nnStack.count() == 0)
        return NULL;
    return nnStack(0);
}


//---------------------------------------------------------------------------
// Return reference to the current neural network data or throw
// exception na_bad_value if no network data is attached
NaNNUnit&
NaLM::nn () const
{
    NaNNUnit	*pNN = NN();
    if(NULL == pNN)
        throw(na_bad_value);
    return *pNN;
}


//---------------------------------------------------------------------------
// Attach neural network data to be used in further operations:
// delta calculations and application.  Several attached neural
// networks must be detached in order.
void
NaLM::AttachNN (NaNNUnit* pNN)
{
    if(NULL == pNN)
        throw(na_null_pointer);

    // check the same architecture
    if(nd != pNN->descr)
        throw(na_not_compatible);

    // push
    nnStack.addl(pNN);
}


//---------------------------------------------------------------------------
// Detach the last attached neural network
void
NaLM::DetachNN ()
{
    // pop
    if(nnStack.count() > 0)
        nnStack.remove(0);
}


//---------------------------------------------------------------------------
// Reset computed changes: delta of weights
// отменяет поправки и dW
void
NaLM::Reset ()
{
    unsigned    iLayer;
    // Reset dWeight and dBias for the next epoch
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
    {
        dWeight[iLayer].init_zero();
        dBias[iLayer].init_zero();
    }
}

//---------------------------------------------------------------------------
// Update the current neural network parameters on the basis of
// computed changes.
void
NaLM::UpdateNN ()
{
    if(NULL == NN())
        // Disabled until the first valid attach
        return;

    unsigned    iInput, iNeuron, iLayer;
    bool        bProhibitBiasChange = DontTouchBias();

    NaMatrix    old_w;
    NaVector    old_b;

    // Apply dWeight and dBias to nn.weight and nn.bias
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
    {

        if(nDebugLvl >= 1)
        {
            NaPrintLog("=== Update layer[%u] ===\n", iLayer);
            old_w = nn().weight[iLayer];
            old_b = nn().bias[iLayer];
        }

        for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron)
        {
            if(nDebugLvl >= 1)
                NaPrintLog("    Neuron[%u]:\n", iNeuron);

            for(iInput = 0; iInput < nd.Inputs(iLayer); ++iInput)
            {
                psWeight[iLayer][iNeuron][iInput] =
                    dWeight[iLayer](iNeuron, iInput);
                nn().weight[iLayer][iNeuron][iInput] +=
                    dWeight[iLayer](iNeuron, iInput);

                if(nDebugLvl >= 1)
                    NaPrintLog("    * W[%u]= %g\t%+g\t--> %g\n",
                               iInput, old_w(iNeuron, iInput),
                               dWeight[iLayer](iNeuron, iInput),
                               nn().weight[iLayer](iNeuron, iInput));
            }
            psBias[iLayer][iNeuron] = dBias[iLayer][iNeuron];

            if(!bProhibitBiasChange)
            {
                nn().bias[iLayer][iNeuron] += dBias[iLayer][iNeuron];

                if(nDebugLvl >= 1)
                    NaPrintLog("    * B= %g\t%+g\t--> %g\n",
                               old_b[iNeuron], dBias[iLayer][iNeuron],
                               nn().bias[iLayer][iNeuron]);
            }
            else
            {
                if(nDebugLvl >= 1)
                    NaPrintLog("    * prohibited bias change: B= %g\n",
                               nn().bias[iLayer][iNeuron]);
            }
        }
    }

    // Reset dWeight and dBias for the next epoch
    Reset();
}

//---------------------------------------------------------------------------
// Compute delta weights based on common delta (see DeltaRule)
void
NaLM::ApplyDelta (unsigned iLayer)
{
    unsigned    iNeuron, iInput;

    // Apply delta weight and delta bias
    NaMatrix    old_dW;
    NaVector    old_dB;

    if(nDebugLvl >= 2)
    {
        NaPrintLog("--- Applied delta for weight and bias [%u]-layer ---\n",
                   iLayer);
        old_dW = dWeight[iLayer];
        old_dB = dBias[iLayer];
    }

    for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron)
    {
        if(nDebugLvl >= 2)
            NaPrintLog("    Neuron[%u]:\n", iNeuron);

        for(iInput = 0; iInput < nd.Inputs(iLayer); ++iInput)
        {
            dWeight[iLayer][iNeuron][iInput] +=
                DeltaWeight(iLayer, iNeuron, iInput);

            if(nDebugLvl >= 2)
                NaPrintLog("    * dW[%u]= %g\t--> %g\n",
                           iInput, old_dW(iNeuron, iInput),
                           dWeight[iLayer](iNeuron, iInput));
        }
        dBias[iLayer][iNeuron] += DeltaBias(iLayer, iNeuron);

        if(nDebugLvl >= 2)
            NaPrintLog("    * dB= %g\t--> %g\n",
                       old_dB[iNeuron], dBias[iLayer][iNeuron]);
    }
}


//---------------------------------------------------------------------------
// Delta rule for the last layer.
// Ytarg is desired vector needs to be compared with Yout
// of the output layer or error value already computed..
// If bError==true then Ytarg means error ready to use without Yout.
// If bError==false then Ytarg means Ydes to compare with Yout.
void
NaLM::DeltaRule (const NaReal* Ytarg, bool bError)
{
    if(NULL == NN())
        // Disabled until the first valid attach
        return;

    NaReal      fError;
    unsigned    iNeuron, iLayer = nd.OutputLayer();

    if(nDebugLvl >= 2)
        NaPrintLog("+++ Standard delta rule [%u]-output +++\n", iLayer);

    if(NULL == Ytarg)
        throw(na_null_pointer);

    // Compute common delta
    for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron)
    {
        // Compute error by comparing values Ydes and Yout
        if(bError)
        {
            // I still don't know whether precomputed error needs to be
            // scaled the same manner as desired value or not.  Anyway
            // I'm sure this bit of code (with disabled error scaling)
            // is good enough to work in real application with outputs
            // without scaling.  Don't scale NN outputs until you will
            // be sure about proper way to scale precomputed error too!
#if 0
            // Error must not be scaled
            fError = Ytarg[iNeuron];
#else
            // Error must be scaled to standard range too
            fError = nn().ScaleData(nn().OutputScaler, nn().StdOutputRange,
                                    Ytarg[iNeuron], iNeuron);
#endif
            if(nDebugLvl >= 3)
                NaPrintLog("    ~ precomp.error[%d]= %g\n", iNeuron, fError);
        }
        else
        {
            NaReal  Ydes_i;

            // Scale Ydes to standard range [-1,1]
            Ydes_i= nn().ScaleData(nn().OutputScaler, nn().StdOutputRange,
                                   Ytarg[iNeuron], iNeuron);

            fError = nn().Yout[iLayer][iNeuron] - Ydes_i;
            if(nDebugLvl >= 2)
                NaPrintLog("    ~ error[%d]= %g\n", iNeuron, fError);
        }

        delta_prev[iLayer][iNeuron] = delta[iLayer][iNeuron];
        delta[iLayer][iNeuron] =
            fError * nn().DerivActFunc(iLayer, nn().Znet[iLayer][iNeuron]);

        if(nDebugLvl >= 2)
            NaPrintLog("    * delta[%u]= %g\n", iNeuron, delta[iLayer][iNeuron]);
    }

    // Apply common delta - accumulate it in dWeight and dBias
    ApplyDelta(iLayer);
}


//---------------------------------------------------------------------------
// Part of delta rule for the hidden layer
// Computes sum of products outcoming weights and target deltas
// on given layer and for given input
NaReal
NaLM::PartOfDeltaRule (unsigned iPrevLayer, unsigned iInput)
{
    if(NULL == NN())
        // Disabled until the first valid attach
        return 0.0;

    unsigned    iPrevNeuron;
    NaReal      fSum = 0.;

    for(iPrevNeuron = 0; iPrevNeuron < nd.Neurons(iPrevLayer); ++iPrevNeuron)
    {

        if(nDebugLvl >= 2)
            NaPrintLog("    ** [%u]: prev_delta= %g,  weight= %g\n",
                       iPrevNeuron, delta[iPrevLayer][iPrevNeuron],
                       nn().weight[iPrevLayer](iPrevNeuron, iInput));

        fSum += delta[iPrevLayer][iPrevNeuron] *
                nn().weight[iPrevLayer](iPrevNeuron, iInput);
    }

    return fSum;
}


//---------------------------------------------------------------------------
// Delta rule for the hidden layer
// iLayer - index of target layer delta computation on the basis of previous
// layer's delta and linked weights.  Usually iPrevLayer=iLayer+1.
void
NaLM::DeltaRule (unsigned iLayer, unsigned iPrevLayer)
{
    if(NULL == NN())
        // Disabled until the first valid attach
        return;

    unsigned    iNeuron;

    if(nDebugLvl >= 2)
        NaPrintLog("+++ Standard delta rule [%u]-hidden [%u]-previous +++\n",
                   iLayer, iPrevLayer);

    // For each neuron of the current layer...
    for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron)
    {

        // Store delta for future usage...
        delta_prev[iLayer][iNeuron] = delta[iLayer][iNeuron];
        // Initialize delta...
        delta[iLayer][iNeuron] = PartOfDeltaRule(iPrevLayer, iNeuron);

        if(nDebugLvl >= 2)
            NaPrintLog("    * sum delta= %g,  Znet= %g,  Deriv= %g\n",
                       delta[iLayer][iNeuron], nn().Znet[iLayer][iNeuron],
                       nn().DerivActFunc(iLayer, nn().Znet[iLayer][iNeuron]));

        // Mulitply delta by derivation of the activation function
        delta[iLayer][iNeuron] *=
            nn().DerivActFunc(iLayer, nn().Znet[iLayer][iNeuron]);

        if(nDebugLvl >= 2)
            NaPrintLog("    * delta[%u]= %g\n", iNeuron, delta[iLayer][iNeuron]);
    }

    // Apply common delta - accumulate it in dWeight and dBias
    ApplyDelta(iLayer);
}


//---------------------------------------------------------------------------
// Compute delta of exact w[i,j,k]
NaReal
NaLM::DeltaWeight (unsigned iLayer, unsigned iNeuron,
                   unsigned iInput)
{
//
    if(NULL == NN())
        // Disabled until the first valid attach
        return 0.0;


    return - nn().Xinp(iLayer)(iInput) *
           delta[iLayer][iNeuron] * LearningRate(iLayer) +
           Momentum(iLayer) * psWeight[iLayer][iNeuron][iInput];

}


//---------------------------------------------------------------------------
// Compute delta of exact b[i,j]
NaReal
NaLM::DeltaBias (unsigned iLayer, unsigned iNeuron)
{
    return - delta[iLayer][iNeuron] * LearningRate(iLayer) +
           Momentum(iLayer) * psBias[iLayer][iNeuron];
}


//---------------------------------------------------------------------------
// Return true if there is a need to prohibit bias change.
bool
NaLM::DontTouchBias ()
{
    bool		bAllZero = true;
    unsigned	iNeuron, iLayer;

    // Find out zero biases
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer)
    {
        for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron)
        {
            if(0.0 != nn().bias[iLayer][iNeuron])
            {
                bAllZero = false;
                break;
            }
        }
        if(!bAllZero)
            break;
    }

    return bAllZero;
}


//---------------------------------------------------------------------------
// Learning rate coefficient for the given layer
NaReal
NaLM::LearningRate (unsigned iLayer) const
{
    if(afkLinear == nd.eLastActFunc &&
            nd.OutputLayer() == iLayer)
    {
        // Last layer -> do special (usually very slow) learning
        return eta_output;
    }
    return NaLMParams::LearningRate(iLayer);
}


//---------------------------------------------------------------------------
// Set flag of debugging messages
void
NaLM::SetDebugLevel (int iDebug)
{
    nDebugLvl = iDebug;
}


//---------------------------------------------------------------------------
NaLMParams::NaLMParams ()
    : eta(0.05), eta_output(0.001), alpha(0.0)
{
    // Dummy
}

//---------------------------------------------------------------------------
// Assignment of the object
NaLMParams&
NaLMParams::operator= (const NaLMParams& r)
{
    eta = r.eta;
    eta_output = r.eta_output;
    alpha = r.alpha;
    return *this;
}

//---------------------------------------------------------------------------
// Learning rate coefficient for the given layer
NaReal
NaLMParams::LearningRate (unsigned iLayer) const
{
    return eta;
}

//---------------------------------------------------------------------------
// Momentum (inertia coefficient) for the given layer
NaReal
NaLMParams::Momentum (unsigned iLayer) const
{
    return alpha;
}

//---------------------------------------------------------------------------
void
NaLMParams::PrintLog () const
{
    NaPrintLog("StdBackProp parameters: learning rate %g,  momentum %g\n",
               eta, alpha);
}


//---------------------------------------------------------------------------
