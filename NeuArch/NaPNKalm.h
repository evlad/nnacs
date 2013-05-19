//-*-C++-*-
/* NaPNKalm.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNKalmH
#define NaPNKalmH

#include <NaMatrix.h>
#include <NaPetri.h>
#include <NaDataIO.h>


//---------------------------------------------------------------------------
// Applied Petri net node: state-space object and Kalman filter.
// Has three inputs: u, v, w and three outputs: y_obs, x_int, x_est.
//---------------------------------------------------------------------------
class NaPNKalman : public NaPetriNode
{
public:

    // Create node for Petri network
    NaPNKalman (const char* szNodeName = "kalman");


    ////////////////
    // Connectors //
    ////////////////

    // Control force (mainstream)
    NaPetriCnInput      u;

    // Disturbation noise
    NaPetriCnInput      v;

    // Observation noise
    NaPetriCnInput      w;

    // Observation output (mainstream)
    NaPetriCnOutput     y_obs;

    // Internal state (is not viewable usually)
    NaPetriCnOutput     x_int;

    // Estimated state (Kalman filter)
    NaPetriCnOutput     x_est;


    ///////////////////
    // Node specific //
    ///////////////////

    // Set space-state equation matrices A, B, C, where:
    //  { x(t+1) = A*x(t) + B*u(t) + v(t)
    //  { y(t)   = C*x(t) + w(t)
    // !!Can be changed asynchronously!!
    virtual void        set_ss_equations (const NaMatrix& mA,
                                          const NaMatrix& mB,
                                          const NaMatrix& mC);

    // Set predefined noise parameters
    //  Ev(t)=0; Ev(t)v'(T)=Q(t)*d_kT
    //  Ew(t)=0; Ew(t)w'(T)=R(t)*d_kT
    // where d_kT - Kroneker symbol: d_kT=1, k=T & d_kT=0, k!=T
    // !!Can be changed asynchronously!!
    virtual void        set_noise_params (const NaMatrix& mQ,
                                          const NaMatrix& mR);

    // Set initial conditions for state space system (x(0)=0 by default)
    virtual void        set_ss_init_cond (const NaVector& vx0);

    // Set initial conditions for Kalman filter: P(x(0)) - covariance of x(0)
    virtual void        set_kalman_init_cond (const NaMatrix& mPx0);


    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector (the only input or NULL)
    virtual NaPetriConnector*   main_input_cn ();

    // Return mainstream output connector (the only output or NULL)
    virtual NaPetriConnector*   main_output_cn ();


    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 4. Allocate resources for internal usage
    virtual void        allocate_resources ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 6. Initialize node activity and setup starter flag if needed
    virtual void        initialize (bool& starter);

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

    // 10. Deallocate resources and close external data
    virtual void        release_and_close ();

protected:

    unsigned            n, m, k;    // useful dimensions (after phase #2)
    NaMatrix            A;          // state step matrix [n,n]
    NaMatrix            B;          // control input influence [n,k]
    NaMatrix            C;          // state observation [m,n]
    NaMatrix            Q;          // covariance of v(t) [n,n]
    NaMatrix            R;          // covariance of w(t) [m,n]

    NaVector            x;          // state of modeled system [n]
    NaVector            x0;         // initial state of modeled system [n]
    NaVector            x_extra;    // extrapolated x(t) [n]

    NaMatrix            Px0;        // covariance of x(0) [n,n]
    NaMatrix            P_extra;    // extrapolated P(x(t)) [n,n]
    NaMatrix            P_tilde;    // P(x(t)) with tilde [n,n]
    
    NaMatrix            K;          // K(t) - Kalman gain matrix [n,m]

    // For debugging purpose
    NaDataFile          *dfK, *dfP_tilde;
};

//---------------------------------------------------------------------------
#endif
