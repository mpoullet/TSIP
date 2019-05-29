/*****************************************************************
 *
 * File..:	predictor2.c
 * Descr.:	Calculation of estimation value using linear prediction
 *					 based on matched contexts
 * Author.:	Tilo Strutz
 * Date..: 	28.02.2013
 * changes:
 * 14.03.2013 exception handling at top and left border
 * 22.03.2013 RLC mode removed
 * 22.03.2013 order of differences di changed
 * 04.04.2013 new: pred_mode
 * 17.04.2013 bug in template Rq fixed
 * 18.04.2013 bugfix in template on the right border
 * 19.04.2013 clipping before xHat is returned
 * 13.05.2013 renamned + integration of predictor2b()
 * 10.09.2013 unused parameters (r, c) removed from parameter lists)
 * 09.09.2014 use directional differences (GET_DIFFa/b() etc.) for A and B
 * 23.01.2015 wA, wB
 * 09.11.2016 th = 2.0F instead 2.5, mu adaptive for each context
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "predictor2.h"

/*---------------------------------------------------------------
 *	set_template()
 *---------------------------------------------------------------*/
void set_template( long *lptr, TEMPLATE *templ, unsigned long posExt,
									unsigned int r, unsigned int c, unsigned int widthExt)
{
/*
    NOP
	UMGHIQ
 TLFCBDJR
 SKEAX
*/

	/* fill lines above image data	*/
	if (r == 0)
	{ 
		if (c == 0) /* fill top left corner	*/
		{ /* lptr[posExt-1] is faked outside with (max - min) /2	*/
			if (OFF_LEFT > 0)
//#if OFF_LEFT > 0
			{
				/* Rn = Rg = Rc = Ra	*/
				if (OFF_TOP > 0) lptr[posExt-1-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt-1-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt-1-widthExt*3] = lptr[posExt-1];
			}
//#endif
			if (OFF_LEFT > 1)
			{
				/* Re = Ra	*/
				lptr[posExt-2] = lptr[posExt-1];
				/* Rf = Rm = R? = Ra	*/
				if (OFF_TOP > 0) lptr[posExt-2-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt-2-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt-2-widthExt*3] = lptr[posExt-1];
			}
			if (OFF_LEFT > 2)
			{
				/* Rk = Ra	*/
				lptr[posExt-3] = lptr[posExt-1];
				/* Rl = R? = R? = Ra	*/
				if (OFF_TOP > 0) lptr[posExt-3-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt-3-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt-3-widthExt*3] = lptr[posExt-1];
			}
			if (OFF_LEFT > 3)
			{
				/* Rs = Ra	*/
				lptr[posExt-4] = lptr[posExt-1];
				if (OFF_TOP > 0) lptr[posExt-4-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt-4-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt-4-widthExt*3] = lptr[posExt-1];
			}
		} /* c== 0*/
		else 
		{ /* c > 0, use Ra to fill everything which is in range to the right */
			if (OFF_LEFT > 0)
			{
				/* Rn = Rg = Rc = Ra	*/
				if (OFF_TOP > 0) lptr[posExt-1-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt-1-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt-1-widthExt*3] = lptr[posExt-1];
			}
			/* Ro = Rh = Rb = Ra;*/
			if (OFF_TOP > 0) lptr[posExt-widthExt  ] = lptr[posExt-1];
			if (OFF_TOP > 1) lptr[posExt-widthExt*2] = lptr[posExt-1];
			if (OFF_TOP > 2) lptr[posExt-widthExt*3] = lptr[posExt-1];

			if (OFF_RIGHT > 0)
			{
				/* Rp = Ri = Rd = Ra;*/
				if (OFF_TOP > 0) lptr[posExt+1-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt+1-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt+1-widthExt*3] = lptr[posExt-1];
			}
			if (OFF_RIGHT > 1)
			{
				/* R? = Rq = Rj = Ra;*/
				if (OFF_TOP > 0) lptr[posExt+2-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt+2-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt+2-widthExt*3] = lptr[posExt-1];
			}
			if (OFF_RIGHT > 2)
			{
				/* R? = R? = Rr = Ra;*/
				if (OFF_TOP > 0) lptr[posExt+3-widthExt  ] = lptr[posExt-1];
				if (OFF_TOP > 1) lptr[posExt+3-widthExt*2] = lptr[posExt-1];
				if (OFF_TOP > 2) lptr[posExt+3-widthExt*3] = lptr[posExt-1];
			}

			if (c == 1)
			{ /* now we have a true signal value and can overwrite the (max-min)/2 fake	*/
				if (OFF_LEFT > 1)
				{
					/* Re = Ra	*/
					lptr[posExt-2] = lptr[posExt-1];
					/* Rf = Rm = R? = Ra	*/
					if (OFF_TOP > 0) lptr[posExt-2-widthExt  ] = lptr[posExt-1];
					if (OFF_TOP > 1) lptr[posExt-2-widthExt*2] = lptr[posExt-1];
					if (OFF_TOP > 2) lptr[posExt-2-widthExt*3] = lptr[posExt-1];
				}
				if (OFF_LEFT > 2)
				{
					/* Rk = Ra	*/
					lptr[posExt-3] = lptr[posExt-1];
					/* Rl = R? = R? = Ra	*/
					if (OFF_TOP > 0) lptr[posExt-3-widthExt  ] = lptr[posExt-1];
					if (OFF_TOP > 1) lptr[posExt-3-widthExt*2] = lptr[posExt-1];
					if (OFF_TOP > 2) lptr[posExt-3-widthExt*3] = lptr[posExt-1];
				}
				if (OFF_LEFT > 3)
				{
					/* Rs = Ra	*/
					lptr[posExt-4] = lptr[posExt-1];
					if (OFF_TOP > 0) lptr[posExt-4-widthExt  ] = lptr[posExt-1];
					if (OFF_TOP > 1) lptr[posExt-4-widthExt*2] = lptr[posExt-1];
					if (OFF_TOP > 2) lptr[posExt-4-widthExt*3] = lptr[posExt-1];
				}
			}	/* c== 1*/
		} /* c > 0	*/
		if (c == widthExt-OFF_LEFT-OFF_RIGHT - 1)	/* right margin */
		{
			/* to the right fill with Rb;*/
			if (OFF_RIGHT > 0) lptr[posExt+1] = lptr[posExt  -widthExt];
			if (OFF_RIGHT > 1) lptr[posExt+2] = lptr[posExt+1-widthExt];
			if (OFF_RIGHT > 2) lptr[posExt+3] = lptr[posExt+2-widthExt];
		}
	} /* r== 0 */
	else
	{
		/* fill border values with something usefull	*/
		if (c == 0)	/* left margin */
		{
			if (OFF_LEFT > 0) lptr[posExt-1] = lptr[posExt  -widthExt]; /* Ra = Rb; */
			if (OFF_LEFT > 1) lptr[posExt-2] = lptr[posExt-1-widthExt]; /* Re = Rc; */
			if (OFF_LEFT > 2) lptr[posExt-3] = lptr[posExt-2-widthExt]; /* Rk = Rf; */
			if (OFF_LEFT > 3) lptr[posExt-4] = lptr[posExt-3-widthExt]; /* Rs = Rl; */
			/* Rc ist jetzt genau der Wert, 
			der in der vorangegangenen Zeile Ra war etc.	*/
		}
		else if (c == widthExt-OFF_LEFT-OFF_RIGHT - 1)	/* right margin */
		{
			/* Rd = Rr = Rj = Rb;*/
			if (OFF_RIGHT > 0) lptr[posExt+1] = lptr[posExt  -widthExt];
			if (OFF_RIGHT > 1) lptr[posExt+2] = lptr[posExt+1-widthExt];
			if (OFF_RIGHT > 2) lptr[posExt+3] = lptr[posExt+2-widthExt];
		}
	}
	assert(TEMPLATE_SIZE > 11);

	templ->R[10] = lptr[posExt-3]; /* k */
	templ->R[4] = lptr[posExt-2]; /* e */
	templ->R[0] = lptr[posExt-1]; /* a */

	templ->R[11] = lptr[posExt-3-widthExt]; /* l */
	templ->R[5]  = lptr[posExt-2-widthExt]; /* f */
	templ->R[2]  = lptr[posExt-1-widthExt]; /* c */
	templ->R[1]  = lptr[posExt  -widthExt]; /* b */
	templ->R[3]  = lptr[posExt+1-widthExt]; /* d */
	templ->R[9]  = lptr[posExt+2-widthExt]; /* j */
	if (MAX_TEMPLATE_SIZE > 17) templ->R[17] = lptr[posExt+3-widthExt]; /* r */

	if (MAX_TEMPLATE_SIZE > 12) templ->R[12] = lptr[posExt-2-widthExt-widthExt]; /* m */
	templ->R[6] = lptr[posExt-1-widthExt-widthExt]; /* g */
	templ->R[7] = lptr[posExt  -widthExt-widthExt]; /* h */
	templ->R[8] = lptr[posExt+1-widthExt-widthExt]; /* i */
	if (MAX_TEMPLATE_SIZE > 16) templ->R[16] = lptr[posExt+2-widthExt-widthExt]; /* q */

	if (MAX_TEMPLATE_SIZE > 13) templ->R[13] = lptr[posExt-1-widthExt-widthExt-widthExt]; /* n */
	if (MAX_TEMPLATE_SIZE > 14) templ->R[14] = lptr[posExt  -widthExt-widthExt-widthExt]; /* o */
	if (MAX_TEMPLATE_SIZE > 15) templ->R[15] = lptr[posExt+1-widthExt-widthExt-widthExt]; /* p */
	if (MAX_TEMPLATE_SIZE > 18) templ->R[18] = lptr[posExt-4]; /* s */
	if (MAX_TEMPLATE_SIZE > 19) templ->R[19] = lptr[posExt-4-widthExt]; /* t */
	if (MAX_TEMPLATE_SIZE > 20) templ->R[20] = lptr[posExt-3-widthExt-widthExt]; /* u */
	
}


/*---------------------------------------------------------------
 *	init_pred2Weights()
 *---------------------------------------------------------------*/
void init_pred2Weights( PRED2 *p_pred2, unsigned int max_value,
											 float wA, float wB)
{
	unsigned int i, k;

	/* learning rate dependend on distance to current pixel	*/
	{
		for ( i = 0; i < NUM_OF_PRED; i++)
		{
			p_pred2->lr_a[i] = 0.0001F;
			p_pred2->lr_b[i] = 0.0001F;
		}
		p_pred2->lr_a[0] = p_pred2->lr_a[1] = p_pred2->lr_a[2] = 		0.0004F;
		p_pred2->lr_a[3] = p_pred2->lr_a[4] =	    		0.0002F;
		p_pred2->lr_b[0] = p_pred2->lr_b[1] = p_pred2->lr_b[2] = 		0.0004F;
		p_pred2->lr_b[3] = p_pred2->lr_b[4] =	    		0.0002F;
		/* if fast adaptation in the beginning is desired
		p_pred2->lr[0] = p_pred2->lr[1] = p_pred2->lr[2] = 		0.001;
		p_pred2->lr[3] = p_pred2->lr[4] =	    		0.0004;
		*/

		/* for quadratic normalisation	*/
		//for ( i = 0; i < NUM_OF_PRED; i++)
		//{
		//	p_pred2->lr_a[i] = 0.0001F;
		//	p_pred2->lr_b[i] = 0.0001F;
		//}
	}

	for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
	{
		p_pred2->count_px[k] = 0;
		p_pred2->sum_abserr_a[k] = 0.;
		p_pred2->sum_abserr_b[k] = 0.;
		p_pred2->sum_abserr_R[k] = 0.;

		for ( i = 0; i < NUM_OF_PRED; i++)
		{
			p_pred2->weights_a[k][i] = 0.0;
			p_pred2->weights_b[k][i] = 0.0;
			p_pred2->weights_R[k][i] = 0.0;
			p_pred2->mean_difference_a[k][i] = max_value / 32 + 1; /* absolute differences	*/
			p_pred2->mean_difference_b[k][i] = max_value / 32 + 1;
			p_pred2->signEQUdiff_a[k][i] = 0; /* correlation between diff at pos i and a	*/
			p_pred2->signEQUdiff_b[k][i] = 0; /* correlation between diff at pos i and b	*/
			p_pred2->signNEQdiff_a[k][i] = 0; /* correlation between diff at pos i and a	*/
			p_pred2->signNEQdiff_b[k][i] = 0; /* correlation between diff at pos i and b	*/
		}
		/* special initialisation of weights	and lerning rates*/
		if (1 /* Y component */)
		{
			/* conservative prediction in the beginning
			 * d0 = A-C
			 * d1 = C-B
			 * d2a = B-D
			 * d2b = B-H
			 * Pa: assumed that X-A = -wA * (C - B) 
			 * Pb: assumed that X-B =  wB * (A - C) 
			 */
				p_pred2->weights_a[k][1] = -wA;
				p_pred2->weights_b[k][0] = wB;
		}
		else  /* U,V component !! needs clever decision !!	*/
		{
				p_pred2->weights_a[k][0]=0.95F; /* for Cb Cr of BathUK images */
				p_pred2->weights_a[k][1]=0.95F;
				p_pred2->weights_a[k][0]=0.5;
				p_pred2->weights_a[k][1]=0.5;
				p_pred2->weights_b[k][0]=0.5;
				p_pred2->weights_b[k][1]=0.5;
				/* simple addition of gradient vectors, as U,V are smooth	*/
		}
	}
}

/*---------------------------------------------------------------
 *	predictor2a()
 *
 *    nop
 *	 mghiq
 *	lfcbdjr
 * skeax
 *
 * differences
 *         n13o15p
 *           12
 *	    m  g  h  i  q
 *     11  5  4  7 14
 *	 l  f  c 1b 2d 8j 16r
 *  10  6  0
 *   k 9e 3a  x
 *---------------------------------------------------------------*/
float predictor2a( PRED2 *p_pred2, TEMPLATE *templ, 
									 unsigned int px, long min_value, long max_value)
{
	long *di = NULL;
	float da, xHat;
	float *wa = NULL;
	unsigned int i;

	di = p_pred2->difference_i_a;

	/* determine differences  */
	//GET_DIFFS()
	//GET_DIFFS_2()

	GET_DIFFSa()		/* more directional	(horizontal) */
	GET_DIFFS_2a()

	/* Predict the next value. */
	wa = p_pred2->weights_a[px];
	da = wa[0] * di[0];
	for (i = 1; i < NUM_OF_PRED; i++)
	{
		da += wa[i] * di[i];
	}

	xHat = CLIP( da + templ->R[0], min_value, max_value);

	return xHat;
}

/*---------------------------------------------------------------
 *	predictor2b()
 *
 *    nop
 *	 mghiq
 *	lfcbdjr
 * skeax
 *
 * differences
 *         n13o15p
 *           12
 *	    m  g  h  i  q
 *     11  5  4  7 14
 *	 l  f  c 1b 2d 8j 16r
 *  10  6  0
 *   k 9e 3a  x
 *---------------------------------------------------------------*/
float predictor2b( PRED2 *p_pred2, TEMPLATE *templ, 
									 unsigned int px, long min_value, long max_value)
{
	long *di = NULL;
	float db, xHat;
	float *wb = NULL;
	unsigned int i;

	/* Predict the next value. */
	di = p_pred2->difference_i_b;

	//GET_DIFFS()
	//GET_DIFFS_2()

	GET_DIFFSb()		/* more directional	(vertical) */
	GET_DIFFS_2b()

	/* Predict the next value. */
	wb = p_pred2->weights_b[px];
	db = wb[0] * di[0];
	for (i = 1; i < NUM_OF_PRED; i++)
	{
		db += wb[i] * di[i];
	}

	xHat = CLIP( db + templ->R[1], min_value, max_value);

	return xHat;
}

/*---------------------------------------------------------------
 *	update_pred2Weights()
 *---------------------------------------------------------------*/
void update_pred2Weights( PRED2 *p_pred2, unsigned int px, 
												 float pred_err_a, float pred_err_b,
												 int pred_mode)
{
	int sign_err, sign_diff;
	long *di = NULL; // , N, N1;
	//float *mean_di_a, *mean_di_b;
	//float mean_di_Xa, mean_di_Xb;
	long *signEQUdiff_a, *signEQUdiff_b;
	long *signNEQdiff_a, *signNEQdiff_b;
	float *p_w = NULL, delta;
	float *mu_a = NULL, *mu_b = NULL;
	float th;
	//double *p_mean = NULL;
	int *p_mean = NULL;
	unsigned int i;

	//N1 = p_pred2->count_px[px];
	p_pred2->count_px[px]++;
	//N = N1 + 1;

	if (pred_mode & 0x01) /* if predictor A is enabled	*/
	{
		mu_a = p_pred2->lr_a;
		di = p_pred2->difference_i_a;
		p_w = p_pred2->weights_a[px];
		p_mean = p_pred2->mean_difference_a[px]; /* pointer assignment	*/
		signEQUdiff_a = p_pred2->signEQUdiff_a[px]; /* signs for polar correlation test	*/
		signNEQdiff_a = p_pred2->signNEQdiff_a[px];
		if (pred_err_a < 0) sign_err = -1;
		else if (pred_err_a > 0) sign_err = 1;
		else sign_err = 0;
		for ( i = 0; i < NUM_OF_PRED; i++)
		{
			//mean_di_a[i] = (mean_di_a[i] * N1 + di[i]) / N;	/* update of mean	of diff*/
			//mean_di_Xa = (mean_di_Xa * N1 + pred_err_a) / N;	/* update of mean	*/
			if (di[i] < 0) sign_diff = -1;
			else if (di[i] > 0) sign_diff = 1;
			else sign_diff = 0;
			if (sign_diff != 0)
			{
			  if (sign_diff == sign_err) signEQUdiff_a[i]++;
				else signNEQdiff_a[i]++;
			}

			if (p_pred2->count_px[px] > 10) /* i*/
			{
				// for normalisation with abs(d[i])
				mu_a[i] = min( 0.0004F, (float)fabs( 1.F* signEQUdiff_a[i] - signNEQdiff_a[i] ) /
																					(signEQUdiff_a[i] + signNEQdiff_a[i] + 1) * 0.01F); //
				mu_a[i] = max (mu_a[i], 0.0000001F);
				// for normalisation with d[i]*d[i]
				//mu_a[i] = min( 0.004F, 1.F * abs( signEQUdiff_a[i] - signNEQdiff_a[i] ) /
				//																 (signEQUdiff_a[i] + signNEQdiff_a[i] + 1) * 0.02F); //
			}
		}

		p_pred2->sum_abserr_a[px] += (float)fabs( pred_err_a);

		if (p_pred2->count_px[px] > 1) /* exlude first error value	*/
			th = 2.0F * p_pred2->sum_abserr_a[px] / p_pred2->count_px[px];
		else
			th = 10; /* default threshold in the beginning	*/

		/* limitation of adaptation	*/
		if (pred_err_a < 0) pred_err_a = max( pred_err_a, -th);
		else pred_err_a = min( pred_err_a, th);
		for ( i = 0; i < NUM_OF_PRED; i++)
		{	   	   
			/* updating of mean absolute difference	*/
			p_mean[i] = (p_mean[i] * 7 + abs( di[i])) >> 3;
			//p_mean[i] = (p_mean[i] * 31 + di[i] * di[i]) >> 5;
			//p_mean[i] = (p_mean[i] * 7 + abs( di[i])) * 0.125;
			/* LMS filter	*/
			if (di[i] != 0 && pred_err_a != 0) /* p_mean has to be updated anyway */
			{
				delta = mu_a[i] * pred_err_a * di[i] / (p_mean[i] + 1);/* avoid division by zero!	*/
				p_w[i] += delta;
			}
		} /* for ( i */
	}
	if (pred_mode & 0x02) /* if predictor B is enabled	*/
	{
		mu_b = p_pred2->lr_b;
		di = p_pred2->difference_i_b;
		p_w = p_pred2->weights_b[px];
		p_mean = p_pred2->mean_difference_b[px]; /* pointer assignment	*/
		signEQUdiff_b = p_pred2->signEQUdiff_b[px];
		signNEQdiff_b = p_pred2->signNEQdiff_b[px];
		if (pred_err_b < 0) sign_err = -1;
		else if (pred_err_b > 0) sign_err = 1;
		else sign_err = 0;
		for ( i = 0; i < NUM_OF_PRED; i++)
		{
			if (di[i] < 0) sign_diff = -1;
			else if (di[i] > 0) sign_diff = 1;
			else sign_diff = 0;
			if (sign_diff != 0)
			{
			  if (sign_diff == sign_err) signEQUdiff_b[i]++;
				else signNEQdiff_b[i]++;
			}

			if (p_pred2->count_px[px] > 10) /* i*/
			{
				// for normalisation with abs(d[i])
				mu_b[i] = min( 0.0004F, (float)fabs( 1.F* signEQUdiff_b[i] - signNEQdiff_b[i] ) /
																					(signEQUdiff_b[i] + signNEQdiff_b[i] + 1) * 0.01F); //
				mu_b[i] = max(mu_b[i], 0.0000001F);
				// for normalisation with d[i]*d[i]
				//mu_b[i] = min( 0.004F, 1.F * abs( signEQUdiff_b[i] - signNEQdiff_b[i] ) /
				//																 (signEQUdiff_b[i] + signNEQdiff_b[i] + 1) * 0.02F); //
			}
		}
		p_pred2->sum_abserr_b[px] += (float)fabs( pred_err_b);

		/* limitation of adaptation	should be adaptive
		 * higher value for more noisy data
		 */
		if (p_pred2->count_px[px] > 1) /* exlude first error value	*/
			th = 2.0F * p_pred2->sum_abserr_b[px] / p_pred2->count_px[px];
		else
			th = 10; /* default threshold in the beginning	*/

		if (pred_err_b < 0) pred_err_b = max( pred_err_b, -th);
		else pred_err_b = min( pred_err_b, th);

		for ( i = 0; i < NUM_OF_PRED; i++)
		{	   	   
			/* updating of mean absolute difference	*/
			p_mean[i] = (p_mean[i] * 7 + abs( di[i]) ) >> 3;
			//p_mean[i] = (p_mean[i] * 31 + di[i] * di[i]) >> 5;
			//p_mean[i] = (p_mean[i] * 7 + abs( di[i])) * 0.125;
			/* LMS filter	*/
			if (di[i] != 0 && pred_err_b != 0) /* p_mean has to be updated anyway */
			{
				delta = mu_b[i] * pred_err_b * di[i] / (p_mean[i] + 1);/* avoid division by zero!	*/
				p_w[i] += delta;
			}
		} /* for ( i */
	}
}
