/*******************************************************************
 *
 * File....: auswahl.h
 * Function: Function prototyping and definition of the AUSWAHL 
 *           (selection) structure.
 * Author..: Thomas Schatton
 * Date....: 11/03/2010, 10.3.2011 (strutz)
 *
 ********************************************************************/

#ifndef AUSWAHL_H_
#define AUSWAHL_H_

#include "..\Typen\types.h"
#include "..\Typen\bild.h"
#include "..\Typen\seiteninfo.h"
#include "..\Typen\parameter.h"
#include "errordb.h"
#include "block.h"
#include "context.h"

/* selection structure */
typedef struct {
	BILD       *src,      /* pointer on input image */
	           *tar;	    /* pointer on output image */
	SEITENINFO **sInfo;	  /* side information */
	BOOL       blockMode; /* flag for block based prediction */
	BLOCK      *bk;			  /* block module */
  KONTEXT    *cx;       /* context module */
  ERRORDB    *db;       /* context data */
	UINT       r, c;			/* current position in image (row, column) */
} AUSWAHL;

/* Constructor & Destructor */
int newAuswahl( AUSWAHL **a, BILD *src, BILD *tar, PARAMETER *p, 
                SEITENINFO **s);
void freeAuswahl(AUSWAHL *a);
	
/* getter for pixels in the local neighborhood */
UINT getPixel(AUSWAHL *a, char val); 
UINT getPixelByIndex( AUSWAHL *a, int i);
UINT getPixelByCoordinates( AUSWAHL *a, int r, int c);
/* getter for gradients of adjacent pixels in the template */
int getGradient(AUSWAHL *a, unsigned char val);
/* returns the current context */
UINT getKontext(AUSWAHL *a); 
UINT getKontextSign( AUSWAHL *a);

#endif