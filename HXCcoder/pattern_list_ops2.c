/*****************************************************
* File..: pattern_list_ops2.c
* Desc..: implementation of H(X|C) coder
* Desc..: operations for manipulation of pattern list
* Author: T. Strutz
* Date..: 20.08.2014
* changes
* 29.08.2014 rescaling maxCount dependent of histogram size
* 22.10.2015 restructuring of update_pattern_list2()
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"
//#include "errmsg.h"	/* for imatrix()	*/

#ifdef _DEBUG
//  #define H_DEBUG
#endif


#ifndef OLD_HXC2

/*--------------------------------------------------------
* create_pattern_list_element()
* put new element in front
*--------------------------------------------------------*/
void	create_pattern_list_element2( 
																	 PATTERN_LIST_ELEM2 *pat_list_top, 
																	 unsigned int r, 
																	 unsigned int c, 
																	 unsigned int numOfElements)
{
	PATTERN_LIST_ELEM2 *new_elem;
	long Y[4] ={-1,-1,-1,-1};

	/* old: top first  second
	* new: top new_el first second	
	*/
	ALLOC( new_elem, 1, PATTERN_LIST_ELEM2);

	new_elem->r = r;	/* store the position of the new pattern */
	new_elem->c = c;

	/* would be better if patterns are sorted according to the value at A position
	* this can speed up the search if the comparison interrupts if A is too high or low
	*/
	new_elem->next = pat_list_top->next; /* put in front of previously first element*/

	pat_list_top->next = new_elem; /* use as first element */
	new_elem->prev = pat_list_top; /* previous element is the top of the list	*/
	/* put esc symbol	in histogram */
	new_elem->top_hist_elem = create_histogram_list_element2( Y, numOfElements, 0);	
	new_elem->hist_size = 1; /* size of the stacked histogram	*/
	new_elem->rescaling_count = 1; /* first occurence of this pattern	*/
}

/*--------------------------------------------------------
* update_pattern_list2()
*--------------------------------------------------------*/
void update_pattern_list2( EXCHANGE_STRUC *exchange,
													long *X, unsigned int numOfElements,
													int X_in_merged_hist_flag, 
													unsigned int maxCountESC)
{
	int match_flag;
	unsigned int comp;
	unsigned int sim_idx;
	HIST_LIST_ELEM2 *current_hist_element, *parent_element=NULL;
	PATTERN_LIST_ELEM2 *pattern_elem;

#ifdef H_DEBUG
	fprintf( stdout, "\n\t update histograms of pattern in similar list with bestSimilarity");
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
		//if (exchange->similar_size[sim_idx] >= exchange->bestSimilarity-1) //much worse
		if (exchange->similar_size[sim_idx] >= exchange->bestSimilarity)
		{
			pattern_elem = exchange->similar_patterns[sim_idx];
#ifdef H_DEBUG
			fprintf( stdout, "\n\t update pattern at (%d,%d)", pattern_elem->r, pattern_elem->c);
#endif

			pattern_elem->rescaling_count++;
			//if (pattern_elem->rescaling_count > 6000)
			if (pattern_elem->rescaling_count > 8000)
			{ /* rescaling	*/
				/* scale down all counts in this histogram by factor 2 */
#ifdef H_DEBUG
				fprintf( stdout, "\n\t\t downscale all counts");
#endif
				pattern_elem->rescaling_count >>= 1;
				/* start at the beginning	*/
				current_hist_element = pattern_elem->top_hist_elem;
				do
				{
					current_hist_element->count >>= 1;
					current_hist_element = current_hist_element->next; 
				} while (current_hist_element != NULL);
			}

			/* pointer to top of histogramm list, but after esc symbol	*/
			parent_element = pattern_elem->top_hist_elem; 
			current_hist_element = pattern_elem->top_hist_elem->next;
			match_flag = 0;
#ifdef H_DEBUG
			fprintf( stdout, "\n\t\t search actual X in histogram");
#endif
			while (current_hist_element != NULL) 
			{
				/* search histogram for X value
				* if X is not yet in histogram, then add list element
				*/
				/* there is another element in histogram list
				* check, whether it is identical to current value
				*/
				match_flag = 1;
				for ( comp = 0; comp < numOfElements; comp++)
				{
					if (X[comp] != current_hist_element->value[comp])
					{
						match_flag = 0; 
						break; /* leave for loop	*/
					}
				}
				if (match_flag)
				{
#ifdef H_DEBUG
					fprintf( stdout, ": X has been found, increment its count");
#endif
					/* increment count of this value and abort search */
					current_hist_element->count++;

					/* check rescaling of ESC-symbol count	*/
#ifdef NIXX					
					//if (current_hist_element->count > maxCount)
					if ( downscale_flag && pattern_elem->access_count > maxCountFac * (pattern_elem->hist_size  + 1))
					{
						HIST_LIST_ELEM2 *cur_hist_element=NULL;
						/* scale down all counts in this histogram by factor 2 */
						/* should we also remove items from list if their count is equal to zero?	
						* coding of a symbol with very small count compared to the top symbol
						* might cause more output than coding it using the global palette
						* no: it slows down processing enormously and creates some extra bytes
						*/
#ifdef H_DEBUG
						fprintf( stdout, "\n\t\t downscale all counts");
#endif						/* start at the beginning	*/
						cur_hist_element = pattern_elem->top_hist_elem;
						do
						{
							cur_hist_element->count >>= 1;
							cur_hist_element = cur_hist_element->next; 
						} while (cur_hist_element != NULL);
					}
					else
					{
#endif
						if (pattern_elem->top_hist_elem->count &&
						    pattern_elem->top_hist_elem != current_hist_element) /* not ESC	*/
						{
							if (current_hist_element->count > maxCountESC) /* adaptive based on tolerances	*/
							{
								/* if current element is not the first one, then
								* scale down count of ESC 
								* good for images with few colours (rouders) or distinct patterns (chart2)
								*/
								pattern_elem->top_hist_elem->count >>= 1;
	#ifdef H_DEBUG
				fprintf( stdout, "\n\t\t downscale count of ESC");
	#endif
							}
						}
#ifdef NIXX					
					}
#endif
					break; /* leave while loop, since X had been found	*/
				} /* if (match_flag */
				/* remember the parent node for allocation of next node	*/
				parent_element = current_hist_element; 
				current_hist_element = current_hist_element->next; 
			} /* while(current_hist_element != NULL); */

			if (!match_flag)
			{
#ifdef H_DEBUG
				fprintf( stdout, ": X has not been found, create new element in histogram");
#endif
				/* X is not yet in histogram list
				* create new element
				*/
				parent_element->next =
					create_histogram_list_element2( X, numOfElements, 1);
				pattern_elem->hist_size++;
			}

			/* evaluate succes of X coding	*/
			if (!X_in_merged_hist_flag)
			{	/* it was not in the merged histogram and an ESC symbol had to be sent instead
				* increment the count of ESC in the histo of this similar pattern
				*/
#ifdef H_DEBUG
				fprintf( stdout, "\n\t\t X was not in merged histogram, increment count of ESC");
#endif
				/* pointer to top of histogramm list and next is esc symbol	*/
				pattern_elem->top_hist_elem->count++; /* chart2 and killersudoku compress better without this */
			} /* if (!X_in_merged_hist_flag)*/

#ifdef H_DEBUG 
			{
				fprintf( stdout, "\n\t\t new histogram:");
				current_hist_element = pattern_elem->top_hist_elem;
				while( current_hist_element != NULL) /* list is terminated with NULL	*/
				{
					fprintf( stdout, " %d x (", current_hist_element->count);
					for( comp = 0; comp < numOfElements; comp++)
					{
						fprintf( stdout, " %d,", current_hist_element->value[comp]);
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
*	free_pattern_list2()
*--------------------------------------------------------------*/
void free_pattern_list2( PATTERN_LIST_ELEM2 *pat_list_top)
{
	PATTERN_LIST_ELEM2 *pat_elem, *tmp_p;
	HIST_LIST_ELEM2 *hist_elem;

	pat_elem = pat_list_top->next;
	while (pat_elem != NULL)
	{
		hist_elem = pat_elem->top_hist_elem;
		while (hist_elem != NULL)
		{
			HIST_LIST_ELEM2 *tmp;

			tmp = hist_elem->next;
			free( hist_elem);
			hist_elem = tmp;
		}

		tmp_p = pat_elem->next;
		free( pat_elem);
		pat_elem = tmp_p;
	}
}

#endif
