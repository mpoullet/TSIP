/*****************************************************************
 *
 * File...:	predictor2.h
 * Purpose:	definitions and prototyping for predictor2.c
 * Date...: 27.03.2013
 *
 * 04.04.2013 new: pred_mode
 * 08.04.2013 new elements in structure PRED2
 * 13.08.2018 include variables for predictLS()
 *****************************************************************/
#ifndef PREDICTOR2_H
#define PREDICTOR2_H

#include "bitbuf.h"

//#include "codec.h"
#include "types_CoBaLP.h"

//#define QUARTIC_DIST
//
#define EUCLID_DIST
//#undef EUCLID_DIST

/* the number should be adapted to the noisiness of the image	*/
#define NUM_OF_PRED	20	/* best 14 für kodim, 15 better for noisy */
#define MAXNUM_OF_PRED_CONTEXTS	300
#define TEMPLATE_SIZE 18
#define MAX_TEMPLATE_SIZE 21 /* must not be smaller than NUM_OF_PRED or TEMPLATE_SIZE*/
#define LS_TEMPLATE_SIZE 6

typedef struct {
	float 	weights_a[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED];
	float 	weights_b[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED];
	float 	weights_R[MAXNUM_OF_PRED_CONTEXTS][TEMPLATE_SIZE];
	float 	weights_L[MAXNUM_OF_PRED_CONTEXTS][TEMPLATE_SIZE];
	long 	lweights_L[MAXNUM_OF_PRED_CONTEXTS][TEMPLATE_SIZE];
	long 	difference_i_a[NUM_OF_PRED];
	/* for mu adaptation */
	//float lr[NUM_OF_PRED];	/* learning rates	for LMS prediction*/
	float lr_a[NUM_OF_PRED];	/* learning rates	for LMS prediction*/
	float lr_b[NUM_OF_PRED];	/* learning rates	for LMS prediction*/
	float lr_R[TEMPLATE_SIZE];	/* learning rates	for LMS prediction*/
	//float 	meandiff_a[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* max. difference in a certain context	*/
	//float 	meandiff_b[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* min. difference in a certain context	*/
	//float 	meandiff_Xa[MAXNUM_OF_PRED_CONTEXTS]; /* mean difference in a certain context	*/
	//float 	meandiff_Xb[MAXNUM_OF_PRED_CONTEXTS]; /* at X position	*/
	long 	signEQUdiff_a[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* 	*/
	long 	signEQUdiff_b[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* 	*/
	long 	signNEQdiff_a[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* 	*/
	long 	signNEQdiff_b[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* 	*/
	/**/
	long 	difference_i_b[NUM_OF_PRED];
	/* mean of differences, normalises actual difference, 
	 * integer is better than floating point for some reason
	 */
	float sum_abserr_a[MAXNUM_OF_PRED_CONTEXTS];
	float sum_abserr_b[MAXNUM_OF_PRED_CONTEXTS];
	float sum_abserr_R[MAXNUM_OF_PRED_CONTEXTS];
	unsigned long count_px[MAXNUM_OF_PRED_CONTEXTS];
	int mean_difference_a[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED]; /* mean of absolute differences	*/
	int mean_difference_b[MAXNUM_OF_PRED_CONTEXTS][NUM_OF_PRED];
	int mean_value_R[MAXNUM_OF_PRED_CONTEXTS][TEMPLATE_SIZE];
} PRED2;


/*
    NOP
	UMGHIQ
 TLFCBDJR
 SKEAX
*/
typedef struct {
//			 0		1		2		3		4		5		6		7		8		9	10	11	12	13	14	15	16	17  18	19	 20
//	long Ra, Rb, Rc, Rd, Re, Rf, Rg, Rh, Ri, Rj, Rk, Rl, Rm, Rn, Ro, Rp, Rq, Rr, Rs, Rt, Ru;
	long R[MAX_TEMPLATE_SIZE];
	float meanR; /* used in predictorR	*/
} TEMPLATE;


#define GET_VALS()\
	di[0] = templ->R[0];\
	di[1] = templ->R[1]; /* b */\
	di[2] = templ->R[2]; /* c */\
	di[3] = templ->R[3]; /* d */\
	di[4] = templ->R[4]; /* e */\
	di[5] = templ->R[7]; /* h */

#define GET_DIFFS()\
	di[0] = templ->R[0] - templ->R[2]; /* a - c */\
	di[1] = templ->R[2] - templ->R[1]; /* c - b */\
	di[2] = templ->R[1] - templ->R[3]; /* b - d */\
	di[3] = templ->R[0] - templ->R[4]; /* a - e */\
	di[4] = templ->R[1] - templ->R[7]; /* b - h */\
  di[5] = templ->R[2] - templ->R[6]; /* c - g */\
  /* 666 * di[6] = templ->R[4] - templ->R[5]; /* e - f */\
  /* 666 * di[7] = templ->R[3] - templ->R[8]; /* d - i */\
  /* 666 * di[8] = templ->R[3] - templ->R[9]; /* d - j */

#define GET_DIFFS_2()\
	/*di[5] = templ->R[2] - templ->R[6]; /* c - g */\
	di[6] = templ->R[4] - templ->R[5]; /* e - f */\
	di[7] = templ->R[3] - templ->R[8]; /* d - i */\
	di[8] = templ->R[3] - templ->R[9]; /* d - j */\
\
	di[9] = templ->R[4] - templ->R[10]; /* e - k */\
	di[10] = templ->R[7] - templ->R[14]; /* h - o */\
	di[11] = templ->R[5] - templ->R[12]; /* f - m */\
\
	di[12] = templ->R[10] - templ->R[11]; /* k - l */\
	di[13] = templ->R[14] - templ->R[13]; /* o - n */\
\
	if (NUM_OF_PRED > 14)\
	{\
		di[14] = templ->R[9] - templ->R[16]; /* j - q */\
	}\
	if (NUM_OF_PRED > 15)\
	{\
		di[15] = templ->R[9] - templ->R[17]; /* j - r */\
	}\
	if (NUM_OF_PRED > 16)\
	{\
		di[16] = templ->R[2] - templ->R[5]; /* c - f */ /* redundant	*/\
	}\
	if (NUM_OF_PRED > 17) \
	{\
/*		di[17] = templ->R[7] - templ->R[8]; /* h - i */  /* redundant	*/\
		di[17] = templ->R[8] - templ->R[15]; /* i - p */  \
	}\
	if (NUM_OF_PRED > 18)			\
	{													\
		di[18] = templ->R[7] - templ->R[6];  /* h - g *//* redundant	*/\
	}\
	if (NUM_OF_PRED > 19)			\
	{													\
		di[19] = templ->R[7] - templ->R[8]; /* h - i */ /* redundant	*/\
	}
// templ->R[15] (p) is missing


#define GET_DIFFSa()\
	di[0] = templ->R[0] - templ->R[2]; /* a - c */\
	di[1] = templ->R[2] - templ->R[1]; /* c - b */\
	di[2] = templ->R[1] - templ->R[3]; /* b - d */\
	di[3] = templ->R[0] - templ->R[4]; /* a - e */\
	di[4] = templ->R[7] - templ->R[8]; /* h - i */\
  di[5] = templ->R[2] - templ->R[5]; /* c - f */

#define GET_DIFFS_2a()\
	di[6] = templ->R[4] - templ->R[10]; /* e - k */\
	di[7] = templ->R[5] - templ->R[11]; /* f - l */\
	di[8] = templ->R[2] - templ->R[5]; /* c - f */ \
	di[9] = templ->R[3] - templ->R[9]; /* d - j */\
	di[10] = templ->R[9] - templ->R[17]; /* j - r */\
	di[11] = templ->R[6] - templ->R[12]; /* g - m */\
	di[12] = templ->R[7] - templ->R[6]; /* h - g */ \
	/* di[10] = templ->R[7] - templ->R[8]; /* h - i already in GET_DIFFSa() */ \
	di[13] = templ->R[8] - templ->R[16]; /* i - q */\
\
	if (NUM_OF_PRED > 14)\
	{\
		di[14] = templ->R[14] - templ->R[13]; /* o - n */\
	}\
	if (NUM_OF_PRED > 15)\
	{\
		di[15] = templ->R[14] - templ->R[15]; /* o - p */\
	}\
	if (NUM_OF_PRED > 16)\
	{\
		di[16] = templ->R[9] - templ->R[16]; /* b - h */\
	}\
	if (NUM_OF_PRED > 17) \
	{\
		di[17] = templ->R[2] - templ->R[6]; /* c - g */\
	}\
	if (NUM_OF_PRED > 18)			\
	{													\
		di[8] = templ->R[3] - templ->R[8]; /* d - i */\
	}\
	if (NUM_OF_PRED > 19)			\
	{													\
		di[19] = templ->R[4] - templ->R[5]; /* e - f */\
	}
	//	di[10] = templ->R[7] - templ->R[14]; /* h - o */\
	//	di[17] = templ->R[8] - templ->R[15]; /* i - p */  \
	

#define GET_DIFFSb()\
	di[0] = templ->R[0] - templ->R[2]; /* a - c */\
	di[1] = templ->R[2] - templ->R[1]; /* c - b */\
	di[2] = templ->R[1] - templ->R[7]; /* b - h */\
	di[3] = templ->R[4] - templ->R[5]; /* e - f */\
	di[4] = templ->R[2] - templ->R[6]; /* c - g */\
  di[5] = templ->R[3] - templ->R[8]; /* d - i */

#define GET_DIFFS_2b()\
	di[6] = templ->R[10] - templ->R[11]; /* k - l */\
	di[7] = templ->R[5] - templ->R[12]; /* f - m */\
	di[8] = templ->R[9] - templ->R[16]; /* j - q */\
	di[9] = templ->R[6] - templ->R[13]; /* g - n */\
	di[10] = templ->R[7] - templ->R[14]; /* h - o */\
	di[11] = templ->R[8] - templ->R[15]; /* i - p */  \
  di[12] = templ->R[2] - templ->R[5]; /* c - f */ \
	di[13] = templ->R[3] - templ->R[9]; /* d - j */\
\
	if (NUM_OF_PRED > 14)\
	{\
		di[14] = templ->R[0] - templ->R[4]; /* a - e */\
	}\
	if (NUM_OF_PRED > 15)\
	{\
		di[15] = templ->R[2] - templ->R[5]; /* c - f */ \
	}\
	if (NUM_OF_PRED > 16)\
	{\
		di[16] = templ->R[1] - templ->R[3]; /* b - d */\
	}\
	if (NUM_OF_PRED > 17) \
	{\
		di[17] = templ->R[7] - templ->R[6];  /* h - g */\
	}\
	if (NUM_OF_PRED > 18)			\
	{													\
		di[18] = templ->R[7] - templ->R[8]; /* h - i */ \
	}\
	if (NUM_OF_PRED > 19)			\
	{													\
		di[19] = templ->R[4] - templ->R[10]; /* e - k */\
	}


/* prototyping	*/
float predictor2a( PRED2 *p_pred2, TEMPLATE *templ, 
									 unsigned int px, long min_value, long max);
float predictor2b( PRED2 *p_pred2, TEMPLATE *templ, 
									 unsigned int px, long min_value, long max);
float predictorR( PRED2 *p_pred2, TEMPLATE *templ, 
									 unsigned int px, long min_value, long max);
void init_pred2Weights( PRED2 *p_pred2, unsigned int max_value, float wA, float wB);
void init_pred2Weights_R( PRED2 *p_pred2, unsigned int max_value);
void init_pred2Weights_L( BitBuffer *bitbuf,
												 IMAGE *imExt, long *lptr, IMAGE *im, 
												 unsigned short num_contexts,
												 unsigned short *pxExt_ptr,
												 PRED2 *p_pred2);
void init_pred2Weights_L_d( BitBuffer *bitbuf,
												 unsigned short num_contexts,
												 PRED2 *p_pred2);
void init_pred2Weights_Mult( IMAGE *imExt, long *lptr, IMAGE *im, 
												 unsigned short num_contexts,
												 unsigned short *bestPred
												 );

float predictorLS( PRED2 *p_pred2, TEMPLATE *p_templ,
									 unsigned short *pxLS_ptr, long *eptrLS, 
									 int r,
									 int c, 
									 unsigned long pos,
									 unsigned int width,
									 long min_value, long max_value);
float predictorMult( TEMPLATE *templ, unsigned short *predMult,
									 unsigned short px, long *eptrLS, 
									 int r,
									 int c, 
									 unsigned long pos,
									 unsigned int width,
									 long min_value, long max_value);

void update_pred2Weights( PRED2 *p_pred2, unsigned int px, 
												 float pred_err_a, float pred_err_b,
												 int pred_mode);
void update_pred2Weights_R( PRED2 *p_pred2, unsigned int px, 
												 float pred_err_R,
												 int pred_mode, TEMPLATE *templ);
void set_template( long *lptr, TEMPLATE *templ, unsigned long posExtm,
									unsigned int r, unsigned int c, unsigned int widthExt);

/*------------------------------------------------------------------*/
#endif

