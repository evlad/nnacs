//-*-C++-*-
//---------------------------------------------------------------------------
#ifndef NaNNOCL2H
#define NaNNOCL2H


#include <NaPetri.h>
#include <NaPNSum.h>
#include <NaPNCmp.h>
#include <NaPNFOut.h>
#include <NaPNQIn.h>
#include <NaPNQOut.h>
#include <NaPNTran.h>
#include <NaPNStat.h>
#include <NaPNTchr.h>
#include <NaPNBu21.h>
#include <NaPNSwit.h>
#include <NaPNTrig.h>
#include <NaPNDely.h>
#include <NaPNTime.h>
#include <NaPNChPt.h>
#include <NaPNFetc.h>
#include <NaPNDerv.h>
#include <NaPNLAnd.h>
#include <NaPNCnst.h>


//---------------------------------------------------------------------------
// Class for NN optimal controller (in Wiener filter terms) learning.
// Prelearned NN identifier and traditional plant model + fixed noise are
// to be given.  Learning take place in closed loop real world system.
class NaNNOptimContrLearnClosedLoop
{
public:/* methods */

  // Create NN-C training control system
  NaNNOptimContrLearnClosedLoop (NaControllerKind ckind);

  // Destroy the object
  virtual ~NaNNOptimContrLearnClosedLoop ();

  ////////////////////
  // Network phases //
  ////////////////////

  // Link the network (tune the net before)
  virtual bool		link_net ();

  // Prepare learning session
  virtual bool		begin_session ();

  // Do one step of operating outside real-world closed loop system
  // Pass data to and from NN: y(k), u(k)->y(k+1)
  virtual NaPNEvent	net_step (const NaReal* y, NaReal* u);

  // Close learning session (update trained NN-C)
  virtual bool		end_session ();


  //////////////////
  // Overloadable //
  //////////////////

  // Check for user break
  virtual bool		user_break ();

  // Each cycle callback
  virtual void		idle_entry ();

public:/* data */

  // Main Petri network module
  NaPetriNet	net;

  // Functional Petri network nodes
  NaPNConstGen		setpnt_gen;// target setpoint generator (constant)
  NaPNFileOutput	nn_y;	// NN plant output
  NaPNFileOutput	on_y;	// real plant output
  NaPNCheckPoint	nn_u;	// NN controller output
  NaPNTransfer		nncontr;// NN controller
  NaPNTransfer		nnplant;// NN plant
  NaPNQueueOutput	plant_u;// real plant input (u)
  NaPNQueueInput	plant_y;// real plant output (y)
  NaPNTeacher		nnteacher;// NN teacher
  NaPNTeacher		errbackprop;// NN error backpropagator
  NaPNBus2i1o		bus_p;	// ((u,y),e)->NN plant former
  NaPNBus2i1o		bus_c;	// (u,e)->NN controller former
  NaPNDelay		delay_c;// (e(i),e(i-1),...e(i-n))->NNC former-delayer
  NaPNDerivative	delta_e;// (1-1/z)*e(k)
  NaPNSum		sum_on;	// plant + noise summator
  NaPNComparator	iderrcomp;// identification error computer
  NaPNStatistics	iderrstat;// identification error statistics
  NaPNComparator	cerrcomp;// control error computer
  NaPNStatistics	cerrstat;// control error statistics
  NaPNSwitcher		switch_y;// (nnp,y)->(y_nn) 
  NaPNTrigger		trig_e;	// pre-teacher e delayer
  NaPNDelay		delay_y;// y -> y(-1), y(-2), ...
  NaPNDelay		delay_u;// u -> u(-1), u(-2), ...
  NaPNFetcher		errfetch;// fetch control error 
  NaPNLogicalAND	land;	// activate just after delay units are ready

private:/* data */

  // Kind of controller
  NaControllerKind	eContrKind;

};


//---------------------------------------------------------------------------
#endif
