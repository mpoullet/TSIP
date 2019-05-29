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
#include "med.h"

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
						sum =  max( 0.000000001, a + b);		\
						wa = a / sum;		\
						wb = b / sum;		\
						//if (wa < 0.25)		\
						//	{ wa = 0.; wb = 1.;}\
						//else if (wb < 0.25) \
						//		{ wb = 0.; wa = 1.;}

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
							sum = max( 0.000000001, a + b + c * 1.0);		\
							wa = a / sum;				\
							wb = b / sum;				\
							wc = c / sum;				\
}

#define WEIGTHING3_off( a, b, c, wa, wb, wc)\
{															\
							sum = max( 0.000000001, a + b + c * 1.0);		\
							wa = a / sum;				\
							wb = b / sum;				\
							wc = c / sum;				\
		if (wa < -1)	\
		{ double off;				\
			off = (-wa - 1) * 0.5;			\
			wa += off * 2; wb -= off; wc -= off;\
		}				\
		if (wb < -1)	\
		{ double off;				\
			off = (-wb - 1) * 0.5;			\
			wb += off * 2; wa -= off; wc -= off;\
		}				\
		if (wc < -1)	\
		{ double off;				\
			off = (-wc - 1) * 0.5;			\
			wc += off * 2; wa -= off; wb -= off;\
		}				\
}

#define WEIGTHING4( a, b, c, d, wa, wb, wc, wd) \
			{																\
								sum = a + b + c + d * 1.0;		\
								wa = a / sum;				\
								wb = b / sum;				\
								wc = c / sum;				\
								wd = d / sum;				\
			}															\

#define WEIGTHING5( a, b, c, d, e, wa, wb, wc, wd, we) \
			{																\
								sum = a + b + c + d + e*1.0;		\
								wa = a / sum;				\
								wb = b / sum;				\
								wc = c / sum;				\
								wd = d / sum;				\
								we = e / sum;				\
			}															\

/*---------------------------------------------------------------
 *	init_meta_prediction()
 *---------------------------------------------------------------*/
int init_meta_prediction( META_PRED *p_meta, int pred_mode)
{
	char *rtn="init_prediction";
	int err = 0;
	unsigned short k, i, j;

	/* mean value of entire prediction-error image	*/
	p_meta->mean_tot = 0.0;

	for ( k = 0; k < NUM_OF_PRED_CONTEXTS; k++)
	{
		p_meta->sum_err_a[k] = 0.;	/* error sum of predictor A	*/
		p_meta->sum_err_b[k] = 0.;	/* error sum of predictor B	*/
		p_meta->sum_err_R[k] = 0.;
		p_meta->sum_err_T[k] = 0.;
		p_meta->sum_err_M[k] = 0.;
		p_meta->count_scaled[k] = 0;

		for ( i = 0; i < NUM_PREDICTORS; i++)
		{
			for ( j = 0; j < NUM_PREDICTORS; j++)
			{
				// be aware of division by zero !
				p_meta->cov[k][i][j] = 0.0;
				//p_meta->sgncorr[k][i][j] = 0;
			}
		}
		p_meta->sum_abserr_a[k] = 0.;	/* for statistics	*/
		p_meta->sum_abserr_b[k] = 0.;
		p_meta->sum_abserr_R[k] = 0.;
		p_meta->sum_abserr[k] = 0;

		//p_meta->abserr_a[k] = 0.001;
		//p_meta->abserr_b[k] = 0.001;
		//p_meta->abserr_R[k] = 0.001;
		//p_meta->abserr_T[k] = 0.001;
		//p_meta->abserr_M[k] = 0.001;

		/* set initial weights for predictor blending	*/
		switch (pred_mode)
		{
			case 0:	/* should not happen	*/
			case 1: 
				p_meta->wwa[k] = 1.0; 
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.0;
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;	
				break; /* only A	*/
			case 2: 
				p_meta->wwb[k] = 1.0; 
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.0;
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;	
				break; /* only B	*/
			case 4: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.0;
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;	
				p_meta->wwTM[k] = 1.0;	
				break; /* only TM	*/
			case 8: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.0;
				p_meta->wwTM[k] = p_meta->wwR[k] = 0.0;
				p_meta->wwMED[k] = 1.0;	
				break; /* only MED	*/

			case 3: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.5;	
				p_meta->wwTM[k] = p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;	
				break; /* A	and B */
			case 5: 
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.5;	
				p_meta->wwb[k] = p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;	
				break; /*  A and TM	*/
			case 6: 
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.5;	
				p_meta->wwa[k] = p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;	
				break; /*  B and TM	*/

			case 9: 
				p_meta->wwa[k] = p_meta->wwMED[k] = 0.5;	
				p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwR[k] = 0.0;	
				break; /*  A and MED	*/

			case 10: 
				p_meta->wwb[k] = p_meta->wwMED[k] = 0.5;	
				p_meta->wwa[k] = p_meta->wwTM[k] = p_meta->wwR[k] = 0.0;	
				break; /*  B and MED	*/

			case 12: 
				p_meta->wwTM[k] = p_meta->wwMED[k] = 0.5;	
				p_meta->wwa[k] = p_meta->wwb[k] = p_meta->wwR[k] = 0.0;	
				break; /*  TM and MED	*/

			case 7: 
				p_meta->wwa[k] = p_meta->wwb[k] =	p_meta->wwTM[k] = 1.0/3.;	
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.0;
				break; /*  A, B and TM	*/

			case 11: 
				p_meta->wwa[k] = p_meta->wwb[k] =	p_meta->wwMED[k] = 1.0/3.;	
				p_meta->wwTM[k] = p_meta->wwR[k] = 0.0;
				break; /*  A, B and MED	*/

			case 13: 
				p_meta->wwa[k] = p_meta->wwTM[k] = p_meta->wwMED[k] = 1.0/3.;	
				p_meta->wwb[k] = p_meta->wwR[k] = 0.0;
				break; /*  A, TM and MED	*/

			case 14: 
				p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwMED[k] = 1.0/3.;	
				p_meta->wwa[k] = p_meta->wwR[k] = 0.0;
				break; /*  B, TM and MED	*/

			case 15: 
				p_meta->wwa[k] =	p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwMED[k] = 0.25;	
				p_meta->wwR[k] = 0.0;
				break; /*  A, B, TM and MED	*/

			case 16: 
				p_meta->wwa[k] =	p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwR[k] = 1.0;
				break; /* only R	*/

			case 17: 
				p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwa[k] =	p_meta->wwR[k] = 0.5;
				break; /*  A, R	*/

			case 18: 
				p_meta->wwa[k] = p_meta->wwTM[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwb[k] =	p_meta->wwR[k] = 0.5;
				break; /*  B, R	*/

			case 20: 
				p_meta->wwa[k] = p_meta->wwb[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwTM[k] =	p_meta->wwR[k] = 0.5;
				break; /*  TM, R	*/

			case 24: 
				p_meta->wwa[k] = p_meta->wwb[k] = p_meta->wwTM[k] = 0.0;	
				p_meta->wwMED[k] =	p_meta->wwR[k] = 0.5;
				break; /* MED, R	*/

			case 19: 
				p_meta->wwTM[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwb[k] = p_meta->wwR[k] = 1.0/3.;
				break; /* A, B, R	*/

			case 21: 
				p_meta->wwb[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwTM[k] = p_meta->wwR[k] = 1.0/3.;
				break; /* A, TM, R	*/

			case 25: 
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwMED[k] = p_meta->wwR[k] = 1.0/3.;
				break; /* A, MED, R	*/

			case 22: 
				p_meta->wwa[k] = p_meta->wwMED[k] = 0.0;	
				p_meta->wwb[k] = p_meta->wwTM[k] = p_meta->wwR[k] = 1.0/3.;
				break; /* B, TM, R	*/

			case 26: 
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.0;	
				p_meta->wwb[k] = p_meta->wwMED[k] = p_meta->wwR[k] = 1.0/3.;
				break; /* B, MED, R	*/

			case 28: 
				p_meta->wwa[k] = p_meta->wwb[k] = 0.0;	
				p_meta->wwTM[k] = p_meta->wwMED[k] = p_meta->wwR[k] = 1.0/3.;
				break; /* TM, MED, R	*/

			case 27: 
				p_meta->wwTM[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwb[k] = 0.25;
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.25;
				break; /* A, B, MED, R	*/

			case 23: 
				p_meta->wwMED[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwb[k] = 0.25;
				p_meta->wwTM[k] = p_meta->wwR[k] = 0.25;
				break; /* A, B, TM, R	*/

			case 29: 
				p_meta->wwb[k] = 0.0;	
				p_meta->wwa[k] = p_meta->wwTM[k] = 0.25;
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.25;
				break; /* A, MED, TM, R	*/

			case 30: 
				p_meta->wwa[k] = 0.0;	
				p_meta->wwb[k] = p_meta->wwTM[k] = 0.25;
				p_meta->wwMED[k] = p_meta->wwR[k] = 0.25;
				break; /* B, MED, TM, R	*/

			case 31: 
				p_meta->wwa[k] = 1./5;	
				p_meta->wwb[k] = p_meta->wwTM[k] = 1./5;
				p_meta->wwMED[k] = p_meta->wwR[k] = 1./5;
				break; /* A, B, MED, TM, R	*/

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
							unsigned long posExt,	/* current position in extended array	*/
							int r, int c,		/* coordinates in image array	*/
							TEMPLATE *templ,
							META_PRED *p_m, PRED2 *p_pred2, 
								int pred_mode, unsigned short num_contexts,
								unsigned long *count_px, LEAST_SQUARES *p_ls)
{
	unsigned short px;

		/* determine context and prediction value	*/
		px = get_context( r, c, num_contexts, templ);
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
				p_m->xHat_T = predictorTM( imExt, lptr, posExt, r, c, p_ls);
			else p_m->xHat_T = 0;

			if (pred_mode & 0x8)	
				p_m->xHat_M = med_predictor_( templ, (long)(im->min_value), (long)(im->max_value));
			else	p_m->xHat_M = 0; /* should never be used	*/

			if (pred_mode & 0x10)	
				p_m->xHat_R_d = predictorR( p_pred2, templ, px, im->min_value, im->max_value);
			else	p_m->xHat_R_d = 0; /* should never be used	*/
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
			if (pred_mode & 0x8)	
			{
				p_m->xHat_M += p_m->sum_err_M[px] / (count_px[px]-NUM_SKIPPED_STEPS);
			}
			if (pred_mode & 0x10)	
			{
				p_m->xHat_R_d += p_m->sum_err_R[px] / (count_px[px]-NUM_SKIPPED_STEPS);
			}
		}
		//p_m->xHat_M = p_m->xHat_R_d;

#ifdef _DEBUG
if (px == 5)
	printf( "\n %f \t %f \t %f \t %f %d", p_m->wwa[px], p_m->wwb[px], 
			p_m->wwTM[px], p_m->wwR[px], px);
#else
		printf( "\n %d %d %d", (int)(255 * p_m->rho[0][1]), (int)(255 * p_m->rho[0][2]), (int)(255 * p_m->rho[1][2]) );
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
				p_m->xHat_d = p_m->xHat_M;		
				break; /* only MED	*/
			case 3: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d;	
				break; /* A	and B */
			case 5: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwTM[px] * p_m->xHat_T;	
				break; /*  A and TM	*/
			case 6: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T;	
				break; /*  B and TM	*/
			case 9: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  A and MED	*/
			case 10: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  B and MED	*/
			case 12: 
				p_m->xHat_d = p_m->wwTM[px] * p_m->xHat_T + p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  TM and MED	*/

			case 7: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwTM[px] * p_m->xHat_T;	
				break; /*  A, B and TM	*/

			case 11: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  A, B and MED	*/

			case 13: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  A, TM and MED	*/

			case 14: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  B, TM and MED	*/

			case 15: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwTM[px] * p_m->xHat_T + p_m->wwMED[px] * p_m->xHat_M;	
				break; /*  A, B, TM and MED	*/

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

			case 24: 
				p_m->xHat_d = p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  MED, R	*/

			case 19: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, B, R	*/

			case 21: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, TM, R	*/

			case 25: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, MED, R	*/

			case 22: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  B, TM, R	*/

			case 26: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  B, MED, R	*/

			case 28: 
				p_m->xHat_d = p_m->wwTM[px] * p_m->xHat_T + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  TM, MED, R	*/



			case 23: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + 
											p_m->wwb[px] * p_m->xHat_b_d + p_m->wwTM[px] * p_m->xHat_T + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*   A, B, TM, R	*/

			case 27: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + 
											p_m->wwb[px] * p_m->xHat_b_d + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*   A, B, MED, R	*/

			case 29: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + 
											p_m->wwTM[px] * p_m->xHat_T + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*   A, TM, MED, R	*/

			case 30: 
				p_m->xHat_d = p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwTM[px] * p_m->xHat_T + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*   B, TM, MED, R	*/

			case 31: 
				p_m->xHat_d = p_m->wwa[px] * p_m->xHat_a_d + p_m->wwb[px] * p_m->xHat_b_d + 
											p_m->wwTM[px] * p_m->xHat_T + p_m->wwMED[px] * p_m->xHat_M + 
											p_m->wwR[px] * p_m->xHat_R_d;	
				break; /*  A, B, TM, MED, R	*/
		}

		p_m->xHat = (long)floor( p_m->xHat_d + 0.5);
		p_m->xHat = CLIP( p_m->xHat, (signed)im->min_value, (signed)im->max_value);

//if (( p_m->xHat_d > 5 + p_m->xHat_b_d))
//{
//	r = r;
//}
//if (( p_m->xHat_d < -5 + p_m->xHat_b_d))
//{
//	r = r;
//}
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
	p_m->pred_err_M = Rx - p_m->xHat_M;
	p_m->pred_err    = Rx - p_m->xHat; /* not necessary at decoder	*/
	/* prediction error:	-RANGEd2 ...0...RANGEm1d2
	 *		-(max_value+1)/2 ...0...(max_value)/2
	 */
	/* not necessary at decoder	*/
	p_m->p_err = MODULO_RANGE( p_m->pred_err, -RANGEd2, RANGEm1d2, RANGE);
}

/*---------------------------------------------------------------
 *	update_meta_prediction()
 *---------------------------------------------------------------*/
void update_meta_prediction( unsigned short px, unsigned short num_contexts, 
								META_PRED *p_m, int pred_mode,
								unsigned long *count_px)
{
	double sum; /* for weighting macros	*/
	double damp = 0.99;
	double perform_a=0, perform_b=0, perform_R=0, perform_T=0, perform_M=0;

	p_m->count_scaled[px]++;
	count_px[px]++;	/* increment counter of actual prediction context	*/

	if (px < num_contexts) /* not for first row/col constant lines */
	{
		if (pred_mode & 0x01) 
		{
			p_m->cov[px][0][0] +=  p_m->pred_err_a * p_m->pred_err_a; /* determine variance	*/
		}
		if (pred_mode & 0x02)
		{
			p_m->cov[px][1][1] +=  p_m->pred_err_b * p_m->pred_err_b;
		}
		if (pred_mode & 0x04) 
		{
			p_m->cov[px][2][2] +=  p_m->pred_err_T * p_m->pred_err_T;
		}
		if (pred_mode & 0x08)
		{
			p_m->cov[px][3][3] +=  p_m->pred_err_M * p_m->pred_err_M;
		}
		if (pred_mode & 0x10)
		{
			p_m->cov[px][4][4] +=  p_m->pred_err_R * p_m->pred_err_R;
		}


		/* co-variances and pearsson correlation	*/
		if (pred_mode == 0x03) /* A, B-------------------------------------------	*/
		{
			p_m->cov[px][0][1] +=  p_m->pred_err_a * p_m->pred_err_b * damp;

			perform_a = p_m->cov[px][1][1] - p_m->cov[px][0][1];
			perform_b = p_m->cov[px][0][0] - p_m->cov[px][0][1];


#ifdef _DEBUG
			p_m->rho[0][1] = p_m->cov[px][0][1] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][1][1]);
#endif
		}
		if (pred_mode == 0x05)  /* A, TM	----------------------------------------*/
		{
			damp = 0.8;
			p_m->cov[px][0][2] +=  p_m->pred_err_a * p_m->pred_err_T * damp;

			perform_a = p_m->cov[px][2][2] - p_m->cov[px][0][2];
			perform_T = p_m->cov[px][0][0] - p_m->cov[px][0][2];
#ifdef _DEBUG
			p_m->rho[0][2] = p_m->cov[px][0][2] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][2][2]);
#endif
		}
		if (pred_mode == 0x09)  /* A, MED	*/
		{
			p_m->cov[px][0][3] +=  p_m->pred_err_a * p_m->pred_err_M * damp;

			perform_a = p_m->cov[px][3][3] - p_m->cov[px][0][3];
			perform_M = p_m->cov[px][0][0] - p_m->cov[px][0][3];
#ifdef _DEBUG
			p_m->rho[0][3] = p_m->cov[px][0][3] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][3][3]);
#endif
		}
		if (pred_mode == 0x11)  /* A, R	*/
		{
			p_m->cov[px][0][4] +=  p_m->pred_err_a * p_m->pred_err_R * damp;

			perform_a = p_m->cov[px][4][4] - p_m->cov[px][0][4];
			perform_R = p_m->cov[px][0][0] - p_m->cov[px][0][4];
#ifdef _DEBUG
			p_m->rho[0][4] = p_m->cov[px][0][4] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][3][4]);
#endif
		}

		if (pred_mode == 0x06)  /* B, TM	*/
		{
			damp = 0.8;
			p_m->cov[px][1][2] +=  p_m->pred_err_b * p_m->pred_err_T * damp;

			perform_b = p_m->cov[px][2][2] - p_m->cov[px][1][2];
			perform_T = p_m->cov[px][1][1] - p_m->cov[px][1][2];
#ifdef _DEBUG
			p_m->rho[1][2] = p_m->cov[px][1][2] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][2][2]);
#endif
		}
		if (pred_mode == 0x0A)   /* B, MED	*/
		{
			p_m->cov[px][1][3] +=  p_m->pred_err_b * p_m->pred_err_M * damp;

			perform_b = p_m->cov[px][3][3] - p_m->cov[px][1][3];
			perform_M = p_m->cov[px][1][1] - p_m->cov[px][1][3];
#ifdef _DEBUG
			p_m->rho[1][3] = p_m->cov[px][1][3] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][3][3]);
#endif
		}
		if (pred_mode == 0x12)  /* B, R	*/
		{
			p_m->cov[px][1][4] +=  p_m->pred_err_b * p_m->pred_err_R * damp;

			perform_b = p_m->cov[px][4][4] - p_m->cov[px][1][4];
			perform_R = p_m->cov[px][1][1] - p_m->cov[px][1][4];
#ifdef _DEBUG
			p_m->rho[1][4] = p_m->cov[px][1][4] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][4][4]);
#endif
		}

		if (pred_mode == 0x0C)   /* TM, MED	*/
		{
			p_m->cov[px][2][3] +=  p_m->pred_err_T * p_m->pred_err_M * damp;

			perform_T = p_m->cov[px][3][3] - p_m->cov[px][2][3];
			perform_M = p_m->cov[px][2][2] - p_m->cov[px][2][3];
#ifdef _DEBUG
			p_m->rho[2][3] = p_m->cov[px][2][3] / 
						sqrt(p_m->cov[px][2][2] * p_m->cov[px][3][3]);
#endif
		}
		if (pred_mode == 0x14)   /* TM, R	*/ 
		{
			p_m->cov[px][2][4] +=  p_m->pred_err_T * p_m->pred_err_R * damp;

			perform_T = p_m->cov[px][4][4] - p_m->cov[px][2][4];
			perform_R = p_m->cov[px][2][2] - p_m->cov[px][2][4];
#ifdef _DEBUG
			p_m->rho[2][4] = p_m->cov[px][2][4] / 
						sqrt(p_m->cov[px][2][2] * p_m->cov[px][4][4]);
#endif
		}

		if (pred_mode == 0x18)    /* MED, R	*/
		{
			p_m->cov[px][3][4] +=  p_m->pred_err_M * p_m->pred_err_R * damp;

			perform_M = p_m->cov[px][4][4] - p_m->cov[px][3][4];
			perform_R = p_m->cov[px][3][3] - p_m->cov[px][3][4];
#ifdef _DEBUG
			p_m->rho[3][4] = p_m->cov[px][3][4] / 
						sqrt(p_m->cov[px][3][3] * p_m->cov[px][4][4]);
#endif
		}

		if (pred_mode == 0x07) /* A, B, TM	-----------------------------------------------*/
		{
			double inv_cov[3][3];

			p_m->cov[px][0][1] +=  p_m->pred_err_a * p_m->pred_err_b;
			p_m->cov[px][0][2] +=  p_m->pred_err_a * p_m->pred_err_T;
			p_m->cov[px][1][2] +=  p_m->pred_err_b * p_m->pred_err_T;

//#ifdef _DEBUG
			p_m->rho[0][1] = p_m->cov[px][0][1] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][1][1]);
			p_m->rho[0][2] = p_m->cov[px][0][2] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][2][2]);
			p_m->rho[1][2] = p_m->cov[px][1][2] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][2][2]);
//#endif

			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{

				if (p_m->rho[0][1] > 0.9)
				{
					if (p_m->cov[px][0][0] < p_m->cov[px][1][1])
					{
						perform_a = p_m->cov[px][2][2] - p_m->cov[px][0][2];
						perform_T = p_m->cov[px][0][0] - p_m->cov[px][0][2];
						perform_b = 0;
					}
					else
					{
						perform_b = p_m->cov[px][2][2] - p_m->cov[px][1][2];
						perform_T = p_m->cov[px][1][1] - p_m->cov[px][1][2];
						perform_a = 0;
					}
				}
				else
				{
					damp = 0.8;
					p_m->cov[px][0][1] *= damp;
					p_m->cov[px][0][2] *= damp;
					p_m->cov[px][1][2] *= damp;
					/* co-variance matrix	w/o normalisation */
					inv_cov[0][0] = (p_m->cov[px][1][1] * p_m->cov[px][2][2] -
													 p_m->cov[px][1][2] * p_m->cov[px][1][2]);

					inv_cov[1][1] = (p_m->cov[px][0][0] * p_m->cov[px][2][2] -
													 p_m->cov[px][0][2] * p_m->cov[px][0][2]);

					inv_cov[2][2] = (p_m->cov[px][0][0] * p_m->cov[px][1][1] -
													 p_m->cov[px][0][1] * p_m->cov[px][0][1]);

					inv_cov[1][0] = inv_cov[0][1]=
													(p_m->cov[px][0][2] * p_m->cov[px][1][2] -
													 p_m->cov[px][0][1] * p_m->cov[px][2][2]);

					inv_cov[2][0] = inv_cov[0][2] = 
													(p_m->cov[px][0][1] * p_m->cov[px][1][2] -
													 p_m->cov[px][1][1] * p_m->cov[px][0][2]);


					inv_cov[1][2] = inv_cov[2][1] = 
													(p_m->cov[px][0][1] * p_m->cov[px][0][2] -
													 p_m->cov[px][0][0] * p_m->cov[px][1][2]);


					perform_a = (inv_cov[0][0] + inv_cov[1][0] + inv_cov[2][0]);
					perform_b = (inv_cov[0][1] + inv_cov[1][1] + inv_cov[2][1]);
					perform_T = (inv_cov[0][2] + inv_cov[1][2] + inv_cov[2][2]);
				}
			}
			else
			{
				perform_a = perform_b = perform_T = 0.33333333333;
			}
		}

		if (pred_mode == 0x0B) /* A, B, MED ------------------------------------	*/
		{
			double inv_cov[3][3];
			p_m->cov[px][0][1] +=  p_m->pred_err_a * p_m->pred_err_b * damp;
			p_m->cov[px][0][3] +=  p_m->pred_err_a * p_m->pred_err_M * damp;
			p_m->cov[px][1][3] +=  p_m->pred_err_b * p_m->pred_err_M * damp;


#ifdef _DEBUG
			p_m->rho[0][1] = p_m->cov[px][0][1] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][1][1]);
			p_m->rho[0][3] = p_m->cov[px][0][3] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][3][3]);
			p_m->rho[1][3] = p_m->cov[px][1][3] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][3][3]);
#endif
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				/* co-variance matrix	w/o normalisation */
				inv_cov[0][0] = (p_m->cov[px][1][1] * p_m->cov[px][3][3] -
												 p_m->cov[px][1][3] * p_m->cov[px][1][3]);

				inv_cov[1][1] = (p_m->cov[px][0][0] * p_m->cov[px][3][3] -
												 p_m->cov[px][0][3] * p_m->cov[px][0][3]);

				inv_cov[2][2] = (p_m->cov[px][0][0] * p_m->cov[px][1][1] -
												 p_m->cov[px][0][1] * p_m->cov[px][0][1]);

				inv_cov[1][0] = inv_cov[0][1]=
												(p_m->cov[px][0][3] * p_m->cov[px][1][3] -
												 p_m->cov[px][0][1] * p_m->cov[px][3][3]);

				inv_cov[2][0] = inv_cov[0][2] = 
												(p_m->cov[px][0][1] * p_m->cov[px][1][3] -
												 p_m->cov[px][1][1] * p_m->cov[px][0][3]);

				inv_cov[1][2] = inv_cov[2][1] = 
												(p_m->cov[px][0][1] * p_m->cov[px][0][3] -
												 p_m->cov[px][0][0] * p_m->cov[px][1][3]);

				perform_a = (inv_cov[0][0] + inv_cov[1][0] + inv_cov[2][0]);
				perform_b = (inv_cov[0][1] + inv_cov[1][1] + inv_cov[2][1]);
				perform_M = (inv_cov[0][2] + inv_cov[1][2] + inv_cov[2][2]);
			}
			else
			{
				perform_a = perform_b = perform_M = 0.33333333333;
			}
		}

		if (pred_mode == 0x13) /* A, B, R ---------------------------------------	*/
		{
			double inv_cov[3][3];
			p_m->cov[px][0][1] +=  p_m->pred_err_a * p_m->pred_err_b * damp;
			p_m->cov[px][0][4] +=  p_m->pred_err_a * p_m->pred_err_R * damp;
			p_m->cov[px][1][4] +=  p_m->pred_err_b * p_m->pred_err_R * damp;


#ifdef _DEBUG
			p_m->rho[0][1] = p_m->cov[px][0][1] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][1][1]);
			p_m->rho[0][2] = p_m->cov[px][0][4] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][4][4]);
			p_m->rho[1][2] = p_m->cov[px][1][4] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][4][4]);
#endif
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				/* co-variance matrix	w/o normalisation */
				inv_cov[0][0] = (p_m->cov[px][1][1] * p_m->cov[px][4][4] -
												 p_m->cov[px][1][4] * p_m->cov[px][1][4]);

				inv_cov[1][1] = (p_m->cov[px][0][0] * p_m->cov[px][4][4] -
												 p_m->cov[px][0][4] * p_m->cov[px][0][4]);

				inv_cov[2][2] = (p_m->cov[px][0][0] * p_m->cov[px][1][1] -
												 p_m->cov[px][0][1] * p_m->cov[px][0][1]);

				inv_cov[1][0] = inv_cov[0][1]=
												(p_m->cov[px][0][4] * p_m->cov[px][1][4] -
												 p_m->cov[px][0][1] * p_m->cov[px][4][4]);

				inv_cov[2][0] = inv_cov[0][2] = 
												(p_m->cov[px][0][1] * p_m->cov[px][1][4] -
												 p_m->cov[px][1][1] * p_m->cov[px][0][4]);

				inv_cov[1][2] = inv_cov[2][1] = 
												(p_m->cov[px][0][1] * p_m->cov[px][0][4] -
												 p_m->cov[px][0][0] * p_m->cov[px][1][4]);

				perform_a = (inv_cov[0][0] + inv_cov[1][0] + inv_cov[2][0]);
				perform_b = (inv_cov[0][1] + inv_cov[1][1] + inv_cov[2][1]);
				perform_R = (inv_cov[0][2] + inv_cov[1][2] + inv_cov[2][2]);
			}
			else
			{
				perform_a = perform_b = perform_R = 0.33333333333;
			}
		}


		if (pred_mode == 0x17) /* A, B, TM, R	*/
		{
			double inv_cov[4][4];

			damp = 0.7;
			p_m->cov[px][0][1] +=  p_m->pred_err_a * p_m->pred_err_b * damp;
			p_m->cov[px][0][2] +=  p_m->pred_err_a * p_m->pred_err_T * damp;
			p_m->cov[px][0][4] +=  p_m->pred_err_a * p_m->pred_err_R * damp;
			p_m->cov[px][1][2] +=  p_m->pred_err_b * p_m->pred_err_T * damp;
			p_m->cov[px][1][4] +=  p_m->pred_err_b * p_m->pred_err_R * damp;
			p_m->cov[px][2][4] +=  p_m->pred_err_T * p_m->pred_err_R * damp;
#ifdef _DEBUG
			p_m->rho[0][1] = p_m->cov[px][0][1] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][1][1]);
			p_m->rho[0][2] = p_m->cov[px][0][2] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][2][2]);
			p_m->rho[0][4] = p_m->cov[px][0][4] / 
						sqrt(p_m->cov[px][0][0] * p_m->cov[px][4][4]);
			p_m->rho[1][2] = p_m->cov[px][1][2] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][2][2]);
			p_m->rho[1][4] = p_m->cov[px][1][4] / 
						sqrt(p_m->cov[px][1][1] * p_m->cov[px][4][4]);
			p_m->rho[2][4] = p_m->cov[px][2][4] / 
						sqrt(p_m->cov[px][2][2] * p_m->cov[px][4][4]);
#endif
			if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
			{
				/* co-variance matrix	w/o normalisation */
				inv_cov[0][0] = p_m->cov[px][1][1] * (p_m->cov[px][2][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][2][4] * p_m->cov[px][2][4])
											-	p_m->cov[px][1][2] * (p_m->cov[px][1][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][2][4])
											+	p_m->cov[px][1][4] * (p_m->cov[px][1][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][2][2]);

				inv_cov[1][1] = p_m->cov[px][0][0] * (p_m->cov[px][2][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][2][4] * p_m->cov[px][2][4])
											-	p_m->cov[px][0][2] * (p_m->cov[px][0][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][4])
											+	p_m->cov[px][0][4] * (p_m->cov[px][0][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][2]);

				inv_cov[2][2] = p_m->cov[px][0][0] * (p_m->cov[px][1][1] * p_m->cov[px][4][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][1][4])
											-	p_m->cov[px][0][1] * (p_m->cov[px][0][1] * p_m->cov[px][4][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][4])
											+	p_m->cov[px][0][4] * (p_m->cov[px][0][1] * p_m->cov[px][1][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][1]);

				inv_cov[3][3] =+p_m->cov[px][0][0] * (p_m->cov[px][1][1] * p_m->cov[px][2][2] -
																							p_m->cov[px][1][2] * p_m->cov[px][1][2])
											-	p_m->cov[px][0][1] * (p_m->cov[px][0][1] * p_m->cov[px][2][2] -
																							p_m->cov[px][0][2] * p_m->cov[px][1][2])
											+	p_m->cov[px][0][2] * (p_m->cov[px][0][1] * p_m->cov[px][1][2] -
																							p_m->cov[px][0][2] * p_m->cov[px][1][1]);

				inv_cov[0][1] = inv_cov[1][0] =
											- p_m->cov[px][0][1] * (p_m->cov[px][2][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][2][4] * p_m->cov[px][2][4])
											+	p_m->cov[px][1][2] * (p_m->cov[px][0][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][4])
											-	p_m->cov[px][1][4] * (p_m->cov[px][0][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][2]);

				inv_cov[0][2] = inv_cov[2][0] =
											+ p_m->cov[px][0][1] * (p_m->cov[px][1][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][2][4])
											-	p_m->cov[px][1][1] * (p_m->cov[px][0][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][4])
											+	p_m->cov[px][1][4] * (p_m->cov[px][0][2] * p_m->cov[px][1][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][2]);

				inv_cov[0][3] = inv_cov[3][0] =
											- p_m->cov[px][0][1] * (p_m->cov[px][1][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][2][2])
											+	p_m->cov[px][1][1] * (p_m->cov[px][0][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][2])
											-	p_m->cov[px][1][2] * (p_m->cov[px][0][2] * p_m->cov[px][1][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][2]);

				inv_cov[1][2] = inv_cov[2][1] =
											- p_m->cov[px][0][0] * (p_m->cov[px][1][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][2][4])
											+	p_m->cov[px][0][1] * (p_m->cov[px][0][2] * p_m->cov[px][4][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][4])
											-	p_m->cov[px][0][4] * (p_m->cov[px][0][2] * p_m->cov[px][1][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][2]);

				inv_cov[1][3] = inv_cov[3][1] =
											+ p_m->cov[px][0][0] * (p_m->cov[px][1][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][2][2])
											-	p_m->cov[px][0][1] * (p_m->cov[px][0][2] * p_m->cov[px][2][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][2][2])
											+	p_m->cov[px][0][2] * (p_m->cov[px][0][2] * p_m->cov[px][1][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][2]);

				inv_cov[2][3] = inv_cov[3][2] =
											- p_m->cov[px][0][0] * (p_m->cov[px][1][1] * p_m->cov[px][2][4] -
																							p_m->cov[px][1][4] * p_m->cov[px][1][2])
											+	p_m->cov[px][0][1] * (p_m->cov[px][0][1] * p_m->cov[px][2][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][2])
											-	p_m->cov[px][0][2] * (p_m->cov[px][0][1] * p_m->cov[px][1][4] -
																							p_m->cov[px][0][4] * p_m->cov[px][1][1]);

				perform_a = (inv_cov[0][0] + inv_cov[1][0] + inv_cov[2][0] + inv_cov[3][0]);
				perform_b = (inv_cov[0][1] + inv_cov[1][1] + inv_cov[2][1] + inv_cov[3][1]);
				perform_T = (inv_cov[0][2] + inv_cov[1][2] + inv_cov[2][2] + inv_cov[3][2]);
				perform_R = (inv_cov[0][3] + inv_cov[1][3] + inv_cov[2][3] + inv_cov[3][3]);
			}
			else
			{
				perform_a = perform_b = perform_T = perform_R = 0.25;
			}
		}


		/* scaling of abserr	*/
		// if ( (count_px[px] % 256) == 0)
		if ( p_m->count_scaled[px] == 128)
		{
			int i, j;

			for ( i = 0; i < NUM_PREDICTORS; i++)
			{
				for ( j = 0; j < NUM_PREDICTORS; j++)
				{
					p_m->cov[px][i][j] *= 0.5;
				}
			}
			p_m->count_scaled[px] >>= 1;
		}

		/* do predictor blending	*/
		{
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

				case 9:
					WEIGTHING2( perform_a, perform_M, p_m->wwa[px], p_m->wwMED[px])
					break;

				case 10:
					WEIGTHING2( perform_b, perform_M, p_m->wwb[px], p_m->wwMED[px])
					break;

				case 12:
					WEIGTHING2( perform_T, perform_M, p_m->wwTM[px], p_m->wwMED[px])
					break;

				case 7:
					WEIGTHING3( perform_a, perform_b, perform_T, 
						p_m->wwa[px], p_m->wwb[px], p_m->wwTM[px])
					break;

				case 11:
					WEIGTHING3( perform_a, perform_b, perform_M, 
						p_m->wwa[px], p_m->wwb[px], p_m->wwMED[px])
					break;

				case 13:
					WEIGTHING3( perform_a, perform_T, perform_M, 
						p_m->wwa[px], p_m->wwTM[px], p_m->wwMED[px])
					break;

				case 14:
					WEIGTHING3( perform_b, perform_T, perform_M, 
						p_m->wwb[px], p_m->wwTM[px], p_m->wwMED[px])
					break;

				case 15:
					WEIGTHING4( perform_a, perform_b, perform_T, perform_M, 
						p_m->wwa[px], p_m->wwb[px], p_m->wwTM[px], p_m->wwMED[px])
					break;

				case 31: /* A, B, TM, MED, R	*/
					WEIGTHING5( perform_a, perform_b, perform_T, perform_M, perform_R, 
						p_m->wwa[px], p_m->wwb[px], p_m->wwTM[px], p_m->wwMED[px], p_m->wwR[px])
					break;

				case 30: /* B, TM, MED, R	*/
					WEIGTHING4( perform_b, perform_T, perform_M, perform_R, 
						p_m->wwb[px], p_m->wwTM[px], p_m->wwMED[px], p_m->wwR[px])
					break;

				case 29: /* A, TM, MED, R	*/
					WEIGTHING4( perform_a, perform_T, perform_M, perform_R, 
						p_m->wwa[px], p_m->wwTM[px], p_m->wwMED[px], p_m->wwR[px])
					break;

				case 27: /* A, B, MED, R	*/
					WEIGTHING4( perform_a, perform_b, perform_M, perform_R, 
						p_m->wwa[px], p_m->wwb[px], p_m->wwMED[px], p_m->wwR[px])
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

				case 25: /* A, MED, R	*/
					WEIGTHING3(perform_a, perform_M, perform_R, 
						p_m->wwa[px], p_m->wwMED[px], p_m->wwR[px])
					break;

				case 22: /* B, TM, R	*/
					WEIGTHING3(perform_b, perform_T, perform_R, 
						p_m->wwb[px], p_m->wwTM[px], p_m->wwR[px])
					break;

				case 26: /* B, MED, R	*/
					WEIGTHING3(perform_b, perform_M, perform_R, 
						p_m->wwb[px], p_m->wwMED[px], p_m->wwR[px])
					break;

				case 28: /* TM, MED, R	*/
					WEIGTHING3(perform_T, perform_M, perform_R, 
						p_m->wwTM[px], p_m->wwMED[px], p_m->wwR[px])
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

				case 24: /* MED, R	*/
					WEIGTHING2(perform_M,perform_R, 
										 p_m->wwMED[px], p_m->wwR[px])
					break;


				default : sum = 0;
			}
			//p_m->wwa[px] = ( 7 * wa2 + p_m->wwa[px])*  0.125;
			//p_m->wwb[px] = ( 7 * wb2 + p_m->wwb[px])*  0.125;
			//p_m->wwR[px] = ( 7* wR2 + p_m->wwR[px])*  0.125;
			//p_m->wwTM[px] = ( 7 * wTM2 + p_m->wwTM[px])*  0.125;
			//p_m->wwMED[px] = ( 7 * wMED2 + p_m->wwMED[px])*  0.125;

		}
		/* accumulate prediction errors for each context	*/
		if (count_px[px] >= NUM_SKIPPED_STEPS) /* skip transient response	*/
		{
			/* for bias compensation	*/
			if (pred_mode & 0x01) p_m->sum_err_a[px] += p_m->pred_err_a;	
			if (pred_mode & 0x02) p_m->sum_err_b[px] += p_m->pred_err_b;
			if (pred_mode & 0x04) p_m->sum_err_T[px] += p_m->pred_err_T;
			if (pred_mode & 0x08) p_m->sum_err_M[px] += p_m->pred_err_M;
			if (pred_mode & 0x10) p_m->sum_err_R[px] += p_m->pred_err_R;

			/* only for statistics	*/
			if (pred_mode & 0x01) p_m->sum_abserr_a[px] += fabs( p_m->pred_err_a); 
			if (pred_mode & 0x02) p_m->sum_abserr_b[px] += fabs( p_m->pred_err_b); 
			if (pred_mode & 0x10) p_m->sum_abserr_R[px] += fabs( p_m->pred_err_R); 
			/* essential	*/
			p_m->sum_abserr[px] += abs( p_m->pred_err); 
		}
		/* total (sum) prediction error, used to compute the average	*/
		p_m->mean_tot += p_m->pred_err; 
	} /* if (px < num_contexts)*/

}
