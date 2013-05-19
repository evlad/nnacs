//-*-C++-*-
/* NaPNFill.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNFillH
#define NaPNFillH

#include <NaPetri.h>

//---------------------------------------------------------------------------
// Applied Petri net node: fill some samples in input stream.
// Has one input and one output of the same dimension.  Simply fills n first
// samples by given value just before network operation.

//---------------------------------------------------------------------------
class NaPNFill : public NaPetriNode
{
public:

  // Create node for Petri network
  NaPNFill (const char* szNodeName = "fill");


  ////////////////
  // Connectors //
  ////////////////

  // Input (mainstream)
  NaPetriCnInput	in;

  // Output (mainstream)
  NaPetriCnOutput	out;

  // Synchronize other nodes (-1 while filling and +1 another time)
  NaPetriCnOutput	sync;


  ///////////////////
  // Quick linkage //
  ///////////////////

  // Return mainstream output connector (the only output or NULL)
  virtual NaPetriConnector*	main_output_cn ();


  ///////////////////
  // Node specific //
  ///////////////////

  // Set number of data portions to fill (zeros by default)
  void		set_fill_number (unsigned n);
  void		set_fill_number (unsigned n, NaVector& vect);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 2. Link connectors inside the node
  virtual void	relate_connectors ();

  // 5. Verification to be sure all is OK (true)
  virtual bool	verify ();

  // 7. Do one step of node activity and return true if succeeded
  virtual bool	activate ();

  // 8. True action of the node (if activate returned true)
  virtual void	action ();

  // 9. Finish data processing by the node (if activate returned true)
  virtual void	post_action ();

private:

  // Number of samples to skip
  unsigned	nFill;

  // Value to fill output with
  NaVector	vFill;

};


//---------------------------------------------------------------------------
#endif
