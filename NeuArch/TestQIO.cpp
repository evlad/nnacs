/* TestQIO.cpp */
static char rcsid[] = "$Id$";

#pragma hdrstop
//#include <condefs.h>

#include "NaPetNet.h"
#include "NaPNQIn.h"
#include "NaPNQOut.h"

//---------------------------------------------------------------------------
//USELIB("NeuArch.lib");
//---------------------------------------------------------------------------
#pragma argsused
int
main (int argc, char **argv)
{
  NaOpenLogFile("TestQIO.log");

  try{
    NaPetriNet	net;
    NaPNQueueInput	qin;
    NaPNQueueOutput	qout;

    qin.verbose(true);
    qout.verbose(true);

    qin.set_data_dim(1);

    /* linkage phase */
    net.link(&qin.out, &qout.in);

    /* runtime phase */
    if(!net.prepare()){
      NaPrintLog("IMPORTANT: verification failed!\n");
    }
    else{
      NaPNEvent	pnev = pneDead;
      NaReal	value = 777.0;

      while(pneDead == pnev)
	{
	  qin.put_data(&value);

	  // Activities cycle
	  do{
	    pnev = net.step_alive();
	  }while(pneAlive == pnev);

	  qout.get_data(&value);
	  printf("qout=%g\n", value);

	  // New value
	  scanf("%lg", &value);
	}

      net.terminate();
    }

    }catch(NaException& ex){
    NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
  }

  return 0;
}
