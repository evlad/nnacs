/* NaNDescr.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <string.h>

#include "NaNDescr.h"


//---------------------------------------------------------------------------
const char*
NaActFuncToStrIO (NaActFuncKind eAFK)
{
    switch(eAFK){

    case afkLinear:
        return "linear";
    case afkTanH:
        return "tanh";
    }
    return "???";
}

//---------------------------------------------------------------------------
NaActFuncKind
NaStrToActFuncIO (const char* s)
{
    if(NULL == s)
        throw(na_null_pointer);

    if(!strcmp(NaActFuncToStrIO(afkLinear), s))
        return afkLinear;
    else if(!strcmp(NaActFuncToStrIO(afkTanH), s))
        return afkTanH;
    return __afkNumber;
}

//---------------------------------------------------------------------------
void    NaNeuralNetDescr::PrintLog () const
{
    NaPrintLog("NaNeuralNetDescr(this=%p):\n  inputs=%u  outputs=%u"
               "  in.repeat=%u  out.repeat=%u  feedback=%u\n"
               "  %s last layer activation function\n"
               "  hidden layers %d total:\n", this,
               nInputsNumber, nOutNeurons, nInputsRepeat, nOutputsRepeat,
               nFeedbackDepth, NaActFuncToStrIO(eLastActFunc), nHidLayers);

    unsigned	i;

    for(i = 0; i < nHidLayers; ++i)
        NaPrintLog("  * layer #%d has %d neurons\n", i, nHidNeurons[i]);

    NaPrintLog("  Input delays:\n");
    if(1 == nInputsNumber)
      for(i = 0; i < nInputsRepeat; ++i)
        NaPrintLog("  * input #%u delayed by %g\n", i, vInputDelays(i));
    else
      for(i = 0; i < nInputsRepeat; ++i)
        NaPrintLog("  * inputs #%u..%u delayed by %g\n",
		   i*nInputsNumber, (i+1)*nInputsNumber-1, vInputDelays(i));

    NaPrintLog("  Output delays:\n");
    if(1 == nOutNeurons)
      for(i = 0; i < nOutputsRepeat; ++i)
        NaPrintLog("  * output #%u delayed by %g\n", i, vOutputDelays(i));
    else
      for(i = 0; i < nOutputsRepeat; ++i)
        NaPrintLog("  * outputs #%u..%u delayed by %g\n",
		   i*nOutNeurons, (i+1)*nOutNeurons-1, vOutputDelays(i));
}

//---------------------------------------------------------------------------
NaNeuralNetDescr::NaNeuralNetDescr (unsigned nIn, unsigned nOut)
:   NaConfigPart("NeuralNetArchitecture")
{
    nHidLayers = 1;

    unsigned    i;
    for(i = 0; i < NaMAX_HIDDEN; ++i)
        if(i < nHidLayers)
            nHidNeurons[i] = 5;
        else
            nHidNeurons[i] = 0;

    nOutNeurons = nOut;
    nInputsNumber = nIn;
    nInputsRepeat = 1;
    nOutputsRepeat = 0;
    nFeedbackDepth = 0;
    eLastActFunc = afkLinear;

    vInputDelays.new_dim(nInputsRepeat);
    vOutputDelays.new_dim(nOutputsRepeat);

    /* default delays */
    for(i = 0; i < nInputsRepeat; ++i)
      vInputDelays[i] = i;
    for(i = 0; i < nOutputsRepeat; ++i)
      vOutputDelays[i] = i;
}


//---------------------------------------------------------------------------
// Default delays
void
NaNeuralNetDescr::MakeDefaultDelays ()
{
    vInputDelays.new_dim(nInputsRepeat);
    vOutputDelays.new_dim(nOutputsRepeat);

    /* default delays */
    unsigned	i;
    for(i = 0; i < nInputsRepeat; ++i)
      vInputDelays[i] = i;
    for(i = 0; i < nOutputsRepeat; ++i)
      vOutputDelays[i] = i;
}


//---------------------------------------------------------------------------
NaNeuralNetDescr::NaNeuralNetDescr (const NaNeuralNetDescr& rDescr)
:   NaConfigPart(rDescr)
{
    nHidLayers = rDescr.nHidLayers;

    unsigned    i;
    for(i = 0; i < NaMAX_HIDDEN; ++i)
        if(i < nHidLayers)
            nHidNeurons[i] = rDescr.nHidNeurons[i];
        else
            nHidNeurons[i] = 0;

    nOutNeurons = rDescr.nOutNeurons;
    nInputsNumber = rDescr.nInputsNumber;
    nInputsRepeat = rDescr.nInputsRepeat;
    nOutputsRepeat = rDescr.nOutputsRepeat;
    nFeedbackDepth = rDescr.nFeedbackDepth;
    eLastActFunc = rDescr.eLastActFunc;

    MakeDefaultDelays();

    if(rDescr.vInputDelays.dim() == nInputsRepeat)
      vInputDelays = rDescr.vInputDelays;
    if(rDescr.vOutputDelays.dim() == nOutputsRepeat)
      vOutputDelays = rDescr.vOutputDelays;
}

//---------------------------------------------------------------------------
NaNeuralNetDescr::~NaNeuralNetDescr ()
{
    // dummy
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaNeuralNetDescr::Save (NaDataStream& ds)
{
    ds.PutComment("Neural network architecture definition");
    ds.PutF("Number of inputs and their repeat factor", "%u %u",
            nInputsNumber, nInputsRepeat);
    ds.PutF("Output repeat factor on the input", "%u", nOutputsRepeat);
    ds.PutF("Feedback depth", "%u", nFeedbackDepth);
    ds.PutF("Number of hidden layers", "%u", nHidLayers);

    unsigned    i;
    for(i = 0; i < nHidLayers; ++i)
        ds.PutF("Hidden layer", "%u", nHidNeurons[i]);

    ds.PutF("Output layer", "%s %u",
            NaActFuncToStrIO(eLastActFunc), nOutNeurons);

    if(ds.GetVersion(NaMajorVerNo) < 1 ||
       ds.GetVersion(NaMajorVerNo) == 1 && ds.GetVersion(NaMinorVerNo) < 1)
      NaPrintLog("WARNING: information about input and output delays "
		 "will be lost while saving.\n");
    else /* Version 1.1 and newer */
      {
	ds.PutComment("Input delays:");
	for(i = 0; i < nInputsRepeat; ++i)
	  ds.PutF(NULL, "%u", (int)vInputDelays(i));

	ds.PutComment("Output delays:");
	for(i = 0; i < nOutputsRepeat; ++i)
	  ds.PutF(NULL, "%u", (int)vOutputDelays(i));
      }
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaNeuralNetDescr::Load (NaDataStream& ds)
{
    NaPrintLog("Data stream version %u.%u\n",
	       ds.GetVersion(NaMajorVerNo), ds.GetVersion(NaMinorVerNo));
    ds.GetF("%u %u", &nInputsNumber, &nInputsRepeat);
    ds.GetF("%u", &nOutputsRepeat);
    ds.GetF("%u", &nFeedbackDepth);
    ds.GetF("%u", &nHidLayers);
    unsigned    i;
    for(i = 0; i < nHidLayers; ++i){
        unsigned    neurons;
        ds.GetF("%u", &neurons);
        if(i < NaMAX_HIDDEN)
            nHidNeurons[i] = neurons;
    }
    if(nHidLayers > NaMAX_HIDDEN){
        nHidLayers = NaMAX_HIDDEN;
        NaPrintLog("Too many hidden layers: only %u are allowed\n",
                   NaMAX_HIDDEN);
    }

    char    szBuf[1024];

    ds.GetF("%s %u", szBuf, &nOutNeurons);
    eLastActFunc = NaStrToActFuncIO(szBuf);

    vInputDelays.new_dim(nInputsRepeat);
    vOutputDelays.new_dim(nOutputsRepeat);

    if(ds.GetVersion(NaMajorVerNo) < 1 ||
       ds.GetVersion(NaMajorVerNo) == 1 && ds.GetVersion(NaMinorVerNo) < 1)
      {
	/* default delays */
	for(i = 0; i < nInputsRepeat; ++i)
	  vInputDelays[i] = i;

	for(i = 0; i < nOutputsRepeat; ++i)
	  vOutputDelays[i] = i;
      }
    else /* Version 1.1 and newer */
      {
	for(i = 0; i < nInputsRepeat; ++i)
	  ds.GetF("%lg", &vInputDelays[i]);

	for(i = 0; i < nOutputsRepeat; ++i)
	  ds.GetF("%lg", &vOutputDelays[i]);
      }
}

//---------------------------------------------------------------------------
NaNeuralNetDescr&
NaNeuralNetDescr::operator= (const NaNeuralNetDescr& rDescr)
{
    nHidLayers = rDescr.nHidLayers;

    unsigned    i;
    for(i = 0; i < NaMAX_HIDDEN; ++i)
        if(i < nHidLayers)
            nHidNeurons[i] = rDescr.nHidNeurons[i];
        else
            nHidNeurons[i] = 0;

    nOutNeurons = rDescr.nOutNeurons;
    nInputsNumber = rDescr.nInputsNumber;
    nInputsRepeat = rDescr.nInputsRepeat;
    nOutputsRepeat = rDescr.nOutputsRepeat;
    nFeedbackDepth = rDescr.nFeedbackDepth;
    eLastActFunc = rDescr.eLastActFunc;

    MakeDefaultDelays();

    if(rDescr.vInputDelays.dim() == nInputsRepeat)
      vInputDelays = rDescr.vInputDelays;
    if(rDescr.vOutputDelays.dim() == nOutputsRepeat)
      vOutputDelays = rDescr.vOutputDelays;

    return *this;
}

//---------------------------------------------------------------------------
bool
NaNeuralNetDescr::operator== (const NaNeuralNetDescr& rDescr) const
{
    if(nHidLayers != rDescr.nHidLayers)
        return false;

    bool    r = true;

    unsigned    i;
    for(i = 0; i < nHidLayers; ++i)
        r = r && (nHidNeurons[i] == rDescr.nHidNeurons[i]);

    r = r && (nOutNeurons == rDescr.nOutNeurons);
    r = r && (nInputsNumber == rDescr.nInputsNumber);
    r = r && (nInputsRepeat == rDescr.nInputsRepeat);
    r = r && (nOutputsRepeat == rDescr.nOutputsRepeat);
    r = r && (nFeedbackDepth == rDescr.nFeedbackDepth);
    r = r && (eLastActFunc == rDescr.eLastActFunc);
    r = r && (vInputDelays == rDescr.vInputDelays);
    r = r && (vOutputDelays == rDescr.vOutputDelays);

    return r;
}

//---------------------------------------------------------------------------
unsigned
NaNeuralNetDescr::MaxInputDelay () const
{
  unsigned	i, nMaxDelay = 0;

  for(i = 0; i < nInputsRepeat; ++i)
    if(nMaxDelay < (unsigned)vInputDelays(i))
      nMaxDelay = (unsigned)vInputDelays(i);

  return nMaxDelay;
}


//---------------------------------------------------------------------------
unsigned
NaNeuralNetDescr::MaxOutputDelay () const
{
  unsigned	i, nMaxDelay = 0;

  for(i = 0; i < nOutputsRepeat; ++i)
    if(nMaxDelay < (unsigned)vOutputDelays(i))
      nMaxDelay = (unsigned)vOutputDelays(i);

  return nMaxDelay;
}

//---------------------------------------------------------------------------
unsigned*
NaNeuralNetDescr::InputDelays () const
{
  unsigned	i, *nDelays = new unsigned[nInputsRepeat];

  for(i = 0; i < nInputsRepeat; ++i)
    nDelays[i] = (unsigned)vInputDelays(i);

  return nDelays;
}

//---------------------------------------------------------------------------
unsigned*
NaNeuralNetDescr::OutputDelays () const
{
  unsigned	i, *nDelays = new unsigned[nOutputsRepeat];

  for(i = 0; i < nOutputsRepeat; ++i)
    nDelays[i] = (unsigned)vOutputDelays(i);

  return nDelays;
}


//---------------------------------------------------------------------------
// Return number of paramter records (see layers)
unsigned
NaNeuralNetDescr::Layers () const
{
    return nHidLayers + 1/* output */;
}


//---------------------------------------------------------------------------
// Return number of neurons in given layer
unsigned
NaNeuralNetDescr::Neurons (unsigned iLayer) const
{
    if(iLayer < Layers() - 1)
        return nHidNeurons[iLayer];
    else if(iLayer == Layers() - 1)
        return nOutNeurons;

    throw(na_out_of_range);

    /* make compiler happy */
    return 0;
}


//---------------------------------------------------------------------------
// Return number of inputs of given layer
unsigned
NaNeuralNetDescr::Inputs (unsigned iLayer) const
{
    if(iLayer == 0)
        return InputsNumber();
    else if(iLayer <= Layers() - 1)
        return nHidNeurons[iLayer - 1];

    throw(na_out_of_range);

    /* make compiler happy */
    return 0;
}


//---------------------------------------------------------------------------
// Index of the first avaiable (input) NN hidden layer
unsigned
NaNeuralNetDescr::InputLayer () const
{
    return 0;
}


//---------------------------------------------------------------------------
// Index of the last (output) NN layer
unsigned
NaNeuralNetDescr::OutputLayer () const
{
    return Layers() - 1;
}


//---------------------------------------------------------------------------
