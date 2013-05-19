//-*-C++-*-
/* NaPCnInp.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPCnInpH
#define NaPCnInpH

#include <NaPetCn.h>

//---------------------------------------------------------------------------
// Petri network node input connector
// "Input" means reading data from source only when they are ready for
// reading.  The only source is accepted.

class NaPetriCnOutput;  // Adjoint connector

//---------------------------------------------------------------------------
// Petri network node connector

//---------------------------------------------------------------------------
class NaPetriCnInput : public NaPetriConnector
{
    friend class NaPetriCnOutput;

public:

    // Link to the host node
    NaPetriCnInput (NaPetriNode* pHost, const char* szCnName = NULL);

    // Destroy the connector
    virtual ~NaPetriCnInput ();

    // Return the adjoint connector
    NaPetriCnOutput*    adjoint ();


    ////////////////
    // Overridden //
    ////////////////

    // Initialize the connector on the start of network life
    virtual void        init ();

    // Return type of the connector
    virtual NaPCnKind   kind ();

    // Return adjoint data vector
    virtual NaVector&   data ();

    // Return true if waits for new data from adjoint connector
    virtual bool        is_waiting ();

    // Complete data waiting period (after activate)
    virtual void        commit_data ();

    // Link the connector with another one and return true on success
    virtual bool        link (NaPetriConnector* pLinked);

    // Describe to the log connector's state
    virtual void        describe ();

protected:/* data */

    // Data identifier
    int                 idData;

};


//---------------------------------------------------------------------------
#endif
