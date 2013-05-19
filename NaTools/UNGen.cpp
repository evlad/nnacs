
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
// Generator for the noise and set point.
// (C)opyright by Eliseev Vladimir
//---------------------------------------------------------------------------

#include "NaLogFil.h"
#include "NaGenerl.h"
#include "NaExcept.h"

#include "NaConfig.h"
#include "NaLinCon.h"
#include "NaLiNois.h"
#include "NaConLaw.h"
#include "NaSysSch.h"
#include "NaDataIO.h"
#include "NaTimer.h"

//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    char    szUFileName[30], szNFileName[30];
    int     nSamples;

    if(3 != argc && 4 != argc){
        printf("Usage: ungen.exe FileName.cna Samples [Suffix]\n"
               "    Will generate files u[Suffix].dat and n[Suffix].dat\n");
        return 1;
    }

    if(4 == argc){
        strcat(strcat(strcpy(szUFileName, "u"), argv[3]), ".dat");
        strcat(strcat(strcpy(szNFileName, "n"), argv[3]), ".dat");
    }else{
        strcat(strcpy(szUFileName, "u"), ".dat");
        strcat(strcpy(szNFileName, "n"), ".dat");
    }

    printf("Setpoint output to '%s'\n", szUFileName);
    printf("Noise output to '%s'\n", szNFileName);

    nSamples = strtol(argv[2], NULL, 10);

    printf("Number of samples is %d\n", nSamples);

    try{
        NaDataFile      *dfSetPoint = OpenOutputDataFile(szUFileName);
        NaDataFile      *dfNoise = OpenOutputDataFile(szNFileName);

        // Main configuration file
        NaConfigFile    auconffile(";NeuCon config", 1, 1);

        // Common system schema configuration
        NaSystemSchema  sysschema;

        // Applied units
        NaControlLaw        au_setpoint(10);
        NaLinearNoiseGen    au_linnoise(10);

        // List of configurable units
        NaConfigPart    *conf_list[] = {
            &sysschema,
            &au_setpoint,
            &au_linnoise
        };

        // Setup unit names
        au_linnoise.SetInstance("Noise");

        // Add units for configuration from file
        auconffile.AddPartitions(NaNUMBER(conf_list), conf_list);

        // Configure units by loading configuration file
        // Automated Control System Schema
        auconffile.LoadFromFile(argv[1]);

        TheTimer.SetSamplingRate(sysschema.fSampRate);
        TheTimer.ResetTime();

        au_setpoint.Reset();
        au_linnoise.Reset();

        for(TheTimer.ResetTime();
            TheTimer.CurrentIndex() < nSamples;
            TheTimer.GoNextTime()){
            NaReal  fDummy, fSetPoint, fNoise;

            dfSetPoint->AppendRecord();
            au_setpoint.Function(&fDummy, &fSetPoint);
            dfSetPoint->SetValue(fSetPoint);

            dfNoise->AppendRecord();
            au_linnoise.Function(&fDummy, &fNoise);
            dfNoise->SetValue(fNoise);
        }

        delete dfSetPoint;
        delete dfNoise;
    }
    catch(NaException& ex){
        NaPrintLog("EXCEPTION: %s\n", NaExceptionMsg(ex));
    }

    return 0;
}
