/*****************************************************
* File..: get_colour_variance.c
* Desc.: determination of eigenvalues of RGB space
* Author: Tilo Strutz
* Date.: 04.04.2014
******************************************************/
#include <math.h>
#include <string.h>
#include <assert.h>

//#include "stats.h"
#include "ginterface.h"
#include "imageFeatures.h"
//#include "colour4.h"
//#include "paeth.h"
//#include "matrix_utils.h"
//#include "prototypes.h"
//#include "eig3.h"
//#include "codec.h"	/* needed for max()	*/

#ifndef M_PI
	#define M_PI        3.14159265358979323846
#endif

#include "eig3.h"


/*---------------------------------------------------------
 * get_colour_variance()
 *---------------------------------------------------------*/
void get_colour_variance( PicData *pd, ImageFeatures *iF)
{
	unsigned long i;
	double d[3], A[3][3], V[3][3];	/* eigen values and vectore*/
	double diffR, diffG, diffB;
	double meanR, meanG, meanB;
	double var_RR, var_RG, var_RB;
	double var_GR, var_GG, var_GB;
	double var_BR, var_BG, var_BB;
	unsigned long	sum_R, sum_G, sum_B;

	if (pd->channel < 3)
	{
		iF->rgbStdDev[0] = 0;
		iF->rgbStdDev[1] = 0;
		/* compute mean values	*/
		sum_R = 0;
		for ( i = 0; i < pd->size; i++) /* all pixels	*/
		{
			sum_R += pd->data[0][i]; 
		}
		meanR = (double)sum_R / pd->size;

		/* compute (co-)variances	*/
		var_RR = 0;
		for ( i = 0; i < pd->size; i++) /* all pixels	*/
		{
			diffR = pd->data[0][i] - meanR; 
			var_RR += diffR * diffR; 
		}
		var_RR /= pd->size;
		iF->rgbStdDev[2] = (unsigned int)( 2 * sqrt( var_RR));
		return;
	}


	/* compute mean values	*/
	sum_R = sum_G = sum_B = 0;
	for ( i = 0; i < pd->size; i++) /* all pixels	*/
	{
		sum_R += pd->data[0][i]; 
		sum_G += pd->data[1][i]; 
		sum_B += pd->data[2][i]; 
	}
	meanR = (double)sum_R / pd->size;
	meanG = (double)sum_G / pd->size;
	meanB = (double)sum_B / pd->size;

	/* compute (co-)variances	*/
	var_RR = var_RG = var_RB = 0;
	var_GR = var_GG = var_GB = 0;
	var_BR = var_BG = var_BB = 0;
	for ( i = 0; i < pd->size; i++) /* all pixels	*/
	{
		diffR = pd->data[0][i] - meanR; 
		diffG = pd->data[1][i] - meanG; 
		diffB = pd->data[2][i] - meanB; 
		var_RR += diffR * diffR; 
		var_RG += diffR * diffG; 
		var_RB += diffR * diffB; 
		var_GG += diffG * diffG; 
		var_GB += diffB * diffG; 
		var_BB += diffB * diffB; 
	}
	/* co-variances
   *  RR  RG  RB
	 *  RG  GG  GB
	 *  RB  GB  BB
	 */
	var_RR /= pd->size;
	var_RG /= pd->size;
	var_RB /= pd->size;
	var_GG /= pd->size;
	var_GB /= pd->size;
	var_BB /= pd->size;

	/* eigenvalues: 2, 1, -1
	 * vectors: (1 1 -1); 
	 */
	//var_RR = 0;
	//var_RG = 1;
	//var_RB = -1;
	//var_GG = 1;
	//var_GB = 0;
	//var_BB = 1;

	/* determinant of
	 * a-l b    c  
	 * b   d-l  e
   * c   e    f-l
	 *
	 * (a-l)(d-l)(f-l) + bce*2 - [(a-l)*e*e + (f-l)*b*b + (d-l) * c*c ] = 0
	 * [(ad +ll -l(a+d)](f-l)  + bce*2 - [ aee + fbb + dcc -l*(bb + cc + ee)] = 0
	 * adf + llf - lf(a+d) - lad - lll + ll(a+d) + bce*2 - aee - fbb - dcc + l*(bb + cc + ee) = 0
	 * lll + ll*(f+a+d) + l*[bb + cc + ee - ad - f*(a+d)]  + adf + bce*2 - aee - fbb - dcc = 0  



	/* 
	 * determine the eigenvectors
	 *
	 * (A - lambda_i*I)*a_i = 0
	 * in LS: J*a = y
	 * eigenvectors = columns of V
	 */

	A[0][0] = var_RR;
	A[0][1] = A[1][0] = var_RG;
	A[0][2] = A[2][0] = var_RB;
	A[1][1] = var_GG;
	A[1][2] = A[2][1] = var_GB;
	A[2][2] = var_BB;
	eigen_decomposition( A, V, d);
	iF->rgbStdDev[0] = (unsigned int)( 2 * sqrt( d[0]));
	iF->rgbStdDev[1] = (unsigned int)( 2 * sqrt( d[1]));
	iF->rgbStdDev[2] = (unsigned int)( 2 * sqrt( d[2]));

#ifdef NIXX
	/* normalisation of eigenvectors	*/
  /* assume d[2] as being the largest eigenvalue */

	sum = fabs( V[0][2]) + fabs( V[1][2]) + fabs( V[2][2]);
	V[0][2] /= sum;
	V[1][2] /= sum;
	V[2][2] /= sum;

	sum = fabs( V[0][1]) + fabs( V[1][1]) + fabs( V[2][1]);
	V[0][1] /= sum;
	V[1][1] /= sum;
	V[2][1] /= sum;

	sum = fabs( V[0][0]) + fabs( V[1][0]) + fabs( V[2][0]);
	V[0][0] /= sum;
	V[1][0] /= sum;
	V[2][0] /= sum;

	/* identify Y component */
	sum = V[0][2] + V[1][2] + V[2][2];
	if (sum > 0.9)
	{
		CT[0][0] = V[0][2];
		CT[0][1] = V[1][2];
		CT[0][2] = V[2][2];
		eigval[0] = d[2];

		CT[1][0] = V[0][1] * 2;
		CT[1][1] = V[1][1] * 2;
		CT[1][2] = V[2][1] * 2;
		eigval[1] = d[1];

		CT[2][0] = V[0][0] * 2;
		CT[2][1] = V[1][0] * 2;
		CT[2][2] = V[2][0] * 2;
		eigval[2] = d[0];
	}
	else 
	{
		sum = V[0][1] + V[1][1] + V[2][1];
		if (sum > 0.9)
		{
			CT[0][0] = V[0][1];
			CT[0][1] = V[1][1];
			CT[0][2] = V[2][1];
			eigval[0] = d[1];

			CT[1][0] = V[0][2] * 2;
			CT[1][1] = V[1][2] * 2;
			CT[1][2] = V[2][2] * 2;
			eigval[1] = d[2];

			CT[2][0] = V[0][0] * 2;
			CT[2][1] = V[1][0] * 2;
			CT[2][2] = V[2][0] * 2;
			eigval[2] = d[0];
		}
		else
		{
			CT[0][0] = V[0][0];
			CT[0][1] = V[1][0];
			CT[0][2] = V[2][0];
			eigval[0] = d[0];

			CT[1][0] = V[0][2] * 2;
			CT[1][1] = V[1][2] * 2;
			CT[1][2] = V[2][2] * 2;
			eigval[1] = d[2];

			CT[2][0] = V[0][1] * 2;
			CT[2][1] = V[1][1] * 2;
			CT[2][2] = V[2][1] * 2;
			eigval[2] = d[1];
		}
	}
	/* exchange U and V, if neccessary */
	if ( fabs( CT[1][0]) < fabs( CT[2][0]) )
	{
		double tmp[3];

		tmp[0] = CT[1][0];
		tmp[1] = CT[1][1];
		tmp[2] = CT[1][2];
		CT[1][0] = CT[2][0];
		CT[1][1] = CT[2][1];
		CT[1][2] = CT[2][2];
		CT[2][0] = tmp[0];
		CT[2][1] = tmp[1];
		CT[2][2] = tmp[2];
	}
	/* invert direction of vector, if necessary	*/
	if ( CT[1][0] < 0.)
	{
		CT[1][0] = -CT[1][0];
		CT[1][1] = -CT[1][1];
		CT[1][2] = -CT[1][2];
	}
#endif
}

#ifdef NIXX
/*---------------------------------------------------------
 * get_colour_variance_2()
 *---------------------------------------------------------*/
int get_colour_variance_2( PicData *pd, ImageFeatures *iF)
{
	int flags[3][256];
	unsigned int cnts[3] = { 0, 0, 0};
	unsigned long i;
	double m, p, q,  phi;
	double eig1, eig2, eig3;
	double sum;
	double d[3], A[3][3], V[3][3];	/* eigen values and vectore*/
	double CT[3][3]; /* matrix for optimal colour transform	*/
	double eigval[3];
	double diffR, diffG, diffB;
	double meanR, meanG, meanB;
	double var_RR, var_RG, var_RB;
	double var_GR, var_GG, var_GB;
	double var_BR, var_BG, var_BB;
	double var_RR_reduced, var_GG_reduced, var_BB_reduced;
	unsigned long	sum_R, sum_G, sum_B;

	/* compute mean values	*/
	sum_R = sum_G = sum_B = 0;

		memset( flags[0], 0, 256);
		memset( flags[1], 0, 256);
		memset( flags[2], 0, 256);
		for ( i = 0; i < pd->size; i++) /* all pixels	*/
		{
			if (flags[0][ pd->data[0][i]] == 0)
			{
				flags[0][ pd->data[0][i]] = 1;
				sum_R += pd->data[0][i]; 
				cnts[0]++;
			}
			if (flags[1][ pd->data[1][i]] == 0)
			{
				flags[1][ pd->data[1][i]] = 1;
				sum_G += pd->data[1][i]; 
				cnts[1]++;
			}
			if (flags[2][ pd->data[2][i]] == 0)
			{
				flags[2][ pd->data[2][i]] = 1;
				sum_B += pd->data[2][i]; 
				cnts[2]++;
			}
		}
		meanR = (double)sum_R / cnts[0];
		meanG = (double)sum_G / cnts[1];
		meanB = (double)sum_B / cnts[2];

	/* compute (co-)variances	*/
	var_RR = var_RG = var_RB = 0;
	var_GR = var_GG = var_GB = 0;
	var_BR = var_BG = var_BB = 0;
	for ( i = 0; i < pd->size; i++) /* all pixels	*/
	{
		diffR = pd->data[0][i] - meanR; 
		diffG = pd->data[1][i] - meanG; 
		diffB = pd->data[2][i] - meanB; 
		var_RR += diffR * diffR; 
		var_RG += diffR * diffG; 
		var_RB += diffR * diffB; 
		var_GG += diffG * diffG; 
		var_GB += diffB * diffG; 
		var_BB += diffB * diffB; 
	}
	/* co-variances
   *  RR  RG  RB
	 *  RG  GG  GB
	 *  RB  GB  BB
	 */
	var_RR /= pd->size;
	var_RG /= pd->size;
	var_RB /= pd->size;
	var_GG /= pd->size;
	var_GB /= pd->size;
	var_BB /= pd->size;

	if (0)
	{
	/* eigenvalues: 2, 1, -1
	 * vectors: (1 1 -1); 
	 */
	//var_RR = 0;
	//var_RG = 1;
	//var_RB = -1;
	//var_GG = 1;
	//var_GB = 0;
	//var_BB = 1;

	// Given symmetric 3x3 matrix M, compute the eigenvalues
	m = (var_RR + var_GG + var_BB)/3;
	// K = M-m*eye(3);
	var_RR_reduced = var_RR - m;
	var_GG_reduced = var_GG - m;
	var_BB_reduced = var_BB - m;
	// q = det(K)/2;
	q = (var_RR_reduced * var_GG_reduced * var_BB_reduced +
	    var_RG * var_GB * var_RB +
			var_RB * var_RG * var_GB -
			var_RR_reduced * var_GB * var_GB -
			var_RG * var_RG * var_BB_reduced -
			var_RB * var_GG_reduced * var_RB
			) / 2;
	p = M_PI;
	//for i=1:3
	//		for j=1:3
	//				p = p + K(i,j)^2;
	p = (
			var_RR_reduced * var_RR_reduced + var_RG*var_RG + var_RB*var_RB +
			var_GG_reduced * var_GG_reduced + var_RG*var_RG + var_GB*var_GB + 
			var_BB_reduced * var_BB_reduced + var_RB*var_RB + var_GB*var_GB
			) / 6;
	 
	phi = 1. / 3. * acos( q / pow( p,(3./2.)));
 
	/* NOTE: the follow formula assume accurate computation and 
	 * therefor q/p^(3/2) should be in range of [1,-1], 
	 * but in real code, because of numerical errors, it must be checked. 
	 * Thus, in case abs(q) >= abs(p^(3/2)), set phi = 0;
	 */
	if (fabs( q) >= fabs( pow(p, (3./2.)))) 		phi = 0;
	if ( phi < 0 ) phi = phi + M_PI / 3;
 
	eig1 = m + 2 * sqrt( p) * cos( phi);
	eig2 = m - sqrt( p) * (cos( phi) + sqrt(3.) * sin( phi));
	eig3 = m - sqrt( p) * (cos( phi) - sqrt(3.) * sin( phi));
	}


	/* 
	 * determine the eigenvectors
	 *
	 * (A - lambda_i*I)*a_i = 0
	 * in LS: J*a = y
	 * eigenvectors = columns of V
	 */

	A[0][0] = var_RR;
	A[0][1] = A[1][0] = var_RG;
	A[0][2] = A[2][0] = var_RB;
	A[1][1] = var_GG;
	A[1][2] = A[2][1] = var_GB;
	A[2][2] = var_BB;
	eigen_decomposition( A, V, d);

	/* normalisation of eigenvectors	*/
  /* assume d[2] as being the largest eigenvalue */

	sum = fabs( V[0][2]) + fabs( V[1][2]) + fabs( V[2][2]);
	V[0][2] /= sum;
	V[1][2] /= sum;
	V[2][2] /= sum;

	sum = fabs( V[0][1]) + fabs( V[1][1]) + fabs( V[2][1]);
	V[0][1] /= sum;
	V[1][1] /= sum;
	V[2][1] /= sum;

	sum = fabs( V[0][0]) + fabs( V[1][0]) + fabs( V[2][0]);
	V[0][0] /= sum;
	V[1][0] /= sum;
	V[2][0] /= sum;

	/* identify Y component */
	sum = V[0][2] + V[1][2] + V[2][2];
	if (sum > 0.9)
	{
		CT[0][0] = V[0][2];
		CT[0][1] = V[1][2];
		CT[0][2] = V[2][2];
		eigval[0] = d[2];

		CT[1][0] = V[0][1] * 2;
		CT[1][1] = V[1][1] * 2;
		CT[1][2] = V[2][1] * 2;
		eigval[1] = d[1];

		CT[2][0] = V[0][0] * 2;
		CT[2][1] = V[1][0] * 2;
		CT[2][2] = V[2][0] * 2;
		eigval[2] = d[0];
	}
	else 
	{
		sum = V[0][1] + V[1][1] + V[2][1];
		if (sum > 0.9)
		{
			CT[0][0] = V[0][1];
			CT[0][1] = V[1][1];
			CT[0][2] = V[2][1];
			eigval[0] = d[1];

			CT[1][0] = V[0][2] * 2;
			CT[1][1] = V[1][2] * 2;
			CT[1][2] = V[2][2] * 2;
			eigval[1] = d[2];

			CT[2][0] = V[0][0] * 2;
			CT[2][1] = V[1][0] * 2;
			CT[2][2] = V[2][0] * 2;
			eigval[2] = d[0];
		}
		else
		{
			CT[0][0] = V[0][0];
			CT[0][1] = V[1][0];
			CT[0][2] = V[2][0];
			eigval[0] = d[0];

			CT[1][0] = V[0][2] * 2;
			CT[1][1] = V[1][2] * 2;
			CT[1][2] = V[2][2] * 2;
			eigval[1] = d[2];

			CT[2][0] = V[0][1] * 2;
			CT[2][1] = V[1][1] * 2;
			CT[2][2] = V[2][1] * 2;
			eigval[2] = d[1];
		}
	}
	/* exchange U and V, if neccessary */
	if ( fabs( CT[1][0]) < fabs( CT[2][0]) )
	{
		double tmp[3];

		tmp[0] = CT[1][0];
		tmp[1] = CT[1][1];
		tmp[2] = CT[1][2];
		CT[1][0] = CT[2][0];
		CT[1][1] = CT[2][1];
		CT[1][2] = CT[2][2];
		CT[2][0] = tmp[0];
		CT[2][1] = tmp[1];
		CT[2][2] = tmp[2];
	}
	/* invert direction of vector, if necessary	*/
	if ( CT[1][0] < 0.)
	{
		CT[1][0] = -CT[1][0];
		CT[1][1] = -CT[1][1];
		CT[1][2] = -CT[1][2];
	}
	return 0;
}

#endif