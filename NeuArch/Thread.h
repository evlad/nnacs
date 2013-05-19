//-*-C++-*-
/* Thread.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef ThreadH
#define ThreadH

#ifdef __BORLANDC__
// Borland C++ threads
#include <Classes.hpp>
#include <syncobjs.hpp>
#endif // __BORLANDC__


#include <NaPetNet.h>

//---------------------------------------------------------------------------
// Applied Petri net node: data file writer.
// Has no input and the only output.  Reads given file value-by-value.

//---------------------------------------------------------------------------
class NaAsyncPetriNet
#ifdef __BORLANDC__
// Borland C++ threads
: public TThread
#endif // __BORLANDC__
{
public:

    // Create thread object for the Petri network (and run it in run_net())
    NaAsyncPetriNet (NaPetriNet* pPNet);

    // Run the network (never return)
    virtual void    run_net ();

    // Check for external need to terminate running net
    virtual bool    check_termination ();

    ////////////////////////
    // Orders for the net //
    ////////////////////////

private:
protected:
    // Main thread method
    void __fastcall Execute();

    // Thread-safe method to update GUI
    void __fastcall UpdateGUI();
public:

    __fastcall TModelProcess(bool CreateSuspended);

    virtual void        start ();
};



//---------------------------------------------------------------------------
#endif
