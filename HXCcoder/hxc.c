/*****************************************************
* File..: hxc.c
* Desc..: implementation of H(X|C) coder
* Author: T. Strutz
* Date..: 02.09.2013
* changes:
* 08.09.2013 new: max_similar_pattern
* 04.02.2012 allocate several pattern lists
 * 13.03.2014 new HXC_MAX_MAXVAL
 * 04.11.2015 bugfix memory leak
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
void HXC_coding( PicData *pd,		/* structure with image data	*/
								BitBuffer *bitbuf, /* interface for file input and output*/
								AutoConfig *aC,  /* parameter of codec */
								int encoder_flag,	/* 1 ==> encoder; 0=> decoder	*/
								unsigned int comp)	/* colour component	*/
{
	long **templ;	/* template for actual pattern	*/
	long **templ2	/* template for list pattern	*/;
	unsigned int r, c; /* row an d column	*/
	unsigned int tolerance[MAX_PATTERN_SIZE];	/* tolerence in pattern comparison	*/
	unsigned int max_similar_pattern;
	unsigned long i;
	unsigned long numLists;
	unsigned long symbol;
	unsigned long K_;	/* number of symbols per component	*/
	unsigned long bytes_old; /* temporary number of written bytes	*/

	/* pointer to array of pointers to single list elements	*/
	PATTERN_LIST_ELEM **similar_elements;	
	PATTERN_LIST *pat_list; /* top of dictionary lists */

	acModell M;	/* normal AC context	*/
	acState acState_obj, *acSt;	/* state of arithmetic coder	*/

	acSt = &acState_obj;

	assert( aC->HXCincreaseFac >= 0);

	/* start with empty dictionary list, +1 because 0 has to be included */
	if (pd->maxVal_pred[comp] > HXC_MAX_MAXVAL)
	{	/* linear array, slower search for matches	*/
		numLists = (pd->maxVal_pred[comp] + 1);
		fprintf( stderr, "\n ! HXCcoder: number of colours to high (%d > %d) !", 
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
		return;
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
	
	/* allocate template for pattern (current + corresponding to pattern list	*/
	templ = imatrix( 1, MAX_PATTERN_SIZE);
	templ2 = imatrix( 1, MAX_PATTERN_SIZE);

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

	/* compute maximal required size in dependence on possible similar pattern
	 * the higher 'tolerance'	the more patterns are regareded as match, which 
	 * have to be stored in pattern list
	 */
	max_similar_pattern = 1; 
	for (c = 0; c < MAX_PATTERN_SIZE; c++)
	{
		max_similar_pattern *= tolerance[c]*2 + 1; 
	}
	/* reasonable maximum number of similar pattern, save guard	*/
	max_similar_pattern = min( max_similar_pattern, 2000); 

	/* allocate array for pointers, one more for list termination, two needed, Why??	*/
	ALLOC( similar_elements, max_similar_pattern+2, PATTERN_LIST_ELEM *);
	if (ERRCODE == MALLOC_FAILED)
	{
		return;
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
			copy_pattern( pd, r, c, templ, comp);	/* get current pattern from image	*/

			/* search list for matching patterns,
			 * collect information about count statistics,
			 * build distribution
			 */
			get_distribution( pd, r, c, templ, templ2, pat_list, similar_elements,
					M.H, K_, tolerance, max_similar_pattern,
					aC->HXCincreaseFac, comp);

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
			fprintf( stdout, "\n r=%d:  %d Bytes  ", r, numBytes - bytes_old);
#endif
		}

#ifdef _DEBUG
		if (r == pd->height-1 && 0)
		{
			PATTERN_LIST_ELEM *cur_elem;
				HIST_LIST_ELEM *hist_elem;

			for (i = 0; i < numLists; i++ )
			{
				cur_elem = pat_list[i].top.next;
				if (cur_elem != NULL) fprintf( stdout, "\n\n Liste: %d ", i);

				while( cur_elem != NULL)
				{
					unsigned int ii;
					fprintf( stdout, "\n(%3d,%3d) ", cur_elem->r, cur_elem->c);
					copy_pattern( pd, cur_elem->r, cur_elem->c, templ, comp);	/* get current pattern	*/
					{
						fprintf( stdout, "Patt:");
						for ( ii = 0; ii < MAX_PATTERN_SIZE; ii++)
						{
							fprintf( stdout, "%d,", templ[0][ii]);
						}
						fprintf( stdout, "; ");
					}
					fprintf( stdout, "\nHist elem ");

					hist_elem = cur_elem->top_hist_elem;
					while (hist_elem != NULL)
					{
						fprintf( stdout, "(%dx) ",hist_elem->count);
						fprintf( stdout, "%d, ", hist_elem->value);
						hist_elem = hist_elem->next;
					}

					cur_elem = cur_elem->next;
				}
			} /* for (i = 0;*/
			fprintf( stdout, "\n");
		} /* if (r =*/
#endif
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


	/* clear arithmetic coding	*/
	free_model( &M);
	FREE( similar_elements);
	free_imatrix( &templ);
	free_imatrix( &templ2);
	
	/* free all allocated patter list elements and the
	 * corresponding histogram elements
	 */
	for (i = 0; i < numLists; i++ )
	{
		free_pattern_list( &(pat_list[i].top));
	}
	FREE( pat_list); 
}

