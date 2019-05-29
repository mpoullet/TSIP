/*****************************************************************
 *
 * File..:	predictorLS.c
 * Descr.:	Calculation of estimation value using linear prediction
 *					 based on matched contexts
 * Author.:	Tilo Strutz
 * Date..: 	13.08.2018 copy from predictorR.c
 * changes:
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include "bitbuf.h"
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "predictor2.h"
#include "contexts.h"
#include "matrix_utils.h"
#include "leastSquaresPred.h"

#define PXLS_SEARCH_RANGE  30

/*---------------------------------------------------------------
 *	init_pred2Weights_L_d() at decoder = read coeffs
 *---------------------------------------------------------------*/
void init_pred2Weights_L_d( BitBuffer *bitbuf,
												 unsigned short num_contexts,
												 PRED2 *p_pred2)
{
	unsigned int px, p;
	long *lwL = NULL;
	float *wL = NULL;

	for ( px = 0; px < num_contexts; px++)
	{
		wL = p_pred2->weights_L[px];
		lwL = p_pred2->lweights_L[px];
		for ( p = 0; p < LS_TEMPLATE_SIZE; p++)
		{
			lwL[p] = rice_decoding( bitbuf, 11);
			if (lwL[p])
			{
				cbool sign;
				sign = BufferGetBit( bitbuf); /* get sign	*/
				if (sign) lwL[p] = - lwL[p];	/* assign to knot	*/
			}
			wL[p] = ( (float)lwL[p] / 1024.F);
		}
	}
}

/*---------------------------------------------------------------
 *	init_pred2Weights_L()
 *---------------------------------------------------------------*/
void init_pred2Weights_L( BitBuffer *bitbuf,
												 IMAGE *imExt, long *lptr, IMAGE *im, 
												 unsigned short num_contexts,
												 unsigned short *pxLS_ptr,
												 PRED2 *p_pred2)
{
	int err = 0;
	unsigned int i, r, c, p;
  unsigned short px;
	unsigned long py, pos;
	unsigned long numOfTargetSamples;
	unsigned long *countPx = NULL;
	long *lwL = NULL;
	float *wL = NULL;

	double **jacobi = NULL;	/* copy covariance matrix C	*/
	double *obs = NULL;	/* copy covariance matrix C	*/
	double fac[LS_TEMPLATE_SIZE]; //, facSum;
	double dummy;
	TEMPLATE templ_obj, *p_templ=NULL;	

	errno = 0;

	/* allocate memory for context counters	*/
	CALLOC( countPx, num_contexts, unsigned long);

	p_templ = &templ_obj;

	/* count contexts	*/
	for ( r = 0, py = OFF_TOP * imExt->width; r < im->height; r++, py += imExt->width)
	{
		for ( c = 0, pos = OFF_LEFT + py; c < im->width; c++, pos++)
		{
			/* copy neighbour values to template,
			* must always be performed, because it must fill the borders for next lines
			*/
			set_template( lptr, p_templ, pos, r, c, imExt->width);
			px = get_contextLS( num_contexts, p_templ, &dummy);
			pxLS_ptr[c + r * im->width] = px;
			if (px < num_contexts) countPx[px]++;
		}
	}

// ToDO: bestimme alle pred Koeffs. für alle px

	for ( px = 0; px < num_contexts; px++)
	{
		wL = p_pred2->weights_L[px];
		lwL = p_pred2->lweights_L[px];
		/* as access is not in raster-scan order, lptr must already be extended at borders */
		if ( countPx[px] == 0)
		{ /* should not happen	*/
			for (p = 0; p < LS_TEMPLATE_SIZE; p++)
			{
				lwL[p] = 0;
				wL[p] = 0.;
			}
			fprintf( stderr, "\n ### Ups! Empty context! ###\n");
			return;
		}

		/* reset all vectors	*/
		jacobi = matrix( countPx[px], LS_TEMPLATE_SIZE);	/* Jacobian */
		obs = vector( countPx[px]);	/* observations */

		i = 0; /* counter for observations	*/
		for ( r = 0, py = OFF_TOP * imExt->width; r < im->height; r++, py += imExt->width)
		{
			for ( c = 0, pos = OFF_LEFT + py; c < im->width; c++, pos++)
			{
				long meanOfPatt;

				if (pxLS_ptr[c + r * im->width] != px) continue; /* wrong context	*/

				set_template( lptr, p_templ, pos, r, c, imExt->width);
				/* get men of pattern	*/
				/* mean of current neighbourhood	*/
				//meanOfPatt = (p_templ->R[0] + p_templ->R[1] + 
				//							p_templ->R[2] + p_templ->R[3] + 2) >> 2;
				meanOfPatt = get_meanOfPattern( p_templ);

				obs[i] = lptr[pos] - meanOfPatt; /* observations	*/
				/* for all predictors					*/
				for (p = 0; p < LS_TEMPLATE_SIZE; p++)
				{
					jacobi[i][p] = p_templ->R[p] - meanOfPatt; /* first derivatives	*/
				}
				i++; /* next position (condition)	*/
			}
		}
		numOfTargetSamples = i;
#ifdef _DEBUG
		if (0)
		{
			FILE *out = fopen("obsJ.txt", "at");
			fprintf( out, "\npxLS = %d\n", px);
			for (i = 0; i < numOfTargetSamples; i++)
			{
				fprintf( out, "%3.0f : ", obs[i]);
				for (p = 0; p < LS_TEMPLATE_SIZE; p++)
				{
					fprintf( out, "%3.0f, ", jacobi[i][p]);
				}
				fprintf( out, "\n");
			}
			fclose( out);
		}
#endif
		if (errno)
		{
			perror( "\n### ");
			fprintf( stderr, "     errno = %d", errno);
			err = errno;
			errno = 0;
		}
		err =	ls_lin(  numOfTargetSamples, LS_TEMPLATE_SIZE, 
						obs, jacobi, fac, stdout);
/* geht nur wenn meanOfPattern nicht abgezogen wird
		facSum = 0.;
		for (p = 0; p < LS_TEMPLATE_SIZE; p++)
		{
			facSum += fac[p];
		}
		if ( facSum < 0)
			 facSum = -facSum;
		for (p = 0; p < LS_TEMPLATE_SIZE; p++)
		{
			fac[p] = fac[p] / facSum;
		}
	*/
		wL = p_pred2->weights_L[px];
		lwL = p_pred2->lweights_L[px];
		for (p = 0; p < LS_TEMPLATE_SIZE; p++)
		{
			lwL[p] = (long)floor( fac[p] * 1024);
			wL[p] = ( (float)lwL[p] / 1024.F);
		}

		free_vector( &obs);
		free_matrix( &jacobi);
	} /* for ( px  */

	/*----- save the prediction coeffs. for all contexts	*/
	fprintf( stdout, "\nprediction coefficients");
	rice_encoding( bitbuf, num_contexts, 8);
	for ( px = 0; px < num_contexts; px++)
	{
		lwL = p_pred2->lweights_L[px];
		fprintf( stdout, "\n%d: ", px);
		for (p = 0; p < LS_TEMPLATE_SIZE; p++)
		{
			fprintf( stdout, "%5d, ", lwL[p]);
			rice_encoding( bitbuf, abs( lwL[p]), 11);
			if (lwL[p] < 0) 
			{
				BufferPutBit( bitbuf, true);
			}
			else if (lwL[p] < 0) 
			{
				BufferPutBit( bitbuf, false);
			}
		}
	}
	fprintf( stdout, "\n");

	FREE( countPx);
}

/*---------------------------------------------------------------
 *	predictorLS()
 *
 *    nop
 *	 mghiq
 *	lfcbdjr
 * skeax
 *
 * differences
 *       n o p
 *	   m g h i q
 *	 l f c b d j r
 *   k e a x
 *---------------------------------------------------------------*/
float predictorLS( PRED2 *p_pred2, TEMPLATE *p_templ,
									 unsigned short *pxLS_ptr, long *eptrLS, 
									 int r,
									 int c, 
									 unsigned long pos,
									 unsigned int width,
									 long min_value, long max_value)
{
	float xHat;
	int n, m;
	long *lwL = NULL;
	long meanOfPatt;
	long sumErr, cnt, off;
	unsigned int i;
	unsigned long pr;

	/* Predict the next value. */
	lwL = p_pred2->lweights_L[ pxLS_ptr[pos] ];
	//wL = p_pred2->weights_L[ pxLS_ptr[pos] ];

	/* mean of current neighbourhood	*/
	//meanOfPatt = (p_templ->R[0] + p_templ->R[1] + 
	//							p_templ->R[2] + p_templ->R[3] + 2) >> 2;
	meanOfPatt = get_meanOfPattern( p_templ);
  /* !!! context quantisation and LS calculation also must use 
	 * this mean value !!!
	 */
	xHat = (float)(lwL[0] * (p_templ->R[0] - meanOfPatt) );
	for (i = 1; i < LS_TEMPLATE_SIZE; i++)
	{
		/* weight: lwl/1024 */
		xHat += (float)(lwL[i] * (p_templ->R[i] - meanOfPatt) );
	}
	 xHat = xHat / 1024.F;
	//xHat = wL[0] * (p_templ->R[0] - meanOfPatt);
	//for (i = 1; i < LS_TEMPLATE_SIZE; i++)
	//{
	//	xHat += wL[i] * (p_templ->R[i] - meanOfPatt);
	//}

	/* check error in vicinity	*/
	sumErr = cnt = 0;
	for ( n = 0; n <= PXLS_SEARCH_RANGE; n++)
	{
		if ( (r - n)  < 0) continue;
		pr = (r - n) * width;
		for ( m = -PXLS_SEARCH_RANGE; m <= PXLS_SEARCH_RANGE; m++)
		{
			if ( (c + m)  < 0  || (c + m)  >= (signed)width) continue;
			if (n ==0 && m >= 0) continue;

			/* point in search space is available	*/
			if (pxLS_ptr[c + m + pr] != pxLS_ptr[pos]) continue;

			/* point in search space has same context	*/ 
			sumErr += eptrLS[c + m + pr];
			cnt++;
		}
	}
	/* calculate off set	*/
	if (cnt > 2) 
		if (sumErr >= 0) off = (sumErr + (cnt >> 1) ) / cnt;
		else             off = (sumErr - (cnt >> 1) ) / cnt;
	else 
		off = 0;
	xHat = CLIP( (xHat + meanOfPatt + off), min_value, max_value);

	return xHat;
}
