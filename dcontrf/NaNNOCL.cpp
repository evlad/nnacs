/* NaNNOCL.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>
#if defined(__MSDOS__) || defined(__WIN32__)
//#include <conio.h>
#endif /* DOS & Win */

#include <NaExcept.h>
#include "NaNNOCL.h"


volatile bool	NaNNOptimContrLearn::bUserBreak = false;

//---------------------------------------------------------------------------
// Create NN-C training control system with stream of given length
// in input or with data files if len=0
NaNNOptimContrLearn::NaNNOptimContrLearn (int len, NaControllerKind ckind,
					  const char* szNetName)
  : net(szNetName), nSeriesLen(len), eContrKind(ckind),
    setpnt_inp("setpnt_inp"),
    setpnt_gen("setpnt_gen"),
    setpnt_out("setpnt_out"),
    noise_inp("noise_inp"),
    noise_gen("noise_gen"),
    noise_out("noise_out"),
    on_y("on_y"),
    nn_y("nn_y"),
    nn_u("nn_u"),
    tradcontr("tradcontr"),
    nncontr("nncontr"),
    nnplant("nnplant"),
    nnplant2("nnplant2"),
    plant("plant"),
    nncteacher("nncteacher"),
    nnpteacher("nnpteacher"),
    errbackprop("errbackprop"),
    bus_p("bus_p"),
    bus_c("bus_c"),
    delay_c("delay_c"),
    delta_e("delta_e"),
    sum_on("sum_on"),
    iderrcomp("iderrcomp"),
    iderrstat("iderrstat"),
    cerrcomp("cerrcomp"),
    cerrstat("cerrstat"),
    devcomp("devcomp"),
    skip_r("skip_r"),
    skip_e("skip_e"),
    skip_u("skip_u"),
    skip_y("skip_y"),
    skip_ny("skip_ny"),
    fill_nn_y("fill_nn_y"),
    delay_u("delay_u"),
    delay_y("delay_y"),
    errfetch("errfetch"),
    cerr_fout("cerr_fout"),
    iderr_fout("iderr_fout"),
    cerr_qout("cerr_qout"),
    iderr_qout("iderr_qout"),
    c_in("c_in"),
    p_in("p_in")
{
  // Nothing to do
}


//---------------------------------------------------------------------------
// Destroy the object
NaNNOptimContrLearn::~NaNNOptimContrLearn ()
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
NaNNOptimContrLearn::link_net ()
{
  try{
    // Link the network

    switch(eContrKind)
      {
      case NaLinearContr:
	if(0 == nSeriesLen){
	  net.link_1n(&setpnt_inp.out,
		      &devcomp.main, &cerrcomp.main, NULL);
	}else{
	  net.link_1n(&setpnt_gen.y,
		      &setpnt_out.in,
		      &devcomp.main, &cerrcomp.main, NULL);
	}
	net.link(&devcomp.cmp, &tradcontr.x);
	break;
      case NaNeuralContrER:
	if(0 == nSeriesLen){
	  net.link_1n(&setpnt_inp.out,
		      &bus_c.in1, &skip_r.in, &cerrcomp.main, NULL);
	}else{
	  net.link_1n(&setpnt_gen.y,
		      &setpnt_out.in,
		      &bus_c.in1, &skip_r.in, &cerrcomp.main, NULL);
	}
	net.link(&skip_r.out, &devcomp.main);
	net.link(&devcomp.cmp, &bus_c.in2);
	net.link(&bus_c.out, &nncontr.x);
	break;
      case NaNeuralContrDelayedE:
	if(0 == nSeriesLen){
	  net.link_1n(&setpnt_inp.out,
		      &skip_r.in, &cerrcomp.main, NULL);
	}
	else{
	  net.link_1n(&setpnt_gen.y,
		      &setpnt_out.in,
		      &skip_r.in, &cerrcomp.main, NULL);
	}
	net.link(&skip_r.out, &devcomp.main);
	net.link(&devcomp.cmp, &delay_c.in);
	net.link(&delay_c.dout, &nncontr.x);
	break;
      case NaNeuralContrEdE:
	if(0 == nSeriesLen){
	  net.link_1n(&setpnt_inp.out,
		      &skip_r.in, &cerrcomp.main, NULL);
	}
	else{
	  net.link_1n(&setpnt_gen.y,
		      &setpnt_out.in,
		      &skip_r.in, &cerrcomp.main, NULL);
	}
	net.link(&skip_r.out, &devcomp.main);
	net.link(&devcomp.cmp, &bus_c.in1);
	net.link(&devcomp.cmp, &delta_e.x);
	net.link(&delta_e.dx, &bus_c.in2);
	net.link(&bus_c.out, &nncontr.x);
	break;
      }

    if(eContrKind == NaLinearContr)
      net.link(&devcomp.cmp, &c_in.in);
    else
      net.link(&bus_c.out, &c_in.in);

    if(eContrKind == NaLinearContr)
      net.link(&tradcontr.y, &nn_u.in);
    else
      net.link(&nncontr.y, &nn_u.in);
    net.link(&nn_u.out, &plant.x);

    net.link(&plant.y, &sum_on.main);
    if(0 == nSeriesLen)
      net.link(&noise_inp.out, &sum_on.aux);
    else{
      net.link(&noise_gen.y, &noise_out.in);
      net.link(&noise_gen.y, &sum_on.aux);
    }
    net.link(&sum_on.sum, &on_y.in);

    net.link(&on_y.out, &skip_y.in);
    net.link(&nn_u.out, &skip_u.in);

    net.link(&skip_y.out, &delay_y.in);
    net.link(&skip_u.out, &delay_u.in);

    net.link(&delay_u.dout, &bus_p.in1);
    net.link(&delay_y.dout, &bus_p.in2);
    net.link(&bus_p.out, &nnplant.x);
    net.link(&bus_p.out, &nnplant2.x);

    net.link(&bus_p.out, &p_in.in);

    net.link(&cerrcomp.cmp, &skip_e.in);
    net.link(&skip_e.out, &errbackprop.errout);

    net.link(&errbackprop.errinp, &errfetch.in);
    if(eContrKind != NaLinearContr)
      net.link(&errfetch.out, &nncteacher.errout);

    net.link(&on_y.out, &skip_ny.in);
    net.link(&skip_ny.out, &iderrcomp.aux);
    net.link(&nnplant2.y, &iderrcomp.main);
    net.link(&iderrcomp.cmp, &iderrstat.signal);

    net.link(&iderrcomp.cmp, &nnpteacher.errout);

    net.link(&nnplant2.y, &fill_nn_y.in);
    net.link(&fill_nn_y.out, &nn_y.in);

    net.link(&on_y.out, &devcomp.aux);
    net.link(&on_y.out, &cerrcomp.aux);
    net.link(&skip_e.out, &cerrstat.signal);

    if(false/*0 == nSeriesLen*/){
      net.link(&cerrstat.stat, &cerr_qout.in);
      net.link(&iderrstat.stat, &iderr_qout.in);
    }
    else{
      net.link(&cerrstat.stat, &cerr_fout.in);
      net.link(&iderrstat.stat, &iderr_fout.in);
    }

  }catch(NaException ex){
    NaPrintLog("EXCEPTION at linkage phase: %s\n", NaExceptionMsg(ex));
  }
}


//---------------------------------------------------------------------------
// Handle signals during neural network training
#ifndef WIN32
void
NaNNOptimContrLearn::on_signal (int nSig, siginfo_t* pInfo, void* pData)
{
  bUserBreak = true;
}
#endif

//---------------------------------------------------------------------------
// Run the network
NaPNEvent
NaNNOptimContrLearn::run_net ()
{
  // Catch signals during learning to make smoother stop
#ifndef WIN32
  struct sigaction act;
  act.sa_sigaction = &on_signal;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGPIPE, &act, NULL);
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);
#endif

  try{ 
    NaVector	rZero(1);
    rZero.init_zero();
    NaVector	rMain(1), rAux(1);
    rMain.init_value(1.);
    rAux.init_value(-1.);

    if(eContrKind != NaLinearContr)
      skip_r.set_skip_number(1);

    devcomp.cmp.set_starter(rZero);
    sum_on.set_gain(rMain, rAux);

    net.set_timing_node((0==nSeriesLen)
			? (NaPetriNode*)&setpnt_inp
			: (NaPetriNode*)&setpnt_gen);

    // Prepare petri net engine
    if(!net.prepare()){
      NaPrintLog("IMPORTANT: verification failed!\n");
    }
    else{
      NaPNEvent       pnev;

      // Activities cycle
      do{
	pnev = net.step_alive();

	idle_entry();

	if(pneAlive == pnev)
	  {
	    if(0 != nSeriesLen && setpnt_gen.activations() > nSeriesLen)
	      pnev = pneDead;
	    else if(user_break())
	      pnev = pneTerminate;
	  }

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
NaNNOptimContrLearn::user_break ()
{
#if defined(__MSDOS__) || defined(__WIN32__)
  if(kbhit()){
    int c = getch();
    if('x' == c || 'q' == c){
      return true;
    }
  }
#endif /* DOS & Win */
  return bUserBreak;
}


//---------------------------------------------------------------------------
// Each cycle callback
void
NaNNOptimContrLearn::idle_entry ()
{
  // Nothing to do
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
