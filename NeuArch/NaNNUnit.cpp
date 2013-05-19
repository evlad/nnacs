/* NaNNUnit.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>

#include "NaRandom.h"
#include "NaNNUnit.h"


//---------------------------------------------------------------------------
NaNNUnit::NaNNUnit ()
  : NaUnit(1, 1, 0), feedback(0), NaConfigPart("NeuralNet")
{
  /* nothing */
}


//---------------------------------------------------------------------------
NaNNUnit::NaNNUnit (const NaNeuralNetDescr& rDescr)
: NaUnit(rDescr.InputsNumber(), rDescr.nOutNeurons, rDescr.nFeedbackDepth),
  descr(rDescr), feedback(rDescr.nFeedbackDepth * rDescr.nOutNeurons),
  NaConfigPart("NeuralNet")
{
    AssignDescr(rDescr);
}


//---------------------------------------------------------------------------
NaNNUnit::NaNNUnit (const NaNNUnit& rUnit)
: NaUnit(rUnit.weight[0].dim_cols(),                    // input
         rUnit.weight[rUnit.Layers() - 1].dim_rows(),   // output neurons
         rUnit.feedback.dim() / rUnit.weight[rUnit.Layers() - 1].dim_rows()),
  descr(rUnit.descr), feedback(rUnit.feedback.dim()),
  NaConfigPart(rUnit)
{
  operator=(rUnit);
}


//---------------------------------------------------------------------------
NaNNUnit::~NaNNUnit ()
{
    // Dummy
}


//---------------------------------------------------------------------------
// Assign the object
NaNNUnit&
NaNNUnit::operator= (const NaNNUnit& rUnit)
{
    NaUnit::operator=((const NaUnit&)rUnit);
    AssignDescr(rUnit.descr);

    // Copy scaler
    InputScaler.min = rUnit.InputScaler.min;
    InputScaler.max = rUnit.InputScaler.max;
    OutputScaler.min = rUnit.OutputScaler.min;
    OutputScaler.max = rUnit.OutputScaler.max;

    // Copy weights, input and intermediate results
    feedback = rUnit.feedback;
    Xinp0 = rUnit.Xinp0;

    for(unsigned i = 0; i < Layers(); ++i) {
      weight[i] = rUnit.weight[i];
      bias[i] = rUnit.bias[i];
      Yout[i] = rUnit.Yout[i];
      Znet[i] = rUnit.Znet[i];
    }

    return *this;
}


//---------------------------------------------------------------------------
// Compare objects
bool
NaNNUnit::operator== (const NaNNUnit& rUnit) const
{
    if(descr != rUnit.descr)
	return false;

    for(unsigned i = 0; i < Layers(); ++i) {
	if(weight[i] != rUnit.weight[i] || bias[i] != rUnit.bias[i])
	    return false;
    }

    return true;
}


//---------------------------------------------------------------------------
// Assign new description
void
NaNNUnit::AssignDescr (const NaNeuralNetDescr& rDescr)
{
    // Change NaNNUnit properties
    descr = rDescr;

    // Change NaUnit properties
    Assign(descr.InputsNumber(), descr.nOutNeurons, descr.nFeedbackDepth);

    // Realloc arrays
    feedback.new_dim(rDescr.nFeedbackDepth * rDescr.nOutNeurons);

    unsigned    iLayer;
    unsigned    nX, nY;

#ifdef NNUnit_DEBUG
    NaPrintLog("NN creation:\n");
#endif // NNUnit_DEBUG

    // Assign dimensions
    nX = descr.InputsNumber() + feedback.dim();
    Xinp0.new_dim(nX);
    for(iLayer = 0; iLayer < Layers(); ++iLayer){
#ifdef NNUnit_DEBUG
        NaPrintLog("  * layer #%u: ", iLayer);
#endif // NNUnit_DEBUG
        if(iLayer < Layers() - 1)
            nY = descr.nHidNeurons[iLayer];
        else
            nY = descr.nOutNeurons;

#ifdef NNUnit_DEBUG
        NaPrintLog("inputs %u, neurons %u\n", nX, nY);
#endif // NNUnit_DEBUG

        weight[iLayer].new_dim(nY, nX);
        bias[iLayer].new_dim(nY);
        Yout[iLayer].new_dim(nY);
        Znet[iLayer].new_dim(nY);

        nX = nY;
    }

    // Define default input scaler [-1,1] <-> [-1,1]
    InputScaler.min.new_dim(InputDim());
    InputScaler.max.new_dim(InputDim());

    InputScaler.min.init_value(-1.0);
    InputScaler.max.init_value(1.0);

    StdInputRange.min.new_dim(InputDim());
    StdInputRange.max.new_dim(InputDim());

    StdInputRange.min.init_value(-1.0);
    StdInputRange.max.init_value(1.0);

    // Define default output scaler [-1,1] <-> [-1,1]
    OutputScaler.min.new_dim(OutputDim());
    OutputScaler.max.new_dim(OutputDim());

    OutputScaler.min.init_value(-1.0);
    OutputScaler.max.init_value(1.0);

    StdOutputRange.min.new_dim(OutputDim());
    StdOutputRange.max.new_dim(OutputDim());

    StdOutputRange.min.init_value(-1.0);
    StdOutputRange.max.init_value(1.0);

#if 0
    if(descr.eLastActFunc == afkLinear){
        unsigned    i, j, base = descr.nInputsNumber * descr.nInputsRepeat;
        // No squashing
        for(i = 0; i < descr.nOutputsRepeat; ++i){
            for(j = 0; j < descr.nOutNeurons; ++j){
                InputScaler.min[base + i * descr.nOutNeurons + j] = 0;
                InputScaler.max[base + i * descr.nOutNeurons + j] = 0;
                StdInputRange.min[base + i * descr.nOutNeurons + j] = 0;
                StdInputRange.max[base + i * descr.nOutNeurons + j] = 0;
            }
        }
        for(j = 0; j < descr.nOutNeurons; ++j){
            OutputScaler.min[j] = 0;
            OutputScaler.max[j] = 0;
            StdOutputRange.min[j] = 0;
            StdOutputRange.max[j] = 0;
        }
    }
#endif
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaNNUnit::Save (NaDataStream& ds)
{
    char        szBuf[80];
    unsigned    iLayer, iNeuron, iInput;

    descr.Save(ds);

    ds.PutComment("Neural network:");

    for(iLayer = InputLayer(); iLayer <= OutputLayer(); ++iLayer){
        sprintf(szBuf, "Layer #%u  (%u inputs, %u neurons)",
                iLayer, weight[iLayer].dim_cols(), weight[iLayer].dim_rows());
        ds.PutComment(szBuf);
        for(iNeuron = 0; iNeuron < weight[iLayer].dim_rows(); ++iNeuron){
            sprintf(szBuf, "Neuron #%u (bias / weights):", iNeuron);
            ds.PutF(szBuf, "%g", bias[iLayer](iNeuron));
            for(iInput = 0; iInput < weight[iLayer].dim_cols(); ++iInput){
                ds.PutF(NULL, "%g", weight[iLayer](iNeuron, iInput));
            }// for inputs of neuron
        }// for neurons of layer
    }// for layers

    ds.PutComment("Input scaler:");
    for(iInput = 0; iInput < (unsigned)InputDim(); ++iInput){
        ds.PutF("min max", "%g %g",
                InputScaler.min(iInput), InputScaler.max(iInput));
    }

    ds.PutComment("Output scaler:");
    for(iNeuron = 0; iNeuron < (unsigned)OutputDim(); ++iNeuron){
        ds.PutF("min max", "%g %g",
                OutputScaler.min(iNeuron), OutputScaler.max(iNeuron));
    }
}


//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaNNUnit::Load (NaDataStream& ds)
{
    unsigned    iLayer, iNeuron, iInput;

    NaNeuralNetDescr    rTmpDescr;
    rTmpDescr.Load(ds);
    AssignDescr(rTmpDescr);

    for(iLayer = InputLayer(); iLayer <= OutputLayer(); ++iLayer){
        for(iNeuron = 0; iNeuron < weight[iLayer].dim_rows(); ++iNeuron){
            ds.GetF("%lg", &bias[iLayer][iNeuron]);
            for(iInput = 0; iInput < weight[iLayer].dim_cols(); ++iInput){
                ds.GetF("%lg", &weight[iLayer][iNeuron][iInput]);
            }// for inputs of neuron
        }// for neurons of layer
    }// for layers

    for(iInput = 0; iInput < (unsigned)InputDim(); ++iInput){
        ds.GetF("%lg %lg",
                &InputScaler.min[iInput], &InputScaler.max[iInput]);
    }
    for(iNeuron = 0; iNeuron < (unsigned)OutputDim(); ++iNeuron){
        ds.GetF("%lg %lg",
                &OutputScaler.min[iNeuron], &OutputScaler.max[iNeuron]);
    }
}


//---------------------------------------------------------------------------
// Store neural net to the file
void
NaNNUnit::Save (const char* szFileName)
{
  NaConfigPart	*conf_list[] = { this };
  NaConfigFile	conf_file(";NeuCon NeuralNet", 1, 1, ".nn");
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.SaveToFile(szFileName);
}


//---------------------------------------------------------------------------
// Retrieve neural net from the file
void
NaNNUnit::Load (const char* szFileName)
{
  NaConfigPart	*conf_list[] = { this };
  NaConfigFile	conf_file(";NeuCon NeuralNet", 1, 1, ".nn");
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.LoadFromFile(szFileName);
}


//---------------------------------------------------------------------------
// Initialize weights and biases
void
NaNNUnit::Initialize ()
{
    unsigned    iLayer;
    float	fMax = 0.2;	/* default version */

    if(NULL != getenv("NA_WEIGHT_INIT_MAX"))
      {
	float	fUserMax = atof(getenv("NA_WEIGHT_INIT_MAX"));
	if(0 == fUserMax)
	  NaPrintLog("Zero value of NA_WEIGHT_INIT_MAX!\n");
	else if(fUserMax < 0)
	  NaPrintLog("Negative value of NA_WEIGHT_INIT_MAX!\n");
	else
	  fMax = fUserMax;
      }
    NaPrintLog("Network weight init in range %g..%g\n", -fMax, fMax);

    for(iLayer = 0; iLayer < Layers(); ++iLayer){
        weight[iLayer].init_random(-fMax, fMax);
        bias[iLayer].init_random(-fMax, fMax);
    }// for layers
}


//---------------------------------------------------------------------------
void
NaNNUnit::PrintLog () const
{
    unsigned    iLayer, iNeuron, iInput;

    NaPrintLog("NaNNUnit(this=%p):\n  includes ", this);
    descr.PrintLog();
    NaUnit::PrintLog();
    NaPrintLog("  network has the next tunable parameters:\n");

    for(iLayer = InputLayer(); iLayer <= OutputLayer(); ++iLayer){
        NaPrintLog("  * layer #%u (%u inputs, %u neurons)\n", iLayer,
                   weight[iLayer].dim_cols(),
                   weight[iLayer].dim_rows());
        for(iNeuron = 0; iNeuron < weight[iLayer].dim_rows(); ++iNeuron){
            NaPrintLog("    * neuron #%u (bias / weights)\n      %+7.3f /",
                       iNeuron, bias[iLayer](iNeuron));
            for(iInput = 0; iInput < weight[iLayer].dim_cols(); ++iInput){
                NaPrintLog("\t%+6.3f", weight[iLayer](iNeuron, iInput));
            }// for inputs of neuron
            NaPrintLog("\n");
        }// for neurons of layer
    }// for layers

    NaPrintLog("  defined scale for output layer:\n");

    for(iNeuron = 0; iNeuron < (unsigned)OutputDim(); ++iNeuron){
        NaPrintLog("    * neuron #%u\tmin=%g\tmax=%g\n", iNeuron,
                   OutputScaler.min(iNeuron), OutputScaler.max(iNeuron));
    }
}


//---------------------------------------------------------------------------
// Input of the layer
NaVector&
NaNNUnit::Xinp (unsigned iLayer)
{
    if(InputLayer() == iLayer)
        return Xinp0;
    return Yout[iLayer-1];
}


//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void
NaNNUnit::Reset ()
{
    // Dummy
}


//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaNNUnit::Function (NaReal* x, NaReal* y)
{
    unsigned    i, j;

    /***** Direct part *****/

#if defined(NNUnit_DEBUG) || defined(NNUnit_SCALE)
    NaPrintLog("NN function:\n");
#endif // NNUnit_DEBUG || NNUnit_SCALE

    // Store x for input layer...
    for(i = 0; i < InputDim(); ++i){
        Xinp0[i] = x[i];
#if defined(NNUnit_DEBUG) || defined(NNUnit_SCALE)
        NaPrintLog("  * direct   in[%u] = %g\n", i, Xinp0[i]);
#endif // NNUnit_DEBUG || NNUnit_SCALE
    }

    // Scale input vector
    ScaleData(InputScaler, StdInputRange, &Xinp0[0], &Xinp0[0],
              InputDim());

#if defined(NNUnit_DEBUG) || defined(NNUnit_SCALE)
    for(i = 0; i < InputDim(); ++i){
        NaPrintLog("  * scaled   in[%u] = %g\n", i, Xinp0[i]);
    }
#endif // NNUnit_DEBUG || NNUnit_SCALE

    // ...plus stored (and scaled) feedback values
    for(i = 0; i < feedback.dim(); ++i){
        Xinp0[InputDim() + i] = feedback[i];
#ifdef NNUnit_DEBUG
        NaPrintLog("  * feedback in[%u] = %g\n",
                   InputDim() + i, Xinp0[InputDim() + i]);
#endif // NNUnit_DEBUG
    }

    // Compute y=f(x, fb)
    unsigned    iLayer, iNeuron, iInput;

    for(iLayer = InputLayer(); iLayer <= OutputLayer(); ++iLayer){

#ifdef NNUnit_DEBUG
        NaPrintLog("  + Layer %u\n", iLayer);
#endif // NNUnit_DEBUG

        for(iNeuron = 0; iNeuron < Neurons(iLayer); ++iNeuron){

#ifdef NNUnit_DEBUG
            NaPrintLog("    + Neuron %u\n", iNeuron);
#endif // NNUnit_DEBUG

            Znet[iLayer][iNeuron] = bias[iLayer][iNeuron];  // sum

#ifdef NNUnit_DEBUG
            NaPrintLog("      ! Bias= %g\n", Znet[iLayer][iNeuron]);
#endif // NNUnit_DEBUG

            for(iInput = 0; iInput < Inputs(iLayer); ++iInput){
                Znet[iLayer][iNeuron] +=
                    Xinp(iLayer)(iInput) * weight[iLayer](iNeuron, iInput);
#ifdef NNUnit_DEBUG
                NaPrintLog("      !! Xinp= %g,  W= %g,  Znet= %g\n",
                           Xinp(iLayer)(iInput),
                           weight[iLayer](iNeuron, iInput),
                           Znet[iLayer][iNeuron]);
#endif // NNUnit_DEBUG
            }// for inputs of the neuron

#ifdef NNUnit_DEBUG
            NaPrintLog("      ! Znet= %g\n", Znet[iLayer][iNeuron]);
#endif // NNUnit_DEBUG

            Yout[iLayer][iNeuron] = ActFunc(iLayer, Znet[iLayer][iNeuron]);

#ifdef NNUnit_DEBUG
            NaPrintLog("      ! Yout= %g\n", Yout[iLayer][iNeuron]);
#endif // NNUnit_DEBUG

        }// for neurons of the layer

    }// for layers

    // Store Yout of the last layer to output y
    for(i = 0; i < OutputDim(); ++i){
        y[i] = Yout[OutputLayer()][i];
    }

    // Check for feedback
    if(0 != FeedbackDepth()){

        /**** Feedback part ****/

        // Shift delayed feedback values
        for(i = 1; i < FeedbackDepth(); ++i)
            for(j = 0; j < OutputDim(); ++j)
                feedback[(i - 1) * OutputDim() + j] =
                    feedback[i * OutputDim() + j];

        // Store y to fb and scale fb to the standard input scale
        i = FeedbackDepth() - 1;
        ScaleData(StdOutputRange, StdInputRange,
                  y, &feedback[i * OutputDim()], OutputDim());
    }

#if defined(NNUnit_SCALE)
    for(i = 0; i < OutputDim(); ++i)
        NaPrintLog("  * scaled  out[%u] = %g\n", i, y[i]);
#endif // NNUnit_SCALE

    // Scale output vector
    ScaleData(StdOutputRange, OutputScaler, y, y, OutputDim());

#if defined(NNUnit_DEBUG) || defined(NNUnit_SCALE)
    // Print resulting output
    for(i = 0; i < OutputDim(); ++i)
        NaPrintLog("  * direct  out[%u] = %g\n", i, y[i]);
#endif // NNUnit_DEBUG || NNUnit_SCALE
}


//---------------------------------------------------------------------------
// Supply unit with feedback values on the first step of
// unit perfomance.  All needed feedback values are stored.
void
NaNNUnit::FeedbackValues (NaReal* fb)
{
    if(NULL == fb)
        throw(na_null_pointer);

    if(0 == FeedbackDepth())
        // No need for feedback values
        return;

#if 0
    unsigned    i, j;

    for(i = 0; i < FeedbackDepth(); ++i)
        for(j = 0; j < OutputDim(); ++j)
            feedback[i * OutputDim() + j] = fb[i * OutputDim() + j];
#endif

    // Scale output vector
    ScaleData(OutputScaler, StdInputRange, fb, &feedback[0],
              OutputDim() * FeedbackDepth());
}


//---------------------------------------------------------------------------
// Return number of paramter records (see layers)
unsigned
NaNNUnit::Layers () const
{
    return descr.nHidLayers + 1/* output */;
}


//---------------------------------------------------------------------------
// Return number of neurons in given layer
unsigned
NaNNUnit::Neurons (unsigned iLayer) const
{
    if(iLayer < Layers() - 1)
        return descr.nHidNeurons[iLayer];
    else if(iLayer == Layers() - 1)
        return descr.nOutNeurons;

    throw(na_out_of_range);

    /* make compiler happy */
    return 0;
}


//---------------------------------------------------------------------------
// Return number of inputs of given layer
unsigned
NaNNUnit::Inputs (unsigned iLayer) const
{
    if(iLayer == 0)
        return descr.InputsNumber();
    else if(iLayer <= Layers() - 1)
        return descr.nHidNeurons[iLayer - 1];

    throw(na_out_of_range);

    /* make compiler happy */
    return 0;
}


//---------------------------------------------------------------------------
// Index of the first avaiable (input) NN hidden layer
unsigned
NaNNUnit::InputLayer () const
{
    return 0;
}


//---------------------------------------------------------------------------
// Index of the last (output) NN layer
unsigned
NaNNUnit::OutputLayer () const
{
    return Layers() - 1;
}


//---------------------------------------------------------------------------
// Compute activation function for the given layer
NaReal
NaNNUnit::ActFunc (unsigned iLayer, NaReal z)
{
    NaReal  f;
    if(OutputLayer() == iLayer && descr.eLastActFunc == afkLinear){
        // Linear transfer
        f = z;
    }else{
         // Sigmoid
        f = tanh(z);
    }
    return f;
}


//---------------------------------------------------------------------------
// Compute derivation of activation function
NaReal
NaNNUnit::DerivActFunc (unsigned iLayer, NaReal z)
{
    NaReal  f;
    if(OutputLayer() == iLayer && descr.eLastActFunc == afkLinear){
        // Linear transfer derivation
        f = 1.;
    }
    else /* sigmoid */{
        f = ActFunc(iLayer, z);
        f = 0.5 * (1. - f * f);
    }
    return f;
}


//---------------------------------------------------------------------------
// Set output scale: [-1,1] -> [yMin,yMax]
void
NaNNUnit::SetOutputScale (const NaReal* yMin, const NaReal* yMax)
{
    if(NULL == yMin || NULL == yMax)
        throw(na_null_pointer);

    unsigned    i, j, base = descr.nInputsNumber * descr.nInputsRepeat;

    // Squashing
    for(i = 0; i < descr.nOutputsRepeat; ++i){
        for(j = 0; j < descr.nOutNeurons; ++j){
            InputScaler.min[base + i * descr.nOutNeurons + j] = yMin[j];
            InputScaler.max[base + i * descr.nOutNeurons + j] = yMax[j];
        }
    }
    for(j = 0; j < descr.nOutNeurons; ++j){
        OutputScaler.min[j] = yMin[j];
        OutputScaler.max[j] = yMax[j];
    }
}


//---------------------------------------------------------------------------
// Set input scale: [yMin,yMax] -> [-1,1]
void
NaNNUnit::SetInputScale (const NaReal* yMin, const NaReal* yMax)
{
    if(NULL == yMin || NULL == yMax)
        throw(na_null_pointer);

    unsigned    i, j;
    for(i = 0; i < descr.nInputsRepeat; ++i){
        for(j = 0; j < descr.nInputsNumber; ++j){
            InputScaler.min[i * descr.nInputsNumber + j] = yMin[j];
            InputScaler.max[i * descr.nInputsNumber + j] = yMax[j];
        }
    }
}


//---------------------------------------------------------------------------
// Scale given vector [iIndex..iIndex+nDim1]; maybe pSrcVect==pDstVect
void
NaNNUnit::ScaleData (const NaNNUnit::NaScaler& rSrcScaler,
                     const NaNNUnit::NaScaler& rDstScaler,
                     const NaReal* pSrcVect,
                     NaReal* pDstVect,
                     unsigned nDim,
		     unsigned iIndex) const
{
    if(NULL == pSrcVect || NULL == pDstVect)
        throw(na_null_pointer);

    unsigned    i;
    for(i = iIndex; i < nDim; ++i){
        NaReal  fDstDiff = rDstScaler.max(i) - rDstScaler.min(i);
        NaReal  fSrcDiff = rSrcScaler.max(i) - rSrcScaler.min(i);
	NaReal	fSrcVal = pSrcVect[i];

        // Preveint scaling if some difference is zero
        if(0 == fDstDiff || 0 == fSrcDiff)
	  {
	    pDstVect[i] = pSrcVect[i];
            fDstDiff = 1.;
            fSrcDiff = 1.;
	  }
	else
	  {
	    pDstVect[i] = rDstScaler.min(i)
	      + (pSrcVect[i] - rSrcScaler.min(i)) * fDstDiff / fSrcDiff;
	  }

#if defined(NNUnit_SCALE)
	NaPrintLog("ScaleData(%d) [%g,%g]->[%g,%g] ==> %g -> %g\n", i,
		   rSrcScaler.min(i), rSrcScaler.max(i),
		   rDstScaler.min(i), rDstScaler.max(i),
		   fSrcVal, pDstVect[i]);
#endif // NNUnit_SCALE
    }
}


//---------------------------------------------------------------------------
// Jog weights and biases
void
NaNNUnit::JogNetwork (NaReal mm)
{
    NaRandomSequence    rSeq;
    unsigned    iLayer, iNeuron, iInput;

    NaReal  vMin, vMax, vRand;
    if(mm < 0){
        vMin = mm;
        vMax = - mm;
    }
    else{
        vMax = mm;
        vMin = - mm;
    }
    rSeq.SetUniformParams(vMin, vMax);
    rSeq.Reset();

    for(iLayer = InputLayer(); iLayer <= OutputLayer(); ++iLayer){
        for(iNeuron = 0; iNeuron < Neurons(iLayer); ++iNeuron){
            rSeq.Function(NULL, &vRand);
            bias[iLayer][iNeuron] += vRand;
            for(iInput = 0; iInput < Inputs(iLayer); ++iInput){
                rSeq.Function(NULL, &vRand);
                weight[iLayer][iNeuron][iInput] += vRand;
            }// for inputs of the neuron
        }// for neurons of the layer
    }// for layers
}


//---------------------------------------------------------------------------

