//-*-C++-*-
/* NaSSModel.h */
//---------------------------------------------------------------------------
#ifndef NaSSModelH
#define NaSSModelH

#include <stdarg.h>

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaConfig.h>
#include <NaDynAr.h>
#include <NaVector.h>
#include <NaMatrix.h>
#include <NaLogFil.h>


// Name of the type for config file
#define NaTYPE_StateSpaceModel	"StateSpaceModel"


//---------------------------------------------------------------------------
// Class for state-space model
class NaStateSpaceModel : public NaUnit, public NaConfigPart
{
    friend class NaPNStateSpace;

public:

    // Registrar for the NaStateSpaceModel
    static NaConfigPart*	NaRegStateSpaceModel ();

    // Make empty (A=[0], B=[0], C=[0], D=[0]) state-space model of
    // with dimension 1 for all signals.
    NaStateSpaceModel ();

    // Make an explicit copy of the state-space model
    NaStateSpaceModel (const NaStateSpaceModel& orig);

    // Destructor
    virtual ~NaStateSpaceModel ();

    // Assignment
    NaStateSpaceModel&    operator= (const NaStateSpaceModel& orig);

    // Print self
    virtual void    PrintLog () const;

    // Print self with indentation
    virtual void    PrintLog (const char* szIndent) const;

    // Is empty (check for special condition of default constructor)
    virtual bool    Empty () const;

    // Make empty
    virtual void    Clean ();


    //***********************************************************************
    // Custom part
    //***********************************************************************

    // Set zero initial state (zero vector).
    virtual void        SetZeroInitialState ();

    // Set initial state (zero vector by default).
    virtual void        SetInitialState (const NaVector& vX0);

    // Set space-state model with matrices A, B, C, D, where:
    //  { x(t+1) = A*x(t) + B*u(t)
    //  { y(t)   = C*x(t) + D*u(t)
    virtual void        SetMatrices (const NaMatrix& mA,
				     const NaMatrix& mB,
				     const NaMatrix& mC,
				     const NaMatrix& mD);

    // Shorter form to set space-state model matrices A, B, C,
    // where:
    //  { x(t+1) = A*x(t) + B*u(t)
    //  { y(t)   = C*x(t)
    virtual void        SetMatrices (const NaMatrix& mA,
				     const NaMatrix& mB,
				     const NaMatrix& mC);

    // The shortest form to set space-state model matrices A, B,
    // where:
    //  { x(t+1) = A*x(t) + B*u(t)
    //  { y(t)   = x(t)
    virtual void        SetMatrices (const NaMatrix& mA,
				     const NaMatrix& mB);

    // Get matrices.
    virtual void        GetMatrices (NaMatrix& mA,
				     NaMatrix& mB,
				     NaMatrix& mC,
				     NaMatrix& mD) const;

    // Get dimensions.
    virtual void	GetDimensions (unsigned& uInputs,
				       unsigned& uOutputs,
				       unsigned& uState) const;

    // Return dimension of state space
    virtual unsigned    StateDim () const;

    // Get internal state.
    virtual void	GetState (NaVector& vX) const;

    //***********************************************************************
    // Unit part
    //***********************************************************************

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(u,t,p)
    virtual void    Function (NaReal* u, NaReal* y);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

    // Store state-space model to given configuration file
    virtual void    Save (const char* szFileName);

    // Read state-space model from given configuration file
    virtual void    Load (const char* szFileName);


protected:/* data */

    // x[n], y[m], u[k]
    unsigned            n, m, k;    // useful dimensions
    NaMatrix            A;          // state step matrix [n,n]
    NaMatrix            B;          // control input influence [n,k]
    NaMatrix            C;          // state observation [m,n]
    NaMatrix            D;          // observation input influence [m,k]

    NaVector            x;          // current state vector
    NaVector            x0;         // initial state vector

};


//---------------------------------------------------------------------------
#endif
