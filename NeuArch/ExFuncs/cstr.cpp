/* cstr.cpp */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define __NaSharedExternFunction
#include "cstr.h"

///-----------------------------------------------------------------------
/// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaCSTRFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaCSTRFunc::NaCSTRFunc ()
    : par(NULL), out(NULL)
{
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options: cstrpar.def [state.dat]
/// where
/// - cstrpar.defhas NaParams format (do not use .par extension due to
///   switching to new log file!)
/// - state is defined as seven variables: time, c_A, c_B, c_C, T, T_c, q_c
/// initial: c_A, c_B, c_C, T, T_c (at time=0)
NaCSTRFunc::NaCSTRFunc (char* szOptions, NaVector& vInit)
    : par(NULL), out(NULL)
{
    ini.var.c_A = 0.4915;
    ini.var.c_B = 2.0042;
    ini.var.c_C = 1.7243;
    ini.var.T   = 363.61;
    ini.var.T_c = 350.15;

    if(vInit.dim() == sizeof(ini.state)/sizeof(ini.state[0])) {
	for(int i = 0; i < vInit.dim(); ++i) {
	    ini.state[i] = vInit[i];
	}
    }
    NaPrintLog("cstr: initial state: c_A=%g, c_B=%g, c_C=%g, T=%g, T_c=%g\n",
	       ini.var.c_A, ini.var.c_B, ini.var.c_C, ini.var.T, ini.var.T_c);

    q = 0.015;
    V = 0.23;
    V_c = 0.21;
    rho = 1020;
    rho_c = 998;
    Cp = 4.02;
    Cp_c = 4.182;
    A = 1.51;
    k = 42.8;
    k10 = 1.55e11;
    k20 = 4.55e25;
    drH_1 = -8.6e4;
    drH_2 -1.82e4;
    c_Av = 4.22;
    c_Bv = 0;
    c_Cv = 0;
    T_v = 328;
    T_vc = 298;
    q_sc = 0.004;
    E1R = 9850;
    E2R = 22019;

    char *szBuffer = strdup(szOptions);
    char *szParFile = strtok(szBuffer, " ");
    char *szOutFile = strtok(NULL, " ");
    if(NULL == szParFile) {
	NaPrintLog("cstr: no parameters file is specified; using defaults\n");
    } else {
	try{
	    NaPrintLog("cstr: try to open parameters in file '%s'\n", szParFile);
	    par = new NaParams(szParFile);
	} catch(...) {
	    NaPrintLog("cstr: failed to open parameters file '%s'; using defaults\n", szParFile);
	    par = NULL;
	}
	if(NULL != par) {
#define CHECKPAR(x) if(!par->CheckParam(#x))NaPrintLog("cstr: '%s' is not defined; using default value %g\n",#x,x);
#define SETPAR(x) CHECKPAR(x) else x=atof(par->GetParam(#x))
	    SETPAR(q);
	    SETPAR(V);
	    SETPAR(V_c);
	    SETPAR(rho);
	    SETPAR(rho_c);
	    SETPAR(Cp);
	    SETPAR(Cp_c);
	    SETPAR(A);
	    SETPAR(k);
	    SETPAR(k10);
	    SETPAR(k20);
	    SETPAR(drH_1);
	    SETPAR(drH_2);
	    SETPAR(c_Av);
	    SETPAR(c_Bv);
	    SETPAR(c_Cv);
	    SETPAR(T_v);
	    SETPAR(T_vc);
	    SETPAR(E1R);
	    SETPAR(E2R);
#undef SETPAR
#undef CHECKPAR
	}
    }
    if(NULL != szOutFile) {
	NaPrintLog("cstr: state output file parametrs file is '%s'\n",
		   szOutFile);
	out = OpenOutputDataFile(szOutFile);
    }
}


///-----------------------------------------------------------------------
/// Destructor
NaCSTRFunc::~NaCSTRFunc ()
{
    delete par;
    delete out;
}


///-----------------------------------------------------------------------
/// Reset operations, that must be done before new modelling
/// session will start.  It's guaranteed that this reset will be
/// called just after Timer().ResetTime().
void
NaCSTRFunc::Reset ()
{
    cur = ini;
}


// ATTENTION!  It's guaranteed that this->Function() will be called
// only once for each time step.  Index of the time step and current
// time can be got from Timer().

///-----------------------------------------------------------------------
/// Compute output on the basis of internal parameters,
/// stored state and external input: y=F(x,t,p)
void
NaCSTRFunc::Function (NaReal* x, NaReal* y)
{
    if(NULL == x || NULL == y)
	return;

    NaReal dt = Timer().GetSamplingRate();	// seconds
    NaReal time = Timer().CurrentTime();	// seconds

    prev = cur;

    NaReal	q_c = *x;
    NaReal k1 = k10*exp(-E1R/prev.var.T);
    NaReal k2 = k20*exp(-E2R/prev.var.T);
    NaReal Q = prev.var.c_A*V*(k1*(-drH_1) + k2*(-drH_2));

    cur.var.c_A += dt*((c_Av - prev.var.c_A)*q/V - (k1+k2)*prev.var.c_A);
    cur.var.c_B += dt*((c_Bv - prev.var.c_B)*q/V + k1*prev.var.c_A);
    cur.var.c_C += dt*((c_Cv - prev.var.c_C)*q/V + k2*prev.var.c_A);
    cur.var.T += dt*((T_v - prev.var.T)*q/V
		     + (Q - A*k*(prev.var.T-prev.var.T_c))/(V*rho*Cp));
    cur.var.T_c += dt*((T_vc - prev.var.T_c)*q_c/V_c
		       + A*k*(prev.var.T-prev.var.T_c)/(V_c*rho_c*Cp_c));

    if(NULL != out) {
	out->AppendRecord();
	out->SetValue(time, 0);
	out->SetValue(cur.var.c_A, 1);
	out->SetValue(cur.var.c_B, 2);
	out->SetValue(cur.var.c_C, 3);
	out->SetValue(cur.var.T, 4);
	out->SetValue(cur.var.T_c, 5);
	out->SetValue(q_c, 6);
    }

    *y = cur.var.T;
}
