/* NaPNTchr.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaPNTchr.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNTeacher::NaPNTeacher (const char* szNodeName)
  : NaPetriNode(szNodeName), nAutoUpdateFreq(0), nLastUpdate(0), auProc(NULL),
    pnn(NULL), bpe(NULL), bLearn(true), 
  ////////////////
  // Connectors //
  ////////////////
  nnout(this, "nnout"),
  desout(this, "desout"),
  errout(this, "errout"),
  errinp(this, "errinp")
{
    // Nothing to do
}


//---------------------------------------------------------------------------
// Destroy the node
NaPNTeacher::~NaPNTeacher ()
{
    delete bpe;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set learning (true by default).  May be set online.
void
NaPNTeacher::set_learning (bool bFlag)
{
  bLearn = bFlag;
  nLastUpdate = 0;
}


//---------------------------------------------------------------------------
// Set up procedure which will be executed each time the updating
// will take place
void
NaPNTeacher::set_auto_update_proc (void (*proc)(int, void*), void* data)
{
  auProc = proc;
  pData = data;
}


//---------------------------------------------------------------------------
// Set up updating frequency; 0 (no update) by default
void
NaPNTeacher::set_auto_update_freq (int nFreq)
{
  nAutoUpdateFreq = nFreq;

  if(0 != nAutoUpdateFreq && nLastUpdate >= nAutoUpdateFreq)
    update_nn();
}


//---------------------------------------------------------------------------
// Set link with petri node neural net unit to teach it (with state
// stored in FIFO)
void
NaPNTeacher::set_nn (NaPNNNUnit* pnNN, unsigned nSkipFirst)
{
    check_tunable();

    if(NULL == pnNN)
	throw(na_null_pointer);

    pnn = pnNN;
    pnn->need_nn_deck(true, nSkipFirst);

    // Create a teacher
    if(NULL != bpe)
        delete bpe;

    // Create teacher and attach NN at the same time
    bpe = new NaStdBackProp(*(pnn->get_nn_unit()));
    //qprop: bpe = new NaQuickProp(*nn);
    if(NULL == bpe)
        throw(na_bad_alloc);
}


//---------------------------------------------------------------------------
// Reset NN weight changes
void
NaPNTeacher::reset_training ()
{
    if(NULL != bpe){
        bpe->Reset();
    }
}


//---------------------------------------------------------------------------
// Update NN weights
void
NaPNTeacher::update_nn ()
{
    if(NULL == bpe)
	return;

    bpe->UpdateNN();
    nLastUpdate = 0;
    ++iUpdateCounter;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNTeacher::relate_connectors ()
{
    errinp.data().new_dim(pnn->get_nn_unit()->InputDim());
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNTeacher::verify ()
{
    if(NULL == pnn || NULL == pnn->get_nn_unit()){
        NaPrintLog("VERIFY FAILED: No NN is set!\n");
        return false;
    }else if(nnout.links() != 0 && desout.links() != 0 && errout.links() == 0){
        // Input pack #1
        return pnn->get_nn_unit()->OutputDim() == nnout.data().dim()
            && pnn->get_nn_unit()->OutputDim() == desout.data().dim()
            && pnn->get_nn_unit()->InputDim() == errinp.data().dim();
    }else if(nnout.links() == 0 && desout.links() == 0 && errout.links() != 0){
        // Input pack #2
        return pnn->get_nn_unit()->OutputDim() == errout.data().dim()
            && pnn->get_nn_unit()->InputDim() == errinp.data().dim();
    }
    NaPrintLog("VERIFY FAILED: "
               "'nnout' & 'desout' or 'errout' must be linked!\n");
    return false;
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNTeacher::initialize (bool& starter)
{
    starter = false;
    iUpdateCounter = 0;

    // Assign parameters
    *(NaStdBackPropParams*)bpe = lpar;
    //qprop: *(NaQuickPropParams*)bpe = lpar;

    if(is_verbose())
	bpe->SetDebugLevel(2);
}


//---------------------------------------------------------------------------
// 7. Do one step of node activity and return true if succeeded
bool
NaPNTeacher::activate ()
{
    bool    bInputDataReady;
    if(errout.links() == 0){
        // Input pack #1
        bInputDataReady = !desout.is_waiting() && !nnout.is_waiting();
    }else{
        // Input pack #2
        bInputDataReady = !errout.is_waiting();
    }
    return bInputDataReady && !errinp.is_waiting();
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNTeacher::action ()
{
    unsigned    iLayer;
    unsigned    iInpLayer = pnn->get_nn_unit()->InputLayer();

    // Take neural network state at the time it's feed forward calculation
    NaNNUnit	pastNN;
    pnn->pop_nn(pastNN);

    if(is_verbose()){
      int	i;
      NaPrintLog("NaPNTeacher (%p, %s[%s]):\n  NN input: ",
		 this, name(), pastNN.GetInstance());
      for(i = 0; i < pastNN.Xinp0.dim(); ++i)
	NaPrintLog(" %g", pastNN.Xinp0[i]);
      if(errout.links() == 0){
	NaPrintLog("\n  NN target: ");
	for(i = 0; i < desout.data().dim(); ++i)
	  NaPrintLog(" %g", desout.data()[i]);
      }else{
	NaPrintLog("\n  NN error: ");
	for(i = 0; i < errout.data().dim(); ++i)
	  NaPrintLog(" %g", errout.data()[i]);
      }
      NaPrintLog("\n");
    }

    if(bLearn){
      // Let's calculate delta weight from the past NN
      bpe->AttachNN(&pastNN);

      // One more activations since last update
      ++nLastUpdate;

      for(iLayer = pastNN.OutputLayer();
	  (int)iLayer >= (int)iInpLayer; --iLayer){
        if(pastNN.OutputLayer() == iLayer){
            // Output layer
            if(errout.links() == 0){
                // Input pack #1
                // Doesn't really need nnout due to it's stored inside NN
                bpe->DeltaRule(&desout.data()[0]);
            }else{
                // Input pack #2
                bpe->DeltaRule(&errout.data()[0], true);
            }
        }else{
            // Hidden layer
            bpe->DeltaRule(iLayer, iLayer + 1);
        }
      }// backward step

      // Compute error on input
      if(errinp.links() > 0){
        unsigned    iInput;
        NaVector    &einp = errinp.data();

        einp.init_zero();
        for(iInput = 0; iInput < einp.dim(); ++iInput){
	    // Or may be += ???  I didn't recognize the difference...
            einp[iInput] -= bpe->PartOfDeltaRule(iInpLayer, iInput);
        }
      }

      // Now let's forget the past NN state since changes should be
      // applied to the current neural net anyway
      bpe->DetachNN();

      // Autoupdate facility
      if(0 != nAutoUpdateFreq && nLastUpdate >= nAutoUpdateFreq){
	NaPrintLog("%s: Automatic update #%d of NN '%s' (%d sample)\n",
		   name(), iUpdateCounter,
		   bpe->nn().GetInstance()? bpe->nn().GetInstance(): "",
		   activations());
	update_nn();

	// Call procedure
	if(NULL != auProc)
	  (*auProc)(iUpdateCounter, pData);
      }// if bLearn is on
    }
}


//---------------------------------------------------------------------------
// 9. Finish data processing by the node (if activate returned true)
void
NaPNTeacher::post_action ()
{
    if(errout.links() == 0){
        // Input pack #1
        desout.commit_data();
        nnout.commit_data();
    }else{
        // Input pack #2
        errout.commit_data();
    }
    errinp.commit_data();
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
