/*******************************************************************
 *
 * File....: matrix.h
 * Function: Definition of a matrix structure and prototyping of
 *           functions for matrix operations.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#ifndef MATRIX_H_
#define MATRIX_H_

#include <math.h>
#include "types.h"

/* matrix structure */
typedef struct {
  float **X;   /* 2-dimensional array containing the matrix values */
  unsigned int
		rows,    /* row count */
    columns; /* column count */
} MATRIX;

/* Constructor & Destructor */
int newMatrix( MATRIX **m, UINT rows, UINT columns);
void freeMatrix( MATRIX *M);

/* matrix inversion */
int matrixInv( MATRIX *A, MATRIX *Inv);

/* matrix multiplication (X = A * B) */
int matrixMul( MATRIX *, MATRIX *, BOOL tpA, BOOL tpB, MATRIX *);

/* output on stdout */
void matrixPrint( MATRIX *);

#endif