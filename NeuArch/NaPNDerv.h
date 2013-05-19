//-*-C++-*-
/* NaPNDerv.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaPNDervH
#define NaPNDervH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: discrete derivative computation: y(k)=(1-1/z)*x(k)
// Has one input and one output

//---------------------------------------------------------------------------
class NaPNDerivative : public NaPetriNode
{
public:

  // Create node for Petri network
  NaPNDerivative (const char* szNodeName = "derivative");


  ////////////////
  // Connectors //
  ////////////////

  // Input (mainstream)
  NaPetriCnInput	x;

  // Output (mainstream)
  NaPetriCnOutput	dx;


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 2. Link connectors inside the node
  virtual void		relate_connectors ();

  // 4. Allocate resources for internal usage
  virtual void		allocate_resources ();

  // 5. Verification to be sure all is OK (true)
  virtual bool		verify ();

  // 8. True action of the node (if activate returned true)
  virtual void		action ();

protected:/* data */

  // Vector of previous input
  NaVector		vPrev;
    
};


//---------------------------------------------------------------------------
#endif
