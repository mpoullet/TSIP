/*****************************************************************
 *
 * File........:	solve_lin.c
 * Function....:	solving linear least squares via SVD
 * Author......:	Tilo Strutz
 * last changes:	25.01.2010
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
#include "errmsg.h"
#include "matrix_utils.h"
#include "macros.h"
//#include "prototypes.h"
#include "leastSquaresPred.h"

//#define QDEBUG 

/*---------------------------------------------------------------
 *	solve_lin()
 *
 *--------------------------------------------------------------*/
#ifdef COVAR_NEEDED
int
solve_lin( int N, int M, double *obs, double *weights,
							double **jacob, double **covar, double *a, 
							FILE *out)
#else
int
solve_lin( int N, int M, double *obs, double *weights,
							double **jacob, double *a, 
							FILE *out)
#endif
{
//	char *rtn = "solve_lin";
	int i, j, n, err = 0;
	double thresh, smax, sqrtwe;
	double **tmpmat = NULL;	/* temporary matrix */
	double **tmpmat2 = NULL;	/* temporary matrix */
	double *s = NULL;	/* singular values */
	double **V = NULL;	/* V matrix */
	double **WJ = NULL;	/* W*J matrix */

		V = matrix( M, M);	/* V matrix for SVD */
		s = vector( M);	/* singular values for SVD */
		WJ = matrix( N, M);	/* temporary matrix */
		tmpmat = matrix( M, M);	/* temporary matrix */
		tmpmat2 = matrix( M, N);	/* temporary matrix */

		/* WJ = sqrt(W)*J  */
		for (i = 0; i < N; i++)
		{
			sqrtwe = sqrt( weights[i]);
			for (j = 0; j < M; j++)
			{
				WJ[i][j] = sqrtwe * jacob[i][j];
			}
		}

#ifdef QDEBUG
		fprintf( out, "\n#\n#== Weight * Jacobian =================");
		for (i = 0; i < N; i++)
		{
			fprintf( out, "\n# ");
			for (j = 0; j < M; j++)
			{
				fprintf( out, " %8.5f", WJ[i][j]);
			}
		}
#endif

		/* do the SVD	*/
		err = singvaldec( WJ, N, M, s, V);
		if (err)
		{
				free_matrix( &V);
				free_vector( &s);
				free_matrix( &WJ);
				free_matrix( &tmpmat);
				free_matrix( &tmpmat2);
				goto endfunc;
		}

#ifdef QDEBUG
		fprintf( out, "\n#\n#== U =================");
		for (i = 0; i < N; i++)
		{
			fprintf( out, "\n# ");
			for (j = 0; j < M; j++)
			{
				fprintf( out, " %8.5f", WJ[i][j]);
			}
		}
		fprintf( out, "\n#\n#== V =================");
		for (i = 0; i < M; i++)
		{
			fprintf( out, "\n# ");
			for (j = 0; j < M; j++)
			{
				fprintf( out, " %8.5f", V[i][j]);
			}
		}
#endif

		/* check the singular values	*/
		smax = 0.0;
		for (j = 0; j < M; j++)
		{
			if (s[j] > smax)	smax = s[j];
		}
		if (smax < TOL_S)
		{
			fprintf( stderr, 
				"\n###\n###     singular matrix, smax = %f",smax);
			fprintf( out, 
				"\n###\n###     singular matrix, smax = %f",smax);
			for (j  = 0; j < M; j++) a[j] = 0;
			err = 1;
			goto endfunc;
		}
		else if (smax > 1.e+31)
		{
			fprintf( stderr, 
				"\n###\n###     degraded matrix, smax = huge");
			fprintf( out, 
				"\n###\n###     degraded matrix, smax = huge");
			for (j  = 0; j < M; j++) a[j] = 0;
			err = 1;
			goto endfunc;
		}

		thresh = MIN( TOL_S * smax, TOL_S);

		//fprintf( out, "\n#\n#  singular values (thresh = %.14G)\n#  ",
		//		thresh);
		//for (j = 0; j < M; j++)
		//{
		//		fprintf( out, "s%d=%.14G, ", j+1, s[j]);
		//}

		/* invert singular values */
		for (j = 0; j < M; j++)
		{
			/* <= in case of smax =0 */
			if (s[j] <= thresh)
					s[j] = 0.0;
			else
					s[j] = 1. / s[j];
		}

		/* V * [diag(1/s[j])] */
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < M; j++)
			{
				tmpmat[i][j] = V[i][j] * s[j];
			}
		}

#ifdef QDEBUG
		fprintf( out, "\n#\n#== V * inv(S) =================");
		for (i = 0; i < M; i++)
		{
			fprintf( out, "\n# ");
			for (j = 0; j < M; j++)
			{
				fprintf( out, " %8.5f", tmpmat[i][j]);
			}
		}
#endif

		/* multiplication of tmpmat with transposed of U  */
		/* result is: inv(W*J) = (V*inv(S)) * U'	*/
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < N; j++)
			{
				tmpmat2[i][j] = 0.;
				for (n = 0; n < M; n++)
				{
					tmpmat2[i][j] += tmpmat[i][n] * WJ[j][n];
				}
			}
		}

#ifdef QDEBUG
		fprintf( out, "\n#\n#== V * inv(S) * U' =================");
		for (i = 0; i < M; i++)
		{
			fprintf( out, "\n# ");
			for (j = 0; j < N; j++)
			{
				fprintf( out, " %8.5f", tmpmat2[i][j]);
			}
		}
#endif

		/* compute the parameter vector a = inv(W*J)*W*y */
		for (j = 0; j < M; j++)
		{
			a[j] = 0.0;
			for (i = 0; i < N; i++)
			{
				a[j] += tmpmat2[j][i] *  sqrt(weights[i]) * obs[i];
			}
		}

#ifdef COVAR_NEEDED
		/* compute covariance matrix V*S^(-2) */
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < M; j++)
			{
				tmpmat[i][j] = V[i][j] * s[j] * s[j];
			}
		}
		/* compute covariance matrix tmpmat * V' */
		multmatsqT( M, covar, tmpmat, V);
#endif

endfunc:
	free_vector( &s);
	free_matrix( &V);
	free_matrix( &tmpmat);
	free_matrix( &tmpmat2);
	free_matrix( &WJ);

	return err;
}
