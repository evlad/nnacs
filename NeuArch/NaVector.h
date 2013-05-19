//-*-C++-*-
/* NaVector.h */
/* $Id$ */
//---------------------------------------------------------------------------
#ifndef NaVectorH
#define NaVectorH

#include <NaGenerl.h>
#include <NaExcept.h>


// Data file input/output
class NaDataFile;


//---------------------------------------------------------------------------
// Class for vector of real numbers storing and operations over it
class NaVector
{
public:

    /*=======================================*
     *      LIFETIME AND CHANGING SIZE       *
     *=======================================*/

    // Coping constructor
    NaVector (const NaVector& rVect);

    // Create vector of n items
    NaVector (unsigned n = 0);

    // Destroying the vector
    virtual ~NaVector ();

    // Assignment of the vector chaning dimension
    virtual NaVector&   operator= (const NaVector& rVect);

    // Assignment of the vector without chaning dimension
    virtual NaVector&   operator= (const NaReal* pVect);

    // Make new size of vector and unlink it from other data
    virtual NaVector&   new_dim (unsigned n);

    /*=======================================*
     *    INFORMATION AND ACCESS TO ITEMS    *
     *=======================================*/

    // Returns dimension of the vector
    virtual unsigned    dim () const;

    // Access to the item
    virtual NaReal&     fetch (unsigned i);
    virtual NaReal&     operator[] (unsigned i);

    // Access to the value of the item
    virtual NaReal      get (unsigned i) const;
    virtual NaReal      operator() (unsigned i) const;

    // Check for equivalence
    virtual bool	operator== (const NaVector& rVect) const;
    virtual bool	operator!= (const NaVector& rVect) const{
      return !operator==(rVect);
    };

    /*=======================================*
     *    GROUP OPERATIONS AND METHODS       *
     *=======================================*/

    // Initialization of the items
    virtual void        init_zero ();
    virtual void        init_value (NaReal v);
    virtual void        init_random (NaReal vMin, NaReal vMax);

    // Get minimum and maximum values of the vector
    virtual void        min_max (NaReal& vMin, NaReal& vMax) const;

    // Get average value of the vector
    virtual NaReal      average () const;

    // Get median value of the vector
    virtual NaReal      median () const;

    // Get R(N) metric of the vector
    virtual NaReal      metric () const;

    // Compute distance in R(N) space between this and that vectors
    virtual NaReal      distance (const NaVector& that) const;

    // Add a number to each item of the vector
    virtual NaVector&   add (NaReal vNum);

    // Add a vector to this one
    virtual NaVector&   add (const NaVector& rVect);

    // Add the result of scalar multiplication to this vector
    virtual NaVector&   add_smult (const NaVector& rVect1,
                                   const NaVector& rVect2);

    // Multiply a number with each item of the vector
    virtual NaVector&   multiply (NaReal vNum);

    // Store in the vector result of scalar multiplication of two vectors
    virtual NaVector&   scalar_multiply (const NaVector& rVect1,
                                         const NaVector& rVect2);

    // Shift vector:
    // >0 - from low indeces to higher ones
    // <0 - from high indeces to lower ones
    virtual NaVector&   shift (int iOffset);

    // Auto correlation series: 0..N
    virtual void        auto_cov (NaVector& rVectACF) const;

    // Mutual correlation series: -N..0..N => 0..2N-1
    virtual void        cov (const NaVector& rVectY,
                             NaVector& rVectMCF) const;

    // Sort items in vector in ascent order
    virtual void        sort ();

    // Revert order of vector items: v[0]->v[N-1], v[1]->v[N-2]
    virtual void        revert ();

    /*=======================================*
     *         PRINTOUT OF THE ARRAY         *
     *=======================================*/

     // Print contents of the array via NaPrintLog() facility
     virtual void       print_contents () const;

    /*=======================================*
     *           LINKING WITH DATA           *
     *=======================================*/

    // Substitute vector's data by other data storage
    virtual void        link_data (NaVector& rVect);
    virtual void        link_data (unsigned n, NaReal* p);

    // Return sign of linked data (not own storage management)
    virtual bool        linked_data () const;

    /*=======================================*
     *             INPUT/OUTPUT              *
     *=======================================*/

     // Read given variable from data file
     virtual void       read_file (NaDataFile* pDF, int iVar = 0);

     // Write vector to data file
     virtual void       write_file (NaDataFile* pDF) const;

    /*=======================================*
     *      IMPORT/EXPORT OF THE ARRAY       *
     *=======================================*/

    // Export matrix to given file
    virtual void        export_to_file (const char* szFilePath) const;

    // Import matrix from given file
    virtual void        import_from_file (const char* szFilePath);

    // Put values to the string
    virtual void        put_string (char* szBuf) const;

    // Get values from the string (up to one of given characters)
    virtual const char* get_string (const char* szBuf,
                                    const char* szDelim = ";");

private:

    bool        bOwnStorage;// true if own storage management
    unsigned    nDim;       // Number of items
    NaReal      *pVect;     // Storage

};

//---------------------------------------------------------------------------
#endif
