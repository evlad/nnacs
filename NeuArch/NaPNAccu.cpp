/* NaPNAccu.cpp */
//---------------------------------------------------------------------------

#include "NaPNAccu.h"

//#define Accum_DEBUG

//---------------------------------------------------------------------------
// Create node for Petri network
NaPNAccumulator::NaPNAccumulator (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  sum(this, "sum"),
  // Data
  iNext(0), fSum(0.0)
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign depth of accumulation of input values; 0 means infinity
void
NaPNAccumulator::set_accum_depth (unsigned nDepth)
{
    check_tunable();

    vBuffer.new_dim(nDepth);
}


//---------------------------------------------------------------------------
// Get depth of accumulation of input values; 0 means infinity
unsigned
NaPNAccumulator::get_accum_depth () const
{
    return vBuffer.dim();
}


//---------------------------------------------------------------------------
// Reset accumulated sum (make it zero).
void
NaPNAccumulator::reset_accum ()
{
    vBuffer.init_zero();
    iNext = 0;
    fSum = 0.0;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNAccumulator::relate_connectors ()
{
    sum.data().new_dim(in.data().dim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNAccumulator::verify ()
{
    return true;
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNAccumulator::initialize (bool& starter)
{
    reset_accum();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNAccumulator::action ()
{
    if(get_accum_depth() > 0) {
#ifdef Accum_DEBUG
	NaPrintLog("------------------------------------------------\n"\
		   "begin: iNext=%u  fSum=%g  vBuffer:\n", iNext, fSum);
	vBuffer.print_contents();

	NaPrintLog("depth=%u input=%g\n", get_accum_depth(), in.data()[0]);
#endif // Accum_DEBUG

	fSum -= vBuffer[iNext];

	vBuffer[iNext] = in.data()[0];
#ifdef Accum_DEBUG
	NaPrintLog("intermeadiate: fSum=%g -> %g\n",
		   fSum, fSum + vBuffer[iNext]);
#endif // Accum_DEBUG

	fSum += vBuffer[iNext];
	if(iNext >= vBuffer.dim() - 1)
	    iNext = 0;
	else
	    ++iNext;

#ifdef Accum_DEBUG
	NaPrintLog("end:   iNext=%u  fSum=%g  vBuffer:\n", iNext, fSum);
	vBuffer.print_contents();
#endif // Accum_DEBUG
    } else {
	fSum += in.data()[0];
    }

    sum.data()[0] = fSum;
}
