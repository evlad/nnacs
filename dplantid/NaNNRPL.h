// -*-C++-*-
//---------------------------------------------------------------------------
#ifndef NaNNRPLH
#define NaNNRPLH

#include <NaPetri.h>
#include <NaPNCmp.h>
#include <NaPNFIn.h>
#include <NaPNFOut.h>
#include <NaPNTran.h>
#include <NaPNStat.h>
#include <NaPNTchr.h>
#include <NaPNBu21.h>
#include <NaPNDely.h>
#include <NaPNSkip.h>
#include <NaPNFetc.h>


//---------------------------------------------------------------------------
// Class for regression NN plant model learning.  Learning to predict one
// sample of plant output on the basis of few previous outputs and control
// sample.
class NaNNRegrPlantLearn
{
public:/* methods */

  // Create the object
  NaNNRegrPlantLearn (NaAlgorithmKind akind,
		      const char* szNetName = "nncp2pn");

  // Destroy the object
  virtual ~NaNNRegrPlantLearn ();

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
  NaPetriNet		net;

  // Functional Petri network nodes
  NaPNFileInput		in_y;		// target plant output
  NaPNFileInput		in_u;		// preset control force
  NaPNFetcher		fetch_y;	// fetch y(k+1) used for training
  NaPNFileOutput	tr_y;		// training input at y(k+1)
  NaPNFileOutput	nn_y;		// NN plant output at y(k+1)
  NaPNNNUnit		nnplant;	// NN plant
  NaPNTeacher		nnteacher;	// NN teacher
  NaPNBus2i1o		bus;		// ((x,y),e)->NN former
  NaPNComparator	errcomp;	// error computer
  NaPNStatistics	statan;		// error estimator
  NaPNStatistics	statan_y;	// target plant output analyzer
  NaPNDelay		delay_y;	// y -> y(-1), y(-2), ...
  NaPNDelay		delay_u;	// u -> u(-1), u(-2), ...
  NaPNDelay		delay_yt;	// delay for target y value
  NaPNSkip		skip_y;		// skip some y due to u isn't available
  NaPNSkip		skip_u;		// skip some u due to y isn't available
  NaPNSkip		skip_yt;	// skip started target y values

private:/* methods */

  // Handle signals during neural network training
#ifndef WIN32
  static void	on_signal (int nSig, siginfo_t* pInfo, void* pData);
#endif

private:/* data */

    // Kind of an algorithm
  NaAlgorithmKind	eAlgoKind;

  // User break flag
  static volatile bool	bUserBreak;

};


//---------------------------------------------------------------------------
#endif
