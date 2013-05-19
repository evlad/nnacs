/* NaPetri.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPetri.h"


//---------------------------------------------------------------------------
// Applied Petri net node:
// Comment...

//---------------------------------------------------------------------------
class NaPNTemplate : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNTemplate (const char* szNodeName = "template");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      in;

    // Another input
    NaPetriCnInput      in2;

    // Output (mainstream)
    NaPetriCnOutput     out;

    // Another output
    NaPetriCnInput      out2;


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();

    // Return mainstream output connector (the only output or NULL)
    virtual NaPetriConnector*   main_output_cn ();


    ///////////////////
    // Node specific //
    ///////////////////

    // Set some parameter of the node
    virtual void        set_something ();


    ///////////////////////
    // Phases of network //
    ///////////////////////

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

};


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNTemplate::NaPNTemplate (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  in2(this, "in2"),
  out(this, "out"),
  out2(this, "out2")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set some parameter of the node
void
NaPNTemplate::set_something ()
{
    check_tunable();

    // Tune here some needed internal parameter
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNTemplate::main_input_cn ()
{
    return &in;
}


//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNTemplate::main_output_cn ()
{
    return &out;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 1. Open input data (pure output nodes) and get their dimensions
void
NaPNTemplate::open_input_data ()
{
}


//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNTemplate::relate_connectors ()
{
}


//---------------------------------------------------------------------------
// 3. Open output data (pure input nodes) and set their dimensions
void
NaPNTemplate::open_output_data ()
{
}


//---------------------------------------------------------------------------
// 4. Allocate resources for internal usage
void
NaPNTemplate::allocate_resources ()
{
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNTemplate::verify ()
{
    return true;
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNTemplate::initialize (bool& starter)
{
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNTemplate::activate ()
{
    return true;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTemplate::action ()
{
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNTemplate::post_action ()
{
}


//---------------------------------------------------------------------------
// 10. Deallocate resources and close external data
void
NaPNTemplate::release_and_close ()
{
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
