/* NaMatrix.cpp */
static char rcsid[] = "$Id$";
//---------------------------------------------------------------------------
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <matrix.h>

#include "NaMath.h"
#include "NaMatrix.h"
#include "NaLogFil.h"


//---------------------------------------------------------------------------

    /*=======================================*
     *      LIFETIME AND CHANGING SIZE       *
     *=======================================*/

//---------------------------------------------------------------------------
// Coping constructor
NaMatrix::NaMatrix (const NaMatrix& rMatr)
{
    // Create the same matrix
    pMatr = new NaReal[rMatr.dim_rows() * rMatr.dim_cols()];
    nDimRow = rMatr.dim_rows();
    nDimCol = rMatr.dim_cols();

    // Copy the contents of that matrix to this one
    operator=(rMatr);
}

//---------------------------------------------------------------------------
// Create vector of nR rows and nC colomns
NaMatrix::NaMatrix (unsigned nR, unsigned nC)
{
    nDimRow = nR;
    nDimCol = nC;
    pMatr = new NaReal[nDimCol * nDimRow];
}

//---------------------------------------------------------------------------
// Destroying the matrix
NaMatrix::~NaMatrix ()
{
    delete[] pMatr;
}

//---------------------------------------------------------------------------
// Assignment of the matrix without changing dimension
NaMatrix&   NaMatrix::operator= (const NaReal* pMatr_)
{
    unsigned    i;
    for(i = 0; i < nDimRow * nDimCol; ++i)
        pMatr[i] = pMatr_[i];

    return *this;
}


//---------------------------------------------------------------------------
// Assignment of the matrix changing dimension
NaMatrix&   NaMatrix::operator= (const NaMatrix& rMatr)
{
    new_dim(rMatr.dim_rows(), rMatr.dim_cols());

    unsigned    i;
    for(i = 0; i < nDimRow * nDimCol; ++i)
        pMatr[i] = rMatr.pMatr[i];

    return *this;
}

//---------------------------------------------------------------------------
// Make new size of matrix
NaMatrix&   NaMatrix::new_dim (unsigned nR, unsigned nC)
{
    if(nR * nC > nDimRow * nDimCol){
        delete[] pMatr;
        pMatr = new NaReal[nR * nC];
        init_zero();
    }
    nDimRow = nR;
    nDimCol = nC;

    return *this;
}


//---------------------------------------------------------------------------

    /*=======================================*
     *    INFORMATION AND ACCESS TO ITEMS    *
     *=======================================*/

//---------------------------------------------------------------------------
// Returns dimension of the matrix
unsigned    NaMatrix::dim_rows () const
{
    return nDimRow;
}

//---------------------------------------------------------------------------
// Returns dimension of the matrix
unsigned    NaMatrix::dim_cols () const
{
    return nDimCol;
}

//---------------------------------------------------------------------------
// Access to the item
NaReal&     NaMatrix::fetch (unsigned iR, unsigned iC)
{
    if(invalid(iR, iC))
        throw(na_out_of_range);

    return *(row_ptr(iR) + iC);
}

//---------------------------------------------------------------------------
// Access to the item
NaReal*     NaMatrix::operator[] (unsigned iR)
{
    if(invalid(iR, 0))
        throw(na_out_of_range);

    return row_ptr(iR);
}

//---------------------------------------------------------------------------
// Access to the value of the item
NaReal      NaMatrix::get (unsigned iR, unsigned iC) const
{
    if(invalid(iR, iC))
        throw(na_out_of_range);

    return *(row_ptr(iR) + iC);
}

//---------------------------------------------------------------------------
// Access to the value of the item
NaReal      NaMatrix::operator() (unsigned iR, unsigned iC) const
{
    return get(iR, iC);
}


//---------------------------------------------------------------------------
// Check for equivalence
bool
NaMatrix::operator== (const NaMatrix& rMatr) const
{
    if(dim_rows() != rMatr.dim_rows() || dim_cols() != rMatr.dim_cols())
	return false;
    unsigned	i, j;
    for(i = 0; i < dim_rows(); ++i)
	for(j = 0; j < dim_cols(); ++j)
	    if(get(i, j) != rMatr.get(i, j))
		return false;
    return true;
}


//---------------------------------------------------------------------------

    /*=======================================*
     *    GROUP OPERATIONS AND METHODS       *
     *=======================================*/

//---------------------------------------------------------------------------
// Initialization of the items by zero
void        NaMatrix::init_zero ()
{
    init_value(0.0);
}

//---------------------------------------------------------------------------
// Initialization of the diagonal items by the given value and other ones by 0
void        NaMatrix::init_diag (NaReal v)
{
    unsigned    i, j;
    for(i = 0; i < nDimRow; ++i){
        for(j = 0; j < nDimCol; ++j){
            if(i == j)
                row_ptr(i)[j] = v;
            else
                row_ptr(i)[j] = 0.0;
        }
    }
}

//---------------------------------------------------------------------------
// Initialization of the items by the given value
void        NaMatrix::init_value (NaReal v)
{
    unsigned    i;
    for(i = 0; i < nDimRow * nDimCol; ++i)
        pMatr[i] = v;
}

//---------------------------------------------------------------------------
// Initialization of the items by uniformly distributed pseudo-random numbers
void        NaMatrix::init_random (NaReal vMin, NaReal vMax)
{
    unsigned    i;
    for(i = 0; i < nDimRow * nDimCol; ++i)
      pMatr[i] = rand_unified(vMin, vMax);
}


//---------------------------------------------------------------------------
// Create (use pRes!=NULL) vector as the copy of the given row
NaVector*   NaMatrix::row_vector (unsigned iR, NaVector* pRes) const
{
    if(invalid(iR, 0))
        throw(na_out_of_range);

    // Use passed vector pRes if it's possible
    NaVector    *pVect = pRes;
    if(NULL == pVect)
        pVect = new NaVector(nDimCol);
    else
        pVect->new_dim(nDimCol);

    unsigned    i;
    for(i = 0; i < nDimCol; ++i)
        pVect->fetch(i) = get(iR, i);

    return pVect;
}

//---------------------------------------------------------------------------
// Create (use pRes!=NULL) vector as the copy of the given column
NaVector*   NaMatrix::col_vector (unsigned iC, NaVector* pRes) const
{
    if(invalid(0, iC))
        throw(na_out_of_range);

    // Use passed vector pRes if it's possible
    NaVector    *pVect = pRes;
    if(NULL == pVect)
        pVect = new NaVector(nDimRow);
    else
        pVect->new_dim(nDimRow);

    unsigned    i;
    for(i = 0; i < nDimRow; ++i)
        pVect->fetch(i) = get(i, iC);

    return pVect;
}

//---------------------------------------------------------------------------
// Add a matrix to this one
NaMatrix&   NaMatrix::add (const NaMatrix& rMatr)
{
    if(dim_rows() != rMatr.dim_rows() ||
       dim_cols() != rMatr.dim_cols())
        throw(na_size_mismatch);

    unsigned    iR, iC;
    for(iR = 0; iR < nDimRow; ++iR)
        for(iC = 0; iC < nDimCol; ++iC)
            *(row_ptr(iR) + iC) += *(rMatr.row_ptr(iR) + iC);

    return *this;
}

//---------------------------------------------------------------------------
// Multiply this matrix by another and place result to given
NaMatrix&   NaMatrix::multiply (const NaMatrix& mAnother,
                                NaMatrix& mResult) const
{
    if(dim_cols() != mAnother.dim_rows()){
        throw(na_size_mismatch);
    }

    unsigned    i, j, k;

    mResult.new_dim(dim_rows(), mAnother.dim_cols());
    mResult.init_zero();

    for(i = 0; i < dim_rows(); ++i){
        for(j = 0; j < mAnother.dim_cols(); ++j){
            for(k = 0; k < dim_cols(); ++k){
                mResult[i][j] += get(i, k) * mAnother(k, j);
            }
        }
    }

    return mResult;
}

//---------------------------------------------------------------------------
// Multiply this matrix by the rArg and place result to vector
NaVector&   NaMatrix::multiply (NaVector& rArg, NaVector& rRes) const
{
    if(dim_cols() != rArg.dim())
        throw(na_size_mismatch);

    rRes.new_dim(dim_rows());

    unsigned    iR, iC;
    for(iR = 0; iR < dim_rows(); ++iR){
        rRes[iR] = 0.0;
        for(iC = 0; iC < dim_cols(); ++iC)
            rRes[iR] += *(row_ptr(iR) + iC) * rArg[iC];
    }
#if 0
    if(dim_rows() != rArg.dim())
        throw(na_size_mismatch);

    rRes.new_dim(dim_cols());

    unsigned    iR, iC;
    for(iR = 0; iR < nDimRow; ++iR){
        rRes[iR] = 0.0;
        for(iC = 0; iC < nDimCol; ++iC)
            rRes[iR] += *(row_ptr(iR) + iC) * rArg[iC];
    }
#endif
    return rRes;
}

//---------------------------------------------------------------------------
// Multiply a number with each item of the matrix
NaMatrix&   NaMatrix::multiply (NaReal vNum)
{
    unsigned    i, j;

    for(i = 0; i < dim_cols(); ++i){
        for(j = 0; j < dim_rows(); ++j){
            fetch(i, j) = vNum * get(i, j);
        }
    }
    return *this;
}

//---------------------------------------------------------------------------
// Return address of the row pointer
NaReal*     NaMatrix::row_ptr (unsigned iR) const
{
    return pMatr + nDimCol * iR;
}

//---------------------------------------------------------------------------
// Check cell address and return true if invalid
bool        NaMatrix::invalid (unsigned iR, unsigned iC) const
{
    return iR >= nDimRow || iC >= nDimCol;
}

//---------------------------------------------------------------------------
// Compute determinant of the matrix
NaReal      NaMatrix::det () const
{
#ifdef __matrix_h
    if(dim_rows() != dim_cols()){
        throw(na_size_mismatch);
    }

    NaReal  vDet = 0;
    MATRIX  mat = mat_creat(dim_rows(), dim_cols(), UNDEFINED);

    unsigned    i, j, n = dim_rows();
    for(i = 0; i < n; ++i){
        for(j = 0; j < n; ++j){
            mat[i][j] = get(i, j);
        }
    }

    vDet = mat_det(mat);
    mat_free(mat);
    return vDet;
#else
    throw(na_not_implemented);
#endif/* __matrix_h */

#if 0
    unsigned    i, j, k, n = dim_rows();


    // Predefined det()
    switch(n){
    case 1:
        vDet = get(0, 0);
        break;
    case 2:
        vDet = get(0, 0) * get (1, 1) - get(0, 1) * get(1, 0);
        break;
    default:{
        // Allocate space for minor matrix
        NaMatrix    mOfMinor(n - 1, n - 1);

        for(i = 0; i < n; ++i){
            // Compose minor
            for(j = 0; j < n - 1; ++j){
                for(k = 0; k < n - 1; ++k){
                    if(k < i){
                        mOfMinor[j][k] = get(1 + j, k);
                    }else if(k >= i){
                        mOfMinor[j][k] = get(1 + j, 1 + k);
                    }
                }
            }// minor composition
            //NaPrintLog("Minor(%d,%d):\n", i, n - 1);
            //mOfMinor.print_contents();

            // Summarize determinant
            if(i % 2){
                vDet -= get(0, i) * mOfMinor.det();
            }else{
                vDet += get(0, i) * mOfMinor.det();
            }
        }
        }break;
    }// end of switch for different cases
     
    //NaPrintLog("det=%g\n", vDet);
    return vDet;
#endif
}

//---------------------------------------------------------------------------
// Compute inverse matrix
NaMatrix&   NaMatrix::inv (NaMatrix& mInv) const
{
#ifdef __matrix_h
    if(dim_rows() != dim_cols()){
        throw(na_size_mismatch);
    }

    MATRIX  mat = mat_creat(dim_rows(), dim_cols(), UNDEFINED);

    unsigned    i, j, n = dim_rows();
    for(i = 0; i < n; ++i){
        for(j = 0; j < n; ++j){
            mat[i][j] = get(i, j);
        }
    }

    MATRIX  minv = mat_inv(mat);
    if(NULL == minv){
        throw(na_bad_value);
    }

    mat_free(mat);
    mInv.new_dim(n, n);
    for(i = 0; i < n; ++i){
        for(j = 0; j < n; ++j){
            mInv.fetch(i, j) = minv[i][j];
        }
    }

    mat_free(minv);
#if 0
    trans(mInv);

    NaReal  vDet = det();

    if(0 == vDet)
        throw(na_bad_value);
    else
        mInv.multiply(1/vDet);
#endif

    return mInv;
#else
    throw(na_not_implemented);
#endif /* __matrix_h */
}

//---------------------------------------------------------------------------
// Compute transposed matrix
NaMatrix&   NaMatrix::trans (NaMatrix& mTrans) const
{
    unsigned    i, j;

    mTrans.new_dim(dim_cols(), dim_rows());
    for(i = 0; i < dim_rows(); ++i){
        for(j = 0; j < dim_cols(); ++j){
            mTrans[j][i] = get(i, j);
        }
    }

    return mTrans;
}

//---------------------------------------------------------------------------

    /*=======================================*
     *         PRINTOUT OF THE ARRAY         *
     *=======================================*/

//---------------------------------------------------------------------------
// Print contents of the array via NaPrintLog() facility
void       NaMatrix::print_contents () const
{
    unsigned    iR, iC;
    NaPrintLog("Matrix(this=%p, rows=%u, cols=%u):\n",
               this, nDimRow, nDimCol);
    for(iR = 0; iR < nDimRow; ++iR){
        for(iC = 0; (int)iC < (int)nDimCol - 1; ++iC)
            NaPrintLog("\t%g", *(row_ptr(iR) + iC));
        NaPrintLog("\t%g\n", *(row_ptr(iR) + nDimCol - 1));
    }
}

//---------------------------------------------------------------------------

    /*=======================================*
     *      IMPORT/EXPORT OF THE ARRAY       *
     *=======================================*/

//---------------------------------------------------------------------------
// Export matrix to given file
void
NaMatrix::export_to_file (const char* szFilePath) const
{
    if(NULL == szFilePath)
        throw(na_null_pointer);

    FILE    *fp = fopen(szFilePath, "wt");
    if(NULL == fp)
        throw(na_cant_open_file);

    fprintf(fp, "%u %u\n", dim_rows(), dim_cols());

    unsigned    i, j;
    for(i = 0; i < dim_rows(); ++i){
         for(j = 0; j < dim_cols(); ++j){
            fprintf(fp, " %g", get(i, j));
         }
         fputc('\n', fp);
    }

    fclose(fp);
}

//---------------------------------------------------------------------------
// Import matrix from given file
void
NaMatrix::import_from_file (const char* szFilePath)
{
    if(NULL == szFilePath)
        throw(na_null_pointer);

    FILE    *fp = fopen(szFilePath, "rt");
    if(NULL == fp)
        throw(na_cant_open_file);

    unsigned    rows, cols;
    if(2 != fscanf(fp, "%u %u\n", &rows, &cols))
        throw(na_read_error);

    new_dim(rows, cols);

    unsigned    i, j;
    for(i = 0; i < dim_rows(); ++i){
         for(j = 0; j < dim_cols(); ++j){
            NaReal  fVal;
            if(1 != fscanf(fp, "%lg", &fVal))
                throw(na_read_error);
            fetch(i, j) = fVal;
         }
    }

    fclose(fp);
}

//---------------------------------------------------------------------------
