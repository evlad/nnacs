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

// Assign new state-space model
void
NaPNStateSpace::set_ss_model (NaStateSpaceModel* pSSModel)
{
  m_pSSModel = pSSModel;
}


// Setup initial state
void
NaPNStateSpace::set_initial_state (const NaVector& vX0)
{
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }
  if(!(vX0.dim() > 0)){
    throw(na_size_mismatch);
  }

  m_pSSModel->SetInitialState(vX0);
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
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  m_pSSModel->SetMatrices(mA, mB, mC, mD);
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
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  m_pSSModel->SetMatrices(mA, mB, mC);
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
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  m_pSSModel->SetMatrices(mA, mB);
}


//---------------------------------------------------------------------------
// Get matrices.
void
NaPNStateSpace::get_ss_matrices (NaMatrix& mA,
				 NaMatrix& mB,
				 NaMatrix& mC,
				 NaMatrix& mD) const
{
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  m_pSSModel->GetMatrices(mA, mB, mC, mD);
}


//---------------------------------------------------------------------------
// Get dimensions.
void
NaPNStateSpace::get_dimensions (unsigned& uInputs,
				unsigned& uOutputs,
				unsigned& uState) const
{
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  m_pSSModel->GetDimensions(uInputs, uOutputs, uState);
}


//---------------------------------------------------------------------------
// Get internal state.
void
NaPNStateSpace::get_state (NaVector& vX) const
{
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  m_pSSModel->GetState(vX);
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
  if(NULL == m_pSSModel){
    throw(na_null_pointer);
  }

  // Assign output dimensions
  y.data().new_dim(m_pSSModel->C.dim_rows());
  x.data().new_dim(m_pSSModel->A.dim_rows());
}


// 5. Verification to be sure all is OK (true)
bool
NaPNStateSpace::verify ()
{
    bool    answer = true;
    unsigned n, m, k;

    if(NULL == m_pSSModel){
      throw(na_null_pointer);
    }

    m_pSSModel->GetDimensions(k, m, n);

    answer = answer && x.data().dim() == n;
    answer = answer && y.data().dim() == m;
    answer = answer && u.data().dim() == k;

    return answer;
}


// 6. Initialize node activity and setup starter flag if needed
void
NaPNStateSpace::initialize (bool& starter)
{
    if(NULL == m_pSSModel){
      throw(na_null_pointer);
    }

    m_pSSModel->Reset();
    m_pSSModel->GetState(x.data());
}


// 8. True action of the node (if activate returned true)
void
NaPNStateSpace::action ()
{
    if(NULL == m_pSSModel){
      throw(na_null_pointer);
    }

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

    m_pSSModel->Function(&u.data()[0], &y.data()[0]);
    m_pSSModel->GetState(x.data());

    if(bVerbose){
        NaPrintLog("### x(t+1): ");
        x.data().print_contents();
    }
}

//---------------------------------------------------------------------------

