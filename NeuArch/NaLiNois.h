//-*-C++-*-
/* NaLiNois.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaLiNoisH
#define NaLiNoisH
//---------------------------------------------------------------------------

#include <NaUnit.h>
#include <NaRandom.h>
#include <NaLinCon.h>
#include <NaConfig.h>

//---------------------------------------------------------------------------
// Unit for linear noise generation.  Unit consists of randomizer and linear
// control block chain.
class NaLinearNoiseGen : public NaUnit, public NaConfigPart
{
public:

    // The data
    NaRandomSequence    RandSeq;    // randomizer
    NaLinearUnitsChain  LinChain;   // linear control block chain

    // Build new generator with default parameters and (1,1) in-out scheme
    NaLinearNoiseGen (unsigned nChainLen);

    // Build copy of the generator
    NaLinearNoiseGen (const NaLinearNoiseGen& rLNGen);

    // Reset operations, that must be done before new modelling
    // session will start.  It's guaranteed that this reset will be
    // called just after Timer().ResetTime().
    virtual void    Reset ();

    // Setup timer
    virtual void    SetupTimer (NaTimer* pTimer_);

    // Compute output as a random signal passed through linear chain
    // x is dummy argument
    virtual void    Function (NaReal* x, NaReal* y);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

};


//---------------------------------------------------------------------------
#endif
