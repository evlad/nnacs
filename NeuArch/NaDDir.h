//-*-C++-*-
/* NaDDir.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaDDirH
#define NaDDirH

#include <NaGenerl.h>
#include <NaExcept.h>


//---------------------------------------------------------------------------
// Selection way
enum NaDataDirIterKind {
    ddikSequence = 0,
    ddikRandomized,
    __ddikNumber
};

//---------------------------------------------------------------------------
// Data flow control.  Must be used with related data storage object with
// random access (by index).  Vector of items is an example. 
class NaDataDirector {
public:

    NaDataDirector (unsigned nVolume, unsigned nFeedback = 0);
    NaDataDirector (const NaDataDirector& dd);
    virtual ~NaDataDirector ();

    // Get data set volumne
    unsigned    GetVolume () const;

    // Get number of data set learning/testing units
    unsigned    GetEpochLen () const;

    // Iterations start
    void        StartEpoch (NaDataDirIterKind ik);

    // Iteration step returns false if epoch finished
    bool        GoNextIndex ();

    // Get current index of the unit (0..GetEpochLen()-1)
    unsigned    GetEpochIndex ();

    // Get index of startup impulse
    unsigned    GetStartupImpulse ();

    // Split data director on two contiguous parts (part1, 1-part1)
    void        Split (NaReal part1, NaDataDirector& dd1,
                       NaDataDirector& dd2);

private:

    unsigned    nDataVolume;
    unsigned    nFeedbackDepth;

    NaDataDirIterKind   eIterKind;
    unsigned    nIterDone;  // 0..GetEpochLen()
    unsigned    iIndex;     // current index: 0..GetEpochLen()-1
    bool        *bUsed;
};


//---------------------------------------------------------------------------
#endif
