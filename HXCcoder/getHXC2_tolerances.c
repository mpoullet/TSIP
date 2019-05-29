/*****************************************************************
*
* File..:	getHXC2_tolerances.c
* Descr.:	tolerance as a function of context-based entropy
* Author.:	Tilo Strutz
* Date..: 	02.11.2014
* changes:
* 11.11.2014 new calculateion based on slope of counts
* 19.03.2015 new calculateion based on similarity of
*						probability density functions
* 23.04.2015 other factor for GREY images
* 10.09.2015 new getHXC2tolerances() for 1_34a
* 29.09.2015 new:  MAX_TOL_A_B
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "stats.h" /* required for heap_sort	*/
#include "imageFeatures.h" 
#include "hxc.h"	/* for MAX_PATTERN_SIZE	*/
#include "codec.h"	/* for max() */
#include "types.h"	/* for CALLOC */
#include "colour4.h"	/* for GREY */

extern  char* const thisVersion; /* in bacoder.cpp	*/

#define invLOG2	1.442695041  /* 1 / ln(2)  */
unsigned long lfdNr;
	


/*---------------------------------------------------------------
*	free_items()
*---------------------------------------------------------------*/
void free_items( C_TREE_NODE *t_node)
{
	if (t_node->next_level != NULL)
	{
		free_items( t_node->next_level); /* recursive call	*/
	}
	if (t_node->next_item != NULL)
	{
		free_items( t_node->next_item); /* recursive call	*/
	}
	//fprintf( stderr, " %d", t_node->lfdNr);
	FREE( t_node);
	lfdNr--;
}

/*---------------------------------------------------------------
*	scan_tree()
*---------------------------------------------------------------*/
void scan_tree( C_TREE_NODE *t_node, int level, unsigned long count_nodes[])
{
	count_nodes[level]++;
	//if (level == 0) /* == A */
	//{
	//	fprintf( stderr, "\n %d", t_node->val);
	//}
	//else fprintf( stderr, ", %d", t_node->val);

	//if (level == 3) /* == D */
	//{
	//	fprintf( stderr, " count: %d", t_node->count);
	//}
	//else if (level == 4) /* == X */
	//{
	//	fprintf( stderr, "(%d)", t_node->count);
	//}

	if (t_node->next_level != NULL)
	{
		scan_tree( t_node->next_level, level+1, count_nodes); /* recursive call	*/
	}
	// if (level < 4) fprintf( stderr, "\n   ");
	if (t_node->next_item != NULL)
	{
		scan_tree( t_node->next_item, level, count_nodes); /* recursive call	*/
	}
}

/*---------------------------------------------------------------
*	create_next_item()
*
*---------------------------------------------------------------*/
C_TREE_NODE *create_next_item( C_TREE_NODE *t_node, unsigned long  X)
{
	CALLOC( t_node->next_item, 1, C_TREE_NODE);
	if (ERRCODE == MALLOC_FAILED)	return NULL;
	t_node = t_node->next_item;
	t_node->val = X;
	t_node->count = 1;
	t_node->next_item = NULL;
	t_node->next_level = NULL;

	t_node->lfdNr = lfdNr;
	lfdNr++; /* global variable	*/
	return t_node;
}


/*---------------------------------------------------------------
*	next_level_node()
*
*---------------------------------------------------------------*/
C_TREE_NODE *next_level_node( C_TREE_NODE *t_node, unsigned long  X)
{
	assert( t_node != NULL);
	if (t_node->next_level == NULL)
	{
		CALLOC( t_node->next_level, 1, C_TREE_NODE);
		if (ERRCODE == MALLOC_FAILED)	return NULL;

		t_node = t_node->next_level;
		t_node->next_item = NULL;
		t_node->next_level = NULL;
		t_node->val = X;
		t_node->count = 1;
		t_node->lfdNr = lfdNr;
		lfdNr++; /* global variable	*/
		return t_node;
	}
	else
	{
		t_node = t_node->next_level;
	}

	while( t_node->val != X && t_node->next_item != NULL)
	{	/* scan items for match	*/
		t_node = t_node->next_item;
	}
	if (t_node->val == X)
	{
		t_node->count++; /* match found	*/
	}
	else /* no match could be found, allocate new item	*/
	{
		t_node = create_next_item( t_node, X);
	}

	return t_node;
}


/*---------------------------------------------------------------
*	getHXC2_tolerances____()
*
*	  f
*	 cbd
* eax
*
*---------------------------------------------------------------*/
void getHXC2_tolerances____( PicData *pd,	AutoConfig *aC, ImageFeatures *iF)
{
	int i;
	int d[MAX_PATTERN_SIZE];	/* offsets for pixel addressing	*/
	unsigned int comp;
	unsigned long n, X, count_nodes[MAX_PATTERN_SIZE+1];
	unsigned long range, cnt_AB_comb;
	unsigned long *count_X = NULL;
	unsigned long count_total;
	unsigned long r, c;
	unsigned long **(H[MAX_PATTERN_SIZE]);
	unsigned long py, pos;
	C_TREE_NODE *tree_root = NULL, *t_node = NULL;

	assert( pd->width > 3);
	assert( pd->height > 3);

	fprintf( stderr, "\n get HXC2 tolerances ...");

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
			for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			{
				aC->HXC2tolerance[comp][i] = 0;
			}
			continue; /* go to next	component */
		}

		/* for each spatial relation (position in pattern) allocate 2D array
		 * and set to zero
		 */
		range = pd->maxVal_pred[comp]+1;
		for ( i = 0; i < MAX_PATTERN_SIZE; i++)
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


		lfdNr = 0;
		CALLOC( tree_root, 1, C_TREE_NODE);
		if (ERRCODE == MALLOC_FAILED)		goto endfunc;

		/* generate trees
		 * path from root (= A value ) to leave (==F) determines a certain pattern
		 * number of trees is equal to number of distinct values for A in this component
		 * number of leaves is equal to number of distinct patterns in this component
		 */
		tree_root->next_item = NULL;
		tree_root->next_level = NULL;
		tree_root->val = 0;
		tree_root->count = 0;
		tree_root->lfdNr = lfdNr;
		lfdNr++;

		for ( i = 0; i <= MAX_PATTERN_SIZE; i++)
		{
			count_nodes[i] = 0;
		}

		/* determination of 2D histograms	*/
		count_total = 0;
		for ( r = 2, py = 2*pd->width; r < pd->height; r++, py+= pd->width)
		{
			for ( c = 2, pos = py + 2; c < pd->width-1; c++, pos++)
			{
				count_total++; 
				X = pd->data[comp][pos];
				count_X[X]++; /* count single values	*/

				for ( i = 0; i < MAX_PATTERN_SIZE; i++)
				{
					/* increment count of pair at positions pos and (pos+d[i]) */
					H[i][ pd->data[comp][ pos+d[i] ]][X]++;
				}

				/* create tree counting of different contexts	*/
				X = pd->data[comp][pos + d[0]];

				t_node = tree_root; /* start at top	A */
				while( t_node->val != X && t_node->next_item != NULL)
				{	/* scan items for match	*/
					t_node = t_node->next_item;
				}
				if (t_node->val == X)
				{
					t_node->count++; /* match found	*/
				}
				else /* no match could be found, allocate new item	*/
				{
					t_node = create_next_item( t_node, X);
				}

				/* B */
				X = pd->data[comp][pos + d[1]];
				t_node = next_level_node( t_node, X);

				/* C */
				X = pd->data[comp][pos + d[2]];
				t_node = next_level_node( t_node, X);

				/* D */
				X = pd->data[comp][pos + d[3]];
				t_node = next_level_node( t_node, X);

				/* E */
				X = pd->data[comp][pos + d[4]];
				t_node = next_level_node( t_node, X);

				/* F */
				X = pd->data[comp][pos + d[5]];
				t_node = next_level_node( t_node, X);

				/* X */
				//X = pd->data[comp][pos];
				//t_node = next_level_node( t_node, X);

			}
		}

		/* count nodes at each level
		 * start at top level = 0
		 */
		scan_tree( tree_root, 0, count_nodes);

		/* D and X are in same spatial relation as A and B
		 * how many patterns AB are there?
		 */
		cnt_AB_comb = 0;
		for ( n = 0; n < range; n++)
		{ 
			for ( X = 0; X < range; X++)
			{
				if (H[3][n][X]) cnt_AB_comb++;
			}
		}

		fprintf( stderr, "\n %d: ", comp);
		/* get tolerance values based on comparisons of entropies	*/
		{
			double entropy1=0., entropy2, entropyM, p, diff;
			double percent, perc;
			double fixed_fac;
			unsigned long N1=0, N2, tol, n_last;
			unsigned long cnt_segments, cnt_empty;

			// fixed_fac = (double)pd->size / (range * range); /* range benachteiligt COMPACTION	*/
			fixed_fac = (double)pd->size ;
			for ( i = 0; i < MAX_PATTERN_SIZE; i++) /* for all positions within pattern	*/
			{
				cnt_empty = 0;
				cnt_segments = 0;
				n_last = 0;

				/* compute probability density functions	*/
				/* first pdf	*/
				n = 0;
				/* compute first entropy	*/
				do
				{
					N1 = 0; /* total count per histogram */
					for ( X = 0; X < range; X++) /* for all possible values at X position	*/
					{
						N1 += H[i][n][X]; /* accumulate total count in first histogram	*/
					}
					entropy1 = 0.; /* reset 	*/
					if (N1)
					{ /* this histogram is not empty	*/
						for ( X = 0; X < range; X++)
						{ 
							if (H[i][n][X]) /* if bin is not empty	*/
							{ /* compute single probability	*/
								p = (double)H[i][n][X] / N1;
								entropy1 -= p * log(p) * invLOG2;
							}
						}
						n_last = n;
					}
					else cnt_empty++;
					n++;
				} while (N1 == 0); /* scan empty histograms	*/
				
				for ( ; n < range; n++) /* for all possible values at position i	*/
				{
					/* second pdf	*/
					N2 = 0; /* total count per histogram */
					for ( X = 0; X < range; X++)
					{
						N2 += H[i][n][X]; /* accumulate total count in 2nd histogram	*/
					}
					entropy2 = 0.; /* reset 	*/
					if (N2)
					{ /* this histogram is not empty	*/
						for ( X = 0; X < range; X++)
						{
							if (H[i][n][X]) /* if bin is not empty	*/
							{ /* compute single probability	*/
								p = (double)H[i][n][X] / N2;
								entropy2 -= p * log(p) * invLOG2;
							}
						}
					}
					/* compute distance between pdfs*/
					entropyM = 0.; /* reset 	*/
					if (N1 && N2) /* both are not empty	*/
					{
						for ( X = 0; X < range; X++)
						{
							if (H[i][n_last][X] || H[i][ n][X]) /* if bin is not empty	*/
							{ /* compute single probability	*/
								p = (double)(H[i][n_last][X] + H[i][n][X]) / (N1 + N2);
								entropyM -= p * log(p) * invLOG2;
							}
						}
						/* compare first entropies with merged entropy */
						diff = entropyM * (N1 + N2) - entropy1 * N1 - entropy2 * N2;
						perc = 100. * diff / (entropy1 * N1 + entropy2 * N2);
						if (diff < 0  || perc < 0.)
						{
							n=n; /* ups */
						}
					} /* if (N1 && N2 )	*/
					else
					{
						diff = 0;
						perc = 0.;
					}
					
					if (N2 == 0)
					{
						cnt_empty++;
						continue; /* exclude from merging */
					}
					
					/* percentage in changed entropy
					 * the smaller N1, N2 the higher the percentage 	
					 */
					if (N1)
					{
						if (i < 2)
						{
							percent = exp( sqrt( fixed_fac / (N1+N2) ) * 0.05 );
						}
						else if ( i < 4)
						{
							percent = exp( sqrt( fixed_fac / (N1+N2) ) * 0.2 );
						}
						else 
						{
							percent = exp( sqrt( fixed_fac / (N1+N2) ) * 0.3);
						}
					}
					else	percent = 0.0; /* should not happen	*/

					/* plus 0.01 for cases entropy1 == 0.  */
					if ( (N2 == 0) ||  perc < percent )
					{ /* merging accepted	*/
						if (N2 == 0) cnt_empty++;
						continue; 
					}
					else
					{ /* merging not possible	*/
						cnt_segments++;

						/* swap pointer	*/
						{ /* if second distribution is not empty, then make it the 
							 * first distribution for next comparison
							 */
							N1 = N2; 
							entropy1 = entropy2;
							n_last = n;
						}
					}
				} /* for n	*/

				/* synthetic images prefer smaller tolerances 
				 * ==> reduce range by empty slots
				 * other images get higher tolerences
				 */

				if (i <= 1)
				{
					/* relation between non-empty bins and number of segments in histogram	*/
					if (aC->rct_type_used == GREY) 
						aC->HXC2tolerance[comp][i] = ((pd->maxVal_pred[comp] - cnt_empty) / (cnt_segments+1) ) >> 3;
					else
						aC->HXC2tolerance[comp][i] = ((pd->maxVal_pred[comp] - cnt_empty) / (cnt_segments+1) ) >> 2;
				}
				else
				{
					double fac;
					/* factor must be at least equal to one	*/
					fac = exp( 0.25 * i ); /* the higher the level i, the more increase
																			  *  of nodes number is allowed	*/
					if (aC->rct_type_used == GREY) 
						fac = exp( 0.75 * i );
					else
						fac = exp( 0.125 * i );

					if (count_nodes[i] > fac * (count_nodes[0] + count_nodes[1])  &&
						  count_nodes[i] > i * pd->maxVal_pred[comp] /* accounts for images with few colours*/
							)
					{ /* number of nodes at level i is significant higher than at previous level
						 * ==> taking this elment (B..F) into account increases the number of patterns a lot
						 */
						if (aC->rct_type_used == GREY) 
							fac = log( 2.0 * count_nodes[i] /  (count_nodes[0] + count_nodes[1]));
						else
							fac = log( 6.0 * count_nodes[i] /  (count_nodes[0] + count_nodes[1]));
						//fac = log( 8.0 * count_nodes[i] /  (count_nodes[0] + count_nodes[1]));
						//fac *= max( 1., 170. / iF->syn_grad[comp]);
						if (i > 3) fac *= 170. / iF->syn_grad;
						aC->HXC2tolerance[comp][i] = (unsigned int)floor( 
							min( pd->maxVal_pred[comp], 
							    (pd->maxVal_pred[comp] - cnt_empty) * fac / (cnt_segments + 1)) +0.5);
						//if (i < 0)
						//{
						//	//fac = count_nodes[i] * 1. / (fac * count_nodes[i-1]);
						//	fac = count_nodes[i] * 0.5 /  count_nodes[i-1];
						//	//aC->HXC2tolerance[comp][i] = (int)floor( max( 0.5, aC->HXC2tolerance[comp][i-1]) * fac + 0.5);
						//	aC->HXC2tolerance[comp][i] = ((range - cnt_empty) / ((cnt_segments>>1) +1) ) >> 1;
						//}
						//else if (i < 4)
						//{
						//	//aC->HXC2tolerance[comp][i] = (range - cnt_empty) >> 2;
						//	aC->HXC2tolerance[comp][i] = (range - cnt_empty) / ((cnt_segments>>1) +1);
						//}
						//else
						//{
						//	//aC->HXC2tolerance[comp][i] = (range - cnt_empty) >> 1;
						//	aC->HXC2tolerance[comp][i] = (range - cnt_empty) / ( (cnt_segments>>2) +1);
						//}
					}
					else
					{
						aC->HXC2tolerance[comp][i] = 
							((pd->maxVal_pred[comp] - cnt_empty) / (cnt_segments+1)) >> 1;
					}
				}

				fprintf( stderr, "%d, ", aC->HXC2tolerance[comp][i]);
			} /* for i	*/

			if (aC->histMode == COMPACTION)
			{
				/* in COMPACTION mode, the tolerances should typically be somewhat smaller	*/
				for ( i = 0; i < MAX_PATTERN_SIZE; i++)
				{
					aC->HXC2tolerance[comp][i] = max( 0, (long)aC->HXC2tolerance[comp][i]-1);
				}
			}

			/* postprocessing	*/
			/* make tolerances monotonically increasing	*/
			if (aC->HXC2tolerance[comp][0] > aC->HXC2tolerance[comp][1])
			{	/* use smaller value for position A and B	*/
				aC->HXC2tolerance[comp][0] = (aC->HXC2tolerance[comp][0] + 3* aC->HXC2tolerance[comp][1] + 2) >> 2;
				//aC->HXC2tolerance[comp][0] = aC->HXC2tolerance[comp][1];
			}
			else 
			{
				aC->HXC2tolerance[comp][0] = (3 * aC->HXC2tolerance[comp][0] + aC->HXC2tolerance[comp][1] + 2) >> 2;
			}
			aC->HXC2tolerance[comp][1] = aC->HXC2tolerance[comp][0];

			if (aC->HXC2tolerance[comp][4] > aC->HXC2tolerance[comp][5])
			{	/* use greater value for position E and F	*/
				aC->HXC2tolerance[comp][4] = (aC->HXC2tolerance[comp][5] + 3* aC->HXC2tolerance[comp][4] + 2) >> 2;
			}
			else
			{
				aC->HXC2tolerance[comp][4] = (3 * aC->HXC2tolerance[comp][5] + aC->HXC2tolerance[comp][4] + 2) >> 2;
			}
			aC->HXC2tolerance[comp][5] = aC->HXC2tolerance[comp][4];

			if (aC->HXC2tolerance[comp][4] < aC->HXC2tolerance[comp][1])
			{
				aC->HXC2tolerance[comp][4] = aC->HXC2tolerance[comp][1];
				aC->HXC2tolerance[comp][5] = aC->HXC2tolerance[comp][1];
			}

			/* average value	*/
			tol = (aC->HXC2tolerance[comp][2] + aC->HXC2tolerance[comp][3]) >> 1;
			if (tol < aC->HXC2tolerance[comp][1])
			{	/* use greater value for position C and D	*/
				aC->HXC2tolerance[comp][2] = aC->HXC2tolerance[comp][1];
				aC->HXC2tolerance[comp][3] = aC->HXC2tolerance[comp][1];
			}
			else if (tol > aC->HXC2tolerance[comp][4])
			{
				aC->HXC2tolerance[comp][2] = aC->HXC2tolerance[comp][4];
				aC->HXC2tolerance[comp][3] = aC->HXC2tolerance[comp][4];
			}
			else 
			{
				aC->HXC2tolerance[comp][2] = tol;
				aC->HXC2tolerance[comp][3] = tol;
			}
			fprintf( stderr, " ==> ");
			for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			{
				fprintf( stderr, "%d, ", aC->HXC2tolerance[comp][i]);
			}

		}		/* get tolerance values based on comparisons of entropies	*/

#ifdef _DEBUG_X
		if (aC->coding_method == HXC2 && 1)
		{
			fprintf( stdout, "\n total count; %d", count_total );
			fprintf( stdout, "\n X counts: ", count_total );
				for ( X = 0; X <= (signed)pd->maxVal_pred[comp]; X++)
				{
					fprintf( stdout, "%d:%d,", X, count_X[X]);
				}
			{
				int val, max = 99999;
				FILE *out=NULL;
				char filename[512];

				for ( i = 0; i < MAX_PATTERN_SIZE; i++)
				{
					if (aC->histMode == COMPACTION)
						sprintf( filename, "2D_hist_col%03d_c%d_p%d_comp.pgm", aC->rct_type_used, comp, i);
					else
						sprintf( filename, "2D_hist_col%03d_c%d_p%d_nomod.pgm", aC->rct_type_used, comp, i);
					out = fopen( filename, "wt");
					fprintf( out,"P2\n");
					fprintf( out,"# created by %s\n", thisVersion);
					fprintf( out,"%d %d\n", pd->maxVal_pred[comp]+1, pd->maxVal_pred[comp]+1);
					fprintf( out,"%d\n", max);

					for ( pos = 0; pos <= (signed)pd->maxVal_pred[comp]; pos++)
					{
						for ( X = 0; X <= (signed)pd->maxVal_pred[comp]; X++)
						{
							val = H[i][pos][X];
							if (val > max) val = max;
							fprintf( out, "%6d", val);
						}
						fprintf( out,"\n");
					}

					fclose( out);
				}
			}
		}
#endif

endfunc:

		//fprintf( stderr, "\n free tree");
		free_items( tree_root);
		/* must be re-allocated for each component.
		 * as the maxvalues determine the allocated space
		 */
		//fprintf( stderr, "\n free H");
		for ( i = 0; i < MAX_PATTERN_SIZE; i++)
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

		for ( comp = 0; comp < pd->channel; comp++)
		{
			for ( i = 0; i < MAX_PATTERN_SIZE; i++)
			{
				sum += aC->HXC2tolerance[comp][i];
				if (aC->HXC2tolerance[comp][i] == HXC2_TOL_EXCEPTION)
				{
					aC->HXC2tolerance[comp][i]++; /* avoid special number	*/
				}
			}
		}
		if (sum == 0)	aC->HXC2increaseFac = 3;
		else		aC->HXC2increaseFac = 1;
	}
	fprintf( stderr, "\n");
	return;
}



/*---------------------------------------------------------------
*	getHXC2_tolerances_H(XX)()
*
*	  f
*	 cbd
* eax
*
*---------------------------------------------------------------*/
void getHXC2_tolerances( PicData *pd,	AutoConfig *aC)
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

	fprintf( stderr, "\n get HXC2 tolerances ...");

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
			double entropy[MAX_PATTERN_SIZE], p;
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
				for ( n = 0; n < range; n++) /* for all possible values at i position	*/
				{
					for ( X = 0; X < range; X++) /* for all possible values at X position	*/
					{
						if (H[i][n][X]) /* if bin is not empty	*/
						{ /* compute single probability	*/
							p = (double)H[i][n][X] / N;
							entropy[i] -= p * log(p) * invLOG2;
						}
					}
				}
				/* problem NOMOD vs. COMPACTION
				 * stretching the histogram has almost no influence on the entropy
				 * but on the tolerances to be used
				 * correction factor is needed! ==> range / count_bins
				 */
				p = pow( 2., entropy[i]);
				if ( aC->rct_type_used == RGBc)
				{
					if ( i < 2) p = p / 32;
					else if ( i < 4) p = p / 4;
					else p = p * 1;
				}
				else
				{
					if ( i < 2) p = p / 16;
					else if ( i < 4) p = p / 8;
					else p = p / 2;
				}
				aC->HXC2tolerance[comp][i] = min( range, (unsigned int)floor( p) * range / count_bins);
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

		for ( comp = 0; comp < pd->channel; comp++)
		{
			fprintf( stderr, "\n %d: ", comp);
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
		if (sum == 0)	aC->HXC2increaseFac = 3;
		//else		aC->HXC2increaseFac = 1;
	}
	fprintf( stderr, "\n");
	return;
}
