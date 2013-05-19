/* NaLinCon.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <string.h>

#include "NaLogFil.h"
#include "NaLinCon.h"


//---------------------------------------------------------------------------
const char*
LinControlKindToStrIO (NaLinearControlKind eK)
{
    switch(eK){
    case lckGain:
        return "gain";
    case lckInertial:
        return "inertial";
    case lckIntegral:
        return "integral";
    case lckDifferential:
        return "differential";
    case lckDiffStatic:
        return "diffstatic";
    case lckOscillation:
        return "oscillation";
    case lckConservative:
        return "conservative";
    case lckDelay:
        return "delay";
    }
    return "???";
}

//---------------------------------------------------------------------------
NaLinearControlKind
StrToLinControlKindIO (const char* s)
{
    if(NULL == s)
        throw(na_null_pointer);

    if(!strcmp(LinControlKindToStrIO(lckGain), s))
        return lckGain;
    else if(!strcmp(LinControlKindToStrIO(lckInertial), s))
        return lckInertial;
    else if(!strcmp(LinControlKindToStrIO(lckIntegral), s))
        return lckIntegral;
    else if(!strcmp(LinControlKindToStrIO(lckDifferential), s))
        return lckDifferential;
    else if(!strcmp(LinControlKindToStrIO(lckDiffStatic), s))
        return lckDiffStatic;
    else if(!strcmp(LinControlKindToStrIO(lckOscillation), s))
        return lckOscillation;
    else if(!strcmp(LinControlKindToStrIO(lckConservative), s))
        return lckConservative;
    else if(!strcmp(LinControlKindToStrIO(lckDelay), s))
        return lckDelay;
    return __lckNumber;
}


//---------------------------------------------------------------------------
//                          class NaLinearUnit
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Make default unit (gain K=1.0)
NaLinearUnit::NaLinearUnit ()
:   NaConfigPart("LinearUnit")
{
    act = false;
    kind = lckGain;
    K = 1.0;
    T = 0.0;
    Rho = 0.0;
    Tau = 0.0;
}


//---------------------------------------------------------------------------
// Make an explicit copy of the unit
NaLinearUnit::NaLinearUnit (const NaLinearUnit& orig)
:   NaConfigPart(orig)
{
    act = orig.act;
    kind = orig.kind;
    K = orig.K;
    T = orig.T;
    Rho = orig.Rho;
    Tau = orig.Tau;
}


//---------------------------------------------------------------------------
// Assignment
NaLinearUnit&   NaLinearUnit::operator= (const NaLinearUnit& orig)
{
    act = orig.act;
    kind = orig.kind;
    K = orig.K;
    T = orig.T;
    Rho = orig.Rho;
    Tau = orig.Tau;

    yd = orig.yd;

    return *this;
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaLinearUnit::Save (NaDataStream& ds)
{
    ds.PutF(NULL, "%d %s %g %g %g %g",
            act, LinControlKindToStrIO(kind), K, T, Rho, Tau);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaLinearUnit::Load (NaDataStream& ds)
{
    char    szBuf[1024];

    ds.GetF("%d %s %lg %lg %lg %lg", &act, szBuf, &K, &T, &Rho, &Tau);

    kind = StrToLinControlKindIO(szBuf);
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void    NaLinearUnit::Reset ()
{
    int d;

    // Reset delay vector
    yd.clean();

    switch(kind){
    case lckGain:
    case lckDifferential:
        // No delay is needed
        break;
    case lckInertial:
    case lckIntegral:
    case lckDiffStatic:
        // 1st order units
        yd.addh(new NaReal(0.0));
        break;
    case lckOscillation:
    case lckConservative:
        // 2nd order units
        yd.addh(new NaReal(0.0));
        yd.addh(new NaReal(0.0));
        break;
    case lckDelay:
        // High order unit
        d = 1 + (int)(Tau / Timer().GetSamplingRate());
        while(d-- != 0)
            yd.addh(new NaReal(0.0));
        break;
    }
}

//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
// Meaning nInDim=1 and nOutDim=1
void    NaLinearUnit::Function (NaReal* x, NaReal* y)
{
    NaReal  &X = (*x), &Y = (*y);

    // Non-actual - direct assignment
    if(!act){
        Y = X;
        return;
    }

    NaReal  Yd;
    NaReal  dT = Timer().GetSamplingRate();
    NaReal  dT2 = dT * dT;
    NaReal  T2 = T * T;

    // Switch among different kinds
    switch(kind){
    case lckGain:
        Yd = Y = K * X;
        break;
    case lckInertial:
        Yd = Y = (K * dT * X + T * yd[0]) / (T + dT);
        /*NaPrintLog("dT=%g, X=%g, (K=%g, T=%g), yd=%g ==> Y=%g\n",
                   dT, X, K, T, yd[0], Y);*/
        break;
    case lckIntegral:
        Yd = Y = yd[0] + K * dT * X;
        break;
    case lckDifferential:
        if(0 == Timer().CurrentIndex())
            Yd = 0;
        else
            Yd = Y = K * (X - Xprev) / dT;
        break;
    case lckDiffStatic:
        if(0 == Timer().CurrentIndex())
            Yd = Y = (K * dT * X + T * yd[0]) / (T + dT);
        else
            Yd = Y = (K * (Tau * (X - Xprev) +
                           dT * X) + T * yd[0]) / (T + dT);
        break;
    case lckOscillation:
        Yd = Y = (K * dT2 * X + T2 * (2 * yd[0] - yd[1]) +
                  2 * Rho * T * dT * yd[0]) / (T2 + 2 * Rho * T * dT + dT2);
        break;
    case lckConservative:
        Yd = Y = (K * dT2 * X + T2 * (2 * yd[0] - yd[1])) / (T2 + dT2);
        break;
    case lckDelay:
        Y = yd[yd.count() - 1];
        Yd = X;
        break;
    }

    // Store previous X for differencial chains
    Xprev = X;

    // Store delayed Y for inertial and delay chains
    ShiftDelay(Yd);
}


//---------------------------------------------------------------------------
void            NaLinearUnit::ShiftDelay (NaReal yn)
{
    if(0 == yd.count())
        return;

    // Remove last
    yd.remove(yd.count() - 1);
    // Add first
    yd.addl(new NaReal(yn));
}


//---------------------------------------------------------------------------
//                          class NaLinearUnitsChain
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Construct the object
NaLinearUnitsChain::NaLinearUnitsChain (unsigned nUnits)
:   NaConfigPart("LinearUnitsChain")
{
    unsigned    i;
    char        szBuf[20];
    for(i = 0; i < nUnits; ++i){
        addh(new NaLinearUnit());

        // Assign instance name to each unit
        sprintf(szBuf, "%d", i);
        fetch(i).SetInstance(szBuf);
    }
    if(count() > 0)
        fetch(0).act = true;

    nMaxDim = 0;
    iobuf[0] = iobuf[1] = NULL;
}

//---------------------------------------------------------------------------
// Copying construct the object
NaLinearUnitsChain::NaLinearUnitsChain (const NaLinearUnitsChain& rUnitsChain)
:   NaConfigPart(rUnitsChain)
{
    int i;
    for(i = 0; i < rUnitsChain.count(); ++i)
        addh(new NaLinearUnit(rUnitsChain.get(i)));
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaLinearUnitsChain::Save (NaDataStream& ds)
{
    ds.PutF("Number of units in the chain", "%u", count());
    ds.PutComment("Unit description by fields:");
    ds.PutComment(" 1 - actuality (1-on/0-off)");
    ds.PutComment(" 2 - kind of unit ('gain','inertial','integral','delay',"
                  "'differencial','diffstatic','oscillation','conservative')");
    ds.PutComment(" 3 - gain constant (K)");
    ds.PutComment(" 4 - time constant (T)");
    ds.PutComment(" 5 - oscillation fading (Rho)");
    ds.PutComment(" 6 - pure delay time constant/"
                  "diff.static top time constant (Tau)");
    for(int i = 0; i < count(); ++i)
        fetch(i).Save(ds);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaLinearUnitsChain::Load (NaDataStream& ds)
{
    int     nUnits, i;
    ds.GetF("%u", &nUnits);
    for(i = 0; i < nUnits; ++i){
        if(i < count())
            fetch(i).Load(ds);
        else
            NaPrintLog("Unit %d skipped due to volume limit.", i);
    }
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void    NaLinearUnitsChain::Reset ()
{
    fetch(0).Reset();
    nMaxDim = InputDim() > OutputDim()? InputDim(): OutputDim();

    for(int i = 1; i < count(); ++i){

        fetch(i).Reset();
        if(get(i - 1).OutputDim() != get(i).InputDim())
            throw(na_size_mismatch);
        if(nMaxDim < get(i).InputDim())
            nMaxDim = get(i).InputDim();
        if(nMaxDim < get(i).OutputDim())
            nMaxDim = get(i).OutputDim();
    }

    // Free previously allocated
    delete iobuf[0];
    delete iobuf[1];

    // Allocate buffers for input/output vectors
    iobuf[0] = new NaReal[nMaxDim];
    iobuf[1] = new NaReal[nMaxDim];
    if(NULL == iobuf[0] || NULL == iobuf[1])
        throw(na_bad_alloc);
}

//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void    NaLinearUnitsChain::Function (NaReal* x, NaReal* y)
{
    if(NULL == x || NULL == y)
        throw(na_null_pointer);

    int i;

    // Chain function computation
    memcpy(iobuf[0], x, sizeof(NaReal) * InputDim());
    for(i = 0; i < count(); ++i){
        fetch(i).Function(iobuf[i%2], iobuf[(i + 1)%2]);
    }
    memcpy(y, iobuf[i%2], sizeof(NaReal) * OutputDim());
}

//---------------------------------------------------------------------------
// Return input dimension
unsigned    NaLinearUnitsChain::InputDim () const
{
    return get(0).InputDim();
}

//---------------------------------------------------------------------------
// Return output dimension
unsigned    NaLinearUnitsChain::OutputDim () const
{
    return get(count() - 1).OutputDim();
}


//---------------------------------------------------------------------------
// Setup timer
void
NaLinearUnitsChain::SetupTimer (NaTimer* pTimer_)
{
    NaUnit::SetupTimer(pTimer_);

    int i;
    for(i = 0; i < count(); ++i){
        fetch(i).SetupTimer(&Timer());
    }
}


//---------------------------------------------------------------------------

