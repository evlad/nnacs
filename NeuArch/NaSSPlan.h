//-*-C++-*-
/* NaSSPlan.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaSSPlanH
#define NaSSPlanH

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaRandom.h>
#include <NaConfig.h>
#include <NaVector.h>
#include <NaMatrix.h>


//---------------------------------------------------------------------------
// Class for discrete linear plant with state space model
class NaDStateSpaceUnit : public NaUnit,
                          public NaConfigPart
{
public:

    // Make default unit: 1x1 with gain 1 and without observation noise
    NaDStateSpaceUnit ();

    // Make an explicit copy of the unit
    NaDStateSpaceUnit (const NaDStateSpaceUnit& orig);

    // Destroy unit
    virtual ~NaDStateSpaceUnit ();

    // Assignment
    NaDStateSpaceUnit&   operator= (const NaDStateSpaceUnit& orig);

    // Setup initial or intermediate state
    virtual void    SetState (const NaVector& vX);

    // Get initial or intermediate state
    virtual void    GetState (NaVector& vX) const;

    // Assign system matrices and remove noise
    virtual void    SetModel (const NaMatrix& mA,
                              const NaMatrix& mB,
                              const NaMatrix& mC);

    // Get system matrices
    virtual void    GetModel (NaMatrix& mA,
                              NaMatrix& mB,
                              NaMatrix& mC) const;

    // Assign noise parameters: w(mean,stddev) v(mean,stddev)
    virtual void    SetDisturbNoise (const NaVector& Wmean,
                                     const NaVector& Wstddev);
    virtual void    SetObservNoise (const NaVector& Vmean,
                                    const NaVector& Vstddev);

    // Get noise parameters: w(mean,stddev) v(mean,stddev)
    virtual void    GetDisturbNoise (NaVector& Wmean,
                                     NaVector& Wstddev) const;
    virtual void    GetObservNoise (NaVector& Vmean,
                                    NaVector& Vstddev) const;

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* u, NaReal* y);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

public:/* data */

    // x(t+1) = A*x(t) + B*u(t) + w(t)
    //   y(t) = C*x(t) + v(t)  
    NaMatrix            A, B, C;    // Matrices
    NaVector            X;          // State

    // A(N,N), B(M,N), C(N,K)
    unsigned            N, M, K;

    // Random noise generators
    NaRandomSequence    **W, **V;
    unsigned            nW, nV;

};


//---------------------------------------------------------------------------
#endif
