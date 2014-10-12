/* dstatsb.cpp */

#include <stdio.h>
#include <string.h>

#include "NaPNAccu.h"
#include "NaPNFIn.h"
#include "NaPNFOut.h"


/** Statistics in sliding base. */
int main(int argc, char **argv)
{
    char    *iname, *ofile;

    if(4 != argc){
        printf("Usage: daccum Depth InputFile OutputFile\n"\
	       " Depth      - number in sequence of accumulated input values\n"\
	       "              where 0 designates infinite depth, 1 means Input=Output\n"\
	       " InputFile  - input data series\n"\
	       " OutputFile - output data of accumulated input data samples\n");
        return 2;
    }

    unsigned nDepth = atoi(argv[1]);
    iname = argv[2];
    ofile = argv[3];

    NaOpenLogFile("daccum.log");

    try{
        // Main Petri network module
        NaPetriNet      net("daccum");

        // Functional Petri network nodes
	NaPNAccumulator accum("accum");
	NaPNFileInput   series("series");
	NaPNFileOutput  output("output");

        // Link the network
	net.link_nodes(&series, &accum, &output, NULL);

        // Configure nodes
        output.set_output_filename(ofile);
        series.set_input_filename(iname);
	accum.set_accum_depth(nDepth);

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
        }
    }
    catch(NaException& ex){
        NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
    }

    return 0;
}
