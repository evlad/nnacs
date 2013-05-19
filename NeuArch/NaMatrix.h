//-*-C++-*-
/* NaMatrix.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaMatrixH
#define NaMatrixH

#include <NaGenerl.h>
#include <NaExcept.h>
#include <NaVector.h>
#include <NaLogFil.h>


//---------------------------------------------------------------------------
// Class for matrix of real numbers storing and operations over it
class NaMatrix
{
public:

    /*=======================================*
     *      LIFETIME AND CHANGING SIZE       *
     *=======================================*/

    // Coping constructor
    NaMatrix (const NaMatrix& rMatr);

    // Create vector of nR rows and nC colomns
    NaMatrix (unsigned nR = 0, unsigned nC = 0);

    // Destroying the matrix
    virtual ~NaMatrix ();

    // Assignment of the matrix changing dimension
    virtual NaMatrix&   operator= (const NaMatrix& rMatr);

    // Assignment of the matrix without changing dimension
    virtual NaMatrix&   operator= (const NaReal* pMatr);

    // Make new size of matrix
    virtual NaMatrix&   new_dim (unsigned nR, unsigned nC);

    /*=======================================*
     *    INFORMATION AND ACCESS TO ITEMS    *
     *=======================================*/

    // Returns dimension of the matrix
    virtual unsigned    dim_rows () const;
    virtual unsigned    dim_cols () const;

    // Access to the item
    virtual NaReal&     fetch (unsigned iR, unsigned iC);
    virtual NaReal*     operator[] (unsigned iR);

    // Access to the value of the item
    virtual NaReal      get (unsigned iR, unsigned iC) const;
    virtual NaReal      operator() (unsigned iR, unsigned iC) const;

     // Check for equivalence
    virtual bool	operator== (const NaMatrix& rMatr) const;
    virtual bool	operator!= (const NaMatrix& rMatr) const{
      return !operator==(rMatr);
    };
   /*=======================================*
     *    GROUP OPERATIONS AND METHODS       *
     *=======================================*/

    // Initialization of the items
    virtual void        init_zero ();
    virtual void        init_diag (NaReal v);
    virtual void        init_value (NaReal v);
    virtual void        init_random (NaReal vMin, NaReal vMax);

    // Create (use pRes!=NULL) vector as the copy of the given row
    virtual NaVector*   row_vector (unsigned iR, NaVector* pRes = NULL) const;

    // Create (use pRes!=NULL) vector as the copy of the given column
    virtual NaVector*   col_vector (unsigned iC, NaVector* pRes = NULL) const;

    // Add a matrix to this one
    virtual NaMatrix&   add (const NaMatrix& rMatr);

    // Multiply this matrix by another and place result to given
    virtual NaMatrix&   multiply (const NaMatrix& mAnother,
                                  NaMatrix& mResult) const;

    // Multiply this matrix by the rArg and place result to vector
    virtual NaVector&   multiply (NaVector& rArg, NaVector& rRes) const;

    // Multiply a number with each item of the matrix
    virtual NaMatrix&   multiply (NaReal vNum);

    // Compute determinant of the matrix
    virtual NaReal      det () const;

    // Compute inverse matrix
    virtual NaMatrix&   inv (NaMatrix& mInv) const;

    // Compute transposed matrix
    virtual NaMatrix&   trans (NaMatrix& mTrans) const;

    /*=======================================*
     *         PRINTOUT OF THE ARRAY         *
     *=======================================*/

    // Print contents of the array via NaPrintLog() facility
    virtual void       print_contents () const;

    /*=======================================*
     *      IMPORT/EXPORT OF THE ARRAY       *
     *=======================================*/

    // Export matrix to given file
    virtual void       export_to_file (const char* szFilePath) const;

    // Import matrix from given file
    virtual void       import_from_file (const char* szFilePath);

private:

    // Return address of the row pointer
    NaReal*     row_ptr (unsigned iR) const;

    // Check cell address and return true if invalid
    bool        invalid (unsigned iR, unsigned iC) const;

    unsigned    nDimRow;    // Number of rows
    unsigned    nDimCol;    // Number of columns
    NaReal      *pMatr;     // Storage

};

//---------------------------------------------------------------------------
#endif
