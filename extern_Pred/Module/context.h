/*******************************************************************
 *
 * File....: context.h
 * Function: Function prototyping and definition of the KONTEXT 
 *           (context) structure.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <math.h>
#include "..\Typen\types.h"

/* context structure */
typedef struct {
	UINT anzKontexte,/* number of possible contexts */
	     cxPx,       /* number of pixels used for context modelling */
		   cxGrad;   /* number of gradients used for context modelling */
} KONTEXT;

/* Constructor & Destructor */
int newKontext(KONTEXT **c, UINT cxGrad, UINT cxPx);
void freeKontext(KONTEXT *c);

#endif