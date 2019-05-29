/************************************************
 *
 * File..:	leastSquaresPred.h
 * Descr.:	Definitionen fuer least-squares prediction
 * Author:	Tilo Strutz
 * Datum: 	10.08.2017
 ************************************************/
#ifndef LSPRED_H
#define LSPRED_H

#include "ginterface.h"


#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif


#ifndef CLIP
#define CLIP( a, b, c) 	(a) > (c) ? (c) : (a) < (b) ? (b) : (a);
#endif

#ifndef MODULO_RANGE
#define MODULO_RANGE( e, l, u, r) 		\
	(e) < (l) ? (e) + (r) : 		\
		(e) > (u) ? (e) - (r) : (e);	
#endif


	/*     141115
	**   13 7 5 816
	** 12 6 2 1 3 917
	** 10 4 0 X
	*/
#define MAX_NUMOFPREDS 18	/* maximum number of predictors to be combined	*/
#define MAX_NUMOFPREDS_COL (18+13+13)	/* maximum number of predictors to be combined	*/
#define MIN_LS_SEARCHSPACE 2
#define MAX_LS_SEARCHSPACE 20
#define NUMOF_COL_LSPREDS	13	/* number of colour predictors */

/* defaults	*/
#define NUMOF_LSPREDS	12	/* default number of predictors to be combined	
													** is overwritten by the programm option
													*/
#define LS_SEARCHSPACE 11	/* default size of search space
													** is overwritten by the programm option
													*/

void lsZeroOrderPrediction( PicData *pd, int numOfPreds, int searchSpace,
									 int useCorrelation_flag,
									 int encode_flag);
void lsHighOrderPrediction( PicData *pd, int numOfPreds, int searchSpace,
									int encode_flag);
void lmsPrediction( PicData *pd, int numOfPreds, int searchSpace,
													 int encode_flag);
void lmsPredictionCol( PicData *pd, int numOfPreds, int searchSpace,
													 int encode_flag);
void lmsPredictionColP( PicData *pd, int numOfPreds, int searchSpace,
									 int encode_flag);

#ifdef COVAR_NEEDED
int
solve_lin( int N, int M, double *obs, double *weights,
							double **jacob, double **covar, double *a, 
							FILE *out)
#else
int
solve_lin( int N, int M, double *obs, double *weights,
							double **jacob, double *a, 
							FILE *out);
#endif

int
svd_inversion( int M, double **normal, double **normal_i, FILE *out);

int
singvaldec( double **a, /* matrix to be decomposed */
					 int N, /* number of lines */
					 int M, /* number of columns	*/
					 double w[], double **v);
int
ls_lin(  int N, int M, double *obs, double **jacob,
	 /* double *weights,*/ double *a, FILE *out);
int IsFiniteNumber(double x) ;

#endif
