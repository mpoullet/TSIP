/*****************************************************
* File..: combine_histogram.c
* Desc..: combines pattern histogram with predictive estimates
* Author: T. Strutz
* Date..: 14.10.2014
* changes:
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "codec.h"
#include "hxc.h"

/*--------------------------------------------------------
 * combine_histogram()
 *--------------------------------------------------------*/
unsigned int combine_histogram( HIST_LIST_ELEM2 *merged_stacked_histogram, 
											 unsigned int numOfElements,
											 long *A, long *B, long *C, long *Xhat,
											 unsigned int *maxVal, int prediction_hit)
{
	int match_flag;
	long devX[4];
	unsigned int comp, num_add = 0, maxC;
	unsigned long total_count, max_count;
	HIST_LIST_ELEM2 *merged_hist_element, *parent_element=NULL;

	/* apply MED predictor	*/
	for ( comp = 0; comp < numOfElements; comp++)
	{
		if (C[comp] <= A[comp])
		{
			if (C[comp] <= B[comp])
			{
				if (A[comp] > B[comp])
				{
					Xhat[comp] = A[comp]; /* C <= B < A	*/
					devX[comp] = abs( A[comp] - C[comp]) >> 2;
				}
				else
				{
					Xhat[comp] = B[comp]; /* C <= A <= B	*/
					devX[comp] = abs( B[comp] - C[comp]) >> 2;
				}
			}
			else
			{
				/* B < C <= A	*/
				Xhat[comp] = A[comp] + B[comp] - C[comp];
				devX[comp] = abs( A[comp] - B[comp]) >> 2;
			}
		}
		else
		{
			if (C[comp] >= B[comp])
			{
				if (A[comp] < B[comp])
				{
					Xhat[comp] = A[comp]; /* A < B <= C	*/
					devX[comp] = abs( A[comp] - C[comp]) >> 2;
				}
				else
				{
					Xhat[comp] = B[comp]; /* B <= A <= C	*/
					devX[comp] = abs( B[comp] - C[comp]) >> 2;
				}
			}
			else
			{
				/* A < C < B	*/
				Xhat[comp] = A[comp] + B[comp] - C[comp];
				devX[comp] = abs(A[comp] - B[comp]) >> 2;
			}
		}
	}

	/* sum up all frequencies	*/
	max_count = 0;
	total_count = 0;
	merged_hist_element = merged_stacked_histogram;

	while( merged_hist_element != NULL)
	{
		total_count += merged_hist_element->count;
		if (max_count < merged_hist_element->count) max_count = merged_hist_element->count;
		/* go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/


	/* now include estimate +- stdDev in histogram	*/
	/* */ 
	maxC = 3;
	for ( comp = 0; comp < numOfElements; comp++)
	{ /* limitation of the deviation avoiding too many variants	*/
		devX[comp] = min( 5, devX[comp]);
		maxC += devX[comp];
	}
	{
		long i0, i1, i2;
		long Xest[4];
		for ( i0 = -devX[0]; i0 <= devX[0]; i0++)
		{
			Xest[0] = min( max( 0, Xhat[0] + i0), (signed)maxVal[0]);
			for ( i1 = -devX[1]; i1 <= devX[1]; i1++)
			{
				Xest[1] = min( max( 0, Xhat[1] + i1), (signed)maxVal[1]);
				for ( i2 = -devX[2]; i2 <= devX[2]; i2++)
				{
					Xest[2] = min( max( 0, Xhat[2] + i2), (signed)maxVal[2]);

						merged_hist_element = merged_stacked_histogram;

						match_flag = 0;
						do
						{
							/* compare it to the current item	if match not found yet */
							if (match_flag == 0)
							{
								match_flag = 1;
								for ( comp = 0; comp < numOfElements; comp++)
								{
									if (Xest[comp] != merged_hist_element->value[comp])
									{
										match_flag = 0;
										break; /* current item is not identical to selectd item from merged list*/
									}
								}
							}
							else
							{
								break; /* leave do-loop	*/
							}
							/* else: go to the next item of merged histogram	*/
							parent_element = merged_hist_element; /*remember for allocation of next element*/
							merged_hist_element = merged_hist_element->next;
						}while( merged_hist_element != NULL);


						if (!match_flag)
						{
							int count;

							count = maxC - abs( i0) - abs( i1) - abs( i2);
							count = (count * max_count + (maxC>>1)) / maxC;
							if (prediction_hit > 0)
							{
								count = count *  prediction_hit;
							}
							else if (prediction_hit < 0)
							{
								count = count / prediction_hit;
							}
							count = max( 1, count); /* at least count = 1	*/
							/* the current item is not yet in the merged histogram
							 * so create a new item
							 */
							parent_element->next = create_histogram_list_element2( 
								Xest, numOfElements, count);
							num_add++;
						}
				} /* for i2	*/
			} /* for i1	*/
		} /* for i0	*/
	}

	return num_add; /* number of added items */
}
