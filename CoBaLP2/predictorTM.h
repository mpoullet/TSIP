/*****************************************************************
 *
 * File...:	predictorTM.h
 * Purpose:	definitions and prototyping for predictor2.c
 * Date...: 09.04.2013
 *
 *****************************************************************/
#ifndef PREDICTORTM_H
#define PREDICTORTM_H

#include "codec_CoBaLP.h"
#include "types_CoBaLP.h"


#define NUM_OF_PARAMS	4
#define NUM_OF_OBSERVATIONS	8

typedef struct {
	double 	*a;
	double 	*obs;
	double 	*weights;
	double 	**J, **covar;
} LEAST_SQUARES;



/* prototyping	*/
void init_predictorTM( IMAGE *im, LEAST_SQUARES *p_ls);
void clean_predictorTM( LEAST_SQUARES *p_ls );
float predictorTM( IMAGE *imExt, long *lptr, 
									 unsigned long pos, /* is posExt	*/
									 int r, int c, LEAST_SQUARES *p_ls);
float predictorCM( IMAGE *imExt, long *lptr, unsigned short *pxExt_ptr,
									 unsigned long posExt, /* is posExt	*/
									 int r, int c);


/*------------------------------------------------------------------*/
#endif

