//-*-C++-*-
/* NaUnit.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaUnitH
#define NaUnitH
//---------------------------------------------------------------------------

#include <NaGenerl.h>
#include <NaTimer.h>
#include <NaDynAr.h>
#include <NaLogFil.h>

//---------------------------------------------------------------------------
// Common class for control scheme unit representation
class NaUnit : public NaLogging
{
public:

    // Construct the object
    NaUnit (unsigned nInDim_ = 1, unsigned nOutDim_ = 1,
            unsigned nFbDepth_ = 0);

    // Copying construct the object
    NaUnit (const NaUnit& rUnit);

    // Destroy the object
    virtual ~NaUnit ();

    // Assign the object
    NaUnit&     operator= (const NaUnit& rUnit);
    NaUnit&     Assign (unsigned nInDim_ = 1, unsigned nOutDim_ = 1,
                        unsigned nFbDepth_ = 0);

    // Access to timer
    virtual NaTimer&    Timer () const;

    // Setup timer
    virtual void        SetupTimer (NaTimer* pTimer_);

    // Reset operations, that must be done before new modelling
    // session will start.  It's guaranteed that this reset will be
    // called just after Timer().ResetTime().
    virtual void    Reset () = 0;

    // ATTENTION!  It's guaranteed that this->Function() will be called
    // only once for each time step.  Index of the time step and current
    // time can be got from Timer().

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y) = 0;

    // ATTENTION!  Feedback loop is organized in derived class
    // internally.  NaUnit does not provide standard methods as
    // a basis for such property.  But even an internal feedback
    // loop needs some support on the first steps of perfomance:
    // initial values.  Next two methods provide standard way for
    // supplying an object with initial values.

    // Supply unit with feedback values on the starting steps of
    // unit perfomance.  Number of values can be got from NeedFeedback().
    // Needs to be redefined to get meaningful feedback property.
    virtual void    FeedbackValues (NaReal* fb);

    // Dimension of feedback vector needed by unit at current time step.
    // 0 means no feedback is needed.  Does not need to be redefined.
    virtual unsigned    NeedFeedback ();

    // Return input dimension
    virtual unsigned    InputDim () const;

    // Return output dimension
    virtual unsigned    OutputDim () const;

    // Return feedback depth
    virtual unsigned    FeedbackDepth () const;

    /*=====================*
     * NaLogging inherited *
     *=====================*/

    virtual void    PrintLog () const;

private:

    // Reference to timer object
    NaTimer     *pTimer;

    // Dimensions of input and output
    unsigned    nInDim, nOutDim;

    // Depth of the internal feedback loop
    // 0 - means no feedback
    // 1 - means using Out(t) = Func(In(t), Out(t-1)
    // n - means using Out(t) = Func(In(t), Out(t-1), ...Out(t-n))
    unsigned    nFbDepth;
};

//---------------------------------------------------------------------------
#endif
