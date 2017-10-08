
//-*-C++-*-
/* NaLM.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaLMH
#define NaLMH
//---------------------------------------------------------------------------

#include <NaDynAr.h>
#include <NaVector.h>
#include <NaMatrix.h>
#include <NaNDescr.h>
#include <NaNNUnit.h>
#include <NaLogFil.h>

//---------------------------------------------------------------------------
// Learning coefficients
class PNNA_API NaLMParams : public NaLogging
{
public:

    NaLMParams ();

    NaReal      eta;        // Learning rate coefficient for hidden layers
    NaReal      eta_output; // Learning rate coefficient for output layer
    NaReal      alpha;      // Momentum (inertia coefficient)

    // Assignment of the object
    NaLMParams&    operator= (const NaLMParams& r);

    // Learning rate coefficient for the given layer
    virtual NaReal  LearningRate (unsigned iLayer) const;

    // Momentum (inertia coefficient) for the given layer
    virtual NaReal  Momentum (unsigned iLayer) const;

    virtual void    PrintLog () const;

};


//---------------------------------------------------------------------------
// Class for standard backpropagation learning environment
class PNNA_API NaLM : virtual public NaLMParams
{
public:/* methods */

    // Prepare object to work with given neural network architecture.
    // Most operations are disabled until attaching neural network data.
    NaLM (NaNeuralNetDescr& rND);

    // Prepare object to work with given neural network passed by
    // reference.  Actually a pointer to the NN object will be stored.
    NaLM (NaNNUnit& rNN);

    virtual ~NaLM ();

    // Set flag of debugging messages:
    // 0 - off (default)
    // 1 - weight change application
    // 2 - delta weight calculation
    // 3 - scaling
    void            SetDebugLevel (int iDebug);

    // Return reference to the neural network architecture
    const NaNeuralNetDescr&	Arch () const;

    // Return pointer to the current neural network data or NULL if no
    // network data is attached
    NaNNUnit*       NN () const;

    // Return reference to the current neural network data or throw
    // exception na_bad_value if no network data is attached
    NaNNUnit&       nn () const;

    // Attach neural network data to be used in further operations:
    // delta calculations and application.  Several attached neural
    // networks must be detached in order.
    virtual void    AttachNN (NaNNUnit* pNN);

    // Detach the last attached neural network
    virtual void    DetachNN ();

    // Reset computed changes: delta of weights
    virtual void    Reset ();

    // Update the current neural network parameters on the basis of
    // computed changes.
    virtual void    UpdateNN ();

    // Delta rule for the last layer.
    // Ytarg is desired vector needs to be compared with Yout
    // of the output layer or error value already computed..
    // If bError==true then Ytarg means error ready to use without Yout.
    // If bError==false then Ytarg means Ydes to compare with Yout.
    virtual void    DeltaRule (const NaReal* Ytarg, bool bError = false);

    // Delta rule for the hidden layer
    // iLayer - index of target layer delta computation on the basis
    // of previous layer's delta and linked weights.
    // Usually iPrevLayer=iLayer+1.
    virtual void    DeltaRule (unsigned iLayer, unsigned iPrevLayer);

    // Part of delta rule for the hidden layer
    // Computes sum of products outcoming weights and target deltas
    // on given layer and for given input
    virtual NaReal  PartOfDeltaRule (unsigned iPrevLayer, unsigned iInput);

    // Compute delta weights based on common delta (see DeltaRule)
    virtual void    ApplyDelta (unsigned iLayer);

    // Compute delta of exact w[i,j,k]
    virtual NaReal  DeltaWeight (unsigned iLayer, unsigned iNeuron,
                                 unsigned iInput);

    // Compute delta of exact b[i,j]
    virtual NaReal  DeltaBias (unsigned iLayer, unsigned iNeuron);

    // Make slower learning rate for last layer
    // Learning rate coefficient for the given layer
    virtual NaReal  LearningRate (unsigned iLayer) const;

    // Return true if there is a need to prohibit bias change.
    virtual bool    DontTouchBias ();

    // -------------------------------
    // LM specifics:

    /// Assign current data pair and target result to calculate output error
    virtual void SetTrainingPair (unsigned iSample, NaReal *pIn,
				  NaReal *pTar, NaReal *pOut);

    /// Assign output error of the NN for iSample input
    virtual void SetOutputError (unsigned iSample, NaReal *pTar, NaReal *pOut);

    /// Calculate correction for iOut output neuron 
    virtual void CalcOutNeuronCorrection (NaReal *pTar, NaReal *pOut, unsigned iOut);

    /// Calculate LM weight adjustments considering given Alpha
    virtual void CalcWeightAdj (NaReal fAlphaLM);

    /// Apply weight adjustments or undo them
    virtual void ApplyWeightAdj (bool bUndo = false);

    virtual void UpdateNN_LM();
    virtual void DegradeNN_LM();
    virtual void Init_Zero_LM();

    // Set number of input,output pairs to calculate Jacobian
    virtual void SetNSamples_LM(int n);

    NaReal Error_NEW;

    //public:/* data */
protected:

    // Initialize the object on the basic of preset nd
    void	Init ();

    // Neural network architecture to deal with
    NaNeuralNetDescr	nd;

    // Currently attached neural networks: the last is the first
    NaDynAr<NaNNUnit*>	nnStack;

    // Intermediate delta (t)
    NaVector    delta[NaMAX_HIDDEN+1];
    // Intermediate delta (t-1)
    NaVector    delta_prev[NaMAX_HIDDEN+1];

    // Accumulated changes: dw(t)
    NaMatrix    dWeight[NaMAX_HIDDEN+1];
    NaVector    dBias[NaMAX_HIDDEN+1];

    // Previous step of change: (w(t-1) - w(t-2))
    NaMatrix    psWeight[NaMAX_HIDDEN+1];
    NaVector    psBias[NaMAX_HIDDEN+1];

    //---------------------------------------
    // Leverberg-Marquardt method specifics:
    // [����� ���������� �����������]

    NaMatrix J_LM;// ������� �������� ������������
    //[��� ����+��� ��������� ��������][���������� ��������� ���*���������� �������]
    unsigned I_LM;//���������� �������������� ���������� ������� ��������
    //[��� ����+��� ��������� ��������]

    /// Number of training pairs (base size of the LM)
    unsigned nSamples_LM;   // [���������� ��������� ���]

    NaVector Errors_LM;//  ������ �� ������ pTar[i]-pOut[i];
    NaVector dW_LM;//��������� ������� �������������
    NaVector COR_LM[NaMAX_HIDDEN+1];// ������ ��������� ��������,�������� ����� ��� �������� ������� ��������
    // Level of debugging messages.
    int         nDebugLvl;
};

//---------------------------------------------------------------------------
#endif
