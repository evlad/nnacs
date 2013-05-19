/* randunified.h */
#ifndef __randunified_h
#define __randunified_h

#include <NaExFunc.h>
#include <NaDataIO.h>

//---------------------------------------------------------------------------
// Class for external function which implements random signal with
// uniform distribution on output.  Maximum random step is limited by
// preset value.  This object is not a function but a generator!
//
// Amax ^
//      |   +-+
//      |  /    +
//      | +      \
//      |         +
// Amin +------------>
//                     T
//
// Randomization parameters:
//  - [Amin, Amax]  - range of output value
//  - Amaxstep - maximum random step (absolute value)

class NaRandUnifiedFunc : public NaExternFunc
{
public:

    /// Make empty (y=x) function
    NaRandUnifiedFunc ();

    /// Make function with given options and initial vector
    /// options (numbers): Amin Amax Amaxstep
    /// where:
    ///  - Amin     - low bound of output value
    ///  - Amax     - high bound of output value
    ///  - Amaxstep - maximum random step (absolute value)
    /// initial: [Afirst]
    ///  - Afirst   - to start with (middle point between Amin and
    ///               Amax by default)
    NaRandUnifiedFunc (char* szOptions, NaVector& vInit);

    /// Destructor
    virtual ~NaRandUnifiedFunc ();

    /// Reset operations, that must be done before new modelling
    /// session will start.  It's guaranteed that this reset will be
    /// called just after Timer().ResetTime().
    virtual void	Reset ();

    // ATTENTION!  It's guaranteed that this->Function() will be called
    // only once for each time step.  Index of the time step and current
    // time can be got from Timer().

    /// Compute output on the basis of internal parameters,
    /// stored state: y=F(t,p)  INPUT x IS NOT USED!
    virtual void	Function (NaReal* x, NaReal* y);

protected:

    /// Characteristics of output uniform distribution
    NaReal	Amin, Amax;

    /// Maximum step with uniform distribution
    NaReal	Amaxstep;

    /// First value
    NaReal	Afirst;

    /// Previous value
    NaReal	Aprev;
};


#endif /* randunified.h */
