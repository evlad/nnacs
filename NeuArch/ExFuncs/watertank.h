/* watertank.h */
#ifndef __watertank_h
#define __watertank_h

#include <NaExFunc.h>
#include <NaDataIO.h>

//---------------------------------------------------------------------------
// Class for external function which implements water tank non-linear plant.
//
//  u
//  ====>  |
//    |    |
//    |    |
//  h |~~~~|
//    |    |_
//    |______==> F
//
// Plant equation: h'=b*u/A-q*(a*sqrt(h)+F)/A
// where:
//  h' - change of water level in time
//  h  - current water level
//  A  - characteristic of tank base area
//  F  - output flow 
//  b  - input flow characteristic
//  a  - output pipe characteristic
//  q  - number of output tubes or output quota
//  u  - controllable input flow
//
// Example:
//  h=5 m
//  A=95 m^2
//  F=0.0153 m^3/s
//  a=9 m^2.5/s
//  b=2 m^3*A*s (depends of u unit of measure)
//  q=0 t<t1  (output flow is closed)
//  q=1 t>=t1 (output flow is open)

class NaWaterTankFunc : public NaExternFunc
{
public:

    /// Make empty (y=x) function
    NaWaterTankFunc ();

    /// Make function with given options and initial vector
    /// options (numbers or data files): A F b a q
    ///  -  A  - characteristic of tank base area
    ///  -  F  - output flow 
    ///  -  b  - input flow characteristic
    ///  -  a  - output pipe characteristic
    ///  -  q  - number of output tubes or output quota
    /// initial: vInit[0] - initial water level (0 by default)
    NaWaterTankFunc (char* szOptions, NaVector& vInit);

    /// Destructor
    virtual ~NaWaterTankFunc ();

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
    NaReal	A;
    NaDataFile  *Afile;

    /// Output flow 
    NaReal	F;
    NaDataFile  *Ffile;

    /// Input flow characteristic
    NaReal	b;
    NaDataFile  *bfile;

    /// Output pipe characteristic
    NaReal	a;
    NaDataFile  *afile;

    /// Output quota
    NaReal	q;
    NaDataFile  *qfile;

    /// The initial level of water in tank
    NaReal	h0;

    /// The current level of water in tank
    NaReal	h;
};


#endif /* watertank.h */
