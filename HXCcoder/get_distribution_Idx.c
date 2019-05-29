/*****************************************************
* File..: get_distribution.c
* Desc..: implementation for H(X|C) coder
* Author: T. Strutz
* Date..: 02.09.2013
* changes:
* 08.09.2013 new: max_similar_pattern (bugfix)
* 14.01.2014 PVL BICT11, Variant 2, variable pattern_size
* 31.01.2014 new Parameters for HXC in aC->
* 13.03.2014 new HXC_MAX_MAXVAL
* 03.04.2014 in get_distribution(): removal of unused 
*						parent_element
* 11.05.2014 increase_value removed, 
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc.h"
#include "ginterface.h"


/*--------------------------------------------------------
* reset_distribution_Idx()
* cumulative histrogram for flat distribution
*--------------------------------------------------------*/
void reset_distribution_Idx( unsigned long *H, unsigned long K)
{
	memset( H, 0, (K+1) * sizeof(unsigned long)); 
	//Speicher füllen mit 0 Werten bei (K+1)* 4 Bytes
}


int compare_pattern_Idx( long *templ, long *templ2, 
												unsigned int pattern_size,
												//pattern_size bestimmt die Größe der Bitmuster
												PALETTE_ELEM *palette
												)
{
	unsigned int i,z;
	int flag[6]; /* Batev: A single flag for every position in a pattern. A pattern is similar, if all pattern_size flags are 1 */
	int sub;

	for( i=0; i < pattern_size; i++) /* Batev: Pattern size can be 6,4 or 2 */
	{
		flag[i] = 0;
	} 

	for( i=0; i < pattern_size; i++)
	{
		if( i < 2) 
		{
			sub = templ[i] - templ2[i];

			if( sub == 0)
			{
				flag[i] = 1;
			}

			else /* Batev: If the values in the two patterns are not equal, the algorithm searches in the similarity list of this value */
			{
				for( z = 0; palette[(templ2[i])].simIndicesTol[z] != -1; z++)
				{
					sub = templ[i] - palette[(templ2[i])].simIndicesTol[z];

					if( sub == 0)
					{
						flag[i] = 1;
						break;
					}
				}
			}
		}

		else if( i < 4)
		{
			sub = templ[i] - templ2[i];

			if( sub == 0)
			{
				flag[i] = 1;
			}

			else
			{
				for( z = 0; palette[(templ2[i])].simIndicesOff1[z] != -1; z++)
				{
					sub = templ[i] - palette[(templ2[i])].simIndicesOff1[z];

					if( sub == 0)
					{
						flag[i] = 1;
						break;
					}
				}
			}

		}

		else if( i < pattern_size)
		{
			sub = templ[i] - templ2[i];

			if( sub == 0)
			{
				flag[i] = 1;
			}

			else
			{
				for( z = 0; palette[ templ2[i] ].simIndicesOff2[z] != -1; z++)
				{
					sub = templ[i] - palette[ templ2[i] ].simIndicesOff2[z];

					if( sub == 0)
					{
						flag[i] = 1;
						break;
					}
				}
			}
		}
	}

	for( i = 0; i < pattern_size; i++)
	{
		if( flag[i] != 1)
		{
			break;
		}

		else if( i == (pattern_size - 1) && flag[i] == 1) /* Batev: If all flags are 1 => similar pattern */ 
		{
			return 1;
		}
	}

	return 0;
} 


/*--------------------------------------------------------
* get_distribution_Idx()
*--------------------------------------------------------*/
void get_distribution_Idx( PicData *pd, unsigned int r, unsigned int c,
													long *templ, long *templ2, 
													PATTERN_LIST *pat_list, PATTERN_LIST_ELEM **similar_elements,
													unsigned long *H, unsigned long K_,
													unsigned int tolerance[],
													unsigned int max_similar_pattern,
													unsigned int HXCincreaseFac,
													unsigned int comp,
													PALETTE_ELEM *palette)
{
	int listNo = 0;
	unsigned int sim_idx = 0;
	//, similar_pattern_count;
	int flag = 0;
	unsigned long i, z;
	int found_similar_flag = 0;
	int listNo1, listNo2;
	int similarity_flag;

	// At start of the search MAX_PATTERN_SIZE = 6 positions
	int current_pattern_size = MAX_PATTERN_SIZE;

	/* set frequency counts to zero	*/
	reset_distribution_Idx( H, K_); 


	// Die Schleife für PatternSize 6,4,2 wiederholen und sofort beenden, 
	// wenn found_similar_flag > 0 
	while (current_pattern_size >= 2 && found_similar_flag == 0)
	{
		sim_idx = 0; /* start at first entry	*/

		PATTERN_LIST_ELEM *current_element /*, *parent_element*/ ;

		/* start at top of pattern list	with respect to value A*/
		//for (tol = -(signed)tolerance[0]; tol <= (signed)tolerance[0]; tol++)

		if ( pd->maxVal_pred[comp] > HXC_MAX_MAXVAL)
		{
			/* compare with position A	*/
			for( i = 0; palette[ templ[0] ].simIndicesTol[i] != -1; i++)
			{
				listNo = palette[ templ[0] ].simIndicesTol[i];
				if (listNo < 0 || listNo > (signed)pd->maxVal_pred[comp]) continue;

				// parent_element = &(pat_list[listNo].top);
				current_element = pat_list[listNo].top.next;

				while( current_element != NULL)
				{
					/* get template	from pattern list */
					copy_patternIdx( pd, current_element->r, current_element->c, templ2, comp);	

					/* compare current pattern with pattern from list	*/
					similarity_flag = compare_pattern_Idx( templ, templ2, 
						current_pattern_size, palette);
					if (similarity_flag)
					{
						found_similar_flag = 1;

						/*Pattern in der Liste gefunden, Abbruch der Schleife */
						modify_distribution( current_element->top_hist_elem, H); /* use counts to biul the distribution*/
						/* abspeichern im Histogramm */
						//similar_elements[sim_idx++] = current_element;
						if (current_pattern_size >= 4) /*Batev: 30.06.2014 : Comparition shows that without it works better */  
							similar_elements[sim_idx++] = current_element;

						assert( sim_idx <= max_similar_pattern);

						//abspeichern des Elements in die Liste similar_elements

						/* avoid writing outside the similar_elements arrray	*/
						// similar_pattern_count++;
						if (sim_idx == max_similar_pattern) 
						{
							/* stop search for similar pattern, 
							* if all possible patterns are found or
							* if a number of 2000 is reached (see value in hxc.c)
							*/	


							break; /* may it be possible that more than one pattern in the list is similar to the current one?
										 * yes! So, this break should not be here!
										 * it might be useful to collect histogram inforn´mation from different similar pattern
										 */
						}
					} /* if similarity	*/
					current_element = current_element->next;

					//parent_element = current_element;
					// Verkettung innerhalb der Liste
				} /* while( current_element	*/
				// Aktuelle Patternlänge um 2 kleiner (6 -> 4 -> 2)
			}
		} /* if ( pd->maxVal_pred[comp] > HXC_MAX_MAXVAL)	*/
		else
		{
			/* compare with position A	*/
			for( i = 0; palette[ templ[0] ].simIndicesTol[i] != -1; i++)
			{
				listNo1 = palette[ templ[0] ].simIndicesTol[i];
				if (listNo1 < 0 || listNo1 > (signed)pd->maxVal_pred[comp]) continue;

				/* compare with position B	*/
				for( z = 0; palette[ templ[1] ].simIndicesTol[z] != -1; z++)
				{
					listNo2 = palette[ templ[1] ].simIndicesTol[z];
					if (listNo2 < 0 || listNo2 > (signed)pd->maxVal_pred[comp]) continue;

					/* compute index (list number) of linear array	*/
					listNo = listNo1 + listNo2 * (pd->maxVal_pred[comp]+1);

					// parent_element = &(pat_list[listNo].top);
					current_element = pat_list[listNo].top.next;

					// found_similar_flag = 0;
					//sim_idx = 0; /* start at first entry	*/

					while( current_element != NULL)
					{
						/* get template	from pattern list */
						copy_patternIdx( pd, current_element->r, current_element->c, templ2, comp);	

						/* compare current pattern with pattern from list	*/
						similarity_flag = compare_pattern_Idx( templ, templ2, 
							current_pattern_size, palette);
						if (similarity_flag)
						{
							found_similar_flag = 1;

							/*Pattern in der Liste gefunden, Abbruch der Schleife */
							modify_distribution( current_element->top_hist_elem, H); /* use counts to biul the distribution*/
							/* abspeichern im Histogramm */
							//similar_elements[sim_idx++] = current_element;
							if (current_pattern_size >= 4) /* Batev: 30.06.2014 : Comparition shows that without it works better */ 
								similar_elements[sim_idx++] = current_element; 

							assert( sim_idx <= max_similar_pattern);

							//abspeichern des Elements in die Liste similar_elements

							/* avoid writing outside the similar_elements arrray	*/
							// similar_pattern_count++;
							if (sim_idx == max_similar_pattern) 
							{
								/* stop search for similar pattern, 
								* if all possible patterns are found or
								* if a number of 2000 is reached (see value in hxc.c)
								*/	

								flag = 1;

								break; /* may it be possible that more than one pattern in the list is similar to the current one?
											 * yes! So, this break should not be here!
											 * it might be useful to collect histogram information from different similar pattern
											 */
							}
						} /* if similarity	*/
						current_element = current_element->next;

						//parent_element = current_element;
						// Verkettung innerhalb der Liste
					} /* while( current_element	*/
					// Aktuelle Patternlänge um 2 kleiner (6 -> 4 -> 2)
					if( flag == 1) 
						break;
				}
				if( flag == 1) 
					break;
			} 
			if( flag == 1)
			{
				break;
			}
		}
		current_pattern_size -= 2;
	} /* while (current_pattern_size	*/

	if (!found_similar_flag || current_pattern_size < 4)
	{
		unsigned int increment_value;
		/* the increment value should be high for images without tolerance
		* and it should be low for more natural images
		*/
		if (tolerance[0] == 0)
		{
			increment_value =  pd->maxVal_pred[comp] * HXCincreaseFac;
			if (HXCincreaseFac == 0) increment_value =  (pd->maxVal_pred[comp]+1) >> 1;
			//if (HXCincreaseFac == 0) increment_value =  (pd->maxVal[comp]+1) >> 2;
		}
		else
		{
			increment_value =  pd->maxVal_pred[comp] * HXCincreaseFac;
			if (HXCincreaseFac == 0) increment_value =  (pd->maxVal_pred[comp]+1) >> 2;
		}

		/* nothin similar found 
		* => create new element in pattern list
		* current_element (= parent_element->next) is not allocated yet
		*/
		if (sim_idx <= max_similar_pattern)
		{
			/* compare with position A and B	*/
			/* compute index (list number) of linear array	*/
			if ( pd->maxVal_pred[comp] > HXC_MAX_MAXVAL)
			{
				listNo = templ[0];
			}
			else
			{
				listNo = templ[0] + templ[1] * (pd->maxVal_pred[comp]+1);
			}
			create_pattern_list_element( &(pat_list[listNo].top), r, c, increment_value);
			pat_list[listNo].pattern_list_size++;
			similar_elements[sim_idx++] = pat_list[listNo].top.next;
		}
	}

	similar_elements[sim_idx] = NULL;

	create_cumulative_histogram( H, K_);
}