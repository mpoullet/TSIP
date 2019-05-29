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
#include "hxc.h"

/*--------------------------------------------------------
 * combine_histogram()
 *--------------------------------------------------------*/
unsigned int combine_histogram( HIST_LIST_ELEM2 *merged_stacked_histogram, 
											 unsigned int numOfElements,
											 long *A, long *B, long *C, long *Xhat)
{
	int match_flag;
	unsigned int comp, num_add = 0;
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
				}
				else
				{
					Xhat[comp] = B[comp]; /* C <= A <= B	*/
				}
			}
			else
			{
				/* B < C <= A	*/
				Xhat[comp] = A[comp] + B[comp] - C[comp];
			}
		}
		else
		{
			if (C[comp] >= B[comp])
			{
				if (A[comp] < B[comp])
				{
					Xhat[comp] = A[comp]; /* A < B <= C	*/
				}
				else
				{
					Xhat[comp] = B[comp]; /* B <= A <= C	*/
				}
			}
			else
			{
				/* A < C < B	*/
				Xhat[comp] = A[comp] + B[comp] - C[comp];
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
				if (Xhat[comp] != merged_hist_element->value[comp])
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
		/* the current item is not yet in the merged histogram
		 * so create a new item
		 */
		parent_element->next = create_histogram_list_element2( 
			Xhat, numOfElements, max_count);
		num_add++;
	}


	return num_add; /* number of added items */
}
