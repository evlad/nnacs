/* dstatsb.cpp */
static char rcsid[] = "$Id$";

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include "NaLogFil.h"
#include "NaGenerl.h"
#include "NaExcept.h"

#include "NaPetri.h"
#include "NaPNStat.h"
#include "NaPNCmp.h"
#include "NaPNFIn.h"
#include "NaPNFOut.h"
#include "NaPNFetc.h"


/** Statistics in sliding base. */
int main(int argc, char **argv)
{
    char    *fname[2], *ofile;
    bool    bTwoSeries = false;
    if(7 == argc)
	bTwoSeries = true;

    if(6 != argc && 7 != argc){
        printf("Usage: dstatsb Options ErrorFile OutputFile\n"
	       "    or dstatsb Options SignalFile1 SignalFile2 OutputFile\n"\
	       " Options - SlidingBase DataColumn StatFormat:\n"\
	       " SlidingBase - number of points in sliding base or 0 for summary statistics\n"\
	       " DataColumn  - variable number to fetch from the data file(s)\n"\
	       " StatFormat  - ALL or ABSMEAN,MEAN,RMS,STDDEV,MAX,MIN,ABSMAX,TIME,TINDEX,VALUE\n");
        return 2;
    }

    int nSlBase = atoi(argv[1]);
    int iCol = atoi(argv[2]);
    if(iCol > 0)
	--iCol;
    char *szStatMask = (!strcasecmp("ALL", argv[3]) || strlen(argv[3]) == 0)? NULL: argv[3];
    if(bTwoSeries) {
	fname[1] = argv[5];
	ofile = argv[6];
    } else {
	fname[0] = argv[4];
	ofile = argv[5];
    }

    NaOpenLogFile("dstatsb.log");

    try{
        // Main Petri network module
        NaPetriNet      net("dstatsb");

        // Functional Petri network nodes
        NaPNFileInput   series1("series1");
        NaPNFileInput   series2("series2");
	NaPNFetcher     fetcher1("fetcher1");
	NaPNFetcher     fetcher2("fetcher2");
        NaPNComparator  compare("compare");
        NaPNStatistics  statan("statan");
        NaPNFileOutput  output("output");

	// To increment timer
	net.set_timing_node(&statan);

        // Link the network
	if(bTwoSeries) {
	    net.link(&series1.out, &fetcher1.in);
	    net.link(&fetcher1.out, &compare.main);
	    net.link(&series2.out, &fetcher2.in);
	    net.link(&fetcher2.out, &compare.aux);
	    net.link(&compare.cmp, &statan.signal);
	    net.link(&statan.stat, &output.in);
	} else {
	    net.link(&series1.out, &fetcher1.in);
	    net.link(&fetcher1.out, &statan.signal);
	    net.link(&statan.stat, &output.in);
	}

        // Configure nodes
        output.set_output_filename(ofile);
        series1.set_input_filename(fname[0]);
	if(bTwoSeries)
	    series2.set_input_filename(fname[1]);
	fetcher1.set_output(iCol);
	fetcher2.set_output(iCol);
	if(szStatMask == NULL)
	    statan.configure_output(NaSM_ALL);
	else
	    statan.configure_output(szStatMask);
	if(nSlBase >= 0)
	    statan.set_floating_gap(nSlBase);

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
