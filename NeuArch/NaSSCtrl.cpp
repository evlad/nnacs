/* NaSSCtrl.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

#include "NaSSCtrl.h"


//---------------------------------------------------------------------------
// Make default unit with gain 1
NaControllerUnit::NaControllerUnit ()
: NaUnit(1, 1), NaConfigPart("GeneralLinearController"), L(1)
{
    L.init_value(1.);
}

//---------------------------------------------------------------------------
// Make an explicit copy of the unit
NaControllerUnit::NaControllerUnit (const NaControllerUnit& orig)
: NaUnit(orig), NaConfigPart(orig), L(1)
{
    *this = orig;
}

//---------------------------------------------------------------------------
// Assignment
NaControllerUnit&
NaControllerUnit::operator= (const NaControllerUnit& orig)
{
    L = orig.L;

    /*NaUnit::*/Assign(L.dim_cols(), L.dim_rows());

    return *this;
}

//---------------------------------------------------------------------------
// Assign controller matrix
void
NaControllerUnit::SetMatrix (const NaMatrix& mL)
{
    L = mL;

    /*NaUnit::*/Assign(L.dim_cols(), L.dim_rows());
}

//---------------------------------------------------------------------------
// Get controller matrix
void
NaControllerUnit::GetMatrix (NaMatrix& mL) const
{
    mL = L;
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling will start
void
NaControllerUnit::Reset ()
{
    // Dummy
}

//---------------------------------------------------------------------------
// Compute control force from the input e to output u
void
NaControllerUnit::Function (NaReal* e, NaReal* u)
{
    unsigned    i;
    NaVector    U(L.dim_rows());
    NaVector    E(L.dim_cols());    E = e;

    L.multiply(E, U);       // U(t)=L*E(t)

    for(i = 0; i < L.dim_rows(); ++i){
        u[i] = - U[i];      // u(t)=-L*E(t)
    }
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaControllerUnit::Save (NaDataStream& ds)
{
    unsigned     i, j;
    char    *szBuf;

    ds.PutComment(" Controller: u(t) = L*x(t)");
    ds.PutF("K N", "%d %d", L.dim_rows(), L.dim_cols());

    ds.PutComment(" Matrix L[K,N]:");
    szBuf = new char[20*L.dim_rows()];
    for(i = 0; i < L.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < L.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", L[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaControllerUnit::Load (NaDataStream& ds)
{
    char        *szBuf, *s, *p;
    unsigned     i, j, N, K;

    ds.GetF("%d %d", &K, &N);

    L.new_dim(K, N);
    for(i = 0; i < L.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < L.dim_cols(); ++j){
            L[i][j] = strtod(s, &p);
            s = p;
        }
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
