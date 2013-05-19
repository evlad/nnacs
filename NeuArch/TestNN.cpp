/* TestNN.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include "NaNNUnit.h"
#include "NaConfig.h"
#include "NaExcept.h"


main (int argc, char* argv[])
{
  if(2 != argc)
    {
      fprintf(stderr, "Usage: TestNN File.nn\n");
      return 1;
    }

  try{
    NaNeuralNetDescr	nndescr;
    NaNNUnit		au_nn(nndescr);

    // Configuration files
    NaConfigPart		*nn_conf_list[] = { &au_nn };
    NaConfigFile		nnfile(";NeuCon NeuralNet", 1, 1);
    nnfile.AddPartitions(NaNUMBER(nn_conf_list), nn_conf_list);
    nnfile.LoadFromFile(argv[1]);

    au_nn.descr.PrintLog();
  }
  catch(NaException& exCode){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(exCode));
  }

  return 0;
}
