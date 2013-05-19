//-*-C++-*-
/* NaPetNet.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPetNetH
#define NaPetNetH

#if defined(unix)
#include <signal.h>
#endif /* unix */

#include <NaTimer.h>
#include <NaDataIO.h>
#include <NaPetCn.h>
#include <NaPetNod.h>
#include <NaInfoSt.h>


//---------------------------------------------------------------------------
// Event in Petri network
typedef enum
{
    pneAlive,       // -> continue processing
    pneDead,        // -> some data source is processed completely
    pneHalted,      // -> some nodes solved to halt the network
    pneTerminate,   // -> affected by external terminate command
    pneError        // -> some fatal error occured

}   NaPNEvent;


//---------------------------------------------------------------------------
// Petri network abstraction implementation

//---------------------------------------------------------------------------
class NaPetriNet
{
public:

    // Create empty network
    NaPetriNet (const char* szNetName = NULL);

    // Destroy the network
    virtual ~NaPetriNet ();

    // Get the network name
    const char*         name ();

    // Get the timer
    NaTimer&            timer ();


    // Runtime information stand
    NaInfoStand         istand; 


    /////////////////////
    // Runtime control //
    /////////////////////

    // Do printouts if true is set as an argument

    // Special facility for time-chart creation
    virtual void        time_chart (bool bDoTimeChart = true);

    // Prepare the network before start and return true if it's ready
    // and false if verification is failed.
    virtual bool        prepare (bool bDoPrintouts = false);

    // Separate initialization (called inside prepare())
    virtual void        initialize (bool bDoPrintouts = false);

    // Run the network for one step and return state of the network
    virtual NaPNEvent   step_alive (bool bDoPrintouts = false);

    // Send gentle termination signal to the network and be sure all
    // data are closed and resources are released after the call
    virtual void        terminate (bool bDoPrintouts = false);


    ///////////////////////
    // Build the network //
    ///////////////////////

    // All these methods are applied before prepare()
    // and after terminate()!

    // Add node to the network without connections
    virtual void        add (NaPetriNode* pNode);

    // Link Src->Dst connection chain
    virtual void        link (NaPetriConnector* pcSrc,
                              NaPetriConnector* pcDst);

    // Link Src->Dst1,..DstN connection chains
    virtual void        link_1n (NaPetriConnector* pcSrc,
				 NaPetriConnector* pcDst1, ...);

    // Link mainstream chain of nodes
    virtual void        link_nodes (NaPetriNode* pNode0, ...);

    // Unlink Src->Dst connection chain
    virtual void        unlink (NaPetriConnector* pcSrc,
                                NaPetriConnector* pcDst);

    // Remove node and unlink all its connections
    //virtual void        unlink (NaPetriNode* pNode);

    // Setup timer
    virtual void        set_timer (NaTimer* pTimer_);

    // Setup timing node; timer steps when node is activated
    virtual void        set_timing_node (NaPetriNode* pTimingNode_);


protected:/* methods */

    // Query for given node: true - exist; false - doesn't
    bool                ask_for_node (NaPetriNode* pNode,
                                      int *pIndex = NULL);

protected:/* data */

    // Name of the network
    char                *szName;

    // External printout hints
    // Managed by szName_printout env. variable; maximum printout can
    // be obtained by value "ipstm"
    bool		bInitPrintout;	// initialize(true)
    bool		bPrepPrintout;	// prepare(true)
    bool		bStepPrintout;	// step_alive(true)
    bool		bTermPrintout;	// terminate(true)
    bool		bMapPrintout;	// activation map
    bool		bDigPrintout;	// digraph represented in dot language

    // In addition it's possible to make some node to be verbose() by
    // setting env. variable with name szNodeName_verbose to not an
    // empty (NULL or "") value at initialization stage

    // Array of nodes
    NaPNodePtrAr        pnaNet;

    // Timer object or default library timer
    NaTimer             *pTimer;

    // Node which activation force timer to step
    NaPetriNode		*pTimingNode;

    // Time chart facility
    bool                bTimeChart;
    NaDataFile          *dfTimeChart;
    int                 iPrevIndex;

    // Activation map
    FILE		*fpMap;

    // Digraph output (dot language for graphviz)
    FILE		*fpDig;

    // Autoname facility counter
    static int          iNetNumber;

#if defined(unix)
    // Flag of user break signal
    static bool		bUserBreak;

    // Signal handler
    static void		user_break (int signum);
#endif /* unix */

};

//---------------------------------------------------------------------------
#endif
