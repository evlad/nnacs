//-*-C++-*-
/* NaPNNNUn.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNNNUnH
#define NaPNNNUnH

#include <NaNNUnit.h>
#include <NaDynAr.h>
#include <NaPetri.h>

//---------------------------------------------------------------------------
// Applied Petri net node: neural net unit with array of states.
// Has one input and one output of the neural network unit.

//---------------------------------------------------------------------------
class NaPNNNUnit : public NaPetriNode
{
public:

  // Create node for Petri network
  NaPNNNUnit (const char* szNodeName = "nnunit");


  ////////////////
  // Connectors //
  ////////////////

  // Input (mainstream)
  NaPetriCnInput	x;

  // Output (mainstream)
  NaPetriCnOutput	y;


  ///////////////////
  // Node specific //
  ///////////////////

  // Assign new neural net unit
  void		set_nn_unit (NaNNUnit* pNN);

  // Get neural net unit pointer
  NaNNUnit*	get_nn_unit ();

  // For compatibility reason
  void		set_transfer_func (NaNNUnit* pNN);

  // Request state storage
  void		need_nn_deck (bool bDeckRequest, unsigned nSkipFirst = 0);

  // Put actual state of NN to the deck (first-in first-out)
  void		push_nn ();

  // Get the most ancient state of NN from the deck (first-in first-out)
  void		pop_nn (NaNNUnit& nnunit);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 1. Open input data and get their dimensions
  virtual void  open_input_data ();

  // 5. Verification to be sure all is OK (true)
  virtual bool	verify ();

  // 6. Initialize node activity and setup starter flag if needed
  virtual void	initialize (bool& starter);

  // 8. True action of the node (if activate returned true)
  virtual void	action ();

protected:/* data */

  // Neural net unit
  NaNNUnit		*nn;

  // 'true' to provide deck and 'false' to decline
  bool			need_deck;

  // number of nn items to skip before starting to store them
  unsigned		skip_deck, skip_rest;

  // First-in first-out structure
  NaDynAr<NaNNUnit>	deck;

};


//---------------------------------------------------------------------------
#endif
