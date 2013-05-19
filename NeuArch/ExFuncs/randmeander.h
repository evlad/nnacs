/* randmeander.h */
#ifndef __randmeander_h
#define __randmeander_h

#include <NaExFunc.h>
#include <NaDataIO.h>

//---------------------------------------------------------------------------
// Class for external function which implements random meander signal
// on output.  This object is not a function but a generator!
//
//        <-L->
// A ^    +---+
//   |    |   |          +--+
//   |    |   +-+        |  |
//   | ---+     |    +---+  |___ ...
//   |          +----+      
// 0 +------------------------------>
//                                   T
//
// Randomization parameters:
//  - [Amin, Amax] - uniform distribution of amplitudes
//  - Lconst       - constant length of constant amplitude
//                                or
//  - [Lmin, Lmax] - uniform distribution of length of constant amplitude
// Lengths must be positive integer.
//
// See drandmea command line utility for the same behavior.

class NaRandMeanderFunc : public NaExternFunc
{
public:

    /// Make empty (y=x) function
    NaRandMeanderFunc ();

    /// Make function with given options and initial vector
    /// options (numbers): Amin Amax Lconst
    ///  or
    /// options (numbers): Amin Amax Lmin Lmax
    /// where:
    ///  - Amin   - for uniform distribution of amplitudes
    ///  - Amax   - for uniform distribution of amplitudes
    ///  - Lmin   - for uniform distribution of length of constant amplitude
    ///  - Lmax   - for uniform distribution of length of constant amplitude
    ///  - Lconst - constant length of constant amplitude
    /// initial: not used
    NaRandMeanderFunc (char* szOptions, NaVector& vInit);

    /// Destructor
    virtual ~NaRandMeanderFunc ();

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

    /// Characteristics of amplitude uniform distribution
    NaReal	Amin, Amax;

    /// Characteristics of amplitude uniform distribution
    int		Lmin, Lmax;

    /// Constant length of constant amplitude
    int		Lconst;

    /// Internal counter for constant amplitude
    int		Lcounter;

    /// Internal storage for constant amplitude
    NaReal	Aconst;
};


#endif /* randmeander.h */
