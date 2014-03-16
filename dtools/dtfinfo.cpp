/* dtf.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaConfig.h>
#include <NaCoFunc.h>
#include <NaTrFunc.h>
#include <NaSSModel.h>


/***********************************************************************
 * Feed input series to given discrete transfer function and put the
 * result to output series.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 2)
    {
      fprintf(stderr, "Error: need 1 argument\n");
      printf("Usage: dtfinfo DiscrTrFunc\n");
      return 1;
    }

  char	*dtf_file = argv[1];

  try{
    NaCombinedFunc	dcof;
    NaTransFunc		dtf;
    NaStateSpaceModel	dssm;

    int	len = strlen(dtf_file);
    if(len >= 3 && !strcmp(dtf_file + len - 3, ".tf")){
      dtf.Load(dtf_file);
      dtf.PrintLog();
    }else if(len >= 4 && !strcmp(dtf_file + len - 4, ".cof")){
      dcof.Load(dtf_file);
      dcof.PrintLog();
    }else if(len >= 4 && !strcmp(dtf_file + len - 4, ".ssm")){
      dssm.Load(dtf_file);
      dssm.PrintLog();
    }else{
      NaPrintLog("Unknown file format of '%s'!\n", dtf_file);
      throw(na_cant_open_file);
    }
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
