/*****************************************************************
*
* File..:	context_quant.c
* Descr.:	Context quantisation based on VQ (LBG algorithm)
* Author.:	Tilo Strutz
* Date..: 	02.09.2014 (based on contexts.c (CoBaLP2)
* changes:
* 21.10.2014 reduce number of contexts, when number of colours is low
* 21.10.2014 stop, when number of empty cells cannot be reduced
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "stats.h" /* required for heap_sort	*/
#include "imageFeatures.h" 
#include "hxc.h"	/* for MAX_PATTERN_SIZE	*/
#include "codec.h"	/* for min() */

#define MAX_NUM_ITERATIONS	100
#define MAX_NUM_CONTEXTS	200
#define NUM_VECTOR_ELEMS (MAX_PATTERN_SIZE +1) /* including X	*/
#define EPS 1e-2
#ifdef _DEBUG
//#define px_verbose 1
#define px_verbose 1
#else
#define px_verbose 0
#endif


/*---------------------------------------------------------------
*	contextQuant()
*
*	  f
*	 cbd
* eax
*
*---------------------------------------------------------------*/
void contextQuant( PicData *pd, unsigned int numColours,
									AutoConfig *aC, int exhaustive_flag)
{
	int k, i, best_cell;
	int convergence_flag, empty_cell_flag;
	int max_flag = 0;
	int max_distance_k;
	int max_k;
	int d[NUM_VECTOR_ELEMS];	/* offsets for pixel addressing	*/
	double max_distance;
	double sum_distance_cell[MAX_NUM_CONTEXTS]; /* sum of all absolute distances to this cell */
	double sumDev[4][NUM_VECTOR_ELEMS];
	double sumDev2[4][NUM_VECTOR_ELEMS];
	unsigned int count;
	long max_count;	/* highest frequency of all cells	*/
	long r, c, iter;
	long size;
	double diff;
	double min_dist, dist;
	double max_of_min_dist, mean_of_min_dist;
	double sum_distance, sum_distance_old;
	/* cells for vector quantisation of texture	*/
	double cell[MAX_NUM_CONTEXTS][4][NUM_VECTOR_ELEMS]; /* four components or each position, incl. X	*/
	double sum_cell[MAX_NUM_CONTEXTS][4][NUM_VECTOR_ELEMS];;
	unsigned short num_empty_cells, num_empty_cells_old, empty_cnt;
	unsigned short num_contexts; /* num of cells, contexts	*/
	unsigned int comp;
	unsigned long pos, numm = 0, n;

	unsigned long count_cell[MAX_NUM_CONTEXTS];	/* counts the usage of cells	*/

	assert( pd->width > 3);
	assert( pd->height > 3);

	fprintf( stderr, "\n context quantisation ...");

	/* offsets	*/
	d[0] = 0;			/* X	*/
	d[1] = -1;		/* A	*/
	d[2] =    - (long)pd->width;	/* B	*/
	d[3] = -1 - (long)pd->width;	/* C	*/
	d[4] = +1 - (long)pd->width;	/* D	*/
	d[5] = -2;										/* E	*/
	d[6] =    - 2*(long)pd->width;	/* F	*/

	/* determine number of contexts (cells)	and number of training vectors
	* assume 32 vectors per cell for statistical relevance
	*/
	size = (pd->width - 2) * (pd->height - 2);
	if (exhaustive_flag) 
	{ /* use all positions as training vector	*/
		numm = size;
	}
	else 
	{
		numm = size >> 5; /* take 3.125 per cent of all pixels	*/
	}

	num_contexts = (unsigned short)min( MAX_NUM_CONTEXTS, numm >> 5);
	/* if low number of colours then reduce the number of contexts,
	 * speeding up the processing
	 */
	if (num_contexts > pow( 3., (int)numColours) ) 
	{
		num_contexts = (unsigned short)pow( 3., (int)numColours);
	}

	if (num_contexts < 4)
	{
		num_contexts = 4; /* at least four contexts	*/
		numm = min( size, num_contexts << 5);
	}
	fprintf( stderr, "\n     %d contexts", num_contexts);

	/* initialize cells with something  */
	for ( k = 0; k < num_contexts; k++)
	{
		if (px_verbose)
		{
			fprintf( stdout, "\n");
		}
		/* for all components	*/
		for ( comp = 0; comp < pd->channel; comp++)
		{
			/* for all elements	*/
			for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
			{
				cell[k][comp][i] =  (k + i * 3) % (pd->maxVal_pred[comp] + 1);
				if (px_verbose)
				{
					fprintf( stdout, " %8.4f", cell[k][comp][i]);
				}
			}
		} /* for comp	*/
	}

	/* reset variables */
	convergence_flag = 0;
	sum_distance_old = 1e9;
	iter = 0;
	num_empty_cells_old = 0;
	empty_cnt = 5;

	/*
	* ##############################################################################
	*/
	while (!convergence_flag)
	{
		iter++;	/* counts number of iterations	*/
		/*
		 * reset cell parameters
		 */
		for ( k = 0; k < num_contexts; k++)
		{
			count_cell[k] = 0;	/* number of assignements	*/
			sum_distance_cell[k] = 0.; /* sum of all absolute distances to this cell */
			/* for all components	*/
			for ( comp = 0; comp < pd->channel; comp++)
			{
				/* for all elements	*/
				for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
				{
					sum_cell[k][comp][i] = 0.;
				}
			}
		}
		max_of_min_dist = 0.;
		mean_of_min_dist = 0;

		empty_cell_flag = 0; /* will be set to 1 if at least one cell is not
												 * assigned to any training vector
												 */
		/* reset accumulated absolute difference (between
		* cells and data vectors)
		*/
		sum_distance = 0; /* for convergence test	*/

		if (!exhaustive_flag) srand( 333); /* seeds pseudorandom generator	*/

		/*--------------------------------------------------
		* assign all training vectors to cells
		*/
		for ( n = 0; n < numm; n++)
		{
			/* select a training vector position	*/
			if (exhaustive_flag) 
			{
				r = 2 + n / (pd->width - 3);
				c = 1 + n % (pd->width - 2);	
			}
			else
			{
				/* random selection of a column and a row
				* avoid positions at the top, left or right
				* consider extended array, which should be filled with reasonable values
				*/
				r = 2 + rand()* (pd->height - 4) / RAND_MAX;
				c = 1 + rand()* (pd->width - 3 ) / RAND_MAX;	
			}

			pos = c + r * pd->width; /* get final position	*/

			/* look for best matching cell */
			min_dist = 99999999;
			best_cell = 0;

			/* compare to all cells	*/
			for ( k = 0; k < num_contexts; k++)
			{
				dist = 0;
				/* for all components	*/
				for ( comp = 0; comp < pd->channel; comp++)
				{
					/* for all elements	*/
					for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
					{
#ifdef EUCLID_DIST
						diff = cell[k][comp][i] - pd->data[comp][pos + d[i]];
						dist += diff * diff; 
#else
						/* city-block distance	*/
						dist += fabs( cell[k][comp][i] - pd->data[comp][pos + d[i]] );
#endif
					} /* for i	*/
				} /* for comp	*/
				if ( min_dist > dist)
				{
					/* copy best cell	*/
					min_dist = dist;
					best_cell = k;
				}
			} /* for ( k = 0; */

			/* remember extreme cases for increasing the cell number	*/
			if (max_of_min_dist < min_dist) 
				max_of_min_dist = min_dist;

			/* used for computation of averaged distance:	*/
			sum_distance_cell[best_cell] += min_dist; 

			/* increment count of selected cell	*/
			count_cell[best_cell]++;

			/* accumulate vectors belonging to best cell	*/
			/* for all components	*/
			for ( comp = 0; comp < pd->channel; comp++)
			{
				for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
				{
					sum_cell[best_cell][comp][i] += pd->data[comp][pos + d[i]];
				}
			}
			/* accumulate absolute differences for convergence test	*/
			sum_distance += min_dist;
		} /* for ( n, all training data	*/

		if (px_verbose)
		{
			fprintf( stdout, "\n iter=%d", iter);
		}

		/* look for cell with highest diversity of assigned training vectors	*/
		max_distance = 0.;
		max_distance_k = 0;
		/* look also for highest count */
		max_count = 0;
		max_k = 0;
		/*
		* compute new cells as average of training vectors 
		* belonging to the cell
		*/

		num_empty_cells = 0;

		for ( k = 0; k < num_contexts; k++) /*  */
		{
			if (px_verbose)
			{
				fprintf( stdout, "\nk=%3d, %6d,", k, count_cell[k]);
			}
			if (count_cell[k]) /* exclude unused cells	*/
			{
				/* for all components	*/
				for ( comp = 0; comp < pd->channel; comp++)
				{
					/* for all elements	*/
					for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
					{
						/* compute average of all assigned vectors	*/
						cell[k][comp][i] = sum_cell[k][comp][i] / count_cell[k]; /* new centre of cell	*/
						if (px_verbose)
						{
							fprintf( stdout, " %7.3f", cell[k][comp][i]);
						}
					}
				}
				/* look for highest diversity	*/
				if ( max_distance < sum_distance_cell[k] / count_cell[k])
				{
					max_distance = sum_distance_cell[k] / count_cell[k];
					max_distance_k = k;	/* remember the cell	*/
				}
				/* look for highest count	*/
				if ( max_count < (signed)count_cell[k])
				{
					max_count = count_cell[k];
					max_k = k;	/* remember the cell	*/
				}
			} /* if (count_cell[k]) */
			else num_empty_cells++;
		} /* for ( k */


		/* save guard, 	*/
		if (iter >= MAX_NUM_ITERATIONS) 
		{
			convergence_flag = 1;
		}
		else
		{
			/* re-initialise not-assigned cells	*/
			/* 
			* set all unused cells close to the one 
			* with highest count or diversity 
			*/
			if (px_verbose)
			{
				fprintf( stdout, "\n modified cells");
			}
			for ( k = 0; k < num_contexts; k++) /* */
			{
				double max_element = 0.;
				if (count_cell[k]) continue; /* only for unused cells		*/

				empty_cell_flag = 1; /* at least one empty cell	*/
				if (px_verbose)
				{
					fprintf( stdout, "\n k=%d, %6d,", k, count_cell[k]);
				}
				/* for all elements copy from most used/diverse one	*/
				for ( comp = 0; comp < pd->channel; comp++)
				{
					for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
					{
						/* mode of re-init has almost no influence	*/
						if ( max_flag)
						{
							cell[k][comp][i] = cell[ max_distance_k][comp][i]; /* cell with highest error	*/
						}
						else
						{
							cell[k][comp][i] = cell[ max_k][comp][i];		/* cell with highest count*/
						}
						if (px_verbose)
						{
							fprintf( stdout, " %7.3f", cell[k][comp][i]);
						}
						if (max_element < fabs( cell[k][comp][i])) max_element = fabs( cell[k][comp][i]);
					}
				} /* for comp	*/
				/* select one element (based on k+iter) and disturb it	*/
				if ( (k+iter) % 3 == 0)
				{
					/* slight positive variation	*/
					//cell[k][(k+iter)%NUM_VECTOR_ELEMS] += 1 + 10.0*(k + 1) / num_contexts; 
					cell[k][k%pd->channel][(k+iter)%NUM_VECTOR_ELEMS] += 
						1 + max_element*(k + 1) / num_contexts; 
				}
				else if ( (k+iter) % 3 == 1)
				{
					/* slight negative variation	*/
					cell[k][k%pd->channel][(k+iter)%NUM_VECTOR_ELEMS] -= 
						1 + max_element * (k + 1) / num_contexts;
					/* worsens results for old .u. and .v. data	:-( */
				}
				else
				{
					int k1, k2;
					double fac;
					fac = (double)rand() / RAND_MAX;
					k1 = rand()* (num_contexts - 1) / RAND_MAX;
					do
					{
						k2 = rand()* (num_contexts - 1) / RAND_MAX;
					} while (k1 == k2);
					for ( comp = 0; comp < pd->channel; comp++)
					{
						for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
						{
							/* kombine with another cell	*/
							cell[k][comp][i] = fac * cell[k1][comp][i] + (1-fac) * cell[k2][comp][i];
						}
					} /* for comp	*/
				} /* (k+iter) % 3  > 1	*/

				if (px_verbose)
				{
					fprintf( stdout, "; %8.4f (%d)", cell[k][0][k%NUM_VECTOR_ELEMS], k%NUM_VECTOR_ELEMS);
				}
			} /* for ( k = 0	*/
		} /* if (iter <= MAX_NUM_ITERATIONS	*/

		if (px_verbose)
		{
			fprintf( stdout, "\n");
			fprintf( stdout, "sumdiff: %f\n", (sum_distance_old - sum_distance) / sum_distance);
		}
		/* check the convergence criterion	*/
		if (sum_distance)
		{
			if ( ((fabs(sum_distance_old - sum_distance) / sum_distance) < EPS) && iter > 5)
			{
				/* go ahead as long as there are too many unassigned cells	*/
				if (num_empty_cells * 5 <  4 * num_contexts)	convergence_flag = 1;
				else if (num_empty_cells  == num_empty_cells_old)
				{
					/* decrement as long empty cells cannot be filled (too less different patterns)*/
					empty_cnt--;
					if (!empty_cnt) convergence_flag = 1;
				}
				else empty_cnt = 5;
			}
		}
		else
		{
				if (!empty_cell_flag)	convergence_flag = 1;
				else if (num_empty_cells  == num_empty_cells_old)
				{
					/* decrement as long empty cells cannot be filled (too less different patterns)*/
					empty_cnt--;
					if (!empty_cnt) convergence_flag = 1;
				}
				else empty_cnt = 5;
		}
		sum_distance_old = sum_distance;
		num_empty_cells_old  = num_empty_cells;

		fprintf( stderr, "\r%d%%", 100 * (iter) / MAX_NUM_ITERATIONS);
		max_flag = !max_flag; /* use opposite next round	*/
	}	/* while !convergence_flag)	*/
	/*
	* ##############################################################################
	*/


	/* get deviation of vector elements at different positions	*/
	/* reset */
	for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
	{
		for ( comp = 0; comp < pd->channel; comp++)
		{
			sumDev[comp][i] = 0.;
			sumDev2[comp][i] = 0.;
		}
	}

	/*--------------------------------------------------
	* assign all training vectors
	* due to random selection, the vectors are different ones 
	* compared to the clustering process
	*/

  /* determine standard deviation only for the cell with highest count
	 */
	max_count = 0;
	max_k = 0;
	/* get cell	*/
	for ( k = 0; k < num_contexts; k++)
	{
		if ( (unsigned)max_count < count_cell[k])
		{
			max_count = count_cell[k];
			max_k = k;
		}
	}

	count = 0;
	for ( n = 0; n < numm; n++)
	{
		if (exhaustive_flag) 
		{
			r = 2 + n / (pd->width - 3);
			c = 1 + n % (pd->width - 2);	
		}
		else
		{
			/* random selection of a column and a row
			* avoid positions at the top, left or right
			* consider extended array, which should be filled with reasonable values
			*/
			r = 2 + rand()* (pd->height - 4) / RAND_MAX;
			c = 1 + rand()* (pd->width - 3 ) / RAND_MAX;	
		}

		pos = c + r * pd->width; /* get final position	*/

		/* Compute the differences */

		/* look for best matching cell */
		min_dist = 99999999;
		best_cell = 0;

		/* compare to all cells	*/
		for ( k = 0; k < num_contexts; k++)
		{
			dist = 0;
			/* for all components	*/
			for ( comp = 0; comp < pd->channel; comp++)
			{
				/* for all elements	*/
				for ( i = 0; i < NUM_VECTOR_ELEMS; i++)
				{
#ifdef EUCLID_DIST
					diff = cell[k][comp][i] - pd->data[comp][pos + d[i]];
					dist += diff * diff; 
#else
					/* city-block distance	*/
					dist += fabs( cell[k][comp][i] - pd->data[comp][pos + d[i]] );
#endif
				} /* for i	*/
			} /* for comp	*/
			if ( min_dist > dist)
			{
				/* copy best cell	*/
				min_dist = dist;
				best_cell = k;
			}
		} /* for ( k = 0; */

		/* average over all cells and vectors	*/
		//if (count_cell[best_cell] > 10) /* count of the training process	*/

		/* only for cell with maximum assignements	*/
		if (best_cell == max_k) /* count of the training process	*/
		{
			for ( i = 1; i < NUM_VECTOR_ELEMS; i++) /* exclude X	*/
			{
				for ( comp = 0; comp < pd->channel; comp++)
				{
					diff = cell[best_cell][comp][i] - pd->data[comp][pos + d[i]];
					sumDev[comp][i-1] += fabs( diff );
					sumDev2[comp][i-1] += diff * diff;
				}
			}
			count++;
		} /* if count > 10 */
	} /* for n */

	{
		double facn = 1./count;
		//double facC = 0.01 * iF->numColours / num_contexts;
		for ( comp = 0; comp < pd->channel; comp++)
		{
			fprintf( stderr, "\n  %d:", comp);
			for ( i = 0; i < NUM_VECTOR_ELEMS - 1; i++) /* exclude X	*/
			{
				double meanDiff, meanSqDiff;

				meanDiff = sumDev[comp][i] * facn;
				meanSqDiff = sumDev2[comp][i] * facn;
				aC->HXC2tolerance[comp][i] = (unsigned int)(sqrt(meanSqDiff - meanDiff * meanDiff) + 0.5);
				aC->HXC2tolerance[comp][i] = max( 0, aC->HXC2tolerance[comp][i]);
				fprintf( stderr, "%d, ", aC->HXC2tolerance[comp][i]);
			}
		}
		fprintf( stderr, "\n");
		//for ( ; i < NUM_VECTOR_ELEMS-1; i++)
		//{
		//	for ( comp = 0; comp < pd->channel; comp++)
		//	{
		//		aC->HXC2tolerance[comp][i] = aC->HXC2tolerance[comp][i-1];
		//	}
		//}
	}
	return;
}


