//-*-C++-*-
/* NaPNFOut.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNFOutH
#define NaPNFOutH

#include <NaPetri.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Applied Petri net node: data file writer.
// Has the only input and no output.  Writes given file value-by-value.

//---------------------------------------------------------------------------
class NaPNFileOutput : public NaPetriNode
{
public:

    // Create node
    NaPNFileOutput (const char* szNodeName = "fileout");

    // Destroy the node
    virtual ~NaPNFileOutput ();


    ////////////////
    // Connectors //
    ////////////////

    // Mainstream output
    NaPetriCnInput      in;


    ///////////////////
    // Node specific //
    ///////////////////

    // Assign new filename as a data destination (before open_input_data()
    // and after release_and_close())
    virtual void        set_output_filename (const char* szFileName);


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 3. Open output data (pure input nodes) and set their dimensions
    virtual void        open_output_data ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 7. Do one step of node activity and return true if succeeded
    virtual bool        activate ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 10. Deallocate resources and close external data
    virtual void        release_and_close ();

protected:

    // Data destination filename
    char                *szDataFName;

    // Data destination file
    NaDataFile          *pDataF;

};


//---------------------------------------------------------------------------
#endif
