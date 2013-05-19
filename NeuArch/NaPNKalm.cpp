/* NaPNKalm.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <math.h>

#include "NaPNKalm.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNKalman::NaPNKalman (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  u(this, "u"),
  v(this, "v"),
  w(this, "w"),
  y_obs(this, "y_obs"),
  x_est(this, "x_est"),
  x_int(this, "x_int")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Set space-state equation matrices A, B, C, where:
//  { x(t+1) = A*x(t) + B*u(t) + v(t)
//  { y(t)   = C*x(t) + w(t)
// !!Can be changed asynchronously!!
void
NaPNKalman::set_ss_equations (const NaMatrix& mA,
                              const NaMatrix& mB,
                              const NaMatrix& mC)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mC.dim_cols() > 0 && mC.dim_rows() > 0 &&
         mA.dim_rows() == mB.dim_rows() &&
         mA.dim_rows() == mC.dim_cols())){
         throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C = mC;
}


//---------------------------------------------------------------------------
// Set predefined noise parameters
//  Ev(t)=0; Ev(t)v'(T)=Q(t)*d_kT
//  Ew(t)=0; Ew(t)w'(T)=R(t)*d_kT
// where d_kT - Kroneker symbol: d_kT=1, k=T & d_kT=0, k!=T
// !!Can be changed asynchronously!!
void
NaPNKalman::set_noise_params (const NaMatrix& mQ,
                              const NaMatrix& mR)
{
    if(!(mQ.dim_cols() > 0 && mQ.dim_rows() > 0 &&
         mR.dim_cols() > 0 && mR.dim_rows() > 0)){
         throw(na_size_mismatch);
    }

    Q = mQ;
    R = mR;
}



//---------------------------------------------------------------------------
// Set initial conditions for state space system (x(0)=0 by default)
void
NaPNKalman::set_ss_init_cond (const NaVector& vx0)
{
    if(!(vx0.dim() > 0)){
         throw(na_size_mismatch);
    }

    x0 = vx0;
}


//---------------------------------------------------------------------------
// Set initial conditions for Kalman filter: P(x(0)) - covariance of x(0)
void
NaPNKalman::set_kalman_init_cond (const NaMatrix& mPx0)
{
    if(!(mPx0.dim_cols() > 0 && mPx0.dim_rows() > 0)){
         throw(na_size_mismatch);
    }

    Px0 = mPx0;
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNKalman::main_input_cn ()
{
    return &u;
}


//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNKalman::main_output_cn ()
{
    return &y_obs;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNKalman::relate_connectors ()
{
    // initialize dimensions
    n = A.dim_rows();
    k = B.dim_cols();
    m = C.dim_rows();

    // Assign output dimensions
    y_obs.data().new_dim(m);
    x_int.data().new_dim(n);
    x_est.data().new_dim(n);
}


//---------------------------------------------------------------------------
// 4. Allocate resources for internal usage
void
NaPNKalman::allocate_resources ()
{
    // Allocate internal buffers
    x.new_dim(n);
    x_extra.new_dim(n);

    P_tilde.new_dim(n, n);
    P_extra.new_dim(n, n);

    if(bVerbose){
        dfK = OpenOutputDataFile("K.dat");
        dfP_tilde = OpenOutputDataFile("P_tilde.dat");
    }else{
        dfK = NULL;
        dfP_tilde = NULL;
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNKalman::verify ()
{
    bool    answer = true;

    answer = answer && x_int.data().dim() == n;
    answer = answer && x_est.data().dim() == n;
    answer = answer && y_obs.data().dim() == m;
    answer = answer && A.dim_rows() == n;
    answer = answer && A.dim_cols() == n;
    answer = answer && C.dim_rows() == m;
    answer = answer && C.dim_cols() == n;
    answer = answer && B.dim_rows() == n;
    answer = answer && B.dim_cols() == k;
    answer = answer && u.data().dim() == k;
    answer = answer && v.data().dim() == n;
    answer = answer && w.data().dim() == m;

    return answer;
}


//---------------------------------------------------------------------------
// 6. Initialize node activity and setup starter flag if needed
void
NaPNKalman::initialize (bool& starter)
{
    x = x0;

    //  *
    // x (0) = 0
    x_extra.init_zero();

    //  *
    // P (0) = P(0)
    P_extra = Px0;
}


//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNKalman::action ()
{
    //           *************************
    //           * Internal state change *
    //           *************************

    if(bVerbose){
        NaPrintLog("           *************************\n"
                   "           * Internal state change *\n"
                   "           *************************\n");

        NaPrintLog("### x(t): ");
        x.print_contents();

        NaPrintLog("### y(t): ");
        y_obs.data().print_contents();
    }

    NaVector    x1(n), x2(n);

    // y(t) = C*x(t) + w(t)
    C.multiply(x, y_obs.data());        // y_obs = C * x
    y_obs.data().add(w.data());         // y_obs += w <=> y_obs = C * x + w

    // x(t+1) = A*x(t) + B*u(t) + v(t)
    A.multiply(x, x1);                  // x1 = A * x
    B.multiply(u.data(), x2);           // x2 = B * u
    x = v.data();                       // x = v
    x.add(x1);                          // x += x1 <=> x = A * x + v
    x.add(x2);                          // x += x2 <=> x = A * x + B * u + v
    x_int.data() = x;                   // x_int = x

    if(bVerbose){
        NaPrintLog("### x(t+1): ");
        x.print_contents();
    }


    //           *************************
    //           *    Kalman's filter    *
    //           *************************

    if(bVerbose){
        NaPrintLog("           *************************\n"
                   "           *    Kalman's filter    *\n"
                   "           *************************\n");

        NaPrintLog("### P_extra: ");
        P_extra.print_contents();
    }

    NaMatrix    At(A);          A.trans(At);
    NaMatrix    Ct(C);          C.trans(Ct);
    NaMatrix    R2(R), Rinv(R.dim_cols(), R.dim_rows());
    NaMatrix    M1(P_tilde.dim_rows(), At.dim_cols());
    NaMatrix    M2(P_extra.dim_rows(), Ct.dim_cols());
    NaMatrix    M3(Ct.dim_rows(), Rinv.dim_cols());
    NaMatrix    M4(C.dim_rows(), P_extra.dim_cols());
    NaMatrix    P_next(n, n);           // P_extra next time (t+1)
    NaVector    x_next(n);              // x_extra next time (t+1)
    NaVector    y_extra(m);

    //_________________________________________________________________
    //         *       T        *       T    -1
    // K(t) = P (t) * C * {C * P (t) * C + R}
    P_extra.multiply(Ct, M2);           // M2 = P_extra * Ct
    C.multiply(M2, R2);                 // R2 = C * M2
    R2.add(R);                          // R2 += R
    R2.inv(Rinv);                       // Rinv = 1/R2
    Ct.multiply(Rinv, M3);              // M3 = Ct * Rinv
    P_extra.multiply(M3, K);            // K = P_extra * M3

    if(bVerbose){
        NaPrintLog("### K: ");
        K.print_contents();
    }

    //_________________________________________________________________
    // ^       *                          *
    // x(t) = x (t) + K(t) * {y(t) - C * x (t)}
    C.multiply(x_extra, y_extra);       // y_extra = C * x_extra
    y_extra.multiply(-1.0);             // y_extra = -y_extra
    y_extra.add(y_obs.data());          // y_extra += y_obs
    K.multiply(y_extra, x_est.data());  // x_est = K * y_extra
    x_est.data().add(x_extra);          // x_est += x_extra

    if(bVerbose){
        NaPrintLog("### x_est: ");
        x_est.data().print_contents();
    }

    //_________________________________________________________________
    //  *            ^
    // x (t+1) = A * x(t)
    A.multiply(x_est.data(), x_next);   // x_next = A * x_est

    if(bVerbose){
        NaPrintLog("### x_next: ");
        x_next.print_contents();
    }

    //_________________________________________________________________
    // ~       *                  *
    // P(t) = P (t) - K(t) * C * P (t)
    C.multiply(P_extra, M4);            // M4 = C * P_extra
    K.multiply(M4, P_tilde);            // P_tilde = K * M4
    P_tilde.multiply(-1.0);             // P_tilde = -P_tilde
    P_tilde.add(P_extra);               // P_tilde += P_extra

    if(bVerbose){
        NaPrintLog("### P_tilde: ");
        P_tilde.print_contents();
    }

    //_________________________________________________________________
    //  *            ~       T
    // P (t+1) = A * P(t) * A + Q
    P_tilde.multiply(At, M1);           // M1 = P_tilde * At
    A.multiply(M1, P_next);             // P_next = A * M1
    P_next.add(Q);                      // P_next += Q

    if(bVerbose){
        NaPrintLog("### P_next: ");
        P_next.print_contents();
    }

    // Make next extrapolated values be current
    x_extra = x_next;
    P_extra = P_next;

    if(dfK || dfP_tilde){
        unsigned	i, j;
        dfK->AppendRecord();

	for(i = 0; i < K.dim_rows(); ++i){
	  for(j = 0; j < K.dim_cols(); ++j){
	    dfK->SetValue(K(i,j), j + i * K.dim_cols());
	  }
	}

        dfP_tilde->AppendRecord();
	for(i = 0; i < P_tilde.dim_rows(); ++i){
	  for(j = 0; j < P_tilde.dim_cols(); ++j){
	    dfP_tilde->SetValue(sqrt(P_tilde(i,j)),
				j + i * P_tilde.dim_cols());
	  }
	}
    }
}


//---------------------------------------------------------------------------
// 10. Deallocate resources and close external data
void
NaPNKalman::release_and_close ()
{
    if(dfK || dfP_tilde){
        delete dfK;
        delete dfP_tilde;
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)

