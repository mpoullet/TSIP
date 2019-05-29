/*****************************************************************
*
* File..:	getHXC2_tolerances_2.c
* Descr.:	tolerance as a function of context-based information_content
* Author.:	Tilo Strutz
* Date..: 	18.09.2015
* changes:
* 29.09.2015 new:  MAX_TOL_A_B
* 08.10.2015 new determination of increasefac
* 28.10.2015 bugfix memory leaks in free_tree_recursive()
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "stats.h" /* required for heap_sort	*/
#include "imageFeatures.h" 
#include "hxc.h"	/* for MAX_PATTERN_SIZE, C_TREE_NODE_2	*/
#include "codec.h"	/* for max() */
#include "types.h"	/* for CALLOC */
#include "colour4.h"	/* for GREY */

extern  char* const thisVersion; /* in bacoder.cpp	*/

#define invLOG2	1.442695041  /* 1 / ln(2)  */

unsigned long lfdNr2; /*already in getHXC2tolerances.c	*/
double penalty;
double information_content;

/*---------------------------------------------------------------
*	merge_tree()
* merge nodes if the patterns are similar within the tolerances
* first call with t_node = root node, then recursive
*---------------------------------------------------------------*/
void merge_tree( C_TREE_NODE_2 *t_node,	AutoConfig *aC, 
								unsigned int numOfElements, 
								unsigned int cur_level)
{
	int match_flag;
	int diff;
	unsigned int comp;
	C_TREE_NODE_2 *current_item, *following_item, *cinl, *finl;


	do	/* loop over all items at this level	*/
	{
		current_item = t_node;
		following_item = t_node->next_item;
		// if (aC->HXC2tolerance[0][0] == 8) fprintf( stderr, "\r lfdNr2: %d         ", t_node->lfdNr);
		while( following_item != NULL)
		{ /* loop over all following items	*/
			/* and compare elements	*/
			match_flag = 1;
			for ( comp = 0; comp < numOfElements; comp++)
			{
				diff = (long)current_item->val[comp] - (long)following_item->val[comp];
				if ((unsigned)abs( diff) > aC->HXC2tolerance[comp][cur_level])
				{
					match_flag = 0;
					break; /* leave foo loop */
				}
			}
			if (match_flag)
			{	
				if (current_item->next_level != NULL)
				{
					/* append next-level item list of following item to 
					* next-level item list of current item
					*/
					cinl = current_item->next_level;
					finl = following_item->next_level;
					current_item->count += following_item->count;

					/* search for end of next-level item list of current item */
					while (cinl->next_item != NULL)
					{
						cinl = cinl->next_item;
					}
					cinl->next_item = finl; /* append	*/
				}

				if (current_item->next_level == NULL) //merge_histograms();
				{
					HIST_LIST_ELEM2 *current_hist_element, *new_hist_element;
					HIST_LIST_ELEM2 *parent_hist_element=NULL, *tmp;

					/* 
					* compare all items from other list (new_item) with all items from
					* current list
					* if equal, then add counts in current item and delete new_item
					* if not, append new item to current histogramm-item list
					*/
					//new_hist_element = current_item->next_item->top_hist_elem;
					new_hist_element = following_item->top_hist_elem;
					while(new_hist_element != NULL) /* as long as other items are there	*/
					{
						int hist_item_match_flag = 0;
						/* search match	*/
						current_hist_element = current_item->top_hist_elem; /* start with first item	*/
						while (current_hist_element != NULL) /* compare to all existing items	*/
						{
							hist_item_match_flag = 1;
							for ( comp = 0; comp < numOfElements; comp++)
							{
								if (current_hist_element->value[comp] != new_hist_element->value[comp])
								{
									hist_item_match_flag = 0; /* not equal	*/
									break; /* leave for loop	*/
								}
							}
							if (hist_item_match_flag)
							{
								current_hist_element->count += new_hist_element->count;
								tmp = new_hist_element; /* remember for removal	*/
								new_hist_element = new_hist_element->next; /* go to next	*/
								/* delete hist element	*/
								free( tmp);
								/* who had pointed to tmp? 
								* pointer should now point to  new_hist_element->next;
								*/
								break; /* leave inner while loop	*/
							}
							else
							{
								parent_hist_element = current_hist_element; /* remember successor	*/
								/* go to next histogram item in current list	*/
								current_hist_element = current_hist_element->next;
							}
						} /* while (current_hist_element */
						if (!hist_item_match_flag)
						{
							/* no suitable item has been found, new_item must be append to
							* current list
							*/
							/* append this hist element to the last item of current hist list	*/
							parent_hist_element->next = new_hist_element;
							new_hist_element = new_hist_element->next; /* go to next	*/
							/* remove address from new_item list	*/
							parent_hist_element->next->next = NULL; 
							current_item->hist_size++; /* increment number of histogram items	*/
						}
					} /* while(new_hist_element */
					/* histogramm content is copied	by now */
					/* update count of current node	*/
					current_item->count += following_item->count;
				} /* end of lowest level histogramm merging	*/ 
				/* look for predecessor of obsolete node	*/
				{ 
					C_TREE_NODE_2 *tmp;
					tmp = current_item;
					while (tmp->next_item != following_item)
					{
						tmp = tmp->next_item;
					}
					/* exlude following_item from list at current level	*/
					tmp->next_item = following_item->next_item;
#ifdef CHECK_MEMORY_LEAKS
					{
		int i;
	printf( "\n m: lfdNr= %d", following_item->lfdNr);
	for ( i = 0; i< 3; i++)
	{
		printf( ", %d", following_item->val[i]);
	}
	printf( " / %d", following_item->top_hist_elem);
}
#endif
					free( following_item); /* delete this node	*/
					lfdNr2--;
					following_item = tmp->next_item; /* for loop termination*/
				}
				//following_item = current_item->next_item;
			} /* if (match_flag) */
			else
			{
				following_item = following_item->next_item;
			}
		} /* while( following_item */

		/* all followers of current node have been inspected and
		* in case of a match the branches have been merged
		* now the merged branches must be inspected at next level
		*/
		if (t_node->next_level != NULL)
		{
			merge_tree( t_node->next_level, aC, numOfElements,
				cur_level+1);
		}

		if (t_node->next_item != NULL)  t_node = t_node->next_item;
	} while( t_node->next_item != NULL);
}

/*---------------------------------------------------------------
*	free_hist()
*---------------------------------------------------------------*/
void free_hist( HIST_LIST_ELEM2 *hist_elem)
{
	//HIST_LIST_ELEM2 *parent_hist_elem;

	//parent_hist_elem = hist_elem;
	if (hist_elem->next != NULL)
	{
		free_hist( hist_elem->next ); /* recursive call	*/
	}
	else
	{
		FREE( hist_elem);
	}
}
/*---------------------------------------------------------------
*	free_tree()
*---------------------------------------------------------------*/
void free_tree( C_TREE_NODE_2 *t_root)
{
	int found_mode;
	int	parent_mode = 0;
	unsigned long lfd;
	C_TREE_NODE_2 *t_node, *parent_node;

	lfd = lfdNr2; /* lfdNr2 is used in getImageFeatures() later on !! */
	while (lfd) /* as long as there are nodes, global variable	*/
	{
		t_node = t_root; /* start at the root of the tree	*/
		parent_node = t_root; /* required for freeing the root node */
		found_mode = 0;
		while (found_mode != 2) /* as long as there are children	*/
		{
			while (t_node->next_item != NULL) 
			{ /* scan for right child / next_item	*/
				parent_node = t_node; /* remember the parent	*/
				parent_mode = 0;	/* child is a right child	*/
				t_node = t_node->next_item; /* go to right child	*/
			}
			found_mode++; /* there is no further right child	*/

			if (t_node->next_level != NULL)
			{ /* go to next level / left child	*/
				parent_node = t_node; /* remember the parent	*/
				parent_mode = 1;	/* child is a left child	*/
				t_node = t_node->next_level; /* go to left child	*/
				found_mode = 0; /* reset flag	*/
			}
			else
			{
				found_mode++; /* there is no left child	*/
			}
		}
		/* free histogram if there is any	*/
		if (t_node->top_hist_elem != NULL) free_hist( t_node->top_hist_elem);
		/* cut tree at parent node	*/
		if (parent_mode) parent_node->next_level = NULL;
		else						 parent_node->next_item = NULL;
		
#ifdef CHECK_MEMORY_LEAKS
		{
		int i;
if (t_node->lfdNr > 550 &&  t_node->lfdNr < 560)
{
	maxLfdNr = maxLfdNr;
}
	printf( "\n f: lfdNr= %d", t_node->lfdNr);
	for ( i = 0; i< 3; i++)
	{
		printf( ", %d", t_node->val[i]);
	}
}
#endif
		free( t_node); /* remove current node	*/
	  lfd--; 
		if (!(lfd % 100)) fprintf( stderr, "\r%.2f%%   ", 100. * lfd / lfdNr2);
	};
}



/*---------------------------------------------------------------
 *	free_tree_recursive()
 *---------------------------------------------------------------*/
void free_tree_recursive( C_TREE_NODE_2 *t_node)
{
	if (t_node->next_level != NULL)
	{
		free_tree_recursive( t_node->next_level); /* recursive call	*/
	}
	if (t_node->next_item != NULL)
	{
		free_tree_recursive( t_node->next_item); /* recursive call	*/
	}

	//while (t_node->next_item != NULL)
	//{
	//	t_node = t_node->next_item;
	//	if (t_node->next_level != NULL)
	//	{
	//		free_tree_recursive( t_node->next_level); /* recursive call	*/
	//	}
	//};

	/* now we are at lowest level and no next neighbour exists	*/ 
	//fprintf( stderr, " %d", t_node->lfdNr);
	if (t_node->top_hist_elem != NULL)
	{
		free_hist( t_node->top_hist_elem);
		//FREE( t_node->top_hist_elem);
	}
#ifdef CHECK_MEMORY_LEAKS
{
		int i;
	printf( "\n r: lfdNr= %d", t_node->lfdNr);
	for ( i = 0; i< 3; i++)
	{
		printf( ", %d", t_node->val[i]);
	}
}
#endif
	FREE( t_node);
	// lfdNr2--;  ==0 checks simply if all nodes are accessed
}

/*---------------------------------------------------------------
*	scan_tree_2()
*---------------------------------------------------------------*/
//void scan_tree_2( C_TREE_NODE_2 *t_node, int level, 
//								 unsigned long count_nodes[])
//{
//	count_nodes[level]++;
//
//	if (t_node->next_level != NULL)
//	{
//		scan_tree_2( t_node->next_level, level+1, count_nodes); /* recursive call	*/
//	}
//	// if (level < 4) fprintf( stderr, "\n   ");
//	if (t_node->next_item != NULL)
//	{
//		scan_tree_2( t_node->next_item, level, count_nodes); /* recursive call	*/
//	}
//}


/*---------------------------------------------------------------
*	get_tree_information_recursive()
* take first appearance of each histogram iten´m into account!!
* => information_content will be larger in very detailed, branched list
*---------------------------------------------------------------*/
void get_tree_information( C_TREE_NODE_2 *t_node)
{

	if (t_node->next_level != NULL)
	{
		get_tree_information( t_node->next_level); /* recursive call	*/
	}
	else
	{
		HIST_LIST_ELEM2 *current_hist_element;


		/* penalty for building a histogram	*/
		information_content += t_node->hist_size * penalty; /* some bits for each entry	*/
		current_hist_element = t_node->top_hist_elem;
		/* update overall information_content	*/
		while ( current_hist_element != NULL)
		{
			double p;
			/* t_node->count is total number of counts in this histogram	*/
			p = (double)current_hist_element->count / t_node->count;
			/* global variable */
			information_content -= current_hist_element->count * log( p) * invLOG2;
			current_hist_element = current_hist_element->next;
		}

	}
	if (t_node->next_item != NULL)
	{
		get_tree_information( t_node->next_item); /* recursive call	*/
	}
}

/*---------------------------------------------------------------
*	create_next_item_2()
*
*---------------------------------------------------------------*/
C_TREE_NODE_2 *create_next_item_2( C_TREE_NODE_2 *t_node, 
																	long  Y[4], unsigned int numOfElements)
{
	unsigned int i;

	CALLOC( t_node->next_item, 1, C_TREE_NODE_2);
	if (ERRCODE == MALLOC_FAILED)	return NULL;
	t_node = t_node->next_item;
	t_node->next_item = NULL;
	t_node->next_level = NULL;
	t_node->top_hist_elem = NULL;
	for ( i = 0; i< numOfElements; i++)
	{
		t_node->val[i] = Y[i];
	}
	t_node->count = 1;
	t_node->lfdNr = lfdNr2;
	lfdNr2++; /* global variable	*/
#ifdef CHECK_MEMORY_LEAKS
	printf( "\n i: lfdNr= %d", t_node->lfdNr);
	for ( i = 0; i< numOfElements; i++)
	{
		printf( ", %d", t_node->val[i]);
	}
#endif
	return t_node;
}

/*---------------------------------------------------------------
*	next_level_node_2()
*
*---------------------------------------------------------------*/
C_TREE_NODE_2 *next_level_node_2( C_TREE_NODE_2 *t_node,	
																 long  Y[], unsigned int numOfElements)
{
	int match_flag, stop_flag;
	unsigned int i;

	assert( t_node != NULL);

	match_flag = 0; /* init, if while loop is not entered	*/

	if (t_node->next_level == NULL) /* next level does not exist yet	*/
	{
		CALLOC( t_node->next_level, 1, C_TREE_NODE_2);
		if (ERRCODE == MALLOC_FAILED)	return NULL;

		t_node = t_node->next_level;
		t_node->next_item = NULL;
		t_node->next_level = NULL;
		t_node->top_hist_elem = NULL;
		for ( i = 0; i < numOfElements; i++)
		{
			t_node->val[i] = Y[i];
		}
		t_node->count = 1;
		t_node->lfdNr = lfdNr2;
		lfdNr2++; /* global variable	*/
#ifdef CHECK_MEMORY_LEAKS
printf( "\n l: lfdNr= %d", t_node->lfdNr);
for ( i = 0; i< numOfElements; i++)
{
	printf( ", %d", t_node->val[i]);
}
#endif
		return t_node;
	}
	else
	{
		t_node = t_node->next_level;

		stop_flag = 0;
		do
		{	/* scan item for match	*/
			match_flag = 1;
			for ( i = 0; i < numOfElements; i++)
			{
				//long diff;
				if (t_node->val[i] != (unsigned)Y[i])
				//diff = (signed)(t_node->val[i]) - Y[i];
				//if ((unsigned)abs( diff) > aC->HXC2tolerance[i][cur_level])
				{
					match_flag = 0; /* does not match	*/
					break; /* exit for loop	*/
				}
			}
			if ( match_flag) break;  /* exit do-while loop	*/
			if (t_node->next_item == NULL)
			{
				stop_flag = 1;
			}
			else
			{
				t_node = t_node->next_item; /* go to next item at this level	*/
			}
		} while (!stop_flag);
	}

	if (match_flag)
	{
		t_node->count++; /* match found	*/
	}
	else /* no match could be found, allocate new item	*/
	{
		t_node = create_next_item_2( t_node, Y, numOfElements);
	}

	return t_node;
}


/*---------------------------------------------------------------
*	update_histogram()
*
*---------------------------------------------------------------*/
void update_histogram( C_TREE_NODE_2 *t_node, 
											long  Y[], unsigned int numOfElements)
{
	int equal_flag;
	unsigned int comp;
	HIST_LIST_ELEM2 *current_hist_element, *parent_element=NULL;

	/* pointer to top of histogramm list	*/
	current_hist_element = t_node->top_hist_elem;
	if (current_hist_element == NULL)
	{
#ifdef H_DEBUG
		fprintf( stdout, "\n\t\t histogram is still empty, start with actual X, count=1");
#endif
		/* there is no histogram yet	*/
		t_node->top_hist_elem =
			create_histogram_list_element2( Y, numOfElements, 1);
		t_node->hist_size++; /* update the number of histogram items	*/
	}
	else
	{	
		/* search histogram for X value
		* if X is not yet in histogram, then add list element
		*/
#ifdef H_DEBUG
		fprintf( stdout, "\n\t\t search actual X in histogram");
#endif
		do 
		{
			/* there is another element in histogram list
			* check, whether it is identical to current value
			*/
			equal_flag = 1;
			for ( comp = 0; comp < numOfElements; comp++)
			{
				if (Y[comp] != current_hist_element->value[comp])
				{
					equal_flag = 0; break;
				}
			}
			if (equal_flag)
			{
#ifdef H_DEBUG
				fprintf( stdout, ": X has been found, increment its count");
#endif
				/* increment count of this value and abort search */
				current_hist_element->count++;
				break;
			} /* if (equal_flag */
			/* remember the parent node for allocation of next node	*/
			parent_element = current_hist_element; 
			current_hist_element = current_hist_element->next; 
		} while(current_hist_element != NULL); 
		if (!equal_flag)
		{
			/* there is no matching item	*/
			parent_element->next =
				create_histogram_list_element2( Y, numOfElements, 1);
			t_node->hist_size++; /* update the number of histogram items	*/
		}
	}
}


/*---------------------------------------------------------------
*	getHXC2_tolerances_tree()
*
*	  f
*	 cbd
* eax
*
* generate tree with six levels (A..F),
* each leave corresponds to a distict pattern
* each leave is associated with a stacked histogram of X counts
* starting from the top level a the branches are merged (quantisation
*  of A values a.s.o.)
* it is tested how the single and total entropies change by combination 
*   of the distributions
*---------------------------------------------------------------*/
int getHXC2_tolerances_tree( PicData *pd,	AutoConfig *aC, ImageFeatures *iF)
{
	int err = 0;
	int match_flag;
	int d[MAX_PATTERN_SIZE];	/* offsets for pixel addressing	*/
	int tol_stop_flag[4][MAX_PATTERN_SIZE], ready_flag;
	//double info[4][MAX_PATTERN_SIZE];
	//double info_old][4][MAX_PATTERN_SIZE];
	double old_information_content;
	unsigned int i, comp, tol, maxTol[4], cnt_break, cnt_stop;
	//unsigned int tol_off, tol_level/*, tol_thresh*/;
	unsigned int numOfElements;
	//unsigned long count_nodes[MAX_PATTERN_SIZE+1];
	/* assume a maximum number of four components	*/
	long A[4], B[4], C[4], D[4], E[4], F[4], X[4];
	unsigned long *count_X = NULL;
	unsigned long count_total, num_pix;
	unsigned long r, c;
	unsigned long py, pos;
	C_TREE_NODE_2 *tree_root = NULL, *t_node = NULL;

	assert( pd->width > 3);
	assert( pd->height > 3);

	fprintf( stderr, "\n get HXC2 tolerances (tree based)...");

	/* offsets	*/
	d[0] = -1;		/* A	*/
	d[1] =    - (long)pd->width;	/* B	*/
	d[2] = -1 - (long)pd->width;	/* C	*/
	d[3] = +1 - (long)pd->width;	/* D	*/
	d[4] = -2;										/* E	*/
	d[5] =    - 2*(long)pd->width;	/* F	*/


	numOfElements = pd->channel;

	lfdNr2 = 0;
	CALLOC( tree_root, 1, C_TREE_NODE_2);
	if (ERRCODE == MALLOC_FAILED)		goto endfunc;

	/* generate trees
	* path from root (= A value ) to leave (==F) determines a certain pattern
	* number of trees is equal to number of distinct values for A in this component
	* number of leaves is equal to number of distinct patterns in this component
	*/
	tree_root->next_item = NULL;
	tree_root->next_level = NULL;
	for ( i = 0; i < numOfElements; i++)
	{
		tree_root->val[i] = 0;
	}
	tree_root->count = 0;
	tree_root->top_hist_elem = NULL;
	tree_root->lfdNr = lfdNr2;
	lfdNr2++;

	/* initialise max tolerances; used as stopp criterion for loop	*/
	maxTol[0] = pd->maxVal_pred[0];
	maxTol[1] = pd->maxVal_pred[1];
	maxTol[2] = pd->maxVal_pred[2];
	maxTol[3] = pd->maxVal_pred[3];
	/* start test with zero tolerance; reset flags	*/
	for ( comp = 0; comp < numOfElements; comp++)
	{
		//if (maxTol[comp] < pd->maxVal_pred[comp]) 	maxTol[comp] = pd->maxVal_pred[comp];

		for ( i = 0; i < aC->pattern_size; i++)
		{
			aC->HXC2tolerance[comp][i] = 0;
			tol_stop_flag[comp][i] = 0;
		}
		for ( ; i < MAX_PATTERN_SIZE; i++)
		{
			aC->HXC2tolerance[comp][i] = pd->maxVal_pred[comp];
			tol_stop_flag[comp][i] = 1;
		}
	}

	fprintf( stderr, "\n");
	/* filling the tree */
	count_total = 0;
	for ( r = 2, py = 2*pd->width; r < pd->height; r++, py+= pd->width)
	{
		for ( c = 2, pos = py + 2; c < pd->width-1; c++, pos++)
		{
			count_total++; 
			/* create tree counting of different contexts	*/
			for ( i = 0; i < numOfElements; i++)
			{
				A[i] = pd->data[i][pos + d[0]];
			}

			t_node = tree_root; /* start at top	A */
			/* search for the right A node	*/
			match_flag = 0; /* init, if while loop is not entered	*/
			while( t_node->next_item != NULL) /* scan list for matching node	*/
			{	/* scan items for match	*/
				t_node = t_node->next_item;
				match_flag = 1;
				for ( i = 0; i < numOfElements; i++)
				{
					if (t_node->val[i] != (unsigned)A[i])
					{
						match_flag = 0; /* does not match	*/
						break; /* exit for loop	*/
					}
				}
				if ( match_flag) break;  /* exit while loop	*/
			}
			if (match_flag)
			{
				t_node->count++; /* match found	*/
			}
			else /* no match could be found, allocate new item	*/
			{
				t_node = create_next_item_2( t_node, A, numOfElements);
			}

			/* go the levels down	*/
			/* B */
			for ( i = 0; i < numOfElements; i++)
			{
				B[i] = pd->data[i][pos + d[1]];
			}
			t_node = next_level_node_2( t_node, B, numOfElements);

			if (aC->pattern_size > 2)
			{
				/* C */
				for ( i = 0; i < numOfElements; i++)
				{
					C[i] = pd->data[i][pos + d[2]];
				}
				t_node = next_level_node_2( t_node, C, numOfElements);

				/* D */
				for ( i = 0; i < numOfElements; i++)
				{
					D[i] = pd->data[i][pos + d[3]];
				}
				t_node = next_level_node_2( t_node, D, numOfElements);
			}

			if (aC->pattern_size > 4)
			{
				/* E */
				for ( i = 0; i < numOfElements; i++)
				{
					E[i] = pd->data[i][pos + d[4]];
				}
				t_node = next_level_node_2( t_node, E, numOfElements);

				/* F */
				for ( i = 0; i < numOfElements; i++)
				{
					F[i] = pd->data[i][pos + d[5]];
				}
				t_node = next_level_node_2( t_node, F, numOfElements);
			}
			/* now fill histogramm of this pattern X */
			/* F */
			for ( i = 0; i < numOfElements; i++)
			{
				X[i] = pd->data[i][pos];
			}
			update_histogram( t_node, X, numOfElements);
		}
		fprintf( stderr, "\r%.2f%% ", 100.0 * (r+1) / pd->height);
	}
	fprintf( stderr, "\n nodes: %ld", lfdNr2);


	/* init */
	information_content = 0.0; /* global variable	*/
	num_pix = 0;
	//penalty = log( 0.5 * iF->numColours) / log(2.);
	//penalty = 0.5 * log( (double)iF->numColours) / log(2.); /* global variable	*/
	//penalty = 0.;
	penalty = log( 1.0 * iF->numColours) / log(2.); /* global variable	*/
	/* get initial information content	*/
	get_tree_information( tree_root->next_item); /* level 0 */
#ifdef _DEBUG
	fprintf( stderr, "\n information_content: %.3f", information_content);
#endif
	cnt_stop = 0;
	ready_flag = 0;
	fprintf( stderr, "\n ");

	cnt_break = 0;
	tol = 1;
	do
	{
		fprintf( stderr, "\r tol = %d", tol);
		for ( comp = 0; comp < numOfElements; comp++)
		{
			/* minus two, as positions i=4,5 can have value increased by 2	*/
			if ((signed)tol >= max( 0, (signed)maxTol[comp]-2) ) 
			{ /* ignore this component	*/
				cnt_break++;
				/* if all components have reached maximum tolerance then stop	*/
				if (cnt_break == pd->channel) ready_flag = 1;
				break;
			}
			for ( i = 0; i < aC->pattern_size; i++)
			{
				if (!tol_stop_flag[comp][i]) /* merging of branches as not been stopped yet */
				{
					aC->HXC2tolerance[comp][i]++; /* try higher tolerance	*/
				}
				else /* goto next tolerance	*/ 
					continue;

				/* ignore empty root item !!	*/
				merge_tree( tree_root->next_item, aC, numOfElements, 
					0 /* start at top level	*/);
				old_information_content = information_content;
				information_content = 0.0; /* global variable	*/
				get_tree_information( tree_root->next_item); /* level 0 */
#ifdef _DEBUG
				//if (old_information_content != information_content)
				{
					fprintf( stderr, "\ntol %d,", tol);
					fprintf( stderr, " comp %d, i= %d:", comp, i);
					{
						unsigned int j, k;
						for ( j = 0; j < numOfElements; j++)
						{
							for ( k = 0; k < aC->pattern_size; k++)
							{
								fprintf( stderr, " %2d", aC->HXC2tolerance[j][k]);
							}
							fprintf( stderr, ",");
						}
					}
					fprintf( stderr, "\n   information_content: %.3f", information_content);
					//fprintf( stderr, " => information_content: %.3f", information_content);
					fprintf( stderr, "  %.3f%%", 100. * information_content / old_information_content - 100.);
				}
#endif
				//if (information_content > info[comp][i])
				if (information_content > old_information_content)
				{			/* if information has increased then take action	*/
					/* resetting does not work as the tree cannot be unmerged :-( */
					//information_content = old_information_content; /* reset to best value	*/

					/* only stop increase of tol, if closer positions have already been stopped	*/
					if (i < 2 || 
						(i < 4 && tol_stop_flag[comp][0] && tol_stop_flag[comp][1]) ||
						(tol_stop_flag[comp][0] && tol_stop_flag[comp][1] && 
						tol_stop_flag[comp][2] && tol_stop_flag[comp][3])
						)
					{
						/* okay, tolerance is too large, take increment back */
						if (!tol_stop_flag[comp][i])
						{
							aC->HXC2tolerance[comp][i]--; /* undo increment	*/
							/* problem: the tree remains merged	*!!!!!!!!!!!!!!!!!!!!!!! */
#ifdef _DEBUG
							fprintf( stderr, "  *");
#endif
							tol_stop_flag[comp][i] = 1; /* stop further increments	*/
							cnt_stop++;
							if (cnt_stop == numOfElements * aC->pattern_size)
							{
								/* all values are freezed, exit all loops	*/
								ready_flag = 1;
								break;
							}
						}
					}
				}
				else if (information_content < old_information_content)
				{
					/* only do some action if new tolerance has changed the tree	*/
					if (i < 2 || 
						(i < 4 && tol_stop_flag[comp][0] && tol_stop_flag[comp][1]) ||
						(tol_stop_flag[comp][0] && tol_stop_flag[comp][1] && 
						tol_stop_flag[comp][2] && tol_stop_flag[comp][3])
						)
					{
						if (
							//(i < 2 && information_content > old_information_content * 0.99) ||
							//(i < 4 && information_content > old_information_content * 0.99875)||
							//(         information_content > old_information_content * 0.9996875)

							/* thresholds should depend on image features
							 * for example: the higher noise (??) the higher the factors
							 * => enabling larger tolerances
							 */
							(i < 2 && information_content > old_information_content * 0.985) ||
							//1.38a(i < 4 && information_content > old_information_content * 0.9975)||
							(i < 4 && information_content > old_information_content * 0.9925)||
							//1.38a(         information_content > old_information_content * 0.999375)
							(         information_content > old_information_content * 0.9975)
							/* improvement must be higher than a certain percentage	*/
							)
						{
							/* okay, tolerance is too large, take increment back */
							if (!tol_stop_flag[comp][i])
							{
								aC->HXC2tolerance[comp][i]--; /* undo increment	*/
								/* problem the tree remains merged	*!!!!!!!!!!!!!!!!!!!!!!! */
#ifdef _DEBUG
								fprintf( stderr, "  *");
#endif
								tol_stop_flag[comp][i] = 1; /* stop further increments	*/
								cnt_stop++;
								if (cnt_stop == numOfElements * aC->pattern_size)
								{
									/* all values are freezed, exit all loops	*/
									ready_flag = 1;
									break;
								}
							}
						} /* (i < 2 && information_content > */
					} /* if (i < 2 || */
				}/* else if ( (information_content <*/
				
					/* all tolerances below should have at least the same value	*/
				{
					unsigned int maxTole, ii;

					ii = (i << 1) >> 1;
					maxTole = max( aC->HXC2tolerance[comp][ii], aC->HXC2tolerance[comp][ii+1]);
					if (i < 4)
					{
						if (aC->HXC2tolerance[comp][4] < maxTole) aC->HXC2tolerance[comp][4] = maxTole;
						if (aC->HXC2tolerance[comp][5] < maxTole) aC->HXC2tolerance[comp][5] = maxTole;
					}
					if (i < 2)
					{
						if (aC->HXC2tolerance[comp][2] < maxTole) aC->HXC2tolerance[comp][2] = maxTole;
						if (aC->HXC2tolerance[comp][3] < maxTole) aC->HXC2tolerance[comp][3] = maxTole;
					}
				}
			} /* for ( i = 0;		*/
			if (ready_flag) break;
		} /* for ( comp = 0; */
		//if (ready_flag) break;
		tol++;
	} while (!ready_flag);

	fprintf( stderr, "\n lfdNr2: %ld", lfdNr2);


	// Problem: wie Pattern quantisieren und Entropie neu ausrechnen??
	// => merging of A nodes

	/* count nodes at each level
	* start at top level = 0
	*/
	//scan_tree_2( tree_root, 0, count_nodes);


#ifdef _DEBUG
	{
		unsigned int  k;
		for ( comp = 0; comp < numOfElements; comp++)
		{
			fprintf( stderr, "\n comp %d:", comp);
			for ( k = 0; k < aC->pattern_size; k++)
			{
				fprintf( stderr, " %2d", aC->HXC2tolerance[comp][k]);
			}
			fprintf( stderr, ",");
		}
	}
#endif


endfunc:

	//fprintf( stderr, "\n free tree");
	// free_tree( tree_root);
	free_tree_recursive( tree_root);
	/* must be re-allocated for each component.
	* as the maxvalues determine the allocated space
	*/
	//fprintf( stderr, "\n free count_X");
	FREE( count_X);

	
	{
		/* do not output values, since the other tolerance determination
		 * scheme will started
		 */
		unsigned int sum = 0;
		int monotony = 0;
		int correlation = 0;

		for ( comp = 0; comp < pd->channel; comp++)
		{
			fprintf( stderr, "\n %d: ", comp);
			monotony += iF->monotony[comp];
			correlation += iF->corrcoeff[comp];

			if (aC->HXC2tolerance[comp][0] > MAX_TOL_A_B)
			{
				/* larger numbers are not usefull	*/
				aC->HXC2tolerance[comp][0] = MAX_TOL_A_B;
			}
			if (aC->HXC2tolerance[comp][1] > MAX_TOL_A_B)
			{
				/* larger numbers are not usefull	*/
				aC->HXC2tolerance[comp][1] = MAX_TOL_A_B;
			}
			for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			{ /* show all tolerances	*/
				sum += aC->HXC2tolerance[comp][i];
				if (aC->HXC2tolerance[comp][i] == HXC2_TOL_EXCEPTION)
				{
					aC->HXC2tolerance[comp][i]++; /* avoid special number	*/
				}
				fprintf( stderr, "%d, ", aC->HXC2tolerance[comp][i]);
			}
		}
		if (iF->numColours < 8000)
		{
			aC->HXC2increaseFac = (unsigned int)floor(0.5 * max( 2, (double)monotony / pd->channel  - 90.));
			if (sum == 0)	aC->HXC2increaseFac = (aC->HXC2increaseFac + 3) / 2;
			fprintf( stdout, "Mon  incr: %d\t", aC->HXC2increaseFac);
			correlation = (int)floor( (double)correlation / pd->channel);
			if (correlation < 50) aC->HXC2increaseFac = 0;
			else if (correlation < 90) aC->HXC2increaseFac = 1;
			else if (correlation < 95) aC->HXC2increaseFac = 2;
			else aC->HXC2increaseFac = 3;
			if (sum == 0)	aC->HXC2increaseFac++;
			if (iF->syn_grad == 200)	aC->HXC2increaseFac++;
			fprintf( stdout, "Corr incr: %d\n", aC->HXC2increaseFac);
			fprintf( stderr, "Corr incr: %d\n", aC->HXC2increaseFac);
		}
		else
		{
			aC->HXC2increaseFac = 0;
			fprintf( stderr, "incr: %d\n", aC->HXC2increaseFac);
		}
	}
	return err;
}
