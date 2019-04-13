/* NaTrFunc.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NaTrFunc.h"

//#define TransFunc_DEBUG

//===========================================================================
//
// Polynomial fraction
//
//===========================================================================

//---------------------------------------------------------------------------
// Empty fraction
NaPolyFrac::NaPolyFrac ()
: NaConfigPart("PolynomialFraction")
{
    // dummy
}

//---------------------------------------------------------------------------
// Fraction A(s)/B(s)
NaPolyFrac::NaPolyFrac (const NaVector& A, const NaVector& B)
: NaConfigPart("PolynomialFraction"), num(A), den(B)
{
    // dummy
}

//---------------------------------------------------------------------------
// Copy of fraction
NaPolyFrac::NaPolyFrac (const NaPolyFrac& pf)
: NaConfigPart(pf), num(pf.num), den(pf.den),
  xprev(pf.xprev), yprev(pf.yprev)
{
    // dummy
}

//---------------------------------------------------------------------------
// Assignment
NaPolyFrac&
NaPolyFrac::operator= (const NaPolyFrac& orig)
{
    num = orig.num;
    den = orig.den;
    xprev = orig.xprev;
    yprev = orig.yprev;

    return *this;
}

//---------------------------------------------------------------------------
// Print self
void
NaPolyFrac::PrintLog () const
{
    unsigned    i;
    NaPrintLog("(");
    for(i = 0; i < num.dim(); ++i){
        if(i > 0){
            NaPrintLog("+");
        }
        NaPrintLog("%g[%d]", num(i), num.dim() - i - 1);
    }
    NaPrintLog(")/(");
    for(i = 0; i < den.dim(); ++i){
        if(i > 0){
            NaPrintLog("+");
        }
        NaPrintLog("%g[%d]", den(i), den.dim() - i - 1);
    }
    NaPrintLog(")\n");
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling will start
void
NaPolyFrac::Reset ()
{
    xprev.new_dim(den.dim() - 1);
    xprev.init_zero();
    yprev.new_dim(den.dim() - 1);
    yprev.init_zero();

    if(xprev.dim() > yprev.dim()){
        NaPrintLog("Problem with physical reason\n");
        throw(na_size_mismatch);
    }
}

//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaPolyFrac::Function (NaReal* x, NaReal* y)
{
    unsigned    i, k;
    NaReal      ib0 = 1./den(0);

#ifdef TransFunc_DEBUG
    yprev.print_contents();
    xprev.print_contents();
#endif // TransFunc_DEBUG

    // compute autoregressive part
    NaReal  ar = 0;
    for(i = 0; i < yprev.dim(); ++i){
#ifdef TransFunc_DEBUG
        NaPrintLog("AR(%d): b(%d){%g} * y(-%d){%g} = %g\n",
                   i, i+1, ib0*den(i+1), i+1, yprev(i),
                   ib0 * den(i+1) * yprev(i));
#endif // TransFunc_DEBUG
        ar += ib0 * den(i+1) * yprev(i);
    }
#ifdef TransFunc_DEBUG
    NaPrintLog("AR=%g\n", ar);
#endif // TransFunc_DEBUG

    // compute moving average part
    NaReal  ma = 0;
    k = den.dim() - num.dim();
    for(i = 0; i < num.dim(); ++i){
        if(0 == k && 0 == i){
	     ma += ib0 * num(i) * *x;
#ifdef TransFunc_DEBUG
	     NaPrintLog("MA(%d): a(%d){%g} * x(current){%g} = %g\n",
			i, i, ib0 * num(i), *x, ib0 * num(i) * *x);
#endif // TransFunc_DEBUG
        }else{
	     ma += ib0 * num(i) * xprev(i+k-1);
#ifdef TransFunc_DEBUG
	     NaPrintLog("MA(%d): a(%d){%g} * x(prev=%d){%g} = %g\n",
			i, i, ib0*num(i), i+k-1, ib0 * xprev(i+k-1),
			ib0 * num(i) * xprev(i+k-1));
#endif // TransFunc_DEBUG
        }
    }
#ifdef TransFunc_DEBUG
    NaPrintLog("MA=%g\n", ma);
#endif // TransFunc_DEBUG
    *y = ma - ar;

    // store current values
    if(xprev.dim() > 0)
      {
	xprev.shift(1);
        xprev[0] = *x;
      }
    if(yprev.dim() > 0)
      {
	yprev.shift(1);
	yprev[0] = *y;
      }
}

//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaPolyFrac::Save (NaDataStream& ds)
{
    unsigned    i;
    char        szBuf[MaxConfigFileLine+1] = "";

    // numerator
    for(i = 0; i < num.dim(); ++i){
        sprintf(szBuf + strlen(szBuf), " %g", num(i));
    }

    // delimiter
    strcat(szBuf, " / ");

    // denumerator
    for(i = 0; i < den.dim(); ++i){
        sprintf(szBuf + strlen(szBuf), " %g", den(i));
    }

    ds.PutF("num/den", "%s", szBuf);
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaPolyFrac::Load (NaDataStream& ds)
{
    unsigned    i;
    char        *szBuf = ds.GetData();
    char        *szNext, *szLine = szBuf;

    num.new_dim(0);
    den.new_dim(0);

    for(i = 0; ; ++i){
        // skip leading spaces
        while(isspace(*szBuf) && *szBuf != '\0')
            ++szBuf;

        // while current string is not a slash and is not EOS...
        if('\0' == *szBuf || '/' == *szBuf || ';' == *szBuf)
            break;

        // read numerator
        num.new_dim(i + 1);
        num[i] = strtod(szBuf, &szNext);

        if(szNext == szBuf){
            // error while reading
            NaPrintLog("Error while parsing numerator '%s'\n", szBuf);
            return;
        }
        szBuf = szNext;
    }

    if('\0' == *szBuf || ';' == *szBuf){
        // error while reading
        NaPrintLog("Error while reading '%s'\n", szLine);
        return;
    }
    // else '/' == *szBuf
    // skip '/'
    ++szBuf;

    for(i = 0; ; ++i){
        // skip leading spaces
        while(isspace(*szBuf) && *szBuf != '\0')
            ++szBuf;

        // while current string is not a slash and is not EOS...
        if('\0' == *szBuf || ';' == *szBuf)
            break;

        // read denumerator
        den.new_dim(i + 1);
        den[i] = strtod(szBuf, &szNext);

        if(szNext == szBuf){
            // error while reading
            NaPrintLog("Error while parsing denumerator '%s'\n", szBuf);
            return;
        }
        szBuf = szNext;
    }
}


//===========================================================================
//
// Transfer function
//
//===========================================================================

//-----------------------------------------------------------------------
// Registrar for the NaTransFunc
NaConfigPart*
NaTransFunc::NaRegTransFunc ()
{
  return new NaTransFunc();
}


//---------------------------------------------------------------------------
// Make empty (K=1) transfer function
NaTransFunc::NaTransFunc ()
: NaConfigPart(NaTYPE_TransFunc), ut(utPolyFrac)
{
  pf.num.new_dim(1);
  pf.num[0] = 1.0;
  pf.den.new_dim(1);
  pf.den[0] = 1.0;
}

//---------------------------------------------------------------------------
// Make A(s)/B(s) polynomial fraction
// NaTransFunc (int nA, NaReal A0, A1, ..., An,
//              int mB, NaReal B0, B1, ..., Bm);
NaTransFunc::NaTransFunc (int nA, ...)
: NaConfigPart(NaTYPE_TransFunc), ut(utPolyFrac)
{
    unsigned    i;
    va_list ap;

    // Numerator's high power
    pf.num.new_dim(nA + 1);

    va_start(ap, nA);

    // Numerator coefficients
    for(i = 0; i < pf.num.dim(); ++i)
        pf.num[i] = va_arg(ap, NaReal);

    // Denumerator's high power
    pf.den.new_dim(1 + va_arg(ap, int));

    // Denumerator coefficients
    for(i = 0; i < pf.den.dim(); ++i)
        pf.den[i] = va_arg(ap, NaReal);

    va_end(ap);
}

//---------------------------------------------------------------------------
// Make A(s)/B(s) polynomial fraction
NaTransFunc::NaTransFunc (const NaVector& A, const NaVector& B)
: NaConfigPart(NaTYPE_TransFunc), ut(utPolyFrac), pf(A, B)
{
    // dummy
}

//---------------------------------------------------------------------------
// Make an explicit copy of the transfer function
NaTransFunc::NaTransFunc (const NaTransFunc& orig)
  : NaUnit(orig), NaConfigPart(NaTYPE_TransFunc),
  ut(orig.ut), pf(orig.pf)
{
    int i;
    for(i = 0; i < orig.items.count(); ++i){
        items.addh(new NaTransFunc(*orig.items(i)));
    }
}

//---------------------------------------------------------------------------
// Destructor
NaTransFunc::~NaTransFunc ()
{
    int i;
    for(i = 0; i < items.count(); ++i){
        delete items[i];
    }
}

//---------------------------------------------------------------------------
// Assignment
NaTransFunc&
NaTransFunc::operator= (const NaTransFunc& orig)
{
    NaUnit::operator=(orig);

    ut = orig.ut;
    pf = orig.pf;

    int i;
    for(i = 0; i < items.count(); ++i){
        delete items[i];
    }
    items.clean();
    for(i = 0; i < orig.items.count(); ++i){
        items.addh(new NaTransFunc(*orig.items(i)));
    }
    return *this;
}

//---------------------------------------------------------------------------
// Print self
void
NaTransFunc::PrintLog () const
{
    NaPrintLog("Transfer function:\n");
    PrintLog("  ");
}

//---------------------------------------------------------------------------
// Print self with indentation
void
NaTransFunc::PrintLog (const char* szIndent) const
{
    NaPrintLog("%s", szIndent);
    if(utPolyFrac == ut){
        pf.PrintLog();
    }else{
        switch(ut){
        case utInOutDim:
	    NaPrintLog("inputs:%u outputs:%d\n", InputDim(), OutputDim());
            break;
        case utInOut:
	    NaPrintLog("strange...\n");
            break;
        case utSum:
            NaPrintLog("+\n");
            break;
        case utProduct:
            NaPrintLog("*\n");
            break;
        }
        int     i;
        char    *szIndentPlus = new char[strlen(szIndent) + strlen("  ") + 1];
        strcat(strcpy(szIndentPlus, szIndent), "  ");
        for(i = 0; i < items.count(); ++i){
	  if(utInOutDim == ut)
	    NaPrintLog("%slink: %d->%d\n",
		       szIndent, i%InputDim(), i/InputDim());
	  items(i)->PrintLog(szIndentPlus);
        }
        delete szIndentPlus;
    }
}

//---------------------------------------------------------------------------
// Reset operations, that must be done before new modelling will start
void
NaTransFunc::Reset ()
{
    int i;
    switch(ut){
    case utSum:
    case utProduct:
    case utInOutDim:
        for(i = 0; i < items.count(); ++i){
            items[i]->Reset();
        }
        break;
    case utPolyFrac:
        pf.Reset();
        break;
    }
}

//---------------------------------------------------------------------------
// Compute output on the basis of internal parameters,
// stored state and external input: y=F(x,t,p)
void
NaTransFunc::Function (NaReal* x, NaReal* y)
{
    int         i, j;
    NaReal      tmp;

    switch(ut){
    case utSum:
        *y = 0.0;
        for(i = 0; i < items.count(); ++i){
            items[i]->Function(x, &tmp);
            *y += tmp;
        }
        break;

    case utProduct:
        tmp = *x;
        for(i = 0; i < items.count(); ++i){
            items[i]->Function(&tmp, y);
            tmp = *y;
        }
        break;

    case utPolyFrac:
        pf.Function(x, y);
        break;

    case utInOutDim:
      // Every output is a sum of values obtained from linked
      // functions
      for(j = 0; j < OutputDim(); ++j) {
	y[j] = 0.0;
	for(i = 0; i < InputDim(); ++i) {
	  items[j*InputDim()+i]->Function(x + i, &tmp);
	  y[j] += tmp;
	}
      }
      break;
    }
}


//---------------------------------------------------------------------------
// Take pointer to scalar function with i input and j output
NaTransFunc*
NaTransFunc::ScalarFunction (unsigned i, unsigned j)
{
  if(i >= InputDim() || j >= OutputDim())
    return NULL;
  if(utPolyFrac == ut && 0 == i && 0 == j)
      return this;

  return items[j * InputDim() + i];
}


//---------------------------------------------------------------------------
// Store configuration data in internal order to given stream
void
NaTransFunc::Save (NaDataStream& ds)
{
    if(utPolyFrac == ut){
      ds.PutF("transfer function", "%s %d",
	      UnionTypeToStrIO(ut), utPolyFrac==ut ? 0: items.count());
      pf.Save(ds);
    }else if(utInOutDim == ut) {
      ds.PutF("MIMO transfer function", "%s %u %u",
	      UnionTypeToStrIO(ut), InputDim(), OutputDim());
      unsigned i, j;
      for(j = 0; j < OutputDim(); ++j) {
	for(i = 0; i < InputDim(); ++i) {
	  ds.PutF("link function", "%s %u %u",
		  UnionTypeToStrIO(utInOut), i, j);
	  items[j*InputDim() + i]->Save(ds);
	}
      }
    }else{ // if(utSum == ut || utProduct == ut)
      ds.PutF("transfer function", "%s %d",
	      UnionTypeToStrIO(ut), utPolyFrac==ut ? 0: items.count());
      int i;
      for(i = 0; i < items.count(); ++i){
	items[i]->Save(ds);
      }
    }
}

//---------------------------------------------------------------------------
// Retrieve configuration data in internal order from given stream
void
NaTransFunc::Load (NaDataStream& ds)
{
    char    szUT[101];
    int     i, j, nItems = 0;
    unsigned nInput = 0, nOutput = 0;

    items.clean();

    // counting words (up to 3 words)
    char	*szData = ds.GetData();
    char        *szBuf = szData;

    for(i = 0; ; ++i){
        // skip leading spaces
        while(isspace(*szBuf) && *szBuf != '\0')
            ++szBuf;

        // while current string is not EOS...
        if('\0' == *szBuf || ';' == *szBuf)
            break;

	while(!isspace(*szBuf) && *szBuf != '\0')
            ++szBuf;
    }

    switch(i) {
    case 2:
      sscanf(szData, "%100s %d", szUT, &nItems);
      break;
    case 3:
      sscanf(szData, "%100s %u %u", szUT, &nInput, &nOutput);
      break;
    default:
      NaPrintLog("Wrong number of tokens in definition of transfer function\n.");
      return;
    }

    switch(ut = StrToUnionTypeIO(szUT)){
    default:
    case __utNumber:
        NaPrintLog("Cannot parse '%s' in definition of transfer function\n.",
                   szUT);
        break;

    case utSum:
    case utProduct:
        for(i = 0; i < nItems; ++i){
            NaTransFunc *tf = new NaTransFunc();
            tf->Load(ds);
            items.addh(tf);
        }
        break;

    case utPolyFrac:
        pf.Load(ds);
        break;

    case utInOutDim:
        // Declare number of inputs and outputs
        Assign(nInput, nOutput);
	nItems = nInput * nOutput;
        for(i = 0; i < nItems; ++i){
	    // Zero link by default
	    NaTransFunc *tf = new NaTransFunc(0, 0.0, 0, 1.0);
            items.addh(tf);
        }

	try{
	  while(true) {
	    ds.GetF("%s %u %u", szUT, &nInput, &nOutput);
	    if(utInOut == StrToUnionTypeIO(szUT)) {
	      
	      NaTransFunc *tf = items[InputDim() * nOutput + nInput];
	      tf->Load(ds);
	    } else {
	      NaPrintLog("Unexpected word '%s' while parsing MIMO function\n",
			 szUT);
	      break;
	    }
	  }
	}
	catch(NaException& ex) {
	  if(ex != na_end_of_file)
	    // It's a problem, let's pass upstack
	    throw(ex);
	}
        break;
    }
}

//---------------------------------------------------------------------------
// Store transfer function to given configuration file
void
NaTransFunc::Save (const char* szFileName)
{
  NaConfigPart	*conf_list[] = { this };
  NaConfigFile	conf_file(";NeuCon transfer", 1, 1, ".tf");
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.SaveToFile(szFileName);
}

//---------------------------------------------------------------------------
// Read transfer function from given configuration file
void
NaTransFunc::Load (const char* szFileName)
{
  NaConfigPart	*conf_list[] = { this };
  NaConfigFile	conf_file(";NeuCon transfer", 1, 1, ".tf");
  conf_file.AddPartitions(NaNUMBER(conf_list), conf_list);
  conf_file.LoadFromFile(szFileName);
}

//-----------------------------------------------------------------------
// Operators
//-----------------------------------------------------------------------

//---------------------------------------------------------------------------
// Add all items of tf to this one by making new ones
NaTransFunc&
NaTransFunc::CopyItems (const NaTransFunc& tf)
{
    int i;
    for(i = 0; i < tf.items.count(); ++i){
        items.addh(new NaTransFunc(*tf.items(i)));
    }
    return *this;
}


//---------------------------------------------------------------------------
// Add all items of tf to this one by moving them from tf
NaTransFunc&
NaTransFunc::MoveItems (NaTransFunc& tf)
{
    while(tf.items.count() > 0){
        items.addh(tf.items[0]);
        tf.items.remove(0);
    }
    return *this;
}


//---------------------------------------------------------------------------
// this = this + tf
NaTransFunc&
NaTransFunc::operator+= (const NaTransFunc& tf)
{
    if(utSum == ut && utSum == tf.ut){
        CopyItems(tf);
    }else if(utSum == ut && utSum != tf.ut){
        items.addh(new NaTransFunc(tf));
    }else if(utSum != ut && utSum == tf.ut){
        NaTransFunc *tf_tmp = new NaTransFunc(*this);
        *this = tf;
        items.addl(tf_tmp);
    }else{ //if(utSum != ut && utSum != tf.ut)
        *this = *this + tf;
    }

    return *this;
}

//---------------------------------------------------------------------------
// this = this * tf
NaTransFunc&
NaTransFunc::operator*= (const NaTransFunc& tf)
{
    if(utProduct == ut && utProduct == tf.ut){
        CopyItems(tf);
    }else if(utProduct == ut && utProduct != tf.ut){
        items.addh(new NaTransFunc(tf));
    }else if(utProduct != ut && utProduct == tf.ut){
        NaTransFunc *tf_tmp = new NaTransFunc(*this);
        *this = tf;
        items.addl(tf_tmp);
    }else{ //if(utProduct != ut && utProduct != tf.ut)
        *this = *this * tf;
    }

    return *this;
}

//---------------------------------------------------------------------------
// tf1 + tf2
NaTransFunc
operator+ (const NaTransFunc& tf1, const NaTransFunc& tf2)
{
    NaTransFunc     tfres;

    if(NaTransFunc::utSum == tf1.ut && NaTransFunc::utSum == tf2.ut){
        tfres = tf1;
        tfres.CopyItems(tf2);
    }else if(NaTransFunc::utSum == tf1.ut && NaTransFunc::utSum != tf2.ut){
        tfres = tf1;
        tfres.items.addh(new NaTransFunc(tf2));     // new one
    }else if(NaTransFunc::utSum != tf1.ut && NaTransFunc::utSum == tf2.ut){
        tfres = tf2;
        tfres.items.addl(new NaTransFunc(tf1));     // new one
    }else{ // nobody among tf1, tf2 is sum
        tfres.ut = NaTransFunc::utSum;
        tfres.items.addh(new NaTransFunc(tf1));     // new one
        tfres.items.addh(new NaTransFunc(tf2));     // new one
    }

    return tfres;
}

//---------------------------------------------------------------------------
// tf1 * tf2
NaTransFunc
operator* (const NaTransFunc& tf1, const NaTransFunc& tf2)
{
    NaTransFunc     tfres;

    if(NaTransFunc::utProduct == tf1.ut
       && NaTransFunc::utProduct == tf2.ut){
        tfres = tf1;
        tfres.CopyItems(tf2);
    }else if(NaTransFunc::utProduct == tf1.ut
             && NaTransFunc::utProduct != tf2.ut){
        tfres = tf1;
        tfres.items.addh(new NaTransFunc(tf2));     // new one
    }else if(NaTransFunc::utProduct != tf1.ut
             && NaTransFunc::utProduct == tf2.ut){
        tfres = tf2;
        tfres.items.addl(new NaTransFunc(tf1));     // new one
    }else{ // nobody among tf1, tf2 is sum
        tfres.ut = NaTransFunc::utProduct;
        tfres.items.addh(new NaTransFunc(tf1));     // new one
        tfres.items.addh(new NaTransFunc(tf2));     // new one
    }

    return tfres;
}


//---------------------------------------------------------------------------
// Input/output operations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const char*
NaTransFunc::UnionTypeToStrIO (UnionType eUT)
{
    switch(eUT){
    case utSum:
        return "sum";
    case utProduct:
        return "product";
    case utPolyFrac:
        return "polyfrac";
    case utInOutDim:
        return "inoutdim";
    case utInOut:
        return "inout";
    default:
        return "?ut?";
    }
}

//---------------------------------------------------------------------------
NaTransFunc::UnionType
NaTransFunc::StrToUnionTypeIO (const char* szUT)
{
    if(!strncmp(szUT, UnionTypeToStrIO(utSum),
	       strlen(UnionTypeToStrIO(utSum)))){
        return utSum;
    }else if(!strncmp(szUT, UnionTypeToStrIO(utProduct),
		      strlen(UnionTypeToStrIO(utProduct)))){
        return utProduct;
    }else if(!strncmp(szUT, UnionTypeToStrIO(utPolyFrac),
		      strlen(UnionTypeToStrIO(utPolyFrac)))){
        return utPolyFrac;
    }else if(!strncmp(szUT, UnionTypeToStrIO(utInOutDim),
		      strlen(UnionTypeToStrIO(utInOutDim)))){
        return utInOutDim;
    }else if(!strncmp(szUT, UnionTypeToStrIO(utInOut),
		      strlen(UnionTypeToStrIO(utInOut)))){
        return utInOut;
    }
    return __utNumber;
}

//---------------------------------------------------------------------------
// Is empty
bool
NaTransFunc::Empty () const
{
    bool    bFlag;
    switch(ut){
    case utSum:
    case utProduct:
        bFlag = (0 == items.count());
        break;
    case utPolyFrac:
        bFlag = (0 == pf.num.dim() && 0 == pf.den.dim());
        break;
    }
    return bFlag;
}

//---------------------------------------------------------------------------
// Make empty
void
NaTransFunc::Clean ()
{
    switch(ut){
    case utSum:
    case utProduct:
        items.clean();
        // no break;
    case utPolyFrac:
        pf.num.new_dim(0);
        pf.den.new_dim(0);
        break;
    }
    ut = utPolyFrac;
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
