/* NaRandom.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "NaMath.h"
#include "NaExcept.h"
#include "NaRandom.h"

//---------------------------------------------------------------------------
// Serialize NaRandomDistribution value
const char*   RandomDistrToStrIO (NaRandomDistribution eRD)
{
    switch(eRD){
    case rdGaussNormal:
        return "gaussian";
    case rdUniform:
        return "uniform";
    default:
        break;
    }
    return "???";
}

//---------------------------------------------------------------------------
// Deserialize NaRandomDistribution value
NaRandomDistribution    StrToRandomDistrIO (const char* str)
{
    if(NULL == str)
        throw(na_null_pointer);
    if(!strcmp(RandomDistrToStrIO(rdGaussNormal), str))
        return rdGaussNormal;
    else if(!strcmp(RandomDistrToStrIO(rdUniform), str))
        return rdUniform;
    return __rdNumber;
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void    NaRandomSequence::Save (NaDataStream& ds)
{
    ds.PutComment("Kinds of random distribution: 'gaussian', 'uniform'");
    ds.PutF("Distribution kind", "%s", RandomDistrToStrIO(eRandomDistr));
    ds.PutF("Gaussian distribution parameters (m D)", "%g %g",
            fMean, fStdDev);
    ds.PutF("Uniform distribution parameters (min max)", "%g %g",
            fMin, fMax);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaRandomSequence::Load (NaDataStream& ds)
{
    char    szBuf[1024];

    ds.GetF("%s", szBuf);
    eRandomDistr = StrToRandomDistrIO(szBuf);

    ds.GetF("%lg %lg", &fMean, &fStdDev);
    ds.GetF("%lg %lg", &fMin, &fMax);
}

//---------------------------------------------------------------------------
// Construct by default: (1,1)
NaRandomSequence::NaRandomSequence ()
: NaUnit(1, 1), NaConfigPart("RandomNumberGenerator")
{
    eRandomDistr = rdGaussNormal;
    fMean = 0.;
    fStdDev = 1.;
    fMin = 0.;
    fMax = 1.;
}

//---------------------------------------------------------------------------
// Copying construct
NaRandomSequence::NaRandomSequence (const NaRandomSequence& rRandSeq)
: NaUnit(rRandSeq), NaConfigPart(rRandSeq)
{
    eRandomDistr = rRandSeq.eRandomDistr;
    fMean = rRandSeq.fMean;
    fStdDev = rRandSeq.fStdDev;
    fMin = rRandSeq.fMin;
    fMax = rRandSeq.fMax;
}

//---------------------------------------------------------------------------
// Destroy the object
NaRandomSequence::~NaRandomSequence ()
{
    // Dummy
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void    NaRandomSequence::Reset ()
{
    // Setup random generator from the system timer
    reset_rand();
}


//---------------------------------------------------------------------------
// Compute pseudo-random output that depends on described
// random sequence law.  Input x does not matter.
void    NaRandomSequence::Function (NaReal* x, NaReal* y)
{
    if(NULL == y)
        throw(na_null_pointer);

    switch(eRandomDistr){

    case rdGaussNormal:
        *y = rand_gaussian(fMean, fStdDev);
        break;
    case rdUniform:
        *y = rand_unified(fMin, fMax);
        break;
    default:
        *y = 0.;    // Not a random
        break;
    }
}

//---------------------------------------------------------------------------
// Setup generator law
void    NaRandomSequence::SetDistribution (NaRandomDistribution eRD)
{
    eRandomDistr = eRD;
}

//---------------------------------------------------------------------------
// Setup distribution parameters
void    NaRandomSequence::SetGaussianParams (NaReal mean, NaReal stddev)
{
    fMean = mean;
    fStdDev = stddev;
}

//---------------------------------------------------------------------------
// Setup distribution parameters
void    NaRandomSequence::SetUniformParams (NaReal min, NaReal max)
{
    fMin = min;
    fMax = max;
}

//---------------------------------------------------------------------------
// Get actual generator law
NaRandomDistribution    NaRandomSequence::GetDistribution () const
{
    return eRandomDistr;
}

//---------------------------------------------------------------------------
// Get distribution parameters
void    NaRandomSequence::GetGaussianParams (NaReal& mean, NaReal& stddev) const
{
    mean = fMean;
    stddev = fStdDev;
}

//---------------------------------------------------------------------------
// Get distribution parameters
void    NaRandomSequence::GetUniformParams (NaReal& min, NaReal& max) const
{
    min = fMin;
    max = fMax;
}

//---------------------------------------------------------------------------
// Store parameters to file
void    NaRandomSequence::Serialize (FILE* fp) const
{
    if(NULL == fp)
        throw(na_null_pointer);

    fprintf(fp, "  Distribution = %s\n"
                "  GaussianMean = %g\n"
                "  GaussianStdDev = %g\n"
                "  UniformMin = %g\n"
                "  UniformMax = %g\n",
            RandomDistrToStrIO(eRandomDistr),
            fMean, fStdDev, fMin, fMax);
}

//---------------------------------------------------------------------------
// Load parameters from file
// !!!Buggy due to bad stop conditionat '[' but not before it.
void    NaRandomSequence::Deserialize (FILE* fp)
{
    if(NULL == fp)
        throw(na_null_pointer);

#define BUFFER_SIZE     100
#define FIELD_DELIM     " ="
    char    *p, buf[BUFFER_SIZE+1];

    // Read file until its end or first line that starts with '['
    while(!feof(fp)){
        // Read the whole line
        fgets(buf, BUFFER_SIZE, fp);

        // Find the first non-space character
        p = buf;
        while(isspace(*p))
            ++p;
        if('\0' == *p)
            // Empty line - go to next one
            continue;
        if('[' == *p)
            // Start of the next section - break reading this one
            break;
        p = strtok(p, FIELD_DELIM);
        if(NULL == p)
            continue;
        else if(!strcmp(p, "Distribution")){
            if(NULL == (p = strtok(NULL, FIELD_DELIM)))
                continue;
            eRandomDistr = StrToRandomDistrIO(p);
        }else if(!strcmp(p, "GaussianMean")){
            if(NULL == (p = strtok(NULL, FIELD_DELIM)))
                continue;
            fMean = strtod(p, NULL);
        }else if(!strcmp(p, "GaussianStdDev")){
            if(NULL == (p = strtok(NULL, FIELD_DELIM)))
                continue;
            fStdDev = strtod(p, NULL);
        }else if(!strcmp(p, "UniformMin")){
            if(NULL == (p = strtok(NULL, FIELD_DELIM)))
                continue;
            fMin = strtod(p, NULL);
        }else if(!strcmp(p, "UniformMax")){
            if(NULL == (p = strtok(NULL, FIELD_DELIM)))
                continue;
            fMax = strtod(p, NULL);
        }
    }
}

//---------------------------------------------------------------------------

