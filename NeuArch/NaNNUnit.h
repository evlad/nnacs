//-*-C++-*-
/* NaNNUnit.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaNNUnitH
#define NaNNUnitH
//---------------------------------------------------------------------------

#include <NaUnit.h>
#include <NaNDescr.h>
#include <NaVector.h>
#include <NaMatrix.h>
#include <NaConfig.h>

//---------------------------------------------------------------------------
// Class for neural network representation based on NaNDescr.
class NaNNUnit : public NaUnit, public NaConfigPart
{
public:

    NaNNUnit ();
    NaNNUnit (const NaNeuralNetDescr& rDescr);
    NaNNUnit (const NaNNUnit& rUnit);
    virtual ~NaNNUnit ();

    // Assign the object
    NaNNUnit&   operator= (const NaNNUnit& rUnit);

    // Compare objects
    bool        operator== (const NaNNUnit& rUnit) const;
    bool        operator!= (const NaNNUnit& rUnit) const{
        return !operator==(rUnit);
    }

    // Assign new description
    void    AssignDescr (const NaNeuralNetDescr& rDescr);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

    // Store neural net to the file
    virtual void    Save (const char* szFileName);

    // Retrieve neural net from the file
    virtual void    Load (const char* szFileName);

    //==================
    // NaUnit inherited 
    //==================

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y);

    // Supply unit with feedback values on the first step of
    // unit perfomance.  All needed feedback values are stored.
    virtual void    FeedbackValues (NaReal* fb);

    //=====================
    // NaLogging inherited 
    //=====================

    virtual void    PrintLog () const;

    //====================
    // New data & methods 
    //====================

    // Return number of parameter records (see layers)
    unsigned        Layers () const;

    // Return number of neurons in given layer
    unsigned        Neurons (unsigned iLayer) const;

    // Return number of inputs of given layer
    // If feedback exists then Inputs(InputLayer())!=InputDim()
    unsigned        Inputs (unsigned iLayer) const;

    // Index of the first avaiable (input) NN hidden layer
    unsigned        InputLayer () const;

    // Index of the last (output) NN layer
    unsigned        OutputLayer () const;

    // NN description
    NaNeuralNetDescr  descr;

    // The vector of feedback values fb(t-n,i),fb(t-n+1,i),...,fb(t-1,i)
    // where i=0..Yout.dim()-1
    NaVector        feedback;

    // Parameters of the network (0.layers()-1)
    NaMatrix        weight[NaMAX_HIDDEN+1];
    NaVector        bias[NaMAX_HIDDEN+1];

    // Results of the computation
    NaVector&       Xinp (unsigned iLayer); // input of the layer
    NaVector        Xinp0;                  // input of the first layer
    NaVector        Yout[NaMAX_HIDDEN+1];   // after ActFunc
    NaVector        Znet[NaMAX_HIDDEN+1];   // before ActFunc

    // Scaler of the output layer
    struct NaScaler {
        NaVector    min;
        NaVector    max;
    }               InputScaler, OutputScaler, StdInputRange, StdOutputRange;

    // Compute activation function for the given layer
    virtual NaReal	ActFunc (unsigned iLayer, NaReal z);

    // Compute derivation of activation function
    virtual NaReal	DerivActFunc (unsigned iLayer, NaReal z);

    // Initialize weights and biases
    virtual void	Initialize ();

    // Jog weights and biases
    virtual void	JogNetwork (NaReal mm);

    // Set output scale: [-1,1] -> [yMin,yMax]
    virtual void    SetOutputScale (const NaReal* yMin, const NaReal* yMax);

    // Set input scale: [yMin,yMax] -> [-1,1]
    virtual void    SetInputScale (const NaReal* yMin, const NaReal* yMax);

    // Scale given vector [iIndex..iIndex+nDim1]; maybe pSrcVect==pDstVect
    virtual void    ScaleData (const NaNNUnit::NaScaler& rSrcScaler,
                               const NaNNUnit::NaScaler& rDstScaler,
                               const NaReal* pSrcVect,
                               NaReal* pDstVect,
                               unsigned nDim,
			       unsigned iIndex = 0) const;

};

//---------------------------------------------------------------------------
#endif
