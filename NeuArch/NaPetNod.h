//-*-C++-*-
/* NaPetNod.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPetNodH
#define NaPetNodH

#include <NaPetCn.h>
#include <NaDynAr.h>


class NaPetriNet;

//---------------------------------------------------------------------------
// Petri net node

//---------------------------------------------------------------------------
class NaPetriNode
{
    friend class        NaPetriNet;
    friend class        NaPetriConnector;

public:

    // Create node for Petri network
    NaPetriNode (const char* szNodeName = NULL);

    // Destroy the node
    virtual ~NaPetriNode ();

    /////////////////
    // Information //
    /////////////////

    // Return name of the parent network
    NaPetriNet*         net () const;

    // Return name of the node
    const char*         name () const;

    // Return number of connectors
    int                 connectors () const;

    // Return pointer to given connector
    NaPetriConnector*   connector (int i) const;

    // Request for external change of some important node parameters
    // 'true' means the net is dead or node is not linked
    // 'false' means the net is alive and node can't be changed this time
    bool                tunable () const;

    // Check whether node is tunable and generate exception
    void                check_tunable () const;

    // Get number of past node's activations (incremented after action() call)
    unsigned            activations () const;

    // Get number of past node calls (incremented after activate() call)
    unsigned            calls () const;

    // Check whether the node is waiting for data
    virtual bool        is_waiting () const;

    // Check whether the node is verbose
    virtual bool        is_verbose () const;


    ////////////
    // Orders //
    ////////////

    // Say 'be verbose while execution' or 'be silent'
    virtual void        verbose (bool bFlag = true);

    // Say 'lets terminate the whole net'
    virtual void        halt ();

    // Say 'the data on all connectors are ready'
    virtual void        commit_data ();

    // Describe to the log node's state
    virtual void        describe ();


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();

    // Return mainstream output connector (the only output or NULL)
    virtual NaPetriConnector*   main_output_cn ();


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 0. Called once when the node becomes part of Petri network
    virtual void	attend_network ();

    // 1. Open input data (pure output nodes) and get their dimensions
    virtual void        open_input_data ();

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 3. Open output data (pure input nodes) and set their dimensions
    virtual void        open_output_data ();

    // 4. Allocate resources for internal usage
    virtual void        allocate_resources ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 6. Initialize node activity and setup starter flag if needed
    virtual void        initialize (bool& starter);

    // 7. Do one step of node activity and return true if succeeded
    virtual bool        activate ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 9. Finish data processing by the node (if activate returned true)
    virtual void        post_action ();

    // 10. Deallocate resources and close external data
    virtual void        release_and_close ();

protected:/* methods */

    // Add the connector
    void                add_cn (NaPetriConnector* pCn);

    // Remove the connector
    void                del_cn (NaPetriConnector* pCn);

    // Ask for connector's index or return -1
    bool                ask_for_cn (NaPetriConnector* pCn,
                                    int* pIndex = NULL);

protected:/* data */

    // Node's name
    char                *szName;

    // Network's pointer
    NaPetriNet          *pNet;

    // Array of node's connectors
    NaPCnPtrAr          pcaPorts;

    // Wants to terminate
    bool                bHalt;

    // Hint of tunable period
    bool                bTunable;

    // Be verbose while execution
    bool                bVerbose;

    // Counter of calls the activate()
    unsigned            nCalls;

    // Counter of true activations
    unsigned            nActivations;

    // Autoname facility counter
    static int          iNodeNumber;

};


//---------------------------------------------------------------------------
// Array of pointers to Petri nodes
typedef NaDynAr<NaPetriNode*>   NaPNodePtrAr;

//---------------------------------------------------------------------------
#endif
