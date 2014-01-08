/* NaPNKalm.cpp */
//---------------------------------------------------------------------------

#include <math.h>

#include "NaPNSS.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNStateSpace::NaPNStateSpace (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  u(this, "u"),
  x(this, "x"),
  y(this, "y"),
  time(this, "time")
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

// Initial state
void
NaPNStateSpace::set_initial_state (const NaVector& vX0)
{
    if(!(vX0.dim() > 0)){
         throw(na_size_mismatch);
    }

    x0 = vX0;
}


//---------------------------------------------------------------------------
// Set space-state equation matrices A, B, C, D, where:
//  { x(t+1) = A*x(t) + B*u(t)
//  { y(t)   = C*x(t) + D*u(t)
// !!Can be changed asynchronously!!
void
NaPNStateSpace::set_ss_matrices (const NaMatrix& mA,
				 const NaMatrix& mB,
				 const NaMatrix& mC,
				 const NaMatrix& mD)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mC.dim_cols() > 0 && mC.dim_rows() > 0 &&
         mD.dim_cols() > 0 && mD.dim_rows() > 0 &&
         mA.dim_rows() == mA.dim_cols() &&
         mA.dim_rows() == mB.dim_rows() &&
         mA.dim_rows() == mC.dim_cols() &&
         mC.dim_rows() == mD.dim_rows() &&
         mB.dim_cols() == mD.dim_cols())){
	throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C = mC;
    D = mD;
}


//---------------------------------------------------------------------------
// Shorter form to set space-state equation matrices A, B, C,
// where:
//  { x(t+1) = A*x(t) + B*u(t)
//  { y(t)   = C*x(t)
// !!Can be changed asynchronously!!
void
NaPNStateSpace::set_ss_matrices (const NaMatrix& mA,
				 const NaMatrix& mB,
				 const NaMatrix& mC)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mC.dim_cols() > 0 && mC.dim_rows() > 0 &&
         mA.dim_rows() == mA.dim_cols() &&
         mA.dim_rows() == mB.dim_rows() &&
         mA.dim_rows() == mC.dim_cols())){
         throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C = mC;
    D.new_dim(C.dim_rows(), B.dim_cols());
    D.init_zero();
}


//---------------------------------------------------------------------------
// The shortest form to set space-state equation matrices A, B,
// where:
//  { x(t+1) = A*x(t) + B*u(t)
//  { y(t)   = x(t)
// !!Can be changed asynchronously!!
void
NaPNStateSpace::set_ss_matrices (const NaMatrix& mA,
				 const NaMatrix& mB)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mA.dim_rows() == mA.dim_cols() &&
         mA.dim_rows() == mB.dim_rows())){
         throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C.new_dim(A.dim_rows(), A.dim_cols());
    C.init_diag(1.0);
    D.new_dim(C.dim_rows(), B.dim_cols());
    D.init_zero();
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector
NaPetriConnector*
NaPNStateSpace::main_input_cn ()
{
    return &u;
}


//---------------------------------------------------------------------------
// Return mainstream output connector
NaPetriConnector*
NaPNStateSpace::main_output_cn ()
{
    return &y;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

// 2. Link connectors inside the node
void
NaPNStateSpace::relate_connectors ()
{
    // initialize dimensions
    n = A.dim_rows();
    k = B.dim_cols();
    m = C.dim_rows();

    // Assign output dimensions
    y.data().new_dim(m);
    x.data().new_dim(n);
}


// 5. Verification to be sure all is OK (true)
bool
NaPNStateSpace::verify ()
{
    bool    answer = true;

    answer = answer && x.data().dim() == n;
    answer = answer && y.data().dim() == m;
    answer = answer && u.data().dim() == k;
    answer = answer && x0.dim() == n;
    answer = answer && A.dim_rows() == n;
    answer = answer && A.dim_cols() == n;
    answer = answer && C.dim_rows() == m;
    answer = answer && C.dim_cols() == n;
    answer = answer && B.dim_rows() == n;
    answer = answer && B.dim_cols() == k;
    answer = answer && D.dim_rows() == m;
    answer = answer && D.dim_cols() == k;

    return answer;
}


// 6. Initialize node activity and setup starter flag if needed
void
NaPNStateSpace::initialize (bool& starter)
{
    x.data().copy(x0);
}


// 8. True action of the node (if activate returned true)
void
NaPNStateSpace::action ()
{
    //           *************************
    //           * Internal state change *
    //           *************************

    if(bVerbose){
        NaPrintLog("           *************************\n"
                   "           * Internal state change *\n"
                   "           *************************\n");

        NaPrintLog("### x(t): ");
        x.data().print_contents();

        NaPrintLog("### y(t): ");
        y.data().print_contents();
    }

    NaVector    x1(n), x2(n), y1(m);

    // y(t) = C*x(t) + D*u(t)
    C.multiply(x.data(), y.data());     // y = C * x
    D.multiply(u.data(), y1);           // y1 = D * u
    y.data().add(y1);                   // y += y1

    // x(t+1) = A*x(t) + B*u(t)
    A.multiply(x.data(), x1);           // x1 = A * x
    B.multiply(u.data(), x2);           // x2 = B * u
    x.data().copy(x1);                  // x = x1
    x.data().add(x2);                   // x += x2

    if(bVerbose){
        NaPrintLog("### x(t+1): ");
        x.data().print_contents();
    }
}

//---------------------------------------------------------------------------

