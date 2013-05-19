//-*-C++-*-
/* NaNNLrn.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaNNLrnH
#define NaNNLrnH
//---------------------------------------------------------------------------

#include <NaStdBPE.h>
#include <NaQProp.h>


//---------------------------------------------------------------------------
// Learning algorithm
enum NaLearningAlg {

    laStdBPE = 0,   // standard BPE with momentum term
    laQuickProp,    // quick propagation
    __laNumber      // special meaning

};

// Learning parameters
struct NaLearningParams {

    NaStdBackPropParams     StdBPE;     // Parameters for BPE
    NaQuickPropParams       QProp;      // Parameters for QuickProp

};


//---------------------------------------------------------------------------
#endif

