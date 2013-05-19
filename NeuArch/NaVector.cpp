/* NaVector.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "NaMath.h"
#include "NaVector.h"
#include "NaLogFil.h"
#include "NaDataIO.h"


//---------------------------------------------------------------------------

    /*=======================================*
     *      LIFETIME AND CHANGING SIZE       *
     *=======================================*/

//---------------------------------------------------------------------------
// Coping constructor
NaVector::NaVector (const NaVector& rVect)
{
    // Create the same vector
    bOwnStorage = true;
    pVect = new NaReal[rVect.dim()];
    nDim = rVect.dim();

    // Copy the contents of that vector to this one
    operator=(rVect);
}

//---------------------------------------------------------------------------
// Create vector of n items
NaVector::NaVector (unsigned n)
{
    // Create the vector
    bOwnStorage = true;
    pVect = new NaReal[n];
    nDim = n;

    // Initialize items by zero
    init_zero();
}

//---------------------------------------------------------------------------
// Destroying the vector
NaVector::~NaVector ()
{
    if(bOwnStorage){
        delete[] pVect;
    }
}

//---------------------------------------------------------------------------
// Assignment of the vector
NaVector&
NaVector::operator= (const NaVector& rVect)
{
    new_dim(rVect.dim());

    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] = rVect.pVect[i];

    return *this;
}

//---------------------------------------------------------------------------
// Assignment of the vector without chaning dimension
NaVector&
NaVector::operator= (const NaReal* pVect_)
{
    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] = pVect_[i];

    return *this;
}

//---------------------------------------------------------------------------
// Make new size of vector and unlink it from other data
NaVector&
NaVector::new_dim (unsigned n)
{
    if(bOwnStorage){
        if(nDim < n){
            NaReal  *pOld = pVect;
            pVect = new NaReal[n];
            memset(pVect, 0, sizeof(NaReal) * n);
            memcpy(pVect, pOld, sizeof(NaReal) * nDim);
            delete[] pOld;
        }
    }else{
        NaReal  *pOld = pVect;
        pVect = new NaReal[n];
        if(nDim < n){
            memset(pVect, 0, sizeof(NaReal) * n);
            memcpy(pVect, pOld, sizeof(NaReal) * nDim);
        }else{
            memcpy(pVect, pOld, sizeof(NaReal) * n);
        }
        bOwnStorage = true;
    }
    nDim = n;

    return *this;
}

//---------------------------------------------------------------------------

    /*=======================================*
     *    INFORMATION AND ACCESS TO ITEMS    *
     *=======================================*/

//---------------------------------------------------------------------------
// Returns dimension of the vector
unsigned
NaVector::dim () const
{
    return nDim;
}

//---------------------------------------------------------------------------
// Check for equivalence
bool
NaVector::operator== (const NaVector& rVect) const
{
  unsigned	i;
  if(dim() != rVect.dim())
    return false;
  for(i = 0; i < dim(); ++i)
    if(get(i) != rVect.get(i))
      return false;
  return true;
}

//---------------------------------------------------------------------------
// Access to the item
NaReal&
NaVector::fetch (unsigned i)
{
    if(i >= dim())
        throw(na_out_of_range);

    return *(pVect + i);
}

//---------------------------------------------------------------------------
// Access to the item
NaReal&
NaVector::operator[] (unsigned i)
{
    return fetch(i);
}

//---------------------------------------------------------------------------
// Access to the value of the item
NaReal
NaVector::get (unsigned i) const
{
    if(i >= dim())
        throw(na_out_of_range);

    return *(pVect + i);
}

//---------------------------------------------------------------------------
// Access to the value of the item
NaReal
NaVector::operator() (unsigned i) const
{
    return get(i);
}

//---------------------------------------------------------------------------

    /*=======================================*
     *    GROUP OPERATIONS AND METHODS       *
     *=======================================*/

//---------------------------------------------------------------------------
// Initialization of the items by zero
void
NaVector::init_zero ()
{
    init_value(0.0);
}

//---------------------------------------------------------------------------
// Initialization of the items by given value
void
NaVector::init_value (NaReal v)
{
    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] = v;
}

//---------------------------------------------------------------------------
// Initialization of the items by uniformly distributed pseudo-random numbers
void
NaVector::init_random (NaReal vMin, NaReal vMax)
{
    unsigned    i;
    for(i = 0; i < dim(); ++i)
      pVect[i] = rand_unified(vMin, vMax);
}

//---------------------------------------------------------------------------
// Get minimum and maximum values of the vector
void
NaVector::min_max (NaReal& vMin, NaReal& vMax) const
{
    unsigned    i;
    for(i = 0; i < dim(); ++i){
        if(0 == i)
            vMin = vMax = pVect[i];
        else{
            if(pVect[i] < vMin)
                vMin = pVect[i];
            if(pVect[i] > vMax)
                vMax = pVect[i];
        }
    }
}

//---------------------------------------------------------------------------
// Get average value of the vector
NaReal
NaVector::average () const
{
    if(0 == dim())
        return 0.0;

    NaReal      sum = 0.0;
    unsigned    i;
    for(i = 0; i < dim(); ++i){
        sum += pVect[i];
    }
    return sum / dim();
}

//---------------------------------------------------------------------------
// Get median value of the vector
NaReal
NaVector::median () const
{
  if(0 == dim())
    return 0.0;

  NaVector	rVect(*this);
  int		N = rVect.dim();

  rVect.sort();

  if(1 == N % 2){
    return rVect((N - 1) / 2);
  }else{
    return (rVect((N / 2) - 1) +
	    rVect((N / 2))) / 2;
  }
}

//---------------------------------------------------------------------------
// Get R(N) metric of the vector
NaReal
NaVector::metric () const
{
    unsigned    i;
    NaReal      vMetric = 0.0;
    for(i = 0; i < dim(); ++i)
        vMetric += pVect[i] * pVect[i];
    return sqrt(vMetric);
}

//---------------------------------------------------------------------------
// Compute distance in R(N) space between this and that vectors
NaReal
NaVector::distance (const NaVector& that) const
{
    unsigned    i;
    NaReal      vDist = 0.0;
    NaReal      vTmp;
    for(i = 0; i < dim(); ++i){
        vTmp = pVect[i] - that.pVect[i];
        vDist += vTmp * vTmp;
    }
    return sqrt(vDist);
}

//---------------------------------------------------------------------------
// Add a number to each item of the vector
NaVector&
NaVector::add (NaReal vNum)
{
    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] += vNum;

    return *this;
}

//---------------------------------------------------------------------------
// Add a vector to this one
NaVector&
NaVector::add (const NaVector& rVect)
{
    if(dim() != rVect.dim())
        throw(na_size_mismatch);

    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] += rVect.pVect[i];

    return *this;
}

//---------------------------------------------------------------------------
// Add the result of scalar multiplication to this vector
NaVector&
NaVector::add_smult (const NaVector& rVect1, const NaVector& rVect2)
{
    if(dim() != rVect1.dim() || dim() != rVect2.dim())
        throw(na_size_mismatch);

    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] += rVect1.pVect[i] * rVect2.pVect[i];

    return *this;
}

//---------------------------------------------------------------------------
// Multiply a number with each item of the vector
NaVector&
NaVector::multiply (NaReal vNum)
{
    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] *= vNum;

    return *this;
}

//---------------------------------------------------------------------------
// Store in the vector result of scalar multiplication of two vectors
NaVector&
NaVector::scalar_multiply (const NaVector& rVect1, const NaVector& rVect2)
{
    if(rVect1.dim() != rVect2.dim())
        throw(na_size_mismatch);

    new_dim(rVect1.dim());

    unsigned    i;
    for(i = 0; i < dim(); ++i)
        pVect[i] = rVect1.pVect[i] * rVect2.pVect[i];

    return *this;
}

//---------------------------------------------------------------------------
// Shift vector:
// >0 - from low indeces to higher ones
// <0 - from high indeces to lower ones
NaVector&
NaVector::shift (int iOffset)
{
    int i;

    if(iOffset >= (int)dim() ||
       iOffset <= - (int)dim()){
       init_zero();
    }else if(iOffset > 0){
        for(i = dim() - iOffset - 1; i >= 0; --i){
            pVect[i + iOffset] = pVect[i];
        }
        for(i = 0; i < iOffset; ++i){
            pVect[i] = 0.0;
        }
    }else if(iOffset < 0){
        for(i = 0; i < (int)dim() + iOffset; ++i){
            pVect[i] = pVect[i - iOffset];
        }
        for(i = dim() + iOffset; i < (int)dim(); ++i){
            pVect[i] = 0.0;
        }
    }
    // else
    //   iOffset == 0
    
    return *this;
}

//---------------------------------------------------------------------------
// Auto correlation series: 0..N
void
NaVector::auto_cov (NaVector& rVectACF) const
{
    rVectACF.new_dim(dim());

    if(0 == dim())
        return;

    unsigned    k, t, N = dim();
    NaReal      fAvg = average();

    // Compute ACF
    for(k = 0; k < N; ++k){
        NaReal  fSum = 0;
        for(t = 0; t < N + k; ++t){
            fSum += (get(t % N) - fAvg) * (get((t + k) % N) - fAvg);
        }
        rVectACF[k] = fSum / N;
    }
}

//---------------------------------------------------------------------------
// Mutual correlation series: -N..0..N => 0..2N-1
void
NaVector::cov (const NaVector& rVectY, NaVector& rVectMCF) const
{
    int         k, t, N = (int)dim();

    if(N > (int)rVectY.dim())
        N = rVectY.dim();

    if(0 == N)
        return;

    rVectMCF.new_dim(2*N - 1);

    NaReal      fAvgX = average(), fAvgY = rVectY.average();

    // Compute MCF
    for(k = - N + 1; k < N; ++k){
        NaReal  fSum = 0;

        if(k < 0){
            for(t = 0; t < N - k; ++t){
                fSum += (rVectY(t % N) - fAvgY) * (get((t - k) % N) - fAvgX);
            }
        }else{// k >= 0
            for(t = 0; t < N + k; ++t){
                fSum += (get(t % N) - fAvgX) * (rVectY((t + k) % N) - fAvgY);
            }
        }
        rVectMCF[k + N - 1] = fSum / N;
    }
}

//---------------------------------------------------------------------------
static int
compare_NaReals (const void *a, const void *b)
{
  const NaReal *da = (const NaReal *) a;
  const NaReal *db = (const NaReal *) b;
     
  return (*da > *db) - (*da < *db);
}

//---------------------------------------------------------------------------
// Sort items in vector in ascent order
void
NaVector::sort ()
{
  qsort(pVect, dim(), sizeof(NaReal), compare_NaReals);
}

//---------------------------------------------------------------------------
// Revert order of vector items: v[0]->v[N-1], v[1]->v[N-2]
void
NaVector::revert ()
{
  NaReal	tmp;
  int		i, N = dim(), N_2 = dim()/2;
  for(i = 0; i < N_2; ++i){
    tmp = fetch(i);
    fetch(i) = fetch(N - 1 - i);
    fetch(N - 1 - i) = tmp;
  }
}

//---------------------------------------------------------------------------

    /*=======================================*
     *         PRINTOUT OF THE ARRAY         *
     *=======================================*/

//---------------------------------------------------------------------------
// Print contents of the array via NaPrintLog() facility
void
NaVector::print_contents () const
{
    unsigned    i;
    NaPrintLog("Vector(this=%p, dim=%u):\n", this, dim());
    for(i = 0; i < dim(); ++i){
        NaPrintLog("\t%g\n", pVect[i]);
    }
}

//---------------------------------------------------------------------------

    /*=======================================*
     *           LINKING WITH DATA           *
     *=======================================*/

//---------------------------------------------------------------------------
// Substitute vector's data by other data storage
void
NaVector::link_data (unsigned n, NaReal* p)
{
    if(bOwnStorage){
        delete[] pVect;
    }
    bOwnStorage = false;
    nDim = n;
    pVect = p;
}

//---------------------------------------------------------------------------
// Substitute vector's data by other data storage
void
NaVector::link_data (NaVector& rVect)
{
    if(bOwnStorage){
        delete[] pVect;
    }
    bOwnStorage = false;
    nDim = rVect.nDim;
    pVect = rVect.pVect;
}

//---------------------------------------------------------------------------
// Return sign of linked data (not own storage management)
bool
NaVector::linked_data () const
{
    return !bOwnStorage;
}

//---------------------------------------------------------------------------

    /*=======================================*
     *             INPUT/OUTPUT              *
     *=======================================*/

//---------------------------------------------------------------------------
// Read given variable from data file
void
NaVector::read_file (NaDataFile* pDF, int iVar)
{
    if(NULL == pDF)
        throw(na_null_pointer);

    new_dim(pDF->CountOfRecord());

    unsigned    i;
    for(i = 0, pDF->GoStartRecord(); pDF->GoNextRecord(); ++i){
        fetch(i) = pDF->GetValue(iVar);
    }   // Cycle for all data in file
}

//---------------------------------------------------------------------------
// Write vector to data file
void
NaVector::write_file (NaDataFile* pDF) const
{
    if(NULL == pDF)
        throw(na_null_pointer);

    unsigned    i;
    for(i = 0; i < dim(); ++i){
        pDF->AppendRecord();
        pDF->SetValue(get(i));
    }
}

//---------------------------------------------------------------------------

    /*=======================================*
     *      IMPORT/EXPORT OF THE ARRAY       *
     *=======================================*/

//---------------------------------------------------------------------------
// Export vector given file
void
NaVector::export_to_file (const char* szFilePath) const
{
    if(NULL == szFilePath)
        throw(na_null_pointer);

    FILE    *fp = fopen(szFilePath, "wt");
    if(NULL == fp)
        throw(na_cant_open_file);

    fprintf(fp, "%u\n", dim());

    unsigned    i;
    for(i = 0; i < dim(); ++i){
         fprintf(fp, "%g\n", get(i));
    }

    fclose(fp);
}

//---------------------------------------------------------------------------
// Import vector from given file
void
NaVector::import_from_file (const char* szFilePath)
{
    if(NULL == szFilePath)
        throw(na_null_pointer);

    FILE    *fp = fopen(szFilePath, "rt"); // rt
    if(NULL == fp)
        throw(na_cant_open_file);

    unsigned    n;
    if(1 != fscanf(fp, "%u\n", &n))
        throw(na_read_error);

    new_dim(n);

    unsigned    i;
    for(i = 0; i < dim(); ++i){
        NaReal  fVal;
        if(1 != fscanf(fp, "%lg", &fVal))
            throw(na_read_error);
        fetch(i) = fVal;
    }

    fclose(fp);
}

//---------------------------------------------------------------------------
// Put values to the string
void
NaVector::put_string (char* szBuf) const
{
    unsigned    i;

    szBuf[0] = '\0';

    // numerator
    for(i = 0; i < dim(); ++i){
        sprintf(szBuf + strlen(szBuf), " %g", get(i));
    }
}

//---------------------------------------------------------------------------
// Get values from the string (up to one of given characters)
const char*
NaVector::get_string (const char* szBuf, const char* szDelim)
{
    const char  *szPtr = szBuf;
    char        *szNext;
    unsigned    i;

    // skip leading spaces
    while(isspace(*szPtr) && *szPtr != '\0' &&
          NULL != strchr(szDelim, *szPtr))
        ++szPtr;

    new_dim(0);

    for(i = 0; ; ++i){
        //??? Strange things: szPtr value differs from (4) to (1) 
        //???NaPrintLog("get_string(1): szPtr='%s'(%p)\n", szPtr, szPtr);

        // skip leading spaces
        while(isspace(*szPtr) && *szPtr != '\0' &&
              NULL != strchr(szDelim, *szPtr))
            ++szPtr;

        //???NaPrintLog("get_string(2): szPtr='%s'(%p)\n", szPtr, szPtr);

        // while current string is not a delimiter and is not EOS...
        if('\0' == *szPtr || NULL != strchr(szDelim, *szPtr))
            break;

        // read number
        NaReal  fVal = strtod(szPtr, &szNext);
        //???NaPrintLog("get_string(): '%s' -> %g, next is '%s'\n",
        //???           szPtr, fVal, szNext);

        if(szNext == szPtr){
            // error while reading
            return szPtr;
        }
        szPtr = szNext;
        //???NaPrintLog("get_string(3): szPtr='%s'(%p)\n", szPtr, szPtr);

        new_dim(i + 1);
        fetch(i) = fVal;

        //???NaPrintLog("get_string(4): szPtr='%s'(%p)\n", szPtr, szPtr);
    }

    return szPtr;
}

//---------------------------------------------------------------------------

