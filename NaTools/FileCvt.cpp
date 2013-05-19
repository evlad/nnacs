/* FileCvt.cpp */
static char rcsid[] = "$Id$";

#include <NaDataIO.h>

//---------------------------------------------------------------------------
int main(int argc, char **argv)
{
  if(3 != argc)
    {
      fprintf(stderr, "Usage: %s FileIn FileOut\n", argv[0]);
      return 1;
    }

  NaOpenLogFile("FileCvt.log");

  NaDataFile	*pInDF = OpenInputDataFile(argv[1]);

  bool		bNotEmpty;
  int		iVar, nVars;
  char		**sVarName;

  pInDF->GetVarNameList(nVars, sVarName);

  NaDataFile	*pOutDF = OpenOutputDataFile(argv[2], bdtAuto, nVars);

  NaPrintLog("Input file '%s':\n"\
	     " - records:   %d\n"\
	     " - variables: %d\n"\
	     "Variable list:\n",
	     argv[1], pInDF->CountOfRecord(), nVars);

  for(iVar = 0; iVar < nVars; ++iVar){
    NaPrintLog(" - %s\n", sVarName[iVar]);
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
