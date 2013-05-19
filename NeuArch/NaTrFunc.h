//-*-C++-*-
/* NaTrFunc.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaTrFuncH
#define NaTrFuncH

#include <stdarg.h>

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaConfig.h>
#include <NaDynAr.h>
#include <NaVector.h>
#include <NaLogFil.h>


//---------------------------------------------------------------------------
// Class for polynomial fraction
class NaPolyFrac : public NaUnit, public NaConfigPart
{
public:

    // Numerator, denumerator of the polynomial fraction
    NaVector    num, den;

    // Empty fraction
    NaPolyFrac ();

    // Fraction A(s)/B(s)
    NaPolyFrac (const NaVector& A, const NaVector& B);

    // Copy of fraction
    NaPolyFrac (const NaPolyFrac& pf);

    // Assignment
    NaPolyFrac&     operator= (const NaPolyFrac& orig);

    // Print self
    virtual void    PrintLog () const;

    //***********************************************************************
    // Unit part
    //***********************************************************************

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

protected:

    // For polynomial transfer computation
    NaVector        xprev, yprev;
};

// Leading declaration
class NaTransFunc;

//---------------------------------------------------------------------------
// Array of units
typedef NaDynAr<NaTransFunc*>   NaTFAr;


// Name of the type for config file
#define NaTYPE_TransFunc	"TransferFunction"


//---------------------------------------------------------------------------
// Class for transfer function
class NaTransFunc : public NaUnit, public NaConfigPart
{
public:

    // Registrar for the NaTransFunc
    static NaConfigPart*	NaRegTransFunc ();

    // Make empty (K=1) transfer function
    NaTransFunc ();

    // Make A(s)/B(s) polynomial fraction
    // NaTransFunc (int nA, NaReal A0, A1, ..., An,
    //              int mB, NaReal B0, B1, ..., Bm);
    NaTransFunc (int nA, ...);

    // Make A(s)/B(s) polynomial fraction
    NaTransFunc (const NaVector& A, const NaVector& B);

    // Make an explicit copy of the transfer function
    NaTransFunc (const NaTransFunc& orig);

    // Destructor
    virtual ~NaTransFunc ();

    // Assignment
    NaTransFunc&    operator= (const NaTransFunc& orig);

    // Print self
    virtual void    PrintLog () const;

    // Print self with indentation
    virtual void    PrintLog (const char* szIndent) const;

    // Is empty
    virtual bool    Empty () const;

    // Make empty
    virtual void    Clean ();


    //***********************************************************************
    // Unit part
    //***********************************************************************

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute output on the basis of internal parameters,
    // stored state and external input: y=F(x,t,p)
    virtual void    Function (NaReal* x, NaReal* y);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

    // Store transfer function to given configuration file
    virtual void    Save (const char* szFileName);

    // Read transfer function from given configuration file
    virtual void    Load (const char* szFileName);

    //-----------------------------------------------------------------------
    // Operators
    //-----------------------------------------------------------------------

    // Add all items of tf to this one by making new ones
    virtual NaTransFunc&    CopyItems (const NaTransFunc& tf);

    // Add all items of tf to this one by moving them from tf
    virtual NaTransFunc&    MoveItems (NaTransFunc& tf);
    
    // this = this + tf
    virtual NaTransFunc&    operator+= (const NaTransFunc& tf);

    // this = this * tf
    virtual NaTransFunc&    operator*= (const NaTransFunc& tf);

    // tf1 + tf2
    friend NaTransFunc  operator+ (const NaTransFunc& tf1,
                                   const NaTransFunc& tf2);
    // tf1 * tf2
    friend NaTransFunc  operator* (const NaTransFunc& tf1,
                                   const NaTransFunc& tf2);

public:

    // Union type
    enum UnionType {
        utSum = 0,      // Root of sum tree (items is defined)
        utProduct,      // Root of production tree (items is defined)
        utPolyFrac,     // Not a subtree root but leaf of tree
        __utNumber
    }           ut;
    NaTFAr      items;  // Subtrees if utSum, utProduct
    NaPolyFrac  pf;     // Numerator, denumerator if utLeaf

    // Input/output operations
    static const char*  UnionTypeToStrIO (UnionType eUT);
    static UnionType    StrToUnionTypeIO (const char* szUT);

};


//---------------------------------------------------------------------------
#endif
