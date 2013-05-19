/* cstr.h */
#ifndef __cstr_h
#define __cstr_h

#include <NaExFunc.h>
#include <NaDataIO.h>
#include <NaParams.h>

//---------------------------------------------------------------------------
// Class for external function which implements continuous stirred
// tank rector (CSTR) plant.
// Two parallel first-order irreversible chemical reactions:
//
//    k1         k2
// A ----> B  A ----> B
//

class NaCSTRFunc : public NaExternFunc
{
public:

    /// Make empty (y=x) function
    NaCSTRFunc ();

    /// Make function with given options and initial vector
    /// options: cstrpar.def [state.dat]
    /// where
    /// - cstrpar.def has NaParams format (do not use .par extension
    ///   due to switching to new log file!)
    /// - state is defined as seven variables: time, c_A, c_B, c_C, T, T_c, q_c
    /// initial: c_A, c_B, c_C, T, T_c (at time=0)
    NaCSTRFunc (char* szOptions, NaVector& vInit);

    /// Destructor
    virtual ~NaCSTRFunc ();

    /// Reset operations, that must be done before new modelling
    /// session will start.  It's guaranteed that this reset will be
    /// called just after Timer().ResetTime().
    virtual void	Reset ();

    // ATTENTION!  It's guaranteed that this->Function() will be called
    // only once for each time step.  Index of the time step and current
    // time can be got from Timer().

    /// Compute output on the basis of internal parameters,
    /// stored state and external input: y=F(x,t,p)
    virtual void	Function (NaReal* x, NaReal* y);

protected:

    /// State (current and initial)
    union {
	NaReal	state[5];
	struct {
	    NaReal c_A;
	    NaReal c_B;
	    NaReal c_C;
	    NaReal T;
	    NaReal T_c;
	}	var;
    }	cur, prev, ini;

    /// Parameters
    NaParams   *par;

    /// Output state with time (min) [OPTIONAL]
    NaDataFile *out;

    /// Parameters
    // Main symbols:
    //  c   - concentrations
    //  T   - temperatures
    //  V   - volumes
    //  rho - densities
    //  Cp  - specific heat capacities
    //  drH - reaction enthalpies
    // Subscripts:
    //  c   - coolant
    //  r   - reacting mixture
    //  s   - steady state value in main operating point

    NaReal q;       // = 0.015
    NaReal V;       // = 0.23
    NaReal V_c;     // = 0.21
    NaReal rho;     // = 1020
    NaReal rho_c;   // = 998
    NaReal Cp;      // = 4.02
    NaReal Cp_c;    // = 4.182
    NaReal A;       // = 1.51
    NaReal k;       // = 42.8
    NaReal k10;     // = 1.55e11
    NaReal k20;     // = 4.55e25
    NaReal drH_1;   // = -8.6e4
    NaReal drH_2;   // -1.82e4
    NaReal c_Av;    // = 4.22
    NaReal c_Bv;    // = 0
    NaReal c_Cv;    // = 0
    NaReal T_v;     // = 328
    NaReal T_vc;    // = 298
    NaReal q_sc;    // = 0.004
    NaReal T_s;     // = 363.61
    NaReal T_sc;    // = 350.15
    NaReal c_sA;    // = 0.4915
    NaReal c_sB;    // = 2.0042
    NaReal c_sC;    // = 1.7243
    NaReal E1R;     // = 9850
    NaReal E2R;     // = 22019

    
};


#endif /* cstr.h */
