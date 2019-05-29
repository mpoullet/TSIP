/*******************************************************************
 *
 * File....: block.h
 * Function: Function prototyping and definition of the BLOCK 
 *           (block processing) structure.
 * Author..: Thomas Schatton
 * Date....: 01/14/2011
 * 20.07.2011 Strutz: include BLOCK minSSE, calcBlockSSE()
 ********************************************************************/

#ifndef BLOCK_H_
#define BLOCK_H_

#include <math.h>
#include "..\Typen\types.h"
#include "..\Typen\bild.h"
#include "..\Typen\parameter.h"

/* block structure */
typedef struct {
	BILD  *imgOut;		            /* pointer to output image */
	UINT  width, height;          /* block width/height */
	UINT  count, countX, countY;	/* number of blocks in the image */
	float minEntropie;            /* minimal entropy */
	ULONG minSSE;									 /* minimal sum of squared erros */
	UINT  bestPredictor;          /* predictor index */
	ULONG **hBlock; /* histogram of each block 
                   * ([block][value] -> pixel count */
  UINT  selCriterion; /* 0: use Entropy, 1: use SSE */
} BLOCK;

/* Constructor & Destructor */
int newBlock( BLOCK **b, BILD *out, PARAMETER *p);
void freeBlock(BLOCK *b);

/* row of first pixel in a block */
UINT startRow(BLOCK *block, UINT blockNr);       
/* column of first pixel in a block */
UINT startCol(BLOCK *block, UINT blockNr);       
/* updates the histogramm for a specific block */
void calcHistogramm(BLOCK *block, UINT blockNr);   
void mergeHistograms( BLOCK *b);
/* calculates the entropy of all pixels in all processed blocks */
float blockEntropie(BLOCK *block);        
float blockSSE( BLOCK *b); /* SSE calculation as an alternative */
unsigned long calcBlockSSE( BLOCK *b, UINT blockNr, int range); /* compute block SSE */

#endif