/*****************************************************************
*
* File........:	ls_lin.c
* Function....:	least squares with alternative matrix inversion
* Author......:	Tilo Strutz
* last changes:	17.08.2017
* 
* LICENCE DETAILS: see software manual
*	free academic use
*  cite source as 
* "Strutz, T.: Data Fitting and Uncertainty. 2nd edition,
*   Springer Vieweg, 2016"
*
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include "errmsg.h"
#include "matrix_utils.h"
//#include "defines.h"
#include "macros.h"
//#include "prototypes.h"
//#include "functions.h"
#include "leastSquaresPred.h"

#ifndef WIN32
#include <sys/time.h>
#else
#include <time.h>
#define random rand
#endif

long ITERAT_MAX;
double NU_FAC;
/*C2TEXfilter_begin*/
//#define QDEBUG
/*C2TEXfilter_end*/

/*---------------------------------------------------------------
*	ls_lin()
*
*--------------------------------------------------------------*/
int
ls_lin(  int N, int M, double *obs, double **jacob,
	 /*double *weights, */double *a, FILE *out)
{
	char *rtn = "ls";
	int err = 0, i, j, n;
	double **normal = NULL;			/* N = J^(T) * W * J	*/
	double **normal_i = NULL;		/* inverse of N				*/
	double *tmpvec = NULL;			/* J^(T) * W * r			*/
	double *deltasq = NULL;	/* = w * [obs - f(x|a) ] ^2 */
#ifdef CHECK_VARIANCE
	int Nfree;
	double *datac = NULL;	/* calculated values based on parameters */
	double chisq, tmp, variance;
#endif											

	/* 
	* allocate memory
	*/

	/* normal matrix N = J^(T) * W * J, its inverse */
	normal = matrix( M, M);
	normal_i = matrix( M, M);

	tmpvec = vector( M);		/* container for J^(T) * W * G	*/
	deltasq = vector( N);		/* remaining differences	*/
#ifdef CHECK_VARIANCE
	datac  = vector( N);		/* calculated data using f(x|a) */
#endif

	/* compute J'*W*J	*/
	for (j = 0; j < M; j++)
	{
		for (i = j; i < M; i++)
		{
			normal[j][i] = 0.;
			for (n = 0; n < N; n++)
			{
				normal[j][i] += jacob[n][j] * jacob[n][i] /** weights[n]*/;
			}
			normal[i][j] = normal[j][i]; /* matrix is symmetric	*/ 
			/* overflow test	*/
			if (!IsFiniteNumber( normal[j][i]))
			{
				err = errmsg( ERR_IS_INFINITE, rtn, "normal", 0);
				goto endfunc;
			}
		}
		if (errno)
		{
			perror( "\n### ");
			fprintf( stderr, "     errno = %d", errno);
			fprintf( out, "\n Error in computation (%d), ", errno);
			fprintf( out, "see standard output (console)\n");
			err = errno;
			errno = 0;
//			goto endfunc;
		}
	}

	/* tmpvec = J^(T) * W * y */
	for (j = 0; j < M; j++)
	{
		tmpvec[j] = 0.;
		for (n = 0; n < N; n++)
		{
			tmpvec[j] += jacob[n][j] * obs[n] /** weights[n]*/;
		}
	}
//if(tmpvec[0] == 0)
//{
//	int z=0;
//}

	/* 
	** inversion of normal matrix
	*/

	err = svd_inversion( M, normal, normal_i, out);
	if (err)
	{
		goto endfunc;
	}	 /* algo_mode SVD */

	/* final matrix multiplication to get parameters
	** a = inv(J'WJ) * J'Wy
	*/
	for (j = 0; j < M; j++)
	{
		a[j] = 0.;
		for (i = 0; i < M; i++)
		{
			a[j] += normal_i[j][i] * tmpvec[i];
		}
	}
	if (errno)
	{
		perror( "\n### ");
		fprintf( stderr, "     errno = %d", errno);
		fprintf( out, "\n Error in computation (%d), ", errno);
		fprintf( out, "see standard output (console)\n");
		err = errno;
		errno = 0;
		goto endfunc;
	}
#ifdef CHECK_VARIANCE
	/* compute weighted and squared differences chi-squared */
	chisq = 0.0;
	Nfree = -M;	/* reduce by number of parameters */
	//if (ls_flag->linear)
	{
		for (i = 0; i < N; i++)
		{
			/* get calculated data points dependent on current
			parameters */
			datac[i] = 0.0;
			for (j = 0; j < M; j++)
			{
				datac[i] += a[j] * jacob[i][j];
			}
			tmp = fabs( obs[i] - datac[i]);
			/* weighted and squared differences */
			deltasq[i] = tmp * tmp;
			chisq += weights[i] * deltasq[i];
			if (weights[i] > 0.)
				Nfree++;
		}
	}

	variance = chisq / (double)Nfree;
#endif

endfunc:

#ifdef CHECK_VARIANCE
	free_vector( &datac);
#endif
	free_vector( &tmpvec);
	free_vector( &deltasq);
	free_matrix( &normal);
	free_matrix( &normal_i);

	return err;
}
