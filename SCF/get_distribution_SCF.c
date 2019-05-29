/*****************************************************
* File..: get_distribution_SCF.c
* Desc..: implementation for SCF coder
* Author: T. Strutz
* Date..: 20.08.2014
* changes:
* 17.11.2015 derived from get_distribution2.c (HXC2)
*			allow matches with similarityCount ==1 to be integrated in 
*            similarity list
* 02.12.2015 modified determination of simCount, similarity deativated
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "scf.h"
#include "codec.h" /* for min(), max() */
#include "ginterface.h"

#ifdef _DEBUG
//  #define H_DEBUG
#endif


// #define VERSION_1_38f

/*--------------------------------------------------------
* compare_pattern_SCF()
* returns the number of similar positions within the pattern
* template, at least A or B must be similar
*--------------------------------------------------------*/
int compare_pattern_SCF( unsigned int numChannel, long **t_actual, long **t_plist, 
												long **tolerance, int increaseTolMode,
												unsigned int pattern_size,
												unsigned long *sad, unsigned char *pattern_flags)
{
	int simCount;
	long diff;
	unsigned int i;
	unsigned int comp;
	/* requires order of A, B, C, D, E, F positions	
	* initialisation with zero required if pattern_size < 6
	*/
	unsigned int flags[6] = { 0, 0, 0, 0, 0, 0}; 
	unsigned int flags_val[6]={32, 16, 8, 4, 2, 1}; /* requires order of A, B, C, D, E, F positions	*/
	/* actually, the 6-bit pattern is not used for any processing step, is it?	*/

	assert( MAX_PATTERN_SIZE == 6); /* operation are limitetd to 6 here	!!	*/

	simCount = 0;	/* number of similar positions	*/
	*pattern_flags = 0; /* reset all flags	*/
	*sad = 0; /* is used to check identity of patterns => SCF_directional	*/
	for ( i = 0; i < pattern_size; i++)
	{
		flags[i] = 1; /* assume match */
		for( comp = 0; comp < numChannel; comp++) /* for all channels	*/
		{
			long tol;
			diff = abs( t_actual[i][comp] - t_plist[i][comp]);
			*sad += diff;
			tol = tolerance[i][comp];
			/* increase tolerance values, if flag is set
			* this is activated when the quadtree segmentation has detected the PHOTO mode,
			* i.e., there are many colours in this region
			*/
			if (increaseTolMode == 2)
			{
				if (i > 3)			tol = min( (tol+1) << 1, tol + 10);	/* E, F */
				else if (i > 1) tol = min( (tol+1) << 1, tol + 5);	/* C, D */
			}
			else if (increaseTolMode == 1)
			{
				if (i > 3)			tol = min( (tol+1) << 1, tol + 5);	/* E, F */
				else if (i > 1) tol = min( tol << 1, tol + 2);			/* C, D */
			}
			else
			{
				/* assume that tolerances are high because of some PHOTO regions
				* in other regions, then the tol's can be reduced
				* this is helpful for images with mixed content, where high tol's are estimated 
				* because of some PHOTO regions
				*/
				if (i > 3)			tol = (tol + 1) >> 1;	/* E, F */
				else if (i > 1) tol = (tol + 1) >> 1;			/* C, D */
			}
			if ( diff > tol ) 
			{
				flags[i] = 0; /* not similar triple	*/
			}
		}
		if (flags[i])
		{
			*pattern_flags |= flags_val[i]; /* assume match, pattern_flags is not further use currently */
		}
	}

	/* final similarity measure	*/
	simCount = flags[0] + flags[1] + flags[2] + flags[3] + flags[4] + flags[5];
	if (simCount < 5)
	{
		if (!flags[0] && !flags[1]) simCount = 0; /* A = B = 0 */
		else
		{
			if (flags[4] && !flags[0]) simCount--; /* E but not A */
			if (flags[5] && !flags[1]) simCount--; /* F but not B */
		}
	}
	return simCount;	/* similar	*/
}

/*--------------------------------------------------------
* get_distribution_SCF()
*--------------------------------------------------------*/
unsigned char get_distribution_SCF( PicData *pd, unsigned int r, 
																	 unsigned int c,
																	 long **t_actual, long **t_plist, 
																	 PATTERN_LIST_SCF *pat_list, EXCHANGE_STRUC_SCF *exchange,
																	 HIST_LIST_ELEM2 *merged_stacked_histogram,
																	 long **tolerance,
																	 unsigned int pattern_size,
																	 int no_new_pattern_flag, long *bias,
																	 unsigned long *patt_arr,
																	 int increaseTolMode, unsigned int maxNumPatterns,
																	 PATTERN_REMOVE_SCF *patternRemove)
{
	int tolA, tolB;
	int found_similar_flag = 0, bias_cnt, max_idx_reached = 0;
	int wait_for_better_simCount; /* if similarity list is full with best matches	*/
	long valTol[6], feature[6], divi0, divi1=1, divi2=1;
	unsigned char pattern_flags, best_pattern_flags = 0;
	unsigned int /*max_comp,*/ idx, comp;
	unsigned int simCount;
	unsigned int sim_idx = 0;
	unsigned long patternListNo = 0;
	unsigned long sad, min_sad;
	unsigned long pos;
	unsigned long penalty, max_penalty = 0;
	PATTERN_LIST_ELEM_SCF *current_pattern, *parent_pattern /*, *best_pattern_match*/ ;

	// !! if a 6-element match cannot be found, one should look for a smaller pattern
	// in order to get some condition for X's propability
	// or use the best match

	/* save own position; this is used to check, whether there is a vertical or horizontal
	* structure with identical colours
	* and will be overwritten, if this pattern already occured at another position
	* i.e. this pattern is already in the list
	*/
	pos = c + r * pd->width;
	patt_arr[pos] = pos; 

	for( comp = 0; comp < pd->channel; comp++)
	{
		bias[comp] = 0;
	}

	/* determine sub lists within the given tolerance	
	*/
	found_similar_flag = 0; /* no similar pattern found yet	*/
	sim_idx = 0; /* start at first entry in similarity list	*/

	//exchange->bestSimilarity = 0; /* nothing found yet	*/
	exchange->bestSimCount = 0; /* nothing found yet	*/
	min_sad = 1 << 24; /* something huge	*/
	pattern_flags = 0; /* reset all flags */

	/* determine proper pattern-list number i.e. root of list 
	*  in general similar patterns should be in same list
	* the more lists are distinguished, 
	*  => the faster the search since the lists are shorter
	*  => the higher the probability to miss a similar pattern
	*  => the higher the probability that non-similar patterns are merged (??)
	*/
	//max_comp = pd->channel;
	//if (pd->channel > 3) max_comp = 3; /* ignore alpha component	*/
	//else if (pd->channel == 2) max_comp = 1;
	patternListNo = idx = 0;

	/* prepare selection of pattern list depending on values A, B	*/
	//feature[0] = t_actual[0][0]; /* A in comp = 0*/
	//feature[1] = t_actual[1][0]; /* B in comp = 0*/
	divi0 = pd->maxVal_pred[0] + 1;
	if (pd->channel > 1)
	{
		valTol[2] = t_actual[0][1]; /* A in comp = 1*/
		valTol[3] = t_actual[1][1]; /* B in comp = 1*/
		divi1 = pd->maxVal_pred[1] + 1;
		if (pd->channel > 2)
		{
			valTol[4] = t_actual[0][2]; /* A in comp = 2*/
			valTol[5] = t_actual[1][2]; /* B in comp = 2*/
			divi1 = pd->maxVal_pred[1] + 1;
			divi2 = pd->maxVal_pred[2] + 1;
		}
		else
		{
			valTol[4] = 0; /* may not > 0 in case of grey-scale images	*/
			valTol[5] = 0;
		}
	}
	else
	{
		valTol[2] = 0; /* may not > 0 in case of grey-scale images	*/
		valTol[3] = 0;
		valTol[4] = 0; /* may not > 0 in case of grey-scale images	*/
		valTol[5] = 0;
	}

	/* plus minus tolerance for A in comp=0 */
	for (tolA = -(signed)tolerance[0][0]; tolA <= (signed)tolerance[0][0]; tolA++)
	{
		valTol[0] = t_actual[0][0] + tolA; /* A in comp = 0*/
		if (valTol[0] < 0 || valTol[0] > (signed)pd->maxVal_pred[0]) 
			continue;
		/* plus minus tolerance for B in comp=0 */
		for (tolB = -(signed)tolerance[1][0]; tolB <= (signed)tolerance[1][0]; tolB++)
		{

			/* the following seems to be identical compared to the usage of max(A,B) and min(A,B) */
			valTol[1] = t_actual[1][0] + tolB; /* B in comp = 0*/
			if (valTol[1] < 0 || valTol[1] > (signed)pd->maxVal_pred[0]) 
				continue;

			/* scale values (inkl. tolerance) to features	*/
			//feature[0] = (QUANT_FEAT_0 * feature[0]) / divi;
			feature[0] = (128 * valTol[0]) / divi0;
			feature[1] = (QUANT_FEAT_1 * valTol[1]) / divi0;
			feature[2] = (QUANT_FEAT_2 * valTol[2]) / divi1;
			feature[3] = (QUANT_FEAT_3 * valTol[3]) / divi1;
			feature[4] = (QUANT_FEAT_4 * valTol[4]) / divi2;
			feature[5] = (QUANT_FEAT_5 * valTol[5]) / divi2;


			/* i + I*j + I*J*k + I*J*K*l + I*J*K*L*m + I*J*K*L*M*n = 
			* i + I*(j + J*k + J*K*l + J*K*L*m + J*K*L*M*n) =
			* i + I*(j + J*(k + K*l + K*L*m + K*L*M*n)) =
			* i + I*(j + J*(k + K*(l + L*m + L*M*n))) =
			* i + I*(j + J*(k + K*(l + L*(m + M*n)))) =
			*/ 
			patternListNo = feature[0] * QUANT_FEAT_1; /* n*M	*/
			patternListNo = (feature[1] + patternListNo) * QUANT_FEAT_2; /* L*(m + M*n)	*/
			/* K*(l + L*(m + M*n))))	*/
			patternListNo = (feature[2] + patternListNo) * QUANT_FEAT_3; 
			/* J*(k + K*(l + L*(m + M*n))) */
			patternListNo = (feature[3] + patternListNo) * QUANT_FEAT_4; 
			/* I*(j + J*(k + K*(l + L*(m + M*n)))) */
			patternListNo = (feature[4] + patternListNo) * QUANT_FEAT_5; 
			patternListNo = (feature[5] + patternListNo); 

			assert( patternListNo < 5000000);


			/* assign the first element in this pattern sub-list	*/
			current_pattern = pat_list[patternListNo].top.next;

			/* do not take account the top element of a list because the
			 * the relation between parent and current elemnt can be destroyed by
			 * create_pattern_list_element_SCF()
			 */
			if (0 && current_pattern != NULL &&  // first element may not be selected, because new element is inserted right here destroying the parent-child relation
				pat_list[patternListNo].pattern_list_size > maxNumPatterns)
			{
				/* if pattern list is not empty AND list size is too high	then activate
				* the removal of one list element
				*/
				patternRemove->patternlist_flag = 1; /* patterns must be checked	*/
				/* copy address of size variable as it must be accesses after encoding the 
				* current pixel
				*/
				patternRemove->addressListSize = &(pat_list[patternListNo].pattern_list_size);
				max_penalty = 0; /* start value for searching low performers	*/
				/* remember pointer to previous list element	*/
				patternRemove->PreviousElementAddr = &(pat_list[patternListNo].top);
				/* remember pointer to current list element	*/
				patternRemove->CurrentElementAddr = current_pattern;
			}
			else
			{ /* nothing has to be removed	*/
				patternRemove->patternlist_flag = 0;
			}
			/* scan the selected list for similar patterns until the end of the
			* list has been found
			*/
			wait_for_better_simCount = 0;
			while( current_pattern != NULL)
			{
				/* there is another pattern in this list	*/
				current_pattern->access_count++; /* pattern is accessed once more	*/
				/* get template	from pattern list */
				copy_pattern2( pd, current_pattern->r, current_pattern->c, t_plist);

#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					unsigned int i, compp;
					fprintf( stdout, "\n\t pattern list No. %d", patternListNo);
					fprintf( stdout, "\n\t found pattern: (");
					for( i = 0; i < MAX_PATTERN_SIZE; i++)
					{
						for( compp = 0; compp < pd->channel; compp++)
						{
							fprintf( stdout, "%d,", t_plist[i][compp]);
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
				simCount = compare_pattern_SCF( pd->channel, t_actual, t_plist, 
					tolerance, increaseTolMode, pattern_size, 
					&sad, &pattern_flags);

				if (min_sad > sad)
				{
					min_sad = sad;
					if (sad == 0)
					{ /*actual pattern is identical to pattern in list*/
						/* save position of this pattern, for comparison of vertical and horizontal	structurs */
						patt_arr[pos] = current_pattern->c + current_pattern->r * pd->width;
					}
				}

				if (exchange->bestSimCount < simCount) 
				{ /* remember the maximum count of similar RGB triplets	within the pattern */
					exchange->bestSimCount = simCount;
					best_pattern_flags = pattern_flags;
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t  take as best match (simCount=%d)", simCount);
					}
#endif
					/* finally, only patterns with bestSimCount will be considered
					* so we can overwrite all collected patterns with lower simCount
					* ==> reset of sim_idx
					* this will also speed up the merging of histograms however the 
					* collecting can last longer as MAX_SIMILAR_PATTERN is reached later
					*/
					sim_idx = 0; 
					wait_for_better_simCount = 0; /* better simCount has been found	*/
				}

				if (!wait_for_better_simCount && simCount > 0 && simCount >= exchange->bestSimCount) 
					/* only the good ones must be stored, because only those with 
					* bestSimCount will be considered later on in the update process;
					* if similarity list is full with bestmatch items do not enter this branch
					*/
				{
					found_similar_flag = 1;		/* similar pattern has been found */			

					/* avoid writing outside the similar_patterns arrray	*/
					if (sim_idx < MAX_SIMILAR_PATTERN) 
					{
						/* save the current pattern in similarity list */
						/* based on similar_patterns[], 
						* the counts in the stacked histogram will be updated later on
						*/
						if (max_idx_reached)
						{
							/* if array is already full, then do not overwrite items with bestmatch, 
							* since these patterns are more recent	(new patterns are included at the top of the list)
							* it has divergent influence on compression performance, however, the computation time
							* is decreased
							*/ 
							while (exchange->simCount[sim_idx] == exchange->bestSimCount)
							{
								if (sim_idx < MAX_SIMILAR_PATTERN-1) 
								{
									sim_idx++;
								}
								else
								{
									/* avoids entering this loop until a better simCount has been found	*/
									wait_for_better_simCount = 1; 
									break; /* leave this while loop, overwrite last item	*/
								}
							}
						}
						exchange->similar_patterns[sim_idx] = current_pattern; 
						/* number of YUV-triple matches	*/
						exchange->simCount[sim_idx]   = simCount; 
						sim_idx++;
						if (sim_idx == MAX_SIMILAR_PATTERN)
						{
							/* idea: if there are more candidates than space in exchange->similar_patterns[],
							* then overwrite entries from the beginning, since there is a chance that
							* there are still patterns with a higher simCount
							* (but: do not overwrite such with bestmatch)
							*/
							sim_idx = 0; /* start from the beginning, overwrite old		*/
							max_idx_reached = 1; /* remember that the array is full	*/
						}
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, 
								"\n\t\t include found pattern in similar list with size: %d", 
								simCount);
						}
#endif
					}

					if (sad == 0) 
					{
						/* stop search for similar pattern if perfect match	*/	
						tolA = tolerance[0][0]; /* stop outer for-loop	*/
						tolB = tolerance[1][0]; /* stop inner for-loop	*/
						break; /* leaves while-loop	*/
					}
				} /* if (simCount > 0 && simCount >=exchange->bestSimCount)	*/
				else
				{
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t\t not similar or similarityCount too low");
					}
#endif
				}
				parent_pattern = current_pattern;
				current_pattern = current_pattern->next;
				if (0 && current_pattern != NULL &&  // for some reason removal of elements leads to memory problems
					  pat_list[patternListNo].pattern_list_size > maxNumPatterns)
				{
					/* if pattern list is not empty AND list size is too high	then activate
					* the removal of one list element
					*/
					patternRemove->patternlist_flag = 1; /* patterns must be checked	*/
					/* copy address of size variable as it must be accesses after encoding the
					* current pixel
					*/
					patternRemove->addressListSize = &(pat_list[patternListNo].pattern_list_size);
					
					/* as one list element has to be removed, check the performance of all elements */
					penalty = current_pattern->access_count / current_pattern->usage_count;
					if (max_penalty <= penalty) // == in case of both are zero
					{
						/* remember list element with highest penalty for removal	*/
						max_penalty = penalty;
						patternRemove->PreviousElementAddr = parent_pattern;
						patternRemove->CurrentElementAddr = current_pattern;
					}
				}

			} /* while( current_pattern	*/

		} /* for tol A	*/
	} /* for tol B	*/

	if (!found_similar_flag) 
	{
#ifdef H_DEBUG
		if ( r >= FIRSTCOL && r <= LASTCOL)
		{
			fprintf( stdout, " no similar pattern found (sim = %d)", exchange->bestSimCount);
		}
#endif
		/* nothing similar found 
		* => create new element in pattern list
		* current_pattern (= parent_element->next) is not allocated yet
		*/

		/* provide pointer to top element of pattern list as the new pattern is 
		* placed at the top
		* no_new_pattern_flag is currently not used (==! 0), is it?
		*/
		if (!no_new_pattern_flag)
		{
			create_pattern_list_element_SCF( &(pat_list[patternListNo].top), r, c, pd->channel);
			pat_list[patternListNo].pattern_list_size++; /*increment number of patterns in the list*/
#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, 
					"\n\t include actual pattern in pattern list No.%d, new list size: %d", 
					patternListNo, pat_list[patternListNo].pattern_list_size);
			}
#endif

			/* include newly created list element into list of similar patterns as its 
			* histogram must be updated after the encoding of the current  X
			*/
			exchange->similar_patterns[sim_idx] = pat_list[patternListNo].top.next;
			/* has full match, must be included into update process	*/
			exchange->simCount[sim_idx]   = MAX_PATTERN_SIZE+1; 
			sim_idx++;
			/* newly created pattern-list elements must not be included in 
			* creation of merged histo	=> '+1' exception handling
			*/
#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, "\n\t include actual pattern in similar list with size: %d", 
					MAX_PATTERN_SIZE+1);
			}
#endif
		}
		else
		{
#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, "\n\t avoid creation of new pattern");
			}
#endif
		}

		exchange->similar_patterns[sim_idx] = NULL; /* terminate list	*/
	}/* if (!found_similar_flag )*/
	else /* something similar had been found !	*/
	{
		/* if the array is full, the set index to last position as the newly created list element
		* has to be included there
		*/
		if (max_idx_reached) sim_idx = MAX_SIMILAR_PATTERN -1;

		if (exchange->bestSimCount < pattern_size) 
		{	/* found similarity was not a full match	*/
			/* so, current pattern must be included into pattern and similarity list	*/
			/* create new pattern list element	*/

			/* no_new_pattern_flag is currently not used (==! 0), is it? */
			if (!no_new_pattern_flag)
			{
				create_pattern_list_element_SCF( &(pat_list[patternListNo].top), r, c, pd->channel);
				pat_list[patternListNo].pattern_list_size++;
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, 
						"\n\t include actual pattern in pattern list No.%d, new list size: %d", 
						patternListNo, pat_list[patternListNo].pattern_list_size);
				}
#endif

				assert( sim_idx <= MAX_SIMILAR_PATTERN);
				//if (sim_idx <= MAX_SIMILAR_PATTERN) should not be larger, should it?
				{
					/* include newly created list element into list of similar patterns as its 
					* histogram must be updated after the encoding of the current  X
					*/
					exchange->similar_patterns[sim_idx] = pat_list[patternListNo].top.next;
					exchange->simCount[sim_idx]   = MAX_PATTERN_SIZE+1; 
					sim_idx++; /* necessary because next element is for termination (=NULL) */
					/* has full match, must be included into update process, but newly created 
					* pattern-list elements must not be included in creation 
					* of merged histo	=> '+1'
					*/
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t include actual pattern in similar list with size: %d", 
							MAX_PATTERN_SIZE+1);
					}
#endif
				}
			}
			else
			{
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, "\n\t avoid creation of new pattern");
				}
#endif
			}
		} /* if (exchange->bestSimilarity < pattern_size) */
		else
		{
			/* if full match happend, no new pattern must included into pattern list
			* inclusion in similarity list has already been done above
			*/
			//sim_idx = sim_idx;
		}

		exchange->similar_patterns[sim_idx] = NULL; /* terminate list	*/

		bias_cnt = 0;
		/* for all collected similar patterns	*/
		for( sim_idx = 0; exchange->similar_patterns[sim_idx] != NULL; sim_idx++)
		{
#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, "\n\t similar list is not empty!");
			}
#endif
			/* newly created pattern-list elements (size = MAX_PATTERN_SIZE+1) must not be 
			* included in creation of merged histo
			* as the decoder does not know these elements yet,
			* take only matches with highest similarity
			* !! same comparison must be in update_pattern_list() !!
			*/
			if (exchange->simCount[sim_idx] == exchange->bestSimCount) /* okay	*/
			{
				/* take only patterns with bestSimilarity/bestSimCount, ignore other	*/
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, "\n\t\t take pattern with bestSimilarity (%d)", exchange->bestSimCount);
					fprintf( stdout, "\n\t\t and use it for  merged_stacked_histogram");
				}
#endif
				current_pattern =  exchange->similar_patterns[sim_idx];
				merge_stacked_histogram( current_pattern->top_hist_elem, 
					merged_stacked_histogram, &(exchange->K_), pd->channel);

				bias_cnt++;
				for( comp = 0; comp < pd->channel; comp++)
				{
					bias[comp] += current_pattern->bias[comp];
				}
			}
		} /* for( sim_idx = 0;	*/
		assert(bias_cnt);
		for( comp = 0; comp < pd->channel; comp++)
		{
			bias[comp] /= bias_cnt; /* average	*/
		}

	} /* something similar had been found !	*/


	return best_pattern_flags;
}

