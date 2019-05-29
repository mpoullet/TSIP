 /*******************************************************************
 *
 * File....: predictor.h
 * Function: Function prototyping and definition of the PREDICTOR_S 
 *           (predictor handling) structure.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 * 28.03.2014 Strutz add halfRangem1 to structure PREDICTOR_S
 ********************************************************************/

#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "types.h"
#include "auswahl.h"
#include "predictors.h"
#include "bias.h"

/* PREDICTOR_S structure */
typedef struct {
	AUSWAHL *a;               /* pointer to selection structure */
	UINT    *assoc;           /* order of activated predictors */
	UINT    activePredictors; /* amount of activated predictors */
	BOOL    *biasActive;      /* bias compensation per predictor */
	BIAS    **bias;           /* array of bias structures */
	BOOL    reconstruction;   /* flag for image reconstruction */
	int     range, halfRange; /* (half) range of pixel values */
	int     halfRangem1;			/* Strutz (half) range of pixel values */
  BOOL    *pActive;         /* activation state of predictors */
} PREDICTOR_S;

/* Constructor & Destructor */
int newPredictor(PREDICTOR_S **p, AUSWAHL *a, PARAMETER *param,
                 BOOL reconst);
void freePredictor(PREDICTOR_S *p);

/* checks whether the predictor specified by a global index is active
 * and returns the predictor's index in *assoc */
int predictorIsActive( PREDICTOR_S *p, UINT pNr);

/* calculation of a prediction value */
UINT xHat(PREDICTOR_S *p, UINT pNr);      
/* calculation of the weighted mean of all prediction values */
UINT xHatW( PREDICTOR_S *p, float *w, UINT **x);
/* computes a prediction error and moves it into the pixel 
 * value range */
UINT predictionError( PREDICTOR_S *p, UINT xH);
/* calculates and writes the prediction error at the current pixel */
void writePredictionError(PREDICTOR_S *p, UINT pNr, BOOL rescale); 
/* calculates and writes the mean prediction error at the current
 * pixel */
int writePredictionErrorW( PREDICTOR_S *p, float *w, UINT **x, 
                           BOOL rescale);
void restorePredictionErrorB( PREDICTOR_S *p, UINT pNr);

#endif