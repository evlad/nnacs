/* NaCSM.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>
#if defined(__MSDOS__) || defined(__WIN32__)
#include <conio.h>
#endif /* DOS & Win */

#include <NaExcept.h>
#include "NaCSM.h"


//---------------------------------------------------------------------------
// Create control system with stream of given length in input or with
// with data files if len=0
NaControlSystemModel::NaControlSystemModel (int len, NaControllerKind ckind)
: net("nncp0pn"), nSeriesLen(len), eContrKind(ckind), vInitial(1),
  bUseCuSum(true), bUseTDG(false), bPlantSSM(false),
  setpnt_inp("setpnt_inp"),
  setpnt_gen("setpnt_gen"),
  chkpnt_r("chkpnt_r"),
  bus2("bus2"),
  bus3("bus3"),
  err_acc("err_acc"),
  nn_x("nn_x"),
  delay("delay"),
  delta_e("delta_e"),
  cmp("cmp"),
  chkpnt_e("chkpnt_e"),
  controller("controller"),
  chkpnt_u("chkpnt_u"),
  noise_inp("noise_inp"),
  noise_gen("noise_gen"),
  chkpnt_n("chkpnt_n"),
  p_plant(NULL),
  plant_cof("plant"),
  plant_ssm("plant"),
  chkpnt_y("chkpnt_y"),
  onsum("onsum"),
  chkpnt_ny("chkpnt_ny"),
  cmp_e("cmp_e"),
  cusum("cusum"),
  cusum_out("cusum_out"),
  dodetect("dodetect"),
  statan_cerr("statan_cerr"),
  statan_r("statan_r"),
  nnplant("nnplant"),
  nn_y("nn_y"),
  nn_e("nn_e"),
  fill_nn_y("fill_nn_y"),
  bus_p("bus_p"),
  iderrcomp("iderrcomp"),
  statan_iderr("statan_iderr"),
  skip_u("skip_u"),
  skip_y("skip_y"),
  delay_u("delay_u"),
  delay_y("delay_y"),
  trdgath("trdgath"),
  tdg_u("tdg_u"),
  tdg_ny("tdg_ny"),
  cerr_fout("cerr_fout"),
  iderr_fout("iderr_fout"),
  plant_x("plant_x"),
  skip_r("skip_r"),
  nnpback("nnpback"),
  nnp_bpe("nnp_bpe")
{
  vInitial.init_zero();
}


//---------------------------------------------------------------------------
// Destroy the object
NaControlSystemModel::~NaControlSystemModel ()
{
    // Nothing to do
}


//---------------------------------------------------------------------------

////////////////////
// Network phases //
////////////////////

//---------------------------------------------------------------------------
// Link the network (tune the net before)
void
NaControlSystemModel::link_net ()
{
    try{
        // Link the network
        net.link_nodes(
		       (0==nSeriesLen)?
		       (NaPetriNode*)&setpnt_inp: (NaPetriNode*)&setpnt_gen,
		       &chkpnt_r,
                       &cmp,
                       &chkpnt_e,
                       NULL);

	switch(eContrKind)
	  {
	  case NaLinearContr:
	    net.link(&chkpnt_e.out, &controller.x);
	    break;
	  case NaNeuralContrDelayedE:
	    net.link(&chkpnt_e.out, &delay.in);
	    net.link(&delay.dout, &controller.x);
	    net.link(&delay.dout, &nn_x.in);
	    break;
	  case NaNeuralContrER:
	    net.link(&chkpnt_r.out, &bus2.in1);
	    net.link(&chkpnt_e.out, &bus2.in2);
	    net.link(&bus2.out, &controller.x);
	    net.link(&bus2.out, &nn_x.in);
	    break;
	  case NaNeuralContrERSumE:
	    net.link(&chkpnt_r.out, &bus3.in1);
	    net.link(&chkpnt_e.out, &bus3.in2);
	    net.link(&chkpnt_e.out, &err_acc.in);
	    net.link(&err_acc.sum, &bus3.in3);
	    net.link(&bus3.out, &controller.x);
	    net.link(&bus3.out, &nn_x.in);
	    break;
	  case NaNeuralContrEdE:
	    net.link(&chkpnt_e.out, &bus2.in1);
	    net.link(&chkpnt_e.out, &delta_e.x);
	    net.link(&delta_e.dx, &bus2.in2);
	    net.link(&bus2.out, &controller.x);
	    net.link(&bus2.out, &nn_x.in);
	    break;
	  }

	net.link_nodes(
		       &controller,
		       &chkpnt_u,
		       p_plant,
		       &chkpnt_y,
		       &onsum,
		       &chkpnt_ny,
		       NULL);

        net.link_nodes(
		       (0==nSeriesLen)?
		       (NaPetriNode*)&noise_inp: (NaPetriNode*)&noise_gen,
                       &chkpnt_n,
                       NULL);

        net.link(&chkpnt_ny.out, &cmp.aux);

	/* training data gathering */
	if(bUseTDG && bUseCuSum)
	  {
	    net.link(&chkpnt_u.out, &trdgath.in1);
	    net.link(&chkpnt_ny.out, &trdgath.in2);
	    net.link(&trdgath.out1, &tdg_u.in);
	    net.link(&trdgath.out2, &tdg_ny.in);
	  }

	/* NN-P BEGIN */
	if(nnplant.get_nn_unit() != NULL)
	  {
	    net.link(&chkpnt_ny.out, &skip_y.in);
	    net.link(&chkpnt_u.out, &skip_u.in);

	    net.link(&skip_y.out, &delay_y.in);
	    net.link(&skip_u.out, &delay_u.in);

	    net.link(&delay_u.dout, &bus_p.in1);
	    net.link(&delay_y.dout, &bus_p.in2);
	    net.link(&bus_p.out, &nnplant.x);

	    net.link(&nnplant.y, &fill_nn_y.in);
	    net.link(&chkpnt_ny.out, &iderrcomp.aux);
	    net.link(&fill_nn_y.out, &iderrcomp.main);
	    net.link(&iderrcomp.cmp, &statan_iderr.signal);
	    net.link(&iderrcomp.cmp, &nn_e.in);
	    net.link(&statan_iderr.stat, &iderr_fout.in);

	    //net.link(&nnplant.y, &fill_nn_y.in);
	    net.link(&fill_nn_y.out, &nn_y.in);
	    //net.link(&iderrstat.stat, &iderr_fout.in);

	    if(bUseCuSum)
	      {
		net.link(&iderrcomp.cmp, &cusum.x);
		net.link(&cusum.sum, &cusum_out.in);
		net.link(&cusum.d, &dodetect.events);
		if(bUseTDG)
		  net.link(&cusum.d, &trdgath.turnon);
	      }
	  }
	/* NN-P END */

        net.link(&chkpnt_n.out, &onsum.aux);

        net.link(&chkpnt_r.out, &cmp_e.main);
        net.link(p_plant->main_output_cn(), &cmp_e.aux);
	if(bPlantSSM)
	  net.link(&plant_ssm.x, &plant_x.in);

        net.link(&cmp_e.cmp, &statan_cerr.signal);
	net.link(&statan_cerr.stat, &cerr_fout.in);

	net.link(&chkpnt_r.out, &skip_r.in);

	/* NN-P BEGIN */
	if(nnplant.get_nn_unit() != NULL)
	  {
	      net.link(&nnplant.y, &nnpback.nnout);
	      net.link(&skip_r.out, &nnpback.desout);

	      net.link(&nnpback.errinp, &nnp_bpe.in);
	  }
	/* NN-P END */

        net.link(&chkpnt_r.out, &statan_r.signal);

    }catch(NaException ex){
        NaPrintLog("EXCEPTION at linkage phase: %s\n", NaExceptionMsg(ex));
    }
}


//---------------------------------------------------------------------------
// Run the network
NaPNEvent
NaControlSystemModel::run_net ()
{
    try{
	NaVector	rMain(onsum.main.data().dim());
	NaVector	rAux(onsum.aux.data().dim());
	rMain.init_value(1.);
	rAux.init_value(-1.);

	cmp.cmp.set_starter(vInitial);
	onsum.set_gain(rMain, rAux);

	net.set_timing_node((NaPetriNode*)&cmp);

        // Prepare petri net engine
        if(!net.prepare()){
            NaPrintLog("IMPORTANT: verification failed!\n");
        }
        else{
            NaPNEvent       pnev;

#if defined(__MSDOS__) || defined(__WIN32__)
            printf("Press 'q' or 'x' for exit\n");
#endif /* DOS & Win */

            // Activities cycle
            do{
                pnev = net.step_alive();

                idle_entry();

		if(0 != nSeriesLen && setpnt_gen.activations() > nSeriesLen)
		    pnev = pneDead;

                if(user_break())
                    pnev = pneTerminate;

            }while(pneAlive == pnev);

            net.terminate();

            return pnev;
        }
    }
    catch(NaException& ex){
        NaPrintLog("EXCEPTION at runtime phase: %s\n", NaExceptionMsg(ex));
    }
    return pneError;
}


//---------------------------------------------------------------------------

//////////////////
// Overloadable //
//////////////////

//---------------------------------------------------------------------------
// Check for user break
bool
NaControlSystemModel::user_break ()
{
#if defined(__MSDOS__) || (defined(__WIN32__) && !defined(__MINGW32__))
    if(kbhit()){
        int c = getch();
        if('x' == c || 'q' == c){
            return true;
        }
    }
#endif /* DOS & Win */
    return false;
}


//---------------------------------------------------------------------------
// Each cycle callback
void
NaControlSystemModel::idle_entry ()
{
    // Dummy
    printf("Sample %u\r", net.timer().CurrentIndex());
    fflush(stdout);
}


//---------------------------------------------------------------------------
// Set initial value of control error
void
NaControlSystemModel::set_initial_cerror (const NaVector& v)
{
  vInitial = v;
}


//---------------------------------------------------------------------------
// Set flag of using cummulative sum features
void
NaControlSystemModel::set_cusum_flag (bool use_cusum)
{
  bUseCuSum = use_cusum;
}


//---------------------------------------------------------------------------
// Set flag of using training data gather features
void
NaControlSystemModel::set_tdg_flag (bool use_tdg)
{
  bUseTDG = use_tdg;
}


//---------------------------------------------------------------------------
// Set plant function
void
NaControlSystemModel::set_plant_function (NaCombinedFunc& cofunc)
{
  plant_cof.set_transfer_func(&cofunc);
  p_plant = &plant_cof;
  bPlantSSM = false;
}


//---------------------------------------------------------------------------
// Set plant model
void
NaControlSystemModel::set_plant_ss_model (NaStateSpaceModel& ssm)
{
  plant_ssm.set_ss_model(&ssm);
  p_plant = &plant_ssm;
  bPlantSSM = true;
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
