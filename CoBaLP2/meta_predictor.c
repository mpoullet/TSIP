/*****************************************************************
 *
 * File..:	meta_predictor.c
 * Descr.:	combines several predictors
 * Author.:	Tilo Strutz
 * Date..: 	08.04.2013
 * changes:
 * 19.04.2013 addition of MED predictor
 * 19.04.2013 modified calculation of meta weights, more weight ti the winner
 * 20.04.2013 modified calculation of meta weights according to variances
 * 23.04.2013 bias compensation also for TM predictor
 * 27.04.2013 inclusion of PAETH mode
 * 15.05.2013 MED and R predictor in parallel, total: 5 predictors
 * 10.09.2013 meta_prediction() some unused parameters removed
 *						unsigned short px; (instead uint)
 * 26.05.2014 update_meta_prediction() reduce penalty for lowest prediction error
 * 17.06.2014 update_meta_prediction() based on inverse covariance matrix
 * 24.06.2014 back to 26c blending, exclude MED
 * 23.01.2015 bugfix, if long run of constant values, variance could have be
 *					  scaled down to zero, check required and reset to 0.00001
 * 09.08.2018 new: predictorCM.c as fast alternative to predictorTM.c using prediction context
 * 14.08.2018 new: predictorLS.c
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "contexts.h"
#include "meta_predictor.h"
#include "predictorTM.h"
//#include "med.h"

//#define EXCLUSION


/*  combination of estimates based on their variances
 *        1/ VAR(a)                   1               VAR(b)
 * wa = ------------------- = ------------------ =--------------
 *      1/VAR(a) + 1/VAR(b)		1 + VAR(a)/(VAR(b)   VAR(b)+VAR(a)
 *
 * remark: devision by N can be excluded
  *        VAR(b)*N
 * wa = ------------------- 
 *       (VAR(b)+VAR(a)) * N
*/
#define WEIGTHING2( a, b, wa, wb)\
						sum =  max( 0.000000001F, a + b);		\
						wa = a / sum;		\
						wb = b / sum;		\

/*  combination of estimates based on their variances
 *        1/ VAR(a)														 1
 * wa = ------------------------- = -------------------------------
 *      1/VAR(a)+1/VAR(b)+1/VAR(c)	1+VAR(a)/(VAR(b)+VAR(a)/(VAR(c) 
 *
 *         VAR(b)*VAR(c)
 * wa = ---------------------------------------------
 *      VAR(b)*VAR(c) + VAR(a)*VAR(c) + VAR(a)*VAR(b)
 */
#define WEIGTHING3( a, b, c, wa, wb, wc)\
{															\
							sum = max( 0.000000001F, a + b + c * 1.F);		\
							wa = a / sum;				\
							wb = b / sum;				\
							wc = c / sum;				\
}

#define WEIGTHING4( a, b, c, d, wa, wb, wc, wd) \
			{																\
								sum = max( 0.000000001F, a + b + c + d * 1.F);		\
								wa = a / sum;				\
								wb = b / sum;				\
								wc = c / sum;				\
								wd = d / sum;				\
			}															\

/*---------------------------------------------------------------
 *	init_meta_prediction()
 *---------------------------------------------------------------*/
int init_meta_prediction( META_PRED *p_meta, int pred_mode)
{
	char *rtn="init_prediction";
	int err = 0;
	unsigned short k;
	/* mean value of entire prediction-error image	*/
	p_meta->mean_tot = 0.0;
	p_meta->zero_dist_flag = 0;

	for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
	{
		p_meta->sum_err_a[k] = 0.;	/* error sum of predictor A	*/
		p_meta->sum_err_b[k] = 0.;	/* error sum of predictor B	*/
		p_meta->sum_err_R[k] = 0.;
		p_meta->sum_err_T[k] = 0.;
		p_meta->sum_err_L[k] = 0.;
		p_meta->count_scaled[k] = 0;
		//p_meta->best_fac[k] = 1.0;

		// be aware of division by zero !
		p_meta->covAA[k] = 0.00001F;
		p_meta->covBB[k] = 0.00001F;
		p_meta->covRR[k] = 0.00001F;
		p_meta->covTT[k] = 0.00001F;
		p_meta->covLL[k] = 0.00001F;

		p_meta->covAB[k] = 0.0F;
		p_meta->covAR[k] = 0.0F;
		p_meta->covAT[k] = 0.0F;
		p_meta->covAL[k] = 0.0F;
		p_meta->covBR[k] = 0.0F;
		p_meta->covBT[k] = 0.0F;
		p_meta->covRT[k] = 0.0F;

		p_meta->sum_abserr_a[k] = 0.;	/* for statistics	*/
		p_meta->sum_abserr_b[k] = 0.;
		p_meta->sum_abserr_R[k] = 0.;
		p_meta->sum_abserr[k] = 0;

		// be aware of division by zero !
		p_meta->abserr_a[k] = 0.00001F;
		p_meta->abserr_b[k] = 0.00001F;
		p_meta->abserr_R[k] = 0.00001F;
		p_meta->abserr_T[k] = 0.00001F;
		p_meta->abserr_L[k] = 0.00001F;

		/* set initial weights for predictor blending	*/
		switch (pred_mode)
		{
			case 0:	/* should not happen	*/
			case 1: 
				p_meta->wwa[k] = 1.0F; 
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.0F;
				p_meta->wwR[k] = 0.0F;	
				p_meta->wwLS[k] = 0.0F;	
				break; /* only A	*/
			case 2: 
				p_meta->wwb[k] = 1.0F; 
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.0F;
				p_meta->wwR[k] = 0.0F;	
				p_meta->wwLS[k] = 0.0F;	
				break; /* only B	*/
			case 4: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.0F;
				p_meta->wwR[k] = 0.0F;	
				p_meta->wwTM[k] = 1.0F;	
				p_meta->wwLS[k] = 0.0F;	
				break; /* only TM	*/
			case 8: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.0F;
				p_meta->wwR[k] = 0.0F;	
				p_meta->wwTM[k] = 0.0F;	
				p_meta->wwLS[k] = 1.0F;	
				break; /* only LS	*/

			case 3: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.5;	
				p_meta->wwTM[k] = p_meta->wwR[k] = 0.0;	
				p_meta->wwLS[k] = 0.0F;	
				break; /* A	and B */
			case 5: 
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.5;	
				p_meta->wwb[k] = p_meta->wwR[k] = 0.0;	
				p_meta->wwLS[k] = 0.0F;	
				break; /*  A and TM	*/
			case 6: 
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.5;	
				p_meta->wwa[k] = p_meta->wwR[k] = 0.0;	
				p_meta->wwLS[k] = 0.0F;	
				break; /*  B and TM	*/

			case 7: 
				p_meta->wwa[k] = p_meta->wwb[k] =	p_meta->wwTM[k] = 1.0F/3.F;	
				p_meta->wwR[k] = 0.0;
				p_meta->wwLS[k] = 0.0F;	
				break; /*  A, B and TM	*/
			case 9: 
				p_meta->wwa[k] = p_meta->wwLS[k] = 0.5;	
				p_meta->wwb[k] = p_meta->wwR[k] = 0.0;	
				p_meta->wwTM[k] = 0.0F;	
				break; /*  A and LS	*/

			case 16: 
				p_meta->wwa[k] =	p_meta->wwb[k] = p_meta->wwTM[k] = 0.0;	
				p_meta->wwLS[k] = 0.0F;	
				p_meta->wwR[k] = 1.0;
				break; /* only R	*/

			case 17: 
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.0;	
				p_meta->wwa[k] =	p_meta->wwR[k] = 0.5;
				p_meta->wwLS[k] = 0.0F;	
				break; /*  A, R	*/

			case 18: 
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.0;	
				p_meta->wwb[k] =	p_meta->wwR[k] = 0.5;
				p_meta->wwLS[k] = 0.0F;	
				break; /*  B, R	*/

			case 20: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.0;	
				p_meta->wwTM[k] =	p_meta->wwR[k] = 0.5;
				p_meta->wwLS[k] = 0.0F;	
			break; /*  TM, R	*/

			case 19: 
				p_meta->wwTM[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwb[k] = p_meta->wwR[k] = 1.F/3.F;
				p_meta->wwLS[k] = 0.0F;	
				break; /* A, B, R	*/

			case 21: 
				p_meta->wwb[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwTM[k] = p_meta->wwR[k] = 1.F/3.F;
				p_meta->wwLS[k] = 0.0F;	
				break; /* A, TM, R	*/

			case 22: 
				p_meta->wwa[k] = 0.0;	
				p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwR[k] = 1.F/3.F;
				p_meta->wwLS[k] = 0.0F;	
				break; /* B, TM, R	*/

			case 23: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.25;
				p_meta->wwTM[k] = p_meta->wwR[k] = 0.25;
				p_meta->wwLS[k] = 0.0F;	
				break; /* A, B, TM, R	*/

			default:
				fprintf(stderr, "\n %s: prediction mode %d is not supported yet\n", rtn, pred_mode);
				perror(" ");
				err = 12;
				goto end;
		}
	}

end:
	return err;
}

void finish_meta_prediction( /*META_PRED *p_meta*/)
{
}

/*---------------------------------------------------------------
 *	meta_predict()
 *---------------------------------------------------------------*/
unsigned short 
meta_predict( IMAGE *im, /* format of real image	*/
							IMAGE *imExt, /* format of extended image	*/
							long *lptr,		/* pointer to extended image-data array	*/
							unsigned short *pxExt_ptr,	/* array of prediction contexts */
							unsigned long posExt,	/* current position in extended array	*/
							int r, int c,		/* coordinates in image array	*/
							TEMPLATE *templ,
							META_PRED *p_m, PRED2 *p_pred2, 
								int pred_mode, 
								unsigned short num_contexts,
								unsigned short num_contextsLS,
								unsigned long *count_px, unsigned long *count_pxLS,
								unsigned short *pxLS_ptr, 
								unsigned short *predMult,
								long *eptrLS,
								LEAST_SQUARES *p_ls)
{
	unsigned short px;
	unsigned long pos;
	double dummy;

		pos = c + r * im->width;
		/* determine context and prediction value	*/
		px = get_context( r, c, num_contexts, templ);
//		px = get_contextLS( num_contexts, templ, &dummy);
		pxExt_ptr[posExt] = px;
		if (0 || pred_mode & 0x8)
			pxLS_ptr[pos] = get_contextLS( num_contextsLS, templ, &dummy);
		//else pxLS_ptr[pos] = 0; no need to fill, is not allocated
		
#ifdef WHYNOT
		/* for some reason this exception handling is useful despite the clever border extension, 
		 * side effects ??
		 */
		if (r+c == 0)	/* first pixel	*/
		{
			p_m->xHat_a_d = p_m->xHat_b_d = p_m->xHat_R_d = p_m->xHat_T = p_m->xHat_M = (im->max_value + im->min_value) / 2;
		}
		else if (px == num_contexts + 1)	/* first row	*/
		{
			p_m->xHat_a_d = p_m->xHat_b_d = p_m->xHat_R_d = p_m->xHat_T = p_m->xHat_M = templ->R[0];
		}
		else if (px == num_contexts + 2)	/* first column	*/
		{
			p_m->xHat_a_d = p_m->xHat_b_d = p_m->xHat_R_d = p_m->xHat_T = p_m->xHat_M = templ->R[1];
		}
		else 
#endif
		{
			if (pred_mode & 0x1)	
				p_m->xHat_a_d = predictor2a( p_pred2, templ, px, im->min_value, im->max_value);
			else	p_m->xHat_a_d = 0; /* should never be used	*/

			if (pred_mode & 0x2)	
				p_m->xHat_b_d = predictor2b( p_pred2, templ, px, im->min_value, im->max_value);
			else	p_m->xHat_b_d = 0; /* should never be used	*/

			if (pred_mode & 0x4)	
				p_m->xHat_T = predictorCM( imExt, lptr, pxExt_ptr, posExt, r, c);
				//p_m->xHat_T = predictorTM( imExt, lptr, posExt, r, c, p_ls);
			else p_m->xHat_T = 0;

			if (pred_mode & 0x10)	
				if ( px < num_contexts)
					p_m->xHat_R_d = predictorR( p_pred2, templ, px, im->min_value, im->max_value);
				else  if ( px == num_contexts + 1)
					p_m->xHat_R_d = (float)templ->R[0]; /* A */
				else  if ( px == num_contexts + 2)
					p_m->xHat_R_d = (float)templ->R[1]; /* B */
			else	p_m->xHat_R_d = 0; /* should never be used	*/

			if (pred_mode & 0x08)	
			{
					p_m->xHat_L = predictorLS( p_pred2, templ, pxLS_ptr, eptrLS, 
						r, c, pos, im->width,
						im->min_value, im->max_value);
					//p_m->xHat_L = predictorMult( templ, predMult, pxLS_ptr[pos], eptrLS, 
					//	r, c, pos, im->width,
					//	im->min_value, im->max_value);
			}
			else	p_m->xHat_L = 0; /* should never be used	*/
		}


		/* bias compensation	*/
		if (count_px[px] > NUM_SKIPPED_STEPS)
		{
			if (pred_mode & 0x1)
			{
				p_m->xHat_a_d += p_m->sum_err_a[px] / (count_px[px]-NUM_SKIPPED_STEPS);
			}
			if (pred_mode & 0x2)	
			{
				p_m->xHat_b_d += p_m->sum_err_b[px] / (count_px[px]-NUM_SKIPPED_STEPS);
			}
			if (pred_mode & 0x4)	
			{
				p_m->xHat_T += p_m->sum_err_T[px] / (count_px[px]-NUM_SKIPPED_STEPS);
			}

			if (pred_mode & 0x10)	
			{
				p_m->xHat_R_d += p_m->sum_err_R[px] / (count_px[px]-NUM_SKIPPED_STEPS);
			}
		}
		if (pred_mode & 0x8)	
		{
			if ( 1 &&count_pxLS[ pxLS_ptr[pos] ] > NUM_SKIPPED_STEPS)
			{
				p_m->xHat_L += p_m->sum_err_L[ pxLS_ptr[pos] ] / 
					                (count_pxLS[ pxLS_ptr[pos] ]-NUM_SKIPPED_STEPS);
			}
		}
		//p_m->xHat_M = p_m->xHat_R_d;


#ifdef _DEBUG
//if (px == 3)
//{
//	printf( "\n %f \t %f \t %f \t %f ", p_m->wwa[px], p_m->wwb[px], 
//			p_m->wwTM[px], p_m->wwR[px]);
//	printf( "\tr= %f \t %f \t %f  ",  p_m->rho[px][0][1], p_m->rho[px][0][2], 
//		p_m->rho[px][1][2]);
//}
//if (px == 5)
//	printf( "\n %f \t %f \t %f \t %f %d", p_m->wwa[px], p_m->wwb[px], 
//			p_m->wwTM[px], p_m->wwR[px], px);
#else
//		printf( "\n %d %d %d", (int)(255 * p_m->rho[0][1]), (int)(255 * p_m->rho[0][2]), (int)(255 * p_m->rho[1][2]) );
#endif

		/* combine activated predictors	*/
		switch (pred_mode)
		{
			case 0:	/* should not happen	*/
			case 1: 
				p_m->xHat_d = p_m->xHat_a_d;	
				break; /* only A	*/
			case 2: 
				p_m->xHat_d = p_m->xHat_b_d;	
				break; /* only B	*/
			case 4: 
				p_m->xHat_d = p_m->xHat_T;		
				break; /* only TM	*/
			case 8: 
				p_m->xHat_d = p_m->xHat_L;		
				break; /* only LS	*/

			case 3: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d;

				//if (p_m->count_scaled[px])
				//{ /* covariance based blending of estimates	*/
				//	double d11, d22, d12, c11, c22, c12, minCov, meanCov, idet;

				//	/* variances	*/
				//	c11 = p_m->covAA[px] / p_m->count_scaled[px];
				//	c22 = p_m->covBB[px] / p_m->count_scaled[px];
				//	minCov = min( c11, c22); /* smallest variance	*/
				//	meanCov = sqrt( c11 * c22); /* maximum bound of covariance, Cauchy's inequality	*/
				//	/* compute covariance, limitation to a maximal value	*/ 
				//	c12 = min( p_m->covAB[px] / p_m->count_scaled[px], ( minCov * 7 + meanCov) * 0.125);
				//	c12 = min( p_m->covAB[px] / p_m->count_scaled[px], minCov);

				//	idet = 1. / c11 * c22 - c12*c12; /* determinant of covariance matrix	*/
				//	/* compute elements of inverse covariance matrix	*/
				//	d11 = c22 * idet;
				//	d22 = c11 * idet;
				//	d12 = -c12 * idet;
				//	/* correct combination of correlated estimates	*/
				//	p_m->xHat_d = (d11 * p_m->xHat_a_d + d22 * p_m->xHat_b_d + 
				//								 d12 * (p_m->xHat_a_d+p_m->xHat_b_d)) / 
				//								(d11 + d22 + d12 + d12);
				//}
				break; /* A	and B */
			case 9: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwLS[px] * p_m->xHat_L;	
				break; /*  A and LS	*/
			case 5: 
				if (p_m->zero_dist_flag)
				{ /* force TM predictor, if exact match had been found	*/
					p_m->xHat_d = p_m->xHat_T;	
				}
				else
				{
					p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwTM[px] * p_m->xHat_T;	
				}

				//	if (p_m->count_scaled[px])
				//	{ /* covariance based blending of estimates	*/
				//		double d11, d22, d12, c11, c22, c12, minCov, meanCov, idet;

				//		/* variances	*/
				//		c11 = p_m->covAA[px] / p_m->count_scaled[px];
				//		c22 = p_m->covTT[px] / p_m->count_scaled[px];
				//		minCov = min( c11, c22); /* smallest variance	*/
				//		meanCov = sqrt( c11 * c22); /* maximum bound of covariance, Cauchy's inequality	*/
				//		/* compute covariance, limitation to a maximal value	*/ 
				//		c12 = min( p_m->covAT[px] / p_m->count_scaled[px], ( minCov * 7 + meanCov) * 0.125);
				//		c12 = min( p_m->covAT[px] / p_m->count_scaled[px], minCov);

				//		idet = 1. / c11 * c22 - c12*c12; /* determinant of covariance matrix	*/
				//		/* compute elements of inverse covariance matrix	*/
				//		d11 = c22 * idet;
				//		d22 = c11 * idet;
				//		d12 = -c12 * idet;
				//		/* correct combination of correlated estimates	*/
				//		p_m->xHat_d = (d11 * p_m->xHat_a_d + d22 * p_m->xHat_T + 
				//									 d12 * (p_m->xHat_a_d+p_m->xHat_T)) / 
				//									(d11 + d22 + d12 + d12);
				//	}
				//}
				break; /*  A and TM	*/
			case 6: 
				if (p_m->zero_dist_flag)
				{ /* force TM predictor, if exact match had been found	*/
					p_m->xHat_d = p_m->xHat_T;	
				}
				else
				{
					p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T;	
				}
				break; /*  B and TM	*/


			case 7: 
				if (p_m->zero_dist_flag)
				{ /* force TM predictor, if exact match had been found	*/
					p_m->xHat_d = p_m->xHat_T;	
				}
				else
				{
					p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwTM[px] * p_m->xHat_T;	
				}
				break; /*  A, B and TM	*/


			case 16: 
				p_m->xHat_d = p_m->xHat_R_d;	
				break; /* only R	*/

			case 17: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, R	*/

			case 18: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  B, R	*/

			case 20: 
				p_m->xHat_d = p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  TM, R	*/

			case 19: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, B, R	*/

			case 21: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, TM, R	*/

			case 22: 
				if (p_m->zero_dist_flag)
				{ /* force TM predictor, if exact match had been found	*/
					p_m->xHat_d = p_m->xHat_T;	
				}
				else
				{
					p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				}
				break; /*  B, TM, R	*/

			case 23: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + 
											p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*   A, B, TM, R	*/
		}

		p_m->xHat = (long)floor( p_m->xHat_d + 0.5);
		p_m->xHat = CLIP( p_m->xHat, (signed)im->min_value, (signed)im->max_value);

		return px;
}

/*---------------------------------------------------------------
 *	meta_error()
 *---------------------------------------------------------------*/
void meta_error( META_PRED *p_m, long Rx, long RANGEd2, 
							 long RANGEm1d2, long RANGE)
{
	/* compute single prediction errors	*/
	p_m->pred_err_a = Rx - p_m->xHat_a_d;
	p_m->pred_err_b = Rx - p_m->xHat_b_d;
	p_m->pred_err_R = Rx - p_m->xHat_R_d;

	p_m->pred_err_T = Rx - p_m->xHat_T;
	p_m->pred_err_L = Rx - p_m->xHat_L;
	p_m->pred_err    = Rx - p_m->xHat; /* not necessary at decoder	*/
	/* prediction error:	-RANGEd2 ...0...RANGEm1d2
	 *		-(max_value+1)/2 ...0...(max_value)/2
	 */
	/* not necessary at decoder	*/
	p_m->p_err = MODULO_RANGE( p_m->pred_err, -RANGEd2, RANGEm1d2, RANGE);

#ifdef _DEBUG
  // output of errors , ie. true value is assumed to be zero
	//printf( "\n %f %f %f", p_m->pred_err_a, p_m->pred_err_b, p_m->pred_err_T );
#else
  // output of errors , ie. true value is assumed to be zero
	// printf( "\n %f %f %f", p_m->pred_err_a, p_m->pred_err_b, p_m->pred_err_T );
  // output of prediction  contexts
	// printf( "\n %f %f %f", p_m->pred_err_a, p_m->pred_err_b, p_m->pred_err_T );
#endif

}

/*---------------------------------------------------------------
 *	update_meta_prediction()
 *---------------------------------------------------------------*/
void update_meta_prediction( unsigned short px, unsigned short num_contexts, 
								META_PRED *p_m, int pred_mode,
								unsigned long *count_px,
								unsigned long *count_pxLS, unsigned short pxLS)
{
	float sum; /* for weighting macros	*/
	double expo, rho_off = 1.005; /* formerly 1.01 */
	float perform_a=0, perform_b=0, perform_R=0, perform_T=0, perform_L=0;

	p_m->count_scaled[px]++;
	count_px[px]++;	/* increment counter of actual prediction context	*/
	count_pxLS[pxLS]++;	/* increment counter of actual prediction context	*/


	if (px < num_contexts) /* not for first row/col constant lines */
	{
		if (pred_mode & 0x01) 
		{
			p_m->abserr_a[px] += fabs( p_m->pred_err_a); /* accumulate absolute prediction errors	*/
			p_m->covAA[px] +=  p_m->pred_err_a * p_m->pred_err_a; /* determine variance	*/
		}
		if (pred_mode & 0x02)
		{
			p_m->abserr_b[px] += fabs( p_m->pred_err_b); 
			p_m->covBB[px] +=  p_m->pred_err_b * p_m->pred_err_b;
		}
		if (pred_mode & 0x04) 
		{
			p_m->abserr_T[px] += fabs( p_m->pred_err_T); 
			p_m->covTT[px] +=  p_m->pred_err_T * p_m->pred_err_T;
		}
		if (pred_mode & 0x08) /* !!! is related to a different px(LS) !!!! */
		{
			p_m->abserr_L[px] += fabs( p_m->pred_err_L); 
			p_m->covLL[px] +=  p_m->pred_err_L * p_m->pred_err_L;
		}
		if (pred_mode & 0x10)
		{
			p_m->abserr_R[px]	+= fabs( p_m->pred_err_R);
			p_m->covRR[px] +=  p_m->pred_err_R * p_m->pred_err_R;
		}


		/* co-variances and pearsson correlation	*/
		if (pred_mode == 0x03) /* A, B-------------------------------------------	*/
		{
			p_m->covAB[px] +=  p_m->pred_err_a * p_m->pred_err_b;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAB[px] = (float)(p_m->covAB[px] / sqrt(p_m->covAA[px] * p_m->covBB[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAB[px]) ) );
				perform_a = (float)pow( p_m->abserr_b[px] / p_m->abserr_a[px], expo);
				perform_b = (float)pow( p_m->abserr_a[px] / p_m->abserr_b[px], expo);
			}
			else
			{
				perform_a = perform_b = 1;
			}
		}

		if (pred_mode == 0x05)  /* A, TM	----------------------------------------*/
		{
			p_m->covAT[px] +=  p_m->pred_err_a * p_m->pred_err_T;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAT[px] = (float)(p_m->covAT[px] /
													sqrt(p_m->covAA[px] * p_m->covTT[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
				perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
				perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
			}
			else
			{
					perform_a = perform_T = 1;
			}
		}

		if (pred_mode == 0x09)  /* A, LS	----------------------------------------*/
		{
			p_m->covAL[px] +=  p_m->pred_err_a * p_m->pred_err_L;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAL[px] = (float)(p_m->covAL[px] /
													sqrt(p_m->covAA[px] * p_m->covLL[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAL[px]) ) );
				perform_a = (float)pow( p_m->abserr_L[px] / p_m->abserr_a[px], expo);
				perform_L = (float)pow( p_m->abserr_a[px] / p_m->abserr_L[px], expo);
			}
			else
			{
					perform_a = perform_L = 1;
			}
		}

		if (pred_mode == 0x11)  /* A, R	*/
		{
			p_m->covAR[px] +=  p_m->pred_err_a * p_m->pred_err_R;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAR[px] = (float)(p_m->covAR[px] /
													sqrt(p_m->covAA[px] * p_m->covRR[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAR[px]) ) );
				perform_a = (float)pow( p_m->abserr_R[px] / p_m->abserr_a[px], expo);
				perform_R = (float)pow( p_m->abserr_a[px] / p_m->abserr_R[px], expo);
			}
			else
			{
					perform_a = perform_R = 1;
			}
		}

		if (pred_mode == 0x06)  /* B, TM	*/
		{
			p_m->covBT[px] +=  p_m->pred_err_b * p_m->pred_err_T;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoBT[px] = (float)(p_m->covBT[px] /
													sqrt(p_m->covBB[px] * p_m->covTT[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
				perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
				perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
			}
			else
			{
					perform_b = perform_T = 1;
			}
		}

		if (pred_mode == 0x12)  /* B, R	*/
		{
			p_m->covBR[px] +=  p_m->pred_err_b * p_m->pred_err_R;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoBR[px] = (float)(p_m->covBR[px] /
													sqrt(p_m->covBB[px] * p_m->covRR[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBR[px]) ) );
				perform_b = (float)pow( p_m->abserr_R[px] / p_m->abserr_b[px], expo);
				perform_R = (float)pow( p_m->abserr_b[px] / p_m->abserr_R[px], expo);
			}
			else
			{
					perform_b = perform_R = 1;
			}
		}

		if (pred_mode == 0x14)   /* TM, R	*/ 
		{
			p_m->covRT[px] +=  p_m->pred_err_T * p_m->pred_err_R;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoRT[px] = (float)(p_m->covRT[px] /
													sqrt(p_m->covTT[px] * p_m->covRR[px]));

				expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
				perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
				perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
			}
			else
			{
				perform_b = perform_R = 1;
			}
		}

		if (pred_mode == 0x07) /* A, B, TM	-----------------------------------------------*/
		{
			p_m->covAB[px] +=  p_m->pred_err_a * p_m->pred_err_b;
			p_m->covAT[px] +=  p_m->pred_err_a * p_m->pred_err_T;
			p_m->covBT[px] +=  p_m->pred_err_b * p_m->pred_err_T;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAB[px] = (float)(p_m->covAB[px] /
													sqrt(p_m->covAA[px] * p_m->covBB[px]));

				if (p_m->rhoAB[px] > 0.8)
				{	
					/* strong correlation between predictor A and B
					 * use the better one in combination with TM
					 */
					if (p_m->abserr_a[px] <= p_m->abserr_b[px])
					{	/* A, TM	*/
						p_m->rhoAT[px] = (float)(p_m->covAT[px] /
															sqrt(p_m->covAA[px] * p_m->covTT[px]));

						expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
						perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
						perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
					}
					else
					{	/* B, TM	*/
						p_m->rhoBT[px] = (float)(p_m->covBT[px] /
															sqrt(p_m->covBB[px] * p_m->covTT[px]));

						expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
						perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
						perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
					}
				}
				else
				{
					double expoAB, expoAT, expoBT;
					p_m->rhoAT[px] = (float)(p_m->covAT[px] /
													sqrt(p_m->covAA[px] * p_m->covTT[px]));
					p_m->rhoBT[px] = (float)(p_m->covBT[px] /
													sqrt(p_m->covBB[px] * p_m->covTT[px]));
					expoAB = min( 16, 1. / (rho_off -  fabs( p_m->rhoAB[px]) ) );
					expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
					expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
					perform_a = (float)(pow( p_m->abserr_b[px] / p_m->abserr_a[px], expoAB) * 
															pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
					perform_b = (float)(pow( p_m->abserr_a[px] / p_m->abserr_b[px], expoAB) * 
															pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
					perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
															pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT));
				}
			}
			else
			{
				perform_a = perform_b = perform_T = 0.33333333333F;
			}
		}


		if (pred_mode == 0x15) /* A, R, TM	-----------------------------------------------*/
		{
			p_m->covAR[px] +=  p_m->pred_err_a * p_m->pred_err_R;
			p_m->covAT[px] +=  p_m->pred_err_a * p_m->pred_err_T;
			p_m->covRT[px] +=  p_m->pred_err_R * p_m->pred_err_T;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAR[px] = (float)(p_m->covAR[px] /
													sqrt(p_m->covAA[px] * p_m->covRR[px]));

				if (p_m->rhoAR[px] > 0.8)
				{	
					/* strong correlation between predictor A and R
					 * use the better one in combination with TM
					 */
					if (p_m->abserr_a[px] <= p_m->abserr_R[px])
					{	/* A, TM */
						p_m->rhoAT[px] = (float)(p_m->covAT[px] /
															sqrt(p_m->covAA[px] * p_m->covTT[px]));

						expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
						perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
						perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
					}
					else
					{
						/* R snd T	*/
						p_m->rhoRT[px] = (float)(p_m->covRT[px] /
															sqrt(p_m->covTT[px] * p_m->covRR[px]));

						expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
						perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
						perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
					}
				}
				else
				{
					double expoAR, expoAT, expoRT;
					p_m->rhoAT[px] = (float)(p_m->covAT[px] /
													sqrt(p_m->covAA[px] * p_m->covTT[px]));
					p_m->rhoRT[px] = (float)(p_m->covRT[px] /
													sqrt(p_m->covRR[px] * p_m->covTT[px]));
					expoAR = min( 16, 1. / (rho_off -  fabs( p_m->rhoAR[px]) ) );
					expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
					expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
					perform_a = (float)(pow( p_m->abserr_R[px] / p_m->abserr_a[px], expoAR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
					perform_R = (float)(pow( p_m->abserr_a[px] / p_m->abserr_R[px], expoAR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
					perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
															pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
				}
			}
			else
			{
				perform_a = perform_R = perform_T = 0.33333333333F;
			}
		}


		if (pred_mode == 0x16) /* B, R, TM	-----------------------------------------------*/
		{
			p_m->covBR[px] +=  p_m->pred_err_b * p_m->pred_err_R;
			p_m->covBT[px] +=  p_m->pred_err_b * p_m->pred_err_T;
			p_m->covRT[px] +=  p_m->pred_err_R * p_m->pred_err_T;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoBR[px] = (float)(p_m->covBR[px] /
													sqrt(p_m->covBB[px] * p_m->covRR[px]));

				if (p_m->rhoBR[px] > 0.8)
				{	
					/* strong correlation between predictor B and R
					 * use the better one in combination with TM
					 */
					if (p_m->abserr_b[px] <= p_m->abserr_R[px])
					{ /* B, TM	*/
						p_m->rhoBT[px] = (float)(p_m->covBT[px] /
															sqrt(p_m->covBB[px] * p_m->covTT[px]));

						expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) ) ;
						perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
						perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
					}
					else
					{	/* R and TM	*/
						p_m->rhoRT[px] = (float)(p_m->covRT[px] /
															sqrt(p_m->covTT[px] * p_m->covRR[px]));

						expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
						perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
						perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
					}
				}
				else
				{
					double expoBR, expoBT, expoRT;
					p_m->rhoBT[px] = (float)(p_m->covBT[px] /
													sqrt(p_m->covBB[px] * p_m->covTT[px]));
					p_m->rhoRT[px] = (float)(p_m->covRT[px] / 
												sqrt(p_m->covRR[px] * p_m->covTT[px]));
					expoBR = min( 16, 1. / (rho_off -  fabs( p_m->rhoBR[px]) ) );
					expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
					expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
					perform_b = (float)(pow( p_m->abserr_R[px] / p_m->abserr_b[px], expoBR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
					perform_R = (float)(pow( p_m->abserr_b[px] / p_m->abserr_R[px], expoBR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
					perform_T = (float)(pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT) * 
															pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
				}
			}
			else
			{
				perform_b = perform_R = perform_T = 0.33333333333F;
			}
		}


		if (pred_mode == 0x13) /* A, B, R ---------------------------------------	*/
		{
			p_m->covAB[px] +=  p_m->pred_err_a * p_m->pred_err_b;
			p_m->covAR[px] +=  p_m->pred_err_a * p_m->pred_err_R;
			p_m->covBR[px] +=  p_m->pred_err_b * p_m->pred_err_R;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				double expoAB, expoAR, expoBR;
				p_m->rhoAB[px] = (float)(p_m->covAB[px] /
												sqrt(p_m->covAA[px] * p_m->covBB[px]));

				p_m->rhoAR[px] = (float)(p_m->covAR[px] /
												sqrt(p_m->covAA[px] * p_m->covRR[px]));
				p_m->rhoBR[px] = (float)(p_m->covBR[px] /
												sqrt(p_m->covBB[px] * p_m->covRR[px]));
				expoAB = min( 16, 1. / (rho_off -  fabs( p_m->rhoAB[px]) ) );
				expoAR = min( 16, 1. / (rho_off -  fabs( p_m->rhoAR[px]) ) );
				expoBR = min( 16, 1. / (rho_off -  fabs( p_m->rhoBR[px]) ) );
				perform_a = (float)(pow( p_m->abserr_b[px] / p_m->abserr_a[px], expoAB) * 
														pow( p_m->abserr_R[px] / p_m->abserr_a[px], expoAR));
				perform_b = (float)(pow( p_m->abserr_a[px] / p_m->abserr_b[px], expoAB) * 
														pow( p_m->abserr_R[px] / p_m->abserr_b[px], expoBR));
				perform_R = (float)(pow( p_m->abserr_a[px] / p_m->abserr_R[px], expoAR) * 
														pow( p_m->abserr_b[px] / p_m->abserr_R[px], expoBR));
			}
			else
			{
				perform_a = perform_b = perform_R = 0.33333333333F;
			}
		}


		if (pred_mode == 0x17) /* A, B, TM, R	*/
		{
			p_m->covAB[px] +=  p_m->pred_err_a * p_m->pred_err_b;
			p_m->covAR[px] +=  p_m->pred_err_a * p_m->pred_err_R;
			p_m->covAT[px] +=  p_m->pred_err_a * p_m->pred_err_T;
			p_m->covBR[px] +=  p_m->pred_err_b * p_m->pred_err_R;
			p_m->covBT[px] +=  p_m->pred_err_b * p_m->pred_err_T;
			p_m->covRT[px] +=  p_m->pred_err_R * p_m->pred_err_T;
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				p_m->rhoAB[px] = (float)(p_m->covAB[px] /
													sqrt(p_m->covAA[px] * p_m->covBB[px]));
				p_m->rhoAR[px] = (float)(p_m->covAR[px] /
													sqrt(p_m->covAA[px] * p_m->covRR[px]));
				p_m->rhoBR[px] = (float)(p_m->covBR[px] /
													sqrt(p_m->covBB[px] * p_m->covRR[px]));

				if (p_m->rhoAB[px] > 0.8)
				{	
					/* strong correlation between predictor A and B
					 * use the better one in combination with TM
					 */
					if (p_m->abserr_a[px] <= p_m->abserr_b[px])
					{
						/* A, R snd T	*/
						p_m->rhoAR[px] = (float)(p_m->covAR[px] /
															sqrt(p_m->covAA[px] * p_m->covRR[px]));

						if (p_m->rhoAR[px] > 0.8)
						{	
							/* strong correlation between predictor A and R
							 * use the better one in combination with TM
							 */
							if (p_m->abserr_a[px] <= p_m->abserr_R[px])
							{	/* A, TM */
								p_m->rhoAT[px] = (float)(p_m->covAT[px] /
																	sqrt(p_m->covAA[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
								perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
								perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
							}
							else
							{
								/* R snd T	*/
								p_m->rhoRT[px] = (float)(p_m->covRT[px] /
																	sqrt(p_m->covTT[px] * p_m->covRR[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
								perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
								perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
							}
						}
						else
						{
							double expoAR, expoAT, expoRT;
							p_m->rhoAT[px] = (float)(p_m->covAT[px] /
															sqrt(p_m->covAA[px] * p_m->covTT[px]));
							p_m->rhoRT[px] = (float)(p_m->covRT[px] /
															sqrt(p_m->covRR[px] * p_m->covTT[px]));
							expoAR = min( 16, 1. / (rho_off -  fabs( p_m->rhoAR[px]) ) );
							expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
							expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
							perform_a = (float)(pow( p_m->abserr_R[px] / p_m->abserr_a[px], expoAR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
							perform_R = (float)(pow( p_m->abserr_a[px] / p_m->abserr_R[px], expoAR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
							perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
																	pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
						}
					}
					else
					{
						/* B, R snd T	*/
						p_m->rhoBR[px] = (float)(p_m->covBR[px] /
															sqrt(p_m->covBB[px] * p_m->covRR[px]));

						if (p_m->rhoBR[px] > 0.8)
						{	
							/* strong correlation between predictor B and R
							 * use the better one in combination with TM
							 */
							if (p_m->abserr_b[px] <= p_m->abserr_R[px])
							{ /* B, TM	*/
								p_m->rhoBT[px] = (float)(p_m->covBT[px] /
																	sqrt(p_m->covBB[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
								perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
								perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
							}
							else
							{	/* R and TM	*/
								p_m->rhoRT[px] = (float)(p_m->covRT[px] /
																	sqrt(p_m->covTT[px] * p_m->covRR[px]));

								min( 16, expo = 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
								perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
								perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
							}
						}
						else
						{
							double expoBR, expoBT, expoRT;
							p_m->rhoBT[px] = (float)(p_m->covBT[px] /
															sqrt(p_m->covBB[px] * p_m->covTT[px]));
							p_m->rhoRT[px] = (float)(p_m->covRT[px] /
															sqrt(p_m->covRR[px] * p_m->covTT[px]));
							expoBR = min( 16, 1. / (rho_off -  fabs( p_m->rhoBR[px]) ) );
							expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
							expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
							perform_b = (float)(pow( p_m->abserr_R[px] / p_m->abserr_b[px], expoBR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
							perform_R = (float)(pow( p_m->abserr_b[px] / p_m->abserr_R[px], expoBR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
							perform_T = (float)(pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT) * 
																	pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
						}
					}
				}
				else	if (p_m->rhoAR[px] > 0.8)
				{	
					/* strong correlation between predictor A and R
					 * use the better one in combination with TM
					 */
					if (p_m->abserr_a[px] <= p_m->abserr_R[px])
					{
						/* A, B snd T	*/
						p_m->rhoAB[px] = (float)(p_m->covAB[px] /
															sqrt(p_m->covAA[px] * p_m->covBB[px]));

						if (p_m->rhoAB[px] > 0.8)
						{	
							/* strong correlation between predictor A and B
							 * use the better one in combination with TM
							 */
							if (p_m->abserr_a[px] <= p_m->abserr_b[px])
							{	/* A, TM	*/
								p_m->rhoAT[px] = (float)(p_m->covAT[px] /
																	sqrt(p_m->covAA[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
								perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
								perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
							}
							else
							{	/* B, TM	*/
								p_m->rhoBT[px] = (float)(p_m->covBT[px] /
																	sqrt(p_m->covBB[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
								perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
								perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
							}
						}
						else
						{
							double expoAB, expoAT, expoBT;
							p_m->rhoAT[px] = (float)(p_m->covAT[px] /
															sqrt(p_m->covAA[px] * p_m->covTT[px]));
							p_m->rhoBT[px] = (float)(p_m->covBT[px] /
															sqrt(p_m->covBB[px] * p_m->covTT[px]));
							expoAB = min( 16, 1. / (rho_off -  fabs( p_m->rhoAB[px]) ) );
							expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
							expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
							perform_a = (float)(pow( p_m->abserr_b[px] / p_m->abserr_a[px], expoAB) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
							perform_b = (float)(pow( p_m->abserr_a[px] / p_m->abserr_b[px], expoAB) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
							perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
																	pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT));
						}
					}
					else
					{
						/* B, R snd T	*/
						p_m->rhoBR[px] = (float)(p_m->covBR[px] /
															sqrt(p_m->covBB[px] * p_m->covRR[px]));

						if (p_m->rhoBR[px] > 0.8)
						{	
							/* strong correlation between predictor B and R
							 * use the better one in combination with TM
							 */
							if (p_m->abserr_b[px] <= p_m->abserr_R[px])
							{ /* B, TM	*/
								p_m->rhoBT[px] = (float)(p_m->covBT[px] /
																	sqrt(p_m->covBB[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) ) ;
								perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
								perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
							}
							else
							{	/* R and TM	*/
								p_m->rhoRT[px] = (float)(p_m->covRT[px] /
																	sqrt(p_m->covTT[px] * p_m->covRR[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) ) ;
								perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
								perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
							}
						}
						else
						{
							double expoBR, expoBT, expoRT;
							p_m->rhoBT[px] = (float)(p_m->covBT[px] /
															sqrt(p_m->covBB[px] * p_m->covTT[px]));
							p_m->rhoRT[px] = (float)(p_m->covRT[px] /
															sqrt(p_m->covRR[px] * p_m->covTT[px]));
							expoBR = min( 16, 1. / (rho_off -  fabs( p_m->rhoBR[px]) ) );
							expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
							expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
							perform_b = (float)(pow( p_m->abserr_R[px] / p_m->abserr_b[px], expoBR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
							perform_R = (float)(pow( p_m->abserr_b[px] / p_m->abserr_R[px], expoBR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
							perform_T = (float)(pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT) * 
																	pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
						}
					}
				}
				else	if (p_m->rhoBR[px] > 0.8)
				{	
					if (p_m->abserr_b[px] <= p_m->abserr_R[px])
					{
						/* A, B snd T	*/
						p_m->rhoAB[px] = (float)(p_m->covAB[px] /
															sqrt(p_m->covAA[px] * p_m->covBB[px]));

						if (p_m->rhoAB[px] > 0.8)
						{	
							/* strong correlation between predictor A and B
							 * use the better one in combination with TM
							 */
							if (p_m->abserr_a[px] <= p_m->abserr_b[px])
							{	/* A, TM	*/
								p_m->rhoAT[px] = (float)(p_m->covAT[px] /
																	sqrt(p_m->covAA[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
								perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
								perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
							}
							else
							{	/* B, TM	*/
								p_m->rhoBT[px] = (float)(p_m->covBT[px] /
																	sqrt(p_m->covBB[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
								perform_b = (float)pow( p_m->abserr_T[px] / p_m->abserr_b[px], expo);
								perform_T = (float)pow( p_m->abserr_b[px] / p_m->abserr_T[px], expo);
							}
						}
						else
						{
							double expoAB, expoAT, expoBT;
							p_m->rhoAT[px] = (float)(p_m->covAT[px] /
															sqrt(p_m->covAA[px] * p_m->covTT[px]));
							p_m->rhoBT[px] = (float)(p_m->covBT[px] /
															sqrt(p_m->covBB[px] * p_m->covTT[px]));
							expoAB = min( 16, 1. / (rho_off -  fabs( p_m->rhoAB[px]) ) );
							expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
							expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
							perform_a = (float)(pow( p_m->abserr_b[px] / p_m->abserr_a[px], expoAB) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
							perform_b = (float)(pow( p_m->abserr_a[px] / p_m->abserr_b[px], expoAB) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
							perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
																	pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT));
						}
					}
					else
					{
						/* A, R snd T	*/
						p_m->rhoAR[px] = (float)(p_m->covAR[px] /
															sqrt(p_m->covAA[px] * p_m->covRR[px]));

						if (p_m->rhoAR[px] > 0.8)
						{	
							/* strong correlation between predictor A and R
							 * use the better one in combination with TM
							 */
							if (p_m->abserr_a[px] <= p_m->abserr_R[px])
							{	/* A, TM */
								p_m->rhoAT[px] = (float)(p_m->covAT[px] /
																	sqrt(p_m->covAA[px] * p_m->covTT[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
								perform_a = (float)pow( p_m->abserr_T[px] / p_m->abserr_a[px], expo);
								perform_T = (float)pow( p_m->abserr_a[px] / p_m->abserr_T[px], expo);
							}
							else
							{
								/* R snd T	*/
								p_m->rhoRT[px] = (float)(p_m->covRT[px] /
																	sqrt(p_m->covTT[px] * p_m->covRR[px]));

								expo = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
								perform_T = (float)pow( p_m->abserr_R[px] / p_m->abserr_T[px], expo);
								perform_R = (float)pow( p_m->abserr_T[px] / p_m->abserr_R[px], expo);
							}
						}
						else
						{
							double expoAR, expoAT, expoRT;
							p_m->rhoAT[px] = (float)(p_m->covAT[px] /
															sqrt(p_m->covAA[px] * p_m->covTT[px]));
							p_m->rhoRT[px] = (float)(p_m->covRT[px] /
															sqrt(p_m->covRR[px] * p_m->covTT[px]));
							expoAR = min( 16, 1. / (rho_off -  fabs( p_m->rhoAR[px]) ) );
							expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
							expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );
							perform_a = (float)(pow( p_m->abserr_R[px] / p_m->abserr_a[px], expoAR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
							perform_R = (float)(pow( p_m->abserr_a[px] / p_m->abserr_R[px], expoAR) * 
																	pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
							perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
																	pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
						}
					}
				}
				else
				{
					double expoAB, expoAR, expoAT;
					double expoBR, expoBT, expoRT;
					p_m->rhoAT[px] = (float)(p_m->covAT[px] /
													sqrt(p_m->covAA[px] * p_m->covTT[px]));
					p_m->rhoBT[px] = (float)(p_m->covBT[px] /
													sqrt(p_m->covBB[px] * p_m->covTT[px]));
					p_m->rhoRT[px] = (float)(p_m->covRT[px] /
													sqrt(p_m->covRR[px] * p_m->covTT[px]));
					expoAB = min( 16, 1. / (rho_off -  fabs( p_m->rhoAB[px]) ) );
					expoAR = min( 16, 1. / (rho_off -  fabs( p_m->rhoAR[px]) ) );
					expoAT = min( 16, 1. / (rho_off -  fabs( p_m->rhoAT[px]) ) );
					expoBR = min( 16, 1. / (rho_off -  fabs( p_m->rhoBR[px]) ) );
					expoBT = min( 16, 1. / (rho_off -  fabs( p_m->rhoBT[px]) ) );
					expoRT = min( 16, 1. / (rho_off -  fabs( p_m->rhoRT[px]) ) );

					perform_a = (float)(pow( p_m->abserr_b[px] / p_m->abserr_a[px], expoAB) * 
															pow( p_m->abserr_R[px] / p_m->abserr_a[px], expoAR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_a[px], expoAT));
					perform_b = (float)(pow( p_m->abserr_a[px] / p_m->abserr_b[px], expoAB) * 
															pow( p_m->abserr_R[px] / p_m->abserr_b[px], expoBR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_b[px], expoBT));
					perform_R = (float)(pow( p_m->abserr_a[px] / p_m->abserr_R[px], expoAR) * 
															pow( p_m->abserr_b[px] / p_m->abserr_R[px], expoBR) * 
															pow( p_m->abserr_T[px] / p_m->abserr_R[px], expoRT));
					perform_T = (float)(pow( p_m->abserr_a[px] / p_m->abserr_T[px], expoAT) * 
															pow( p_m->abserr_b[px] / p_m->abserr_T[px], expoBT) * 
															pow( p_m->abserr_R[px] / p_m->abserr_T[px], expoRT));
				}
			}
			else
			{
				perform_a = perform_b = perform_R = perform_T = 0.25F;
			}
		}

		/* scaling of abserr	*/
		// if ( (count_px[px] % 256) == 0)
		if ( p_m->count_scaled[px] == 128)
		{
			p_m->abserr_a[px] *= 0.5F; if (p_m->abserr_a[px] < 0.00001F) p_m->abserr_a[px] = 0.00001F;
			p_m->abserr_b[px] *= 0.5F; if (p_m->abserr_b[px] < 0.00001F) p_m->abserr_b[px] = 0.00001F;
			p_m->abserr_R[px] *= 0.5F; if (p_m->abserr_R[px] < 0.00001F) p_m->abserr_R[px] = 0.00001F;
			p_m->abserr_T[px] *= 0.5F; if (p_m->abserr_T[px] < 0.00001F) p_m->abserr_T[px] = 0.00001F;
			p_m->abserr_L[px] *= 0.5F; if (p_m->abserr_L[px] < 0.00001F) p_m->abserr_L[px] = 0.00001F;

			/* avoid zeros as the variances are used as divisors	*/
			p_m->covAA[px] *= 0.5F; if (p_m->covAA[px] < 0.00001F) p_m->covAA[px] = 0.00001F;
			p_m->covBB[px] *= 0.5F; if (p_m->covBB[px] < 0.00001F) p_m->covBB[px] = 0.00001F;
			p_m->covRR[px] *= 0.5F; if (p_m->covRR[px] < 0.00001F) p_m->covRR[px] = 0.00001F;
			p_m->covTT[px] *= 0.5F; if (p_m->covTT[px] < 0.00001F) p_m->covTT[px] = 0.00001F;
			p_m->covLL[px] *= 0.5F; if (p_m->covLL[px] < 0.00001F) p_m->covLL[px] = 0.00001F;
			p_m->covAB[px] *= 0.5F;
			p_m->covAR[px] *= 0.5F;
			p_m->covAT[px] *= 0.5F;
			p_m->covBR[px] *= 0.5F;
			p_m->covBT[px] *= 0.5F;
			p_m->covRT[px] *= 0.5F;

			p_m->count_scaled[px] >>= 1;
		}

		/* do predictor blending	*/
		switch (pred_mode)
		{
			case 3:
				WEIGTHING2( perform_a, perform_b, p_m->wwa[px], p_m->wwb[px])
				break;
			case 5:
				WEIGTHING2( perform_a, perform_T, p_m->wwa[px], p_m->wwTM[px])
				break;
			case 6:
				WEIGTHING2( perform_b, perform_T, p_m->wwb[px], p_m->wwTM[px])
				break;


			case 7:
				WEIGTHING3( perform_a, perform_b, perform_T, 
					p_m->wwa[px], p_m->wwb[px], p_m->wwTM[px])
				break;

			case 9:
				WEIGTHING2( perform_a, perform_L, p_m->wwa[px], p_m->wwLS[px])
				break;

			case 23: /* A, B, TM, R	*/
				WEIGTHING4( perform_a, perform_b, perform_T, perform_R, 
					p_m->wwa[px], p_m->wwb[px], p_m->wwTM[px], p_m->wwR[px])
				break;

			case 19: /* A, B, R	*/
				WEIGTHING3(perform_a, perform_b, perform_R, 
					p_m->wwa[px], p_m->wwb[px], p_m->wwR[px])
				break;

			case 21: /* A, TM, R	*/
				WEIGTHING3(perform_a, perform_T, perform_R, 
					p_m->wwa[px], p_m->wwTM[px], p_m->wwR[px])
				break;

			case 22: /* B, TM, R	*/
				WEIGTHING3(perform_b, perform_T, perform_R, 
					p_m->wwb[px], p_m->wwTM[px], p_m->wwR[px])
				break;


			case 17: /* A, R	*/
				WEIGTHING2(perform_a,perform_R, 
					p_m->wwa[px], p_m->wwR[px])
				break;

			case 18: /* B, R	*/
				WEIGTHING2(perform_b,perform_R, 
									 p_m->wwb[px], p_m->wwR[px])
				break;

			case 20: /* TM, R	*/
				WEIGTHING2(perform_T,perform_R, 
									 p_m->wwTM[px], p_m->wwR[px])
				break;

			default : sum = 0;
		}

		/* accumulate prediction errors for each context	*/
		if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
		{
			/* for bias compensation	*/
			if (pred_mode & 0x01) p_m->sum_err_a[px] += p_m->pred_err_a;	
			if (pred_mode & 0x02) p_m->sum_err_b[px] += p_m->pred_err_b;
			if (pred_mode & 0x04) p_m->sum_err_T[px] += p_m->pred_err_T;
			if (pred_mode & 0x10) p_m->sum_err_R[px] += p_m->pred_err_R;

			/* only for statistics	*/
			if (pred_mode & 0x01) p_m->sum_abserr_a[px] += (float)fabs( p_m->pred_err_a);
			if (pred_mode & 0x02) p_m->sum_abserr_b[px] += (float)fabs( p_m->pred_err_b);
			if (pred_mode & 0x10) p_m->sum_abserr_R[px] += (float)fabs( p_m->pred_err_R);
			/* essential	*/
			p_m->sum_abserr[px] += abs( p_m->pred_err); 
		}
		/* total (sum) prediction error, used to compute the average	*/
		p_m->mean_tot += p_m->pred_err; 
	} /* if (px < num_contexts)*/

	if (count_pxLS[pxLS] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
	{
		if (pred_mode & 0x08) p_m->sum_err_L[pxLS] += p_m->pred_err_L;
	}
	if (count_pxLS[pxLS] == 128)
	{
		count_pxLS[pxLS] = count_pxLS[pxLS] >> 1;
		p_m->sum_err_L[pxLS] = p_m->sum_err_L[pxLS] * 0.5F;
	}
}
