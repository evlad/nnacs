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
  if(argc != 3 && argc != 4 && argc != 5)
    {
      fprintf(stderr, "Error: need 2, 3 or 4 arguments\n");
      fprintf(stderr,
	      "Usage: drand Length Mean StdDev [DiscrTrFunc]\n"\
	      "    or drand InputSeries DiscrTrFunc\n"\
	      "DRAND_SAFE=something to avoid the same seed in srand()\n");
      return 1;
    }

  NaOpenLogFile("drand.log");

  int	length;
  float	mean;
  float	stddev;
  char	*dtf_file = NULL;
  NaDataFile	*ins = NULL;

  if(argc == 3)
    {
      ins = OpenInputDataFile(argv[1]);
      dtf_file = argv[2];
    }
  else
    {
      length = atol(argv[1]);
      mean = atof(argv[2]);
      stddev = atof(argv[3]);
      if(argc == 5)
	dtf_file = argv[4];
    }

  try{
    int		i;
    NaTransFunc	dtf;	/* K=1 */

    if(NULL != dtf_file)
      {
	NaConfigPart	*conf_list[] = { &dtf };
	NaConfigFile	conf_file(";NeuCon transfer", 1, 0);
	conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
	conf_file.LoadFromFile(dtf_file);
      }

    if(NULL == ins)
      {
	// See DRAND_SAFE to prevent dependent random series
	reset_rand();
      }
    else
      {
	ins->GoStartRecord();
	length = ins->CountOfRecord();
      }

    dtf.Reset();

    for(i = 0; i < length; ++i)
      {
	NaReal	fRand, fOut;

	if(NULL == ins)
	  fRand = rand_gaussian(mean, stddev);
	else
	  {
	    fRand = ins->GetValue();
	    ins->GoNextRecord();
	  }

	dtf.Function(&fRand, &fOut);

	printf("%g\n", fOut);
      }

    delete ins;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
