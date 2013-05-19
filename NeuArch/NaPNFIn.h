//-*-C++-*-
/* NaPNFIn.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNFInH
#define NaPNFInH

#include <NaPetri.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Applied Petri net node: data file reader.
// Has no input and the only output.  Reads given file value-by-value.

//---------------------------------------------------------------------------
class NaPNFileInput : public NaPetriNode
{
public:

    // Create node
    NaPNFileInput (const char* szNodeName = "filein");

    // Destroy the node
    virtual ~NaPNFileInput ();


    ////////////////
    // Connectors //
    ////////////////

    // Mainstream output
    NaPetriCnOutput     out;


    ///////////////////
    // Node specific //
    ///////////////////

    // Assign new filename as a data source (before open_input_data() and
    // after release_and_close())
    virtual void        set_input_filename (const char* szFileName);


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 1. Open input data (pure output nodes) and get their dimensions
    virtual void        open_input_data ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 7. Do one step of node activity and return true if succeeded
    virtual bool        activate ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 10. Deallocate resources and close external data
    virtual void        release_and_close ();

protected:

    // Data source filename
    char                *szDataFName;

    // Data source file
    NaDataFile          *pDataF;

};


//---------------------------------------------------------------------------
#endif
 
