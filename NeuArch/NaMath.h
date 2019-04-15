//-*-C++-*-
/* NaMath.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaMathH
#define NaMathH

// Especially to get defined M_PI for MSVC
#define _USE_MATH_DEFINES

#include <math.h>

#include <NaGenerl.h>

//---------------------------------------------------------------------------

// PI number (if is not defined)
#ifndef M_PI
#  define M_PI	3.14159265358979323846
#endif /* M_PI */

//---------------------------------------------------------------------------

//////////////////////////////
// Random number generation //
//////////////////////////////

extern "C" {

/// Setup random generator from the system timer
PNNA_API void            reset_rand ();

/// Unified destributed random number
PNNA_API NaReal          rand_unified (NaReal fMin, NaReal fMax);

/// Gaussian normal random number distribution
PNNA_API NaReal          rand_gaussian (NaReal fMean, NaReal fStdDev);


/// Cross correlation function calculation between F1 and F2 with
/// result vector placed to B.
/// Input:
/// \arg F1[n] - first vector;
/// \arg F2[n] - second vector;
/// \arg n - number of samples in F1 and F2;
/// \arg ip - maximum shift (recommended value: 0 < ip <= n/2);
/// \arg ind1 - type of CCF: 0-simple CCF; 1-normalized CCF;
/// \arg ind2 - balancing input vectors to average zero level: 0-off, 1-on;
/// Output:
/// \arg B[2*ip+1] - resulting CCF;
/// Return:
///  0 - OK
///  1 - null pointers
///  2 - illegal dimensions
///  3 - wrong dimensions
PNNA_API int
ccf1c (NaReal F1[], NaReal F2[], int n, NaReal B[], int ip, int ind1 = 0, int ind2 = 0);

};

//---------------------------------------------------------------------------
#endif
