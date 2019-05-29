/*******************************************************************
 *
 * File....: bild.h
 * Function: Function prototyping and definition of the BILD (image)
 *           structure.
 * Author..: Thomas Schatton
 * Date....: 11/02/2010
 *
 ********************************************************************/

#ifndef BILD_H_
#define BILD_H_

#include <math.h>
#include <stdlib.h>
#include "types.h"

//#define BORDERPIXEL 0 /* substituted by a->src->range >> 1	*/

/* image structure */
typedef struct {
	UINT	**wert;	      /* pixel values (one channel, 2-dimensional) */
	ULONG length;				/* amount of pixels */
	UINT	width, height;/* image width and height */
	BYTE	bitDepth;			/* color depth */
	UINT	maxVal;			/*  max. intensity in this component */
	UINT	range;			/*  maxVal + 1  (2 ^ bitDepth) */
} BILD;

/* Constructor & Destructor */
int newBild( BILD **img, UINT width, UINT height, BYTE bitDepth /*, UINT maxVal*/);
void freeBild(BILD *img);

float entropie(BILD *img);            /* entropy calculation */
int histogramm(ULONG **h, BILD *img); /* creates a histogram of the pixel values */
#endif