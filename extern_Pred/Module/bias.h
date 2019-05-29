/*******************************************************************
 *
 * File....: bias.h
 * Function: Function prototyping and definition of the BIAS 
 *           (bias compensation) structure.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#ifndef BIAS_H_
#define BIAS_H_

#include <math.h>
#include "..\Typen\types.h"
#include "..\Typen\parameter.h"
#include "auswahl.h"

/* bias structure */
typedef struct {
	AUSWAHL *aw;    /* pointer on selection structure */
	long    *A,     /* SAD of prediction errors */
		      *Bi,     /* bias compensation */
		      *C,     /* correction value per context (offset) */
		      *N;     /* number of events per context */ 
	UINT    reset;  /* adaption frequency */
} BIAS;

/* Construktor & Destructor */
int newBias( BIAS **b, AUSWAHL *a, PARAMETER *p);
void freeBias(BIAS *b);

/* rescales the bias compensation */
void rescale(BIAS *b, int e);   
/* returns the correction value for the current context */
int  getOffset(BIAS *b); 

#endif