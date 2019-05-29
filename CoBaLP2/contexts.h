/************************************************
 *
 * File...:	contexts.h
 * Descr..:	Definitions for context handling
 * Author.:	Tilo Strutz
 * Date...: 13.05.2013
 * changes:
 * 28.05.2013 Ra_shift, scaling of Ra in kn[][]
 * 19.06.2013 new PREDE and TEMPLATE_E
 * 03.07.2013 exhaustive_flag
 ************************************************/
#ifndef CONTEXTS_H
#define CONTEXTS_H

#include "predictor2.h"
#include "types_CoBaLP.h"

//#define NUM_PX_CONTEXTS 48
#define NUM_PX_CONTEXTS MAXNUM_OF_PRED_CONTEXTS
#define TEMPLATE_E_SIZE 19 /* A...R + average in pred. context	*/
//#define TEMPLATE_E_SIZE 11 /* A...J + average in pred. context	*/

#define NUM_ESTIMATORS	3

/* for prediction of absolute error (for determination of coding context)	*/
typedef struct {
	double 	estimate[NUM_ESTIMATORS];	/* 	*/
	double 	weights_e[NUM_PX_CONTEXTS][NUM_ESTIMATORS];	/* 	*/
	double	varianceN[NUM_PX_CONTEXTS][NUM_ESTIMATORS];
	long cnt[NUM_PX_CONTEXTS];	/* count the contexts, for scaling of varianceN	*/
	unsigned char weight_flag[NUM_PX_CONTEXTS];
	int weight_flag_cnt;
	unsigned int countdown;
} PREDE;


/*
    NOP
	UMGHIQ
 TLFCBDJR
 SKEAX
*/
typedef struct {
//			 0		1		2		3		4		5		6		7		8		9	10	11	12	13	14	15	16	17  18	19	 20
//	long Ra, Rb, Rc, Rd, Re, Rf, Rg, Rh, Ri, Rj, Rk, Rl, Rm, Rn, Ro, Rp, Rq, Rr, Rs, Rt, Ru;
	long e[MAX_TEMPLATE_SIZE];
	//double estimate[MAX_TEMPLATE_SIZE];
	//double mean19;
} TEMPLATE_E;

unsigned short get_context( int r, int c, 
												 unsigned short num_contexts,
												 TEMPLATE *templ);
unsigned short get_contextLS( unsigned short num_contexts,
												 TEMPLATE *templ, double *min_dist);
long get_meanOfPattern( TEMPLATE *templ);

void  contextsExt( long *lptr, IMAGE *imExt, unsigned short *num_contexts,
						unsigned char *const_rows, 
						unsigned char *const_cols,
						int exhaustive_flag);
void contextsExtLS( long *lptr, /* extended image array	*/
								 IMAGE *imExt, /* parameters of extended image	*/
								 unsigned short *num_px, /* number of prediction contexts	*/
						unsigned char *const_rows, 
						unsigned char *const_cols,
						int exhaustive_flag);

void init_weights_e( PREDE *p_predE);

//unsigned long get_averaged_error( IMAGE *im, long *eptr, 
//														 unsigned long pos, int r, int c,
//														 unsigned short *px_ptr,
//														 unsigned long sum_abserr, 
//														 unsigned long count_px		/* in current context			*/
//															);
unsigned long get_averaged_error( IMAGE *im, long *eptr, 
							unsigned long pos, int r, int c,
							unsigned short *px_ptr,
							unsigned long sum_abserr, /* averaged absolut error	*/
							unsigned long count_px,		/* in current context			*/
							PREDE *p_predE, TEMPLATE_E *templ,
							unsigned long num_abs_scaled_errors);

void update_weights_e( PREDE *p_predE, unsigned short px, 
												 long abs_err,
												 /* TEMPLATE_E *templ,*/
												 unsigned int reset);

//unsigned short get_AC_context( IMAGE *im, long *eptr, 
//														 unsigned long pos, int r, int c,
//														 unsigned short *px_ptr,
//														 unsigned int th_AC[],
//														 unsigned short used_contexts,
//														 unsigned long sum_abserr, 
//														 unsigned long count_px		/* in current context			*/
//													);
unsigned short get_AC_context( IMAGE *im, long *eptr, 
														 unsigned long pos, int r, int c,
														 unsigned short *px_ptr,
														 unsigned int th_AC[],
														 unsigned short used_contexts,
														 unsigned long sum_abserr, 
							unsigned long count_px,		/* in current context			*/
							PREDE *p_predE, TEMPLATE_E *templ,
							unsigned long num_abs_scaled_errors,
							unsigned long *abs_err);



#endif
