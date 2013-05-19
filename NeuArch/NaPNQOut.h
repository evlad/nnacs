//-*-C++-*-
/* NaPNQOut.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNQOutH
#define NaPNQOutH

#include <NaPetri.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Applied Petri net node: external data writer.
// Has the only input and no output.  Writes given queue value-by-value.

//---------------------------------------------------------------------------
class NaPNQueueOutput : public NaPetriNode
{
public:

  // Create node
  NaPNQueueOutput (const char* szNodeName = "queueout");

  // Destroy the node
  virtual ~NaPNQueueOutput ();


  ////////////////
  // Connectors //
  ////////////////

  // Mainstream input
  NaPetriCnInput	in;


  ///////////////////
  // Node specific //
  ///////////////////

  // Get dimension of output data
  virtual unsigned	get_data_dim ();

  // Set maximum number of stored items or only last one (n=0)
  virtual void		set_queue_limit (unsigned n);

  // Get data from the queue
  virtual void		get_data (NaReal* pPortion);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 6. Initialize node activity and setup starter flag if needed
  virtual void		initialize (bool& starter);

  // 7. Do one step of node activity and return true if succeeded
  virtual bool		activate ();

  // 8. True action of the node (if activate returned true)
  virtual void		action ();

protected:

  // Queue buffer
  NaVector		vQueue;

  // Queue limit length
  unsigned		nQLimit;

};


//---------------------------------------------------------------------------
#endif
