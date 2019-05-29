/*******************************************************************
 * File....: adjmap.c
 * Function: Contains the functions implementing the adjacency-mapping
 *           algorithm.
 * Author..: Toia-Cret Titus-Ruben
 * Date....: 09.05.2012
 * 30.07.2012 Strutz: changes for TSIP v1.14
 * 30.07.2012 Strutz: use ALLOC for all allocations
 * 28.08.2013 bug fix: pd->bitperchannel must be updated after adjacency mapping
 ********************************************************************/
#include "stdafx.h"
#include "adjmap.h"
/*------------------------------------------------------------------
 * typedef struct AdjData
 *
 * This is used as an internal wrapper structure for the sake of the
 * adjacency-based algorithm. The PicData structure is not
 * comprehensive enough to hold all the information needed for the
 * adj-algorithm, and a new structure was needed.
 * The additional data included is the palette, the palette
 * occurrence ratios, and the number of colours. This is somewhat
 * of a pseudo-facade design pattern choice.
 *-----------------------------------------------------------------*/
typedef struct AdjData
{
	PicData* pd;
	unsigned char* palette;
	double* palette_ratios;
	uint numColours;
} AdjData;

/* Internal functions */
void Adjmap_buildmap2( AdjData* img);
void Adjmap_buildmap( AdjData* img, int sort_palette = 0);
void Adjmap_buildremap( AdjData* img);
void Adjmap_preinitialise( AdjData* img, plist* adj_arr);
void Adjmap_buildmap_from_remap( AdjData* img);
void Adjmap_buildpixels_from_map( AdjData* img);


/*
 * Encoder functions
 */


/*------------------------------------------------------------------
 * uint Adjmap_perform_map2( )
 * 
 * An interface function for the main body of the adjacency-based
 * remapping algorithm. Internally, it calls other functions which
 * incrementally apply the algorithm.
 * The PicData* structure and the uint* palette are wrapped into
 * the internally-used Adjmap structure.
 * The usage of an uint* array to represent an RGB array instead
 * of a char* array is more expensive (1 extra byte), but it allows
 * for the use of the already present sorting functions in the
 * codebase.
 *-----------------------------------------------------------------*/
uint Adjmap_perform_map2( PicData* pd, unsigned char *palette, uint numColours)
{
	AdjData* img;

	ALLOC( img, 1, AdjData);
	img->pd = pd;
	img->numColours = numColours;

	img->palette = palette;
	/*
	Do not transmit any other value than 0, as the encoder cannot write
	the palette if it occurrence-sorted.
	*/
	Adjmap_buildmap2( img); /* Strutz: modified version	*/
	Adjmap_buildremap(img);

	free(img);

	/* determine new bit depth of data in component 0 */
	{
		int bpc;
		long max_val = 0;
		unsigned long pos;

		for ( pos = 0; pos < pd->size; pos++)
		{
			if (max_val < (signed)pd->data[0][pos]) max_val = pd->data[0][pos];
		}
		bpc = 0;
		while (max_val)
		{
			bpc++;
			max_val >>= 1;
		}
		pd->bitperchannel = bpc;
		pd->bitperchan[0] = bpc;
	}
	return numColours;
}



/*------------------------------------------------------------------
 * void Adjmap_buildmap2( AdjData* img)
 *
 * The first internal procedure used in the adjacency mapping
 * algorithm. 
 * not: This procedure counts the number of distinct colours,
 *			constructs and sorts the palette, 
 * and replaces the internal
 * PicData's data[0] colour values with the newly-obtained palette
 * values.
 *
 *-----------------------------------------------------------------*/
void Adjmap_buildmap2( AdjData* img)
{
	uint i, idx;
	uint numColours = img->numColours;
	uint pixel_cnt = img->pd->size;
	uint* pixels_map = img->pd->data[0];
	//PicData* pd = img->pd;
	double* palette_ratios = NULL;


	//Second pass through sorted pixels, construct palette and
	//construct mapped pixel array;
	//palette_ratios = (double*) malloc(numColours * sizeof(double));
	ALLOC( palette_ratios, numColours, double);

	for(i = 0; i < numColours; i++)
	{
		palette_ratios[i] = 0; //Initialise with 0.
	}

	for (i = 0; i < pixel_cnt; i++)
	{
		idx = pixels_map[i];
		palette_ratios[idx]++;
	}
	//Finish colour ratio array.
	for (i = 0; i < numColours; i++)
	{
		palette_ratios[i] = palette_ratios[i] / pixel_cnt;
	}

	img->palette_ratios = palette_ratios;
}


/*------------------------------------------------------------------
 * void Adjmap_buildremap(AdjData* img)
 *
 * The final internal procedure of the adj. algorithm. The procedure
 * replaces the mapped values in the internal PicData's data[0] with
 * remapped values, in a way that the original map can be reconstructed
 * exclusively by means of the palette.
 * Also uses another internal procedure, Adjmap_preinitialise which
 * initialises the adjacency lists with values based on colour similarity.
 *-----------------------------------------------------------------*/
void Adjmap_buildremap( AdjData* img)
{
	PicData* pd = img->pd;
	uint pixel_cnt = pd->size;
	uint* pixels_map = pd->data[0];
	uint numColours = img->numColours;
	uint i, priority;
	uint* pixels_remap = NULL;
	plist* adj_arr = NULL;

	//pixels_remap = (uint*) malloc(pixel_cnt * sizeof(uint));
	ALLOC( pixels_remap, pixel_cnt, uint);
	//adj_arr = (plist*) malloc(numColours * sizeof(plist));
	ALLOC( adj_arr, numColours, plist);

	//Initialise adjacency lists
	for (i = 0; i < numColours; i++)
	{
		adj_arr[i].index = i;
		adj_arr[i].first = NULL;
		adj_arr[i].length = 0;
	}
	Adjmap_preinitialise(img, adj_arr);

	uint index, index_prev;
	pixels_remap[0] = pixels_map[0];
	for (i = 1; i < pixel_cnt; i++)
	{
		index = pixels_map[i];
		index_prev = pixels_map[i-1];
		plist adj = adj_arr[index_prev];
		
		//If respective pair has been encountered
		if ((priority = plist_get_priority( adj, index)) != -1)
		{
			pixels_remap[i] = priority;
		}
		else //Place a "marker" symbol.
		{
			pixels_remap[i] = adj.length + index;
		}
		plist_add_occ( &adj_arr[index_prev], index);
	}
	//Copy back img_remap to img_map
	for(i = 0; i < pixel_cnt; i++)
	{
		pixels_map[i] = pixels_remap[i];
	}

	//Free memory
	free(pixels_remap);
	for (i = 0; i < numColours; i++)
	{
		plist_free(&adj_arr[i], 1);
	}
	free(adj_arr);
}

/*###############################################################
 * Decoder functions
 */

/*------------------------------------------------------------------
 * void Adjmap_perform_rebuild2( )
 *
 * An interface function for the reversed adjacency-based remapping
 * algorithm, which rebuilds the original pixel map, and then the
 * original pixel colour data.
 * The PicData* structure and uint* palette are wrapped into the
 * internally-used Adjmap structure.
 *-----------------------------------------------------------------*/

void Adjmap_perform_rebuild2( PicData* pd, unsigned char* palette, 
														 uint numColours)
{
	AdjData* img = NULL;
	
	//img = (AdjData*) malloc(sizeof(AdjData));
	ALLOC( img, 1, AdjData);

	img->pd = pd;
	img->numColours = numColours;

	img->palette = palette;
	Adjmap_buildmap_from_remap( img);

//	Adjmap_buildpixels_from_map(img);
}


/*------------------------------------------------------------------
 * void Adjmap_buildmap_from_remap(AdjData* img)
 *
 * Reconstructs the mapped pixel values from the remapped ones,
 * using the palette, and reversing the adjacency-remapping
 * algorithm.
 *-----------------------------------------------------------------*/
void Adjmap_buildmap_from_remap( AdjData* img)
{
	PicData* pd = img->pd;
	uint pixel_cnt = pd->size;
	uint numColours = img->numColours;
	uint index_curr, index, i;
	pnode* node = NULL;
	
	uint* pixels_remap = pd->data[0];
	uint* pixels_map = NULL;
	plist* adj_arr = NULL;


	//pixels_map = (uint*) malloc( pixel_cnt * sizeof(uint));
	ALLOC( pixels_map, pixel_cnt, uint);
	//adj_arr = (plist*) malloc(numColours * sizeof(plist));
	ALLOC( adj_arr, numColours, plist);

	index_curr = index = pixels_remap[0];

	//Initialise adjacency lists
	for (i = 0; i < numColours; i++)
	{
		adj_arr[i].index = i;
		adj_arr[i].first = NULL;
		adj_arr[i].length = 0;
	}
	Adjmap_preinitialise( img, adj_arr);

	//Build original image map.
	pixels_map[0] = index_curr; //The 0th index is identical in both arrays.
	for (i = 1; i < pixel_cnt; i++)
	{
		plist* adj = &adj_arr[index_curr];
		index = pixels_remap[i];
		if (index >= adj->length) //New colour pair
		{
			index_curr = pixels_remap[i] - adj->length;
		}
		else
		{
			node = plist_get_node( *adj, index);
			index_curr = node->index;
		}
		plist_add_occ( adj, index_curr);
		pixels_map[i] = index_curr;
	}

	//Copy back the values.
	for(i = 0; i < pixel_cnt; i++)
	{
		pixels_remap[i] = pixels_map[i];
	}

	//Free memory
	free(pixels_map);
	for (i = 0; i < numColours; i++)
	{
		plist_free( &adj_arr[i]);
	}
	free( adj_arr);
}

/*------------------------------------------------------------------
 * void Adjmap_buildpixels_from_map(AdjData* img)
 *
 * Reconstructs the pixel colours values from the pixel map, using
 * the information in the colour palette. Adapted to the PicData
 * structure, splits the colour data into the 3 respective channels.
 *-----------------------------------------------------------------*/
void Adjmap_buildpixels_from_map( AdjData* img)
{
	unsigned char* palette = img->palette;
	PicData* pd = img->pd;
	uint col;

	for (uint i = 0; i < pd->size; i++)
	{
		//index = pd->data[0][i];
		//pd->data[0][i] = (palette[index] >> 8) & 0xFF;
		//pd->data[1][i] = (palette[index] >> 16) & 0xFF;
		//pd->data[2][i] = palette[index] & 0xFF;

		col = palette[ pd->data[0][i] ];
		pd->data[2][i] = col & 0xff;
		col >>= 8;
		pd->data[1][i] = col & 0xff;
		col >>= 8;
		pd->data[0][i] = col;
	}
}


/*###############################################################
 * general functions
 */


/*------------------------------------------------------------------
 * void Adjmap_preinitialise(AdjData* img, plist* adj_arr)
 *
 * This procedure uses the palette to add preliminary information
 * in the adjacency list, thus reducing (most of the time) the
 * overall entropy of the final remap.
 *-----------------------------------------------------------------*/
void Adjmap_preinitialise( AdjData* img, plist* adj_arr)
{
	int flag, threshold = 1;
	uint i, j, i3, j3;
	unsigned char* palette = img->palette;
	uint numColours = img->numColours;

	for (i = 0, i3=0; i < numColours; i++, i3+=3)
	{
		/*
		Use the following "if" to limit the amount of
		adjacency lists to be considered.
		*/
		/* if (palette_ratios[i] >= 0.00001) */
		{
			for (j = 0, j3=0; j < numColours; j++, j3+=3)
			{
				if (i == j)
				{
					continue;
				}
				flag = abs(palette[i3] - palette[j3]) <= threshold &&
							abs(palette[i3+1] - palette[j3+1]) <= threshold &&
							abs(palette[i3+2] - palette[j3+2]) <= threshold;

				if (flag)
				{
					plist_add_occ(&adj_arr[i], j);
				}
			}
		}
	}
}
