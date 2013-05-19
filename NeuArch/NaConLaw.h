//-*-C++-*-
/* NaConLaw.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaConLawHPP
#define NaConLawHPP

#include <stdio.h>

#include <NaGenerl.h>
#include <NaDynAr.h>
#include <NaTimer.h>
#include <NaUnit.h>
#include <NaDataIO.h>
#include <NaLiNois.h>
#include <NaConfig.h>

//---------------------------------------------------------------------------
// Control law source
enum NaControlLawSource
{
    clsDescription = 0,
    clsFromFile,
    clsManual,
    clsRandom,
    __clsNumber
};

// Enum <-> Text
const char*         SourceToStrIO (NaControlLawSource s);
NaControlLawSource  StrToSourceIO (const char* str);

//---------------------------------------------------------------------------
// Control law item kind
enum NaControlLawKind
{
    clkConstant = 0,
    clkMeander,
    clkSine,
    clkSineSine,
    clkPike,
    __clkNumber
};

// Enum <-> Text
const char*         KindToStrIO (NaControlLawKind k);
const char*         KindToStrGUI (NaControlLawKind k);
NaControlLawKind    StrToKindIO (const char* str);

//---------------------------------------------------------------------------
// Description of one control law item

class NaControlLawItem : public NaConfigPart
{
public:
    NaControlLawItem ();

    NaControlLawKind    kind;
    NaReal              value;      // Relative value
    NaReal              duration;   // How long lasts, s
    NaReal              period;     // For periodic laws, s

    // Compute x=f(t) considering that for periodic kinds
    // x is relative to some base level.  Duration is not
    // considered at all.  Time is counted from zero.
    NaReal      Function (NaReal time, NaReal Dt);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

};

//---------------------------------------------------------------------------
// Array of control law items

typedef NaDynAr<NaControlLawItem>   NaControlLawItems;

//---------------------------------------------------------------------------
// Control law general description and usage
class NaControlLaw : public NaUnit, public NaConfigPart
{
public:

    //---------------------------------------------
    // BEGIN: Data area

    NaReal              current_value;

    NaControlLawSource  source;

    // clsDescription
    NaControlLawItems   items;

    // clsFromFile
    char                *filepath;
    NaDataFile          *dataio;
    int                 prev_index;

    // clsManual
    NaReal              start_value;

    // clsRandom
    NaLinearNoiseGen    lin_rand;

    // END: Data area
    //---------------------------------------------

public:

    // Construct the object
    NaControlLaw (unsigned nChainLen);

    // Copying construct the object
    NaControlLaw (const NaControlLaw& rUnit);

    // Destroy the object
    virtual ~NaControlLaw ();

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Setup timer
    virtual void    SetupTimer (NaTimer* pTimer_);

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

    // Return current value of the control law
    NaReal  CurrentValue ();

    // Find control law item for this time and get start time
    // for this item
    int     FindItem (NaReal time, NaReal& start);

    // Calculate total time
    NaReal  TotalTimeLength ();

};

//---------------------------------------------------------------------------
#endif
