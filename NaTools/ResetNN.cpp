
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaMath.h>
#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>
#include <NaConfig.h>
#include <NaNNUnit.h>
#include <kbdif.h>

//---------------------------------------------------------------------------
// Reset neural network and maybe put it to another file
// (C)opyright by Eliseev Vladimir
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
  int     argn = argc - 1;
  char    **args = argv + 1;
  char    *szFileNN, *szNewFileNN = NULL;

  NaOpenLogFile("ResetNN.log");

  // See DRAND_SAFE to prevent dependent random weights
  reset_rand();

  try{
    NaNeuralNetDescr    nnd;    // Default NN description 
    NaConfigFile        nnfile(";NeuCon NeuralNet", 1, 1);

    if(argc != 2 && argc != 3)
      {
	printf("Neural network initializer %s\n", nnfile.Magic());
	printf("Usage: %s File.nn [NewFile.nn]\n", argv[0]);
	return 1;
      }

    // Some default NN description
    NaNNUnit            nnu(nnd);

    NaConfigPart        *conf_list[] = { &nnu };
    nnfile.AddPartitions(NaNUMBER(conf_list), conf_list);

    // Read neural network from file
    nnfile.LoadFromFile(argv[1]);

    nnu.Initialize();

    // Store neural network to file
    if(argc == 3)
      nnfile.SaveToFile(argv[2]);
    else
      nnfile.SaveToFile(argv[1]);
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}

//---------------------------------------------------------------------------
