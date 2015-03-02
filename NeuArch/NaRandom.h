//-*-C++-*-
/* NaRandom.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaRandomH
#define NaRandomH
//---------------------------------------------------------------------------

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaLinCon.h>

//---------------------------------------------------------------------------
// Types of random generator laws
enum NaRandomDistribution
{
    rdGaussNormal = 0,  // normal Gaussian distribution law
    rdUniform,          // uniform distribution law
    __rdNumber          // special meaning
};

//---------------------------------------------------------------------------
// Serialize/deserialize NaRandomDistribution value
PNNA_API const char*   RandomDistrToStrIO (NaRandomDistribution eRD);
PNNA_API NaRandomDistribution    StrToRandomDistrIO (const char* str);

//---------------------------------------------------------------------------
// Class for random sequence generation
class PNNA_API NaRandomSequence : public NaUnit, public NaConfigPart
{
public:

    // Construct by default: (1,1)
    NaRandomSequence ();

    // Construct arbitrary output dimension >=1
    NaRandomSequence (unsigned nOutDim);

    // Copying construct
    NaRandomSequence (const NaRandomSequence& rRandSeq);

    // Destroy the object
    virtual ~NaRandomSequence ();

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute pseudo-random output that depends on described
    // random sequence law.  Input x does not matter.
    virtual void    Function (NaReal* x, NaReal* y);

    // Setup generator law
    void    SetDistribution (NaRandomDistribution eRD, unsigned iOut = 0);

    // Setup distribution parameters
    void    SetGaussianParams (NaReal mean, NaReal stddev, unsigned iOut = 0);
    void    SetUniformParams (NaReal min, NaReal max, unsigned iOut = 0);

    // Get actual generator law
    NaRandomDistribution    GetDistribution (unsigned iOut = 0) const;

    // Get distribution parameters
    void    GetGaussianParams (NaReal& mean, NaReal& stddev,
			       unsigned iOut = 0) const;
    void    GetUniformParams (NaReal& min, NaReal& max,
			      unsigned iOut = 0) const;

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

private:

    struct Params {
      // How to produce random numbers?
      NaRandomDistribution    eRandomDistr;

      // Gaussian distribution parameters
      NaReal      fMean;      // Mean value
      NaReal      fStdDev;    // Standard deviation

      // Uniform distribution parameters
      NaReal      fMin;       // Low value limit
      NaReal      fMax;       // High value limit

    }	*params;
};


//---------------------------------------------------------------------------
#endif
