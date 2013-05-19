//-*-C++-*-
/* NaPNQIn.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNQInH
#define NaPNQInH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: external data reader.
// Has no input and the only output.  Reads data to queue value-by-value.

//---------------------------------------------------------------------------
class NaPNQueueInput : public NaPetriNode
{
public:

  // Create node
  NaPNQueueInput (const char* szNodeName = "queuein");

  // Destroy the node
  virtual ~NaPNQueueInput ();


  ////////////////
  // Connectors //
  ////////////////

  // Mainstream output
  NaPetriCnOutput	out;


  ///////////////////
  // Node specific //
  ///////////////////

  // Set dimension of input data
  virtual void		set_data_dim (unsigned n = 1);

  // Put data to the queue
  virtual void		put_data (const NaReal* pPortion);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 1. Open input data (pure output nodes) and get their dimensions
  virtual void		open_input_data ();

  // 6. Initialize node activity and setup starter flag if needed
  virtual void		initialize (bool& starter);

  // 7. Do one step of node activity and return true if succeeded
  virtual bool		activate ();

  // 8. True action of the node (if activate returned true)
  virtual void		action ();

protected:

  // Queue buffer
  NaVector		vQueue;

  // Data size portion
  unsigned		nDataSize;

};


//---------------------------------------------------------------------------
#endif
