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
extern "C" {
#endif /* __cplusplus */

/* prototypes of matrix package */
MATRIX      mat_error (int);
MATRIX      _mat_creat (int, int);
MATRIX      mat_creat (int, int, int);
MATRIX      mat_fill (MATRIX, int);
int         mat_free (MATRIX);
MATRIX      mat_copy (MATRIX);
MATRIX      mat_colcopy1 (MATRIX, MATRIX, int, int);
int         fgetmat (MATRIX, FILE *);
MATRIX      mat_dump (MATRIX);
MATRIX      mat_dumpf (MATRIX, char *);
MATRIX      mat_fdump (MATRIX, FILE *);
MATRIX      mat_fdumpf (MATRIX, char *, FILE *);

MATRIX      mat_add (MATRIX, MATRIX);
MATRIX      mat_sub (MATRIX, MATRIX);
MATRIX      mat_mul (MATRIX, MATRIX);
double      mat_diagmul (MATRIX);
MATRIX      mat_tran (MATRIX);
MATRIX      mat_inv (MATRIX);
MATRIX      mat_SymToeplz (MATRIX);

int         mat_lu (MATRIX, MATRIX);
MATRIX      mat_backsubs1 (MATRIX, MATRIX, MATRIX, MATRIX, int);
MATRIX      mat_lsolve (MATRIX, MATRIX);

MATRIX      mat_submat (MATRIX, int, int);
double      mat_cofact (MATRIX, int, int);
double      mat_det (MATRIX);
double      mat_minor (MATRIX, int, int);

MATRIX      mat_durbin (MATRIX);
MATRIX      mat_lsolve_durbin (MATRIX, MATRIX);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* __matrix_h */

