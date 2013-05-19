//-*-C++-*-
//---------------------------------------------------------------------------
#ifndef NaNNOCLH
#define NaNNOCLH


#include <NaPetri.h>
#include <NaPNSum.h>
#include <NaPNCmp.h>
#include <NaPNFIn.h>
#include <NaPNFOut.h>
#include <NaPNTran.h>
#include <NaPNRand.h>
#include <NaPNStat.h>
#include <NaPNTchr.h>
#include <NaPNBu21.h>
#include <NaPNDely.h>
#include <NaPNTime.h>
#include <NaPNChPt.h>
#include <NaPNFetc.h>
#include <NaPNDerv.h>
#include <NaPNQOut.h>
#include <NaPNNNUn.h>
#include <NaPNSkip.h>
#include <NaPNFill.h>


//---------------------------------------------------------------------------
// Class for NN optimal controller (in Wiener filter terms) learning.
// Prelearned NN identifier and traditional plant model + fixed noise are
// to be given.
class NaNNOptimContrLearn
{
public:/* methods */

  // Create NN-C training control system with stream of given length
  // in input or with data files if len=0
  NaNNOptimContrLearn (int len, NaControllerKind ckind,
		       const char* szNetName = "nncp3pn");

  // Destroy the object
  virtual ~NaNNOptimContrLearn ();

  ////////////////////
  // Network phases //
  ////////////////////

  // Link the network (tune the net before)
  virtual void		link_net ();

  // Run the network
  virtual NaPNEvent	run_net ();


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
  NaPNFileInput   setpnt_inp; // target setpoint series (file)
  NaPNRandomGen   setpnt_gen; // target setpoint generator (stream)
  NaPNFileOutput  setpnt_out; // target setpoint series output from generator
  NaPNFileInput   noise_inp;  // preset noise series (file)
  NaPNRandomGen   noise_gen;  // preset noise generator (stream)
  NaPNFileOutput  noise_out;  // preset noise series output from generator
  NaPNCheckPoint  on_y;       // plant (free) + noise (fixed) output
  NaPNFileOutput  nn_y;       // NN plant output
  NaPNCheckPoint  nn_u;       // NN controller output
  NaPNTransfer    tradcontr;  // traditional controller
  NaPNNNUnit      nncontr;    // NN controller
  NaPNNNUnit      nnplant;    // NN plant
  NaPNNNUnit      nnplant2;   // NN plant to teach
  NaPNTransfer    plant;      // plant model
  NaPNTeacher     nncteacher; // NN controller teacher
  NaPNTeacher     nnpteacher; // NN plant teacher
  NaPNTeacher     errbackprop;// NN error backpropagator
  NaPNBus2i1o     bus_p;      // ((u,y),e)->NN plant former
  NaPNBus2i1o     bus_c;      // (u,e)->NN controller former
  NaPNDelay       delay_c;    // (e(i),e(i-1),...e(i-n))->NNC former-delayer
  NaPNDerivative  delta_e;    // (1-1/z)*e(k)
  NaPNSum         sum_on;     // plant + noise summator
  NaPNComparator  iderrcomp;  // identification error computer
  NaPNStatistics  iderrstat;  // identification error statistics
  NaPNComparator  cerrcomp;   // control error computer (r_k-y_k)
  NaPNStatistics  cerrstat;   // control error statistics
  NaPNComparator  devcomp;    // deviation computation (r_k-y_{k-1})
  NaPNDelay       delay_y;    // y -> y(-1), y(-2), ...
  NaPNDelay       delay_u;    // u -> u(-1), u(-2), ...
  NaPNFetcher     errfetch;   // fetch control error 
  NaPNFileOutput  cerr_fout;  // output statistics of control error (file)
  NaPNQueueOutput cerr_qout;  // output statistics of control error (queue)
  NaPNFileOutput  iderr_fout; // output statistics of identif. error (file)
  NaPNQueueOutput iderr_qout; // output statistics of identif. error (queue)
  NaPNSkip        skip_y;     // skip some y due to u isn't available
  NaPNSkip        skip_u;     // skip some u due to y isn't available
  NaPNSkip        skip_e;     // pre-teacher e delayer
  NaPNSkip        skip_ny;    // skip some not meaning n+y values
  NaPNSkip        skip_r;     // skip first reference tick due to starter
  NaPNFill        fill_nn_y;  // fill by zeros nn_y output to fit on_y

  NaPNFileOutput  c_in;       // logging controller input
  NaPNFileOutput  p_in;       // logging plant input

private:/* methods */

  // Handle signals during neural network training
#ifndef WIN32
  static void	on_signal (int nSig, siginfo_t* pInfo, void* pData);
#endif

private:/* data */

  // Kind of controller
  NaControllerKind	eContrKind;

  // Length of series or 0 for data input
  int			nSeriesLen;

  // User break flag
  static volatile bool	bUserBreak;

};


//---------------------------------------------------------------------------
#endif
