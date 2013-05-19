//-*-C++-*-
/* NaPNChPt.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNChPtH
#define NaPNChPtH

#include <NaPetri.h>
#include <NaPNFOut.h>


//---------------------------------------------------------------------------
// Applied Petri net node: check point
// Inherits all file output features + pipe from input to output and
// switchable information stand trace.

//---------------------------------------------------------------------------
class NaPNCheckPoint : public NaPNFileOutput
{
public:

    // Create node for Petri network
    NaPNCheckPoint (const char* szNodeName = "chkpnt");


    ////////////////
    // Connectors //
    ////////////////

    // Output (mainstream)
    NaPetriCnOutput     out;


    ///////////////////
    // Node specific //
    ///////////////////

    // Switch for information stand trace of values on the node
    virtual void        set_info_trace (bool flag);


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 4. Allocate resources for internal usage
    virtual void        allocate_resources ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:

    // Switcher for information stand tracing
    bool                bInfo;

    // Information stand trace id
    NaTraceId           idTrace;

};


//---------------------------------------------------------------------------
#endif
 
