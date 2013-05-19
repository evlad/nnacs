/* watertank.cpp */

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define __NaSharedExternFunction
#include "watertank.h"


///-----------------------------------------------------------------------
/// Create local external function
extern "C" NaExternFunc*
NaCreateExternFunc (char* szOptions, NaVector& vInit)
{
  return new NaWaterTankFunc(szOptions, vInit);
}


///-----------------------------------------------------------------------
/// Make empty (y=x) function
NaWaterTankFunc::NaWaterTankFunc ()
    : A(0.0), F(0.0), b(0.0), a(0.0), q(0.0), h0(0.0),
      Afile(NULL), Ffile(NULL), bfile(NULL), afile(NULL), qfile(NULL)
{
}


///-----------------------------------------------------------------------
/// Make function with given options and initial vector
/// options: A F b a
///  -  A  - characteristic of tank base area
///  -  F  - output flow (0 is allowed)
///  -  b  - input flow characteristic
///  -  a  - output pipe characteristic
///  -  q  - number of output tubes or output quota
/// initial: vInit[0] - initial water level (0 by default)
NaWaterTankFunc::NaWaterTankFunc (char* szOptions, NaVector& vInit)
    : A(0.0), F(0.0), b(0.0), a(0.0), q(0.0), h0(0.0),
      Afile(NULL), Ffile(NULL), bfile(NULL), afile(NULL), qfile(NULL)
{
    char	*szToken, *szRest, *szThis = strdup(szOptions);
    NaReal	fTest;

    szToken = strtok(szThis, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	A = fTest;
    else
	Afile = OpenInputDataFile(szToken);

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	F = fTest;
    else
	Ffile = OpenInputDataFile(szToken);

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	b = fTest;
    else
	bfile = OpenInputDataFile(szToken);

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	a = fTest;
    else
	afile = OpenInputDataFile(szToken);

    szToken = strtok(NULL, " ");
    fTest = strtod(szToken, &szRest);
    if(szToken != szRest)
	q = fTest;
    else
	qfile = OpenInputDataFile(szToken);

    free(szThis);

  if(vInit.dim() > 0)
      h0 = vInit[0];
}


///-----------------------------------------------------------------------
/// Destructor
NaWaterTankFunc::~NaWaterTankFunc ()
{
    delete Afile;
    delete Ffile;
    delete bfile;
    delete afile;
    delete qfile;
}


///-----------------------------------------------------------------------
/// Reset operations, that must be done before new modelling
/// session will start.  It's guaranteed that this reset will be
/// called just after Timer().ResetTime().
void
NaWaterTankFunc::Reset ()
{
    NaReal dt = Timer().GetSamplingRate();
    if(Afile)
	Afile->GoStartRecord();
    if(Ffile)
	Ffile->GoStartRecord();
    if(bfile)
	bfile->GoStartRecord();
    if(afile)
	afile->GoStartRecord();
    if(qfile)
	qfile->GoStartRecord();

    h = h0;
    NaPrintLog("watertank: h(t=0)=%g  dt=%g\n", h, dt);
}


///-----------------------------------------------------------------------
/// Compute output on the basis of internal parameters,
/// stored state and external input: y=F(x,t,p)
void
NaWaterTankFunc::Function (NaReal* x, NaReal* y)
{
    NaReal dt = Timer().GetSamplingRate();

    if(NULL == x || NULL == y)
	return;

    if(Afile)
	A = Afile->GetValue();
    if(Ffile)
	F = Ffile->GetValue();
    if(bfile)
	b = bfile->GetValue();
    if(afile)
	a = afile->GetValue();
    if(qfile)
	q = qfile->GetValue();

    if(A <= 0.0 || b <= 0.0 || a <= 0.0 || q < 0.0) {
	NaPrintLog("watertank: zero or negative parameters: A=%g b=%g a=%g q=%gx\n",
		   A, b, a, q);
	y[0] = h;
	return;
    }

    // h'=b*u/A-(a*sqrt(h)+F)/A
    NaReal u = x[0], &h1 = y[0];

    if(h <= 0)
	h1 = 0.0;
    else
	h1 = h + dt * (b*u - q*(a * sqrt(h) + F))/A;

#if 0
    NaPrintLog("watertank: h(t=%g)=%g: u=%g A=%g F=%g b=%g a=%g q=%g => h=%g\n",
	       Timer().CurrentTime(), h, u, A, F, b, a, q, h1);
#endif

    if(Afile)
	Afile->GoNextRecord();
    if(Ffile)
	Ffile->GoNextRecord();
    if(bfile)
	bfile->GoNextRecord();
    if(afile)
	afile->GoNextRecord();
    if(qfile)
	qfile->GoNextRecord();
}
