/*****************************************************************
 *
 * File........:	svd_inversion.c
 * Function....:	matrix inversion via SVD
 * Author......:	Tilo Strutz
 * last changes:	18.01.2010
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
//#include "functions.h"
#include "leastSquaresPred.h"

/*---------------------------------------------------------------
 *	svd_inversion()
 *
 *--------------------------------------------------------------*/
int
svd_inversion( int M, double **normal, double **normal_i, FILE *out)
{
//	char *rtn = "svd_inversion";
	int i, j, err = 0;
	double thresh, smax;
	double **tmpmat = NULL;	/* temporary matrix */
	double *s = NULL;	/* singular values */
	double **V = NULL;	/* V matrix */

		V = matrix( M, M);	/* V matrix for SVD */
		s = vector( M);	/* singular values for SVD */
		tmpmat = matrix( M, M);	/* temporary matrix */

		err = singvaldec( normal, M, M, s, V);
		if (err)
		{
				free_matrix( &V);
				free_vector( &s);
				free_matrix( &tmpmat);
				goto endfunc;
		}

		smax = 0.0;
		for (j = 0; j < M; j++)
		{
			if (s[j] > smax)					smax = s[j];
		}
		if (smax < TOL_S2)
		{
			fprintf( stderr, 
				"\r### singular matrix, smax = %.4f",smax);
			fprintf( out, 
				"\r### singular matrix, smax = %.4f",smax);

			err = 1;
			goto endfunc;
		}
		else if (smax > 1.e+31)
		{
			fprintf( stderr, 
				"\n###\n### degraded matrix, smax = huge");
			fprintf( out, 
				"\n###\n### degraded matrix, smax = huge");
			err = 1;
			goto endfunc;
		}

		thresh = MIN( TOL_S * smax, TOL_S);

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
		/* get inverse of normal by multiplication of tmpmat with
		   transposed of normal */
		multmatsqT( M, normal_i, tmpmat, normal);


endfunc:
	free_vector( &s);
	free_matrix( &V);
	free_matrix( &tmpmat);

	return err;
}
