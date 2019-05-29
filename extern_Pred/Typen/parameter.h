/*******************************************************************
 *
 * File....: parameter.h
 * Function: Function prototyping and definition of the PARAMETER
 *           structure.
 * Author..: Thomas Schatton
 * Date....: 01/13/2011
 *
 ********************************************************************/

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "types.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* parameter structure */
typedef struct {
	/* relevant for block mode */
	BOOL blockMode;	/* block mode: true, context mode: false */
  UINT blockWidth, blockHeight;  /* block dimensions */
  UINT selCriterion;  /* selection criterion */
	/* relevant for context mode */
	BOOL wMeanMode; /* use weighted means of all predictors */
	UINT cxPx,			/* amount of pixels [0..10] and gradients .. */
       cxGrad;    /* .. [0..9] used for context modelling      */
	/* predictor parameters */
	UINT anzPredictors;	/* amount of available predictors */
	BOOL *predictors;		/* list of active predictors */
	BOOL *bias;			    /* list of active bias concatenations */
  UINT biasReset;     /* frequency of bias adaption */
  /* template matching */
  float tmSigma;       /* sigma for template matching predictor */
} PARAMETER;

/* Constructor & Destructor */
int newParameter( PARAMETER **p, UINT anzPredictors);
void freeParameter(PARAMETER *p);

/* prints current parameters on stdout */
int printParameter( PARAMETER *p);

/* serialization */
int serializeParameter( UINT **stream, PARAMETER *p);
int newParameterFromStream( PARAMETER **p, UINT *in);

#endif