/*****************************************************
* File..: hist_ops.c
* Desc..: implementation of H(X|C) coder
* Desc..: operations of histogram list
* Author: T. Strutz
* Date..: 04.09.2013
 * 11.05.2014 increase_value removed, 
 * 11.05.2014 new increase factor in create_cumulative_histogram()
******************************************************/
#include <stdlib.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"

/*--------------------------------------------------------
 * create_histogram_list_element()
 *--------------------------------------------------------*/
HIST_LIST_ELEM * create_histogram_list_element( 
				long X,
				unsigned int increment_value)
{
	HIST_LIST_ELEM *new_elem;

	ALLOC( new_elem, 1, HIST_LIST_ELEM);
	new_elem->value = X;
	new_elem->count = increment_value;
	new_elem->next = NULL;

	return new_elem;
}


/*--------------------------------------------------------
 * create_cumulative_histogram()
 *--------------------------------------------------------*/
void create_cumulative_histogram( unsigned long *H, 
					unsigned long K)
{
	unsigned long i, sum;

	sum = 0;
	for( i = 0; i <= K; i++)
	{
		sum += H[i];
	}
	while ( sum + K+1 >= Q1)
	{
		for( i = 0; i <= K; i++)
		{
			H[i] >>= 1;
		}
		sum >>= 1;
	}
	/* H0 remains unchanged	*/
	for( i = 1; i <= K; i++)
	{
		H[i] += H[i-1] + 1;
	}
#ifndef NIXX
	/* keep cumulative value below Q1	*/
	while (H[K] >= Q1)
	{
		unsigned long diff, next_diff;
      
		next_diff = H[1] - H[0];
    for ( i = 1; i < K; i++)
		{
			diff = next_diff;
			next_diff = H[i+1] - H[i];
			H[i] =  H[i-1] + ((diff + 1) >> 1);
    }
		H[K] =  H[K-1] + ((next_diff + 1) >> 1);
  }
#endif
}

/*--------------------------------------------------------
 * modify_distribution()
 *--------------------------------------------------------*/
void modify_distribution( HIST_LIST_ELEM *top_hist_elem, 
												 unsigned long *H)
{
	unsigned long value;
	HIST_LIST_ELEM *current_hist_element;

	current_hist_element = top_hist_elem;

	/* copy all histogram values to distribution	*/ 
	while( current_hist_element != NULL) /* list is terminated with NULL	*/
	{
		value = current_hist_element->value;
		/* increase histogram by count of this symbol	value	*/
		H[value + 1] += current_hist_element->count;
		current_hist_element = current_hist_element->next;
	}
}
