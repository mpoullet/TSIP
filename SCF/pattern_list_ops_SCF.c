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
#include "scf.h"
#include "types.h"
//#include "errmsg.h"	/* for imatrix()	*/

#ifdef _DEBUG
//  #define H_DEBUG
#endif

/*--------------------------------------------------------
* create_pattern_list_element_SCF()
* put new element in front
*--------------------------------------------------------*/
void	create_pattern_list_element_SCF( 
																	 PATTERN_LIST_ELEM_SCF *pat_list_top, 
																	 unsigned int r, 
																	 unsigned int c, 
																	 unsigned int numOfElements)
{
	unsigned int comp;
	PATTERN_LIST_ELEM_SCF *new_elem;
	long Y[4] ={-1,-1,-1,-1};

	/* old: top first  second
	* new: top new_el first second	
	*/
	ALLOC( new_elem, 1, PATTERN_LIST_ELEM_SCF);

	new_elem->r = r;	/* store the position of the new pattern */
	new_elem->c = c;

	/* would be better if patterns are sorted according to the value at A position
	* this can speed up the search if the comparison interrupts if A is too high or low
	*/
	new_elem->next = pat_list_top->next; /* put in front of previously first element*/

	pat_list_top->next = new_elem; /* use as first element */
//	new_elem->prev = pat_list_top; /* previous element is the top of the list	*/
	/* put esc symbol	in histogram */
	new_elem->top_hist_elem = create_histogram_list_element2( Y, numOfElements, 0);	
	new_elem->hist_size = 1; /* size of the stacked histogram	*/
	new_elem->rescaling_count = 1; /* first occurence of this pattern	*/
	new_elem->access_count = 1; /* first access of this pattern	*/
	new_elem->usage_count = 1; /* first use of this pattern	*/
	for ( comp = 0; comp < numOfElements; comp++)
	{
		new_elem->bias[comp] = 0; 
	}
}




/*--------------------------------------------------------
* update_pattern_list_SCF()
* update histograms from all patters listed in exchange list
*--------------------------------------------------------*/
void update_pattern_list_SCF( EXCHANGE_STRUC_SCF *exchange,
													long *X, 
													unsigned int numOfElements,
													int X_in_merged_hist_flag, 
													unsigned int maxCountESC,
													unsigned int rescaling_threshold,
													long *Xhat_MAP
													)
{
	int match_flag;
	long perr, off[4];
	unsigned int comp;
	unsigned int sim_idx;
	HIST_LIST_ELEM2 *current_hist_element, *parent_element=NULL;
	PATTERN_LIST_ELEM_SCF *pattern_elem;

#ifdef H_DEBUG
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
	fprintf( stdout, "\n\t update histograms of pattern in similar list with bestSimilarity");
			}
#endif
	/* compute prediction error	*/
	for ( comp= 0; comp < numOfElements; comp++)
	{
		perr = X[comp] - Xhat_MAP[comp];
		if (perr > 0) off[comp] = 1;
		else if (perr < 0) off[comp] = -1;
		else off[comp] = 0;
	}

	/* for all elements in similarity list	*/
	for( sim_idx = 0; exchange->similar_patterns[sim_idx] != NULL; sim_idx++)
	{
		/* can be larger than bestSimilarity because MAX_PATTERN_SIZE+1 is used for 
		* newly generated pattern list elements
		* refers to the number of similar elements within the pattern
		* take only those, which had been used for generating the merged histogram in 
		* get_distribution_SCF.c
		* !! same comparison must be in get_distribution_SCF() !!
		*/
		if (exchange->simCount[sim_idx] >= exchange->bestSimCount)
		{
			pattern_elem = exchange->similar_patterns[sim_idx];
#ifdef H_DEBUG
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
			fprintf( stdout, "\n\t update pattern at (%d,%d)", pattern_elem->r, pattern_elem->c);
			}
#endif

			/* update bias for all patterns, which are similar to current pattern	*/
			for ( comp= 0; comp < numOfElements; comp++)
			{
				pattern_elem->bias[comp] += off[comp];
			}
			pattern_elem->usage_count++; /* = sum of all frequencies in histogram	*/
			if (pattern_elem->access_count)
				pattern_elem->access_count--; /* reduce penalty; increases the chance to stay in list	*/
			pattern_elem->rescaling_count++;

			if (pattern_elem->rescaling_count > rescaling_threshold)
			{ /* rescaling	*/
				/* scale down all counts in this histogram by factor 2 */
#ifdef H_DEBUG
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
				fprintf( stdout, "\n\t\t downscale all counts");
			}
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
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
			fprintf( stdout, "\n\t\t search actual X in histogram");
			}
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
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
					fprintf( stdout, ": X has been found, increment its count");
			}
#endif
					/* increment count of this value and abort search */
					current_hist_element->count++;

					/* check rescaling of ESC-symbol count	*/

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
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
			fprintf( stdout, "\n\t\t downscale count of ESC");
			}
#endif
						}
					}
					break; /* leave while loop, since X had been found	*/
				} /* if (match_flag */
				/* remember the parent node for allocation of next node	*/
				parent_element = current_hist_element; 
				current_hist_element = current_hist_element->next; 
			} /* while(current_hist_element != NULL); */

			if (!match_flag)
			{
#ifdef H_DEBUG
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
				fprintf( stdout, ": X has not been found, create new element in histogram");
			}
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
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
			{
				fprintf( stdout, "\n\t\t X was not in merged histogram, increment count of ESC");
			}
#endif
				/* pointer to top of histogramm list and next is esc symbol	*/
				pattern_elem->top_hist_elem->count++; /* chart2 and killersudoku compress better without this */
			} /* if (!X_in_merged_hist_flag)*/

#ifdef H_DEBUG 
			if ( r_glob >= FIRSTCOL && r_glob <= LASTCOL)
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
		} /* if (exchange->simCount[sim_idx] >= */
	} /* 	for( sim_idx = 0; similar_patterns[sim_idx] != NULL;*/
	//printf("%d ", sim_idx);
}


/*---------------------------------------------------------------
*	free_pattern_list_SCF()
*--------------------------------------------------------------*/
void free_pattern_list_SCF( PATTERN_LIST_ELEM_SCF *pat_list_top)
{
	PATTERN_LIST_ELEM_SCF *pat_elem, *tmp_p;
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
