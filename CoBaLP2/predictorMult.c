/*****************************************************************
 *
 * File..:	predictorMult.c
 * Descr.:	selction of primitive predictors
 *					 based on matched contexts
 * Author.:	Tilo Strutz
 * Date..: 	17.08.2018
 * changes:
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "predictor2.h"
#include "contexts.h"
#include "matrix_utils.h"
#include "leastSquaresPred.h"

#define MULT_SEARCH_RANGE  30
#define MULT_NUM_PREDS 16
#ifndef LOG2
	#define LOG2	0.69314718	/* used for entropy calculation	*/
#endif
typedef struct {
	unsigned long hist[MULT_NUM_PREDS][512];
} MULT_HIST;
/*---------------------------------------------------------------
 *	predictSelected()
 *---------------------------------------------------------------*/
long predictSelected( TEMPLATE *templ, unsigned short p)
{
	unsigned short co;
	int pCoeff[MULT_NUM_PREDS][8] = {
		//A  B  C  D  E  F Denom Den/2
		{ 1, 0, 0, 0, 0, 0, 1, 0}, // 0	A
		{ 0, 1, 0, 0, 0, 0, 1, 0}, // 1	B
		{ 1, 1, 0, 0, 0, 0, 2, 0}, // 2  (A+B ) / 2
		{ 1, 1, 1, 0, 0, 0, 3, 1}, // 3  (A+B+C + 1) / 3
		{ 1, 0, 1, 0, 0, 0, 2, 0}, // 4  (A+C) / 2
		{ 1, 0, 0, 1, 0, 0, 2, 0}, // 5  (A+D) / 2
		{ 0, 1, 1, 0, 0, 0, 2, 0}, // 6  (B+C) / 2
		{ 4, 4,-1,+1, 0, 0, 8, 4}, // 7   A+B+ (-C+D)/4 = (4A + 4B - C+D + 4) / 8
		{ 4, 4, 1,-1, 0, 0, 8, 4}, // 8   A+B+ (C-D)/4 = (4A + 4B + C-D + 4) / 8
		{ 1, 1,-1, 0, 0, 0, 1, 0}, // 9   A+  B-C
		{ 2, 1,-1, 0, 0, 0, 2, 1}, // 10   A+ (B-C) / 2 = (2A+ B-C  +1) / 2
		{ 4, 1,-1, 0, 0, 0, 4, 2}, // 11   A+ (B-C) / 4 = (4A+ B-C  +2) / 4
		{ 2, 1,-1, 0, 0, 0, 8, 4}, // 12   A+ (B-C) / 8 = (8A+ B-C  +4) / 8
		{ 1, 2,-1, 0, 0, 0, 2, 1}, // 13   B+ (A-C) / 2 = (2B+ A-C  +1) / 2
		{ 1, 4,-1, 0, 0, 0, 4, 2}, // 14   B+ (A-C) / 4 = (4B+ A-C  +2) / 4
		{ 0, 0, 0, 0, 0, 0, 1, 0}, // 15 MAP
	};
		long xHat;

		if (p < 15)
		{
			xHat = templ->R[0] * pCoeff[p][0];
			for (co = 1; co < 6; co++)
			{
					xHat += templ->R[co] * pCoeff[p][co];
			}
			xHat = (xHat + pCoeff[p][7]) / pCoeff[p][6]; /* normalisation	*/
		}
		else
		{
			long maxAB = max( templ->R[0], templ->R[1]); 
			long minAB = min( templ->R[0], templ->R[1]);

			if (templ->R[2] >= maxAB)	xHat = minAB;
			else if (templ->R[2] <= minAB)	xHat = maxAB;
			else
			{
				xHat = templ->R[0] + templ->R[1] - templ->R[2];
			}
		}
		return xHat;
}

/*---------------------------------------------------------------
 *	init_pred2Weights_Mult()
 *---------------------------------------------------------------*/
void init_pred2Weights_Mult( IMAGE *imExt, long *lptr, IMAGE *im, 
												 unsigned short num_contexts,
												 unsigned short *bestPred
												 )
{
	long predErr, offset;
	unsigned int idx, r, c;
  unsigned short p, px;
  // unsigned short bestPred[MAXNUM_OF_PRED_CONTEXTS];
	unsigned long py, pos;
	unsigned long *countPx = NULL;
	//unsigned long hist[MAXNUM_OF_PRED_CONTEXTS][MULT_NUM_PREDS][512];
	double dummy;
	double bestEntropy[MAXNUM_OF_PRED_CONTEXTS];
	//double H[MAXNUM_OF_PRED_CONTEXTS][MULT_NUM_PREDS];
	double **H = NULL; // [MAXNUM_OF_PRED_CONTEXTS][MULT_NUM_PREDS];
	TEMPLATE templ_obj, *templ=NULL;
	MULT_HIST *m_hist = NULL;

	errno = 0;

	H = matrix(MAXNUM_OF_PRED_CONTEXTS, MULT_NUM_PREDS);

	/* allocate memory for context counters	*/
	CALLOC( countPx, num_contexts, unsigned long);

	CALLOC( m_hist, num_contexts, MULT_HIST);

	templ = &templ_obj;
	offset = 1 << (im->bit_depth -1);

	/* clear histogramms	*/
	for (px = 0; px < num_contexts; px++)
	{
		countPx[px] = 0; // reset counter
		for (p = 0; p < MULT_NUM_PREDS; p++) /* for all predictors */
		{
			for (idx = 0; idx < 512; idx++) /* for all bins */
			{
				m_hist[px].hist[p][idx] = 0;
			}
		}
	}
	/* go through entire image	*/
	for ( r = 0, py = OFF_TOP * imExt->width; r < im->height; r++, py += imExt->width)
	{
		for ( c = 0, pos = OFF_LEFT + py; c < im->width; c++, pos++)
		{
			/* copy neighbour values to template,
			* must always be performed, because it must fill the borders for next lines
			*/
			set_template( lptr, templ, pos, r, c, imExt->width);
			px = get_contextLS( num_contexts, templ, &dummy);
	//		px = get_context( max(1, r), max(1, c), num_contexts, templ);
			countPx[px]++;

			/* check all predictors */
			for (p = 0; p < MULT_NUM_PREDS; p++) /* for all predictors */
			{
				long xHat;
				//xHat = templ->R[0] * pCoeff[p][0];
				//for (co = 1; co < 6; co++)
				//{
				//		xHat += templ->R[co] * pCoeff[p][co];
				//}
				//xHat = xHat / pCoeff[p][6]; /* normalisation	*/
				xHat = predictSelected( templ, p);
				predErr = lptr[pos] - xHat;	/* get estimation error	*/
				idx = min( 511, max( 0, predErr + offset)); /* shift and clip	*/
				m_hist[px].hist[p][idx]++;
			}
		}
	}
	/* compute entropies of different predictors and different contexts	*/
	for (px = 0; px < num_contexts; px++)
	{
		bestPred[px] = 0;
		bestEntropy[px] = 9999999.;

		for (p = 0; p < MULT_NUM_PREDS; p++) /* for all predictors */
		{
			double prob;		/* entropy of prediction errors	*/ 
			unsigned long sum = 0;

			H[px][p] = 0.0;
			for ( idx = 0; idx < 512; idx++)
			{
				if (m_hist[px].hist[p][idx])
				{
					sum += m_hist[px].hist[p][idx];
					prob = (double)m_hist[px].hist[p][idx] / (double)countPx[px];
					H[px][p] -= (prob * log( prob) / LOG2);
				}
			}
			if (bestEntropy[px] > H[px][p])
			{
				bestEntropy[px] = H[px][p];
				bestPred[px] = p;
			}
		}
	}
	FREE( m_hist)
	FREE( countPx);
	free_matrix(&H);
}

/*---------------------------------------------------------------
 *	predictorMult()
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
float predictorMult( TEMPLATE *templ, unsigned short *predMult,
									 unsigned short px, long *eptrLS, 
									 int r,
									 int c, 
									 unsigned long pos,
									 unsigned int width,
									 long min_value, long max_value)
{
	float xHat;
	//int n, m;
	//long sumErr, cnt, off;
	//unsigned int i;
	//unsigned long pr;

	xHat = (float)predictSelected( templ, predMult[ px] );

	/* check error in vicinity	*/
	//sumErr = cnt = 0;
	//for ( n = 0; n <= MULT_SEARCH_RANGE; n++)
	//{
	//	if ( (r - n)  < 0) continue;
	//	pr = (r - n) * width;
	//	for ( m = -MULT_SEARCH_RANGE; m <= MULT_SEARCH_RANGE; m++)
	//	{
	//		if ( (c + m)  < 0  || (c + m)  >= (signed)width) continue;
	//		if (n ==0 && m >= 0) continue;

	//		/* point in search space is available	*/
	//		if (pxLS_ptr[c + m + pr] != pxLS_ptr[pos]) continue;

	//		/* point in search space has same context	*/ 
	//		sumErr += eptrLS[c + m + pr];
	//		cnt++;
	//	}
	//}
	///* calculate off set	*/
	//if (cnt > 2) 
	//	if (sumErr >= 0) off = (sumErr + (cnt >> 1) ) / cnt;
	//	else             off = (sumErr - (cnt >> 1) ) / cnt;
	//else 
	//	off = 0;
	xHat = CLIP( (xHat), min_value, max_value);

	return xHat;
}
