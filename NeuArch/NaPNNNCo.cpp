/* NaPNNNCo.cpp */
//---------------------------------------------------------------------------

#include "NaPNNNCo.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNNNController::NaPNNNController (const char* szNodeName)
  : NaPetriNode(szNodeName),
    split_er("nnc_split_er"),
    delay_e("nnc_delay_e"),
    delay_r("nnc_delay_r"),
    skip_e("nnc_skip_e"),
    skip_r("nnc_skip_r"),
    merge_er("nnc_merge_er"),
    nnunit("nnc_nnunit"),
    ////////////////
    // Connectors //
    ////////////////
    x(split_er.in),
    y(nnunit.y)
{
    // Make e to be main input and r to be the rest
    split_er.set_out_dim_proportion(1, 0);
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNNNController::main_input_cn ()
{
    return &x;
}


//---------------------------------------------------------------------------
// Return mainstream output connector (the only output or NULL)
NaPetriConnector*
NaPNNNController::main_output_cn ()
{
    return &y;
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

//---------------------------------------------------------------------------
// Assign new neural net
void
NaPNNNController::set_nn_unit (NaNNUnit* pNN)
{
    nnunit.set_nn_unit(pNN);

    delay_e.set_delay(pNN->descr.nInputsRepeat, pNN->descr.InputDelays());
    delay_r.set_delay(pNN->descr.nOutputsRepeat, pNN->descr.OutputDelays());

    unsigned nLagE = delay_e.get_max_delay();
    unsigned nLagR = delay_r.get_max_delay();
    if(nLagE < nLagR)
	skip_e.set_skip_number(nLagR - nLagE);
    else
	skip_r.set_skip_number(nLagE - nLagR);
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 0. Called once when the node becomes part of Petri network
void
NaPNNNController::attend_network ()
{
    // Let's link internal nodes
    net()->link(&split_er.out1, &skip_e.in);
    net()->link(&skip_e.out, &delay_e.in);
    net()->link(&split_er.out2, &skip_r.in);
    net()->link(&skip_r.out, &delay_r.in);
    net()->link(&delay_e.dout, &merge_er.in1);
    net()->link(&delay_r.dout, &merge_er.in2);
    net()->link(&merge_er.out, &nnunit.x);
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNNNController::verify ()
{
    return 2 == x.data().dim() && 1 == y.data().dim();
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
