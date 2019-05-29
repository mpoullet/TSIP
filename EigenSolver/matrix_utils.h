/*****************************************************************
 *
 * File........:	matrix_utils.h
 * Function....:	special functions (prototyping)
 * Author......:	Tilo Strutz
 * last changes: 20.10.2007, 29.3.2011
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 *****************************************************************/

#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

int *ivector( long N);
unsigned int *uivector( long N);
float *fvector( long N);
double *vector( long N);
double **matrix( long M, long N);
float **fmatrix( long N, long M);

void free_ivector( int *v[]);
void free_uivector( unsigned int *v[]);
void free_vector( double *v[]);
void free_matrix( double **m[]);
void free_fmatrix( float **m[]);


double determinant_2x2( double **a);
double determinant_3x3( double **a);
double inverse_4x4( double **a, double **b);
double inverse_5x5( double **a, double **b);
void coFactor_2x2( double **a, double **b);
void coFactor_3x3( double **a, double **b);

void multmatsq( int M, double **a, double **b, double **c);
void multmatsqT( int N, double **a, double **b, double **c);

#endif
