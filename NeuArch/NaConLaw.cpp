/* NaConLaw.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "NaMath.h"
#include "NaLogFil.h"
#include "NaConLaw.h"


//---------------------------------------------------------------------------
const char*     KindToStrIO (NaControlLawKind k)
{
    switch(k)
    {
    case clkConstant:
        return "constant";
    case clkMeander:
        return "meander";
    case clkSine:
        return "sine";
    case clkSineSine:
        return "sine_sine";
    case clkPike:
        return "pike";
    default:
        return "?kind?";
    }
}

//---------------------------------------------------------------------------
const char*     KindToStrGUI (NaControlLawKind k)
{
    switch(k)
    {
    case clkConstant:
        return "Уровень";
    case clkMeander:
        return "Меандр";
    case clkSine:
        return "Синусоида";
    case clkSineSine:
        return "Синусоида синуса";
    case clkPike:
        return "Пик";
    default:
        return "?kind?";
    }
}

//---------------------------------------------------------------------------
NaControlLawKind    StrToKindIO (const char* str)
{
    if(!strcmp(str, KindToStrIO(clkConstant)))
        return clkConstant;
    else if(!strcmp(str, KindToStrIO(clkMeander)))
        return clkMeander;
    else if(!strcmp(str, KindToStrIO(clkSine)))
        return clkSine;
    else if(!strcmp(str, KindToStrIO(clkSineSine)))
        return clkSineSine;
    else if(!strcmp(str, KindToStrIO(clkPike)))
        return clkPike;
    else
        return __clkNumber;
}

//---------------------------------------------------------------------------
const char*       SourceToStrIO (NaControlLawSource s)
{
    switch(s)
    {
    case clsDescription:
        return "description";
    case clsFromFile:
        return "from_file";
    case clsManual:
        return "manual";
    case clsRandom:
        return "random";
    default:
        return "?source?";
    }
}

//---------------------------------------------------------------------------
NaControlLawSource  StrToSourceIO (const char* str)
{
    if(!strcmp(str, SourceToStrIO(clsDescription)))
        return clsDescription;
    else if(!strcmp(str, SourceToStrIO(clsFromFile)))
        return clsFromFile;
    else if(!strcmp(str, SourceToStrIO(clsManual)))
        return clsManual;
    else if(!strcmp(str, SourceToStrIO(clsRandom)))
        return clsRandom;
    else
        return __clsNumber;
}

//---------------------------------------------------------------------------
NaControlLaw::NaControlLaw (unsigned nChainLen)
:   NaConfigPart("ControlLaw"), lin_rand(nChainLen)
{
    source = clsDescription;
    dataio = NULL;
    filepath = NULL;
    items.addh(new NaControlLawItem());
}

//---------------------------------------------------------------------------
NaControlLaw::NaControlLaw (const NaControlLaw& orig)
:   NaConfigPart(orig), items(orig.items), lin_rand(orig.lin_rand)
{
    source = orig.source;
    dataio = NULL;
    filepath = new char[strlen(orig.filepath) + 1];
    strcpy(filepath, orig.filepath);
}

//---------------------------------------------------------------------------
NaControlLaw::~NaControlLaw ()
{
    delete dataio;
    delete[] filepath;
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaControlLaw::Save (NaDataStream& ds)
{
    ds.PutComment("Control law definition (set point)");
    ds.PutF("Current source kind", "%s", SourceToStrIO(source));

    // clsDescription
    ds.PutComment("Source kind 'description': set point by description");
    ds.PutF("Number of items", "%u", items.count());
    ds.PutComment(" 1 - Law kind ('constant', 'meander', 'pike', 'sine',"
                  " 'sine_sine')");
    ds.PutComment(" 2 - Relative value");
    ds.PutComment(" 3 - Duration (how long lasts), s");
    ds.PutComment(" 4 - Period (for periodic laws, s");

    for(int i = 0; i < items.count(); ++i){
        items[i].Save(ds);
    }

    // clsFromFile
    ds.PutComment("Source kind 'from_file': set point from file");
    ds.PutF("File path", "%s", NULL==filepath? "empty": filepath);

    // clsManual
    ds.PutComment("Source kind 'manual': manual set point");
    ds.PutF("Start value", "%g", start_value);

    // clsRandom
    ds.PutComment("Source kind 'random': randomized set point");
    lin_rand.Save(ds);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaControlLaw::Load (NaDataStream& ds)
{
    char    szBuf[1024];

    ds.GetF("%s", szBuf);
    source = StrToSourceIO(szBuf);

    // clsDescription
    int    nItems, i;
    ds.GetF("%u", &nItems);
    items.clean();
    for(i = 0; i < nItems; ++i){
        items.addh(new NaControlLawItem());
        items[i].Load(ds);
    }

    // clsFromFile
    ds.GetF("%s", szBuf);
    filepath = new char[strlen(szBuf) + 1];
    strcpy(filepath, szBuf);
    dataio = NULL;

    // clsManual
    ds.GetF("%lg", &start_value);

    // clsRandom
    lin_rand.Load(ds);
}

//---------------------------------------------------------------------------
NaControlLawItem::NaControlLawItem ()
:   NaConfigPart("ControlLawItem")
{
    kind = clkConstant;
    value = 0.0;
    duration = 100.0;
    period = 1.0;
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaControlLawItem::Save (NaDataStream& ds)
{
    ds.PutF(NULL, "%s %g %g %g", KindToStrIO(kind), value, duration, period);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaControlLawItem::Load (NaDataStream& ds)
{
    char    szBuf[1024];

    ds.GetF("%s %lg %lg %lg", &szBuf, &value, &duration, &period);
    kind = StrToKindIO(szBuf);
}

//---------------------------------------------------------------------------
// Compute x=f(t) considering that for periodic kinds
// x is relative to some base level.  Duration is not
// considered at all.  Time is counted from some zero.
NaReal      NaControlLawItem::Function (NaReal time, NaReal Dt)
{
    NaReal  time_in_period = fmod(time, period);
    NaReal  half_period = period * 0.5;

    switch(kind){

    /*********************************
     *         Non-periodic          *
     *********************************/

    case clkConstant:
        return value;

    /*********************************
     *           Periodic            *
     *********************************/

    case clkSine:
        return 0.5 * value * (1. + sin(2 * M_PI * time / period - 0.5 * M_PI));

    case clkSineSine:
        return 0.5 * value *
            (1. + sin(M_PI * 0.5 * sin(2 * M_PI * time / period - 0.5 * M_PI)));

    case clkPike:
        if(time_in_period < Dt)
            // almost start of the period
            return value;
        else
            // almost half of the period
            return 0.;

    case clkMeander:
        if(time_in_period < half_period)
            // first half of the period
            return value;
        else
            // second half of the period
            return 0.;
    }
    return 0.;
}

//---------------------------------------------------------------------------
// Find control law item for this time and get start time
// for this item
int     NaControlLaw::FindItem (NaReal time, NaReal& start)
{
    if(time < 0.0){
        start = 0.0;
        return 0;
    }

    NaReal  Ttotal = 0.0;
    for(int i = 0; i < items.count(); ++i){
        Ttotal += items[i].duration;
        if(time < Ttotal){
            start = Ttotal - items[i].duration;
            return i;
        }
    }

    // Prolongate the last item's value
    start = Ttotal - items[items.count() - 1].duration;
    return items.count() - 1;
}

//---------------------------------------------------------------------------
// Calculate total time
NaReal  NaControlLaw::TotalTimeLength ()
{
    NaReal  Ttotal = 0.0;
    for(int i = 0; i < items.count(); ++i){
        Ttotal += items[i].duration;
    }
    return Ttotal;
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void    NaControlLaw::Reset ()
{
    NaPrintLog("Control law (set point) init\n");

    switch(source){

    case clsRandom:
        lin_rand.Reset();
        break;

    case clsDescription:
        NaPrintLog("..description (%d items) with total duration %g s\n",
                   items.count(), TotalTimeLength());
        break;

    case clsManual:
        NaPrintLog("..manual from value %g\n", start_value);
        current_value = start_value;
        break;

    case clsFromFile:
        NaPrintLog("..from file \"%s\"\n", filepath);
        try{
            // Open the file
            dataio = OpenInputDataFile(filepath);
            dataio->GoStartRecord();
            prev_index = 0;
        }
        catch(NaException exCode){
            // Handle file exceptions
            dataio = NULL;
            NaPrintLog("..can't open file \"%s\": exception code %d\n",
                       filepath, -1);
        }
        break;
    }
}


//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void    NaControlLaw::Function (NaReal* x, NaReal* y)
{
    switch(source){

    case clsRandom:
        lin_rand.Function(x, y);
        return;

    case clsDescription:
        {
        NaReal  Tcur = Timer().CurrentTime();
        NaReal  Tstart;
        int     i, Icur = FindItem(Tcur, Tstart);
        NaReal  Vstart = 0.0;

        if(clkConstant != items[Icur].kind){
            // For each periodic kind
            for(i = 0; i < Icur; ++i)
                // For each non-periodic kind
                if(clkConstant == items[i].kind)
                    Vstart =
                        items[i].Function(Timer().GetSamplingRate()
                                          * Timer().CurrentIndex(),
                                          Timer().GetSamplingRate());
            current_value =
                Vstart + items[Icur].Function(Tcur - Tstart,
                                              Timer().GetSamplingRate());
        }
        else{
            // For each non-periodic kind
            current_value =
                items[Icur].Function(Tcur - Tstart,
                                     Timer().GetSamplingRate());
        }}
        break;

    case clsManual:
        // nothing to change
        break;

    case clsFromFile:
        //while(prev_index < Timer().CurrentIndex()){
        //    ++prev_index;
        //    if(dataio)  dataio->GoNextRecord();
        //}
        if(dataio){
            current_value = dataio->GetValue();
            dataio->GoNextRecord();
        }else{
            current_value = 0.;
        }
        break;
    }
    *y = current_value;
}


//---------------------------------------------------------------------------
// Return current value of the control law
NaReal  NaControlLaw::CurrentValue ()
{
    NaReal  x = 0.0/* dummy */, y = 0.0;
    Function(&x, &y);   // + implicit current time and index
    return y;
}


//---------------------------------------------------------------------------
// Setup timer
void
NaControlLaw::SetupTimer (NaTimer* pTimer_)
{
    NaUnit::SetupTimer(pTimer_);

    lin_rand.SetupTimer(&Timer());
}


//---------------------------------------------------------------------------
