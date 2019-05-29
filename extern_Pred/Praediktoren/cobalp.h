/*******************************************************************
 *
 * File....: cobalp.h
 * Function: Definition of predictor variables and prototyping of
 *           CoBaLP functions.
 * Author..: Tilo Strutz (CoBaLP)
 *           Thomas Schatton (compatibility adjustments)
 * Date....: 11/04/2010
 *
 ********************************************************************/

#ifndef _COBALP_H_
#define _COBALP_H_

#include <math.h>
#include "..\Typen\types.h"
#include "..\Module\auswahl.h"

#define COBALP_NUM_OF_PRED       9
#define COBALP_NUM_OF_CONTEXTS 200

static float *cobalpLr,      /* learning rates (per pred) */
      **cobalpW;      /* weights (per context & pred) */
static int   **cobalpMeanD;

/* initialization of weights and learning rates */
int cobalpInitialize(AUSWAHL *a); 
/* free memory */
void cobalpFree();

/* weight adaption */
void cobalpAdaption(AUSWAHL *a, int pred_err);
/* context modelling */
int cobalpGetContext(AUSWAHL *a);
/* calculation of the prediction value */
int cobalpXHat(AUSWAHL *a);

#endif