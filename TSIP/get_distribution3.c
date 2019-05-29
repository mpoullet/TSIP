/*****************************************************
* File..: get_distribution3.c
* Desc..: implementation for H(X|C)3 coder
* Author: T. Strutz
* Date..: 02.09.2015
* changes:
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc3.h"
#include "ginterface.h"

#ifdef _DEBUG
//#define H_DEBUG
#endif

/*--------------------------------------------------------
* compare_pattern3()
* returns the number of similar positions within the pattern
* template, at least A or B must be similar
*--------------------------------------------------------*/
int compare_pattern3( long *t_actual, long *t_plist, 
										long *tolerance, unsigned long *sad)
{
	int flag = 1, simCount;
	long diff;
	unsigned int i;

	*sad = 0; /* reset sum of absolute differences	*/
	simCount = 0; /* reset count of similar triplets	*/
	flag = 1; /* assume similarRGB triple	*/
	for ( i = 0; i < PATTERN_SIZE3; i++)
	{
		{
			diff = abs( t_actual[i] - t_plist[i]);
			*sad += diff; 
			/* weighting of A/B, C/D, E/F	*/
			/* good for 5colors but not for ACTIN or WOBIB	*/
			//*sad += diff * (1 + (i>>1) ); 
			if ( diff > tolerance[i] ) 
			{
				flag = 0; /* not similar triple	*/
			}
		}
		if (flag)
		{ /* RGB triples are similar	*/
			if ( simCount)
				simCount++; /* increment as long all positions in order C,D,E,F are similar*/
			else if (i < 2) 
				simCount++; /* set to one only if at position A or B	*/
		}
	}
	return simCount;	/* similar	*/
}

/*--------------------------------------------------------
* get_distribution3()
*--------------------------------------------------------*/
void get_distribution3( unsigned int *lptr, unsigned long pos, long maxVal,
													long *t_actual, long *t_plist, 
													PATTERN_LIST3 *pat_list, EXCHANGE_STRUC3 *exchange,
													HIST_LIST_ELEM3 *merged_stacked_histogram,
													long *tolerance,
													unsigned int max_similar_pattern,
													unsigned int increaseFac,
													unsigned int adressing_mode)
{
	int tolA, tolB, listNo = 0;
	int found_similar_flag = 0;
	int listNo1, listNo2;
	unsigned int similarityCount;
	unsigned int sim_idx = 0;
	unsigned long sad, min_sad;
	PATTERN_LIST_ELEM3 *current_pattern, *best_pattern_match /*, *parent_element*/ ;


// !! if a 6-element match cannot be found, one should look for a smaller pattern
	// in order to get some condition for X's propability
	// or use the best match

		/* determine sub lists within the given tolerance	
		 */

		found_similar_flag = 0; /* no similar pattern found yet	*/
		sim_idx = 0; /* start at first entry in similarity list	*/

		/* start at top of pattern list	with respect to value A in channel 0*/
		exchange->bestSimilarity = 0; 
		min_sad = 1 << 24; /* something huge	*/
		best_pattern_match = NULL; /* look for the best match	*/
		for (tolA = -(signed)tolerance[0]; tolA <= (signed)tolerance[0]; tolA++)
		{
		 for (tolB = -(signed)tolerance[1]; tolB <= (signed)tolerance[1]; tolB++)
		 {
			/* at first, check whether vector or matrix list addressing is used	*/
			if (!adressing_mode)
			{
				/* vector addressing !	*/
				/* compare with position A of channel 0	*/
				listNo = tolA + t_actual[0];
				tolB = tolerance[1]+999; /* stop inner loop	 as B is not of importance*/
				if (listNo < 0 || listNo > (signed)maxVal) continue;
			}
			else
			{	/* matrix addressing !	*/
				/* compare with position A	*/
				listNo1 = tolA + t_actual[0];
				if (listNo1 < 0 || listNo1 > maxVal) continue;

				/* compare with position B	*/
				listNo2 = tolB + t_actual[1];
				if (listNo2 < 0 || listNo2 > maxVal) 
					continue;

				/* compute index (list number) of linear array	*/
				listNo = listNo1 + listNo2 * (maxVal+1);
			}
			/* assign the first element in this pattern sub-list	*/
			current_pattern = pat_list[listNo].top.next;


#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t inspect pattern list");
				fprintf( stdout, " No. %d", listNo);
			}
#endif

			/* scan the selected list for similar patterns until the end of the
			 * list has been found
			 */
			while( current_pattern != NULL)
			{
				/* there is another pattern in this list	*/

				/* get template	from pattern list */
				copy_pattern3( lptr, current_pattern->pos, t_plist);

#ifdef H_DEBUG
				if ( pos < MAX_DEBUG_POS)
				{
					unsigned int i;
						fprintf( stdout, "\n\t found pattern: (");
						for( i = 0; i < PATTERN_SIZE3; i++)
						{
							{
								fprintf( stdout, "%d,", t_plist[i]);
							}
							fprintf( stdout, ";");
						}
						fprintf( stdout, ")");
						fflush( stdout);
				}
#endif

				/* compare current pattern with pattern from list
				 * get number of similar positions within the pattern template
				 */
				similarityCount = compare_pattern3( t_actual, t_plist, 
																							tolerance, &sad);
				if (min_sad > sad) 
				{ /* remember the pattern with highest similarity	*/
					min_sad = sad;
					best_pattern_match = current_pattern;
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
						fprintf( stdout, "\n\t  take as best match");
					}
#endif
				}
				if (exchange->bestSimilarity < similarityCount) 
				{ /* remember the maximum count of similat RGB triplets	wizhin the pattern */
					exchange->bestSimilarity = similarityCount;
				}

				if (similarityCount > 1 /* at least A and B must be similar	*/
					 && similarityCount >=exchange->bestSimilarity) /* only the good ones must be stored	*/
				{
					found_similar_flag = 1;		/* similar pattern has been found */

					/* integrate the corresponding stacked histogram into the global one	*/
					// integrate later
					//merge_stacked_histogram( current_pattern->top_hist_elem, 
					//				merged_stacked_histogram, K_, pd->channel, increaseFac); 
					
					assert( sim_idx <= max_similar_pattern);

					/* avoid writing outside the similar_patterns arrray	*/
					if (sim_idx < max_similar_pattern) 
					{
						/* abspeichern im Histogramm */
						/* based on similar_patterns[], the counts in the stacked histogram will be updated	*/
						exchange->similar_patterns[sim_idx] = current_pattern; // good for list_folders
						exchange->similar_size[sim_idx++] = similarityCount; /* number of triple matches	*/
#ifdef H_DEBUG
						if ( pos < MAX_DEBUG_POS)
						{
								fprintf( stdout, "\n\t\t include found pattern in similar list with size: %d", 
									similarityCount);
						}
#endif
					}
					else
					{
						/* stop search for similar pattern, 
						* if a number of 2000 is reached (see value in hxc.c)
					 */	
						break; 
					}
				} /* if similarity	*/
				else
				{
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
							fprintf( stdout, "\n\t\t not similar ");
					}
#endif
				}
				current_pattern = current_pattern->next;
			} /* while( current_pattern	*/
		 } /* for (tolB =	*/
		} /* for (tolA =	*/

	if (!found_similar_flag)
	{
		unsigned int increment_value;

#ifdef H_DEBUG
		if ( pos < MAX_DEBUG_POS)
		{
			fprintf( stdout, " no similar pattern found (sim = %d)", exchange->bestSimilarity);
		}
#endif
		/* nothing similar found 
		* => create new element in pattern list
		* current_pattern (= parent_element->next) is not allocated yet
		*/
		/* compare with position A and B	*/
		/* compute index (list number) of linear array	*/
		/* at first, check whether vector or matrix list addressing is used	*/
		if (!adressing_mode)
		{
			/* vector addressing !	*/
			listNo = t_actual[0];
		}
		else
		{
			assert( t_actual[0] >= 0); assert( t_actual[1] >= 0);
			listNo = t_actual[0] + t_actual[1] * (maxVal+1);
		}
		increment_value = 1;
		/* provide pointer to top element of pattern list as the new pattern ist placed at the top	*/
		create_pattern_list_element3( &(pat_list[listNo].top), pos, increment_value);
		pat_list[listNo].pattern_list_size++; /* increment number of patterns in the list	*/
#ifdef H_DEBUG
		if ( pos < MAX_DEBUG_POS)
		{
			fprintf( stdout, "\n\t include actual pattern in pattern list No.%d, new list size: %d", 
				listNo, pat_list[listNo].pattern_list_size);
		}
#endif
		/* if (sim_idx <= max_similar_pattern) 
		 * the check above is obsolete as only a single pattern is used when nothing similar was found
		 */
		{
			/* include newly created list element into list of similar patterns as its histogram
			 * must be updated after the encoding of the current  X
			 */
			exchange->similar_patterns[sim_idx] = pat_list[listNo].top.next;
			/* has full match, must be included into update process	*/
			exchange->similar_size[sim_idx++] = PATTERN_SIZE3 + 1; 
			/* newly created pattern-list elements must not be included in 
			 * creation of merged histo	=> '+1' exception handling
			 */
#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t include actual pattern in similar list with size: %d (exception handling)", 
					PATTERN_SIZE3+1);
			}
#endif
		}
		//else
		//{
		//	sim_idx = sim_idx;
		//}
	

		/* use single histogram of best pattern_match	*/
		if ( best_pattern_match != NULL)
		{
			/* integrate the corresponding stacked histogram into the global one	*/
			merge_stacked_histogram3( best_pattern_match->top_hist_elem, 
							merged_stacked_histogram, &(exchange->K_), increaseFac, pos /* for debugging*/); 
#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t best_pattern_match used for merged_stacked_histogram");
				fprintf( stdout, "\n\t ");
			}
#endif
		}
		else
		{
#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, ", no best pattern available");
			}
#endif
		}
		exchange->similar_patterns[sim_idx] = NULL; /* terminate list	*/
	}/* if (!found_similar_flag )*/
	else /* something similar had been found !	*/
	{
		if (exchange->bestSimilarity < PATTERN_SIZE3) /* found similarity was not a full match	*/
		{	/* current pattern must be included into pattern list	*/
			/* create new pattern list element	*/
			/* compare with position A and B	*/
			/* compute index (list number) of linear array	*/
			/* at first, check whether vector or matrix list addressing is used	*/
			if (!adressing_mode)
			{
				listNo = t_actual[0];
			}
			else
			{
				listNo = t_actual[0] + t_actual[1] * (maxVal+1);
			}
			create_pattern_list_element3( &(pat_list[listNo].top), pos, 1);
			pat_list[listNo].pattern_list_size++;
#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t include actual pattern in pattern list No.%d, new list size: %d", 
					listNo, pat_list[listNo].pattern_list_size);
			}
#endif

			if (sim_idx <= max_similar_pattern)
			{
				/* include newly created list element into list of similar patterns as its histogram
				 * must be updated after the encoding of the current  X
				 */
				exchange->similar_patterns[sim_idx] = pat_list[listNo].top.next;
				exchange->similar_size[sim_idx++] = PATTERN_SIZE3 + 1; 
				/* has full match, must be included into update process, but newly created 
				 * pattern-list elements must not be included in creation of merged histo	=> '+1'
				 */
#ifdef H_DEBUG
				if ( pos < MAX_DEBUG_POS)
				{
					fprintf( stdout, "\n\t include actual pattern in similar list with size: %d", 
						PATTERN_SIZE3+1);
				}
#endif
			}
		} /* if (exchange->bestSimilarity < MAX_PATTERN_SIZE) */
		else
		{
			sim_idx = sim_idx;
		}

		exchange->similar_patterns[sim_idx] = NULL; /* terminate list	*/
		/* integrate the histogram of the best matches	*/
		/* get best match	*/
		//max_similarity = 0;
		//for( sim_idx = 0; similar_patterns[sim_idx] != NULL; sim_idx++)
		//{
		//	if (max_similarity < similar_size[sim_idx] &&
		//			similar_size[sim_idx] <= MAX_PATTERN_SIZE) 
		//	{ /* can be larger as MAX_PATTERN_SIZE for newly generated pattern list elements */
		//		max_similarity = similar_size[sim_idx];
		//	}
		//}

		for( sim_idx = 0; exchange->similar_patterns[sim_idx] != NULL; sim_idx++)
		{
#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\t similar list is not empty!");
			}
#endif
			/* newly created pattern-list elements (size = MAX_PATTERN_SIZE+1) must not be 
			 * included in creation of merged histo
			 * as the decoder does not know these elements yet,
			 * take only matches with highest similarity
			 */
			if (exchange->similar_size[sim_idx] == exchange->bestSimilarity) /* okay	*/
//			if (exchange->similar_size[sim_idx] == MAX_PATTERN_SIZE)   //good for 5colors, mostly worse
//			if (exchange->similar_size[sim_idx] >= exchange->bestSimilarity -1) good for 5colors, stadtplan-museum,  worse for other
			{
#ifdef H_DEBUG
				if ( pos < MAX_DEBUG_POS)
				{
						fprintf( stdout, "\n\t\t take pattern with bestSimilarity (%d)", exchange->bestSimilarity);
						fprintf( stdout, "\n\t\t and use it for  merged_stacked_histogram");
				}
#endif
				current_pattern =  exchange->similar_patterns[sim_idx];
				merge_stacked_histogram3( current_pattern->top_hist_elem, 
								merged_stacked_histogram, &(exchange->K_), increaseFac, pos /* for debugging*/); 
			}
		} /* for( sim_idx = 0;	*/
	} /* something similar had been found !	*/
}

