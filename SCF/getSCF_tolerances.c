/*****************************************************************
*
* File..:	getSCF_tolerances.c
* Descr.:	tolerance as a function of context-based information_content
* Author.:	Tilo Strutz
* Date..: 	26.11.2015
* changes:
* 26.11.2015 copy from getHXC2_tolerances2.c
* 26.11.2015 chnaged threshold for tree merging
* 29.04.2016 new: getSCF_tolerances_HXY()
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "stats.h" /* required for heap_sort	*/
#include "imageFeatures.h" 
#include "hxc.h"	/* for MAX_PATTERN_SIZE, C_TREE_NODE_2	*/
#include "infotree.h"	/* for 	*/
#include "codec.h"	/* for max() */
#include "types.h"	/* for CALLOC */
#include "colour4.h"	/* for GREY */

extern  char* const thisVersion; /* in bacoder.cpp	*/

// see infotree.h
//#define invLOG2	1.442695041  /* 1 / ln(2)  */
//
//extern unsigned long lfdNr; /*already in getHXC2tolerances.c	*/
//extern double penalty;
//extern double information_content;


/*---------------------------------------------------------------
*	getSCF_tolerances_tree()
*
*	  f
*	 cbd
* eax
*
* generate tree with six levels (A..F),
* each leave corresponds to a distict pattern
* each leave is associated with a stacked histogram of X counts
* starting from the top level, the branches are merged (quantisation
*  of A values a.s.o.)
* it is tested how the single and total entropies change by combination 
*   of the distributions
*---------------------------------------------------------------*/
int getSCF_tolerances_tree( PicData *pd,	AutoConfig *aC, ImageFeatures *iF)
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

	fprintf( stderr, "\n get SCF tolerances (tree based)...");

	/* offsets	*/
	d[0] = -1;		/* A	*/
	d[1] =    - (long)pd->width;	/* B	*/
	d[2] = -1 - (long)pd->width;	/* C	*/
	d[3] = +1 - (long)pd->width;	/* D	*/
	d[4] = -2;										/* E	*/
	d[5] =    - 2*(long)pd->width;	/* F	*/


	numOfElements = pd->channel;

	lfdNr = 0;
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
	tree_root->lfdNr = lfdNr;
	lfdNr++;

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
	fprintf( stderr, "\n nodes: %ld", lfdNr);


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
				//if (information_content > old_information_content)
				if (information_content >= old_information_content)
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
							/* thresholds should depend on image features
							 * for example: the higher noise (??) the higher the factors
							 * => enabling larger tolerances
							 */
							(i < 2 && information_content > old_information_content * 0.985) ||
							//1.38a(i < 4 && information_content > old_information_content * 0.9975)||
							(i < 4 && information_content > old_information_content * 0.99)||
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

	fprintf( stderr, "\n lfdNr: %ld", lfdNr);


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


/*---------------------------------------------------------------
*	getHXC2_tolerances_H(XX)()
*
*	  f
*	 cbd
* eax
*
* estimate thresholds based on pair-wise joint entropies
*---------------------------------------------------------------*/
void getSCF_tolerances_HXY( PicData *pd,	AutoConfig *aC, ImageFeatures *iF)
{
	int d[MAX_PATTERN_SIZE];	/* offsets for pixel addressing	*/
	unsigned int i;
	unsigned int comp;
	unsigned long n, X;
	unsigned long range;
	unsigned long *count_X = NULL, count_bins;
	unsigned long count_total;
	unsigned long r, c;
	unsigned long **(H[MAX_PATTERN_SIZE]);
	unsigned long py, pos;

	assert( pd->width > 3);
	assert( pd->height > 3);

	fprintf( stderr, "\n get SCF tolerances HXY ...");

	/* offsets	*/
	d[0] = -1;		/* A	*/
	d[1] =    - (long)pd->width;	/* B	*/
	d[2] = -1 - (long)pd->width;	/* C	*/
	d[3] = +1 - (long)pd->width;	/* D	*/
	d[4] = -2;										/* E	*/
	d[5] =    - 2*(long)pd->width;	/* F	*/

	for (comp = 0; comp < pd->channel; comp++) /* for all components	*/
	{
		if (pd->maxVal_pred[comp] == 0)
		{ /* empty component	*/
			// for ( i = 0; i < aC->pattern_size; i++)
			for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			{
				aC->HXC2tolerance[comp][i] = 0;
			}
			continue; /* go to next	component */
		}

		for ( i = 0; i < aC->pattern_size; i++)
		{
			aC->HXC2tolerance[comp][i] = 0;
		}
		for ( ; i < MAX_PATTERN_SIZE; i++)
		{
			aC->HXC2tolerance[comp][i] = pd->maxVal_pred[comp];
		}

		/* for each spatial relation (position in pattern) allocate 2D array
		 * and set to zero
		 */
		range = pd->maxVal_pred[comp]+1;
		for ( i = 0; i < aC->pattern_size; i++)
		//for ( i = 0; i < MAX_PATTERN_SIZE; i++)
		{
			CALLOC( H[i], range, unsigned long*);
			for ( X = 0; X < (signed)range; X++)
			{
				CALLOC( H[i][X], range, unsigned long);
			}
			if (ERRCODE == MALLOC_FAILED)	goto endfunc;
		}
		CALLOC( count_X, range, unsigned long);
		if (ERRCODE == MALLOC_FAILED)	goto endfunc;



		/* determination of 2D histograms	*/
		count_total = 0;
		for ( r = 2, py = 2*pd->width; r < pd->height; r++, py+= pd->width)
		{
			for ( c = 2, pos = py + 2; c < pd->width-1; c++, pos++)
			{
				count_total++; 
				X = pd->data[comp][pos];
				count_X[X]++; /* count single values	*/

				//for ( i = 0; i < aC->pattern_size; i++)
				for ( i = 0; i < aC->pattern_size; i++)
				{
					/* increment count of pair at positions pos and (pos+d[i]) */
					H[i][ pd->data[comp][ pos+d[i] ]][X]++;
				}

			}
		}

		/* count non empty bins	*/
		count_bins = 0;
		for ( X = 0; X < range; X++) /* for all possible values at X position	*/
		{
			if (count_X[X]) count_bins++;
		}



		/* get tolerance values based on comparisons of entropies	*/
		{
			double entropy[MAX_PATTERN_SIZE], p, invN;
			unsigned long N;

			for ( i = 0; i < aC->pattern_size; i++) /* for all positions within pattern	*/
			//for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			{

				N = 0; /* total count per histogram */
				for ( n = 0; n < range; n++) /* for all possible values at i position	*/
				{
					for ( X = 0; X < range; X++) /* for all possible values at X position	*/
					{
						N += H[i][n][X]; /* accumulate total count in first histogram	*/
					}
				}
				entropy[i] = 0;
				invN = 1. / N;
				for ( n = 0; n < range; n++) /* for all possible values at i position	*/
				{
					for ( X = 0; X < range; X++) /* for all possible values at X position	*/
					{
						if (H[i][n][X]) /* if bin is not empty	*/
						{ /* compute single probability	*/
							p = (double)H[i][n][X] * invN;
							entropy[i] -= p * log(p) * invLOG2;
						}
					}
				}
				/* problem NOMOD vs. COMPACTION
				 * stretching the histogram has almost no influence on the entropy
				 * but on the tolerances to be used
				 * correction factor is needed! ==> range / count_bins
				 */
#ifdef NEWTOL1
				if (i < 2)
				{
					p = pow( 2., entropy[i]) * 0.03125;
				}
				else if (i < 4)
				{
					double e;
					e = 0.125* (entropy[0] + entropy[1]);
					p = pow( 2., e + entropy[i]) * 0.03125;
				}
				else 
				{
					double e;
					e = 0.125* (entropy[0] + entropy[1] + entropy[2] + entropy[3]);
					p = pow( 2., e + entropy[i]) * 0.03125;
				}
#else
				if (i < 2)
				{
					//p = pow( 1.5, entropy[i]) * 0.0625;
					//p = pow( 1.4, entropy[i]) * 0.0625;
					//p = pow( 1.3, entropy[i]) * 0.0625;
					p = pow( 1.3, entropy[i]) * 0.06;
				}
				else if (i < 4)
				{
					double e;
					//e = 0.125* (entropy[0] + entropy[1]);
					//e = 0.25* (entropy[0] + entropy[1]);
					e = 0.4* (entropy[0] + entropy[1]);
					//p = pow( 1.4, e + entropy[i]) * 0.0625;
					//p = pow( 1.3, e + entropy[i]) * 0.0625;
					p = pow( 1.3, e + entropy[i]) * 0.06;
					/* for GREY images, the tolerance thresholds must b highr	*/ 
					if (pd->channel == 1) p = pow( 1.3, e + entropy[i]) * 0.2; /* not testet yet	*/
				}
				else 
				{
					double e;
					//e = 0.125* (entropy[0] + entropy[1] + entropy[2] + entropy[3]);
					//e = 0.25* (entropy[0] + entropy[1] + entropy[2] + entropy[3]);
					e = 0.4* (entropy[0] + entropy[1] + entropy[2] + entropy[3]);
					//p = pow( 1.4, e + entropy[i]) * 0.0625;
					//p = pow( 1.3, e + entropy[i]) * 0.0625;
					p = pow( 1.3, e + entropy[i]) * 0.06;
					/* for GREY images, the tolerance thresholds must b highr	*/ 
					if (pd->channel == 1) p = pow( 1.3, e + entropy[i]) * 0.3; /* not testet yet	*/
				}
#endif
				//if ( aC->rct_type_used == RGBc)
				//{
				//	if ( i < 2) p = p / 32;
				//	else if ( i < 4) p = p / 4;
				//	else p = p * 1;
				//}
				//else
				//{
				//	if ( i < 2) p = p / 16;
				//	else if ( i < 4) p = p / 8;
				//	else p = p / 2;
				//}
				//aC->HXC2tolerance[comp][i] = min( range, (unsigned int)floor( p) * range / count_bins);
				aC->HXC2tolerance[comp][i] = min( range, (unsigned int)floor( p * (double)range / count_bins));
			} /*for ( i = 0; */
		}

endfunc:

		/* must be re-allocated for each component.
		 * as the maxvalues determine the allocated space
		 */
		//fprintf( stderr, "\n free H");
		for ( i = 0; i < aC->pattern_size; i++)
		//for ( i = 0; i < MAX_PATTERN_SIZE; i++)
		{
			for ( X = 0; X < range; X++)
			{
				FREE( H[i][X]);
			}
			//fprintf( stderr, "\n free H[%d]", i);
			FREE( H[i]);
		}
		//fprintf( stderr, "\n free count_X");
		FREE( count_X);
	} /* for (comp = 0;*/

	{
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
			//for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			for ( i = 0; i < aC->pattern_size; i++)
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
			if (sum == 0)	aC->HXC2increaseFac = 2;
			else if (sum < 7)	aC->HXC2increaseFac = 1;
			fprintf( stderr, "incr: %d\n", aC->HXC2increaseFac);
		}
	}
	fprintf( stderr, "\n");
	return;
}
