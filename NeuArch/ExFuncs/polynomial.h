/* polynomial.h */
#ifndef __polynomial_h
#define __polynomial_h

#include <NaExFunc.h>
#include <NaDynAr.h>

//---------------------------------------------------------------------------
// Class for external polynomial function:
//          n         n-1
// y = k * x + k   * x   + ... + k
//      n       n-1               0
//
// where:
//  n    - polynom order
//  k[i] - coefficients
//  x    - input
//  y    - output

class NaPolynomialFunc : public NaExternFunc
{
public:

    /// Make empty (y=x) function
    NaPolynomialFunc ();

    /// Make function with given options and initial vector
    /// options: k[n] k[n-1] ... k[0]
    ///  -  n    - polynom order
    ///  -  k[i] - coefficients
    /// initial: -
    NaPolynomialFunc (char* szOptions, NaVector& vInit);

    /// Destructor
    virtual ~NaPolynomialFunc ();

    /// Reset operations, that must be done before new modelling
    /// session will start.  It's guaranteed that this reset will be
    /// called just after Timer().ResetTime().
    virtual void	Reset ();

    // ATTENTION!  It's guaranteed that this->Function() will be called
    // only once for each time step.  Index of the time step and current
    // time can be got from Timer().

    /// Compute output on the basis of internal parameters,
    /// stored state and external input: y=F(x,t,p)
    virtual void	Function (NaReal* x, NaReal* y);

protected:

    /// Characteristic of tank base area
    NaDynAr<NaReal>	vfK;
};


#endif /* polynomial.h */
