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
 * 09.07.2014 increase_value re-implemented, 
 * 03.09.2014 first test sim_idx < max_similar_pattern, then assignement
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc.h"
#include "ginterface.h"


/*--------------------------------------------------------
* reset_distribution()
* cumulative histrogram for flat distribution
*--------------------------------------------------------*/
void reset_distribution( unsigned long *H, unsigned long K)
{
	memset( H, 0, (K+1) * sizeof(unsigned long)); 
	//Speicher füllen mit 0 Werten bei (K+1)* 4 Bytes
}


/*--------------------------------------------------------
* compare_pattern()
*--------------------------------------------------------*/
int compare_pattern( long **templ, long **templ2, 
										unsigned int tolerance[],
										unsigned int pattern_size
										//pattern_size bestimmt die Größe der Bitmuster
										)
{
	unsigned int i;
	{
		for ( i = 0; i < pattern_size; i++)
		{
			if ( (unsigned)abs( templ[0][i] - templ2[0][i]) > tolerance[i] ) 
				return 0;
		}
	}
	return 1;
	//Vergleich des Patterns mit einem Eintrag in der Liste, Ergebnis des Vergleichs > tolerance[i]-> Ergebnis = 0( nicht ähnlich), ansonsten Ergebnis =1 (ähnlich)

}

/*--------------------------------------------------------
* get_distribution()
*--------------------------------------------------------*/
void get_distribution( PicData *pd, unsigned int r, unsigned int c,
											long **templ, long **templ2, 
											PATTERN_LIST *pat_list, PATTERN_LIST_ELEM **similar_elements,
											unsigned long *H, unsigned long K_,
											unsigned int tolerance[],
											unsigned int max_similar_pattern,
											unsigned int HXCincreaseFac,
											unsigned int comp)
{
	int listNo = 0;
	unsigned int sim_idx = 0;
	//, similar_pattern_count;

	int found_similar_flag = 0;
	// Anfangslänge für die Suche auf MAX_PATTERN_SIZE = 6 setzen
	int current_pattern_size = MAX_PATTERN_SIZE;

	/* set frequency counts to zero	*/
	reset_distribution( H, K_);
	
		found_similar_flag = 0;
		sim_idx = 0; /* start at first entry	*/

	// Die Schleife für PatternSize 6,4,2 wiederholen und sofort beenden, 
	// wenn found_similar_flag > 0 
	while (current_pattern_size >= 2 && found_similar_flag == 0)
	{
		int listNo1, listNo2, tol;
		int similarity_flag;

		PATTERN_LIST_ELEM *current_element /*, *parent_element*/ ;

		/* is this correct? should not the distribution reset only once (before while)?*/
		//reset_distribution( H, K_);

			/* start at top of pattern list	with respect to value A*/
		for (tol = -(signed)tolerance[0]; tol <= (signed)tolerance[0]; tol++)
		{
			if ( pd->maxVal_pred[comp] > HXC_MAX_MAXVAL)
			{
				/* compare with position A	*/
				listNo = tol + templ[0][0];
				if (listNo < 0 || listNo > (signed)pd->maxVal_pred[comp]) continue;
			}
			else
			{
				/* compare with position A	*/
				listNo1 = tol + templ[0][0];
				if (listNo1 < 0 || listNo1 > (signed)pd->maxVal_pred[comp]) continue;

				/* compare with position B	*/
				listNo2 = tol + templ[0][1];
				if (listNo2 < 0 || listNo2 > (signed)pd->maxVal_pred[comp]) continue;

				/* compute index (list number) of linear array	*/
				listNo = listNo1 + listNo2 * (pd->maxVal_pred[comp]+1);
			}
			// parent_element = &(pat_list[listNo].top);
			current_element = pat_list[listNo].top.next;

			while( current_element != NULL)
			{
				/* get template	from pattern list */
				copy_pattern( pd, current_element->r, current_element->c, templ2, comp);	

				/* compare current pattern with pattern from list	*/
				similarity_flag = compare_pattern( templ, templ2, 
					tolerance, current_pattern_size);
				if (similarity_flag)
				{
					found_similar_flag = 1;

					/*Pattern in der Liste gefunden, Abbruch der Schleife */
					modify_distribution( current_element->top_hist_elem, H); /* use counts to biul the distribution*/
					/* abspeichern im Histogramm */

					assert( sim_idx <= max_similar_pattern);

					//abspeichern des Elements in die Liste similar_elements

					/* avoid writing outside the similar_elements arrray	*/
					// similar_pattern_count++;
					if (sim_idx < max_similar_pattern) 
					{
						/* based on similar_elements[], the counts in the stacked histogram will be updated	*/
						//similar_elements[sim_idx++] = current_element; // not so good
						if (current_pattern_size == 6) similar_elements[sim_idx++] = current_element; // good for list_folders
						//if (current_pattern_size >= 4) similar_elements[sim_idx++] = current_element; // good for chart2
					}
					else
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
		} /* for (tol =	*/
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
				listNo = templ[0][0];
			}
			else
			{
				listNo = templ[0][0] + templ[0][1] * (pd->maxVal_pred[comp]+1);
			}
			create_pattern_list_element( &(pat_list[listNo].top), r, c, increment_value);
			pat_list[listNo].pattern_list_size++;
			similar_elements[sim_idx++] = pat_list[listNo].top.next;
		}
	}

	similar_elements[sim_idx] = NULL;

	/* if tolerance > 0 then Parzen windowing of histogram */
	if (tolerance[0] && 0)
	{
		unsigned int g;
		unsigned long *H2;

		ALLOC( H2, K_+1, unsigned long);
		memcpy( H2, H, sizeof(unsigned long) * K_+1);

		if (H2[0])
		{
			H[0] += H2[0]+1;
			H[1] += (H2[0]+1) >> 1;
		}
		for ( g = 1; g < K_; g++)
		{
			if (H2[g])
			{
				H[g-1] += (H2[g]+1) >> 1;
				H[g] += H2[g]+1;
				H[g+1] += (H2[g]+1) >> 1;
			}
		}
		if (H2[K_])
		{
			H[K_-1] += (H2[K_]+1) >> 1;
			H[K_] += H2[K_]+1;
		}
		FREE( H2);
	}


	/* prepare cumulative histogram, needs K instead of full size	*/
	create_cumulative_histogram( H, K_);
}