/* drand.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaMath.h>
#include <NaConfig.h>
#include <NaTrFunc.h>
#include <NaDataIO.h>


/***********************************************************************
 * Generates randomized series with normal gaussian sources of given
 * mean and std.dev.  Puts the series to output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 4 && argc != 5)
    {
      fprintf(stderr, "Error: need 3 or 4 arguments\n");
      fprintf(stderr,
	      "Usage: drand Length Mean StdDev [DiscrTrFunc]\n"\
	      "DRAND_SAFE=something to avoid the same seed in srand()\n");
      return 1;
    }

  NaOpenLogFile("drand.log");

  int	length;
  float	mean;
  float	stddev;
  char	*dtf_file = NULL;

  length = atol(argv[1]);
  mean = atof(argv[2]);
  stddev = atof(argv[3]);
  if(argc == 5)
    dtf_file = argv[4];

  try{
    int		i, j, dim;
    NaTransFunc	dtf;	/* K=1 */

    if(NULL != dtf_file)
      {
	NaConfigPart	*conf_list[] = { &dtf };
	NaConfigFile	conf_file(";NeuCon transfer", 1, 1);
	conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
	conf_file.LoadFromFile(dtf_file);

	NaPrintLog("Custom function with %d inputs, %d outputs\n",
		   dtf.InputDim(), dtf.OutputDim());
      }
    dim = dtf.OutputDim();

    if(1 != dtf.InputDim())
      {
	fprintf(stderr, "Input dimension of a function must be 1, not %d\n",
		dtf.InputDim());
	return 1;
      }

    // See DRAND_SAFE to prevent dependent random series
    reset_rand();
    dtf.Reset();

    NaReal fRand, fOut;
    for(i = 0; i < length; ++i)
      {
	for(j = 0; j < dim; ++j)
	  {
	    fRand = rand_gaussian(mean, stddev);
	    dtf.ScalarFunction(0, j)->Function(&fRand, &fOut);
	    if(j == 0)
	      printf("%10g", fOut);
	    else
	      printf(" %10g", fOut);
	  }
	putchar('\n');
      }
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
