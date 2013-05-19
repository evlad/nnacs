
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__MSDOS__) || defined(__WIN32__) || defined(WIN32)
#include <io.h>     /* for access() */
#   define access _access
#   ifndef F_OK
#       define F_OK        0
#   endif /* F_OK */
#else
#include <unistd.h>
#endif /* defined(__MSDOS__) || defined(__WIN32__) */

#include <NaMath.h>
#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>
#include <NaConfig.h>
#include <NaNNUnit.h>
#include <kbdif.h>

//---------------------------------------------------------------------------
// Create neural network file with randomized weights
// (C)opyright by Eliseev Vladimir
//---------------------------------------------------------------------------

/* #ifdef WIN32
#include <io.h>
#define access _access
#define F_OK 0
#endif
 */
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
  int     argn = argc - 1;
  char    **args = argv + 1;
  char    *fname, deffname[] = "new.nn";
  char    *nname, defnname[] = "Undefined";

  reset_rand();

  try{
    NaNeuralNetDescr    nnd;    // Default NN description 
    NaConfigFile        nnfile(";NeuCon NeuralNet", 1, 1);

    printf("Neural network maker %s\n", nnfile.Magic());
    printf("Usage: %s [File.nn [NameOfNN [InDim [InRep [OutDim\n"
	   "       [OutRep [Feedback [OutAct [HidNum [Hid0 Hid1...]]]]]\n",
	   argv[0]);

    if(argn > 0)
      {
	--argn;		/* used argument */
	fname = args[0];  ++args;
	printf("Target filename: %s\n", fname);
      }
    else
      fname = ask_user_name("Target filename", deffname);

    // Some default NN description
    NaNNUnit            nnu(nnd);

    NaConfigPart        *conf_list[] = { &nnu };
    nnfile.AddPartitions(NaNUMBER(conf_list), conf_list);

    // Check for existant file
    if(!access(fname, F_OK)){
        // File is exist
        printf("Changing NN file '%s'\n", fname);

        // Read neural network from file
        nnfile.LoadFromFile(fname);
    }else{
        // New file
        printf("Creating new NN file '%s'\n", fname);
    }

    if(argn > 0)
      {
	--argn;		/* used argument */
	nname = args[0];  ++args;
	printf("Name of NN instance: %s\n", nname);
      }
    else
      nname = ask_user_name("Name of NN instance", defnname);

    nnu.SetInstance(nname);

    // Ask for NN description
    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.nInputsNumber = atoi(args[0]);  ++args;
	printf("Input dimension: %d\n", nnd.nInputsNumber);
      }
    else
      nnd.nInputsNumber = ask_user_int("Input dimension",
				       nnd.nInputsNumber);

    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.nInputsRepeat = atoi(args[0]);  ++args;
	printf("Input repeats: %d\n", nnd.nInputsRepeat);
      }
    else
      nnd.nInputsRepeat = ask_user_int("Input repeats",
				       nnd.nInputsRepeat);

    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.nOutNeurons = atoi(args[0]);  ++args;
	printf("Output dimension: %d\n", nnd.nOutNeurons);
      }
    else
      nnd.nOutNeurons = ask_user_int("Output dimension",
				     nnd.nOutNeurons);

    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.nOutputsRepeat = atoi(args[0]);  ++args;
	printf("Output repeats: %d\n", nnd.nOutputsRepeat);
      }
    else
      nnd.nOutputsRepeat = ask_user_int("Output repeats",
					nnd.nOutputsRepeat);

    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.nFeedbackDepth = atoi(args[0]);  ++args;
	printf("Feedback depth: %d\n", nnd.nFeedbackDepth);
      }
    else
      nnd.nFeedbackDepth = ask_user_int("Feedback depth",
					nnd.nFeedbackDepth);

    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.eLastActFunc = (NaActFuncKind) atoi(args[0]);  ++args;
	printf("Output activation (0-linear; 1-sigmoid): %d\n",
	       nnd.eLastActFunc);
      }
    else
      nnd.eLastActFunc =
        (NaActFuncKind) ask_user_int("Output activation (0-linear; 1-sigmoid)",
				     nnd.eLastActFunc);

    if(argn > 0)
      {
	--argn;		/* used argument */
	nnd.nHidLayers = atoi(args[0]);  ++args;
	printf("Number of hidden layers (0-3): %d\n", nnd.nHidLayers);
      }
    else
      nnd.nHidLayers = ask_user_int("Number of hidden layers (0-3)",
				    nnd.nHidLayers);

    if(nnd.nHidLayers > NaMAX_HIDDEN){
        printf("Not more than %u layers are allowed.\n", NaMAX_HIDDEN);
        nnd.nHidLayers = NaMAX_HIDDEN;
    }

    unsigned    iLayer;
    for(iLayer = 0; iLayer < nnd.nHidLayers; ++iLayer){
        // Ask for number of neurons in given layer
	if(argn > iLayer)
	  {
	    nnd.nHidNeurons[iLayer] = atoi(args[iLayer]);
	    printf("Hidden layer #%u: %d\n", iLayer, nnd.nHidNeurons[iLayer]);
	  }
	else
	  {
	    char    prompt[100];
	    sprintf(prompt, "Hidden layer #%u", iLayer);
	    nnd.nHidNeurons[iLayer] = ask_user_int(prompt,
						   nnd.nHidNeurons[iLayer]);
	  }
    }

    nnu.AssignDescr(nnd);

    nnu.Initialize();

    // Store neural network to file
    nnfile.SaveToFile(fname);
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}

//---------------------------------------------------------------------------
