/*****************************************************
* File..: hxc_d.c
* Desc..: implementation of H(X|C) decoder
* Author: T. Strutz
* Date..: 05.09.2013
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"


/*--------------------------------------------------------
 * decode_HXC()
 *--------------------------------------------------------*/
void decode_HXC( PicData *pd, BitBuffer *bitbuf, 
								 unsigned char indexed_flag)
{
	long **templ;	/* template for actual pattern	*/
	long **templ2	/* template for list pattern	*/;
	long symbol[3];	/* three components per pixel	*/
	unsigned int r, c; /* row an d column	*/
	unsigned long K_[3];	/* number of symbols per component	*/
	unsigned long bytes_old; /* temporary number of written bytes	*/
	unsigned int tolerance[PATTERN_SIZE];	/* tolerence in pattern comparison	*/

	/* pointer to array of pointers to single list elements	*/
	PATTERN_LIST_ELEM **similar_elements;	
	PATTERN_LIST pattern_list, *pat_list; /* top of dictionary	list */

	acModell M[NUM_OF_COMPONENTS];	/* normal AC context	*/
	acState acState_obj, *acSt;	/* state of arithmetic coder	*/

	acSt = &acState_obj;

	/* start with empty list	*/
	pat_list = &pattern_list;
	pat_list->pattern_list_size = 0;
	pat_list->top.next = NULL;
	pat_list->top.prev = NULL;
	pat_list->top.top_hist_elem = NULL; /* empty histogramm list	*/
	pat_list->top.r = 0;
	pat_list->top.c = 0;
	
	/* allocate template for pattern (current + corresponding to pattern list	*/
	templ = imatrix( NUM_OF_COMPONENTS, PATTERN_SIZE);
	templ2 = imatrix( NUM_OF_COMPONENTS, PATTERN_SIZE);

	if (indexed_flag)
	{
		/* zero tolerance	*/
		for (c = 0; c < PATTERN_SIZE; c++)
		{
			tolerance[c] = 0;
		}
	}
	else
	{
		/* zero tolerance	*/
		for (c = 0; c < PATTERN_SIZE; c++)
		{
			tolerance[c] = 2; /* should be updated somehow	*/
		}
	}

	/* compute maximal required size in dependence on possible similar pattern	*/
	r = 1; 
	for (c = 0; c < PATTERN_SIZE; c++)
	{
		r *= tolerance[c]*2 + 1; 
	}
	/* allocate array for pointers, one more for list termination	*/
	CALLOC( similar_elements, r+1, PATTERN_LIST_ELEM *);

	/* prepare arithmetic coding	*/
	start_decoding( acSt, bitbuf); /* reset internal values	*/

	/* build models or arithmetic coding	*/
	start_model( &M[0], pd->maxVal[0] +1);
	K_[0] = M[0].K;	/* copy size of cumulative histogram 	*/
	if ( pd->channel == 3)
	{
		start_model( &M[1], pd->maxVal[1] +1);
		start_model( &M[2], pd->maxVal[2] +1);
		K_[1] = M[1].K;
		K_[2] = M[2].K;
	}


	fprintf( stderr, "\n");
	/* process image	*/
	for( r = 0; r < pd->height; r++)
	{
		bytes_old = numBytes; /* check bytes per row	*/
		for( c = 0; c < pd->width; c++) 
		{
			copy_pattern( pd, r, c, templ);	/* get current pattern	*/

			/* search list for matching patterns,
			 * collect information about count statistics,
			 * build distribution
			 */
			get_distribution( pd, r, c, templ, templ2, pat_list, similar_elements,
					M[0].H, M[1].H, M[2].H, K_, tolerance);

			/* do the arithmetic decoding	*/
			symbol[0] = decode_AC( &M[0], acSt, bitbuf);

			/* store decoded signal value	*/
			pd->data[0][c + r * pd->width] = symbol[0];

			if ( pd->channel == 3)
			{
				symbol[1] = decode_AC( &M[1], acSt, bitbuf);
				symbol[2] = decode_AC( &M[2], acSt, bitbuf);
				pd->data[1][c + r * pd->width] = symbol[1];
				pd->data[2][c + r * pd->width] = symbol[2];
			}
			/* include information about true signal value in list	*/
			update_pattern_list( similar_elements, symbol, pd->channel);
			//if ( pd->channel == 3)
			//{
			//	update_pattern_list( similar_elements, symbol2, 1);
			//	update_pattern_list( similar_elements, symbol3, 2);
			//}
		} /* for c */

		fprintf( stderr, "\r%3.2f  pattern: %d", 100.*(r+1) / pd->height, pat_list->pattern_list_size);
		//fprintf( stdout, "\n%3.2f  pattern: %d", 100.*(r+1) / pd->height, pat_list->pattern_list_size);

		if (r == 15 && 0)
		{
			PATTERN_LIST_ELEM *cur_elem;
				HIST_LIST_ELEM *hist_elem;

			cur_elem = pat_list->top.next;

			while( cur_elem != 0)
			{
				unsigned int i, k;
				fprintf( stdout, "\n(%3d,%3d) ", cur_elem->c, cur_elem->r);
				copy_pattern( pd, cur_elem->r, cur_elem->c, templ);	/* get current pattern	*/
				for ( k = 0; k < pd->channel; k++)
				{
					for ( i = 0; i < PATTERN_SIZE; i++)
					{
						fprintf( stdout, "%d,", templ[k][i]);
					}
					fprintf( stdout, "; ");
				}
				fprintf( stdout, "\nHist elem ");

				hist_elem = cur_elem->top_hist_elem;
				while (hist_elem != NULL)
				{
					fprintf( stdout, "(%dx) ",hist_elem->count);
					for ( k = 0; k < pd->channel; k++)
					{
						fprintf( stdout, "%d, ", hist_elem->value[k]);
					}
					hist_elem = hist_elem->next;
				}

				cur_elem = cur_elem->next;
			}
			fprintf( stdout, "\n");
		}
	} /* for r */

	/* clear arithmetic coding	*/
	free_model( &M[0]);
	if ( pd->channel == 3)
	{
		free_model( &M[1]);
		free_model( &M[2]);
	}
	FREE( similar_elements);
	free_imatrix( &templ);
	free_imatrix( &templ2);

	/* free all allocated patter list elements and the
	 * corresponding histogram elements
	 */
	free_pattern_list( &(pat_list->top));
}

