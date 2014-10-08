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
  ds.PutF("Random sequence multioutput dimension", "%u", OutputDim());
  char szLabel[100];
  for(unsigned i = 0 ; i < OutputDim(); ++i)
    {
      sprintf(szLabel, "Distribution type; #%u", i);
      ds.PutF(szLabel, "%s", RandomDistrToStrIO(params[i].eRandomDistr));
      switch(params[i].eRandomDistr)
	{
	case rdGaussNormal:
	  ds.PutF("m D", "%g %g", params[i].fMean, params[i].fStdDev);
	  break;
	case rdUniform:
	  ds.PutF("min max", "%g %g", params[i].fMin, params[i].fMax);
	  break;
	default:
	  ds.PutComment("Unknown parameters");
	  break;
	}
    }
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void    NaRandomSequence::Load (NaDataStream& ds)
{
    char    szBuf[1024];
    unsigned nOutDim;

    ds.GetF("%u", &nOutDim);
    if(nOutDim < 1)
      throw(na_out_of_range);

    delete[] params;
    Assign(1, nOutDim);

    params = new Params[OutputDim()];

    for(unsigned i = 0 ; i < OutputDim(); ++i)
      {
	ds.GetF("%s", szBuf);
	params[i].eRandomDistr = StrToRandomDistrIO(szBuf);

	switch(params[i].eRandomDistr)
	  {
	  case rdGaussNormal:
	    ds.GetF("%lg %lg", &params[i].fMean, &params[i].fStdDev);
	    break;
	  case rdUniform:
	    ds.GetF("%lg %lg", &params[i].fMin, &params[i].fMax);
	    break;
	  }
      }
}

//---------------------------------------------------------------------------
// Construct by default: (1,1)
NaRandomSequence::NaRandomSequence ()
  : NaUnit(1, 1), NaConfigPart("RandomNumberGenerator")
{
  params = new Params[1];
  params[0].eRandomDistr = rdGaussNormal;
  params[0].fMean = 0.;
  params[0].fStdDev = 1.;
  params[0].fMin = 0.;
  params[0].fMax = 1.;
}

//---------------------------------------------------------------------------
// Construct arbitrary output dimension >=1
NaRandomSequence::NaRandomSequence (unsigned nOutDim)
  : NaUnit(nOutDim, 1), NaConfigPart("RandomNumberGenerator")
{
  params = new Params[OutputDim()];
  for(unsigned i = 0 ; i < OutputDim(); ++i)
    {
      params[i].eRandomDistr = rdGaussNormal;
      params[i].fMean = 0.;
      params[i].fStdDev = 1.;
      params[i].fMin = 0.;
      params[i].fMax = 1.;
    }
}

//---------------------------------------------------------------------------
// Copying construct
NaRandomSequence::NaRandomSequence (const NaRandomSequence& rRandSeq)
  : NaUnit(rRandSeq), NaConfigPart(rRandSeq)
{
  params = new Params[OutputDim()];
  for(unsigned i = 0 ; i < OutputDim(); ++i)
    {
      params[i].eRandomDistr = rRandSeq.params[i].eRandomDistr;
      params[i].fMean = rRandSeq.params[i].fMean;
      params[i].fStdDev = rRandSeq.params[i].fStdDev;
      params[i].fMin = rRandSeq.params[i].fMin;
      params[i].fMax = rRandSeq.params[i].fMax;
    }
}

//---------------------------------------------------------------------------
// Destroy the object
NaRandomSequence::~NaRandomSequence ()
{
  delete[] params;
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

    for(unsigned i = 0 ; i < OutputDim(); ++i)
      {
	Params *p = params + i;
	switch(p->eRandomDistr)
	  {
	  case rdGaussNormal:
	    y[i] = rand_gaussian(p->fMean, p->fStdDev);
	    break;
	  case rdUniform:
	    y[i] = rand_unified(p->fMin, p->fMax);
	    break;
	  default:
	    y[i] = 0.;    // Not a random
	    break;
	  }
      }
}

//---------------------------------------------------------------------------
// Setup generator law
void    NaRandomSequence::SetDistribution (NaRandomDistribution eRD,
					   unsigned iOut)
{
  if(iOut >= OutputDim())
    throw(na_out_of_range);

  params[iOut].eRandomDistr = eRD;
}

//---------------------------------------------------------------------------
// Setup distribution parameters
void    NaRandomSequence::SetGaussianParams (NaReal mean, NaReal stddev,
					     unsigned iOut)
{
  if(iOut >= OutputDim())
    throw(na_out_of_range);

  params[iOut].fMean = mean;
  params[iOut].fStdDev = stddev;
}

//---------------------------------------------------------------------------
// Setup distribution parameters
void    NaRandomSequence::SetUniformParams (NaReal min, NaReal max,
					    unsigned iOut)
{
  if(iOut >= OutputDim())
    throw(na_out_of_range);

  params[iOut].fMin = min;
  params[iOut].fMax = max;
}

//---------------------------------------------------------------------------
// Get actual generator law
NaRandomDistribution    NaRandomSequence::GetDistribution (unsigned iOut) const
{
  if(iOut >= OutputDim())
    throw(na_out_of_range);

  return params[iOut].eRandomDistr;
}

//---------------------------------------------------------------------------
// Get distribution parameters
void    NaRandomSequence::GetGaussianParams (NaReal& mean, NaReal& stddev,
					     unsigned iOut) const
{
  if(iOut >= OutputDim())
    throw(na_out_of_range);

  mean = params[iOut].fMean;
  stddev = params[iOut].fStdDev;
}

//---------------------------------------------------------------------------
// Get distribution parameters
void    NaRandomSequence::GetUniformParams (NaReal& min, NaReal& max,
					    unsigned iOut) const
{
  if(iOut >= OutputDim())
    throw(na_out_of_range);

  min = params[iOut].fMin;
  max = params[iOut].fMax;
}

#if 0
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
#endif
//---------------------------------------------------------------------------
