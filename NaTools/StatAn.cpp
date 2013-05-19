
//---------------------------------------------------------------------------
// Statistics computation for the given input file
// (C)opyright by Eliseev Vladimir
//---------------------------------------------------------------------------

#include "NaLogFil.h"
#include "NaGenerl.h"
#include "NaExcept.h"

#include "NaPetri.h"
#include "NaPNStat.h"
#include "NaPNCmp.h"
#include "NaPNFIn.h"


//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    char    *fname[2];
    
    NaOpenLogFile("StatAn.log");

    if(2 != argc && 3 != argc){
        printf("Usage: StatAn ErrorFile\n"
	       "    or StatAn SignalFile1 SignalFile2\n");
        return 2;
    }

    fname[0] = argv[1];
    if(3 == argc)
      fname[1] = argv[2];

    try{
        // Main Petri network module
        NaPetriNet      net("statan");

        // Functional Petri network nodes
        NaPNFileInput   series1("series1");
        NaPNFileInput   series2("series2");
        NaPNComparator  compare("compare");
        NaPNStatistics  statan("statistics");

        // Link the network
	if(3 == argc)
	  {
	    net.link(&series1.out, &compare.main);
	    net.link(&series2.out, &compare.aux);
	    net.link(&compare.cmp, &statan.signal);
	  }
	else /* if(2 == argc) */
	  {
	    net.link(&series1.out, &statan.signal);
	  }

        // Configure nodes
        series1.set_input_filename(fname[0]);
	if(3 == argc)
	  series2.set_input_filename(fname[1]);

        // Prepare petri net engine
        if(!net.prepare()){
            NaPrintLog("IMPORTANT: verification failed!\n");
        }
        else{
            NaPNEvent       pnev;

            do{
                pnev = net.step_alive();
            }while(pneAlive == pnev);

            net.terminate();

            NaPrintLog("IMPORTANT: net is dead due to ");
            switch(pnev){
            case pneTerminate:
                NaPrintLog("user break.\n");
                break;

            case pneHalted:
                NaPrintLog("internal halt.\n");
                break;

            case pneDead:
                NaPrintLog("data are exhausted.\n");
                break;

            default:
                NaPrintLog("unknown reason.\n");
                break;
            }

            statan.print_stat(NULL);
        }
    }
    catch(NaException& ex){
        NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
    }

    return 0;
}
