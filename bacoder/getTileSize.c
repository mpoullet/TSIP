/*****************************************************
 * File..: getTileSize.c
 * Desc..: determination of optimal size of tiles
 * Author: T. Strutz
 * Date..: 16.11.2015
 * changes
 * 15.12.2015 skip tiling for images with too less patterns
 * 31.08.2016 adaptive tiling based on colour entropy
 * 04.09.2016 bugfix calculation of tile size
 ******************************************************/
#include <assert.h>
#include <math.h>
#include "codec.h" /* for max()	*/
#include "types.h"	/* for CALLOC */

unsigned long getBlockNumPatterns( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);
unsigned long GetNumColoursBlock( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end, 
														 //unsigned long *maxCount,
														double *colEntropy);

/*--------------------------------------------------------
* getTileSize()
*-------------------------------------------------------*/
void  getTileSize( PicData *pd, unsigned long *tileWidth,
								 unsigned long *tileHeight, 
								 unsigned long *maxTileWidth,
								 unsigned long *maxTileHeight,
								 unsigned int numColours)
{
	int *divXarr=NULL, *divYarr=NULL;
	long *result=NULL, best_result;
	double p, entropy;
	double max_colEntr, min_colEntr;
	unsigned int divX, divY, min_divX, min_divY, max_div;
	unsigned int w, h, flag;
	unsigned int x, y, x_end, y_end, idx;
	unsigned int pos=0;

	fprintf( stderr, "\n get tile sizes ...          ");


	/* at first, determine maximum width and high	=> tiling of very large images	*/
	w = pd->width;
	h = pd->height;

	if (w > h) flag = 1;
	else flag = 0;
	while (w * h > (1L << 22) )
	{
		if (flag)
		{
			w = (w +1 ) /2;
			flag = 0;
		}
		else
		{
			h = (h +1 ) /2;
			flag = 1;
		}
	}
	w = max( w, 64); /* min of 64 will be checked in GUI	*/
	h = max( h, 64); /* min of 64 will be checked in GUI	*/


  /* tiling based on non-iniform distribution of patterns in the image	*/ 

	/* determine minimal divisor	*/
	min_divY = (pd->height + h - 1) / h;
	min_divX = (pd->width  + w - 1) / w;

	w = (pd->width  + (min_divX - 1) ) / min_divX;
	h = (pd->height + (min_divY - 1) ) / min_divY;
	*maxTileHeight = h;
	*maxTileWidth = w;

	/* determine maximal divisor, tile size at least 256x 256	*/
	max_div = (pd->height + 127 ) / 128;
	max_div = min( max_div, (pd->width + 127) / 128);
	max_div = min( max_div, 10); /* stop here otherwise it takes too long	*/

	max_div = min( max_div, 5); /* stop here otherwise it takes too long	*/

	if (max_div <= min_divX  || max_div <= min_divY || numColours < 8000)
	{
		/* no further tiling when image is big */
		*tileWidth = w;
		*tileHeight = h;
		/* store division for logging process	*/
		pd->x_tiles = min_divX;
		pd->y_tiles = min_divY;
		return /*max_div*/;
	}
	else
	{/* XXXXXXXXXXXXXXXXXXXXXXXXX  getBlockNumPatterns() takes too much time */
		//*tileWidth = w;
		//*tileHeight = h;
		///* store division for logging process	*/
		//pd->x_tiles = min_divX;
		//pd->y_tiles = min_divY;
		//fprintf( stderr, 
		//	"\n %dx%d tiles, getTileSize() is deactivated as it is too slow!!!!\n",
		//	pd->x_tiles, pd->y_tiles);
		//return;
	}

#ifdef BlockNumPatterns
	unsigned int sum_patt, patt, max_patt, min_patt;
	unsigned int *pattArr = NULL;
	/* getBlockNumPatterns based on ABCD only	*/
	patt = getBlockNumPatterns( pd, 0, pd->height-1, 0, pd->width-1);

	if (patt < 2000)
	{ /* if too less patterns in entire image skip tiling	*/
		*tileWidth = w;
		*tileHeight = h;
		/* store division for logging process	*/
		pd->x_tiles = min_divX;
		pd->y_tiles = min_divY;
		return;
	}

	CALLOC( result, (max_div + 1) * (max_div + 1), long);
	CALLOC( divXarr, (max_div + 1) * (max_div + 1), int);
	CALLOC( divYarr, (max_div + 1) * (max_div + 1), int);
	CALLOC( pattArr, (max_div + 1) * (max_div + 1), unsigned int);

	/* test all combinations of divisions in horizontal and vertical direction	*/
	best_result = -1;
	for (divY = 1; divY <= max_div; divY++)
	{
		for (divX = 1; divX <= max_div; divX++)
		{
			/* array position for the current combination of horizontal and vertical tiling */
			pos = (divX - 1) + (divY - 1) * max_div;
			divXarr[pos] = divX; /* remember tiling	*/
			divYarr[pos] = divY;

			/* divX = divY = 1; no tiling	*/
			if (divX <= min_divX || divY <= min_divY) 
			{ 
				result[pos] = 1000; 
				continue;
			}
			fprintf( stderr, "%2dx%2d", divX, divY);
			fprintf( stderr, "\b\b\b\b\b");
			/* count patterns	*/
			sum_patt = 0;
			min_patt = pd->size;  /* assume maximum number	*/
			max_patt = 0; 
			h = (pd->height + divY - 1)/ divY;
			w = (pd->width  + divX - 1)/ divX;
			idx = 0;

			/* for each tile	*/
			for (y = 0; y < pd->height; y += h)
			{
				for (x = 0; x < pd->width; x += w)
				{
					/* (x,y) is top left corner,	(x_end,y_end) is bottom right corner */
					y_end = min( y+h, pd->height) - 1;
					x_end = min( x+w, pd->width) - 1;
					/* getBlockNumPatterns based on ABCD only	*/
					patt = getBlockNumPatterns( pd, y, y_end, x, x_end);
					pattArr[idx] = patt; /* store number of patterns	*/
					sum_patt += pattArr[idx]; /* total number of patterns	*/
					idx++;
					//if (min_patt > patt) min_patt = patt;
					//if (max_patt < patt) max_patt = patt;
				}
			}

			/* compute entropy of patterns distribution	
			 * idea: the higher the entropy the more uniform the patterns are spread, the
			 * lower the chance that tiling will improve the compression performance
			 */
			entropy = 0.;
			for (x = 0; x < idx; x++)
			{
				p = (double)pattArr[x] / sum_patt;
				entropy -= p * log( p);
			}
			//result[pos] = (1000 * sum_patt) / (divX * divY * (max_patt - min_patt + 1));
			/* normalised by H0, Enstcheidungsgehalt	*/
			assert( divX * divY > 1);
			result[pos] = (long)( 1000 * entropy / log( (double)divX * divY) ) ;
			/* add overhead penalty, the smaler the image  and the higher the divisors the
			 * larger the penalty
			 */
			result[pos] += (divX * divY - 1) * 10000000 / pd->size;
		}
	}
	FREE( pattArr);
#else
	{
		double colEntropy, sum_colEntr, inv_sum_colEntr;
		double *colEntrArr = NULL;
		//unsigned long maxCount;

		//uint dummy = GetNumColoursBlock( pd, 0, pd->height-1, 0, pd->width-1, /* &maxCount, */ &(colEntropy));
		if (numColours < 1000)
		{ /* if too less colours in entire image, skip tiling	*/
			*tileWidth = w;
			*tileHeight = h;
			/* store division for logging process	*/
			pd->x_tiles = min_divX;
			pd->y_tiles = min_divY;
			return;
		}

		CALLOC( result, (max_div + 1) * (max_div + 1), long);
		CALLOC( divXarr, (max_div + 1) * (max_div + 1), int);
		CALLOC( divYarr, (max_div + 1) * (max_div + 1), int);
		CALLOC( colEntrArr, (max_div + 1) * (max_div + 1), double);

		/* test all combinations of divisions in horizontal and vertical direction	*/
		best_result = -1;
		pos = 0;
		for (divY = 1; divY <= max_div; divY++)
		{
			for (divX = 1; divX <= max_div; divX++)
			{
				/* array position for the current combination of horizontal and vertical tiling */
				// pos = (divX - 1) + (divY - 1) * max_div;
				divXarr[pos] = divX; /* remember tiling	*/
				divYarr[pos] = divY;

				/* divX = divY = 1; no tiling	*/
				if (divX <= min_divX && divY <= min_divY) 
				{ 
					result[pos++] = 1000; 
					continue;
				}
				fprintf( stderr, "%2dx%2d", divX, divY);
				fprintf( stderr, "\b\b\b\b\b");
				/* count patterns	*/
				sum_colEntr = 0.;
				max_colEntr = 0.;
				min_colEntr = 999999.;
				h = (pd->height + divY - 1)/ divY;
				w = (pd->width  + divX - 1)/ divX;
				idx = 0;

				/* for each tile	*/
				for (y = 0; y < pd->height; y += h)
				{
					for (x = 0; x < pd->width; x += w)
					{
						/* (x,y) is top left corner,	(x_end,y_end) is bottom right corner */
						y_end = min( y+h, pd->height) - 1;
						x_end = min( x+w, pd->width) - 1;
						//patt = getBlockNumPatterns( pd, y, y_end, x, x_end);
						numColours = GetNumColoursBlock( pd, y, y_end, x, x_end,/* &maxCount, */&(colEntropy));
						colEntrArr[idx] = colEntropy; /* store number of patterns	*/
						sum_colEntr += colEntrArr[idx]; /* total number of patterns	*/
						idx++;
						if (min_colEntr > colEntropy) min_colEntr = colEntropy;
						if (max_colEntr < colEntropy) max_colEntr = colEntropy;
					}
				}

				/* compute entropy of patterns distribution	
				 * idea: the higher the entropy the more uniform the patterns are spread, the
				 * lower the chance that tiling will improve the compression performance
				 */
				entropy = 0.;
				inv_sum_colEntr = 1. / sum_colEntr;
				for (x = 0; x < idx; x++)
				{
					if ( colEntrArr[x] > 0.)
					{
						p = (double)colEntrArr[x] * inv_sum_colEntr;
						entropy -= p * log( p);
					}
				}
				/* normalised by H0, Enstcheidungsgehalt	*/
				assert( (divX + divY) > 2);
				result[pos] = (long)( 1000 * entropy / log( (double)divX * divY) ) ;

				/* add overhead penalty, the smaller the image and the higher the divisors the
				 * larger the penalty
				 */
				//result[pos] += (divX * divY - 1) * 10000000 / pd->size;
				pos++;
			}
		}
		FREE( colEntrArr);
	}
#endif
	/* check, whether entropy is small enough and which tiling results to 
	 * lowest entropy  => most non-uniform distribution of patterns
	 */
	y = pos; /* remember maximum pos	*/
	pos = 0;
	best_result = result[pos];
	for (x = 1; x < y; x++) 
	{
		if (result[x] < 800 && best_result > result[x])
		{
			best_result = result[x];
			pos = x;
		}
	}


	*tileWidth = (pd->width + divXarr[pos] - 1)/ divXarr[pos];
	*tileHeight = (pd->height + divYarr[pos]-1) / divYarr[pos];

	/* store division for logging process	*/
	pd->x_tiles = divXarr[pos];
	pd->y_tiles = divYarr[pos];

	FREE( result);
	FREE( divXarr);
	FREE( divYarr);
	return /* div */;
}

