/*****************************************************
* File..: pattern_list_ops3.c
* Desc..: implementation of H(X|C)3 coder
* Desc..: operations for manipulation of pattern list
* Author: T. Strutz
* Date..: 02.09.2015
* changes
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hxc3.h"
#include "types.h"
#include "errmsg.h"	/* for lvector()	*/

/*--------------------------------------------------------
 * copy_pattern3()  
 * D C B A X
 *--------------------------------------------------------*/
void copy_pattern3( unsigned int *lptr, unsigned long pos, long *templ)
{
	if (pos > 0)
	{
		templ[0] = lptr[pos-1];	/* A */
		if (pos > 1)
		{
			templ[1] = lptr[pos-2];	/* B */
			if (pos > 2)
			{
				templ[2] = lptr[pos-3];	/* C */
				if (pos > 3)
				{
					templ[3] = lptr[pos-4];	/* D */
				}
				else
				{
					templ[3] = templ[2];	/* D */
				}
			}
			else
			{
				templ[2] = templ[1];	/* C */
				templ[3] = templ[2];	/* D */
			}
		}
		else
		{
			templ[1] = templ[0];	/* B */
			templ[2] = templ[1];	/* C */
			templ[3] = templ[2];	/* D */
		}
	}
	else
	{
		templ[0] = 0;	/* A */
		templ[1] = 0;	/* B */
		templ[2] = 0;	/* C */
		templ[3] = 0;	/* D */
	}
}


/*--------------------------------------------------------
 * create_pattern_list_element()
 * put new element in front
 *--------------------------------------------------------*/
void	create_pattern_list_element3( 
				PATTERN_LIST_ELEM3 *pat_list_top, 
				unsigned long pos,
				unsigned int increment_value)
{
	PATTERN_LIST_ELEM3 *new_elem;

	/* old: top first  second
	 * new: top new_el first second	
	 */
	ALLOC( new_elem, 1, PATTERN_LIST_ELEM3);

	new_elem->pos = pos;	/* store the position of the new pattern */

	/* would be better if patterns are sorted according to the value at A position
	 * this can speed up the search if the comparison interrupts if A is too high or low
	 */
	new_elem->next = pat_list_top->next; /* put in front of previously first element*/

	pat_list_top->next = new_elem; /* use as first element */
	new_elem->prev = pat_list_top; /* previous element is the top of the list	*/
	new_elem->top_hist_elem = NULL;	/* empty histogram list	*/
	new_elem->increment_value = increment_value;
	new_elem->hist_size = 0; /* size of the stacked gistogram	*/
}

/*--------------------------------------------------------
 * update_pattern_list3()
 *--------------------------------------------------------*/
void update_pattern_list3( EXCHANGE_STRUC3 *exchange,
													long X,
													int included_flag, unsigned int maxCountESC,
													unsigned int maxCountFac, unsigned long pos)
{
	int equal_flag;
	unsigned int sim_idx, maxCount;
	HIST_LIST_ELEM3 *current_hist_element, *parent_element=NULL;
	PATTERN_LIST_ELEM3 *pattern_elem;

	/* get best match	*/
#ifdef H_DEBUG
	if ( pos < MAX_DEBUG_POS)
	{
		fprintf( stdout, "\n\t update histograms of pattern in similar list with bestSimilarity");
	}
#endif

	/* for all elements in similarity list	*/
	for( sim_idx = 0; exchange->similar_patterns[sim_idx] != NULL; sim_idx++)
	{
		/* can be larger than bestSimilarity because MAX_PATTERN_SIZE+1 is used for 
	   * newly generated pattern list elements
		 * refers to the number of similar elements within the pattern
		 * take only those, which had been used for generating the merged histogram in 
		 * get_distribution2.c
		 */
		if (exchange->similar_size[sim_idx] >= exchange->bestSimilarity)
//		if (exchange->similar_size[sim_idx] >= exchange->bestSimilarity-1) much worse
		{
			pattern_elem = exchange->similar_patterns[sim_idx];
#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t update pattern at pos (%d)", pattern_elem->pos);
			}
#endif
			/* set limit for histogram rescaling dependent on histogram size	*/
			maxCount = (pattern_elem->hist_size + 1) * maxCountFac; /* has to be checked ##### */

			/* pointer to top of histogramm list	*/
			current_hist_element = pattern_elem->top_hist_elem;
			if (current_hist_element == NULL)
			{
#ifdef H_DEBUG
				if ( pos < MAX_DEBUG_POS)
				{
					fprintf( stdout, "\n\t\t histogram is still empty, start with actual X, count=1");
				}
#endif
				/* there is no histogram yet	*/
				pattern_elem->top_hist_elem =
					create_histogram_list_element3( X, pattern_elem->increment_value);
				//if (pattern_elem->increment_value > 1)
				//			pattern_elem->increment_value--;
				pattern_elem->hist_size++; /* update the number of histogram items	*/
			}
			else
			{	
				/* search histogram for X value
				 * if X is not yet in histogram, then add list element
				 */
#ifdef H_DEBUG
				if ( pos < MAX_DEBUG_POS)
				{
					fprintf( stdout, "\n\t\t search actual X in histogram");
				}
#endif
				do 
				{
					/* there is another element in histogram list
					 * check, whether it is identical to current value
					 */
					equal_flag = 1;
					if (X != current_hist_element->value)
					{
						equal_flag = 0; 
					}
					if (equal_flag)
					{
#ifdef H_DEBUG
						if ( pos < MAX_DEBUG_POS)
						{
							fprintf( stdout, ": X has been found, increment its count");
						}
#endif
						/* increment count of this value and abort search */
						if (current_hist_element->count < maxCount)
						{
							//current_hist_element->count++;
							current_hist_element->count += pattern_elem->increment_value;
							//if (pattern_elem->increment_value > 1)
							//	pattern_elem->increment_value--;
							//if (current_hist_element->count > 8) // value has to be check ##########
							if (current_hist_element->count > maxCountESC) /* adaptive based on tolerances	*/
							{
								if (current_hist_element != pattern_elem->top_hist_elem)
								{ /* if current element is not the first one
									/* scale down count of ESC (if present, it is  the first element) */
									pattern_elem->top_hist_elem->count >>= 1;
								}
							}
						}
						else
						{
							//	HIST_LIST_ELEM2 *par_element=NULL;
							/* scale down all counts in this histogram by factor 2 */
							/* should we also remove items from list if their count is equal to zero?	
							 * coding of a symbol with very small count compared to the top symbol
							 * might cause more output than coding it using the global palette
							 * no: it slows down processing enormously and creates some extra bytes
							 */
							current_hist_element = pattern_elem->top_hist_elem;
							do
							{
									current_hist_element->count >>= 1;
									current_hist_element = current_hist_element->next; 
							} while (current_hist_element != NULL);
						}
						break;
					} /* if (equal_flag */
					/* remember the parent node for allocation of next node	*/
					parent_element = current_hist_element; 
					current_hist_element = current_hist_element->next; 
				} while(current_hist_element != NULL); 

				if (!equal_flag)
				{
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
						fprintf( stdout, ": X has not been found, create new element in histogram");
					}
#endif
					/* X is not yet in histogram list
					* create new element
					*/
					parent_element->next =
						create_histogram_list_element3( X, pattern_elem->increment_value);
					pattern_elem->hist_size++;
				}
			} /* if (current_hist_element != NULL)*/

			/* evaluate succes of X coding	*/
			if (!included_flag)
			{	/* it was not in the merged histogram and an ESC symbol had to be sent instead
				 * increment the count of ESC in the histo of this similar pattern
				 */
				int match_flag;
				int identical_flag;
				long Y =-1;

#ifdef H_DEBUG
				if ( pos < MAX_DEBUG_POS)
				{
					fprintf( stdout, "\n\t\t X was not in merged histogram");
				}
#endif
				/* search ESC item in histogram of this simmilar pattern	*/
				pattern_elem = exchange->similar_patterns[sim_idx];

				/* pointer to top of histogramm list	*/
				current_hist_element = pattern_elem->top_hist_elem;
				match_flag = 0;
				while( current_hist_element != NULL) /* search in entire stacked histogram	*/
				{
					/* compare single histogram item	*/
					identical_flag = 1;
					if (current_hist_element->value != Y)
					{
						identical_flag = 0;
						 /* current hist element is not identical to theh ESC item */
					}
					if (identical_flag)
					{
						match_flag = 1;
						break;
					}
					/* remember the parent node for allocation of next node	*/
					parent_element = current_hist_element; 
					current_hist_element = current_hist_element->next; 
				} /* while( current_hist_element != NULL);*/
				if (match_flag)
				{
					current_hist_element->count++; /* increment count of ESC item	*/
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
							fprintf( stdout, ": increment count of ESC symbol");
					}
#endif
				}
				else /* create ESC item	at top of histogram */
				{
					HIST_LIST_ELEM3 *temp;
					temp = pattern_elem->top_hist_elem;
					pattern_elem->top_hist_elem =
						create_histogram_list_element3( Y, 1 /* increment. first count */);
					pattern_elem->top_hist_elem->next = temp;
					pattern_elem->hist_size++;
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
							fprintf( stdout, ": create new histogram item for ESC symbol");
					}
#endif
				}
			} /* if (!included_flag)*/
#ifdef H_DEBUG 
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t\t new histogram:");
				current_hist_element = pattern_elem->top_hist_elem;
				while( current_hist_element != NULL) /* list is terminated with NULL	*/
				{
					fprintf( stdout, " %d x (", current_hist_element->count);
					{
						fprintf( stdout, " %d,", current_hist_element->value);
					}
					fprintf( stdout, ")");
					/* goto to the next histogram element	*/
					current_hist_element = current_hist_element->next;
				}
			}
#endif
		} /* if (exchange->similar_size[sim_idx] >= */
	} /* 	for( sim_idx = 0; similar_patterns[sim_idx] != NULL;*/
}

/*---------------------------------------------------------------
 *	free_pattern_list3()
 *--------------------------------------------------------------*/
void free_pattern_list3( PATTERN_LIST_ELEM3 *pat_list_top)
{
	PATTERN_LIST_ELEM3 *pat_elem, *tmp_p;
	HIST_LIST_ELEM3 *hist_elem;

	pat_elem = pat_list_top->next;
	while (pat_elem != NULL)
	{
		hist_elem = pat_elem->top_hist_elem;
		while (hist_elem != NULL)
		{
			HIST_LIST_ELEM3 *tmp;

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
 *	lvector()
 *	create a vector with subscript range v[0..N-1]
 *--------------------------------------------------------------*/
long *
lvector( long N)
{
	int err = 0;
	long *v;

	v = (long*)calloc( N, sizeof(long));
	if (v == NULL)
	{
		err = errmsg( ERR_ALLOCATE, "lvector", " ", 0);
		exit( err);
	}
	return v;
}
/*---------------------------------------------------------------
 * free a vector allocated by ivector()
 *--------------------------------------------------------------*/
void free_lvector( long *v[])
{
	if (*v != NULL)
		free( *v);
	*v = NULL;
}