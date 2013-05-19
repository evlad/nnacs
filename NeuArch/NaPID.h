//-*-C++-*-
/* NaPID.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPIDH
#define NaPIDH
//---------------------------------------------------------------------------

#include <NaUnit.h>
#include <NaConfig.h>

//---------------------------------------------------------------------------
// Class for PID controller
class NaPID : public NaUnit, public NaConfigPart
{
public:

    // Create default gain PID controller
    NaPID ();
    
    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y);


    NaReal  Kp;     // Proportional gain
    NaReal  Ki;     // Integration gain
    NaReal  Ti;     // Integration time value
    NaReal  Kd;     // Derivation gain
    NaReal  Td;     // Derivation time value

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

private:

    NaReal  Xprev, Xsum;
};


//---------------------------------------------------------------------------
#endif
