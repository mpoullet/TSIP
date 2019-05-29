/*****************************************************************
 *
 * File...:	meta_predictor.h
 * Purpose:	definistions and prototyping for meta predictor
 * Date...: 09.04.2013
 *
 * 19.04.2013 addition of MED
 * 10.09.2013 unsigned short num_contexts, argument for finish_meta_prediction()
 * 24.06.2014 back to 26c blending, exclude MED
 * 09.08.2018 redefinition of meta_predict()
 *****************************************************************/
#ifndef META_PREDICTOR_H
#define META_PREDICTOR_H

#include "predictor2.h"
#include "predictorTM.h"

#define NUM_PREDICTORS	5


typedef struct {
	long	xHat;			/* final estimate	*/
	long 	p_err;	/* pred_err mmodulo RANGE	*/
	long 	pred_err;	/* true value minus final xHat	*/
	float	xHat_a_d;	/* estimate of predictor A	*/
	float	xHat_b_d;	/* estimate of predictor B	*/
	float	xHat_R_d;	/* estimate of predictor B	*/
	float	xHat_T;	/* estimate of predictor TM	*/
	float	xHat_L;	/* estimate of predictor LS	*/
	float	xHat_d;	/* combined estimate	*/
	float 	pred_err_a;	/* error from lin.predictor A	*/
	float 	pred_err_b;	/* error from lin. predictor B	*/
	float 	pred_err_R;	/* error from lin. predictor B	*/
	float 	pred_err_T;	/* error from template matching */
	float 	pred_err_L;	/* error from lest squares */
	/* for bias correction observe mean values of prediction errors	*/
	float sum_err_a[MAXNUM_OF_PRED_CONTEXTS];
	float sum_err_b[MAXNUM_OF_PRED_CONTEXTS];
	float sum_err_R[MAXNUM_OF_PRED_CONTEXTS];
	float sum_err_T[MAXNUM_OF_PRED_CONTEXTS];
	float sum_err_L[MAXNUM_OF_PRED_CONTEXTS];
	unsigned int count_scaled[MAXNUM_OF_PRED_CONTEXTS];

	// covariances of prediction errors
	//float cov[MAXNUM_OF_PRED_CONTEXTS][NUM_PREDICTORS][NUM_PREDICTORS];
	//float rho[MAXNUM_OF_PRED_CONTEXTS][NUM_PREDICTORS][NUM_PREDICTORS];
	//float rho_ave[MAXNUM_OF_PRED_CONTEXTS][NUM_PREDICTORS][NUM_PREDICTORS];
  float covAA[MAXNUM_OF_PRED_CONTEXTS];
  float covBB[MAXNUM_OF_PRED_CONTEXTS];
  float covRR[MAXNUM_OF_PRED_CONTEXTS];
  float covTT[MAXNUM_OF_PRED_CONTEXTS];
  float covLL[MAXNUM_OF_PRED_CONTEXTS];
  float covAB[MAXNUM_OF_PRED_CONTEXTS];
  float covAR[MAXNUM_OF_PRED_CONTEXTS];
  float covAT[MAXNUM_OF_PRED_CONTEXTS];
  float covAL[MAXNUM_OF_PRED_CONTEXTS];
  float covBR[MAXNUM_OF_PRED_CONTEXTS];
  float covBT[MAXNUM_OF_PRED_CONTEXTS];
  float covRT[MAXNUM_OF_PRED_CONTEXTS];
  float rhoAB[MAXNUM_OF_PRED_CONTEXTS];
  float rhoAR[MAXNUM_OF_PRED_CONTEXTS];
  float rhoAT[MAXNUM_OF_PRED_CONTEXTS];
  float rhoAL[MAXNUM_OF_PRED_CONTEXTS];
  float rhoBR[MAXNUM_OF_PRED_CONTEXTS];
  float rhoBT[MAXNUM_OF_PRED_CONTEXTS];
  float rhoRT[MAXNUM_OF_PRED_CONTEXTS];

	float sum_abserr_a[MAXNUM_OF_PRED_CONTEXTS];
	float sum_abserr_b[MAXNUM_OF_PRED_CONTEXTS];
	float sum_abserr_R[MAXNUM_OF_PRED_CONTEXTS];
	long sum_abserr[MAXNUM_OF_PRED_CONTEXTS];
	float mean_tot;
	/* for adaptive weighted combination of estimates	*/
	float wwa[MAXNUM_OF_PRED_CONTEXTS];
	float wwb[MAXNUM_OF_PRED_CONTEXTS];
	float wwR[MAXNUM_OF_PRED_CONTEXTS];
	float wwTM[MAXNUM_OF_PRED_CONTEXTS];
	float wwLS[MAXNUM_OF_PRED_CONTEXTS];

	double abserr_a[MAXNUM_OF_PRED_CONTEXTS];
	double abserr_b[MAXNUM_OF_PRED_CONTEXTS];
	double abserr_R[MAXNUM_OF_PRED_CONTEXTS];
	double abserr_T[MAXNUM_OF_PRED_CONTEXTS];
	double abserr_L[MAXNUM_OF_PRED_CONTEXTS];

	//unsigned long sgncorr[MAXNUM_OF_PRED_CONTEXTS][NUM_PREDICTORS][NUM_PREDICTORS];
	/**/
	long 	difference_i_b[NUM_OF_PRED];

	int zero_dist_flag;
	/* mean of differences, normalises actual difference, 
	 * integer is better than floating point for some reason
	 */
} META_PRED;


/* prototyping	*/
unsigned short 
meta_predict( IMAGE *im, /* format of real image	*/
							IMAGE *imExt, /* format of extended image	*/
							long *lptr,		/* pointer to extended image-data array	*/
							unsigned short *pxExt_ptr, /* pointer to extended px array	*/
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
								LEAST_SQUARES *p_ls);

void meta_error( META_PRED *p_m, long Rx, long RANGEd2, 
							 long RANGEm1d2, long RANGE);
int init_meta_prediction( META_PRED *p_meta, int pred_mode);
void finish_meta_prediction();
void update_meta_prediction( unsigned short px, unsigned short num_contexts, 
								META_PRED *p_meta, int pred_mode,
								unsigned long *count_px,
								unsigned long *count_pxLS, unsigned short pxLS);
void clean_meta_prediction( META_PRED *p_meta);

/*------------------------------------------------------------------*/
#endif

