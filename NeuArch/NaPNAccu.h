//-*-C++-*-
/* NaPNAccu.h */
//---------------------------------------------------------------------------
#ifndef NaPNAccuH
#define NaPNAccuH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: accumator for an input
// Has input and output for the same dimension.
// Accumation may be infinite or of limited depth.

//---------------------------------------------------------------------------
class NaPNAccumulator : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNAccumulator (const char* szNodeName = "accumulator");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      main;

    // Output (mainstream)
    NaPetriCnOutput     sum;


    ///////////////////
    // Node specific //
    ///////////////////

    // Assign depth of accumulation of input values; 0 means infinity
    virtual void	set_accum_depth (unsigned nDepth);

    // Get depth of accumulation of input values; 0 means infinity
    virtual unsigned	get_accum_depth () const;


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 6. Initialize node activity and setup starter flag if needed
    virtual void        initialize (bool& starter);

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:/* data */

    // Cyclic buffer (depth size) for storing input values to have sum
    // of them.
    NaVector		vBuffer;

    // Index of next value to store input, so the previous one should
    // be subtracted from sum.
    unsigned		iNext;

    // Current value of accumulator
    NaReal		fSum;
};


//---------------------------------------------------------------------------
#endif
