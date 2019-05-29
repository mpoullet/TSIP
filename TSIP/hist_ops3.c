/*****************************************************
* File..: hist_ops3.c
* Desc..: implementation of H(X|C)3 coder
* Desc..: operations of histogram list
* Author: T. Strutz
* Date..: 02.09.2015
******************************************************/
#include <stdlib.h>
#include <assert.h>
#include "hxc3.h"
#include "types.h"

#ifdef _DEBUG
//#define H_DEBUG
#endif

/*--------------------------------------------------------
 * create_histogram_list_element3()
 *--------------------------------------------------------*/
HIST_LIST_ELEM3 *create_histogram_list_element3( 
				long X, unsigned int increment_value)
{
	HIST_LIST_ELEM3 *new_elem;

	ALLOC( new_elem, 1, HIST_LIST_ELEM3);
	new_elem->count = increment_value;	/* always = 1 in the moment	*/
	new_elem->next = NULL;
	new_elem->value = X;

	return new_elem;
}

/*--------------------------------------------------------
 * merge_stacked_histogram3()
 *--------------------------------------------------------*/
void merge_stacked_histogram3( 
		HIST_LIST_ELEM3 *top_hist_elem, /* first item of hist of current pattern	*/
		HIST_LIST_ELEM3 *merged_stacked_histogram, /* to be updated	*/
		unsigned long *numOfHistItems,	/* in the merged histogramm	*/
		unsigned int increaseFac,	/* multiplier for counts	*/
		unsigned long pos)	/* just for debugging	*/
{
	int match_flag;
	HIST_LIST_ELEM3 *merged_hist_element=NULL, *parent_element=NULL;
	HIST_LIST_ELEM3 *current_hist_element=NULL;

#ifdef H_DEBUG
	if ( pos < MAX_DEBUG_POS)
	{
		/* first hist element of a particular pattern in the list	*/
		current_hist_element = top_hist_elem;

		fprintf( stdout, "\n\t current merged histo:");
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
			if (current_hist_element->value != merged_hist_element->value)
			{
				identical_flag = 0;
				/* current hist element is not identical to selectet item from 
							  * merged list*/
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
			merged_hist_element->count += current_hist_element->count * increaseFac;
		}
		else
		{
			//if ((*numOfHistItems) < MAX_HIST_SIZE) /**/
			{
				/* the current item is not yet in the merged histogram
				 * so create a new item
				 */
				parent_element->next = create_histogram_list_element3( 
					current_hist_element->value, 1);
				/* copy the count from current histogram	*/
				parent_element->next->count = current_hist_element->count * increaseFac;

				(*numOfHistItems)++; /* increment number of items in merged histogram	*/
			}
		}
		/* goto to the next histogram element	*/
		current_hist_element = current_hist_element->next;
	} /* while( current_hist_element != NULL)*/	

#ifdef H_DEBUG
	if ( pos < MAX_DEBUG_POS)
	{
		fprintf( stdout, "\n\t merged histo:");
		current_hist_element = merged_stacked_histogram;
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
}

/*---------------------------------------------------------------
 *	free_stacked_hist3()
 *--------------------------------------------------------------*/
void free_stacked_hist3( HIST_LIST_ELEM3 *stacked_histogram)
{
	HIST_LIST_ELEM3 *hist_elem;

	hist_elem = stacked_histogram->next;
	while (hist_elem != NULL)
	{
		HIST_LIST_ELEM3 *tmp;

		tmp = hist_elem->next;
		free( hist_elem);
		hist_elem = tmp;
	}
}

/*---------------------------------------------------------------
 *	X_in_merged_hist3()
 * checks, whether X is in the merged histogram
 * gets its position (*symbol)
 * copies the counts into a histogram (*H)
 * gives the number of histogram items (*K)
 *--------------------------------------------------------------*/
int X_in_merged_hist3( HIST_LIST_ELEM3 *merged_stacked_histogram, 
										 long X, 
										 unsigned long *symbol, unsigned long *H, unsigned long *K)
{
	int match_flag;
	unsigned int num;
	HIST_LIST_ELEM3 *merged_hist_element;

	match_flag = 0;
	*symbol = 0; /* X is not in the merged histogram */
	H[0] = 0; /* first element remains zero	*/
	num = 1;
	H[num] = merged_stacked_histogram->count; /* count of ESC item	*/

	/* ->next: ignore ESC item	*/ 
	merged_hist_element = merged_stacked_histogram->next;

	while( merged_hist_element != NULL)
	{
		num++;
		H[num] = merged_hist_element->count;

		/* compare it to the current item	if match not found yet */
		if (match_flag == 0)
		{
			match_flag = 1;
			if (X != merged_hist_element->value)
			{
				match_flag = 0;
				/* current item is not identical to selectd item from merged list*/
			}
			if (match_flag)
			{
				match_flag = 1;
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
 *	get_merged_hist3()
 *--------------------------------------------------------------*/
void get_merged_hist3( HIST_LIST_ELEM3 *merged_stacked_histogram, 
										 unsigned long *H, unsigned long *K)
{
	unsigned int num;
	HIST_LIST_ELEM3 *merged_hist_element;

	H[0] = 0;
	num = 1;
	H[num] = merged_stacked_histogram->count; /* count of ESC item	*/

	/* ->next: ignore ESC item	*/ 
	merged_hist_element = merged_stacked_histogram->next;

	while( merged_hist_element != NULL)
	{
		num++;
		H[num] = merged_hist_element->count; /* first element remains zero	*/

		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} /* while( merged_hist_*/

	*K = num;
	return;
}

/*---------------------------------------------------------------
 *	get_X_from_merged_hist3()
 *--------------------------------------------------------------*/
void get_X_from_merged_hist3( HIST_LIST_ELEM3 *merged_stacked_histogram, 
								long *X,  unsigned long symbol)
{
	unsigned int num;
	HIST_LIST_ELEM3 *merged_hist_element;

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
			*X = merged_hist_element->value;
			break;
		}
		/* else: go to the next item of merged histogram	*/
		merged_hist_element = merged_hist_element->next;
	} while( merged_hist_element != NULL);
}

/*---------------------------------------------------------------
 *	integrate_X3()
 *--------------------------------------------------------------*/
void integrate_X3( HIST_LIST_ELEM3 *palette, long X, 
								 unsigned long *numOfPaletteItems)
{
	unsigned int flag;
	HIST_LIST_ELEM3 *current_item, *new_item;

	/* check, whether X is already in palette
	 * if yes: increment count
	 * if no: include X 
	 */
	current_item = palette->next;
  flag = 0; /*in case tha while loop is not entered	*/
	while(current_item != NULL) /* look ahead*/
	{
		/* check whether X is identical to item	*/
		{
			flag = 1;
			if (X != current_item->value)
			{
				flag = 0; 
			}
			if (flag)  /* match */
			{
				current_item->count++;
				break;
			}
		}
		current_item = current_item->next;
	}

	if( !flag)
	{
			/* integration in front */
		new_item = create_histogram_list_element3( X, 1 /* increment	*/);
		new_item->next = palette->next;
		palette->next = new_item;
		(*numOfPaletteItems)++;
	}
}

/*---------------------------------------------------------------
 *	X_in_palette3(), get position (=symbol) of X in the list
 * and create histogram
 *--------------------------------------------------------------*/
unsigned long X_in_palette3( HIST_LIST_ELEM3 *palette, long X, 
								 unsigned long *H)
{
	int flag;
	unsigned long symbol, idx;
	HIST_LIST_ELEM3 *current_item;

	/* 
	 */
	current_item = palette;
	idx = 0;
	H[0] = 0;	/* always	*/
	H[1] = current_item->count; /* count of symbol 0	*/
	symbol = 0;

	while(current_item->next != NULL) /* look ahead*/
	{
		idx++;
		H[idx+1] = current_item->next->count;

		/* check whether X is identical to item	*/
		if (symbol == 0) /* if X has not been found yet	*/
		{
			flag = 1;
			if (X != current_item->next->value)
			{
				flag = 0; 
			}
			if (flag) symbol = idx; /* match */
		}
		current_item = current_item->next;
	}

	return symbol;
}

/*---------------------------------------------------------------
 *	get_X_from_palette3()
 *--------------------------------------------------------------*/
void get_X_from_palette3( HIST_LIST_ELEM3 *palette, unsigned long symbol, 
								 long *X)
{
	unsigned long idx;
	HIST_LIST_ELEM3 *current_item;

	/* check, whether X is already in palette
	 * include X in sorted manner
	 */
	current_item = palette;
	idx = 0;

	while(current_item->next != NULL) /* look ahead*/
	{
		idx++;

		/* check whether symbol is identical to item idx	*/
		if (symbol == idx) 
		{
			/* copy X item->value to X	*/
			*X = current_item->next->value;
			break;
		}
		current_item = current_item->next;
	}
}
