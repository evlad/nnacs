/* NaSSPlan.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

#include "NaSSPlan.h"


//---------------------------------------------------------------------------
// Make default unit: 1x1 with gain 1 and without observation noise
NaDStateSpaceUnit::NaDStateSpaceUnit ()
: NaUnit(1, 1), NaConfigPart("DiscreteStateSpaceModel"),
  X(1), A(1,1), B(1,1), C(1,1), W(NULL), V(NULL), nW(0), nV(0)
{
    A.init_value(1.);
    B.init_value(1.);
    C.init_value(1.);
    X.init_value(0.);

    N = M = K = 1;
}

//---------------------------------------------------------------------------
// Make an explicit copy of the unit
NaDStateSpaceUnit::NaDStateSpaceUnit (const NaDStateSpaceUnit& orig)
: NaUnit(orig), NaConfigPart(orig),
  X(1), A(1,1), B(1,1), C(1,1), W(NULL), V(NULL), nW(0), nV(0)
{
    *this = orig;
}

//---------------------------------------------------------------------------
// Destroy unit
NaDStateSpaceUnit::~NaDStateSpaceUnit ()
{
    unsigned     i;

    // Remove disturbance noise
    if(NULL != W){
        for(i = 0; i < nW; ++i){
            delete W[i];
        }
        delete[] W;
        W = NULL;
        nW = 0;
    }

    // Remove observation noise
    if(NULL != V){
        for(i = 0; i < nV; ++i){
            delete V[i];
        }
        delete[] V;
        V = NULL;
        nV = 0;
    }
}

//---------------------------------------------------------------------------
// Assignment
NaDStateSpaceUnit&
NaDStateSpaceUnit::operator= (const NaDStateSpaceUnit& orig)
{
    SetModel(orig.A, orig.B, orig.C);
    SetState(orig.X);

    N = orig.N;
    M = orig.M;
    K = orig.K;

    /*NaUnit::*/Assign(K, M);

    unsigned     i;
    
    // Copy generators
    if(NULL != orig.W){
        nW = N;
        W = new NaRandomSequence*[N];
        for(i = 0; i < N; ++i){
            W[i] = new NaRandomSequence(*(orig.W[i]));
        }
    }
    if(NULL != orig.V){
        nV = M;
        V = new NaRandomSequence*[M];
        for(i = 0; i < M; ++i){
            V[i] = new NaRandomSequence(*(orig.V[i]));
        }
    }

    return *this;
}

//---------------------------------------------------------------------------
// Setup initial or intermediate state
void
NaDStateSpaceUnit::SetState (const NaVector& vX)
{
    X = vX;
}

//---------------------------------------------------------------------------
// Get initial or intermediate state
void
NaDStateSpaceUnit::GetState (NaVector& vX) const
{
    vX = X;
}

//---------------------------------------------------------------------------
// Assign system matrices and remove noise
void
NaDStateSpaceUnit::SetModel (const NaMatrix& mA,
                             const NaMatrix& mB,
                             const NaMatrix& mC)
{
    if(mA.dim_cols() != mA.dim_rows() ||
       mA.dim_rows() != mB.dim_rows() ||
       mC.dim_cols() != mC.dim_cols()){
       throw(na_size_mismatch);
    }

    unsigned     i;

    // Remove disturbance noise
    if(NULL != W){
        for(i = 0; i < N; ++i){
            delete W[i];
        }
        delete[] W;
        W = NULL;
    }

    // Remove observation noise
    if(NULL != V){
        for(i = 0; i < M; ++i){
            delete V[i];
        }
        delete[] V;
        V = NULL;
    }

    A = mA;
    B = mB;
    C = mC;

    N = A.dim_rows();
    K = B.dim_cols();
    M = C.dim_rows();

    /*NaUnit::*/Assign(K, M);
}

//---------------------------------------------------------------------------
// Get system matrices
void
NaDStateSpaceUnit::GetModel (NaMatrix& mA,
                             NaMatrix& mB,
                             NaMatrix& mC) const
{
    mA = A;
    mB = B;
    mC = C;
}

//---------------------------------------------------------------------------
// Assign noise parameters: mean, stddev
void
NaDStateSpaceUnit::SetDisturbNoise (const NaVector& Wmean,
                                    const NaVector& Wstddev)
{
    unsigned     i;

    if(Wmean.dim() != N || Wstddev.dim() != N){
        throw(na_size_mismatch);
    }

    // Remove disturbance noise
    if(NULL != W){
        for(i = 0; i < nW; ++i){
            delete W[i];
        }
        delete[] W;
        W = NULL;
        nW = 0;
    }

    // Setup new random generators
    nW = N;
    W = new NaRandomSequence*[N];
    for(i = 0; i < N; ++i){
        W[i] = new NaRandomSequence();
        W[i]->SetDistribution(rdGaussNormal);
        W[i]->SetGaussianParams(Wmean(i), Wstddev(i));
    }
}

//---------------------------------------------------------------------------
// Assign noise parameters: mean, stddev
void
NaDStateSpaceUnit::SetObservNoise (const NaVector& Vmean,
                                   const NaVector& Vstddev)
{
    unsigned     i;

    if(Vmean.dim() != M || Vstddev.dim() != M){
        throw(na_size_mismatch);
    }

    // Remove observation noise
    if(NULL != V){
        for(i = 0; i < nV; ++i){
            delete V[i];
        }
        delete[] V;
        V = NULL;
        nV = 0;
    }

    // Setup new random generators
    nV = M;
    V = new NaRandomSequence*[M];
    for(i = 0; i < M; ++i){
        V[i] = new NaRandomSequence();
        V[i]->SetDistribution(rdGaussNormal);
        V[i]->SetGaussianParams(Vmean(i), Vstddev(i));
    }
}


//---------------------------------------------------------------------------
// Get noise parameters: mean, stddev
void
NaDStateSpaceUnit::GetDisturbNoise (NaVector& Wmean,
                                    NaVector& Wstddev) const
{
    if(NULL == W){
        Wmean.new_dim(0);
        Wstddev.new_dim(0);
    }else{
        Wmean.new_dim(N);
        Wstddev.new_dim(N);
        unsigned     i;
        for(i = 0; i < N; ++i){
            /* For gaussian parameters! */
            W[i]->GetGaussianParams(Wmean[i], Wstddev[i]);
        }
    }
}

//---------------------------------------------------------------------------
// Get noise parameters: mean, stddev
void
NaDStateSpaceUnit::GetObservNoise (NaVector& Vmean,
                                   NaVector& Vstddev) const
{
    if(NULL == V){
        Vmean.new_dim(0);
        Vstddev.new_dim(0);
    }else{
        Vmean.new_dim(M);
        Vstddev.new_dim(M);
        unsigned     i;
        for(i = 0; i < M; ++i){
            /* For gaussian parameters! */
            V[i]->GetGaussianParams(Vmean[i], Vstddev[i]);
        }
    }
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling will start
void
NaDStateSpaceUnit::Reset ()
{
    unsigned     i;

    if(NULL != W){
        for(i = 0; i < N; ++i){
            W[i]->Reset();
        }
    }
    if(NULL != V){
        for(i = 0; i < M; ++i){
            V[i]->Reset();
        }
    }
}

//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaDStateSpaceUnit::Function (NaReal* u, NaReal* y)
{
    NaVector    U(K); U = u;
    NaVector    Y(M), X1(N), X2(N);

    A.multiply(X, X1);      // X(t+1)=A*X(t)
    B.multiply(U, X2);      // B*U(t)
    X1.add(X2);             // X(t+1)+=B*U(t)

    C.multiply(X, Y);       // Y(t)=C*X(t)

    // Add disturbance and observation noises
    unsigned     i;
    NaReal  fNoise;
    if(NULL != W){
        for(i = 0; i < N; ++i){
            W[i]->Function(NULL, &fNoise);
            X1[i] += fNoise;
        }
    }
    if(NULL != V){
        for(i = 0; i < M; ++i){
            V[i]->Function(NULL, &fNoise);
            Y[i] += fNoise;
        }
    }

    // Get observation
    for(i = 0; i < M; ++i){
        y[i] = Y[i];
    }

    // X(t+1) -> X(t)
    X = X1;
}

//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaDStateSpaceUnit::Save (NaDataStream& ds)
{
    unsigned     i, j;
    char    *szBuf;

    ds.PutComment(" State space discrete model:");
    ds.PutComment(" x(t+1) = A*x(t) + B*u(t) + w(t)");
    ds.PutComment(" y(t)   = C*x(t) + v(t)");
    ds.PutF("N M K", "%d %d %d", N, M, K);

    ds.PutComment(" Matrix A:");
    szBuf = new char[20*A.dim_rows()];
    for(i = 0; i < A.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < A.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", A[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    ds.PutComment(" Matrix B:");
    szBuf = new char[20*B.dim_rows()];
    for(i = 0; i < B.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < B.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", B[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    ds.PutComment(" Matrix C:");
    szBuf = new char[20*C.dim_rows()];
    for(i = 0; i < C.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < C.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", C[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    ds.PutComment(" Initial state x(0):");
    for(i = 0; i < X.dim(); ++i){
        ds.PutF(NULL, "%g", X[i]);
    }

    ds.PutF("Disturbance noise w(t) (mean, stddev)", "%d", NULL != W);
    if(NULL != W){
        for(i = 0; i < nW; ++i){
            NaReal  fMean, fStdDev;
            W[i]->GetGaussianParams(fMean, fStdDev);
            ds.PutF(NULL, "%g %g", fMean, fStdDev);
        }
    }

    ds.PutF("Observation noise v(t) (mean, stddev)", "%d", NULL != V);
    if(NULL != V){
        for(i = 0; i < nV; ++i){
            NaReal  fMean, fStdDev;
            V[i]->GetGaussianParams(fMean, fStdDev);
            ds.PutF(NULL, "%g %g", fMean, fStdDev);
        }
    }
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaDStateSpaceUnit::Load (NaDataStream& ds)
{
    char        *szBuf, *s, *p;
    unsigned     i, j;

    ds.GetF("%d %d %d", &N, &M, &K);
    NaPrintLog("N=%d  M=%d  K=%d\n", N, M, K);

    A.new_dim(N, N);
    for(i = 0; i < A.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < A.dim_cols(); ++j){
            A[i][j] = strtod(s, &p);
            s = p;
        }
    }
    A.print_contents();

    B.new_dim(N, K);
    for(i = 0; i < B.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < B.dim_cols(); ++j){
            B[i][j] = strtod(s, &p);
            s = p;
        }
    }
    B.print_contents();

    C.new_dim(M, N);
    for(i = 0; i < C.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < C.dim_cols(); ++j){
            C[i][j] = strtod(s, &p);
            s = p;
        }
    }
    C.print_contents();

    X.new_dim(N);
    for(i = 0; i < X.dim(); ++i){
        NaReal  fVal;
        ds.GetF("%lg", &fVal);
        X[i] = fVal;
        //NaPrintLog("%d --> %g\n", i, X[i]);
    }
    X.print_contents();

    int     bFlag;
    NaReal  fMean, fStdDev;

    // Remove disturbance noise
    if(NULL != W){
        for(i = 0; i < nW; ++i){
            delete W[i];
        }
        delete[] W;
        W = NULL;
        nW = 0;
    }
    // Load new disturbance noise parameters
    ds.GetF("%d", &bFlag);
    NaPrintLog("Do load disturbance noise parameters: %d\n", bFlag);
    if(bFlag){
        nW = N;
        W = new NaRandomSequence*[N];
        for(i = 0; i < N; ++i){
            ds.GetF("%lg %lg", &fMean, &fStdDev);
            NaPrintLog("mean=%g stddev=%g\n", fMean, fStdDev);
            W[i] = new NaRandomSequence();
            W[i]->SetDistribution(rdGaussNormal);
            W[i]->SetGaussianParams(fMean, fStdDev);
        }
    }

    // Remove observation noise
    if(NULL != V){
        for(i = 0; i < M; ++i){
            delete V[i];
        }
        delete[] V;
        V = NULL;
        nV = 0;
    }
    // Load new observation noise parameters
    ds.GetF("%d", &bFlag);
    NaPrintLog("Do load observation noise parameters: %d\n", bFlag);
    if(bFlag){
        nV = M;
        V = new NaRandomSequence*[M];
        for(i = 0; i < M; ++i){
            ds.GetF("%lg %lg", &fMean, &fStdDev);
            NaPrintLog("mean=%g stddev=%g\n", fMean, fStdDev);
            V[i] = new NaRandomSequence();
            V[i]->SetDistribution(rdGaussNormal);
            V[i]->SetGaussianParams(fMean, fStdDev);
        }
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
