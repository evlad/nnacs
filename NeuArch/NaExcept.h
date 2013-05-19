//-*-C++-*-
/* NaExcept.h */
/* $Id$ */
#ifndef __NaExcept_h
#define __NaExcept_h

#include <stdlib.h>

//#ifndef unix

//#define try		if(1)
//#define catch(x)	else
//#define throw(x)	exit(x)

//#endif /* unix */

/* Error codes in natools modules. */
enum NaException
{
  na_null_pointer = 0,	/* NULL pointer emerged but must not */
  na_out_of_range,	/* index is out of allowed range */
  na_size_mismatch,	/* size parameters of objects in
			 * operation must correlate in some
                         * manner but they don't */
  na_bad_value,		/* some variable don't have expected value */
  na_bad_alloc,		/* can't allocate memory */
  na_cant_open_file,	/* open file failure */
  na_not_implemented,	/* the feature is not available */
  na_read_error,	/* can't read something for some reason */
  na_write_error,	/* can't write something for some reason */
  na_end_of_partition,	/* end of partition is reached; nothing to read */
  na_end_of_file,	/* end of file is reached; nothing to read */
  na_not_compatible,	/* connectors or their hosts are not compatible */
  na_not_tunable,	/* some object can't be changed so at this moment */
  na_not_found		/* some object is not found as expected */
};


#ifdef __cplusplus
extern "C" {
#endif /* C++ */

/* Return text message instead of number identifier */
const char* NaExceptionMsg (enum NaException exCode);

#ifdef __cplusplus
}
#endif /* C++ */

#endif /* NaExcept.h */

