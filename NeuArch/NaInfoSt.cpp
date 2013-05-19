/* NaInfoSt.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaStrOps.h"
#include "NaLogFil.h"
#include "NaExcept.h"

#include "NaInfoSt.h"



//---------------------------------------------------------------------------
// Create stand object
NaInfoStand::NaInfoStand ()
{
    // Nothing to do
}


//---------------------------------------------------------------------------
// Destroy stand object
NaInfoStand::~NaInfoStand ()
{
    int     i;
    for(i = 0; i < tga.count(); ++i){
        delete[] tga[i].name;
    }
}


//---------------------------------------------------------------------------
// Re-init stand
void
NaInfoStand::init ()
{
    int     i;
    for(i = 0; i < tga.count(); ++i){
        tga[i].fresh = false;
        tga[i].data = 0.0;
    }
}


//---------------------------------------------------------------------------

//////////////////////////
// Trace graph facility //
//////////////////////////

//---------------------------------------------------------------------------
// Add new trace graph to the stand and return its id
NaTraceId
NaInfoStand::new_trace (const char* name)
{
    NaTraceStruct   trst;

    if(NULL == name)
        throw(na_null_pointer);

    trst.trid = tga.count();
    trst.name = newstr(name);
    trst.fresh = false;
    trst.data = 0.0;        // dummy number

    return tga.addh(trst);
}


//---------------------------------------------------------------------------
// Get name of the trace graph by trace id
const char*
NaInfoStand::get_trace_name (NaTraceId idTrGr)
{
    return tga[idTrGr].name;
}


//---------------------------------------------------------------------------
// Trace id iterator
NaTraceId
NaInfoStand::start_trace ()
{
    return 0;
}


//---------------------------------------------------------------------------
bool
NaInfoStand::is_last_trace (NaTraceId idTrGr)
{
    return idTrGr >= tga.count();
}


//---------------------------------------------------------------------------
NaTraceId
NaInfoStand::next_trace (NaTraceId idTrGr)
{
    return 1 + idTrGr;
}


//---------------------------------------------------------------------------
// Check for freshness of the trace data
bool
NaInfoStand::is_fresh_trace (NaTraceId idTrGr)
{
    return tga[idTrGr].fresh;
}


//---------------------------------------------------------------------------
// Read fresh trace graph data (and turn off freshness)
NaReal
NaInfoStand::get_trace_data (NaTraceId idTrGr)
{
    if(!tga[idTrGr].fresh){
        NaPrintLog("Getting non-fresh data of trace graph '%s'.\n",
                   tga[idTrGr].name);
    }else{
        tga[idTrGr].fresh = false;
    }
    return tga[idTrGr].data;
}


//---------------------------------------------------------------------------
// Update trace graph data (make it fresh)
void
NaInfoStand::put_trace_data (NaTraceId idTrGr, NaReal fData)
{
    if(tga[idTrGr].fresh){
        NaPrintLog("Previous fresh data of trace graph '%s' were lost.\n",
                   tga[idTrGr].name);
    }else{
        tga[idTrGr].fresh = true;
    }
    tga[idTrGr].data = fData;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
