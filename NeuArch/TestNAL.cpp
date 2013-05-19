/* TestNAL.cpp */
static char rcsid[] = "$Id$";

#pragma hdrstop
#include <condefs.h>
#include <conio.h>

#include "NaLogFil.h"
#include "NaVector.h"

#include "NaPetri.h"
#include "PNRandom.h"
#include "PNPrintr.h"
#include "NaPNTime.h"
#include "NaPNCmp.h"
#include "NaPNFIn.h"
#include "NaPNFOut.h"
#include "NaPNTrig.h"
#include "NaPNDely.h"
#include "NaPNStat.h"

//---------------------------------------------------------------------------
USELIB("NeuArch.lib");
USEUNIT("PNRandom.cpp");
USEUNIT("PNPrintr.cpp");
USELIB("..\StaDev\stadev32.lib");
USEFILE("..\StaDev\stadev.h");
USELIB("PetriNet.lib");
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    NaPNEvent       pnev;
    NaPetriNet      net;
    NaPNRandomGen   randgen;
    NaPNPrinter     printer;
    NaPNTimer       timer;
    NaPNFileInput   finput;
    NaPNFileOutput  fout_timer("fout_timer");
    NaPNFileOutput  fout_rand("fout_rand");
    NaPNFileOutput  fout_delayed("fout_delayed");
    NaPNFileOutput  foutput("foutput");
    NaPNComparator  compar;
    NaPNTrigger     trigger;
    NaPNDelay       delay;
    NaPNStatistics  stat;

    NaOpenLogFile("TestNAL.log");


    //net.link_nodes(&finput, &printer, NULL);
    //net.link_nodes(&randgen, &compar, &foutput, NULL);

    net.link_nodes(&randgen, &timer, NULL);
    net.link_nodes(&randgen, &stat, NULL);
    net.link(&timer.time, &delay.in);
    net.link(&timer.samp, &trigger.in);

    net.link(&delay.dout, &printer.in);
    net.link(&delay.sync, &trigger.turn);
    net.link(&trigger.out, &fout_delayed.in);

    //net.link(&timer.time, &fout_timer.in);
    //net.link(&randgen.rand, &timer.sync);
    //net.link(&randgen.rand, &fout_rand.in);

    timer.SetSamplingRate(0.2);
    randgen.set_rand_dim(1);
    //fout_timer.set_output_filename("test_tim.dat");
    //fout_rand.set_output_filename("test_rnd.dat");
    //foutput.set_output_filename("test.dat");
    fout_delayed.set_output_filename("test.dat");
    //finput.set_input_filename("test.sta");
    //delay.set_delay(4);

    if(!net.prepare()){
        NaPrintLog("verification failed\n");
    }
    else{
        do{
            pnev = net.step_alive();

            if(kbhit()){
                int c = getch();
                if('x' == c || 'q' == c)
                    pnev = pneTerminate;
            }
        }while(pneAlive == pnev);

        net.terminate();

        NaPrintLog("net is dead: ");
        switch(pnev){
        case pneTerminate:
            NaPrintLog("user break\n");
            break;

        case pneHalted:
            NaPrintLog("internal halt\n");
            break;

        case pneDead:
            NaPrintLog("data are exhausted\n");
            break;
        }

        stat.print_stat("Statistics:");
    }

    return 0;
}
