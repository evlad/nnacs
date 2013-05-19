/* TestIO.cpp */
static char rcsid[] = "$Id$";

#pragma hdrstop
#include <condefs.h>

#include <NaDataIO.h>

//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    NaDataFile  *pInDF = OpenInputDataFile(argv[1]);
    NaDataFile  *pOutDF = OpenOutputDataFile(argv[2]);
    return 0;
}
