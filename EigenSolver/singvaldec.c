/*****************************************************************
 *
 * File........:	singvaldec.c
 * Function....:	singular value decomposition
 * Author......:	Tilo Strutz
 * last changes:	20.10.2007
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "matrix_utils.h"
#include "macros.h"

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))


/*---------------------------------------------------------------
 *	euclid_dist()
 *--------------------------------------------------------------*/
double
euclid_dist( double a, double b)
{
	double abs_a, abs_b, val, dval;

	abs_a = fabs( a);
	abs_b = fabs( b);
	if (abs_a > abs_b)
	{
		dval = abs_b / abs_a;
		val = abs_a * sqrt( 1.0 + dval * dval);
		return val;
	}
	else
	{
		if (abs_b == 0.0)
			return 0.0;
		else
		{
			dval = abs_a / abs_b;
			val = abs_b * sqrt( 1.0 + dval * dval);
			return val;
		}
	}
}

/*---------------------------------------------------------------
 *	singvaldec()
 *  singular value decomposition
 * translation from http://www.pdas.com/programs/fmm.f90
 *--------------------------------------------------------------*/
int
singvaldec( double **a, /* matrix to be decomposed */
					 int N, /* number of lines */
					 int M, /* number of columns	*/
					 double w[], double **v)
{
	char *rtn = "singvaldec";
	int err = 0;
	int flag, i, its, j, jj, k, l = 0, nm = 0;
	double anorm, c, f, g, h, s, scale, x, y, z, *rv1;

	rv1 = vector( M);

	/*
	 * housholder reduction to bidiagonal form
	 */
	g = scale = anorm = 0.0;
	for (i = 0; i < M; i++)
	{
		l = i + 1;
		assert( i >= 0);
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < N)
		{
			for (k = i; k < N; k++)
			{
				scale += fabs( a[k][i]);
			}
			if (scale)
			{
				for (k = i; k < N; k++)
				{
					a[k][i] /= scale;
					s += a[k][i] * a[k][i];
				}
				f = a[i][i];
				assert( s>= 0);
				g = -SIGN( sqrt( s), f);
				h = f * g - s;
				a[i][i] = f - g;
				for (j = l; j < M; j++)
				{
					s = 0.0;
					for (k = i; k < N; k++)
					{
						s += a[k][i] * a[k][j];
					}
					f = s / h;
					for (k = i; k < N; k++)
					{
						a[k][j] += f * a[k][i];
					}
				}
				for (k = i; k < N; k++)
					a[k][i] *= scale;
			}
		}
		w[i] = scale * g;
		g = s = scale = 0.0;
		if (i < N && i != M - 1)
		{
			for (k = l; k < M; k++)
			{
				scale += fabs( a[i][k]);
			}
			if (scale)
			{
				for (k = l; k < M; k++)
				{
					a[i][k] /= scale;
					s += a[i][k] * a[i][k];
				}
				f = a[i][l];
				assert( s>= 0);
				g = -SIGN( sqrt( s), f);
				h = f * g - s;
				a[i][l] = f - g;
				for (k = l; k < M; k++)
				{
					assert( k >= 0);
					rv1[k] = a[i][k] / h;
				}
				for (j = l; j < N; j++)
				{
					for (s = 0.0, k = l; k < M; k++)
						s += a[j][k] * a[i][k];
					for (k = l; k < M; k++)
						a[j][k] += s * rv1[k];
				}
				for (k = l; k < M; k++)
					a[i][k] *= scale;
			}
		}
		anorm = MAX( anorm, (fabs( w[i]) + fabs( rv1[i])));
	}

	/*
	 * accumulation of right-hand transformations
	 */
	for (i = M - 1; i >= 0; i--)
	{
		if (i < M - 1)
		{
			if (g)
			{
				for (j = l; j < M; j++)
				{
					v[j][i] = (a[i][j] / a[i][l]) / g;
				}
				for (j = l; j < M; j++)
				{
					for (s = 0.0, k = l; k < M; k++)
						s += a[i][k] * v[k][j];
					for (k = l; k < M; k++)
						v[k][j] += s * v[k][i];
				}
			}
			for (j = l; j < M; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		assert( i >= 0);
		g = rv1[i];
		l = i;
	}

	/*
	 * accumulation of left-hand transformations
	 */
	for (i = MIN( N, M) - 1; i >= 0; i--)
	{
		l = i + 1;
		g = w[i];
		for (j = l; j < M; j++)
			a[i][j] = 0.0;
		if (g)
		{
			g = 1.0 / g;
			for (j = l; j < M; j++)
			{
				for (s = 0.0, k = l; k < N; k++)
					s += a[k][i] * a[k][j];
				f = (s / a[i][i]) * g;
				for (k = i; k < N; k++)
					a[k][j] += f * a[k][i];
			}
			for (j = i; j < N; j++)
				a[j][i] *= g;
		}
		else
		{
			for (j = i; j < N; j++)
				a[j][i] = 0.0;
		}
		++a[i][i];
	}

	/*
	 * diagonalization of the bidiagonal form
	 */
	for (k = M - 1; k >= 0; k--)	/* loop over singular values */
	{
		assert( k >= 0);
		for (its = 0; its < 30; its++)	/* loop over allowed
																			   iterations */
		{
			flag = 1;
			for (l = k; l >= 0; l--)	/* test for splitting */
			{
				nm = l - 1;	/* note that rv1[0] is always zero */
				if (( double)( fabs( rv1[l]) + anorm) == anorm)
				{
					flag = 0;
					break;
				}
				if (( double)( fabs( w[nm]) + anorm) == anorm)
					break;
			}
			if (flag)
			{
				assert( l >= 0);
				c = 0.0;	/* cancellation of rv1[l] if l greater than 1 */
				s = 1.0;
				for (i = l; i < k; i++)
				{
					f = s * rv1[i];
					rv1[i] = c * rv1[i];
					if (( double)( fabs( f) + anorm) == anorm)
						break;
					g = w[i];
					h = euclid_dist( f, g);
					w[i] = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 0; j < N; j++)
					{
						y = a[j][nm];
						z = a[j][i];
						a[j][nm] = y * c + z * s;
						a[j][i] = z * c - y * s;
					}
				}
			}
			/* test for convergence */
			z = w[k];
			if (l == k)
			{
				if (z < 0.0)	/* singular value is made non-negative */
				{
					w[k] = -z;
					for (j = 0; j < M; j++)
						v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 30)
			{
				fprintf( stderr,
					"\n%s: No convergence after 30 iterations(SVD)\n", rtn);
				err = 57;
				goto endfunc;
			}
			/* shift from bottom 2 by 2 minor */
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
				assert( k >= 0);
			h = rv1[k];
			f =
				( (y - z) * (y + z) + (g - h) * (g +
				h)) / (2.0 * h * y);
			g = euclid_dist( f, 1.0);
			f =
				( (x - z) * (x + z) + h * (( y / (f + SIGN( g,
				f))) - h)) / x;

			/* next qr transformation */
			c = s = 1.0;
			for (j = l; j <= nm; j++)
			{
				i = j + 1;
				assert( i >= 0);
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = euclid_dist( f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for (jj = 0; jj < M; jj++)
				{
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x * c + z * s;
					v[jj][i] = z * c - x * s;
				}
				z = euclid_dist( f, h);
				w[j] = z;
				if (z)	/* rotation can be arbitrary if z is zero */
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for (jj = 0; jj < N; jj++)
				{
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y * c + z * s;
					a[jj][i] = z * c - y * s;
				}
			}
			assert( l >= 0);
			assert( k >= 0);
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}

endfunc:	
	free_vector( &rv1);
	return err;
}
