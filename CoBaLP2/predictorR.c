/*****************************************************************
 *
 * File..:	predictorR.c
 * Descr.:	Calculation of estimation value using linear prediction
 *					 based on matched contexts
 * Author.:	Tilo Strutz
 * Date..: 	30.04.2013
 * changes:
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "predictor2.h"

/*---------------------------------------------------------------
 *	init_pred2Weights_R()
 *---------------------------------------------------------------*/
void init_pred2Weights_R( PRED2 *p_pred2, unsigned int max_value)
{
	unsigned int i, k;

	/* learning rate dependend on distance to current pixel	*/
	{
		for ( i = 0; i < TEMPLATE_SIZE; i++)
		{
			p_pred2->lr_R[i] = 0.0001F;
		}
		p_pred2->lr_R[0] = p_pred2->lr_R[1] = p_pred2->lr_R[2] = 		0.0004F;
		p_pred2->lr_R[3] = p_pred2->lr_R[4] =	    		0.0002F;
	}

	for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
	{
		for ( i = 0; i < TEMPLATE_SIZE; i++)
		{
			p_pred2->weights_R[k][i] = 0.0;
			p_pred2->mean_value_R[k][i] = max_value / 32 + 1;
		}
		/* special initialisation of weights	and lerning rates*/
//		if (1 /* Y component */)
		{
				p_pred2->weights_R[k][0] = 0.5;
				p_pred2->weights_R[k][1] = 0.5;
		}
		//else  /* U,V component !! needs clever decision !!	*/
		//{
		//}
	}
}

/*---------------------------------------------------------------
 *	predictorR()
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
float predictorR( PRED2 *p_pred2, TEMPLATE *templ,
									 unsigned int px, long min_value, long max_value)
{
	float xHat;
	float *wR = NULL;
	unsigned int i;

	/* get mean of template */
	templ->meanR = (float)templ->R[0];
	for (i = 1; i < TEMPLATE_SIZE; i++)
	{
		templ->meanR += templ->R[i];
	}
	templ->meanR /= TEMPLATE_SIZE;

	/* Predict the next value. */
	wR = p_pred2->weights_R[px];

	xHat = wR[0] * (templ->R[0] - templ->meanR);
//	xHat = wR[0] * (templ->R[0] - 0);
	for (i = 1; i < TEMPLATE_SIZE; i++)
	{
		xHat += wR[i] * (templ->R[i] - templ->meanR);
//		xHat += wR[i] * (templ->R[i] - 0);
	}
	/* meanR inluences xHat with a factor wsum= Sum( w_i)
	 * when scaling meanR by this factor befor next operation 
	 * the weights can go bananas, why?
	 */
	xHat = CLIP( xHat + templ->meanR, min_value, max_value);
//	xHat = CLIP( xHat , min_value, max_value);

	return xHat;
}

/*---------------------------------------------------------------
 *	update_pred2Weights_R()
 *---------------------------------------------------------------*/
void update_pred2Weights_R( PRED2 *p_pred2, unsigned int px, 
												 float pred_err_R,
												 int pred_mode, TEMPLATE *templ)
{
	float *p_w = NULL, delta;
	float *mu = NULL;
	float th;
	//double *p_mean = NULL;
	int *p_mean = NULL;
	unsigned int i;

	mu = p_pred2->lr_R;

	if (pred_mode & 0x10) /* if predictor R is enabled	*/
	{
		p_w = p_pred2->weights_R[px];
		p_mean = p_pred2->mean_value_R[px]; /* pointer assignment	*/

		/* limitation of adaptation	*/
		p_pred2->sum_abserr_R[px] += (float)fabs( pred_err_R);
		//if (p_pred2->count_px[px] > 10)
			th = 2.5F * p_pred2->sum_abserr_R[px] / p_pred2->count_px[px];
		//else
		//	th = 10; /* default threshold in the beginning	*/

		if (pred_err_R < 0) pred_err_R = max( pred_err_R, -th);
		else pred_err_R = min( pred_err_R, th);

		for ( i = 0; i < TEMPLATE_SIZE; i++)
		{	   	   
			/* updating of mean absolute difference	*/
			p_mean[i] = (p_mean[i] * 7 + abs( templ->R[i] - (long)templ->meanR)) >> 3;
//			p_mean[i] = (p_mean[i] * 7 + abs( templ->R[i] - 0)) >> 3;
			/* LMS filter	*/
			if ( pred_err_R != 0) /* p_mean has to be updated anyway */
			{
				delta = mu[i] * pred_err_R * (templ->R[i]-templ->meanR) / (p_mean[i] + 1);/* avoid division by zero!	*/
	//			delta = mu[i] * pred_err_R * (templ->R[i]-0) / (p_mean[i] + 1);/* avoid division by zero!	*/
				p_w[i]   += delta;
			}
		} /* for ( i */
	}
}
