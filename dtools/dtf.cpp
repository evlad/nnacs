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
#include <NaParams.h>


/***********************************************************************
 * Feed input series to given discrete transfer function and put the
 * result to output series.
 ***********************************************************************/
int main (int argc, char* argv[])
{
    char	*dtf_file = NULL, *in_file = NULL, *out_file = NULL, *par_file = NULL;

  switch(argc){
  case 2:
      par_file = argv[1];
      break;

  case 4:
      dtf_file = argv[1];
      in_file = argv[2];
      out_file = argv[3];
      break;

  default:
      fprintf(stderr, "Error: need 1 or 3 arguments:\n");
      printf("Usage: dtf DiscrTrFunc InSeries OutSeries\n"	\
	     "  or   dtf ParFile\n");
      return 1;
  }

  NaOpenLogFile("dtf.log");

  try{
    NaUnit	*func = NULL;
    NaCombinedFunc	dcof;
    NaTransFunc		dtf;
    NaStateSpaceModel	dssm;

    if(NULL != par_file){
	NaParams	par(par_file, 0, NULL);

	NaPrintLog("Run dtf with %s\n", par_file);

	par.ListOfParamsToLog();
	dtf_file = strdup(par("plant_tf"));
	in_file = strdup(par("in_file"));
	out_file = strdup(par("out_file"));
    }

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
    if(NULL == dfIn)
	throw(na_cant_open_file);
    NaDataFile	*dfOut = OpenOutputDataFile(out_file, bdtAuto,
					    func->OutputDim());
    if(NULL == dfOut)
	throw(na_cant_open_file);

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
