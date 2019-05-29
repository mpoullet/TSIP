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
															 unsigned int x_start, unsigned int x_end)
{
	unsigned char predHor_flag, predVer_flag;
	unsigned int y, x, comp;
	unsigned long pos, py, posH, posV;


	assert( y_end >= y_start);
	assert( x_end >= x_start);
	assert( y_end < pd->height);
	assert( x_end < pd->width);

	/* default: no simple prediction possible	*/
	/* horizontal prediction	*/
	predHor_flag = 0;
	if (x_start) /* not the first column	*/
	{
		predHor_flag = 1; /* assume that horizontal prediction is possible	*/
		/* check all lines of the block	*/
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			posH = py + x_start - 1; /* left neighbour of block line	*/
			/* check all columns of the block	*/
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				/* compare all components	of pixels */
				for( comp = 0; comp < pd->channel; comp++)
				{
					if (pd->data[comp][pos] != pd->data[comp][posH])
					{
						/* reset prediction flag	*/
						predHor_flag = 0;
						y = y_end + 1;
						x = x_end + 1; /* enforce exit of loops	*/
						break;
					}
				}
			}
		}
	} /* if (x_start) */

	/* vertical prediction	*/
	predVer_flag = 0;
	if (y_start) /* not the first row	*/
	{
		predVer_flag = 1;/* assume that vertical prediction is possible	*/
		/* check all columns of the block	*/
		for (x = x_start, pos = x_start; x <= x_end; x++, pos++)
		{
			posV = pos + (y_start-1) * pd->width; /* top neighbour of block column	*/
			/* check all lines of the block	*/
			for (y = y_start, py = pos + y_start * pd->width; y <= y_end; 
				y++, py += pd->width)
			{
				/* compare all components	of pixels */
				for( comp = 0; comp < pd->channel; comp++)
				{
					if (pd->data[comp][py] != pd->data[comp][posV])
					{
						predVer_flag = 0;	/* reset prediction flag	*/
						y = y_end + 1;
						x = x_end + 1; /* enforce exit of loops	*/
						break;
					}
				}
			}
		}
	} /* if (x_start) */

	return ( predHor_flag + 2 * predVer_flag);
}

/*---------------------------------------------------------
* getBlockNumColours()
* counting of unique colours in the block
*---------------------------------------------------------*/
unsigned long getBlockNumColours( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end)
{
	unsigned int y, x;
	unsigned long pos, py, pc;
	unsigned long numColours, colour, size;
	unsigned long *col_ptr = NULL;


	assert( y_end >= y_start);
	assert( x_end >= x_start);
	assert( y_end < pd->height);
	assert( x_end < pd->width);

	/* array for colours */
	size = (y_end - y_start + 1) * (x_end - x_start + 1);
	ALLOC( col_ptr, size, unsigned long);

	pc = 0;
	if (pd->channel >= 3)
	{
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				col_ptr[pc++] = ((unsigned long)(pd->data[2][pos])<<16)+
					((unsigned long)(pd->data[1][pos])<<8) + pd->data[0][pos];
			}
		}
	}
	else
	{
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				col_ptr[pc++] = pd->data[0][pos];
			}
		}
	}

	/* sort colours */
	heap_sort( size, col_ptr);

	/* count colours */
	colour = col_ptr[0];
	numColours = 1;
	for (pos = 1u; pos < size; pos++)
	{
		if (colour != col_ptr[pos])
		{
			numColours++;
			colour = col_ptr[pos];
		}		
	}

	FREE( col_ptr);
	return numColours;
}

/*---------------------------------------------------------
* getBlockNumPatterns()
* counting of unique Patterns in the block
* due to complexity we determine number separately for each
* component and use the maximum one / the sum ?
*---------------------------------------------------------*/
unsigned long getBlockNumPatterns( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end)
{
	unsigned int y, x, comp;
	unsigned int patternNum;
	unsigned long pos, py, pc;
	unsigned long numPatterns, pattern, size;
	unsigned long *pat_ptr = NULL;


	assert( y_end >= y_start);
	assert( x_end >= x_start);
	assert( y_end < pd->height);
	assert( x_end < pd->width);

	/* array for colours */
	size = (y_end - y_start + 1) * (x_end - x_start + 1);
	ALLOC( pat_ptr, size, unsigned long);

	patternNum = 0;
	for ( comp = 0; comp < pd->channel; comp++)
	{
		pc = 0;
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				unsigned long A, B, C, D;

				if (x > 0)
				{
					A = pd->data[comp][pos-1];
					if ( y > 0)
					{
						C = pd->data[comp][pos-1-pd->width];
					}
					else C = 0;
				}
				else A = C = 0;
				
				if ( y > 0)
				{
					B = pd->data[comp][pos-pd->width];
					if ( x < pd->width - 1)
					{
						D = pd->data[comp][pos+1-pd->width];
					}
					else D = 0;
				}
				else B = D = 0;
				
				pat_ptr[pc++] = A + (B<<8) + (C<<16) + (D<<24);
			}
		}

		/* sort colours */
		heap_sort( size, pat_ptr);

		/* count colours */
		pattern = pat_ptr[0];
		numPatterns = 1;
		for (pos = 1u; pos < size; pos++)
		{
			if (pattern != pat_ptr[pos])
			{
				numPatterns++;
				pattern = pat_ptr[pos];
			}		
		}
		patternNum = max( patternNum, numPatterns);
	} /* for comp */

	FREE( pat_ptr);
	return patternNum;
}

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
														 unsigned int x_start, unsigned int x_end)
{
	unsigned int y, x;
	unsigned int patternNum;
	unsigned long pos, py, pc;
	unsigned long numPatterns, size;
	unsigned long long *pat_ptr = NULL, pattern;
	unsigned long long A0, B0, C0;
	unsigned long long A1, B1, C1;
	unsigned long long A2, B2;

	if (pd->channel < 3)
	{
		return getBlockNumPatterns( pd, y_start, y_end, x_start, x_end);
	}

	assert( y_end >= y_start);
	assert( x_end >= x_start);
	assert( y_end < pd->height);
	assert( x_end < pd->width);

	/* array for colours */
	size = (y_end - y_start + 1) * (x_end - x_start + 1);
	ALLOC( pat_ptr, size, unsigned long long);

	patternNum = 0;
	// for ( comp = 0; comp < pd->channel; comp++)
	{
		pc = 0;
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				if (x > 0)
				{
					A0 = pd->data[0][pos-1];
					A1 = pd->data[1][pos-1];
					A2 = pd->data[2][pos-1];
					if ( y > 0)
					{
						C0 = pd->data[0][pos-1-pd->width];
						C1 = pd->data[1][pos-1-pd->width];
					}
					else
					{
						C0 = C1 = 0;
					}
				}
				else A0 = A1 = A2 = C0 = C1 = 0;
				
				if ( y > 0)
				{
					B0 = pd->data[0][pos-pd->width];
					B1 = pd->data[1][pos-pd->width];
					B2 = pd->data[2][pos-pd->width];
				}
				else B0 = B1 = B2 = 0;
				
				pat_ptr[pc++] = A0 + (A1<<8) + (A2<<16) + (B0<<24) + 
									(B1<<32) + (B2<<40) + (C0<<48) + (C1<<56);
			}
		}

		/* sort colours */
		heap_sort_ll( size, pat_ptr);

		/* count colours */
		pattern = pat_ptr[0];
		numPatterns = 1;
		for (pos = 1u; pos < size; pos++)
		{
			if (pattern != pat_ptr[pos])
			{
				numPatterns++;
				pattern = pat_ptr[pos];
			}		
		}
		patternNum = max( patternNum, numPatterns);
	} /* for comp */

	FREE( pat_ptr);
	return patternNum;
}

/*--------------------------------------------------------
* quadtree()
* preparation of quadtree structure, define top level
*-------------------------------------------------------*/
int quadtree( PicData *pd, 
							QUADTREE_ELEM *quadstruc)
{
	int top_level;
	QUADTREE_ELEM *qe;

	// ALLOC( arr_orig, pd->size, unsigned char);

	/* initialise structure for 5 levels fromm 4x4 to 64x64 blocks	*/
	qe = &(quadstruc[0]);
	qe->quad_width = 4;
	qe->threshold = SCF_COLNUM_THRESH1_4_DEFAULT;
	qe->threshold2 = SCF_COLNUM_THRESH2_4_DEFAULT;
	qe->width  = (pd->width  + 3) / 4;
	qe->height = (pd->height + 3) / 4;
	qe->size = qe->width * qe->height;
	top_level = 0;

	qe = &(quadstruc[1]);
	qe->quad_width = 8;
	qe->threshold = SCF_COLNUM_THRESH1_8_DEFAULT;
	qe->threshold2 = SCF_COLNUM_THRESH2_8_DEFAULT;
	qe->width  = (quadstruc[0].width  + 1) / 2;
	qe->height = (quadstruc[0].height + 1) / 2;
	qe->size = qe->width * qe->height;
	if (qe->size > 1) top_level = 1;

	qe = &(quadstruc[2]);
	qe->quad_width = 16;
	qe->threshold = SCF_COLNUM_THRESH1_16_DEFAULT;
	qe->threshold2 = SCF_COLNUM_THRESH2_16_DEFAULT;
	qe->width  = (quadstruc[1].width  + 1) / 2;
	qe->height = (quadstruc[1].height + 1) / 2;
	qe->size = qe->width * qe->height;
	if (qe->size > 1) top_level = 2;

	qe = &(quadstruc[3]);
	qe->quad_width = 32;
	qe->threshold = SCF_COLNUM_THRESH1_32_DEFAULT;
	qe->threshold2 = SCF_COLNUM_THRESH2_32_DEFAULT;
	qe->width  = (quadstruc[2].width  + 1) / 2;
	qe->height = (quadstruc[2].height + 1) / 2;
	qe->size = qe->width * qe->height;
	if (qe->size > 1) top_level = 3;

	qe = &(quadstruc[4]);
	qe->quad_width = 64;
	qe->threshold = SCF_COLNUM_THRESH1_64_DEFAULT;
	qe->threshold2 = SCF_COLNUM_THRESH2_64_DEFAULT;
	qe->width  = (quadstruc[3].width  + 1) / 2;
	qe->height = (quadstruc[3].height + 1) / 2;
	qe->size = qe->width * qe->height;
	if (qe->size > 1) top_level = 4;

	/* allocate arraye for each level	*/
	//for (level = top_level; level >= 0; level--)
	//{
	//		unsigned int w, h, qw, off, th;
	//		/* short cut to parameters at this level	*/
	//		w = quadstruc[level].width;
	//		h = quadstruc[level].height;
	//		qw = quadstruc[level].quad_width;
	//		off = qw - 1;
	//		th = quadstruc[level].threshold;

	//		CALLOC( quadstruc[level].arr, quadstruc[level].size, unsigned char);

	//} /* for level*/

	return top_level;
}

