/* NaStdBPE.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include "NaLogFil.h"
#include "NaStdBPE.h"

//---------------------------------------------------------------------------
// Initialize the object on the basic of preset nd
void
NaStdBackProp::Init ()
{
    unsigned    /*iInput, iNeuron, */iLayer;

    // Assign dimensions
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){
        psWeight[iLayer].new_dim(nd.Neurons(iLayer), nd.Inputs(iLayer));
        psBias[iLayer].new_dim(nd.Neurons(iLayer));
        dWeight[iLayer].new_dim(nd.Neurons(iLayer), nd.Inputs(iLayer));
        dBias[iLayer].new_dim(nd.Neurons(iLayer));
        delta[iLayer].new_dim(nd.Neurons(iLayer));
        delta_prev[iLayer].new_dim(nd.Neurons(iLayer));
    }

    // Initalize all values by zero
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){
        psWeight[iLayer].init_zero();
        psBias[iLayer].init_zero();
        dWeight[iLayer].init_zero();
        dBias[iLayer].init_zero();
        delta[iLayer].init_zero();
        delta_prev[iLayer].init_zero();
    }
}


//---------------------------------------------------------------------------
// Prepare object to work with given neural network architecture.
// Most operations are disabled until attaching neural network data.
NaStdBackProp::NaStdBackProp (NaNeuralNetDescr& rND)
    : nd(rND), nDebugLvl(0)
{
    Init();
}


//---------------------------------------------------------------------------
// Prepare object to work with given neural network passed by
// reference.  Actually a pointer to the NN object will be stored.
NaStdBackProp::NaStdBackProp (NaNNUnit& rNN)
    : nd(rNN.descr), nDebugLvl(0)
{
    Init();
    AttachNN(&rNN);
}


//---------------------------------------------------------------------------
NaStdBackProp::~NaStdBackProp ()
{
    // Dummy
}

//---------------------------------------------------------------------------
// Return reference to the neural network architecture
const NaNeuralNetDescr&
NaStdBackProp::Arch () const
{
    return nd;
}


//---------------------------------------------------------------------------
// Return pointer to the current neural network data or NULL if no
// network data is attached
NaNNUnit*
NaStdBackProp::NN () const
{
    if(nnStack.count() == 0)
	return NULL;
    return nnStack(0);
}


//---------------------------------------------------------------------------
// Return reference to the current neural network data or throw
// exception na_bad_value if no network data is attached
NaNNUnit&
NaStdBackProp::nn () const
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
NaStdBackProp::AttachNN (NaNNUnit* pNN)
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
NaStdBackProp::DetachNN ()
{
    // pop
    if(nnStack.count() > 0)
	nnStack.remove(0);
}


//---------------------------------------------------------------------------
// Reset computed changes: delta of weights
void
NaStdBackProp::Reset ()
{
    unsigned    iLayer;
    // Reset dWeight and dBias for the next epoch
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){
        dWeight[iLayer].init_zero();
        dBias[iLayer].init_zero();
    }
}

//---------------------------------------------------------------------------
// Update the current neural network parameters on the basis of
// computed changes.
void
NaStdBackProp::UpdateNN ()
{
    if(NULL == NN())
	// Disabled until the first valid attach
	return;

    unsigned    iInput, iNeuron, iLayer;
    bool        bProhibitBiasChange = DontTouchBias();

    NaMatrix    old_w;
    NaVector    old_b;

    // Apply dWeight and dBias to nn.weight and nn.bias
    for(iLayer = nd.InputLayer(); iLayer <= nd.OutputLayer(); ++iLayer){

	if(nDebugLvl >= 1) {
	    NaPrintLog("=== Update layer[%u] ===\n", iLayer);
	    old_w = nn().weight[iLayer];
	    old_b = nn().bias[iLayer];
	}

        for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron){
	    if(nDebugLvl >= 1)
		NaPrintLog("    Neuron[%u]:\n", iNeuron);

            for(iInput = 0; iInput < nd.Inputs(iLayer); ++iInput){
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

	    if(!bProhibitBiasChange){
		nn().bias[iLayer][iNeuron] += dBias[iLayer][iNeuron];

		if(nDebugLvl >= 1)
		    NaPrintLog("    * B= %g\t%+g\t--> %g\n",
			       old_b[iNeuron], dBias[iLayer][iNeuron],
			       nn().bias[iLayer][iNeuron]);
	    }else{
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
NaStdBackProp::ApplyDelta (unsigned iLayer)
{
    unsigned    iNeuron, iInput;

    // Apply delta weight and delta bias
    NaMatrix    old_dW;
    NaVector    old_dB;

    if(nDebugLvl >= 2) {
	NaPrintLog("--- Applied delta for weight and bias [%u]-layer ---\n",
		   iLayer);
	old_dW = dWeight[iLayer];
	old_dB = dBias[iLayer];
    }

    for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron){
	if(nDebugLvl >= 2)
	    NaPrintLog("    Neuron[%u]:\n", iNeuron);

        for(iInput = 0; iInput < nd.Inputs(iLayer); ++iInput){
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
NaStdBackProp::DeltaRule (const NaReal* Ytarg, bool bError)
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
    for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron){
        // Compute error by comparing values Ydes and Yout
        if(bError){
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
	    nn().ScaleData(nn().OutputScaler, nn().StdOutputRange,
			   &(Ytarg[iNeuron]), &fError, 1, iNeuron);
#endif
	    if(nDebugLvl >= 3)
		NaPrintLog("    ~ precomp.error[%d]= %g\n", iNeuron, fError);
        }else{
            NaReal  Ydes_i;

            // Scale Ydes to standard range [-1,1]
	    nn().ScaleData(nn().OutputScaler, nn().StdOutputRange,
			   &(Ytarg[iNeuron]), &Ydes_i, 1, iNeuron);

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
NaStdBackProp::PartOfDeltaRule (unsigned iPrevLayer, unsigned iInput)
{
    if(NULL == NN())
	// Disabled until the first valid attach
	return 0.0;

    unsigned    iPrevNeuron;
    NaReal      fSum = 0.;

    for(iPrevNeuron = 0; iPrevNeuron < nd.Neurons(iPrevLayer); ++iPrevNeuron){

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
NaStdBackProp::DeltaRule (unsigned iLayer, unsigned iPrevLayer)
{
    if(NULL == NN())
	// Disabled until the first valid attach
	return;

    unsigned    iNeuron;

    if(nDebugLvl >= 2)
	NaPrintLog("+++ Standard delta rule [%u]-hidden [%u]-previous +++\n",
		   iLayer, iPrevLayer);

    // For each neuron of the current layer...
    for(iNeuron = 0; iNeuron < nd.Neurons(iLayer); ++iNeuron){

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
NaStdBackProp::DeltaWeight (unsigned iLayer, unsigned iNeuron,
                            unsigned iInput)
{
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
NaStdBackProp::DeltaBias (unsigned iLayer, unsigned iNeuron)
{
    return - delta[iLayer][iNeuron] * LearningRate(iLayer) +
        Momentum(iLayer) * psBias[iLayer][iNeuron];
}


//---------------------------------------------------------------------------
// Return true if there is a need to prohibit bias change.
bool
NaStdBackProp::DontTouchBias ()
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
NaStdBackProp::LearningRate (unsigned iLayer) const
{
    if(afkLinear == nd.eLastActFunc &&
       nd.OutputLayer() == iLayer){
       // Last layer -> do special (usually very slow) learning
       return eta_output;
    }
    return NaStdBackPropParams::LearningRate(iLayer);
}


//---------------------------------------------------------------------------
// Set flag of debugging messages
void
NaStdBackProp::SetDebugLevel (int iDebug)
{
    nDebugLvl = iDebug;
}


//---------------------------------------------------------------------------
NaStdBackPropParams::NaStdBackPropParams ()
: eta(0.05), eta_output(0.001), alpha(0.0)
{
    // Dummy
}

//---------------------------------------------------------------------------
// Assignment of the object
NaStdBackPropParams&
NaStdBackPropParams::operator= (const NaStdBackPropParams& r)
{
    eta = r.eta;
    eta_output = r.eta_output;
    alpha = r.alpha;
    return *this;
}

//---------------------------------------------------------------------------
// Learning rate coefficient for the given layer
NaReal
NaStdBackPropParams::LearningRate (unsigned iLayer) const
{
    return eta;
}

//---------------------------------------------------------------------------
// Momentum (inertia coefficient) for the given layer
NaReal
NaStdBackPropParams::Momentum (unsigned iLayer) const
{
    return alpha;
}

//---------------------------------------------------------------------------
void
NaStdBackPropParams::PrintLog () const
{
    NaPrintLog("StdBackProp parameters: learning rate %g,  momentum %g\n",
               eta, alpha);
}


//---------------------------------------------------------------------------
