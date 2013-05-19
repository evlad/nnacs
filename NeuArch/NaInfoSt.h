//-*-C++-*-
/* NaInfoSt.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaInfoStH
#define NaInfoStH

//---------------------------------------------------------------------------
// Stand with different runtime information.  It's used for passing some
// data from working petri network to the world (GUI of an application, for
// example).

#include <NaGenerl.h>
#include <NaDynAr.h>


//---------------------------------------------------------------------------
// Identifier for the trace graph
typedef int     NaTraceId;

//---------------------------------------------------------------------------
// Trace graph description
struct NaTraceStruct
{
    NaTraceId   trid;       // trace id
    char        *name;      // name of the trace
    bool        fresh;      // flag of freshness of data
    NaReal      data;       // current trace data
};


//---------------------------------------------------------------------------
class NaInfoStand
{
public:

    // Create stand object
    NaInfoStand ();

    // Destroy stand object
    virtual ~NaInfoStand ();


    // Re-init stand
    void            init ();


    //////////////////////////
    // Trace graph facility //
    //////////////////////////

    // Add new trace graph to the stand and return its id
    NaTraceId       new_trace (const char* name);

    // Get name of the trace graph by trace id
    const char*     get_trace_name (NaTraceId idTrGr);

    // Trace id iterator
    NaTraceId       start_trace ();
    bool            is_last_trace (NaTraceId idTrGr);
    NaTraceId       next_trace (NaTraceId idTrGr);

    // Check for freshness of the trace data
    bool            is_fresh_trace (NaTraceId idTrGr);

    // Read fresh trace graph data (and turn off freshness)
    NaReal          get_trace_data (NaTraceId idTrGr);

    // Update trace graph data (make it fresh)
    void            put_trace_data (NaTraceId idTrGr, NaReal fData);

protected:

    // Array of trace graphs
    NaDynAr<NaTraceStruct>      tga;    // Trace Graph Array

};



//---------------------------------------------------------------------------
#endif
