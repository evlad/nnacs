#ifndef __matrix_h
#define __matrix_h

#include <stdio.h>

/*
*-----------------------------------------------------------------------------
*	file:	matrix.h
*	desc:	matrix mathematics header file
*	by:	ko shu pui, patrick
*	date:	24 nov 91	v0.1b
*	revi:
*	ref:
*       [1] Mary L.Boas, "Mathematical Methods in the Physical Sciene,"
*	John Wiley & Sons, 2nd Ed., 1983. Chap 3.
*
*-----------------------------------------------------------------------------
*/

/*
*-----------------------------------------------------------------------------
*	internal matrix structure
*-----------------------------------------------------------------------------
*/
typedef struct {
	int	row;
	int	col;
	}	MATHEAD;

typedef struct {
	MATHEAD	head;
	/*
	* only the starting address of the following will be
	* returned to the C programmer, like malloc() concept
	*/
	double	*matrix;
	}	MATBODY;

typedef	double	**MATRIX;

#define	Mathead(a)	    ((MATHEAD *)((MATHEAD *)(a) - 1))
#define MatRow(a)	    (Mathead(a)->row)
#define	MatCol(a)	    (Mathead(a)->col)

/*
*----------------------------------------------------------------------------
*	mat_errors definitions
*----------------------------------------------------------------------------
*/
#define	MAT_MALLOC	    1
#define MAT_FNOTOPEN	2
#define	MAT_FNOTGETMAT	3

/*
*----------------------------------------------------------------------------
*	matrice types
*----------------------------------------------------------------------------
*/
#define UNDEFINED   -1
#define ZERO_MATRIX 0
#define	UNIT_MATRIX 1

#ifdef __cplusplus
#define FOR_CXX		extern "C"
#else
#define FOR_CXX
#endif

#ifdef _WIN32
#  ifdef MATRIX_EXPORTS
#    define MATRIX_API FOR_CXX __declspec(dllexport)
#  else
#    define MATRIX_API FOR_CXX __declspec(dllimport)
#  endif
#else
#  define MATRIX_API
#endif


/* prototypes of matrix package */
MATRIX_API MATRIX      mat_error (int);
MATRIX_API MATRIX      _mat_creat (int, int);
MATRIX_API MATRIX      mat_creat (int, int, int);
MATRIX_API MATRIX      mat_fill (MATRIX, int);
MATRIX_API int         mat_free (MATRIX);
MATRIX_API MATRIX      mat_copy (MATRIX);
MATRIX_API MATRIX      mat_colcopy1 (MATRIX, MATRIX, int, int);
MATRIX_API int         fgetmat (MATRIX, FILE *);
MATRIX_API MATRIX      mat_dump (MATRIX);
MATRIX_API MATRIX      mat_dumpf (MATRIX, char *);
MATRIX_API MATRIX      mat_fdump (MATRIX, FILE *);
MATRIX_API MATRIX      mat_fdumpf (MATRIX, char *, FILE *);

MATRIX_API MATRIX      mat_add (MATRIX, MATRIX);
MATRIX_API MATRIX      mat_sub (MATRIX, MATRIX);
MATRIX_API MATRIX      mat_mul (MATRIX, MATRIX);
MATRIX_API double      mat_diagmul (MATRIX);
MATRIX_API MATRIX      mat_tran (MATRIX);
MATRIX_API MATRIX      mat_inv (MATRIX);
MATRIX_API MATRIX      mat_SymToeplz (MATRIX);

MATRIX_API int         mat_lu (MATRIX, MATRIX);
MATRIX_API MATRIX      mat_backsubs1 (MATRIX, MATRIX, MATRIX, MATRIX, int);
MATRIX_API MATRIX      mat_lsolve (MATRIX, MATRIX);

MATRIX_API MATRIX      mat_submat (MATRIX, int, int);
MATRIX_API double      mat_cofact (MATRIX, int, int);
MATRIX_API double      mat_det (MATRIX);
MATRIX_API double      mat_minor (MATRIX, int, int);

MATRIX_API MATRIX      mat_durbin (MATRIX);
MATRIX_API MATRIX      mat_lsolve_durbin (MATRIX, MATRIX);


#endif /* __matrix_h */

