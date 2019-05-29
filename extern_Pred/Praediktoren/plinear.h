/*******************************************************************
 *
 * File....: plinear.h
 * Function: Definition of predictor variables and prototyping of
 *           non-recursive linear prediction functions.
 * Author..: Thomas Schatton
 * Date....: 01/14/2010
 *
 ********************************************************************/

#ifndef _PLINEAR_H_
#define _PLINEAR_H_

#include <math.h>
#include "..\Typen\types.h"
#include "..\Module\auswahl.h"
#include "Funktionen\matrix.h"

#define PLINEARLQ_P 10    /* order of linear prediction */

/* Matrices for computation of a[] */
static MATRIX *plinearX,
       *plinearXtMulW,
       *plinearXtMulWMulX, 
       *plinearInv,
       *plinearW,
       *plinearY;     
/* Vectors of precessor values and weights */
static MATRIX *plinearVx,
       *plinearVa;    

/* initialization of predictor matrices */
int plinearInitialize(); 
/* free matrices */
void plinearFree();

/* calculation of prediction value */
int plinearXHat(AUSWAHL *a);

#endif