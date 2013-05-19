/* dprod.cpp */
static char rcsid[] = "$Id$";

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include <NaLogFil.h>
#include <NaGenerl.h>
#include <NaExcept.h>

#include <NaDataIO.h>


/***********************************************************************
 * Read discrete signal, multiply it by constant and print the product
 * to output.
 ***********************************************************************/
int main (int argc, char* argv[])
{
  if(argc != 3)
    {
      fprintf(stderr, "Error: need 2 arguments\n");
      fprintf(stderr, "Usage: dmult Multiplier SignalSeries\n");
      return 1;
    }

  NaOpenLogFile("dmult.log"); 

  double	fCoef = atof(argv[1]);

  try{
    NaDataFile	*dfSeries = OpenInputDataFile(argv[2]);

    dfSeries->GoStartRecord();

    do{
      printf("%g\n", dfSeries->GetValue() * fCoef);
    }while(dfSeries->GoNextRecord());

    delete dfSeries;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
