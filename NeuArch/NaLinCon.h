//-*-C++-*-
/* NaLinCon.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaLinConH
#define NaLinConH

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaConfig.h>

//---------------------------------------------------------------------------
// Kinds of linear control units
enum NaLinearControlKind
{
    lckGain = 0,
    lckInertial,
    lckIntegral,
    lckDifferential,
    lckDiffStatic,
    lckOscillation,
    lckConservative,
    lckDelay,
    __lckNumber
};


//---------------------------------------------------------------------------
const char*         LinControlKindToStrIO (NaLinearControlKind eK);
NaLinearControlKind StrToLinControlKindIO (const char* s);

//---------------------------------------------------------------------------
// Class for linear control units
class NaLinearUnit : public NaUnit, public NaConfigPart
{
public:

    // Make default unit (gain K=1.0)
    NaLinearUnit ();

    // Make an explicit copy of the unit
    NaLinearUnit (const NaLinearUnit& orig);

    // Assignment
    NaLinearUnit&   operator= (const NaLinearUnit& orig);

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

    bool                act;    // on/off flag
    NaLinearControlKind kind;   // kind of the unit
    NaReal              K;      // pure gain of the unit
    NaReal              T;      // time constant
    NaReal              Rho;    // absorption
    NaReal              Tau;    // pure delay

protected:

    //NaReal              y1;     // y[n-1] value
    //NaReal              y2;     // y[n-2] value
    NaDynAr<NaReal>     yd;     // variable delay
    NaReal              Xprev;  // previous value for differencial

    void            ShiftDelay (NaReal yn);
};


//---------------------------------------------------------------------------
// Class for chain of linear control units
class NaLinearUnitsChain :
    public NaDynAr<NaLinearUnit>,
    public NaUnit,
    public NaConfigPart
{
public:

    // Construct the object
    NaLinearUnitsChain (unsigned nUnits = 0);

    // Copying construct the object
    NaLinearUnitsChain (const NaLinearUnitsChain& rUnitsChain);

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Setup timer
    virtual void    SetupTimer (NaTimer* pTimer_);

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y);

    // Return input dimension
    virtual unsigned    InputDim () const;

    // Return output dimension
    virtual unsigned    OutputDim () const;

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

protected:

    // Two pointers with nMaxDim memory arrays
    NaReal      *iobuf[2];

    // Computed after Reset() maximum dimension among units in chain
    unsigned    nMaxDim;

};


//---------------------------------------------------------------------------
#endif
