/* randmeander.h */
#ifndef __randmeander_h
#define __randmeander_h

#include <NaExFunc.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Class for external function which implements random meander signal
// on output.  This object is not a function but a generator!
//
// Multidimensional output
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
//  - Lconst       - constant length of constant amplitude (dim=1)
//                                or
//  - [Lmin, Lmax] - uniform distribution of length of constant amplitude (dim>=1)
// Lengths must be positive integer.
//
// See drandmea command line utility for the same behaviour.

class NaRandMeanderFunc : public NaExternFunc
{
public:

    /// Make empty (y=x) function
    NaRandMeanderFunc ();

    /// Make function with given options and initial vector
    /// options (numbers): Amin Amax Lconst
    ///  or
    /// options (numbers): Amin1 Amax1 Lmin1 Lmax1 [Amin2 Amax2 Lmin2 Lmax2 ...]
    /// where:
    ///  - AminN  - for uniform distribution of amplitudes
    ///  - AmaxN  - for uniform distribution of amplitudes
    ///  - LminN  - for uniform distribution of length of constant amplitude
    ///  - LmaxN  - for uniform distribution of length of constant amplitude
    ///  - Lconst - constant length of constant amplitude
    ///  - N      - dimension index for multiple outputs
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

  struct DimDescr {

    /// Characteristics of amplitude uniform distribution
    NaReal	Amin, Amax;

    /// Characteristics of amplitude uniform distribution
    int		Lmin, Lmax;

    /// Internal counter for constant amplitude
    int		Lcounter;

    /// Internal storage for constant amplitude
    NaReal	Aconst;

    DimDescr () : Amin(0.0), Amax(0.0), Lmin(0), Lmax(0),
		  Lcounter(0), Aconst(0.0) {}

  };

  /// Array of dimension descriptors
  DimDescr	*ddescr;
  
  /// Number of items in the array
  unsigned	n_ddescr;

  /// Add new dimension descriptor to the array
  void		ddescr_addh (const DimDescr& dd);
  
};


#endif /* randmeander.h */
