/**********************************************************************
 * File...: k_opt_heuristic.c
 * Purpose: heuristic for optimisation of sinal values order
 * Author.: Tilo Strutz
 * Date:..: 18.05.2015
 * changes:
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <k-opt.h>

/*----------------------------------------------------
 * get_costs_()
 *----------------------------------------------------*/
unsigned long get_costs_( int N, int *C, int *pv)
{
	int i, j;
	unsigned long cost = 0;

	for (i = 0; i < N; i++) /* rows	*/
	{
		//for (j = i+2; j < N; j++)	/* columns	*/
		for (j = i+1; j < N; j++)	/* columns	*/
		{
			/* direkt neighbours (j=i+1) get a weight of zero	*/
			cost += C[ pv[i] * N + pv[j] ] * (j-i);
		}
	}
	return cost;
}

/*----------------------------------------------------
 * get_costs()
 *----------------------------------------------------*/
unsigned long get_costs( int N, int *C, int *pv)
{
	int i, j;
	unsigned long cost = 0;

	for (i = 0; i < N; i++) /* rows	*/
	{
		for (j = i+2; j < N; j++)	/* columns	*/
		{
			/* direkt neighbours (j=i+1) get a weight of zero	*/
			cost += C[ pv[i] * N + pv[j] ] * (j-i-1);
		}
	}
	return cost;
}

/*----------------------------------------------------
 * k_opt_heuristic()
 * tries to find the best tour
 *----------------------------------------------------*/
void k_opt_heuristic( int *permu_v, int optim_mode, int N, int *C, 
										int *duration, unsigned long *bestCosts, int *inbest_trial, 
										int *innumTrials)
{
	// set defaults for resulting variables
	int i, r;
	int best_trial = 1;
	int *bestOrder; /* remember the best tour so far	*/
	int numTrials = -1;
	int random_flag = 2; // start without seed
	unsigned long actualCosts;

	int clock_start;
	int clock_stop;
	time_t t;
	time(&t);
	srand((unsigned int)t);

	*duration = -1;
	// bestOrder = (int*) malloc((N+1) * sizeof(int));
	CALLOC( bestOrder, N, int);
	assert(bestOrder != NULL);
	/* start with original order	*/
	for (i = 0; i < N; i++)
	{
		bestOrder[i] = i;
	}

	/* compute initial path length	*/
	clock_start = clock(); // save current time

	/* pertubation of tour double bridge	*/

	if (random_flag != 0)
	{
	//	numTrials = (int)(96 * log10(argN));
		numTrials = (int)(596 * log10( (double)N));
		/* if N is too small there is a problem to initiate 
		 * the double bridge permutation
		 */
		if (N < 10) numTrials = 2;
	}
	numTrials = 1; /* one pass must be enough */

	/* for all trials	*/
	for ( r = 1; r <= numTrials; r++)
	{
		//fprintf( stderr, "\r           trial %4d/%4d   ", r, numTrials);
		if ((r != 1) && (random_flag != 0) && N >= 7 /* save guard	*/)
		{
			int idx_v[5];
			int stop = 0;
			int j, len_seg1, *seg1, len_seg2, *seg2, len_seg3, *seg3;
			int first, last;

			/* determine four positions for the double bridge permutation	*/
			while (stop == 0)
			{
				int *perm_v, ii;

				CALLOC( perm_v, N, int);

				randperm( N, perm_v);

				/* take first four cities	*/
				for (ii = 0; ii < 4; i++)
				{				
					idx_v[ii] = perm_v[ii];
				}
				// sort cities
				for (ii = 0; ii < 4; ii++)
				{
					for (j = 0; j < 3; j++)
					{
						if (idx_v[j] > idx_v[j+1])
						{
							int tmp = idx_v[j+1];
							idx_v[j+1] = idx_v[j];
							idx_v[j] = tmp;
						}
					}
				}
				/* minimum tour distance	*/
				if (idx_v[1] < (idx_v[0] + 2))
				{				
					idx_v[1] = idx_v[0] + 2;
				}
				if (idx_v[2] < (idx_v[1] + 2))
				{				
					idx_v[2] = idx_v[1] + 2;
				}
				if (idx_v[3] < (idx_v[2] + 2))
				{				
					idx_v[3] = idx_v[2] + 2;
				}
				if (idx_v[3] < N)
				{				
					stop = 1;
				}
				FREE( perm_v);
			}
			/* pertubation by double bridge
			 * two segments are interchanged with a third segment in between
			 * which keeps its position
			 * copy of 1st segment in native order
			*/

			i = idx_v[0];
			j = idx_v[1]-1;
			
			first = (i % N);
			last  = j;
			len_seg1 = get_seg_length( N, first, last);
			CALLOC( seg1, len_seg1, int);


			/* if i == j then the segment length is one */
			get_segment( /*N,*/ permu_v, first, seg1, len_seg1);

			/* copy of 2nd segment in native order */
			i = idx_v[2];
			j = idx_v[3]-1;
			first = ((i-1) % N)+1;
			last  = j;
			len_seg2 = get_seg_length( N, first, last);
			CALLOC( seg2, len_seg2, int);

			get_segment( /*N,*/ permu_v, first, seg2, len_seg2);

			// copy of 3rd segment in native order
			i = idx_v[3];
			j = idx_v[4]-1;
			first = ((i-1) % N)+1;
			last  = j;
			len_seg3 = get_seg_length( N, first, last);
			CALLOC( seg3, len_seg3, int);
			get_segment( /*N,*/ permu_v, first, seg3, len_seg3);

			//insert segments
			i = idx_v[1];
			put_segment( /*N,*/ permu_v, ((i-1) % N)+1, seg3, len_seg3, 0);
			i = i + len_seg3;
			put_segment( /*N,*/ permu_v, ((i-1) % N)+1, seg2, len_seg2, 0);
			i = i + len_seg2;
			put_segment( /*N,*/ permu_v, ((i-1) % N)+1, seg1, len_seg1, 0);

			FREE( seg1);
			FREE( seg2);
			FREE( seg3);
		}/* end if */

		actualCosts = *bestCosts;
		//  optimisation
		if (optim_mode == 2)
		{
			actualCosts = two_opt_heuristic( N, C, permu_v, *bestCosts);
		}
		

		/* compute total costs */
		//actualCosts = get_costs( N, C, permu_v);
		if (*bestCosts > actualCosts)
		{
			*bestCosts = actualCosts;
			for ( i = 0; i < N; i++)
			{
				bestOrder[i] = permu_v[i];
			}
			best_trial = r;
		}
		else
		{
			int ii;

			for (ii = 0; ii < N; ii++)
			{
				permu_v[ii] = bestOrder[ii]; // reset to best tour
			}
		}
		fprintf( stderr, "trial %4d/%4d   actualCosts: %d; bestTrial: %4d, bestCosts: %d ", 
			r, numTrials, actualCosts,
			best_trial, *bestCosts); 
	}// for r = 1:numOfTrials

	clock_stop = clock();
	*duration = clock_stop - clock_start; // measure the elapsed time

	//Parameter Rückgabe
	*inbest_trial = best_trial;
	*innumTrials = numTrials;

	FREE( bestOrder);

	return;
}
