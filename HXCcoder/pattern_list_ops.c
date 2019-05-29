/*****************************************************
* File..: pattern_list_ops.c
* Desc..: implementation of H(X|C) coder
* Desc..: operations for manipulation of pattern list
* Author: T. Strutz
* Date..: 04.09.2013
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"
#include "errmsg.h"	/* for imatrix()	*/

/*--------------------------------------------------------
 * create_pattern_list_element()
 * put new element in front
 *--------------------------------------------------------*/
void	create_pattern_list_element( 
				PATTERN_LIST_ELEM *pat_list_top, 
				unsigned int r, 
				unsigned int c,
				unsigned int increment_value)
{
	PATTERN_LIST_ELEM *new_elem;

	/* old: top first  second
	 * new: top new_el first second	
	 */
	ALLOC( new_elem, 1, PATTERN_LIST_ELEM);

	new_elem->r = r;
	new_elem->c = c;

	/* would be better if patterns are sorted according to the value at A position
	 * this can speed up the search if the comparison interrupts if A is too high or low
	 */
	new_elem->next = pat_list_top->next; /* put in front of previously first element*/

	pat_list_top->next = new_elem; /* use as first element */
	new_elem->prev = pat_list_top;
	new_elem->top_hist_elem = NULL;	/* empty histogram list	*/
	new_elem->increment_value = increment_value;
}

/*--------------------------------------------------------
 * update_pattern_list()
 *--------------------------------------------------------*/
void update_pattern_list( PATTERN_LIST_ELEM **similar_elements,
													long X)
{
	int equal_flag;
	unsigned int sim_idx;
	HIST_LIST_ELEM *current_hist_element, *parent_element=NULL;
	PATTERN_LIST_ELEM *pattern_elem;

	/* for all elements in similarity list	*/
	for( sim_idx = 0; similar_elements[sim_idx] != NULL; sim_idx++)
	{
		pattern_elem = similar_elements[sim_idx];

		/* pointer to top of histogramm list	*/
		current_hist_element = pattern_elem->top_hist_elem;
		if (current_hist_element == NULL)
		{
			/* there is no histogram yet	*/
			pattern_elem->top_hist_elem =
				create_histogram_list_element( X, pattern_elem->increment_value);
			if (pattern_elem->increment_value > 1)
						pattern_elem->increment_value--;
		}
		else
		{	
			/* search histogram for X value
			 * if X is not yet in histogram, then add list element
			 */
			do 
			{
				/* there is another element in histogram list
				 * check, whether it is identical to current value
				 */
				equal_flag = 1;
				{
					if (X != current_hist_element->value)
					{
						equal_flag = 0; 
					}
				}
				if (equal_flag)
				{
					/* increment  count of this value	 and abort search */
					current_hist_element->count += pattern_elem->increment_value;
					if (pattern_elem->increment_value > 1)
						pattern_elem->increment_value--;
					break;
				}
				parent_element = current_hist_element; 
				current_hist_element = current_hist_element->next; 
			} while(current_hist_element != NULL); 

			if (!equal_flag)
			{
				/* X is not yet in histogram list
				* create new element
				*/
				parent_element->next =
					create_histogram_list_element( X, pattern_elem->increment_value);
			}
		}
	}
}

/*---------------------------------------------------------------
 *	free_pattern_list()
 *--------------------------------------------------------------*/
void free_pattern_list( PATTERN_LIST_ELEM *pat_list_top)
{
	PATTERN_LIST_ELEM *pat_elem, *tmp_p;
	HIST_LIST_ELEM *hist_elem;

	pat_elem = pat_list_top->next;
	while (pat_elem != NULL)
	{
		hist_elem = pat_elem->top_hist_elem;
		while (hist_elem != NULL)
		{
			HIST_LIST_ELEM *tmp;

			tmp = hist_elem->next;
			free( hist_elem);
			hist_elem = tmp;
		}
		
		tmp_p = pat_elem->next;
		free( pat_elem);
		pat_elem = tmp_p;
	}
}

/*---------------------------------------------------------------
 *	imatrix()
 *	create a array with subscript range v[0..N-1][0..M-1]
 *--------------------------------------------------------------*/
long **imatrix( long N, long M)
{
	int err = 0;
	long i;
	long **m;

	/* allocate pointers to rows */
	m = (long **)malloc( N * sizeof(long*));
	if (m == NULL)
	{
		err = errmsg( ERR_ALLOCATE, "imatrix", " ", 0);
		exit( err);
	}

	/* allocate all rows  */
	m[0] = (long*)calloc( M * N, sizeof(long));
	if (m[0] == NULL)
	{
		err = errmsg( ERR_ALLOCATE, "imatrix", " ", 0);
		exit( err);
	}

	/* set pointers to rows */
	for (i = 1; i < N; i++)
	{
		m[i] = m[i - 1] + M;
	}

	/* return pointer to array of pointers to rows */
	return m;
}

/*---------------------------------------------------------------
 * free a matrix allocated by imatrix()
 *--------------------------------------------------------------*/
void free_imatrix( long **m[])
{

	if (*m != NULL)
	{
		if (*m[0] != NULL)
			free( *m[0]);
		free( *m);
	}
	*m = NULL;
}
