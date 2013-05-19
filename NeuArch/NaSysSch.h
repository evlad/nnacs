//-*-C++-*-
/* NaSysSch.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaSysSchH
#define NaSysSchH

#include <NaGenerl.h>
#include <NaConfig.h>

//---------------------------------------------------------------------------
// System schema definition: feedback on/off, noise on/off, sampling rate,
// object model (stub), controller model (stub).

class NaSystemSchema : public NaConfigPart
{
public:/* methods */

    NaSystemSchema ();

    //***********************************************************************
    // Abstract operations: store and retrieve data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);


    //***********************************************************************
    // Abstract operations: put/parse object and controller model
    // Dummy implementation
    //***********************************************************************

    virtual void    ParseObjectModel (char* szBuf);
    virtual void    ParseControllerModel (char* szBuf);

    virtual void    PutObjectModel (char* szBuf);
    virtual void    PutControllerModel (char* szBuf);


public:/* data */

    // Noise on/off (on by default)
    bool            bNoise;

    // Feedback loop on/off (on by default)
    bool            bFeedback;

    // Sampling rate, s (1 by default)
    NaReal          fSampRate;

};


//---------------------------------------------------------------------------
#endif
 
