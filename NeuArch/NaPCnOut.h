//-*-C++-*-
/* NaPCnOut.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPCnOutH
#define NaPCnOutH

#include <NaPetCn.h>

//---------------------------------------------------------------------------
// Petri network node input connector
// "Output" means writing data to few destinations (>=0) and waiting for
// all of them have read the data.

class NaPetriCnInput;  // Adjoint connector

//---------------------------------------------------------------------------
// Petri network node connector

//---------------------------------------------------------------------------
class NaPetriCnOutput : public NaPetriConnector
{
    friend class NaPetriCnInput;

public:

    // Link to the host node
    NaPetriCnOutput (NaPetriNode* pHost, const char* szCnName = NULL);

    // Destroy the connector
    virtual ~NaPetriCnOutput ();

    // Return given adjoint connector {iCn=0..links()-1}
    NaPetriCnInput*     adjoint (int iCn);


    ////////////////
    //  Specific  //
    ////////////////

    // Say 'generate initial impulse in network'
    virtual void	set_starter (const NaVector& rInitial);


    ////////////////
    // Overridden //
    ////////////////

    // Initialize the connector on the start of network life
    virtual void        init ();

    // Return type of the connector
    virtual NaPCnKind   kind ();

    // Return data vector
    virtual NaVector&   data ();

    // Return true if waits for until data will be read by all adjoint
    // connectors
    virtual bool        is_waiting ();

    // Complete data waiting period (after activate)
    virtual void        commit_data ();

    // Link the connector with another one and return true on success
    virtual bool        link (NaPetriConnector* pLinked);

    // Unlink the connector from this one and return true on success
    virtual bool        unlink (NaPetriConnector* pLinked);

    // Unlink all connectors from this one
    virtual void        unlink ();

    // Describe to the log connector's state
    virtual void        describe ();
    
protected:/* data */

    // Data identifier
    int                 idData;

    // Processed connector's counter
    int                 nPrCnt;

    // Data of the connector
    NaVector            rData;

    // Starter flag and data
    bool		bStarter;
    NaVector		rData0;

};


//---------------------------------------------------------------------------
#endif
