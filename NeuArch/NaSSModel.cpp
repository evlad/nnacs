/* NaSSModel.cpp */
//---------------------------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NaSSModel.h"


//---------------------------------------------------------------------------
// Registrar for the NaStateSpaceModel
NaConfigPart*
NaStateSpaceModel::NaRegStateSpaceModel ()
{
    return new NaStateSpaceModel();
}


//---------------------------------------------------------------------------
// Make empty (A=[0], B=[0], C=[0], D=[0]) state-space model of
// with dimension 1 for all signals.
NaStateSpaceModel::NaStateSpaceModel ()
    : NaUnit(1, 1),
      A(1, 1), B(1, 1), C(1, 1), D(1, 1), x(1), x0(0),
      n(1), m(1), k(1),
      NaConfigPart(NaTYPE_StateSpaceModel)
{
    A.fetch(0,0) = 0.0;
    B.fetch(0,0) = 0.0;
    C.fetch(0,0) = 0.0;
    D.fetch(0,0) = 0.0;
}


//---------------------------------------------------------------------------
// Make an explicit copy of the state-space model
NaStateSpaceModel::NaStateSpaceModel (const NaStateSpaceModel& orig)
    : NaUnit(orig),
      A(orig.A), B(orig.B), C(orig.C), D(orig.D), x(orig.x), x0(orig.x0),
      n(orig.n), m(orig.m), k(orig.k),
      NaConfigPart(orig)
{
    Assign(k, m);
}


//---------------------------------------------------------------------------
// Destructor
NaStateSpaceModel::~NaStateSpaceModel ()
{
}


//---------------------------------------------------------------------------
// Assignment
NaStateSpaceModel&
NaStateSpaceModel::operator= (const NaStateSpaceModel& orig)
{
    orig.GetMatrices(A, B, C, D);
    orig.GetDimensions(k, m, n);
    Assign(k, m);
}


//---------------------------------------------------------------------------
// Print self
void
NaStateSpaceModel::PrintLog () const
{
}


//---------------------------------------------------------------------------
// Print self with indentation
void
NaStateSpaceModel::PrintLog (const char* szIndent) const
{
}


//---------------------------------------------------------------------------
// Is empty (check for special condition of default constructor)
bool
NaStateSpaceModel::Empty () const
{
    return (1 == A.dim_rows() && 1 == A.dim_cols() && 0.0 == A.get(0, 0) &&
	    1 == B.dim_rows() && 1 == B.dim_cols() && 0.0 == B.get(0, 0) &&
	    1 == C.dim_rows() && 1 == C.dim_cols() && 0.0 == C.get(0, 0) &&
	    1 == D.dim_rows() && 1 == D.dim_cols() && 0.0 == D.get(0, 0));
}


//---------------------------------------------------------------------------
// Make empty
void
NaStateSpaceModel::Clean ()
{
    A.new_dim(1, 1);
    B.new_dim(1, 1);
    C.new_dim(1, 1);
    D.new_dim(1, 1);

    n = m = k = 1;
    Assign(k, m);

    A.fetch(0,0) = 0.0;
    B.fetch(0,0) = 0.0;
    C.fetch(0,0) = 0.0;
    D.fetch(0,0) = 0.0;
}


//***********************************************************************
// Custom part
//***********************************************************************

//---------------------------------------------------------------------------
// Set initial state.
void
NaStateSpaceModel::SetInitialState (const NaVector& vX0)
{
    x0 = vX0;
}


//---------------------------------------------------------------------------
// Set zero initial state (zero vector).
void
NaStateSpaceModel::SetZeroInitialState ()
{
    x0.new_dim(0);
}


//---------------------------------------------------------------------------
// Set space-state model with matrices A, B, C, D, where:
//  { x(t+1) = A*x(t) + B*u(t)
//  { y(t)   = C*x(t) + D*u(t)
void
NaStateSpaceModel::SetMatrices (const NaMatrix& mA,
				const NaMatrix& mB,
				const NaMatrix& mC,
				const NaMatrix& mD)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mC.dim_cols() > 0 && mC.dim_rows() > 0 &&
         mD.dim_cols() > 0 && mD.dim_rows() > 0 &&
         mA.dim_rows() == mA.dim_cols() &&
         mA.dim_rows() == mB.dim_rows() &&
         mA.dim_rows() == mC.dim_cols() &&
         mC.dim_rows() == mD.dim_rows() &&
         mB.dim_cols() == mD.dim_cols())){
	throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C = mC;
    D = mD;

    n = A.dim_rows();
    m = C.dim_rows();
    k = B.dim_cols();

    Assign(k, m);
}


//---------------------------------------------------------------------------
// Shorter form to set space-state model matrices A, B, C,
// where:
//  { x(t+1) = A*x(t) + B*u(t)
//  { y(t)   = C*x(t)
void
NaStateSpaceModel::SetMatrices (const NaMatrix& mA,
				const NaMatrix& mB,
				const NaMatrix& mC)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mC.dim_cols() > 0 && mC.dim_rows() > 0 &&
         mA.dim_rows() == mA.dim_cols() &&
         mA.dim_rows() == mB.dim_rows() &&
         mA.dim_rows() == mC.dim_cols())){
         throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C = mC;
    D.new_dim(C.dim_rows(), B.dim_cols());
    D.init_zero();

    n = A.dim_rows();
    m = C.dim_rows();
    k = B.dim_cols();

    Assign(k, m);
}


//---------------------------------------------------------------------------
// The shortest form to set space-state model matrices A, B,
// where:
//  { x(t+1) = A*x(t) + B*u(t)
//  { y(t)   = x(t)
void
NaStateSpaceModel::SetMatrices (const NaMatrix& mA,
				const NaMatrix& mB)
{
    if(!(mA.dim_cols() > 0 && mA.dim_rows() > 0 &&
         mB.dim_cols() > 0 && mB.dim_rows() > 0 &&
         mA.dim_rows() == mA.dim_cols() &&
         mA.dim_rows() == mB.dim_rows())){
         throw(na_size_mismatch);
    }

    A = mA;
    B = mB;
    C.new_dim(A.dim_rows(), A.dim_cols());
    C.init_diag(1.0);
    D.new_dim(C.dim_rows(), B.dim_cols());
    D.init_zero();

    n = A.dim_rows();
    m = C.dim_rows();
    k = B.dim_cols();

    Assign(k, m);
}


//---------------------------------------------------------------------------
// Get matrices.
void
NaStateSpaceModel::GetMatrices (NaMatrix& mA,
				NaMatrix& mB,
				NaMatrix& mC,
				NaMatrix& mD) const
{
    mA = A;
    mB = B;
    mC = C;
    mD = D;
}


//---------------------------------------------------------------------------
// Get dimensions.
void
NaStateSpaceModel::GetDimensions (unsigned& uInputs,
				  unsigned& uOutputs,
				  unsigned& uState) const
{
    uInputs = k;
    uOutputs = m;
    uState = n;
}


//---------------------------------------------------------------------------
// Get internal state.
void
NaStateSpaceModel::GetState (NaVector& vX) const
{
    vX = x;
}


//***********************************************************************
// Unit part
//***********************************************************************

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling
// will start
void
NaStateSpaceModel::Reset ()
{
    x.new_dim(n);
    if(x0.dim() == 0)
	x.init_zero();
    else if(x0.dim() == x.dim())
	x.copy(x0);
    else
	throw(na_size_mismatch);
}


//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(u,t,p)
void
NaStateSpaceModel::Function (NaReal* pU, NaReal* pY)
{
    NaVector	u(k), y(m);
    u = pU;

    NaVector    x1(n), x2(n), y1(m);

    // y(t) = C*x(t) + D*u(t)
    C.multiply(x, y);            // y = C * x
    D.multiply(u, y1);           // y1 = D * u
    y.add(y1);                   // y += y1

    // x(t+1) = A*x(t) + B*u(t)
    A.multiply(x, x1);           // x1 = A * x
    B.multiply(u, x2);           // x2 = B * u
    x.copy(x1);                  // x = x1
    x.add(x2);                   // x += x2

    if(pY != NULL) {
	for(unsigned i = 0; i < y.dim(); ++i)
	    pY[i] = y[i];
    }
}


//***********************************************************************
// Store and retrieve configuration data
//***********************************************************************

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaStateSpaceModel::Save (NaDataStream& ds)
{
    unsigned     i, j;
    char    *szBuf, szComment[100];

    ds.PutComment(" State space discrete time model:");
    ds.PutComment("   x(t+1) = A*x(t) + B*u(t)");
    ds.PutComment("   y(t)   = C*x(t) + D*u(t)");
    ds.PutComment(" Matrices: A(N,N), B(N,K), C(M,N), D(M,K)");
    ds.PutComment("   N - state vector, M - outputs, K - inputs");
    ds.PutF("Dimensions N M K", "%u %u %u", n, m, k);

    sprintf(szComment, " Matrix A(%u,%u), %u numbers:", n, n, n*n);
    ds.PutComment(szComment);
    szBuf = new char[20*A.dim_cols()];
    for(i = 0; i < A.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < A.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", A[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    sprintf(szComment, " Matrix B(%u,%u), %u numbers:", n, k, n*k);
    ds.PutComment(szComment);
    szBuf = new char[20*B.dim_cols()];
    for(i = 0; i < B.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < B.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", B[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    sprintf(szComment, " Matrix C(%u,%u), %u numbers:", m, n, m*n);
    ds.PutComment(szComment);
    szBuf = new char[20*C.dim_cols()];
    for(i = 0; i < C.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < C.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", C[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    sprintf(szComment, " Matrix D(%u,%u), %u numbers:", m, k, m*k);
    ds.PutComment(szComment);
    szBuf = new char[20*D.dim_cols()];
    for(i = 0; i < D.dim_rows(); ++i){
        szBuf[0] = '\0';
        for(j = 0; j < D.dim_cols(); ++j){
            sprintf(szBuf + strlen(szBuf), " %g", D[i][j]);
        }
        ds.PutF(NULL, "%s", szBuf);
    }
    delete[] szBuf;

    // Since x0 may be em[ty due to zero initial conditions, then
    // special algorithm to save it is used.
    ds.PutComment(" Initial state x(0):");
    szBuf = new char[20*n];
    szBuf[0] = '\0';
    for(i = 0; i < n; ++i){
	NaReal fX0i = (i < x0.dim())? x0[i]: 0.0;
	sprintf(szBuf + strlen(szBuf), " %g", fX0i);
    }
    ds.PutF(NULL, "%s", szBuf);
    delete[] szBuf;
}


//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaStateSpaceModel::Load (NaDataStream& ds)
{
    char        *szBuf, *s, *p;
    unsigned     i, j;

    ds.GetF("%u %u %u", &n, &m, &k);
    NaPrintLog("N=%d  M=%d  K=%d\n", n, m, k);

    A.new_dim(n, n);
    for(i = 0; i < A.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < A.dim_cols(); ++j){
            A[i][j] = strtod(s, &p);
            s = p;
        }
    }
    A.print_contents();

    B.new_dim(n, k);
    for(i = 0; i < B.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < B.dim_cols(); ++j){
            B[i][j] = strtod(s, &p);
            s = p;
        }
    }
    B.print_contents();

    C.new_dim(m, n);
    for(i = 0; i < C.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < C.dim_cols(); ++j){
            C[i][j] = strtod(s, &p);
            s = p;
        }
    }
    C.print_contents();

    D.new_dim(m, k);
    for(i = 0; i < D.dim_rows(); ++i){
        szBuf = ds.GetData();
        s = szBuf;
        for(j = 0; j < D.dim_cols(); ++j){
            D[i][j] = strtod(s, &p);
            s = p;
        }
    }
    D.print_contents();

    // Just to have right dimension
    x.new_dim(n);

    x0.new_dim(n);
    szBuf = ds.GetData();
    s = szBuf;
    for(i = 0; i < x0.dim(); ++i){
	x0[i] = strtod(s, &p);
	s = p;
    }
    x0.print_contents();

    Assign(k, m);
}


//---------------------------------------------------------------------------
// Store state-space model to given configuration file
void
NaStateSpaceModel::Save (const char* szFileName)
{
  NaConfigPart	*conf_list[] = { this };
  NaConfigFile	conf_file(";NeuCon StateSpaceModel", 1, 0, ".ssm");
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.SaveToFile(szFileName);
}


//---------------------------------------------------------------------------
// Read state-space model from given configuration file
void
NaStateSpaceModel::Load (const char* szFileName)
{
  NaConfigPart	*conf_list[] = { this };
  NaConfigFile	conf_file(";NeuCon StateSpaceModel", 1, 0, ".ssm");
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.LoadFromFile(szFileName);
}
