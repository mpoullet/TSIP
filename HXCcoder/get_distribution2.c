/*****************************************************
* File..: get_distribution2.c
* Desc..: implementation for H(X|C)2 coder
* Author: T. Strutz
* Date..: 20.08.2014
* changes:
* 29.08.2014 double for-loop tolA, tolB
* 03.09.2014 first test sim_idx < max_similar_pattern, then assignement
* 04.02.2015 adressing_mode always used
* 02.09.2015 Bugfix in adressing tolerance[0][1] vs  tolerance[1][0]
* 21.10.2015 remove *best_pattern_match
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc.h"
#include "ginterface.h"

#ifdef _DEBUG
//  #define H_DEBUG
#endif

#ifndef OLD_HXC2
/*--------------------------------------------------------
* compare_pattern2()
* returns the number of similar positions within the pattern
* template, at least A or B must be similar
*--------------------------------------------------------*/
int compare_pattern2( unsigned int numChannel, long **t_actual, long **t_plist, 
										long **tolerance, unsigned int pattern_size,
										unsigned long *sad)
{
	int flag = 1, simCount;
	long diff;
	unsigned int i;
	unsigned int comp;

	*sad = 0; /* reset sum of absolute differences	*/
	simCount = 0; /* reset count of similar triplets	*/
	flag = 1; /* assume similarRGB triple	*/
	for ( i = 0; i < pattern_size; i++)
	{
		for( comp = 0; comp < numChannel; comp++) /* for all channels	*/
		{
			diff = abs( t_actual[i][comp] - t_plist[i][comp]);
			*sad += diff; 
			/* weighting of A/B, C/D, E/F	*/
			/* good for 5colors but not for ACTIN or WOBIB	*/
			//*sad += diff * (1 + (i>>1) ); 
			if ( diff > tolerance[i][comp] ) 
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
* get_distribution2()
*--------------------------------------------------------*/
void get_distribution2( PicData *pd, unsigned int r, unsigned int c,
													long **t_actual, long **t_plist, 
													PATTERN_LIST2 *pat_list, EXCHANGE_STRUC *exchange,
													HIST_LIST_ELEM2 *merged_stacked_histogram,
													long **tolerance,
													unsigned int max_similar_pattern,
													//unsigned int increaseFac,
													unsigned int adressing_mode,
													unsigned int pattern_size,
													int no_new_pattern_flag)
{
	int tolA, tolB, listNo = 0;
	int found_similar_flag = 0;
	int listNo1, listNo2;
	unsigned int similarityCount;
	unsigned int sim_idx = 0;
	unsigned long sad, min_sad;
	PATTERN_LIST_ELEM2 *current_pattern /*, *best_pattern_match*/ /*, *parent_element*/ ;


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
		/* obsolete best_pattern_match = NULL; /* look for the best match	*/
		for (tolA = -(signed)tolerance[0][0]; tolA <= (signed)tolerance[0][0]; tolA++)
		{
		 for (tolB = -(signed)tolerance[1][0]; tolB <= (signed)tolerance[1][0]; tolB++)
		 {
			/* at first, check whether vector or matrix list addressing is used	*/
			if (!adressing_mode)
			{
				/* vector addressing !	*/
				/* compare with position A of channel 0	*/
				listNo = tolA + t_actual[0][0];
				tolB = tolerance[1][0]+999; /* stop inner loop	 as B is not of importance*/
				if (listNo < 0 || listNo > (signed)pd->maxVal_pred[0]) continue;
			}
			else
			{	/* matrix addressing !	*/
				/* compare with position A	*/
				listNo1 = tolA + t_actual[0][0];
				if (listNo1 < 0 || listNo1 > (signed)pd->maxVal_pred[0]) continue;

				/* compare with position B	*/
				listNo2 = tolB + t_actual[1][0];
				if (listNo2 < 0 || listNo2 > (signed)pd->maxVal_pred[0]) 
					continue;

				/* compute index (list number) of linear array	*/
				listNo = listNo1 + listNo2 * (pd->maxVal_pred[0]+1);
			}
			/* assign the first element in this pattern sub-list	*/
			current_pattern = pat_list[listNo].top.next;

			/* scan the selected list for similar patterns until the end of the
			 * list has been found
			 */
			while( current_pattern != NULL)
			{
				/* there is another pattern in this list	*/

				/* get template	from pattern list */
				copy_pattern2( pd, current_pattern->r, current_pattern->c, t_plist);

#ifdef H_DEBUG
	{
		unsigned int i, comp;
			fprintf( stdout, "\n\t pattern list No. %d", listNo);
			fprintf( stdout, "\n\t found pattern: (");
			for( i = 0; i < MAX_PATTERN_SIZE; i++)
			{
				for( comp = 0; comp < pd->channel; comp++)
				{
					fprintf( stdout, "%d,", t_plist[i][comp]);
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
				similarityCount = compare_pattern2( pd->channel, t_actual, t_plist, 
																							tolerance, pattern_size, &sad);
				if (min_sad > sad) 
				{ /* remember the pattern with highest similarity	*/
					min_sad = sad;
					//obsolete best_pattern_match = current_pattern;
#ifdef H_DEBUG
				fprintf( stdout, "\n\t  take as best match (similarityCount=%d)", similarityCount);
#endif
				}
				if (exchange->bestSimilarity < similarityCount) 
				{ /* remember the maximum count of similat RGB triplets	within the pattern */
					exchange->bestSimilarity = similarityCount;
				}

				if (similarityCount > 1 /* at least A and B must be similar	*/
					 && similarityCount >=exchange->bestSimilarity) 
					 /* only the good ones must be stored, because only those with 
					  * bestSimilarity will be considered
						*/
				{
					found_similar_flag = 1;		/* similar pattern has been found */					
					assert( sim_idx <= max_similar_pattern);

					/* avoid writing outside the similar_patterns arrray	*/
					if (sim_idx < max_similar_pattern) 
					{
						/* abspeichern im Histogramm */
						/* based on similar_patterns[], 
						 * the counts in the stacked histogram will be updated
						 */
						// good for list_folders
						exchange->similar_patterns[sim_idx] = current_pattern; 
						/* number of YUV-triple matches	*/
						exchange->similar_size[sim_idx++] = similarityCount; 
#ifdef H_DEBUG
					fprintf( stdout, 
						"\n\t\t include found pattern in similar list with size: %d", 
						similarityCount);
#endif
					}
					else
					{
						/* stop search for similar pattern, 
						* if a number of 2000 is reached (see value in hxc2.c)
					 */	
						break; 
					}
				} /* if similarity	*/
				else
				{
#ifdef H_DEBUG
					fprintf( stdout, "\n\t\t not similar or similarityCount too low");
#endif
				}
				current_pattern = current_pattern->next;
			} /* while( current_pattern	*/
		 } /* for (tolB =	*/
		} /* for (tolA =	*/

	if (!found_similar_flag)
	{
#ifdef H_DEBUG
	fprintf( stdout, " no similar pattern found (sim = %d)", exchange->bestSimilarity);
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
			listNo = t_actual[0][0];
		}
		else
		{
			assert( t_actual[0][0] >= 0); assert( t_actual[1][0] >= 0);
			listNo = t_actual[0][0] + t_actual[1][0] * (pd->maxVal_pred[0]+1);
		}

		/* provide pointer to top element of pattern list as the new pattern is 
		 * placed at the top
		 */
		if (!no_new_pattern_flag)
		{
			create_pattern_list_element2( &(pat_list[listNo].top), r, c, pd->channel);
			pat_list[listNo].pattern_list_size++; /*increment number of patterns in the list*/
#ifdef H_DEBUG
		fprintf( stdout, 
			"\n\t include actual pattern in pattern list No.%d, new list size: %d", 
			listNo, pat_list[listNo].pattern_list_size);
#endif
		
			/* include newly created list element into list of similar patterns as its 
			 * histogram must be updated after the encoding of the current  X
			 */
			exchange->similar_patterns[sim_idx] = pat_list[listNo].top.next;
			/* has full match, must be included into update process	*/
			exchange->similar_size[sim_idx++] = MAX_PATTERN_SIZE+1; 
			/* newly created pattern-list elements must not be included in 
			 * creation of merged histo	=> '+1' exception handling
			 */
#ifdef H_DEBUG
			fprintf( stdout, "\n\t include actual pattern in similar list with size: %d", 
				MAX_PATTERN_SIZE+1);
#endif
		}
		else
		{
#ifdef H_DEBUG
			fprintf( stdout, "\n\t avoid creation of new pattern");
#endif
		}
	
		exchange->similar_patterns[sim_idx] = NULL; /* terminate list	*/
	}/* if (!found_similar_flag )*/
	else /* something similar had been found !	*/
	{
		if (exchange->bestSimilarity < pattern_size) 
		{	/* found similarity was not a full match	*/
			/* current pattern must be included into pattern list	*/
			/* create new pattern list element	*/

			/* compare with position A and B	*/
			/* compute index (list number) of linear array	*/
			/* at first, check whether vector or matrix list addressing is used	*/
			if (!adressing_mode)
			{
				listNo = t_actual[0][0];
			}
			else
			{
				listNo = t_actual[0][0] + t_actual[1][0] * (pd->maxVal_pred[0]+1);
			}
			if (!no_new_pattern_flag)
			{
				create_pattern_list_element2( &(pat_list[listNo].top), r, c, pd->channel);
				pat_list[listNo].pattern_list_size++;
	#ifdef H_DEBUG
				fprintf( stdout, 
					"\n\t include actual pattern in pattern list No.%d, new list size: %d", 
					listNo, pat_list[listNo].pattern_list_size);
	#endif

				if (sim_idx <= max_similar_pattern)
				{
					/* include newly created list element into list of similar patterns as its 
					 * histogram must be updated after the encoding of the current  X
					 */
					exchange->similar_patterns[sim_idx] = pat_list[listNo].top.next;
					exchange->similar_size[sim_idx++] = MAX_PATTERN_SIZE+1; 
					/* has full match, must be included into update process, but newly created 
					 * pattern-list elements must not be included in creation 
					 * of merged histo	=> '+1'
					 */
	#ifdef H_DEBUG
				fprintf( stdout, "\n\t include actual pattern in similar list with size: %d", 
					MAX_PATTERN_SIZE+1);
	#endif
				}
			}
			else
			{
	#ifdef H_DEBUG
				fprintf( stdout, "\n\t avoid creation of new pattern");
	#endif
			}
		} /* if (exchange->bestSimilarity < MAX_PATTERN_SIZE) */
		else
		{
			sim_idx = sim_idx;
		}

		exchange->similar_patterns[sim_idx] = NULL; /* terminate list	*/

		/* for all collected similar patterns	*/
		for( sim_idx = 0; exchange->similar_patterns[sim_idx] != NULL; sim_idx++)
		{
#ifdef H_DEBUG
	fprintf( stdout, "\n\t similar list is not empty!");
#endif
			/* newly created pattern-list elements (size = MAX_PATTERN_SIZE+1) must not be 
			 * included in creation of merged histo
			 * as the decoder does not know these elements yet,
			 * take only matches with highest similarity
			 */
			if (exchange->similar_size[sim_idx] == exchange->bestSimilarity) /* okay	*/
//	if (exchange->similar_size[sim_idx] == PATTERN_SIZE)   //good for 5colors, mostly worse
//	if (exchange->similar_size[sim_idx] >= exchange->bestSimilarity -1) good for 5colors, stadtplan-museum,  worse for other
			{
#ifdef H_DEBUG
	fprintf( stdout, "\n\t\t take pattern with bestSimilarity (%d)", exchange->bestSimilarity);
	fprintf( stdout, "\n\t\t and use it for  merged_stacked_histogram");
#endif
				current_pattern =  exchange->similar_patterns[sim_idx];
				merge_stacked_histogram( current_pattern->top_hist_elem, 
								merged_stacked_histogram, &(exchange->K_), pd->channel); 
			}
		} /* for( sim_idx = 0;	*/
	} /* something similar had been found !	*/
}

#endif