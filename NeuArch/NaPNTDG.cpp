/* NaPNTDG.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <string.h>

#include "NaPNTDG.h"


//---------------------------------------------------------------------------
// Create node
NaPNTrainDataGath::NaPNTrainDataGath (const char* szNodeName)
  : NaPetriNode(szNodeName),
    ////////////////
    // Connectors //
    ////////////////
    turnon(this, "turnon"),
    in1(this, "in1"),
    in2(this, "in2"),
    turnoff(this, "turnoff"),
    out1(this, "out1"),
    out2(this, "out2")
{
  nStDetLen = 1;
  fMinHits = 0.0;

  par[0].nCells = par[1].nCells = 1;
  par[0].fNSigma = par[1].fNSigma = 1.0;
}


//---------------------------------------------------------------------------
// Destroy the node
NaPNTrainDataGath::~NaPNTrainDataGath ()
{
}

//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Setup parameters:
// n_stpardet - length for statistic parameters determination
// n_cells    - number of cells in each dimension
// n_sigma    - width of data evaluation range in number of sigma
// percent    - percent of non-zero multidim cells to stop data gathering
void
NaPNTrainDataGath::set_parameters (int n_stpardet, int n_cells,
				   NaReal n_sigma, NaReal percent)
{
  nStDetLen = (n_stpardet < 1)? 1: n_stpardet;
  fMinHits = (percent < 0)? 0.0: percent;

  par[0].nCells = par[1].nCells = (n_cells < 1)? 1: n_cells;
  par[0].fNSigma = par[1].fNSigma = (n_sigma <= 0.0)? 1.0: n_sigma;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNTrainDataGath::relate_connectors ()
{
  out1.data().new_dim(in1.data().dim());
  out2.data().new_dim(in2.data().dim());
  turnoff.data().new_dim(1);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNTrainDataGath::verify ()
{
  return (turnon.links() == 0 ||
	  turnon.links() > 0 && 1 == turnon.data().dim())
    && 1 == in1.data().dim()
    && 1 == in2.data().dim();
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNTrainDataGath::initialize (bool& starter)
{
  /* turned on at init if no links to turn on */
  bTurnedOn = (turnon.links() == 0);
  mxHits = new int[par[0].nCells * par[1].nCells];
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTrainDataGath::action ()
{
  if(bTurnedOn)
    {
      par[0].vSeries.push_back(in1.data()[0]);
      par[1].vSeries.push_back(in2.data()[0]);
      /*
      NaPrintLog("'%s' gathered data at %d\n",
		 name(), net()->timer().CurrentIndex());
      */
      // Pass data through the node
      out1.data() = in1.data();
      out2.data() = in2.data();
      turnoff.data()[0] = 0;

      out1.commit_data();
      out2.commit_data();

      if(par[0].vSeries.size() < nStDetLen)
	{
	  par[0].fSum += in1.data()[0];
	  par[0].fSum2 += in1.data()[0] * in1.data()[0];

	  par[1].fSum += in2.data()[0];
	  par[1].fSum2 += in2.data()[0] * in2.data()[0];
	}
      else if(par[0].vSeries.size() == nStDetLen)
	{
	  NaPrintLog("'%s' calculated statistics (at %d):\n",
		     name(), net()->timer().CurrentIndex());
	  for(int i = 0; i < 2; ++i)
	    {
	      par[i].fAvg = par[i].fSum / par[i].vSeries.size();
	      par[i].fSigma = sqrt(par[i].fSum2 / par[i].vSeries.size()
				   - par[i].fAvg * par[i].fAvg);
	      par[i].fRange[0] = par[i].fAvg - par[i].fNSigma * par[i].fSigma;
	      par[i].fRange[1] = par[i].fAvg + par[i].fNSigma * par[i].fSigma;
	      NaPrintLog("\taverage=%g\tsigma=%g\trange: %g .. %g\n",
			 par[i].fAvg, par[i].fSigma,
			 par[i].fRange[0], par[i].fRange[1]);
	    }

	  // make classification for previous inputs
	  for(int j = 0; j < par[0].vSeries.size(); ++j)
	    {
	      int	i1 = par[0].cellIndex(par[0].vSeries[j]);
	      int	i2 = par[1].cellIndex(par[1].vSeries[j]);
	      int	i = i1 * par[1].nCells + i2;
	      if(mxHits[i] == 0)
		++nNonZeroHits;
	      ++mxHits[i];
	    }
	  NaPrintLog("'%s' has %d non-zero cells (%4.1f%%) (at %d)\n",
		     name(), nNonZeroHits,
		     100.0 * nNonZeroHits / (par[0].nCells * par[1].nCells),
		     net()->timer().CurrentIndex());
	}
      else if(par[0].vSeries.size() > nStDetLen)
	{
	  int	i1 = par[0].cellIndex(in1.data()[0]);
	  int	i2 = par[1].cellIndex(in2.data()[0]);
	  int	i = i1 * par[1].nCells + i2;
	  if(mxHits[i] == 0)
	    ++nNonZeroHits;
	  ++mxHits[i];
	}

      if(100.0 * nNonZeroHits / (par[0].nCells * par[1].nCells) >= fMinHits)
	{
	  turnoff.data()[0] = 1;
	  bTurnedOn = false;

	  NaPrintLog("'%s' reached %d non-zero cells (%4.1f%%) -> OFF (at %d)\n",
		     name(), nNonZeroHits,
		     100.0 * nNonZeroHits / (par[0].nCells * par[1].nCells),
		     net()->timer().CurrentIndex());
	  halt();
	}
    }
  else if(turnon.links() > 0)
    if(0 != turnon.data()[0])
      {
	bTurnedOn = true;

	NaPrintLog("'%s' is ON (at %d)\n",
		   name(), net()->timer().CurrentIndex());

	for(int i = 0; i < par[0].nCells * par[1].nCells; ++i)
	  mxHits[i] = 0;
	nNonZeroHits = 0;

	par[0].fSum = par[1].fSum = 0.0;
	par[0].fSum2 = par[1].fSum2 = 0.0;

	par[0].vSeries.clear();
	par[1].vSeries.clear();
      }
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNTrainDataGath::post_action ()
{
  turnon.commit_data();
  in1.commit_data();
  in2.commit_data();
  turnoff.commit_data();
}


//---------------------------------------------------------------------------
// 10. Deallocate resources and close external data
void
NaPNTrainDataGath::release_and_close ()
{
  delete[] mxHits;
  mxHits = NULL;
}

//---------------------------------------------------------------------------
