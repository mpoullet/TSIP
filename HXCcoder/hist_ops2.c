/*****************************************************
* File..: hist_ops2.c
* Desc..: implementation of H(X|C) coder
* Desc..: operations of histogram list
* Author: T. Strutz
* Date..: 20.08.2014
* changes
* 15.10.2014 new: fac = 2000 in X_in_palette()
******************************************************/
#include <stdlib.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"
#include "codec.h"	/* for max( a, b);	*/

#ifdef _DEBUG
//  #define H_DEBUG
#endif

/*--------------------------------------------------------
 * create_histogram_list_element2()
 *--------------------------------------------------------*/
HIST_LIST_ELEM2 * create_histogram_list_element2( 
				long *X, 
				unsigned int numOfElements, /* size of  X */
				unsigned int count
				)
{
	unsigned int i;
	HIST_LIST_ELEM2 *new_elem;

	ALLOC( new_elem, 1, HIST_LIST_ELEM2);
	/* required for copying counts from pattern histogram 
	 * to merged histogram
	 */
	new_elem->count = count; 
	new_elem->next = NULL;
	for ( i = 0; i < numOfElements; i++)
	{
		new_elem->value[i] = X[i];
	}
	for ( ; i < 4; i++)
	{
		new_elem->value[i] = 0; /* dummy values for remaining elements	*/
	}

	return new_elem;
}

/*--------------------------------------------------------
 * merge_stacked_histogram()
 *--------------------------------------------------------*/
void merge_stacked_histogram( 
		HIST_LIST_ELEM2 *top_hist_elem, /* first item of hist of current pattern	*/
		HIST_LIST_ELEM2 *merged_stacked_histogram, /* to be updated	*/
		unsigned long *numOfHistItems,	/* in the merged histogramm	*/
		unsigned int numOfElements /* size of item value */
		)
{
	int match_flag;
	unsigned int comp;
	HIST_LIST_ELEM2 *merged_hist_element=NULL, *parent_element=NULL;
	HIST_LIST_ELEM2 *current_hist_element=NULL;

#ifdef H_DEBUG
	/* first hist element of a particular pattern in the list	*/
	current_hist_element = merged_stacked_histogram;

	fprintf( stdout, "\n\t current merged histo:");
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

	/* first hist element of a particular pattern in the list	*/
	current_hist_element = top_hist_elem;

	fprintf( stdout, "\n\t pattern histo:");
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
#endif

	/* first hist element of a particular pattern in the list	*/
	current_hist_element = top_hist_elem;

	/* integrate all histogram items from current list to merged list	*/ 
	while( current_hist_element != NULL) /* list is terminated with NULL	*/
	{

		/* search identical item in merged histogram, 
		 * start with first item (ignore dummy top element) */
		// not needed here: parent_element = merged_stacked_histogram;
		merged_hist_element = merged_stacked_histogram;
		match_flag = 0;
		do
		{
			int identical_flag;
			/* there is already one item in the merged histogram	*/

			/* compare it to the current histogram item	*/
			identical_flag = 1;
			for ( comp = 0; comp < numOfElements; comp++)
			{
				if (current_hist_element->value[comp] != merged_hist_element->value[comp])
				{
					identical_flag = 0;
					break; /* current hist element is not identical to selectet item from 
								  * merged list*/
				}
			}
			if (identical_flag)
			{
				match_flag = 1;
				break;
			}
			/* else: go to the next item of merged histogram	*/
			parent_element = merged_hist_element; /*remember for allocation of next element*/
			merged_hist_element = merged_hist_element->next;
		} while( merged_hist_element != NULL);

		if (match_flag)
		{
			/* increase histogram by count of this item	*/
			merged_hist_element->count += current_hist_element->count;
		}
		else
		{
				/* the current item is not yet in the merged histogram
				 * so create a new item
				 */
				parent_element->next = create_histogram_list_element2( 
					current_hist_element->value, numOfElements, current_hist_element->count);

				(*numOfHistItems)++; /* increment number of items in merged histogram	*/
		}
		/* goto to the next histogram element	*/
		current_hist_element = current_hist_element->next;
	} /* while( current_hist_element != NULL)*/	

#ifdef H_DEBUG
	fprintf( stdout, "\n\t merged histo:");
	current_hist_element = merged_stacked_histogram;
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
#endif
}

/*---------------------------------------------------------------
 *	free_stacked_hist()
 *--------------------------------------------------------------*/
void free_stacked_hist( HIST_LIST_ELEM2 *stacked_histogram)
{
	HIST_LIST_ELEM2 *hist_elem;

	hist_elem = stacked_histogram->next;
	while (hist_elem != NULL)
	{
		HIST_LIST_ELEM2 *tmp;

		tmp = hist_elem->next;
		free( hist_elem);
		hist_elem = tmp;
	}
}

/*---------------------------------------------------------------
 *	X_in_merged_hist()
 * checks, whether X is in the merged histogram
 * gets its position (*symbol)
 * copies the counts into a histogram (*H)
 * gives the number of histogram items (*K)
 *--------------------------------------------------------------*/
int X_in_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 long *X, unsigned int numOfElements, 
										 unsigned long *symbol, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac)
{
	int match_flag;
	unsigned int comp;
	unsigned int num;
	HIST_LIST_ELEM2 *merged_hist_element;

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

		/* compare it to the current item	if match not found yet */
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
		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/

	*K = num;
	return match_flag;
}

/*---------------------------------------------------------------
 *	get_merged_hist()
 *--------------------------------------------------------------*/
void get_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac)
{
	unsigned int num;
	HIST_LIST_ELEM2 *merged_hist_element;

	num = 0;
	H[0] = 0;/* first element remains zero	*/

	/* include ESC item	*/ 
	merged_hist_element = merged_stacked_histogram;

	while( merged_hist_element != NULL)
	{
		num++;
		H[num] = merged_hist_element->count * increaseFac; 

		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/

	*K = num;
	return;
}

/*---------------------------------------------------------------
 *	get_X_from_merged_hist()
 *--------------------------------------------------------------*/
void get_X_from_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
								long *X, unsigned int numOfElements, unsigned long symbol)
{
	unsigned int num;
	unsigned int comp;
	HIST_LIST_ELEM2 *merged_hist_element;

	/* ->next: ignore ESC item	*/ 
	merged_hist_element = merged_stacked_histogram->next;
	num = 0;

	assert( merged_hist_element!= NULL );
	do
	{
		num++;

		if (symbol == num)
		{
			/* get item	*/
			for ( comp = 0; comp < numOfElements; comp++)
			{
				X[comp] = merged_hist_element->value[comp];
			}
			break;
		}
		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} while( merged_hist_element != NULL);
}

/*---------------------------------------------------------------
 *	integrate_X()
 *--------------------------------------------------------------*/
void integrate_X( HIST_LIST_ELEM2 *palette, long *X, 
								 unsigned int numOfElements, 
								 unsigned long *numOfPaletteItems)
{
	unsigned int flag;
	unsigned int comp;
	HIST_LIST_ELEM2 *current_item, *new_item;

	/* check, whether X is already in palette
	 * if yes: increment count
	 * if no: include X 
	 */
	current_item = palette->next; /* first item is escape symbol	*/
  flag = 0; /*in case tha while loop is not entered	*/
	while (current_item != NULL) /* look ahead*/
	{
		/* check whether X is identical to item	*/
		{
			flag = 1;
			for (comp = 0; comp < numOfElements; comp++)
			{
				if (X[comp] != current_item->value[comp])
				{
					flag = 0; 
					break; /* leave for loop	*/
				}
			}
			if (flag)  /* match */
			{
				current_item->count++;
				break; /* leave while loop	*/
			}
		}
		current_item = current_item->next;
	}

	if( !flag)
	{
			/* integration in front */
		new_item = create_histogram_list_element2( X, numOfElements, 1);
		new_item->next = palette->next;
		palette->next = new_item;
		(*numOfPaletteItems)++;
	}
	else /* count has been increased check for rescaling	*/
	{
		// 1.36(a) if (current_item->count >  4 + ((*numOfPaletteItems) >> 1) )
		// 1.36b better for images with more colours
		/* see also scf.c after "palette->count++;" same rescaling procedure !!!	*/
		if (current_item->count >  4 + ((*numOfPaletteItems) >> 2) )
		{
			current_item = palette; /* start with symbol ==0 item */
			while(current_item != NULL) 
			{
				current_item->count >>= 1;  /* scale down	*/
				current_item = current_item->next;
			}
		}
	}
}

/*---------------------------------------------------------------
 *	X_in_palette(), get position (=symbol) of X in the list
 *--------------------------------------------------------------*/
unsigned long X_in_palette( HIST_LIST_ELEM2 *palette, long *X, 
								 unsigned int numOfElements)
{
	int flag;
	unsigned int comp;
	unsigned long symbol, idx;
	HIST_LIST_ELEM2 *current_item;

	/* 
	 */
	current_item = palette;
	idx = 0;
	symbol = 0;

	flag = 0;
	while(current_item->next != NULL && !flag) /* look ahead*/
	{
		idx++;

		flag = 1;
		/* check whether X is identical to item	*/
		for (comp = 0; comp < numOfElements; comp++)
		{
			if (X[comp] != current_item->next->value[comp])
			{
				flag = 0; 
				break; /* leave for-loop	*/
			}
		}
		if (flag) symbol = idx; /* match */
		current_item = current_item->next;
	}

	return symbol;
}

/*---------------------------------------------------------------
 *	palette_to_histogram()
 *--------------------------------------------------------------*/
void palette_to_histogram( HIST_LIST_ELEM2 *palette, 
														unsigned long *H, long *Xhat,
								 unsigned int numOfElements, long *predTol,
								 int size_flag)
{
	int flag;
	long max_diff = 0;
	unsigned int comp;
	unsigned long idx;
	HIST_LIST_ELEM2 *current_item;

	current_item = palette;
	idx = 0;
	H[0] = 0;	/* always	*/
	idx++;
	H[1] = current_item->count * MODE1_INCREMENT; /* count of symbol 0	(ESC) */

	while (current_item->next != NULL) /* look ahead, exclude escape-symbol*/
	{
		idx++;
		if (size_flag) /* do not increase counts when size of palette is low */
		{
			/* check whether Xhat is identical or similar to item; if yes then
			 * increase its count depending on similiarity
			 * expactation: colour is similar to neighbours
			 */
			flag = 1;
			max_diff = 0;
			for (comp = 0; comp < numOfElements; comp++)
			{
				long diff;
				diff = abs( Xhat[comp] - current_item->next->value[comp]);
				if (max_diff < diff) max_diff = diff;
				// 1.36a/b  if (diff > predTol[comp])
				// 1.36c
				if (diff > predTol[comp] * 4)  /* rather emperical	*/
				/* predTol is MAD of prediction errors, 
				 * 1.48 * MAD is approx. equal to sigma for normal distribution
				 * ==> factor 3 ==> 2 * sigma
				 */
				{
					flag = 0; /* not similar */
					break; /* leave for loop	*/
				}
			}
		}
		else
		{
			flag = 0;
		}

		if (flag)
		{
			long count;
			/* increase count as this triple has a higher probability	*/
			/* this is a rather crude modification and should be optimised 
			 * in a information-theoretical manner
			 */
			// 1.36 count = current_item->next->count * max( 1, (4 - max_diff));
			// 1.36a
			count = current_item->next->count * max( 1, (8 - max_diff));
			H[idx] = count * MODE1_INCREMENT;
		}
		else
		{ /* normal value	*/
			H[idx] = current_item->next->count * MODE1_INCREMENT;
		}
		current_item = current_item->next;
	}

	return;
}

/*---------------------------------------------------------------
 *	get_X_from_palette()
 *--------------------------------------------------------------*/
void get_X_from_palette( HIST_LIST_ELEM2 *palette, unsigned long symbol, 
								 long *X, unsigned int numOfElements)
{
	unsigned int comp;
	unsigned long idx;
	HIST_LIST_ELEM2 *current_item;

	/* check, whether X is already in palette
	 * include X in sorted manner
	 */
	current_item = palette;
	idx = 0;

	while(current_item->next != NULL) /* look ahead */
	{
		idx++;

		/* check whether symbol is identical to item idx	*/
		if (symbol == idx) 
		{
			/* copy X item->value to X	*/
			for (comp = 0; comp < numOfElements; comp++)
			{
				X[comp] = current_item->next->value[comp];
			}
			break;
		}
		current_item = current_item->next;
	}
}
