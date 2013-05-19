//-*-C++-*-
/* NaSSCtrl.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaSSCtrlH
#define NaSSCtrlH

#include <NaGenerl.h>
#include <NaUnit.h>
#include <NaConfig.h>
#include <NaVector.h>
#include <NaMatrix.h>


//---------------------------------------------------------------------------
// Class for general MIMO linear controller: u(t) = L*x(t)
class NaControllerUnit : public NaUnit,
                         public NaConfigPart
{
public:

    // Make default unit with gain 1
    NaControllerUnit ();

    // Make an explicit copy of the unit
    NaControllerUnit (const NaControllerUnit& orig);

    // Assignment
    NaControllerUnit&   operator= (const NaControllerUnit& orig);

    // Assign controller matrix
    virtual void    SetMatrix (const NaMatrix& mL);

    // Get controller matrix
    virtual void    GetMatrix (NaMatrix& mL) const;

    // Reset operations, that must be done before new modelling
    // will start
    virtual void    Reset ();

    // Compute control force from the input e to output u
    virtual void    Function (NaReal* e, NaReal* u);

    //***********************************************************************
    // Store and retrieve configuration data
    //***********************************************************************

    // Store configuration data in internal order to given stream
    virtual void    Save (NaDataStream& ds);

    // Retrieve configuration data in internal order from given stream
    virtual void    Load (NaDataStream& ds);

public:/* data */

    // y(t) = L*x(t)
    NaMatrix            L;

};


//---------------------------------------------------------------------------
#endif
