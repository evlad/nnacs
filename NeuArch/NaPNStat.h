//-*-C++-*-
/* NaPNStat.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNStatH
#define NaPNStatH

#include <NaPetri.h>


// Condition sign
#define LESS_THAN	(-1)
#define EQUAL_TO	0
#define GREATER_THAN	1

// Statistics id
#define NaSI_ABSMEAN	0
#define NaSI_MEAN	1
#define NaSI_RMS	2
#define NaSI_STDDEV	3
#define NaSI_MAX	4
#define NaSI_MIN	5
#define NaSI_ABSMAX	6
#define NaSI_TIME	7
#define NaSI_TINDEX	8
#define NaSI_VALUE	9

#define NaSI_number	10
#define NaSI_bad_id	(-1)

#define NaSIdToMask(id)	(1<<(id))

// Statistics mask
#define NaSM_ABSMEAN	NaSIdToMask(NaSI_ABSMEAN)
#define NaSM_MEAN	NaSIdToMask(NaSI_MEAN)
#define NaSM_RMS	NaSIdToMask(NaSI_RMS)
#define NaSM_STDDEV	NaSIdToMask(NaSI_STDDEV)
#define NaSM_MAX	NaSIdToMask(NaSI_MAX)
#define NaSM_MIN	NaSIdToMask(NaSI_MIN)
#define NaSM_ABSMAX	NaSIdToMask(NaSI_ABSMAX)
#define NaSM_TIME	NaSIdToMask(NaSI_TIME)
#define NaSM_TINDEX	NaSIdToMask(NaSI_TINDEX)
#define NaSM_VALUE	NaSIdToMask(NaSI_VALUE)

#define NaSM_ALL	(NaSM_ABSMEAN | NaSM_MEAN | NaSM_RMS | NaSM_STDDEV | \
			 NaSM_MAX | NaSM_MIN | NaSM_ABSMAX | \
			 NaSM_TIME | NaSM_TINDEX | NaSM_VALUE)


// Stat identifier string<->id conversion
const char*	NaStatIdToText (int stat_id);
int		NaStatTextToId (const char* szStatText);


//---------------------------------------------------------------------------
// Applied Petri net node: compute statistics for the N-dimensional signal.
// Has the only input which can be N-dimensional.  Computes mean, standard
// deviation, root mean sum and some other informative N-dimension values.
// First items of these statistics can be accessed from the only output and
// passed so to other nodes.  The node provides simple way for making
// decision about stopping net computation (see halt_condition()).

//---------------------------------------------------------------------------
class NaPNStatistics : public NaPetriNode
{
public:

  // Create node for Petri network
  NaPNStatistics (const char* szNodeName = "statistics");


  ////////////////
  // Connectors //
  ////////////////

  // Input (mainstream)
  NaPetriCnInput	signal;

  // Output vector (see configure_output() and NaSM_* macros):
  //  [0] absolute mean value: |S(x)/n|
  //  [1] mean value: S(x)/n
  //  [2] root mean square value: S(^2)/n
  //  [3] standard deviation value: S(^2)/n - (S(x)/n)^2
  //  [4] maximum value: Max(x)
  //  [5] minimum value: Min(x)
  //  [6] absolute maximum value: Max(|Max(x)|,|Min(x)|)
  //  [7] current time of statistics
  //  [8] index of the current time of statistics
  //  [9] input value to calculate statistics
  NaPetriCnOutput	stat;


  ///////////////////
  // Node specific //
  ///////////////////

  // Configure computation rule as continuos or floating gap
  void		set_floating_gap (unsigned gap_width);

  // Configure output values by string (delimiters may vary)
  void		configure_output (const char* szStatMask,
				  const char* szDelim = " ,;");

  // Configure output values by mask
  void		configure_output (int stat_mask = NaSM_ALL);

  // Setup net stop condition:
  // sign<0 --> stop if statistics value is less than value
  // sign=0 --> stop if statistics value is equal than value
  // sign>0 --> stop if statistics value is greater than value
  void		halt_condition (int stat_id, int sign, NaReal value);

  // Print to the log statistics
  void		print_stat (const char* szTitle = NULL);


  /////////////////////////
  // Computed statistics //
  /////////////////////////

  // Mean value: M(signal)
  NaVector	Mean;

  // Root mean square: M(signal^2)
  NaVector	RMS;

  // Standard deviation value: d(signal) = sqrt(M(signal^2) - M(signal)^2)
  NaVector	StdDev;

  // Minimum value of the series
  NaVector	Min;

  // Maximum value of the series
  NaVector	Max;



  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 2. Link connectors inside the node
  virtual void	relate_connectors ();

  // 4. Allocate resources for internal usage
  virtual void	allocate_resources ();

  // 5. Verification to be sure all is OK (true)
  virtual bool	verify ();

  // 6. Initialize node activity and setup starter flag if needed
  virtual void	initialize (bool& starter);

  // 8. True action of the node (if activate returned true)
  virtual void	action ();

  // 9. Finish data processing by the node (if activate returned true)
  virtual void	post_action ();

private:

  // Check for given condition
  static bool	check_condition (NaReal stat, int sign, NaReal value);

  // Output mask
  int		mOutStat;

  // Computation gap width or 0 if floating
  unsigned	nGapWidth;

  // Counter of activations inside gap
  unsigned	nGapAct;

  /////////////////////////
  // Temporal statistics //
  /////////////////////////

  NaVector	Sum;
  NaVector	Sum2;

  /////////////////////////
  // Halt condition part //
  /////////////////////////

  int		mHalt;
  struct {
    int		sign;
    NaReal	value;
  }		HaltCond[NaSI_number];

};


//---------------------------------------------------------------------------
#endif
