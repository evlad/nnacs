//-*-C++-*-
/* NaGenerl.h */
/* $Id$ */
#ifndef __NaGeneral_h
#define __NaGeneral_h

#include <stdio.h>


/*************************************************************
 * Universal atomic type for data item representation.
 *************************************************************/
typedef double	NaReal;


/*************************************************************
 * Universal precision for floating point operations.
 *************************************************************/
#define NaPRECISION     1e-10


/*************************************************************
 * Compute number of items in explicitly defined array
 *************************************************************/
#define NaNUMBER(x)     (sizeof(x)/sizeof(x[0]))


/*************************************************************
 * Get sign of a number
 *************************************************************/
#define NaSIGN(x)       ((x) < 0? -1: 1)

/*************************************************************
 * Maximum number of hidden layers in neural network.
 *************************************************************/
#define NaMAX_HIDDEN    3


/*************************************************************
 * Common logging stream.
 *************************************************************/
extern FILE *fpNaLog;

#ifdef WIN32
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif // C++
int vsscanf(
    const char  *buffer,
    const char  *format,
    va_list     argPtr
);
#ifdef __cplusplus
};
#endif // C++
#endif // WIN32

#endif /* NaGeneral.h */
