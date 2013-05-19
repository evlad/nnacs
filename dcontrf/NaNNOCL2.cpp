/* NaNNOCL2.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>
#if defined(__MSDOS__) || defined(__WIN32__)
#include <conio.h>
#endif /* DOS & Win */

#include <NaExcept.h>
#include "NaNNOCL2.h"


//---------------------------------------------------------------------------
// Create NN-C training control system
NaNNOptimContrLearnClosedLoop
::NaNNOptimContrLearnClosedLoop (NaControllerKind ckind)
  : net("nncp3pn"), eContrKind(ckind),
    setpnt_gen("setpnt_gen"),
    on_y("on_y"),
    nn_y("nn_y"),
    nn_u("nn_u"),
    nncontr("nncontr"),
    nnplant("nnplant"),
    plant_u("plant_u"),
    plant_y("plant_y"),
    nnteacher("nnteacher"),
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
    switch_y("switch_y"),
    trig_e("trig_e"),
    delay_u("delay_u"),
    delay_y("delay_y"),
    errfetch("errfetch")
{
  net.set_timing_node(&setpnt_gen);
}


//---------------------------------------------------------------------------
// Destroy the object
NaNNOptimContrLearnClosedLoop::~NaNNOptimContrLearnClosedLoop ()
{
  // Deactivate the network
  net.terminate();
}


//---------------------------------------------------------------------------

////////////////////
// Network phases //
////////////////////

//---------------------------------------------------------------------------
// Link the network (tune the net before)
bool
NaNNOptimContrLearnClosedLoop::NaNNOptimContrLearnClosedLoop::link_net ()
{
  try{
    // Link the network

    switch(eContrKind)
      {
      case NaNeuralContrER:
	net.link(&setpnt_gen.out, &bus_c.in1);
	net.link(&setpnt_gen.out, &cerrcomp.main);
	net.link(&cerrcomp.cmp, &bus_c.in2);
	net.link(&bus_c.out, &nncontr.x);
	break;
      case NaNeuralContrDelayedE:
	net.link(&setpnt_gen.out, &cerrcomp.main);
	net.link(&cerrcomp.cmp, &delay_c.in);
	net.link(&delay_c.dout, &nncontr.x);
	break;
      case NaNeuralContrEdE:
	net.link(&setpnt_gen.out, &cerrcomp.main);
	net.link(&cerrcomp.cmp, &bus_c.in1);
	net.link(&cerrcomp.cmp, &delta_e.x);
	net.link(&delta_e.dx, &bus_c.in2);
	net.link(&bus_c.out, &nncontr.x);
	break;
      }

    net.link(&nncontr.y, &nn_u.in);

    net.link(&nn_u.out, &plant_u.in);
    /* u -> plant_u.in { real plant plant_y.out -> y } */
    net.link(&plant_y.out, &on_y.in);

    net.link(&delay_u.dout, &bus_p.in1);
    net.link(&delay_y.dout, &bus_p.in2);
    net.link(&bus_p.out, &nnplant.x);

    net.link(&plant_y.out, &delay_y.in);
    net.link(&nn_u.out, &delay_u.in);

    net.link(&delay_u.sync, &land.in1);
    net.link(&delay_y.sync, &land.in2);

    net.link(&land.out, &trig_e.turn);
    net.link(&cerrcomp.cmp, &trig_e.in);
    net.link(&trig_e.out, &errbackprop.errout);

    net.link(&errbackprop.errinp, &errfetch.in);
    net.link(&errfetch.out, &nnteacher.errout);

    net.link(&land.out, &switch_y.turn);
    net.link(&nnplant.y, &switch_y.in1);
    net.link(&plant_y.out, &switch_y.in2);
    net.link(&switch_y.out, &nn_y.in);

    net.link(&plant_y.out, &iderrcomp.main);
    net.link(&switch_y.out, &iderrcomp.aux);
    net.link(&iderrcomp.cmp, &iderrstat.signal);

    net.link(&plant_y.out, &cerrcomp.aux);
    net.link(&cerrcomp.cmp, &cerrstat.signal);

  }catch(NaException ex){
    NaPrintLog("EXCEPTION at linkage phase: %s\n", NaExceptionMsg(ex));
    return false;
  }

  // Prepare petri net engine
  if(!net.prepare(/* true */))
    {
      NaPrintLog("IMPORTANT: verification failed!\n");
      return false;
    }

  return true;
}


//---------------------------------------------------------------------------
// Prepare learning session
bool
NaNNOptimContrLearnClosedLoop::begin_session ()
{
  try{
    // Initialize/re-initialize petri net engine
    net.initialize();

  }catch(NaException ex){
    NaPrintLog("EXCEPTION at linkage phase: %s\n", NaExceptionMsg(ex));
    return false;
  }

  return true;
}


//---------------------------------------------------------------------------
// Do one step of operating outside real-world closed loop system
// Pass data to and from NN: y(k), u(k)->y(k+1)
NaPNEvent
NaNNOptimContrLearnClosedLoop::net_step (const NaReal* y, NaReal* u)
{
  try{ 
    NaPNEvent	pnev;

    // Put experimental data to the network
    plant_y.put_data(y);

    // Get controller force
    plant_u.get_data(u);

    // Activities cycle
    do{
      pnev = net.step_alive();

      idle_entry();

    }while(pneAlive == pnev);

    if(user_break())
      pnev = pneTerminate;

    return pnev;
  }
  catch(NaException& ex){
    NaPrintLog("EXCEPTION at runtime phase: %s\n", NaExceptionMsg(ex));
  }
}


//---------------------------------------------------------------------------
// Close learning session (update trained NN-C)
bool
NaNNOptimContrLearnClosedLoop::end_session ()
{
  try{
    nnteacher.update_nn();

  }catch(NaException ex){
    NaPrintLog("EXCEPTION at end of session: %s\n", NaExceptionMsg(ex));
    return false;
  }

  return true;
}


//---------------------------------------------------------------------------

//////////////////
// Overloadable //
//////////////////

//---------------------------------------------------------------------------
// Check for user break
bool
NaNNOptimContrLearnClosedLoop::user_break ()
{
#if defined(__MSDOS__) || defined(__WIN32__)
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
NaNNOptimContrLearnClosedLoop::idle_entry ()
{
  // Nothing to do
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
