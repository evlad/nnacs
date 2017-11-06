/* NaUnit.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include "NaUnit.h"

//---------------------------------------------------------------------------
//                               class PNNA_API NaUnit
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Construct the object
NaUnit::NaUnit (unsigned nInDim_, unsigned nOutDim_, unsigned nFbDepth_)
{
    pTimer = &TheTimer;
    Assign(nInDim_, nOutDim_, nFbDepth_);
}

//---------------------------------------------------------------------------
// Copying construct the object
NaUnit::NaUnit (const NaUnit& rUnit)
{
    operator=(rUnit);
}

//---------------------------------------------------------------------------
// Destroy the object
NaUnit::~NaUnit ()
{
    // empty
}

//---------------------------------------------------------------------------
// Assign the object
NaUnit&
NaUnit::operator= (const NaUnit& rUnit)
{
    pTimer = rUnit.pTimer;
    nInDim = rUnit.nInDim;
    nOutDim = rUnit.nOutDim;
    nFbDepth = rUnit.nFbDepth;
    return *this;
}

//---------------------------------------------------------------------------
NaUnit&
NaUnit::Assign (unsigned nInDim_, unsigned nOutDim_, unsigned nFbDepth_)
{
    if(nInDim_ == 0 || nOutDim_ == 0)
        throw(na_bad_value);
    nInDim = nInDim_;
    nOutDim = nOutDim_;
    nFbDepth = nFbDepth_;

    return *this;
}

//---------------------------------------------------------------------------
// Setup timer
void
NaUnit::SetupTimer (NaTimer* pTimer_)
{
    if(NULL == pTimer_)
        throw(na_null_pointer);
    pTimer = pTimer_;
}


//---------------------------------------------------------------------------
// Access to timer
NaTimer&
NaUnit::Timer () const
{
    return *pTimer;
}

//---------------------------------------------------------------------------
// Return input dimension
unsigned
NaUnit::InputDim () const
{
    return nInDim;
}

//---------------------------------------------------------------------------
// Return output dimension
unsigned
NaUnit::OutputDim () const
{
    return nOutDim;
}

//---------------------------------------------------------------------------
// Return feedback depth
unsigned
NaUnit::FeedbackDepth () const
{
    return nFbDepth;
}

//---------------------------------------------------------------------------
// Supply unit with feedback values on the starting steps of
// unit perfomance.  Number of values see in NeedFeedback().
void
NaUnit::FeedbackValues (NaReal* fb)
{
    // No feedback loop by default
}

//---------------------------------------------------------------------------
// Dimension of feedback vector needed by unit at current time step.
// 0 means no feedback is needed.
unsigned
NaUnit::NeedFeedback ()
{
    unsigned    nTimeIndex = (unsigned)Timer().CurrentIndex();

    if(nTimeIndex >= nFbDepth)
        // All needed feedback values are already computed by the unit
        return 0;
    // Some feedback values are still needed.
    return nFbDepth - nTimeIndex;
}

//---------------------------------------------------------------------------
void
NaUnit::PrintLog () const
{
    NaPrintLog("NaUnit(this=%p): InputDim=%u OutputDim=%u FeedbackDepth=%u\n"
               "+ Timer(%p, dT=%g s): T=%g s, I=%u\n",
               this, InputDim(), OutputDim(), FeedbackDepth(),
               &Timer(), Timer().GetSamplingRate(), Timer().CurrentTime(),
               Timer().CurrentIndex());
}

//---------------------------------------------------------------------------
/// Apply function repeatedly for the given input vectors, putting the
/// result to output ones.  Reset() is not called inside.
/// \return true in case of success and false if size of input
/// vector of function does not match to the size of input vector
/// in data set.
bool
NaUnit::Function (NaDynAr<NaVector>& rVecIn, NaDynAr<NaVector>& rVecOut)
{
    rVecOut.clean();
    NaVector	vResult(OutputDim());
    NaReal	*pOut = &vResult.fetch(0);
    for(unsigned i = 0; i < rVecIn.count(); ++i) {
	if(InputDim() != rVecIn[i].dim())
	    return false;
	NaReal	*pIn = &rVecIn[i].fetch(0);
	Function(pIn, pOut);
	rVecOut.addh(vResult);
    }
    return true;
}

//---------------------------------------------------------------------------
