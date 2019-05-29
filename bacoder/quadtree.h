/*****************************************************
* File..: quadtree.c
* Desc..: determination of image features
* Author: T. Strutz
* Date..: 02.11.2015
******************************************************/
#include <assert.h>
#include <math.h>
#include "stats.h"
#include "imageFeatures.h"
#include "codec.h" /* for max()	*/
#include "scf.h"	/* 	*/
#include "types.h"	/* for CALLOC */


/*---------------------------------------------------------
* getBlockPredMode()
* checks, whether current block can be predicted horizontally
* or vertically or both
*---------------------------------------------------------*/
unsigned char getBlockPredMode( PicData *pd, 
															 unsigned int y_start, unsigned int y_end,
															 unsigned int x_start, unsigned int x_end);
}

/*---------------------------------------------------------
* getBlockNumColours()
* counting of unique colours in the block
*---------------------------------------------------------*/
unsigned long getBlockNumColours( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);
}

/*---------------------------------------------------------
* getBlockNumPatterns()
* counting of unique Patterns in the block
* due to complexity we determine number separately for each
* component and use the maximum one / the sum ?
*---------------------------------------------------------*/
unsigned long getBlockNumPatterns( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);

/*---------------------------------------------------------
* getBlockNumPatterns2()
* counting of unique Patterns in the block
* due to complexity we determine number based on
* comp 0: A, B, C
* comp 1: A, B, C
* comp 2: A, B,
*---------------------------------------------------------*/
unsigned long getBlockNumPatterns2( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);
}

/*--------------------------------------------------------
* quadtree()
* preparation of quadtree structure, define top level
*-------------------------------------------------------*/
int quadtree( PicData *pd, 
							QUADTREE_ELEM *quadstruc);

