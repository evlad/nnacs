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
const char*   RandomDistrToStrIO (NaRandomDistribution eRD);
NaRandomDistribution    StrToRandomDistrIO (const char* str);

//---------------------------------------------------------------------------
// Class for random sequence generation
class NaRandomSequence : public NaUnit, public NaConfigPart
{
public:

    // Construct by default: (1,1)
    NaRandomSequence ();

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
    void    SetDistribution (NaRandomDistribution eRD);

    // Setup distribution parameters
    void    SetGaussianParams (NaReal mean, NaReal stddev);
    void    SetUniformParams (NaReal min, NaReal max);

    // Get actual generator law
    NaRandomDistribution    GetDistribution () const;

    // Get distribution parameters
    void    GetGaussianParams (NaReal& mean, NaReal& stddev) const;
    void    GetUniformParams (NaReal& min, NaReal& max) const;

    // Store/load parameters from file
    void    Serialize (FILE* fp) const;
    void    Deserialize (FILE* fp);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

private:

    // How to produce random numbers?
    NaRandomDistribution    eRandomDistr;

    // Gaussian distribution parameters
    NaReal      fMean;      // Mean value
    NaReal      fStdDev;    // Standard deviation

    // Uniform distribution parameters
    NaReal      fMin;       // Low value limit
    NaReal      fMax;       // High value limit
};


//---------------------------------------------------------------------------
#endif
