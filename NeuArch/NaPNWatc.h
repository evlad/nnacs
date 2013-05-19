//-*-C++-*-
/* NaPNWatc.h */
//---------------------------------------------------------------------------
#ifndef NaPNWatcH
#define NaPNWatcH

#include <NaPetri.h>


//---------------------------------------------------------------------------
// Applied Petri net node: watch point
// Allows to watch input data as well by overloaded method as external
// function.
//---------------------------------------------------------------------------
class NaPNWatcher : public NaPetriNode
{
public:

  // Create node for Petri network
  NaPNWatcher (const char* szNodeName = "watcher");


  ////////////////
  // Connectors //
  ////////////////

  // Input (mainstream)
  NaPetriCnInput      events;


  ///////////////////
  // Quick linkage //
  ///////////////////

  // Return mainstream input connector (the only input or NULL)
  virtual NaPetriConnector*   main_input_cn ();


  ///////////////////
  // Node specific //
  ///////////////////

  // Prototype of the external function to watch incoming data events.
  // If the function returns \b false then network has to be
  // terminated immediately
  typedef bool (*ExternalFunc)(void* pUserData,
			       const NaVector& rEvents,
			       NaTimer& rTimer);

  // Attach external watcher function; if it returns \b false then
  // network must be stopped
  virtual void        attach_function (ExternalFunc pExtFunc,
				       void* pUserData = NULL);

  // Built-in watcher function.  Called before attached function.
  // Returns \b true to allow call external functions to process
  // data events and \b false otherwise.  Returns \b true by default.
  virtual bool	watcher (const NaVector& rEvents);


  ///////////////////////
  // Phases of network //
  ///////////////////////

  // 8. True action of the node (if activate returned true)
  virtual void        action ();

protected:/* data */

  // External function to call if not equal to NULL
  ExternalFunc	m_pExtFunc;

  // Custom data to pass to external function
  void		*m_pUserData;

};


//---------------------------------------------------------------------------
#endif
