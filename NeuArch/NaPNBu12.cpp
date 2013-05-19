/* NaPNBu12.cpp */
static char rcsid[] = "$Id: NaPNBu12.cpp 418 2010-01-09 19:01:41Z efox $";
//---------------------------------------------------------------------------

#include "NaPNBu12.h"


//---------------------------------------------------------------------------
// Create node for Petri network
NaPNBus1i2o::NaPNBus1i2o (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in"),
  out1(this, "out1"),
  out2(this, "out2"),
  ////////////////////////
  // Default proportion //
  ////////////////////////
  nOutDimProp1(1),
  nOutDimProp2(1)
{
    // Nothing to do
}


//---------------------------------------------------------------------------

///////////////////
// Node specific //
///////////////////

/// Set output connector proportion: n1/n2
/// If n1=n2 then out1=out2[+1]
/// If n1=0 then out2 has dim=n2
/// If n2=0 then out1 has dim=n1
void
NaPNBus1i2o::set_out_dim_proportion (int n1, int n2)
{
    nOutDimProp1 = n1;
    nOutDimProp2 = n2;
}


//---------------------------------------------------------------------------

///////////////////
// Quick linkage //
///////////////////

//---------------------------------------------------------------------------
// Return mainstream input connector (the only input or NULL)
NaPetriConnector*
NaPNBus1i2o::main_input_cn ()
{
    return &in;
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 2. Link connectors inside the node
void
NaPNBus1i2o::relate_connectors ()
{
    if(0 == nOutDimProp1) {
	int nOut1 = in.data().dim() - nOutDimProp2;
	if(nOut1 < 0)
	    nOut1 = 0;
	out1.data().new_dim(nOut1);
	out2.data().new_dim(in.data().dim() - nOut1);
    }
    else if(0 == nOutDimProp2) {
	int nOut2 = in.data().dim() - nOutDimProp1;
	if(nOut2 < 0)
	    nOut2 = 0;
	out2.data().new_dim(nOut2);
	out1.data().new_dim(in.data().dim() - nOut2);
    }
    else {
	double fRatio2 = (double)nOutDimProp2 / (nOutDimProp1 + nOutDimProp2);

	out2.data().new_dim(fRatio2 * in.data().dim());
	out1.data().new_dim(in.data().dim() - out2.data().dim());
    }
}


//---------------------------------------------------------------------------
// 5. Verification to be sure all is OK (true)
bool
NaPNBus1i2o::verify ()
{
    return in.data().dim() == out1.data().dim() + out2.data().dim();
}

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNBus1i2o::action ()
{
    unsigned    i;
    for(i = 0; i < out1.data().dim(); ++i){
        out1.data()[i] = in.data()[i];
    }
    for(i = 0; i < out2.data().dim(); ++i){
        out2.data()[i] = in.data()[out1.data().dim() + i];
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
