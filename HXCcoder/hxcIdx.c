/*****************************************************
* File..: hxc.c
* Desc..: implementation of H(X|C) coder
* Author: T. Strutz
* Date..: 02.09.2013
* changes:
* 08.09.2013 new: max_similar_pattern
* 04.02.2012 allocate several pattern lists
 * 13.03.2014 new HXC_MAX_MAXVAL
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"
#include "codec.h"	/* for min( a, b);	*/
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"


/*--------------------------------------------------------
 * HXC_coding()
 *--------------------------------------------------------*/
void HXC_codingIdx( PicData *pd,		/* structure with image data	*/
								BitBuffer *bitbuf, /* interface for file input and output*/
								AutoConfig *aC,  /* parameter of codec */
								int encoder_flag,	/* 1 ==> encoder; 0=> decoder	*/
								unsigned int comp,  /* colour component	*/
								//unsigned int numColors,  /* Image features Added 15.05.2014 */ 
								unsigned char *palette)	/* Original palette Added 15.05.2014 */
{
	int err = 0;
	long templ[MAX_PATTERN_SIZE];	/* template for actual pattern	*/
	long templ2[MAX_PATTERN_SIZE];	/* template for list pattern	*/
	unsigned int r, c; /* row an d column	*/
	unsigned int tolerance[MAX_PATTERN_SIZE];	/* tolerence in pattern comparison	*/
	unsigned int max_similar_pattern;	/* reasonable number of similar patterns	*/
	unsigned int maxVal;
	unsigned long i;
	unsigned long numLists = 0;
	unsigned long symbol;
	unsigned long K_;	/* number of symbols per component	*/
	unsigned long bytes_old; /* temporary number of written bytes	*/

	/* 15.05.2014 Batev: */
	PALETTE_ELEM *palettePointer = NULL;
	unsigned int  max_sim_idx = 0;

	/* pointer to array of pointers to single list elements	*/
	PATTERN_LIST_ELEM **similar_elements = NULL;	
	PATTERN_LIST *pat_list = NULL; /* top of dictionary lists */

	acModell M;	/* normal AC context	*/
	acState acState_obj, *acSt;	/* state of arithmetic coder	*/

	acSt = &acState_obj;
	assert( aC->HXCincreaseFac >= 0);

	maxVal = pd->maxVal_pred[comp];

	/* 15.05.2014 Allocate palette array of structrures PALETTE_ELEM */
	ALLOC( palettePointer, maxVal+1, PALETTE_ELEM);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXCidx;
	}
	for (i = 0; i <= maxVal; i++)
	{
		palettePointer[i].simIndicesTol = NULL;
		palettePointer[i].simIndicesOff1 = NULL;
		palettePointer[i].simIndicesOff2 = NULL;
	}


	/* start with empty dictionary list, +1 because 0 has to be included */
	if (pd->maxVal_pred[comp] > HXC_MAX_MAXVAL)
	{	/* linear array, slower search for matches	*/
		numLists = (pd->maxVal_pred[comp] + 1);
		fprintf( stderr, "\n ! HXCcoder: number of colours too high (%d > %d) !", 
			pd->maxVal_pred[comp], HXC_MAX_MAXVAL);
		fprintf( stderr, "\n ! HXCcoder: must switch to linear adressing causing slow search!\n");
	}
	else
	{
		numLists = (pd->maxVal_pred[comp] + 1) * (pd->maxVal_pred[comp] + 1);
	}
	ALLOC( pat_list, numLists, PATTERN_LIST);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXCidx;
	}
	for (i = 0; i < numLists; i++)
	{
		(pat_list[i]).pattern_list_size = 0;
		(pat_list[i]).top.next = NULL;
		(pat_list[i]).top.prev = NULL;
		(pat_list[i]).top.top_hist_elem = NULL; /* empty histogramm list	*/
		(pat_list[i]).top.r = 0;
		(pat_list[i]).top.c = 0;
	}
	
	/* pre init
	 *     F
   *   C B D
	 * E A X
	 */
	/* tolerance defines the maximum difference between pixels of current template and
	 * pixels of reference template. If difference is higher than tolerance, then
	 * reference template is NOT regarded as match
	 */
	for (c = 0; c < MAX_PATTERN_SIZE; c++)
	{
		tolerance[c] = aC->HXCtolerance; /* should be updated somehow	*/
	}
	/* the closer to X the lower the  tolerance, avois negativ values	*/
	tolerance[2] = tolerance[0] + aC->HXCtoleranceOffset1;
	tolerance[3] = tolerance[1] + aC->HXCtoleranceOffset1;
	tolerance[4] = tolerance[2] + aC->HXCtoleranceOffset2;
	tolerance[5] = tolerance[3] + aC->HXCtoleranceOffset2;

	/* 15.05.2014 Reasonable number of similar indices arrays*/
	max_sim_idx = min( maxVal+2, 200); 

	/* 15.05.2014 Batev: First initialization of the palette */
		/* Batev: Copies the palette in a new array of 
			structures ( PALETTE_ELEM ) and allocates
			arrays, where similar patterns are stored */
	err = BuildPalette( palette, maxVal, palettePointer, max_sim_idx); 
	if (err)
	{
		goto endHXCidx;
	}
	/* 15.05.2014 Batev: Collect similar palette elements */
	CollectSimilarPaletteElements( maxVal, palettePointer, 
			tolerance[1], tolerance[3], tolerance[5],	max_sim_idx);
	

	/* compute maximal required size in dependence on possible similar pattern
	 * the higher 'tolerance'	the more patterns are regareded as match, which 
	 * have to be stored in pattern list
	 */
	//max_similar_pattern = 3; 
	//for (c = 0; c < MAX_PATTERN_SIZE; c++)
	//{
	//	max_similar_pattern *= tolerance[c]*2 + 1; 
	//}
	///* reasonable maximum number of similar pattern, save guard	*/
	//max_similar_pattern = min( max_similar_pattern, 2000); 
	max_similar_pattern = 2000;  /* set to maximum	*/

	/* allocate array for pointers, one more for list termination, two needed, Why??	*/
	/* Batev: One more for list termination, and the second one for the current pattern,  
	 * if it is to be added, as well
	 */
	ALLOC( similar_elements, max_similar_pattern+2, PATTERN_LIST_ELEM *);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXCidx;
	}

	/* prepare arithmetic coding	*/
	if (encoder_flag)
	{
		start_encoding( acSt); /* reset internal values	*/
	}
	else
	{
		start_decoding( acSt, bitbuf); /* reset internal values	*/
	}

	/* build models or arithmetic coding	*/
	start_model( &M, pd->maxVal_pred[comp] +1);
	K_ = M.K;	/* copy size of cumulative histogram 	*/


	fprintf( stderr, "\n");
	/* process image	*/
	for( r = 0; r < pd->height; r++)
	{
		bytes_old = numBytes; /* check bytes per row	*/
		for( c = 0; c < pd->width; c++) 
		{
			copy_patternIdx( pd, r, c, templ, comp);	/* get current pattern from image	*/

			/* search list for matching patterns,
			 * collect information about count statistics,
			 * build distribution
			 */
			get_distribution_Idx( pd, r, c, templ, templ2, pat_list, similar_elements,
					M.H, K_, tolerance, max_similar_pattern,
					aC->HXCincreaseFac, comp, palettePointer);


			if (encoder_flag)
			{
				/* do the encoding based on the determined distribution	*/
				symbol = pd->data[comp][c + r * pd->width];
				/* do the arithmetic coding	*/
				encode_AC( &M, acSt, symbol, bitbuf);
			}
			else
			{
				/* do the arithmetic decoding	*/
				symbol = decode_AC( &M, acSt, bitbuf);

				/* store decoded signal value	*/
				pd->data[comp][c + r * pd->width] = symbol;
			}

			/* include information about true signal value in list	*/
			update_pattern_list( similar_elements, symbol);
		} /* for( c = 0 */
	
		/* show number of pattern in actual list */
		{
			unsigned long numPattern = 0;
			for (i = 0; i < numLists; i++)
			{
				numPattern+= pat_list[i].pattern_list_size;
			}
			fprintf( stderr, "\r%3.2f  pattern: %d  ", 100.*(r+1) / pd->height, numPattern);
		}

		if (encoder_flag)
		{
			fprintf( stderr, "  %d Bytes  ", numBytes - bytes_old);
#ifdef _DEBUG
			fprintf( stdout, "\n r=%d: %d Bytes  ", r, numBytes - bytes_old);
#endif
		}

	} /* for( r = 0 */

	if (encoder_flag)
	{
		finish_encode( acSt, bitbuf);	/* finish arithmetic coding	*/
		fprintf( stderr, "\ntotal:  %d Bytes", numBytes);

		aC->num_HXC_patterns = 0;
		// for (i = 0; i <= pd->maxVal[comp]; i++ )
		for (i = 0; i < numLists; i++ )
		{
			aC->num_HXC_patterns += pat_list[i].pattern_list_size;
		}
	}

endHXCidx:
	/* clear arithmetic coding	*/
	free_model( &M);
	FREE( similar_elements);
/*	free_imatrixIdx( &templ); Batev: 28.06.2014 It's a vector now
	free_imatrixIdx( &templ2); */
	
	/* free all allocated patter list elements and the
	 * corresponding histogram elements
	 */
	for (i = 0; i < numLists; i++ )
	{
		free_pattern_list( &(pat_list[i].top));
	}
	FREE( pat_list);

	for (i = 0; i <= maxVal; i++)
	{
		free( palettePointer[i].simIndicesTol);
		free( palettePointer[i].simIndicesOff1);
		free( palettePointer[i].simIndicesOff2);
	}
	free( palettePointer);
}


/* ------------------------------------------------------------------------------
 * void BuildPalette();
 * ------------------------------------------------------------------------------ */ 

int BuildPalette(unsigned char *palette,       /* original palette */
				  unsigned int maxVal,      /* equal to iF->numColours */
				  PALETTE_ELEM *palettePointer, /* pointer to array for the new palette */
				  unsigned long max_sim_idx)    /* max number of possible similar indices */
{
	unsigned long m; //, z; // going through loops 

	for (m = 0; m <= maxVal; m++)
	{
		(palettePointer[m]).Blue = palette[m*3];			/* *palette stores colours in BRG BRG BRG format */ 
		(palettePointer[m]).Red = palette[m*3 + 1];
		(palettePointer[m]).Green = palette[m*3 + 2];

			// allocate array for similar indices for every palette element with tolerance[1]
				/* plus 1, because '-1'-element needs space	*/
        ALLOC( palettePointer[m].simIndicesTol , max_sim_idx + 1 , signed long); 
				if (ERRCODE == MALLOC_FAILED)
				{
					return 1;
				}
				palettePointer[m].simIndicesTol[0] = m;
				/* First position of each similar list is the index itself. It is used
				 * when a proper pattern list is searched */

				/* Initialization of the whole array tooks too much time */ 
				palettePointer[m].simIndicesTol[1] = -1;   // 20.05.2014
    

      // allocate array for similar indices for every palette element with tolerance[3]
		
        ALLOC( palettePointer[m].simIndicesOff1, max_sim_idx + 1, signed long); 
				if (ERRCODE == MALLOC_FAILED)
				{
					return 1;
				}
				palettePointer[m].simIndicesOff1[0] = m;
			  palettePointer[m].simIndicesOff1[1] = -1; // Used later as a flag  20.05.2014

			// allocate array for similar indices for every palette element with tolerance[5]
    
        ALLOC( palettePointer[m].simIndicesOff2, max_sim_idx + 1, signed long); 
				if (ERRCODE == MALLOC_FAILED)
				{
					return 1;
				}
				palettePointer[m].simIndicesOff2[0] = m;
	      palettePointer[m].simIndicesOff2[1] = -1; // Used later as a flag 20.05.2014
  }
	return 0;
}

/* ------------------------------------------------------------------------------
 * void CollectSimilarPaletteElements();
 * ------------------------------------------------------------------------------ */

void CollectSimilarPaletteElements( unsigned int maxVal, 
									PALETTE_ELEM *palettePointer, 
								   unsigned int tol, unsigned int Off1, unsigned int Off2,
									 unsigned int max_sim_idx)
{
	unsigned long Index;
	unsigned long IdxToCompare;
	int subR, subG, subB;
	unsigned int zIndex, zCompare; 

	/* Batev: Collects similar patterns for positions A and B */
	if( tol > 0) /* If tolerance = 0, there are no similar patterns. It saves time */
	{
		for( Index = 0; Index <= maxVal; Index++)
		{ /* Goes through all indices in the palette */
			for( zIndex = 1; zIndex < max_sim_idx; zIndex++)
			{ /* Find first empty position in the similarity list for the processing index */
				if( palettePointer[Index].simIndicesTol[zIndex] == -1)
				{
					break;
				}
			}
			for( IdxToCompare = Index+1; IdxToCompare <= maxVal; IdxToCompare++)
			{/* Goes through all indices higher than the processing one
  			* and checks if they are similar or not */
				subR = palettePointer[Index].Red - palettePointer[IdxToCompare].Red;
				subG = palettePointer[Index].Green - palettePointer[IdxToCompare].Green;
				subB = palettePointer[Index].Blue - palettePointer[IdxToCompare].Blue;

				/* If the values of all three components differ less than the tolerance 
				 * value IdxToCompare added to the current index similarity list and 
				 * current index to the IdxToCompare similarity list */ 
				if( subR >= -(signed) tol && subR <= (signed)tol && 
						subG >= -(signed) tol && subG <= (signed)tol && 
						subB >= -(signed) tol && subB <= (signed)tol)
				{
					/* If list of index X is full, but this of index Y not, and they are similar. After collecting => X is similar to Y,
					 * but Y is not similar to X */ 
					if( zIndex < max_sim_idx - 1)
					{
						/* Write the similar index in the list */
						palettePointer[Index].simIndicesTol[zIndex] = IdxToCompare;
						zIndex++;
						/* Move the flag for emptiness by one position */
						palettePointer[Index].simIndicesTol[zIndex] = -1; 
					}

					for( zCompare=1; zCompare < max_sim_idx-1; zCompare++)
					{
						/* Find first empty position */
						if( palettePointer[IdxToCompare].simIndicesTol[zCompare] == -1)
						{
							palettePointer[IdxToCompare].simIndicesTol[zCompare] = Index;
							palettePointer[IdxToCompare].simIndicesTol[zCompare + 1] = -1;  
							break;
						} // if(-1)
					}// for(zCompare)
				}// if (similar)
			} // for(IdxToCompare)
		} // for(Index)
	} //if( tol > 0)

	/* Batev: Collects similar patterns for positions C and D */
	if( Off1 > 0) /* If tolerance = 0, there are no similar patterns. It saves time */
	{
		for( Index = 0; Index <= maxVal; Index++)
		{
			for( zIndex = 1; zIndex < max_sim_idx; zIndex++)
			{
				if( palettePointer[Index].simIndicesOff1[zIndex] == -1)
				{
					break;
				}
			}
		
			for( IdxToCompare = Index+1; IdxToCompare <= maxVal; IdxToCompare++)
			{
				subR = palettePointer[Index].Red - palettePointer[IdxToCompare].Red;
				subG = palettePointer[Index].Green - palettePointer[IdxToCompare].Green;
				subB = palettePointer[Index].Blue - palettePointer[IdxToCompare].Blue;

				if( subR >= -(signed) Off1 && subR <= (signed)Off1 && 
						subG >= -(signed) Off1 && subG <= (signed)Off1 && 
						subB >= -(signed) Off1 && subB <= (signed)Off1)
				{
					if( zIndex < max_sim_idx - 1)
					{
						palettePointer[Index].simIndicesOff1[zIndex] = IdxToCompare;
						zIndex++;
						palettePointer[Index].simIndicesOff1[zIndex] = -1; 
					}

					for( zCompare=1; zCompare < max_sim_idx-1; zCompare++)
					{
						if( palettePointer[IdxToCompare].simIndicesOff1[zCompare] == -1)
						{
							palettePointer[IdxToCompare].simIndicesOff1[zCompare] = Index;
							palettePointer[IdxToCompare].simIndicesOff1[zCompare + 1] = -1;  
							break;
						} // if(-1)
					}// for(zCompare)
				}// if (similar)
			} // for(IdxToCompare)
		} // for(Index)
	} //if( Off1 > 0)

	/* Batev: Collects similar patterns for positions E and F */
	if( Off2 > 0) /* If tolerance = 0, there are no similar patterns. It saves time */
	{
		for( Index = 0; Index <= maxVal; Index++)
		{
			for( zIndex = 1; zIndex < max_sim_idx; zIndex++)
			{
				if( palettePointer[Index].simIndicesOff2[zIndex] == -1)
				{
					break;
				}
			}
		
			for( IdxToCompare = Index+1; IdxToCompare <= maxVal; IdxToCompare++)
			{
				subR = palettePointer[Index].Red - palettePointer[IdxToCompare].Red;
				subG = palettePointer[Index].Green - palettePointer[IdxToCompare].Green;
				subB = palettePointer[Index].Blue - palettePointer[IdxToCompare].Blue;

				if( subR >= -(signed) Off2 && subR <= (signed)Off2 &&
						subG >= -(signed) Off2 && subG <= (signed)Off2 &&
						subB >= -(signed) Off2 && subB <= (signed)Off2)
				{
					if( zIndex < max_sim_idx - 1)
					{
						palettePointer[Index].simIndicesOff2[zIndex] = IdxToCompare;
						zIndex++;
						palettePointer[Index].simIndicesOff2[zIndex] = -1; 
					}	

					for( zCompare=1; zCompare < max_sim_idx-1; zCompare++)
					{
						if( palettePointer[IdxToCompare].simIndicesOff2[zCompare] == -1)
						{
							palettePointer[IdxToCompare].simIndicesOff2[zCompare] = Index;
							palettePointer[IdxToCompare].simIndicesOff2[zCompare + 1] = -1;  
							break;
						} // if(-1)
					}// for(zCompare)
				}// if (similar)
			} // for(IdxToCompare)
		} // for(Index)
	} //if( Off2 > 0)
} // function






				  