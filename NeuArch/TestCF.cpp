/* TestCF.cpp */
static char rcsid[] = "$Id$";

#include <stdio.h>

#include "NaCoFunc.h"


/* Test for NaCombinedFunc class */
int
main (int argc, char* argv[])
{
  if(argc != 2){
    fprintf(stderr, "Usage: TestCF file.cof\n");
    return 1;
  }

  NaOpenLogFile("TestCF.log");

  NaCombinedFunc	*cofu = NULL;

  try{
    cofu = new NaCombinedFunc();

    cofu->Load(argv[1]);

    NaReal	x[] = { 0.1, -0.2, 0.3, -0.4, 0.5, -0.6, 0.7, -0.8, 0.9, -1 };
    NaReal	y[NaNUMBER(x)];
    unsigned	i;

    cofu->PrintLog();
    cofu->Reset();

    for(i = 0; i < NaNUMBER(x); ++i){
      cofu->Function(x + i, y + i);
      NaPrintLog("x[%u]=%g\t y[%u]=%g\n", i, x[i], i, y[i]);
    }

    delete cofu;
  }
  catch(NaException ex){
    NaPrintLog("main: %s\n", NaExceptionMsg(ex));
    delete cofu;
  }
  return 0;
}
