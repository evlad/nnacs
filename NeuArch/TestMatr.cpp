/* TestMatr.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#pragma hdrstop
#include <condefs.h>
#include <stdio.h>

#include "NaMatrix.h"
#include "NaLogFil.h"

//---------------------------------------------------------------------------
USELIB("NeuArch.lib");
USELIB("..\Matrix.041\Matrix.lib");
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char **argv)
{
    NaMatrix    A(10, 10);
    NaVector    B(10);

    NaOpenLogFile("TestMatr.log");

    FILE        *fp = fopen("TestMatr.dat", "r");
    unsigned    i, j, n = 10;

#if 0
    n = 2;
    A.new_dim(n, n);
    for(i = 0; i < n; ++i)
        for(j = 0; j < n; ++j)
            A[i][j] = (i % (1 + j))? 3: -2;
#else
    for(i = 0; i < n; ++i)
        for(j = 0; j < n; ++j)
            fscanf(fp, "%lg", &(A[i][j]));

    for(i = 0; i < n; ++i)
        fscanf(fp, "%lg", &(B[i]));
#endif

    NaPrintLog("Matrix A\n");
    A.print_contents();

    NaPrintLog("Det(A) = %g\n", A.det());

    NaMatrix    InvA(A);
    A.inv(InvA);

    NaPrintLog("Inv A\n");
    InvA.print_contents();

    NaMatrix    Unit(A);

    A.multiply(InvA, Unit);
    NaPrintLog("A x Inv A\n");
    Unit.print_contents();

    fclose(fp);

    return 0;
}
