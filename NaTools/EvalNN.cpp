/* EvalNN.cpp */
static char rcsid[] = "$Id";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaConfig.h>
#include <NaNNUnit.h>
#include <NaDataIO.h>
#include <kbdif.h>


/***********************************************************************
 * Read input data vector.  Return 'true' on success and 'false' on EOF.
 ***********************************************************************/
bool
read_input (NaDataFile* df, unsigned vec_len, NaReal* vec)
{
  static bool	bFirst = true;
  unsigned	vec_i;

  if(NULL == df)
    {
      /* read stdin */
      for(vec_i = 0; vec_i < vec_len; ++vec_i)
	if(1 != scanf("%lg", vec + vec_i))
	  return false;
    }
  else
    {
      /* read data file */
      if(bFirst)
	{
	  bFirst = false;
	  if(!df->GoStartRecord())
	    return false;
	}
      else if(!df->GoNextRecord())
	return false;

      for(vec_i = 0; vec_i < vec_len; ++vec_i)
	vec[vec_i] = df->GetValue(vec_i);
    }

  return true;
}


/***********************************************************************
 * Write output data vector.
 ***********************************************************************/
void
write_output (NaDataFile* df, unsigned vec_len, const NaReal* vec)
{
  unsigned	vec_i;

  if(NULL == df)
    {
      /* put to stdout */
      for(vec_i = 0; vec_i < vec_len; ++vec_i)
	if(vec_i == vec_len - 1)
	  printf("%g\n", vec[vec_i]);
	else
	  printf("%g ", vec[vec_i]);
    }
  else
    {
      /* put to data file */
      df->AppendRecord();

      for(vec_i = 0; vec_i < vec_len; ++vec_i)
	df->SetValue(vec[vec_i], vec_i);
    }
}


/***********************************************************************
 * Test neural network on array of preset data.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc < 2 || argc > 4)
    {
      fprintf(stderr, "Usage: EvalNN File.nn [ InputData [ OutputData ]]\n");
      return 1;
    }

  NaOpenLogFile("EvalNN.log");

  try{
    NaNeuralNetDescr    nnd;    // Default NN description 
    NaConfigFile        nnfile(";NeuCon NeuralNet", 1, 1);

    // Some default NN description
    NaNNUnit            nnu(nnd);

    NaConfigPart        *conf_list[] = { &nnu };
    nnfile.AddPartitions(NaNUMBER(conf_list), conf_list);

    // Read neural network from file
    nnfile.LoadFromFile(argv[1]);

    unsigned	nInpDim = nnu.InputDim();
    unsigned	nOutDim = nnu.OutputDim();

    NaReal	*pInpData = new NaReal[nInpDim];
    NaReal	*pOutData = new NaReal[nOutDim];

    // Open input data: stdin or given file
    bool	bNotEmpty;
    NaDataFile	*dfIn = NULL;
    if(argc > 2)
      {
	dfIn = OpenInputDataFile(argv[2]);
	if(NULL == dfIn)
	  throw(na_cant_open_file);
	bNotEmpty = dfIn->GoStartRecord();
	if(!bNotEmpty)
	  {
	    fprintf(stderr, "Error: Empty input file\n");
	    throw(na_end_of_file);
	  }
      }

    // Open output data: stdout or given file
    NaDataFile	*dfOut = NULL;
    if(argc > 3)
      {
	dfOut = OpenOutputDataFile(argv[3], bdtAuto, nOutDim);
	if(NULL == dfOut)
	  throw(na_cant_open_file);
      }

    nnu.Reset();
    while(read_input(dfIn, nInpDim, pInpData))
      {
	nnu.Function(pInpData, pOutData);
	write_output(dfOut, nOutDim, pOutData);
      }

    delete[] pInpData;
    delete[] pOutData;

    delete dfIn;
    delete dfOut;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
