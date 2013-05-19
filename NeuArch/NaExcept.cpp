/* NaExcept.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <stdio.h>

#include "NaExcept.h"


//---------------------------------------------------------------------------
// Return text message instead of number identifier
const char*
NaExceptionMsg (enum NaException exCode)
{
    const char  *szMsg;
    char        szBuf[100];

    switch(exCode){
    case na_null_pointer:
        szMsg = "NULL pointer emerged but must not.";
        break;
    case na_out_of_range:
        szMsg = "index is out of allowed range.";
        break;
    case na_size_mismatch:
        szMsg = "size parameters of objects in operation "
                "must correlate in some manner but they don't.";
        break;
    case na_bad_value:
        szMsg = "some variable don't have expected value.";
        break;
    case na_bad_alloc:
        szMsg = "can't allocate memory.";
        break;
    case na_cant_open_file:
        szMsg = "open file failure.";
        break;
    case na_not_implemented:
        szMsg = "the feature is not available.";
        break;
    case na_read_error:
        szMsg = "can't read something for some reason.";
        break;
    case na_write_error:
        szMsg = "can't write something for some reason.";
        break;
    case na_end_of_partition:
        szMsg = "end of partition is reached; nothing to read.";
        break;
    case na_end_of_file:
        szMsg = "end of file is reached; nothing to read.";
        break;
    case na_not_compatible:
        szMsg = "connectors or their hosts are not compatible.";
        break;
    case na_not_tunable:
        szMsg = "some object can't be changed so at this moment.";
        break;
    case na_not_found:
        szMsg = "some object is not found as expected.";
        break;
    default:
        sprintf(szBuf, "undefined #%d.", exCode);
        szMsg = szBuf;
        break;
    }

    return szMsg;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
