/*****************************************************
* File..: hist_ops_SCF.c
* Desc..: implementation of H(X|C) coder
* Desc..: operations of histogram list
* Author: T. Strutz
* Date..: 17.11.2015
* changes
* 19.11.2015 some function moved from scf.c
******************************************************/
#include <stdlib.h>
#include <assert.h>
#include "scf.h"
#include "types.h"
#include "codec.h"	/* for max( a, b);	*/

#ifdef _DEBUG
//  #define H_DEBUG
#endif

/*---------------------------------------------------------------
*	modify_merged_hist()
* checks, whether X is in the merged histogram
* increase its count
* used, when SCF_directional is enabled
*--------------------------------------------------------------*/
void modify_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
												long *X, unsigned int numOfElements, 
												unsigned int Hit_count)
{
	int match_flag;
	unsigned int comp;
	HIST_LIST_ELEM2 *merged_hist_element;

	match_flag = 0;

	/* exclude ESC item	*/ 
	merged_hist_element = merged_stacked_histogram->next;

	while( merged_hist_element != NULL)
	{
		match_flag = 1;
		for ( comp = 0; comp < numOfElements; comp++)
		{
			if (X[comp] != merged_hist_element->value[comp])
			{
				match_flag = 0;
				break; /* current item is not identical to selected item from merged list*/
			}
		}
		if (match_flag)
		{
			if (merged_hist_element->count > Hit_count)
			{
				merged_hist_element->count += Hit_count; /*must be reconsidered */
			}
			else merged_hist_element->count <<= 1; /* doubling	*/
			/* avoid to high numbers causing drawbacks when pixels do not match	*/

			break; /*leave while loop	*/
		}
		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/

	return;
}

/*---------------------------------------------------------------
 *	X_in_merged_hist_SCF()
 * checks, whether X is in the merged histogram
 * gets its position (*symbol)
 * copies the counts into a histogram (*H)
 * gives the number of histogram items (*K)
 * !!! Xhat is currently not used in scf.c ??? is it?
 *--------------------------------------------------------------*/
int X_in_merged_hist_SCF( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 long *X, unsigned int numOfElements, 
										 unsigned long *symbol, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac,
										 long *Xhat)
{
	int match_flag;
	long tmp_value[4];
	unsigned long tmp_cnt;
	unsigned int comp;
	unsigned int num;
	HIST_LIST_ELEM2 *merged_hist_element;

	for ( comp = 0; comp < numOfElements; comp++)
	{
		tmp_value[comp] = 0;
	}
	tmp_cnt = 0;
	match_flag = 0;
	*symbol = 0; /* X is not in the merged histogram */
	num = 0;
	H[0] = 0; /* first element remains zero	*/

	/* include ESC item	*/ 
	merged_hist_element = merged_stacked_histogram;

	while( merged_hist_element != NULL)
	{
		num++;
		H[num] = merged_hist_element->count * increaseFac;

		/* compute weighted average colour from all		*/
		if (merged_hist_element->value[comp] >= 0)
		{	/* skip ESC symbol	*/
			tmp_cnt += merged_hist_element->count;
			for ( comp = 0; comp < numOfElements; comp++)
			{
				tmp_value[comp] += merged_hist_element->count *
					                 merged_hist_element->value[comp];
			}
			/* use colour with maximal count instead of weighted average*/	
			//if (tmp_cnt < merged_hist_element->count)
			//{
			//	tmp_cnt = merged_hist_element->count;
			//	for ( comp = 0; comp < numOfElements; comp++)
			//	{
			//		tmp_value[comp] = merged_hist_element->value[comp];
			//	}
			//}
		}
		/* compare X to the current item	if match not found yet */
		if (match_flag == 0)
		{
			match_flag = 1;
			for ( comp = 0; comp < numOfElements; comp++)
			{
				if (X[comp] != merged_hist_element->value[comp])
				{
					match_flag = 0;
					break; /* current item is not identical to selected item from merged list*/
				}
			}
			if (match_flag)
			{
				*symbol = num-1; /* indices start with zero	*/
			}
		}
		/* else: go to the next item of merged histogram	
		 * in order to collect data for Xhat (tmp_value[] )
		 */
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/

	if (tmp_cnt)
	{
		for ( comp = 0; comp < numOfElements; comp++)
		{
			Xhat[comp] = (tmp_value[comp] + (tmp_cnt>>1) ) / tmp_cnt;
			//Xhat[comp] = tmp_value[comp];
		}
	}
	else /* indicate that nothing has been found	*/
	{
		for ( comp = 0; comp < numOfElements; comp++)
		{
			Xhat[comp] = -1;
		}
	}
	*K = num;

	return match_flag;
}

/*---------------------------------------------------------------
 *	get_merged_hist_SCF()
 * called by decoder
 * must provide similar function as X_in_merged_hist_SCF()
 * !!! Xhat is currently not used in scf.c
 *--------------------------------------------------------------*/
void get_merged_hist_SCF( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac,
										 long *Xhat, unsigned int numOfElements)
{
	long tmp_value[4];
	unsigned long tmp_cnt;
	unsigned int num, comp;
	HIST_LIST_ELEM2 *merged_hist_element;

	for ( comp = 0; comp < numOfElements; comp++)
	{
		tmp_value[comp] = 0;
	}
  tmp_cnt = 0;
	num = 0;
	H[0] = 0;/* first element remains zero	*/

	/* include ESC item	*/ 
	merged_hist_element = merged_stacked_histogram;

	while( merged_hist_element != NULL)
	{
		num++;
		H[num] = merged_hist_element->count * increaseFac; 

		/* compute weighted average colour from all		*/
		if (merged_hist_element->value[comp] >= 0)
		{	/* skip ESC symbol	*/
			tmp_cnt += merged_hist_element->count;
			for ( comp = 0; comp < numOfElements; comp++)
			{
				tmp_value[comp] += merged_hist_element->count *
					                 merged_hist_element->value[comp];
			}
			/* use colour with maximal count instead of weighted average*/	
			//if (tmp_cnt < merged_hist_element->count)
			//{
			//	tmp_cnt = merged_hist_element->count;
			//	for ( comp = 0; comp < numOfElements; comp++)
			//	{
			//		tmp_value[comp] = merged_hist_element->value[comp];
			//	}
			//}
		}

		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/

	if (tmp_cnt)
	{
		for ( comp = 0; comp < numOfElements; comp++)
		{
			Xhat[comp] = (tmp_value[comp] + (tmp_cnt>>1) ) / tmp_cnt;
			//Xhat[comp] = tmp_value[comp];
		}
	}
	else
	{
		for ( comp = 0; comp < numOfElements; comp++)
		{
			Xhat[comp] = -1;
		}
	}
	*K = num;
	return;
}

/*---------------------------------------------------------------
 *	palette_to_histogram_SCF()
 *--------------------------------------------------------------*/
void palette_to_histogram_SCF( HIST_LIST_ELEM2 *palette, 
														unsigned long *H, 
														long *Xhat_Merged, long *Xhat_MAP,
								 unsigned int numOfElements,
								 int size_flag, long diffMaxCalculated)
{
	int flag;
	//PM_MA diffMax changed depending on numOfColours
	long abs_diff, sum_diff=0, diffMax = diffMaxCalculated;
	unsigned int comp;
	unsigned long idx;
	HIST_LIST_ELEM2 *current_item;

	current_item = palette;
	idx = 0;
	H[0] = 0;	/* always	*/
	idx++;
	H[1] = current_item->count * MODE1_INCREMENT_SCF; /* count of symbol 0	(ESC) */
	 
	while (current_item->next != NULL) /* look ahead, exclude escape-symbol*/
	{
		idx++;
		if (size_flag) /* do not increase counts when size of palette is low */
		{
			/* check whether Xhat is identical or similar to item; if yes then
			 * increase its count depending on similiarity
			 * expectation: colour is similar to neighbours
			 */
			flag = 0;
			sum_diff = 0;
			for (comp = 0; comp < numOfElements; comp++)
			{
				abs_diff = abs( Xhat_MAP[comp] - current_item->next->value[comp]);
				if (abs_diff <= diffMax)  /* rather empirical	*/
				{
					flag++; /*  similar */
					sum_diff += abs_diff;
				}
			}
			if (flag < 2 && Xhat_Merged[comp] >= 0)
			{/* maximal one component is similar and Xhat_Merged is available	*/
				flag = 0;
				sum_diff = 0;
				for (comp = 0; comp < numOfElements; comp++)
				{
					abs_diff = abs( Xhat_Merged[comp] - current_item->next->value[comp]);
					if (abs_diff <= diffMax)  /* rather empirical	*/
					{
						flag++; /*  similar */
						sum_diff += abs_diff;
					}
				}
			}
		}
		else
		{ 
			flag = 0;
		}

		if (flag == 2) /* two components are similar	*/
		{ /* normal value	*/
			H[idx] = current_item->next->count  * MODE1_INCREMENT_SCF * 2;
		}
		else if (flag == 3) /* all components are similar	*/
		{
			// sum_diff <= 3* diffMax !!
			/*  8 <= fac = diffmax *2 - sumdiff/3 = 16 - MAD  <= 16 */
			H[idx] = current_item->next->count  * MODE1_INCREMENT_SCF * (diffMax*6 - sum_diff) / flag;
		}
		else
		{ /* normal count value if nothing is similar */
			H[idx] = current_item->next->count  * MODE1_INCREMENT_SCF;
		}
		current_item = current_item->next;
	}

	return;
}


/*---------------------------------------------------------------
 *	greyPalette_to_histogram_SCF()
 *--------------------------------------------------------------*/
void greyPalette_to_histogram_SCF( HIST_LIST_ELEM2 *palette, 
														unsigned long *H, 
														long *Xhat_MAP,
								 int size_flag, int meanAbs)
{
	int flag;
	long abs_diff;
	unsigned long idx;
	HIST_LIST_ELEM2 *current_item;

	current_item = palette;
	idx = 0;
	H[0] = 0;	/* always	*/
	idx++;
	H[1] = current_item->count * MODE1_INCREMENT_SCF; /* count of symbol 0	(ESC) */
	if (size_flag) H[1] = 0;

	while (current_item->next != NULL) /* look ahead, exclude escape-symbol*/
	{
		idx++;
		flag = 0;
		if (size_flag) /* do not increase counts when size of palette is low */
		{
			/* check whether Xhat is identical or similar to item; if yes then
			 * increase its count depending on similiarity
			 * expectation: colour is similar to neighbours
			 */
			abs_diff = abs( Xhat_MAP[0] - current_item->next->value[0]);
			if (abs_diff >= 2*meanAbs)  /* rather empirical	*/
			{
				H[idx] = 0;
			}
			else
			{
				int maxVal;
				maxVal = max( 1, 300 - meanAbs);
				H[idx] = maxVal - abs_diff * maxVal / (2*meanAbs); /* triangle	*/
			}
		}
		else
		{ /* normal count value if nothing is similar */
			H[idx] = current_item->next->count  * MODE1_INCREMENT_SCF;
		}
		current_item = current_item->next;
	}

	return;
}
