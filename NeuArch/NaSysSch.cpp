/* NaSysSch.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <string.h>

#include "NaSysSch.h"


//---------------------------------------------------------------------------
NaSystemSchema::NaSystemSchema ()
: NaConfigPart("SystemSchema"),
  bNoise(true),
  bFeedback(true),
  fSampRate(1.0)
{
    // Dummy
}


//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaSystemSchema::Save (NaDataStream& ds)
{
    char    szBuf[100];

    ds.PutF("Global control feedback loop switcher", "%d", bFeedback);

    ds.PutF("Additive noise to object output switcher", "%d", bNoise);

    PutControllerModel(szBuf);
    ds.PutF("Controller model", "%s", szBuf);

    PutObjectModel(szBuf);
    ds.PutF("Object model", "%s", szBuf);

    ds.PutF("Sampling rate, s", "%g", fSampRate);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaSystemSchema::Load (NaDataStream& ds)
{
    int     vInt;
    float   vFloat;
    char    szBuf[100];

    ds.GetF("%d", &vInt);
    bFeedback = !!vInt;

    ds.GetF("%d", &vInt);
    bNoise = !!vInt;

    ds.GetF("%s", szBuf);
    ParseControllerModel(szBuf);

    ds.GetF("%s", szBuf);
    ParseObjectModel(szBuf);

    ds.GetF("%g", &vFloat);
    fSampRate = vFloat;
}


//---------------------------------------------------------------------------
// Put object model
void
NaSystemSchema::PutObjectModel (char* szBuf)
{
    strcpy(szBuf, "undefined");
}


//---------------------------------------------------------------------------
// Parse object model
void
NaSystemSchema::ParseObjectModel (char* szBuf)
{
    // Dummy
}


//---------------------------------------------------------------------------
// Put controller model
void
NaSystemSchema::PutControllerModel (char* szBuf)
{
    strcpy(szBuf, "undefined");
}


//---------------------------------------------------------------------------
// Parse controller model
void
NaSystemSchema::ParseControllerModel (char* szBuf)
{
    // Dummy
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
