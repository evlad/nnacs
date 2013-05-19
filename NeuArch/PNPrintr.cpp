/* PNPrintr.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <stdio.h>

#include "NaLogFil.h"
#include "PNPrintr.h"


//---------------------------------------------------------------------------
NaPNPrinter::NaPNPrinter (const char* szNodeName)
: NaPetriNode(szNodeName),
  ////////////////
  // Connectors //
  ////////////////
  in(this, "in")
{
    // nothing more
}


//---------------------------------------------------------------------------

///////////////////////
// Phases of network //
///////////////////////

//---------------------------------------------------------------------------
// 8. True action of the node (if activate returned true)
void
NaPNPrinter::action ()
{
    NaVector    &vect = in.data();
    unsigned    i;

    printf("%s:", name());
    NaPrintLog("%s:", name());
    for(i = 0; i < vect.dim(); ++i){
        printf("\t%g", vect[i]);
        NaPrintLog("\t%g", vect[i]);
    }
    printf("\n");
    NaPrintLog("\n");
}


//---------------------------------------------------------------------------
#pragma package(smart_init)
