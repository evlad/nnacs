/* NaPetNet.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdarg.h>
#include <string.h>

#include "NaExcept.h"
#include "NaLogFil.h"
#include "NaStrOps.h"
#include "NaPetNet.h"
#include "NaPetCn.h"


//---------------------------------------------------------------------------
// Autoname facility counter
int     NaPetriNet::iNetNumber = 0;


#if defined(unix)
//---------------------------------------------------------------------------
// Flag of user break signal
bool		NaPetriNet::bUserBreak = false;


//---------------------------------------------------------------------------
// Signal handler
void
NaPetriNet::user_break (int signum)
{
    bUserBreak = true;

    // Repeat register the signal handler oncve again
    struct sigaction	sa, sa_old;
    sa.sa_handler = user_break;
    sa.sa_flags = SA_ONESHOT;
    sigaction(SIGINT, &sa, &sa_old);
    sigaction(SIGTERM, &sa, &sa_old);
    sigaction(SIGQUIT, &sa, &sa_old);
}
#endif /* unix */


//---------------------------------------------------------------------------
// Create empty network
NaPetriNet::NaPetriNet (const char* szNetName)
{
    // Setup default timer
    pTimer = &TheTimer;
    bTimeChart = false;
    dfTimeChart = NULL;
    pTimingNode = NULL;

    fpMap = NULL;
    fpDig = NULL;

    if(NULL == szNetName)
        szName = autoname("pnet", iNetNumber);
    else
        szName = newstr(szNetName);


    /* setup verbose from program environment */
    char	szEnvName[1024];
    sprintf(szEnvName, "%s_printout", szName);
    if(NULL == getenv(szEnvName))
      {
	bInitPrintout = false;				// initialize(???)
	bPrepPrintout = false;				// prepare(???)
	bStepPrintout = false;				// step_alive(???)
	bTermPrintout = false;				// terminate(???)
	bMapPrintout = false;				// no activation map
	bDigPrintout = true;				// no digraph output
      }
    else
      {
	char	*szEnvValue = getenv(szEnvName);
	bInitPrintout = !!strchr(szEnvValue, 'i');	// initialize(true)
	bPrepPrintout = !!strchr(szEnvValue, 'p');	// prepare(true)
	bStepPrintout = !!strchr(szEnvValue, 's');	// step_alive(true)
	bTermPrintout = !!strchr(szEnvValue, 't');	// terminate(true)
	bMapPrintout = !!strchr(szEnvValue, 'm');	// print activation map
	bDigPrintout = !!strchr(szEnvValue, 'g');	// digraph output
      }

    if(bDigPrintout) {
	char	*szDigName = new char[strlen(name()) + sizeof(".dot")];
	sprintf(szDigName, "%s.dot", name());
	fpDig = fopen(szDigName, "wt");
	if(NULL == fpDig){
	    NaPrintLog("Failed to write digraph output\n");
	}else{
	    NaPrintLog("Digraph output will be put to '%s'\n", szDigName);
	}
	delete[] szDigName;

	fprintf(fpDig, "digraph %s {\n"\
		"  graph [ rankdir = LR ] ;\n"\
		"  node [ shape = record ] ;\n\n",
		name());
    }
}


//---------------------------------------------------------------------------
// Destroy the network
NaPetriNet::~NaPetriNet ()
{
    if(NULL != fpDig)
	fclose(fpDig);
    delete dfTimeChart;
    delete[] szName;
}


//---------------------------------------------------------------------------
// Get the network name
const char*
NaPetriNet::name ()
{
    return szName;
}


//---------------------------------------------------------------------------
// Get the timer
NaTimer&
NaPetriNet::timer ()
{
    return *pTimer;
}


//---------------------------------------------------------------------------
// Query for given node: true - exist; false - doesn't
bool
NaPetriNet::ask_for_node (NaPetriNode* pNode, int *pIndex)
{
    int i;
    for(i = 0; i < pnaNet.count(); ++i){
        if(pNode == pnaNet[i]){
            if(NULL != pIndex){
                *pIndex = i;
            }
            return true;
        }
    }
    return false;
}


//---------------------------------------------------------------------------

/////////////////////
// Runtime control //
/////////////////////

//---------------------------------------------------------------------------
// Prepare the network before start and return true if it's ready and false
// if verification failed.
bool
NaPetriNet::prepare (bool bDoPrintouts)
{
    int     iNode;
    bool    bFailed;

    if(bPrepPrintout)
      bDoPrintouts = bPrepPrintout;

    if(NULL != fpDig) {
	// Digraph is created already
	fprintf(fpDig, "}\n");
	fclose(fpDig);
	fpDig = NULL;
    }

    /* Create activation map file */
    if(bMapPrintout){
      char	*szMapName = new char[strlen(name()) + sizeof(".map")];
      sprintf(szMapName, "%s.map", name());
      fpMap = fopen(szMapName, "wt");
      if(NULL == fpMap){
	NaPrintLog("Failed to write activation map\n");
      }else{
	NaPrintLog("Activation map will be put to '%s'\n", szMapName);
      }
      delete[] szMapName;
    }

    // Do preparation phases

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #0: preparation.\n", name());
    }

    iPrevIndex = -1;
    dfTimeChart = NULL;
    if(bTimeChart){
        char    *fname = new char[strlen(name()) + sizeof(".grf")];
        sprintf(fname, "%s.grf", name());

        if(bDoPrintouts){
            NaPrintLog("Time chart is on -> %s.\n", fname);
        }

        dfTimeChart = OpenOutputDataFile(fname);
        if(NULL == dfTimeChart){
            NaPrintLog("time chart file opening failed.\n");
        }else{
	    const char    *msg = "Time chart for petri network '%s'";
            char    *title = new char[strlen(msg) + strlen(name())];
            sprintf(title, msg, name());
            dfTimeChart->SetVarName(0, "*TIME*");
	    delete[] title;
        }

	delete[] fname;
    }

    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        // Disallow changes of node parameters
        pnaNet[iNode]->bTunable = false;

        // Setup 0 past calls for node activity
        pnaNet[iNode]->nCalls = 0;

        // Setup 0 past activations of node
        pnaNet[iNode]->nActivations = 0;

        // Need to initialize connectors
        int iCn;
        for(iCn = 0; iCn < pnaNet[iNode]->connectors(); ++iCn){
            if(bDoPrintouts){
                NaPrintLog("node '%s', connector '%s'\n",
                           pnaNet[iNode]->name(),
                           pnaNet[iNode]->connector(iCn)->name());
            }
            pnaNet[iNode]->connector(iCn)->init();
        }

        // Add node to time chart
        if(NULL != dfTimeChart){
            dfTimeChart->SetVarName(1 + iNode, pnaNet[iNode]->name());
        }
    }

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #1: open input data.\n", name());
    }

    // 1. Open input data (pure output nodes) and get their dimensions
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            if(bDoPrintouts){
                NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }
            pnaNet[iNode]->open_input_data();

	    /* describe only output connectors */
	    if(bDoPrintouts && pnaNet[iNode]->is_verbose()){
	      int	iCn;
	      NaPetriNode	&node = *pnaNet[iNode];

	      for(iCn = 0; iCn < node.connectors(); ++iCn){
		if(pckOutput == node.connector(iCn)->kind()){
		  NaPrintLog("  #%d ", iCn + 1);
		  node.connector(iCn)->describe();
		}
	      }
	    }
        }catch(NaException exCode){
            NaPrintLog("Open input data phase (#1): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #2: link connectors inside the node.\n",
                   name());
    }

    // 2. Link connectors inside the node
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            if(bDoPrintouts){
                NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }
            pnaNet[iNode]->relate_connectors();

	    /* describe all connectors */
	    if(bDoPrintouts && pnaNet[iNode]->is_verbose()){
	      int	iCn;
	      NaPetriNode	&node = *pnaNet[iNode];

	      for(iCn = 0; iCn < node.connectors(); ++iCn){
		NaPrintLog("  #%d ", iCn + 1);
		node.connector(iCn)->describe();
	      }
	    }
        }catch(NaException exCode){
            NaPrintLog("Link connectors phase (#2): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #3: open output data.\n", name());
    }

    // 3. Open output data (pure input nodes) and set their dimensions
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            if(bDoPrintouts){
                NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }
            pnaNet[iNode]->open_output_data();

	    /* describe only input connectors */
	    if(bDoPrintouts && pnaNet[iNode]->is_verbose()){
	      int	iCn;
	      NaPetriNode	&node = *pnaNet[iNode];

	      for(iCn = 0; iCn < node.connectors(); ++iCn){
		if(pckInput == node.connector(iCn)->kind()){
		  NaPrintLog("  #%d ", iCn + 1);
		  node.connector(iCn)->describe();
		}
	      }
	    }
        }catch(NaException exCode){
            NaPrintLog("Open output data phase (#3): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #4: allocate resources.\n", name());
    }

    // Call dim_ready() for each connector
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        // Need to setup connections
        int iCn;
        for(iCn = 0; iCn < pnaNet[iNode]->connectors(); ++iCn){
	  try{
            pnaNet[iNode]->connector(iCn)->dim_ready();
	  }catch(NaException exCode){
            NaPrintLog("Failure at %s.%s.%s.dim_ready().\n"
                       "Caused by exception: %s\n",
		       name(), pnaNet[iNode]->name(),
		       pnaNet[iNode]->connector(iCn)->name(),
		       NaExceptionMsg(exCode));
	  }
        }
    }

    // 4. Allocate resources for internal usage
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            if(bDoPrintouts){
                NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }
            pnaNet[iNode]->allocate_resources();
        }catch(NaException exCode){
            NaPrintLog("Allocate resources phase (#4): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #5: verification.\n", name());
    }

    // 5. Verification to be sure all is OK (true)
    bFailed = false;
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            if(bDoPrintouts){
                NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }
            if(!pnaNet[iNode]->verify()){
                bFailed = bFailed || true;
                NaPrintLog("Node '%s' verification failed!\n",
                           pnaNet[iNode]->name());
            }
        }catch(NaException exCode){
            bFailed = bFailed || true;
            NaPrintLog("Verification phase (#5): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }

    if(bDoPrintouts){
        NaPrintLog("Petri net '%s' map:\n", name());
        for(iNode = 0; iNode < pnaNet.count(); ++iNode){
            int         iCn;
            NaPetriNode &node = *pnaNet[iNode];

            NaPrintLog("* node '%s', connectors:\n", node.name());
            for(iCn = 0; iCn < node.connectors(); ++iCn){
                NaPrintLog("  #%d ", iCn + 1);
                node.connector(iCn)->describe();
            }
        }
    }

    if(bFailed){
        return false;
    }

    if(bDoPrintouts){
        NaPrintLog("# net '%s', phase #6: initialization.\n", name());
    }

    // 6. Initialize node activity and setup starter flag if needed
    initialize(bDoPrintouts);

#if defined(unix)
    // Prepare value of handling user break
    bUserBreak = false;

    // Register the signal handler
    struct sigaction	sa, sa_old;
    sa.sa_handler = user_break;
    sa.sa_flags = SA_ONESHOT;
    sigaction(SIGINT, &sa, &sa_old);
    sigaction(SIGTERM, &sa, &sa_old);
    sigaction(SIGQUIT, &sa, &sa_old);
#endif /* unix */

    return true;
}


//---------------------------------------------------------------------------
// Separate initialization (called inside prepare())
void
NaPetriNet::initialize (bool bDoPrintouts)
{
    int     iNode;

    if(bInitPrintout)
      bDoPrintouts = bInitPrintout;

    // 6. Initialize node activity and setup starter flag if needed
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            bool    bStarter = false;
	    bool    bVerbose = false;

	    if(NULL == pnaNet[iNode])
	      throw(na_null_pointer);

	    // Check for verbosity set in env. variable
	    if(strlen(pnaNet[iNode]->name()) > 0){
	      char	*szNodeVerbose = new char[strlen(pnaNet[iNode]->name())
						 + sizeof("_verbose")];
	      strcat(strcpy(szNodeVerbose, pnaNet[iNode]->name()), "_verbose");
	      if(NULL != getenv(szNodeVerbose))
		if(strlen(getenv(szNodeVerbose)) > 0)
		  bVerbose = true;
	      delete[] szNodeVerbose;
	    }

            if(bDoPrintouts){
                NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }

	    // Don't touch verbosity in case of direct setting from
	    // the program...
	    if(bVerbose)
	      pnaNet[iNode]->verbose(bVerbose);

	    // ...but inform an user in log file anyway
	    if(pnaNet[iNode]->is_verbose())
	      NaPrintLog("node '%s' is VERBOSE\n", pnaNet[iNode]->name());

            pnaNet[iNode]->initialize(bStarter);

            if(bStarter){
                if(bDoPrintouts){
                    NaPrintLog("node '%s' is starter.\n",
                               pnaNet[iNode]->name());
                }

                // Each starter node must commit output data at initialize
                // phase of execution due to semantics may be complicated
                // enough and be obscured from net.
                //OLD If the node is starter then it produced output data
                //OLD on the initialize phase already
                //OLD pnaNet[iNode]->commit_data(pckOutput);
            }
        }catch(NaException exCode){
            NaPrintLog("Initialization phase (#6): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }
}


//---------------------------------------------------------------------------
// Run the network for one step and return state of the network
NaPNEvent
NaPetriNet::step_alive (bool bDoPrintouts)
{
    int     iNode;
    int     nHalted = 0;
    int     iActive = 0;

    if(bStepPrintout)
      bDoPrintouts = bStepPrintout;

    if(bDoPrintouts){
        NaPrintLog("# net '%s', step phases 7, 8, 9.\n", name());
    }

    // Add point to timechart
    if(NULL != dfTimeChart){
        dfTimeChart->AppendRecord();

        // Time track
        if(iPrevIndex == timer().CurrentIndex()){
            dfTimeChart->SetValue(0.0/* old */, 0/* time */);
        }else{
            dfTimeChart->SetValue(0.8/* new */, 0/* time */);
        }
    }

    if(bDoPrintouts)
      NaPrintLog("----------------------------------------\n");

    if(NULL != fpMap){
      if(0 == ftell(fpMap)){

	/* print line */
	for(iNode = 0; iNode < pnaNet.count(); ++iNode){
	  fprintf(fpMap, "==");
	}/* iNode */
	fputc('\n', fpMap);

	/* compute max length among names */
	int	nMaxLen = 0;

	for(iNode = 0; iNode < pnaNet.count(); ++iNode){
	  if(strlen(pnaNet[iNode]->name()) > nMaxLen){
	    nMaxLen = strlen(pnaNet[iNode]->name());
	  }
	}/* iNode */

	int	iLine;

	/* print header */
	for(iLine = 0; iLine < nMaxLen; ++iLine){
	  for(iNode = 0; iNode < pnaNet.count(); ++iNode){
	    int	iNameLen = strlen(pnaNet[iNode]->name());
	    if(iNameLen + iLine >= nMaxLen){
	      fprintf(fpMap, " %c",
		      pnaNet[iNode]->name()[iLine - (nMaxLen - iNameLen)]);
	    }else{
	      fprintf(fpMap, "  ");
	    }
	  }/* iNode */
	  fputc('\n', fpMap);
	}/* iLine */

	/* print line */
	for(iNode = 0; iNode < pnaNet.count(); ++iNode){
	  fprintf(fpMap, "--");
	}/* iNode */
	fputc('\n', fpMap);

      }/* for the first time */
    }/* activation map */

    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
      bool    bActivate = false;

      try{
	NaPetriNode &node = *pnaNet[iNode];

	// 7. Do one step of node activity and return true if succeeded
	if(0 == node.connectors()) {
	    if(bDoPrintouts || node.is_verbose()){
		NaPrintLog("node '%s' does not have connectors: skip it.\n",
			   node.name());
	    }
	}
	else {
	    if(bDoPrintouts || node.is_verbose()){
		NaPrintLog("node '%s' try to activate.\n",
			   node.name());
	    }

	    bActivate = node.activate();

	    // Count calls
	    ++node.nCalls;

	    if(bDoPrintouts || node.is_verbose()){
		NaPrintLog("node '%s' is %sactivated.\n",
			   node.name(), bActivate?"" :"not ");
	    }
	}
      }catch(NaException exCode){
	NaPrintLog("Step of node activity phase (#7): node '%s' fault.\n"
		   "Caused by exception: %s\n",
		   pnaNet[iNode]->name(), NaExceptionMsg(exCode));
	bActivate = false;
      }

      /* activation map */
      if(NULL != fpMap){
	if(bActivate){
	  fprintf(fpMap, " a");
	}else{
	  fprintf(fpMap, "  ");
	}
      }/* activation map */

      if(bActivate){
	++iActive;

	try{
	  NaPetriNode &node = *pnaNet[iNode];

	  // 8. True action of the node (if activate returned true)
	  if(bDoPrintouts || node.is_verbose()){
	    NaPrintLog("node '%s' action.\n", node.name());
	  }

	  // Print contents of linked inputs
	  if(node.is_verbose()){
            int	iCn;

            NaPrintLog("node '%s', input data:\n", node.name());
            for(iCn = 0; iCn < node.connectors(); ++iCn){
	      if(node.connector(iCn)->links() > 0 &&
		 node.connector(iCn)->kind() == pckInput){
		// Only used input connectors
                NaPrintLog("  #%d '%s': ",
			   iCn + 1, node.connector(iCn)->name());
		node.connector(iCn)->data().print_contents();
	      }
            }
	  }

	  node.action();

	  // Print contents of linked outputs
	  if(node.is_verbose()){
            int	iCn;

            NaPrintLog("node '%s', output data:\n", node.name());
            for(iCn = 0; iCn < node.connectors(); ++iCn){
	      if(node.connector(iCn)->links() > 0 &&
		 node.connector(iCn)->kind() == pckOutput){
		// Only used output connectors
                NaPrintLog("  #%d '%s': ",
			   iCn + 1, node.connector(iCn)->name());
		node.connector(iCn)->data().print_contents();
	      }
            }
	  }

	  // Count activations
	  ++node.nActivations;

	  // Node is timing one - let's update timer
	  if(pnaNet[iNode] == pTimingNode)
	      timer().GoNextTime();

	}catch(NaException exCode){
	  NaPrintLog("True action phase (#8): node '%s' fault.\n"
		     "Caused by exception: %s\n",
		     pnaNet[iNode]->name(), NaExceptionMsg(exCode));
	}

	try{
	  NaPetriNode &node = *pnaNet[iNode];

	  // 9. Finish data processing by the node (if activate
	  //    returned true)
	  if(bDoPrintouts || node.is_verbose()){
	    NaPrintLog("node '%s' post action.\n",
		       node.name());
	  }
	  node.post_action();
	}catch(NaException exCode){
	  NaPrintLog("Postaction phase (#9): node '%s' fault.\n"
		     "Caused by exception: %s\n",
		     pnaNet[iNode]->name(), NaExceptionMsg(exCode));
	}

	// Check for internal halt
	if(pnaNet[iNode]->bHalt){
	  ++nHalted;
	}
      }

      try{
	if(bDoPrintouts || pnaNet[iNode]->is_verbose()){
	  pnaNet[iNode]->describe();
	}
      }catch(NaException exCode){
	// Skip...
      }

      // Add node point to time chart
      if(NULL != dfTimeChart){
	if(bActivate){
	  dfTimeChart->SetValue(-(1 + iNode) + 0.8/* active */, 1+iNode);
	}else{
	  dfTimeChart->SetValue(-(1 + iNode)/* passive */, 1+iNode);
	}
      }
    }

    /* activation map */
    if(NULL != fpMap){
      fputc('\n', fpMap);
    }

    //// Add point to timechart
    //if(NULL != dfTimeChart){
    //    dfTimeChart->AppendRecord();
    //
    //    // Time track
    //    dfTimeChart->SetValue(0.0/* old */, 0/* time */);
    //}

    if(0 == iActive){
        return pneDead;
    }else if(0 != nHalted){
        return pneHalted;
    }

#if defined(unix)
    if(bUserBreak)
      return pneTerminate;
#endif /* unix */

    return pneAlive;
}


//---------------------------------------------------------------------------
// Send gentle termination signal to the network and be sure all
// data are closed and resources are released after the call
void
NaPetriNet::terminate (bool bDoPrintouts)
{
    int     iNode;

    if(bTermPrintout)
      bDoPrintouts = bTermPrintout;

    if(bDoPrintouts){
      // Get statistics from each node
      NaPrintLog("# net '%s', obtain statistics:\n", name());
      for(iNode = 0; iNode < pnaNet.count(); ++iNode){
	NaPrintLog(" * node '%s' was called %u times and %u times "\
		   "was activated.\n",
		   pnaNet[iNode]->name(),
		   pnaNet[iNode]->nCalls,
		   pnaNet[iNode]->nActivations);
      }

      NaPrintLog("# net '%s', phase #10: deallocation and closing data.\n",
		 name());
    }

    // 10. Deallocate resources and close external data
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        try{
            if(bDoPrintouts){
		NaPrintLog("node '%s'\n", pnaNet[iNode]->name());
            }
            pnaNet[iNode]->release_and_close();
        }catch(NaException exCode){
            NaPrintLog("Deallocate resources phase (#10): node '%s' fault.\n"
                       "Caused by exception: %s\n",
                       pnaNet[iNode]->name(), NaExceptionMsg(exCode));
        }
    }

    // Call final() for each connector
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        // Need to shutdown connections
        int iCn;
        for(iCn = 0; iCn < pnaNet[iNode]->connectors(); ++iCn){
	  try{
            pnaNet[iNode]->connector(iCn)->final();
	  }catch(NaException exCode){
            NaPrintLog("Failure at %s.%s.%s.final().\n"
                       "Caused by exception: %s\n",
		       name(), pnaNet[iNode]->name(),
		       pnaNet[iNode]->connector(iCn)->name(),
		       NaExceptionMsg(exCode));
	  }
        }
    }

    // Allow changes of node parameters
    for(iNode = 0; iNode < pnaNet.count(); ++iNode){
        pnaNet[iNode]->bTunable = true;
    }

    if(NULL != dfTimeChart){
        delete dfTimeChart;
        dfTimeChart = NULL;
    }

#if defined(unix)
    // Reset the signal handler
    struct sigaction	sa, sa_old;
    sa.sa_handler = SIG_DFL;
    sigaction(SIGINT, &sa, &sa_old);
    sigaction(SIGTERM, &sa, &sa_old);
    sigaction(SIGQUIT, &sa, &sa_old);
#endif /* unix */

    /* Close activation map file */
    if(NULL != fpMap)
      {
	fclose(fpMap);
	fpMap = NULL;
      }
}


//---------------------------------------------------------------------------
// Special facility for time-chart creation
void
NaPetriNet::time_chart (bool bDoTimeChart)
{
    bTimeChart = bDoTimeChart;
}


//---------------------------------------------------------------------------

///////////////////////
// Build the network //
///////////////////////

//---------------------------------------------------------------------------
// All these methods are applied before prepare()
// and after terminate()!

//---------------------------------------------------------------------------
// Link Src->Dst connection chain
void
NaPetriNet::link (NaPetriConnector* pcSrc, NaPetriConnector* pcDst)
{
    if(NULL == pcSrc || NULL == pcDst)
        throw(na_null_pointer);

    if(NULL != fpDig) {
	fprintf(fpDig, "  %s:%s -> %s:%s ;\n",
		pcSrc->host()->name(), pcSrc->name(),
		pcDst->host()->name(), pcDst->name());
    }

    NaPrintLog("Link %s.%s & %s.%s\n",
               pcSrc->host()->name(), pcSrc->name(),
               pcDst->host()->name(), pcDst->name());

#if 1
    // Why not?
    if(pcSrc->host() == pcDst->host())
        // Can't link together two connectors of the same host node
        throw(na_not_compatible);
#endif

    add(pcSrc->host());
    add(pcDst->host());

    if(!pcSrc->link(pcDst) || !pcDst->link(pcSrc))
        // Can't link together two connectors of different type
        throw(na_not_compatible);
}


//---------------------------------------------------------------------------
// Link Src->Dst1,..DstN connection chains
void
NaPetriNet::link_1n (NaPetriConnector* pcSrc, NaPetriConnector* pcDst1, ...)
{
  NaPetriConnector	*pcDst;
  va_list		val;
  const char		*szExMsg =
    "Broken link from '%s.%s' to '%s.%s'.\n"
    "Caused by exception: %s\n";

  if(NULL == pcSrc)
    throw(na_null_pointer);

  for(pcDst = pcDst1, va_start(val, pcDst1);
      NULL != pcDst;
      pcDst = va_arg(val, NaPetriConnector*)){
    try{
      link(pcSrc, pcDst);
    }catch(NaException exCode){
      NaPrintLog(szExMsg,
		 pcSrc->host()->name(), pcSrc->name(),
		 pcDst->host()->name(), pcDst->name(),
		 NaExceptionMsg(exCode));
    }
  }

  va_end(val);
}


//---------------------------------------------------------------------------
// Link mainstream chain of nodes
void
NaPetriNet::link_nodes (NaPetriNode* pNode0, ...)
{
    NaPetriNode *pNodeCur, *pNodePrev;
    va_list     val;
    const char  *szExMsg =
        "Broken mainstream chain from '%s' to '%s' node.\n"
        "Caused by exception: %s\n";

    pNodePrev = NULL;
    pNodeCur = pNode0;

    for(va_start(val, pNode0);
        NULL != pNodeCur;
        pNodeCur = va_arg(val, NaPetriNode*)){

        // Link prev with current
        if(NULL != pNodePrev){
            try{
                if(NULL == pNodePrev->main_output_cn()){
                    NaPrintLog("Undefined mainstream output connector"
                               " for node '%s'\n", pNodePrev->name());
                }else if(NULL == pNodeCur->main_input_cn()){
                    NaPrintLog("Undefined mainstream input connector"
                               " for node '%s'\n", pNodeCur->name());
                }else{
                    link(pNodePrev->main_output_cn(),
                         pNodeCur->main_input_cn());
                }
            }catch(NaException exCode){
                NaPrintLog(szExMsg, pNodePrev->name(), pNodeCur->name(),
                           NaExceptionMsg(exCode));
            }
        }
        pNodePrev = pNodeCur;
    }

    va_end(val);
}


//---------------------------------------------------------------------------
// Add node to the network without connections
void
NaPetriNet::add (NaPetriNode* pNode)
{
    if(ask_for_node(pNode)){
        // Don't add the same node twice
        return;
    }

    if(NULL != fpDig) {
	if(pNode->connectors() > 0) {
	    fprintf(fpDig, "  %s [\n    label=\"%s|{",
		    pNode->name(), pNode->name());
	    int nInputs = 0;
	    for(int i = 0; i < pNode->connectors(); ++i) {
		NaPetriConnector *pCn = pNode->connector(i);
		if(pckInput == pCn->kind()) {
		    if(0 == nInputs)
			fprintf(fpDig, "{<%s>%s", pCn->name(), pCn->name());
		    else
			fprintf(fpDig, "|<%s>%s", pCn->name(), pCn->name());
		    ++nInputs;
		}
	    }
	    if(nInputs > 0)
		fprintf(fpDig, "}");
	    int nOutputs = 0;
	    for(int i = 0; i < pNode->connectors(); ++i) {
		NaPetriConnector *pCn = pNode->connector(i);
		if(pckOutput == pCn->kind()) {
		    if(0 == nOutputs && 0 == nInputs)
			fprintf(fpDig, "{<%s>%s", pCn->name(), pCn->name());
		    else if(0 == nOutputs)
			fprintf(fpDig, "|{<%s>%s", pCn->name(), pCn->name());
		    else
			fprintf(fpDig, "|<%s>%s", pCn->name(), pCn->name());
		    ++nOutputs;
		}
	    }
	    if(nOutputs > 0)
		fprintf(fpDig, "}");
	    fprintf(fpDig, "}\"\n  ] ;\n");
	}
    }

    NaPrintLog("Add node %s to network %s\n", pNode->name(), name());

    pnaNet.addh(pNode);

    // Setup pointer to the network
    pNode->pNet = this;

    // Perform some actions in new attendee
    pNode->attend_network();
}


//---------------------------------------------------------------------------
// Unlink Src->Dst connection chain
void
NaPetriNet::unlink (NaPetriConnector* pcSrc, NaPetriConnector* pcDst)
{
    if(NULL == pcSrc || NULL == pcDst)
        throw(na_null_pointer);

    pcSrc->unlink(pcDst);
    pcDst->unlink(pcSrc);

    NaPrintLog("Unlink %s.%s & %s.%s\n",
               pcSrc->host()->name(), pcSrc->name(),
               pcDst->host()->name(), pcDst->name());
}


//---------------------------------------------------------------------------
// Setup timer
void
NaPetriNet::set_timer (NaTimer* pTimer_)
{
    if(NULL == pTimer_)
        // Restore default timer
        pTimer = &TheTimer;
    else
        pTimer = pTimer_;
}


//---------------------------------------------------------------------------
// Setup timing node; timer steps when node is activated
void
NaPetriNet::set_timing_node (NaPetriNode* pTimingNode_)
{
  pTimingNode = pTimingNode_;

  if(NULL == pTimingNode)
    NaPrintLog("Timing node is off\n");
  else
    NaPrintLog("Timing node '%s' (%p)\n", pTimingNode->name(), pTimingNode);
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
