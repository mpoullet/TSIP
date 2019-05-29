/*****************************************************
* File..: permute_histogram.c
* Desc..: permutation of values
* Author: T. Strutz
* Date..: 11.11.2013
* 01.04.2014 bugfix length must be at least 3
* 08.05.2015 new permute_histogram_kopt()
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "codec.h"
#include "ginterface.h"
#include "types.h" /* for CALLOC*/
#include "k-opt.h"

/*--------------------------------------------------------
* permute_histogram()
*--------------------------------------------------------*/
unsigned int permute_histogram( PicData *pd, 
															 int comp,
															 unsigned int length,
															 unsigned int *idx2val,
															 unsigned int *val2idx)
{
	int test_flag = 0, deltaCost;
	unsigned int maxVal, exchange_flag, maxIdx;
	unsigned long i, j, k, **aM=NULL, * aMval = NULL;
	unsigned int idx;
	unsigned long pos;
	unsigned long cost, initCosts, cost_old, cost_old2;

	if (length < 3)
	{
		/* permutation does not make sense	*/
		return 0;
	}

	/* number of different values in component */
	maxVal = length - 1;

	/* if compaction was done, the vector is already initialised	*/
	/* for (i = 0; i < maxVal+1; i++) idx2val[i] = i;	*/

	/* double indexed array for adjacency matrix	*/
	/* array of pointers	*/
	CALLOC( aM, maxVal, unsigned long*);
	/* 1D array of values	*/
	CALLOC( aMval, (maxVal+1)*(maxVal)/ 2, unsigned long);

	idx = 0;
	for (i = 0; i < maxVal; i++)
	{
		/* array for frequency of neighbourhoods of i	*/
		//CALLOC( aM[i], maxVal-i, unsigned long);
		/* get pointers to parts of the values array	*/
		aM[i] = &(aMval[idx]);
		idx = idx + (maxVal-i);
	}

	/* adjacency matrix (i,j) is only allocated for j>i
	 * i\j 0 1 2 3 4
	 * 0 | - * * * *
	 * 1 |   - * * *
	 * 2 |     - * *
	 * 3 |       - *
	 * 4 |         -
	 */

	if (test_flag)
	{
		for ( i = 0; i < maxVal; i++)
		{
			for ( j = 0; j < maxVal-i; j++)
			{
				aM[i][j] = (i+1)*(j+1) % 9;
			}
		}
		aMval[0] = 2;
		aMval[1] = 2;
		aMval[2] = 3;
		aMval[3] = 2;
		aMval[4] = 2;

		aMval[5] = 1;
		aMval[6] = 2;
		aMval[7] = 0;
		aMval[8] = 8;

		aMval[9] = 5;
		aMval[10] = 8;
		aMval[11] = 2;

		aMval[12] = 2;
		aMval[13] = 0;
		aMval[14] = 2;
	}
	else
	{
		/* count neighbourhoods	*/
		//*max_val = pd->data[comp][0];
		for ( i = 1; i < pd->size; i++)
		{
			unsigned int v1, v2;
			v1 = val2idx[ pd->data[comp][i] ]; /* compensate for compaction	*/
			v2 = val2idx[ pd->data[comp][i-1]];
			if (v1 < v2)
			{
				aM[v1][v2-v1-1]++;
			}
			else if (v1 > v2)
			{
				aM[v2][v1-v2-1]++;
			}
			//if (*max_val < v1) *max_val = v1;
		}
	}

	/* compute initial cost	*/
	initCosts = 0;
	for (i = 0; i < maxVal; i++) /* rows	*/
	{
		for (j = 0; j < maxVal - i; j++)	/* columns	*/
		{
			initCosts = initCosts + aM[i][j] * (j + 1);
		}
	}
	cost = cost_old = cost_old2 = initCosts;

	/* bubble sort	*/
	for (k = 0; k <= maxVal; k++)
	{
    exchange_flag = 0;
		/* bubble i and (i+1) one round	*
		 * i\j 0 1 2 3 4
		 * 0 | - * * * *
		 * 1 |   - * * *
		 * 2 |     - * *
		 * 3 |       - *
		 * 4 |         -
		 */
		for (i = 0; i < maxVal; i++)
		{
			deltaCost = 0;
      /* check success of exchange column-wise	*/
			for (j = 0; j < i; j++) /* is a row	*/
			{
         deltaCost = deltaCost + aM[j][i-j-1] - aM[j][i-j];
			}
      /* check success of exchange row-wise	*/
      for (j = 0; j < maxVal-i - 1; j++)  /* is a column */
			{
        deltaCost = deltaCost - aM[i][j+1] + aM[i+1][j];
			}

			if (deltaCost <= 0)
			{
				long tmp;
				exchange_flag = 1;
        /*  exchange values	in permutation vector */
        tmp = idx2val[i];
        idx2val[i] = idx2val[i+1];
        idx2val[i+1] = tmp;

        /*  exchange values	in matrix columns */
				for (j = 0; j < i; j++) /* is a row	*/
				{
					long tmpL;
					tmpL = aM[j][i-1-j];
					aM[j][i-1-j] = aM[j][i-j];
					aM[j][i-j] = tmpL;
				}
        /*  exchange values	in matrx rows */
				for (j = 0; j < maxVal-1-i; j++)  /* is a column */
				{
					long tmpL;
					tmpL = aM[i][j+1];
					aM[i][j+1] = aM[i+1][j];
					aM[i+1][j] = tmpL;
				}
			} /* if (deltaCost < 0)*/
		} /* for (i	*/
		
		/* check success exchanging first row and last column	
		 * i\j 0 1 2 3 4
		 * 0 | - * * * *
		 * 1 |   - * * *
		 * 2 |     - * *
		 * 3 |       - *
		 * 4 |         -
		 */
		deltaCost = 0;
		for (j = 0; j < maxVal-1; j++) /* 	*/
		{
       deltaCost = deltaCost + ( (maxVal-1-j) - (j + 1) ) * aM[0][j];
			 deltaCost = deltaCost + ( (j + 1) - (maxVal-1-j) ) * aM[j+1][maxVal-2-j];
		}
		if (deltaCost <= 0)
		{
			long tmp;
			exchange_flag = 1;
      /*  exchange values	in permutation vector */
      tmp = idx2val[0];
      idx2val[0] = idx2val[maxVal];
      idx2val[maxVal] = tmp;

      /*  exchange values	in matrix columns */
			for (j = 0; j < maxVal-1; j++) /* 	*/
			{
				long tmpL;
				tmpL = aM[0][j];
				aM[0][j] = aM[j+1][maxVal-2-j];
				aM[j+1][maxVal-2-j] = tmpL;
			}
		} /* if (deltaCost < 0)*/

		/* determine actual cost	*/
		cost_old2 = cost_old;
		cost_old = cost;
		cost = 0;
		for (i = 0; i < maxVal; i++) /* rows	*/
		{
				for (j = 0; j < maxVal - i; j++)	/* columns	*/
				{
						cost = cost + aM[i][j] * (j + 1);
				}
		}
#ifdef Q_DEBUG
		fprintf( stderr, "\n %03d: cost: %d", k, cost);
#endif
		if (exchange_flag == 0)
		{
				break;
		}
		if (cost_old2 == cost)
		{
				break;
		}
	} /* for (k	*/


	/* do the permutaion	*/
	maxIdx = 0;
	if ( 105 * cost < 100 * initCosts)  /* percentage 5% */
	{
		for (i = 0; i < maxVal + 1; i++)
		{
			val2idx[ idx2val[i]] = i;
		}
		for ( pos = 0; pos < pd->size; pos++)
		{
			assert( val2idx[ pd->data[comp][pos] ] <= maxVal);
			pd->data[comp][pos] = val2idx[ pd->data[comp][pos] ];
			if (maxIdx < pd->data[comp][pos]) 
				maxIdx = pd->data[comp][pos];
		}
		maxIdx++; /* length of array is needed	*/
	}

	/* free allocated memory	*/
	FREE( aM);
	FREE( aMval);

	return maxIdx;
}

/*--------------------------------------------------------
* restore_histogram_permute()
*--------------------------------------------------------*/
void restore_histogram_permute( PicData *pd, 
											 int comp,
											 unsigned int *idx2val)
{
	unsigned int maxVal = 0;
	unsigned long pos;

	for ( pos = 0; pos < pd->size; pos++)
	{
		//assert( pd->data[comp][pos] < idx2val_len);
		assert( pd->data[comp][pos] >= 0);
		pd->data[comp][pos] = idx2val[ pd->data[comp][pos] ];
	}

	/* restore maxVal in array	*/
	for ( pos = 0; pos <= pd->maxVal_rct[comp]; pos++)
	{
		if (maxVal < idx2val[pos]) maxVal = idx2val[pos];
	}
	pd->maxVal_rct[comp] = maxVal;
}



/*--------------------------------------------------------
* permute_histogram_kopt()
*--------------------------------------------------------*/
unsigned int permute_histogram_kopt( PicData *pd, 
															 int comp,
															 unsigned int length,
															 unsigned int *idx2val,
															 unsigned int *val2idx)
{
	unsigned int maxIdx;
	unsigned long i, N;
	unsigned int idx;
	unsigned long pos;
	unsigned long bestCosts;
	//Variableninitialsierung
	int optim_mode, duration, best_trial, numTrials, counter;
	int *permu_v = NULL, *perm2idx = NULL;		/* permutation vector	*/
	int **C = NULL, *C1 = NULL;				/* matrix of distances	*/

	if (length < 3)
	{
		/* permutation does not make sense	*/
		return 0;
	}

	//Variablendeklaration
	optim_mode = 2; /* 2-opt heuristic	*/

	duration = -2;
	best_trial = -2;
	numTrials = -2;
	counter = 0;

	/* number of different values in component */
	N = length;


	/* if compaction was done, the vector is already initialised	*/
	/* for (i = 0; i < N+1; i++) idx2val[i] = i;	*/


	/* adjacency matrix (i,j)	*/
	CALLOC( C, N, int*);
	/* 1D array of values	*/
	CALLOC( C1, N*N, int);
	
	idx = 0; 
	for ( i = 0; i < N; i++)
	{
		/* get pointers to parts of the values array	*/
		C[i] = &(C1[idx]);
		idx = idx + N;
	}

	/* count neighbourhoods	*/
	/* distance between cities is invers to counts	!!! */
	for ( pos = 1; pos < pd->size; pos++)
	{
		unsigned int v1, v2;
		v1 = val2idx[ pd->data[comp][pos]  ]; /* compensate for compaction	*/
		v2 = val2idx[ pd->data[comp][pos-1]];
		C[v1][v2]++;
		if (v1 != v2) /* on the main diagonal of the square matrix	*/
			C[v2][v1]++;
	}


	
	CALLOC( permu_v, N, int);
	CALLOC( perm2idx, N, int);

	/* start with original order	*/
	for (i = 0; i < N; i++)
	{
		permu_v[i] = i;
	}
	bestCosts = get_costs( N, C1, permu_v);

	fprintf( stderr, "\n initial costs: %d  ", bestCosts);

	k_opt_heuristic( permu_v, optim_mode, N, C1, &duration, 
												&bestCosts, &best_trial, &numTrials);

	printf( "\n %5d", N);
	printf( " %9d", duration);
	printf( " %5d of %5d", best_trial, numTrials);
	printf( " %7d", bestCosts);
	printf( " optim_mode: %d", optim_mode);
	printf("\t PERMUTATION: ");
	//    int i;
	for (i = 0; i < N; i++)
	{
		printf("%d", permu_v[i]);
		printf(" ");
	}
	printf( "\n");


	/* copy permutation vector into C style vector	*/
	for (i = 0; i < N; i++)
	{
		/* idx2val cannot be a copy of permu_v[i] because permu_v contains numbers
		 * startung from zero to maxVal and
		 * idx2val must contain the original data e.g. after RCT
		 */
		//idx2val[i] = permu_v[i];
		perm2idx[i] = idx2val[ permu_v[i] ];
	}
	for (i = 0; i < N; i++)
	{ /* copy back	*/
		idx2val[i] = perm2idx[i];
	}


	/*
	 * do the permutaion
	 */
	maxIdx = 0;
	//if ( 101 * cost < 100 * initCosts)  /* percentage 1% */
	{
		for (i = 0; i < N; i++)
		{
			val2idx[ idx2val[i]] = i;
		}
		for ( pos = 0; pos < pd->size; pos++)
		{
			assert( val2idx[ pd->data[comp][pos] ] < N);
			pd->data[comp][pos] = val2idx[ pd->data[comp][pos] ];
			if (maxIdx < pd->data[comp][pos]) 
				maxIdx = pd->data[comp][pos];
		}
		maxIdx++; /* length of array is needed	*/
	}


	/* free allocated memory	*/
	FREE( permu_v);
	FREE( permu_v);
	FREE( C);
	FREE( C1);

	return maxIdx;
}
