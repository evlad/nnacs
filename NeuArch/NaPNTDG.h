//-*-C++-*-
/* NaPNTDG.h */
/* $Id$ */
#ifndef __NaPNTDG_h
#define __NaPNTDG_h

#include <vector>
#include <math.h>

#include <NaPetri.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Applied Petri net node: train data gathering.
// Has two data inputs, one boolean input and one boolean output.

//---------------------------------------------------------------------------
class NaPNTrainDataGath : public NaPetriNode
{
public:

  // Create node
  NaPNTrainDataGath (const char* szNodeName = "trdatagath");

  // Destroy the node
  virtual ~NaPNTrainDataGath ();


  ////////////////
  // Connectors //
  ////////////////

  // Turn on trigger (main) - optional (true if not linked)
  NaPetriCnInput	turnon;

  // First dimension input
  NaPetriCnInput	in1;

  // Second dimension input
  NaPetriCnInput	in2;

  // Turn off trigger (main)
  NaPetriCnOutput	turnoff;

  // First dimension output
  NaPetriCnOutput	out1;

  // Second dimension output
  NaPetriCnOutput	out2;


  ///////////////////
  // Node specific //
  ///////////////////

  // Setup parameters:
  // n_stpardet - length for statistic parameters determination
  // n_cells    - number of cells in each dimension
  // n_sigma    - width of data evaluation range in number of sigma
  // percent    - percent of non-zero multidim cells to stop data gathering
  void	set_parameters (int n_stpardet, int n_cells,
			NaReal n_sigma, NaReal percent);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 2. Link connectors inside the node
  virtual void		relate_connectors ();

  // 5. Verification to be sure all is OK (true)
  virtual bool		verify ();

  // 6. Initialize node activity and setup starter flag if needed
  virtual void		initialize (bool& starter);

  // 8. True action of the node (if activate returned true)
  virtual void		action ();

  // 9. Finish data processing by the node (if activate returned true)
  virtual void		post_action ();

  // 10. Deallocate resources and close external data
  virtual void		release_and_close ();

protected:

  bool		bTurnedOn;	// gathering data
  int		nStDetLen;	// length for statistic parameters
				// determination
  NaReal	fMinHits;	// percent of non-zero cells coverage
  struct Parameters {
    // initial
    int		nCells;	// number of cells in the dimension to split the range
    NaReal	fNSigma;// number of sigma as a bounds of range

    // statistical parameters
    NaReal	fAvg;	// average
    NaReal	fSigma;	// sigma

    NaReal	fRange[2];// effective range

    // auxiliary
    NaReal	fSum, fSum2;

    // runtime
    std::vector<NaReal>	vSeries;

    int		cellIndex (NaReal fValue) {
      int j = (int)(nCells * (fValue - fRange[0]) / (fRange[1] - fRange[0]));
      if(j < 0)
	return 0;
      if(j >= nCells)
	return nCells - 1;
      return j;
    }
  };

  Parameters	par[2];		// parameters for each dimension
  int		*mxHits;	// matrix of hit counter cells
  int		nNonZeroHits;	// number of non-zero hit cells

};


#endif /* NaPNTDG.h */
