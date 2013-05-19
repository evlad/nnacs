/* NaLiNois.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaLiNois.h"


//---------------------------------------------------------------------------
// Build new generator with default parameters and (1,1) in-out scheme
NaLinearNoiseGen::NaLinearNoiseGen (unsigned nChainLen)
:   LinChain(nChainLen), NaConfigPart("LinearNoiseGenerator")
{
    // Linear chain initialization
    LinChain[0].act = true;
    RandSeq.SetInstance("InLinearNoiseGenerator");
    LinChain.SetInstance("InLinearNoiseGenerator");
}

//---------------------------------------------------------------------------
// Build copy of the generator
NaLinearNoiseGen::NaLinearNoiseGen (const NaLinearNoiseGen& rLNGen)
:   RandSeq(rLNGen.RandSeq), LinChain(rLNGen.LinChain), NaConfigPart(rLNGen)
{
    // Dummy
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaLinearNoiseGen::Save (NaDataStream& ds)
{
    RandSeq.Save(ds);    // randomizer
    LinChain.Save(ds);   // linear control block chain
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaLinearNoiseGen::Load (NaDataStream& ds)
{
    RandSeq.Load(ds);    // randomizer
    LinChain.Load(ds);   // linear control block chain
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// session will start.  It's guaranteed that this reset will be
// called just after Timer().ResetTime().
void    NaLinearNoiseGen::Reset ()
{
    RandSeq.Reset();
    LinChain.Reset();
}

//---------------------------------------------------------------------------
// Compute output as a random signal passed through linear chain
// x is dummy argument
void    NaLinearNoiseGen::Function (NaReal* x, NaReal* y)
{
    NaReal  fRandNum;

    RandSeq.Function(x, &fRandNum);
    LinChain.Function(&fRandNum, y);
}

//---------------------------------------------------------------------------
// Setup timer
void
NaLinearNoiseGen::SetupTimer (NaTimer* pTimer_)
{
    NaUnit::SetupTimer(pTimer_);

    RandSeq.SetupTimer(&Timer());
    LinChain.SetupTimer(&Timer());
}


//---------------------------------------------------------------------------

