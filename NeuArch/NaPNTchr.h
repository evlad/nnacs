//-*-C++-*-
/* NaPNTchr.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNTchrH
#define NaPNTchrH

#include <NaStdBPE.h>
//qprop: #include <NaQProp.h>
#include <NaPNNNUn.h>
#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: neural network teacher.
// Works in close collaboration with neural network to be learned.

//---------------------------------------------------------------------------
class NaPNTeacher : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNTeacher (const char* szNodeName = "teacher");

    // Destroy the node
    virtual ~NaPNTeacher ();
    

    ////////////////
    // Connectors //
    ////////////////

    // ATTENTION! No mainstream input!
    // Inputs must be linked by pack #1 or pack #2

    // Pack #1 of inputs: NN output
    NaPetriCnInput      nnout;

    // Pack #1 of inputs: desired output
    NaPetriCnInput      desout;

    // Pack #2 of inputs: computed error on output
    NaPetriCnInput      errout;

    // Output (mainstream): syntesized error on input
    NaPetriCnOutput     errinp;


    ///////////////////
    // Node specific //
    ///////////////////

    // Set learning (true by default).  May be set online.
    virtual void	set_learning (bool bFlag);

    // Set up updating frequency; 0 (no update) by default
    virtual void	set_auto_update_freq (int nFreq);

    // Set up procedure which will be executed each time the updating
    // will take place
    virtual void	set_auto_update_proc (void (*proc)(int, void*),
					      void* data);

    // Set link with Petri node neural net unit to teach it (with
    // state stored in FIFO, skipping few first states)
    virtual void        set_nn (NaPNNNUnit* pnNN, unsigned nSkipFirst = 0);

    // Reset NN weight changes
    virtual void        reset_training ();

    // Update NN weights
    virtual void        update_nn ();

    // Learning parameters
    NaStdBackPropParams lpar;
    //NaQuickPropParams   lpar;


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 6. Initialize node activity and setup starter flag if needed
    virtual void        initialize (bool& starter);

    // 7. Do one step of node activity and return true if succeeded
    virtual bool        activate ();

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 9. Finish data processing by the node (if activate returned true)
    virtual void        post_action ();

protected:/* data */

    // Neural net petri node to teach
    NaPNNNUnit          *pnn;

    // Autoupdate frequency (in activations); 0 means no autoupdate
    int			nAutoUpdateFreq;

    // Auto update calling procedure
    void		(*auProc)(int iIter, void* pData);
    void		*pData;

    // Number of activations since last update
    int			nLastUpdate;

    // Number of updates
    int			iUpdateCounter;

    // Method of teaching
    NaStdBackProp       *bpe;
    //qprop: NaQuickProp         *bpe;

    // Flag of active delta-rule calculation and application
    bool		bLearn;

};


//---------------------------------------------------------------------------
#endif
 
