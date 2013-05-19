/* NaDDir.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <stdlib.h>

#include "NaDDir.h"

//---------------------------------------------------------------------------
NaDataDirector::NaDataDirector (unsigned nVolume, unsigned nFeedback)
{
    nDataVolume = nVolume;
    nFeedbackDepth = nFeedback;

    if(nDataVolume <= nFeedbackDepth){
        nDataVolume = 0;
        nFeedbackDepth = 0;
        //throw(na_bad_value);
    }

    eIterKind = __ddikNumber;
    nIterDone = 0;
    bUsed = new bool[GetEpochLen()];
}

//---------------------------------------------------------------------------
NaDataDirector::NaDataDirector (const NaDataDirector& dd)
{
    nDataVolume = dd.nDataVolume;
    nFeedbackDepth = dd.nFeedbackDepth;

    eIterKind = dd.eIterKind;
    nIterDone = dd.nIterDone;
    bUsed = new bool[GetEpochLen()];
    for(unsigned i = 0; i < GetEpochLen(); ++i){
        bUsed[i] = dd.bUsed[i];
    }
}

//---------------------------------------------------------------------------
NaDataDirector::~NaDataDirector ()
{
    delete bUsed;
}

//---------------------------------------------------------------------------
// Get index of startup impulse
unsigned
NaDataDirector::GetStartupImpulse ()
{
    return 0;
}

//---------------------------------------------------------------------------
// Split data director on two contiguous parts (part1, 1-part1)
void        NaDataDirector::Split (NaReal part1,
                                   NaDataDirector& dd1,
                                   NaDataDirector& dd2)
{
    if(part1 < 0.0 || part1 > 1.0)
        throw(na_bad_value);

    dd1.nDataVolume = (unsigned)(part1 * nDataVolume);
    if(dd1.nDataVolume < nFeedbackDepth)
        dd1.nDataVolume = 0;
    if(nDataVolume < dd1.nDataVolume + nFeedbackDepth)
        dd2.nDataVolume = 0;
    else if(0 == dd1.nDataVolume)
        dd2.nDataVolume = nDataVolume;
    else
        dd2.nDataVolume = nDataVolume - dd1.nDataVolume + nFeedbackDepth;

    if(dd1.nDataVolume < nFeedbackDepth)
        dd1.nFeedbackDepth = 0;
    else
        dd1.nFeedbackDepth = nFeedbackDepth;
    if(dd2.nDataVolume < nFeedbackDepth)
        dd2.nFeedbackDepth = 0;
    else
        dd2.nFeedbackDepth = nFeedbackDepth;

    dd1.eIterKind = __ddikNumber;
    dd2.eIterKind = __ddikNumber;

    dd1.nIterDone = 0;
    dd2.nIterDone = 0;

    dd1.bUsed = new bool[dd1.GetEpochLen()];
    dd2.bUsed = new bool[dd2.GetEpochLen()];
}

//---------------------------------------------------------------------------
// Get data set volumne
unsigned    NaDataDirector::GetVolume () const
{
    return nDataVolume;
}

//---------------------------------------------------------------------------
// Get number of data set learning/testing units
unsigned    NaDataDirector::GetEpochLen () const
{
    return nDataVolume - nFeedbackDepth;
}

//---------------------------------------------------------------------------
// Iterations start
void        NaDataDirector::StartEpoch (NaDataDirIterKind ik)
{
    switch(ik){

    case ddikSequence:
        eIterKind = ik;
        iIndex = nIterDone = 0;
        break;

    case ddikRandomized:
        eIterKind = ik;
        iIndex = nIterDone = 0;
        for(unsigned i = 0; i < GetEpochLen(); ++i){
            bUsed[i] = false;
        }
        break;
    }
}

//---------------------------------------------------------------------------
// Iteration step returns false if epoch finished
bool        NaDataDirector::GoNextIndex ()
{
    if(nIterDone >= GetEpochLen())
        return false;

    switch(eIterKind){

    case ddikSequence:
        iIndex = nIterDone;
        break;

    case ddikRandomized:
        do{
            iIndex = (unsigned)(GetEpochLen() *
				((double) rand() / (RAND_MAX + 1)));
            if(iIndex >= GetEpochLen())
                continue;
        }while(bUsed[iIndex]);

        bUsed[iIndex] = true;
        break;
    }
    ++nIterDone;

    return true;
}

//---------------------------------------------------------------------------
// Get current index of the unit (0..GetEpochLen()-1)
unsigned    NaDataDirector::GetEpochIndex ()
{
    return iIndex + nFeedbackDepth;
}

//---------------------------------------------------------------------------

