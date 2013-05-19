//-*-C++-*-
/* NaPNAct.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNActH
#define NaPNActH

#include <NaPetri.h>

//---------------------------------------------------------------------------
// Applied Petri net node: some action unit.
// Has one input which operates as a sync signal and no outputs

typedef void (*NaActionProc)(void* pUserData);

//---------------------------------------------------------------------------
class NaPNActor : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNActor (const char* szNodeName = "actor");

    ////////////////
    // Connectors //
    ////////////////

    // Input sync stream (mainstream)
    NaPetriCnInput      sync;

    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();


    ///////////////////
    // Node specific //
    ///////////////////

    // Setup some external procedure and its argument to call
    // on data ready
    virtual void        set_action (NaActionProc ap, void* ud);

    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:/* data */

    // Action procedure pointer
    NaActionProc        pActionProc;

    // User data to pass to action proc
    void                *pUserData;

};


//---------------------------------------------------------------------------
#endif
