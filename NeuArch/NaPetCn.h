//-*-C++-*-
/* NaPetCn.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPetCnH
#define NaPetCnH

#include <NaDynAr.h>
#include <NaVector.h>
#include <NaDataIO.h>

//---------------------------------------------------------------------------
// Forward declaration
class NaPetriNode;
class NaPetriConnector;


//---------------------------------------------------------------------------
// Array of pointers to Petri node connectors
typedef NaDynAr<NaPetriConnector*>  NaPCnPtrAr;


//---------------------------------------------------------------------------
// Petri network node connector's type
typedef enum
{
    pckInput,       // synchronous input stream
    pckOutput       // synchronous output stream

}   NaPCnKind;


//---------------------------------------------------------------------------
// Petri network node connector

//---------------------------------------------------------------------------
class NaPetriConnector // : public NaVector
{
public:

    // Link to the host node
    NaPetriConnector (NaPetriNode* pHost, const char* szCnName = NULL);

    // Destroy the connector
    virtual ~NaPetriConnector ();

    // Return pointer to host node
    NaPetriNode*        host () const;

    // Return name of the connector
    const char*         name () const;

    // Return number of links of this connector
    virtual int         links () const;


    ///////////////////////
    // Need to implement //
    ///////////////////////

    // Describe to the log connector's state
    virtual void        describe () = 0;

    // Initialize the connector on the start of network life
    virtual void        init () = 0;

    // Called when dimension is already defined
    virtual void        dim_ready ();

    // Called at network termination
    virtual void        final ();

    // Return type of the connector
    virtual NaPCnKind   kind () = 0;

    // Return data vector
    virtual NaVector&   data () = 0;

    // Return sign of waiting for new data
    virtual bool        is_waiting () = 0;

    // Complete data waiting period (after activate)
    virtual void        commit_data ();

    // Link the connector with another one and return true on success
    virtual bool        link (NaPetriConnector* pLinked) = 0;

    // Unlink the connector from this one and return true on success
    virtual bool        unlink (NaPetriConnector* pLinked);

    // Unlink all connectors from this one
    virtual void        unlink ();

protected:/* data */

    // Host node
    NaPetriNode         *pNode;

    // Connector's name
    char                *szName;

    // Linked connectors
    NaPCnPtrAr          pcaLinked;

    // File for logging all data, passed through the connector
    NaDataFile          *dfTrace;

    // Autoname facility counter
    static int          iCnNumber;

};


#endif
