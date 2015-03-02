// NaPNSS.h
//-*-C++-*-
//---------------------------------------------------------------------------
#ifndef NaPNSSH
#define NaPNSSH

#include <NaPetri.h>
#include <NaUnit.h>
#include <NaMatrix.h>
#include <NaVector.h>
#include <NaSSModel.h>


//---------------------------------------------------------------------------
// Applied Petri net node: state space model unit.
// May has many inputs and many outputs

//---------------------------------------------------------------------------
class PNNA_API NaPNStateSpace : public NaPetriNode, public NaTimer
{
public:

    // Create node for Petri network
    NaPNStateSpace (const char* szNodeName = "statespace");


    ////////////////
    // Connectors //
    ////////////////

    // Input (mainstream)
    NaPetriCnInput      u;

    // Input time stream
    NaPetriCnInput      time;

    // Output (mainstream)
    NaPetriCnOutput     y;

    // State
    NaPetriCnOutput     x;

    ///////////////////
    // Quick linkage //
    ///////////////////

    // Return mainstream input connector
    virtual NaPetriConnector*   main_input_cn ();

    // Return mainstream output connector
    virtual NaPetriConnector*   main_output_cn ();

    ///////////////////
    // Node specific //
    ///////////////////

    // Assign new state-space model
    virtual void        set_ss_model (NaStateSpaceModel* pSSModel);

    // Setup initial state
    virtual void        set_initial_state (const NaVector& vX0);

    // Set space-state equation matrices A, B, C, D, where:
    //  { x(t+1) = A*x(t) + B*u(t)
    //  { y(t)   = C*x(t) + D*u(t)
    // !!Can be changed asynchronously!!
    virtual void        set_ss_matrices (const NaMatrix& mA,
					 const NaMatrix& mB,
					 const NaMatrix& mC,
					 const NaMatrix& mD);

    // Shorter form to set space-state equation matrices A, B, C,
    // where:
    //  { x(t+1) = A*x(t) + B*u(t)
    //  { y(t)   = C*x(t)
    // !!Can be changed asynchronously!!
    virtual void        set_ss_matrices (const NaMatrix& mA,
					 const NaMatrix& mB,
					 const NaMatrix& mC);

    // The shortest form to set space-state equation matrices A, B,
    // where:
    //  { x(t+1) = A*x(t) + B*u(t)
    //  { y(t)   = x(t)
    // !!Can be changed asynchronously!!
    virtual void        set_ss_matrices (const NaMatrix& mA,
					 const NaMatrix& mB);

    // Get matrices.
    virtual void        get_ss_matrices (NaMatrix& mA,
					 NaMatrix& mB,
					 NaMatrix& mC,
					 NaMatrix& mD) const;

    // Get dimensions.
    virtual void	get_dimensions (unsigned& uInputs,
					unsigned& uOutputs,
					unsigned& uState) const;

    // Get internal state.
    virtual void	get_state (NaVector& vX) const;

    ///////////////////////
    // Phases of network //
    ///////////////////////

    // 2. Link connectors inside the node
    virtual void        relate_connectors ();

    // 5. Verification to be sure all is OK (true)
    virtual bool        verify ();

    // 6. Initialize node activity and setup starter flag if needed
    virtual void        initialize (bool& starter);

    // 8. True action of the node (if activate returned true)
    virtual void        action ();

protected:/* data */

  NaStateSpaceModel	*m_pSSModel;

protected:/* methods */
    
};


//---------------------------------------------------------------------------
#endif
