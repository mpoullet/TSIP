/*******************************************************************
 *
 * File....: predictors.h
 * Function: Prototyping of predictor functions.
 * Author..: Thomas Schatton
 * Date....: 01/13/2010
 *
 ********************************************************************/

#ifndef PREDICTORS_H_
#define PREDICTORS_H_

#include <string.h>
#include "..\Typen\types.h"
#include "..\Typen\parameter.h"
#include "auswahl.h"

#include "..\Praediktoren\plinear.h"
#include "..\Praediktoren\cobalp.h"
#include "..\Praediktoren\tmatching.h"

#define PREDICTORCOUNT 6  /* number of implemented predictors */

/* returns the name of the predictor indexed by x */
char *getPredictorName(int x);

/* function pointer array referencing the predictor functions */
UINT (*predictor[])(AUSWAHL *a);

/* initialization and destruction of predictor variables */
int initPredictors(AUSWAHL *a, BOOL *pActive, PARAMETER *param);
void freePredictors(BOOL *pActive);

/* predictor adaption */
void adaptPredictor(AUSWAHL *a, BOOL *pActive, UINT pNr, int pError);

/* predictor functions */
UINT pMED(AUSWAHL *a);
UINT pPaeth(AUSWAHL *a);
UINT pLinearLQ( AUSWAHL *a);
UINT pGAP( AUSWAHL *a);
UINT pCoBALP( AUSWAHL *a);
UINT pTMatching( AUSWAHL *a);

#endif