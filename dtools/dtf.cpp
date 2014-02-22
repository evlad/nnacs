/* dtf.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
#include <locale.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaConfig.h>
#include <NaCoFunc.h>
#include <NaTrFunc.h>
#include <NaDataIO.h>
#include <NaTimer.h>
#include <NaSSModel.h>


/***********************************************************************
 * Feed input series to given discrete transfer function and put the
 * result to output series.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 4)
    {
      fprintf(stderr, "Error: need 3 arguments\n");
      printf("Usage: dtf DiscrTrFunc InSeries OutSeries\n");
      return 1;
    }

  char	*dtf_file = argv[1];
  char	*in_file = argv[2];
  char	*out_file = argv[3];

  NaOpenLogFile("dtf.log");

  try{
    NaUnit	*func = NULL;
    NaCombinedFunc	dcof;
    NaTransFunc		dtf;
    NaStateSpaceModel	dssm;

    int	len = strlen(dtf_file);
    if(len >= 3 && !strcmp(dtf_file + len - 3, ".tf")){
      dtf.Load(dtf_file);
      func = &dtf;
    }else if(len >= 4 && !strcmp(dtf_file + len - 4, ".cof")){
      dcof.Load(dtf_file);
      func = &dcof;
    }else if(len >= 4 && !strcmp(dtf_file + len - 4, ".ssm")){
      dssm.Load(dtf_file);
      func = &dssm;
    }else{
      NaPrintLog("Unknown file format of '%s'!\n", dtf_file);
      throw(na_cant_open_file);
    }

    NaDataFile	*dfIn = OpenInputDataFile(in_file);
    NaDataFile	*dfOut = OpenOutputDataFile(out_file, bdtAuto,
					    func->OutputDim());

    dfIn->GoStartRecord();
    func->Reset();

    NaReal	*pfIn = new NaReal[func->InputDim()];
    NaReal	*pfOut = new NaReal[func->OutputDim()];

    do{
      for(unsigned i = 0; i < func->InputDim(); ++i)
	pfIn[i] = dfIn->GetValue(i);

      func->Function(pfIn, pfOut);

      dfOut->AppendRecord();

      for(unsigned i = 0; i < func->OutputDim(); ++i)
	dfOut->SetValue(pfOut[i], i);

      TheTimer.GoNextTime();
    }while(dfIn->GoNextRecord());

    delete[] pfOut;
    delete[] pfIn;

    delete dfOut;
    delete dfIn;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
