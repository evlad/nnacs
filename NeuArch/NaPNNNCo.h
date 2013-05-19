//-*-C++-*-
/* NaPNNNCo.h */
//---------------------------------------------------------------------------
#ifndef NaPNNNCoH
#define NaPNNNCoH

#include <NaPNNNUn.h>
#include <NaPNDerv.h>
#include <NaPNDely.h>
#include <NaPNBu12.h>
#include <NaPNBu21.h>
#include <NaPNSkip.h>

//---------------------------------------------------------------------------
// Applied Petri net node: neural net controller unit with related units.
// Has one input and one output of the neural network unit.
// ATTENTION! Input dimension must be 2 and output dimension must be 1!

//---------------------------------------------------------------------------
class NaPNNNController : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNNNController (const char* szNodeName = "nncontroller");


    //////////////////
    // Inside nodes //
    //////////////////

    NaPNBus1i2o		split_er;	///< [e,r]->[e],[r]
    NaPNDelay		delay_e;	///< [e]->[e,e/z,e/z^2,...]
    NaPNDelay		delay_r;	///< [r]->[r,r/z,r/z^2,...]
    NaPNSkip		skip_e;		///< make equal delay for both r and e
    NaPNSkip		skip_r;		///< make equal delay for both r and e
    NaPNBus2i1o		merge_er;	///< [e,...],[r,...]->[e,...,r,...]
    NaPNNNUnit		nnunit;		///< u=NN(e,...,r,...)
    NaPNDerivative	delta_e;	///< (1-1/z)*e(k) - N/A

    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream) - split_er.in
    NaPetriCnInput	&x;

    // Output (mainstream) - nnunit.y
    NaPetriCnOutput	&y;

    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();

    // Return mainstream output connector (the only output or NULL)
    virtual NaPetriConnector*   main_output_cn ();


    ///////////////////
    // Node specific //
    ///////////////////

    // Assign new neural net unit
    void	set_nn_unit (NaNNUnit* pNN);

    // Get neural net unit pointer
    NaNNUnit*	get_nn_unit () {
	return nnunit.get_nn_unit();
    }

    // For compatibility reason
    void	set_transfer_func (NaNNUnit* pNN) {
	nnunit.set_transfer_func(pNN);
    }

    // Request state storage
    void	need_nn_deck (bool bDeckRequest, unsigned nSkipFirst = 0) {
	nnunit.need_nn_deck(bDeckRequest, nSkipFirst);
    }

    // Put actual state of NN to the deck (first-in first-out)
    void	push_nn () {
	nnunit.push_nn();
    }

    // Get the most ancient state of NN from the deck (first-in first-out)
    void	pop_nn (NaNNUnit& nn) {
	nnunit.pop_nn(nn);
    }

    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 0. Called once when the node becomes part of Petri network
    virtual void	attend_network ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

};


//---------------------------------------------------------------------------
#endif
