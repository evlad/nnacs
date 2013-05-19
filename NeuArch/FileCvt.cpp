/* FileCvt.cpp */
static char rcsid[] = "$Id$";

#pragma hdrstop
#include <condefs.h>

#include <NaDataIO.h>

//---------------------------------------------------------------------------
#ifdef BORLANDC
USELIB("NeuArch.lib");
USELIB("..\StaDev\stadev32.lib");
#endif

//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    NaDataFile  *pInDF = OpenInputDataFile(argv[1]);
    NaDataFile  *pOutDF = OpenOutputDataFile(argv[2]);
    bool        bNotEmpty;
    int         iVar, nVars;
    char        **sVarName;

    pInDF->GetVarNameList(nVars, sVarName);

    printf("Input file '%s':\n"\
           " - records:   %d\n"\
           " - variables: %d\n"\
           "Variable list:\n",
           argv[1], pInDF->CountOfRecord(), nVars);

    for(iVar = 0; iVar < nVars; ++iVar){
        printf(" - %s\n", sVarName[iVar]);
        pOutDF->SetVarName(iVar, sVarName[iVar]);
    }

    for(bNotEmpty = pInDF->GoStartRecord();
        bNotEmpty;
        bNotEmpty = pInDF->GoNextRecord()){

        pOutDF->AppendRecord();
        for(iVar = 0; iVar < nVars; ++iVar){
            pOutDF->SetValue(pInDF->GetValue(iVar), iVar);
        }
    }

    delete pInDF;
    delete pOutDF;

    return 0;
}
