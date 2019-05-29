/*****************************************************************
 *
 * File..:	contextsLS.c
 * Descr.:	Contexts for linear prediction based on VQ (LBG algorithm)
 * Author.:	Tilo Strutz
 * Date..: 	14.08.2018 copy from contexts.c
 * changes:
 * 14.08.2018 no differences, direct use of signal values
 * 16.08.2018 use of signal values minus the mean
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "predictor2.h"
#include "contexts.h"
#include "stats.h" /* required for heap_sort	*/
#include "matrix_utils.h"

#define EPS 1e-4
#ifdef _DEBUG
	 #define px_verbose 1
	//#define px_verbose 0
#else
	#define px_verbose 0
#endif

/* knots (cells) for vector quantisation of texture	*/
double knLS[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM_LS]; /* required in encoder.c	*/

#define OK_COMP
//#undef OK_COMP

/*---------------------------------------------------------------
 *	get_meanOfPattern()
 *---------------------------------------------------------------*/
long get_meanOfPattern( TEMPLATE *templ)
{
	//return (templ->R[0] + templ->R[1] + 1) >> 1;

	//return (templ->R[0] + templ->R[1] + 
	//	      templ->R[2] + templ->R[3] + 2) >> 2;

	//return (templ->R[0] + templ->R[1] + 
	//	      templ->R[0] + templ->R[1] + 
	//	      templ->R[2] + templ->R[3] + 3) / 6;

	//return (templ->R[0] + templ->R[1] + 
	//	      templ->R[2] + templ->R[3] + 
	//	      templ->R[4] + templ->R[5] + 3) / 6;

	/* good choice	*/
	return (templ->R[0] + templ->R[1] + 
		      templ->R[0] + templ->R[1] + 
		      templ->R[2] + templ->R[3] + 
		      templ->R[4] + templ->R[5] + 4) >> 3;
					
	
	/* is worse */
	 //return 0; 
}
/*---------------------------------------------------------------
 *	get_contextLS()
 *---------------------------------------------------------------*/
unsigned short get_contextLS( unsigned short num_contexts,
												 TEMPLATE *templ, double *min_dist)
{
	long diff;
	double dist; /* knLS[][] are rounded to integer	*/
	double meanOfPatt;
	unsigned short best_kn, k, i;
	/*                           A-m  B-m  C-m   B-E  A-F  C-D	*/
	//float w[NUM_KNOT_ELEM_LS] = {1.F, 1.F, 0.8F, 1.F, 1.F, 1.F}; 
	/*                           A-m  B-m  C-m   D-m  B-E  A-F  */
	float w[NUM_KNOT_ELEM_LS] = {1.F, 1.F, 1.F, 1.F, 1.F, 1.F, 1.F}; 


	/* determine context number */

	/* look for best matching knot */
	*min_dist = 99999999.;
	best_kn = 0;
	/* mean of current neighbourhood	*/
	//meanOfPatt = (templ->R[0] + templ->R[1] + 
	//	            templ->R[2] + templ->R[3] + 2) >> 2;
	meanOfPatt = get_meanOfPattern( templ);
		/* for all knots	*/
	for ( k = 0; k < num_contexts; k++)
	{
		dist = 0;
		/* for all elements	*/
		for ( i = 0; i < NUM_KNOT_ELEM_LS - 3; i++)
		{
			diff = (long)(knLS[k][i] - (templ->R[i] - meanOfPatt) );
			dist += diff * diff * w[i];
		}
		/* must also be considered in k-means clustering	contextsExtLS() */
		diff = (long)knLS[k][NUM_KNOT_ELEM_LS - 1] - (templ->R[1] - templ->R[4]);
		dist += diff * diff * w[i];
		diff = (long)knLS[k][NUM_KNOT_ELEM_LS - 2] - (templ->R[0] - templ->R[5]);
		dist += diff * diff * w[i];
		diff = (long)knLS[k][NUM_KNOT_ELEM_LS - 3] - (templ->R[2] - templ->R[3]);
		dist += diff * diff * w[i];
		if ( *min_dist > dist)
		{
			/* copy best knot	*/
			*min_dist = dist;
			best_kn = k;
		}
	}
	return (best_kn); /* return context number	*/
}

/*---------------------------------------------------------------
 *	contextsExt()
 *
 *	 f
 *	cbd
 * eax
 *
 *---------------------------------------------------------------*/
void contextsExtLS( long *lptr, /* extended image array	*/
								 IMAGE *imExt, /* parameters of extended image	*/
								 unsigned short *num_px, /* number of prediction contexts	*/
						unsigned char *const_rows, 
						unsigned char *const_cols,
						int exhaustive_flag)
{
	int k, i, best_kn;
	int convergence_flag, empty_kn_flag;
	int d[NUM_KNOT_ELEM_LS]; /* offsets */
	int collect_flag = 1;
	int max_flag = 0;
	int max_distance_k;
	double meanOfPatt;
	double max_distance;
	double sum_distance_kn[MAXNUM_OF_PRED_CONTEXTS];
	int max_k;
	long max_count;	/* highest frequency of all knots	*/
	long r, c, iter;
	double min_dist;
	double max_of_min_dist, mean_of_min_dist;
	double sum_distance, sum_distance_old;
	//double sum_kn[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM_LS];
	double **sum_kn = NULL;// [MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM_LS];
	unsigned short num_empty_knots;
	unsigned short num_contexts; /* num of cells, knots, contexts	*/
	unsigned long	num_training_data;	/* counts the number of training vectors	*/
	unsigned long pos, numm = 0, n;
	unsigned long cnt_sum, thresh;

	unsigned long count_kn[MAXNUM_OF_PRED_CONTEXTS];	/* counts the usage of knots	*/
	TEMPLATE templ;

	fprintf( stderr, "\n     context quantisation ...");

	sum_kn = matrix(MAXNUM_OF_PRED_CONTEXTS, NUM_KNOT_ELEM_LS);

	num_contexts = *num_px;
	/* -3 since three contexts are reserved for 1st row, 1st column and RLC mode  ?????*/
	assert( num_contexts <= MAXNUM_OF_PRED_CONTEXTS-3);

	/* offsets	*/
	d[0] = -1;
	d[1] =    - (long)imExt->width;
	d[2] = -1 - (long)imExt->width;
	d[3] = +1 - (long)imExt->width;
	d[4] = -2;
	d[5] =    - 2*(long)imExt->width;

	/* set knots to zero */
	for ( k = 0; k < num_contexts; k++)
	{
		int val = k * 200 / num_contexts;
		if (px_verbose)
		{
			fprintf( stdout, "\n");
		}
		/* for all elements	*/
		for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
		{
			//int sign = (1 - 2 * (1 && (k & (1 << i) )));
			knLS[k][i] = val;
			if (px_verbose)
			{
				fprintf( stdout, " %5.1f", knLS[k][i]);
			}
		}
	}

	cnt_sum = 0;
	convergence_flag = 0;
	sum_distance_old = 1e9;
	iter = 0;

	srand( 333); /* seeds pseudorandom generator	*/

	/*
	 * ##############################################################################
	 */
	while (!convergence_flag)
	{

		iter++;	/* counts number of iterations	*/
		/*
		 * reset knot parameters
		 */
		for ( k = 0; k < num_contexts; k++)
		{
			count_kn[k] = 0;	/* number of assignements	*/
			sum_distance_kn[k] = 0.; /* */
			/* for all elements	*/
			for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
			{
				sum_kn[k][i] = 0.;
			}
		}
		max_of_min_dist = 0.;
		mean_of_min_dist = 0;

		empty_kn_flag = 0; /* will be set to 1 if at least one knot is not
												* assigned to any training vector
												*/
		/* reset accumulated absolute difference (between
		 * knots and data vectors)
		 */
		sum_distance = 0;
		num_training_data = 0;

		/* need 150 trainingvectors per context	 */
		if (exhaustive_flag) 
		{
			numm = (imExt->width - OFF_LEFT - OFF_RIGHT - 2) *
							(imExt->height - OFF_TOP - 2);
		}
		else 
		{
			long size;
			size = (imExt->width - OFF_LEFT - OFF_RIGHT - 2) *
							(imExt->height - OFF_TOP - 2);
			//numm = max( 1500, 150 * num_contexts);
			numm = max( min( 4500, size), 450 * num_contexts);
		}

		if (!exhaustive_flag) srand( 333); /* seeds pseudorandom generator	*/

		/*--------------------------------------------------
		 * assign all training vectors
		 */
		for ( n = 0; n < numm; n++)
		{
			if (exhaustive_flag) 
			{
				r = 1 + n / (imExt->width - OFF_LEFT - OFF_RIGHT- 2);
				c = 1 + n % (imExt->width - OFF_LEFT - OFF_RIGHT - 2);	
			}
			else
			{
				/* random selection of a column and a row
				 * avoid positions at the top, left or right
				 * consider extended array, which should be filled with reasonable values
				 */
				r = rand()* (imExt->height - 1 - OFF_TOP) / RAND_MAX;
				c = rand()* (imExt->width - 1 - OFF_LEFT - OFF_RIGHT) / RAND_MAX;	
			}

			if ( !const_rows[r] && !const_cols[c]) /* exclude constant lines	*/
			{
				r += OFF_TOP;	/* move into acvtive area	*/
				c += OFF_LEFT;
				pos = c + r * imExt->width; /* get final position	*/

				/* Compute the differences */
				/* differences must be same as in  get_contextLS()	 !!!*/
				for ( i = 0; i < LS_TEMPLATE_SIZE; i++)
				{
					templ.R[i] = lptr[pos+d[i]];
				}

				/* look for best matching knot */
				best_kn = get_contextLS( num_contexts, &templ, &min_dist);

				/* remember extreme cases for increasing the knot number	*/
				if (max_of_min_dist < min_dist) 
					max_of_min_dist = min_dist;

				sum_distance_kn[best_kn] += min_dist; 
				/* assign values of current position to best knot*/
				count_kn[best_kn]++;

				/* mean of current neighbourhood	*/
				//meanOfPatt = (templ.R[0] + templ.R[1] + 
				//	            templ.R[2] + templ.R[3] + 2) >> 2;
				meanOfPatt = get_meanOfPattern( &templ);

				/* accumulate vectors belonging to best knot	*/
				for ( i = 0; i < NUM_KNOT_ELEM_LS - 3; i++)
				{
					sum_kn[best_kn][i] += (templ.R[i] - meanOfPatt);
				}
				sum_kn[best_kn][ NUM_KNOT_ELEM_LS-1] += (templ.R[1] - templ.R[4]);
				sum_kn[best_kn][ NUM_KNOT_ELEM_LS-2] += (templ.R[0] - templ.R[5]);
				sum_kn[best_kn][ NUM_KNOT_ELEM_LS-3] += (templ.R[2] - templ.R[3]);
				/* accumulate absolute differences for convergence test	*/
				sum_distance += min_dist;
				num_training_data++;
			} /* if constant	*/
			else
			{
				if (!exhaustive_flag) n--;	/* try again with other coordinates	*/
			}
		} /* for ( pos, all training data	*/


		if (px_verbose)
		{
			fprintf( stdout, "\n iter=%d", iter);
		}

		{
			/* look for knot with highest diversity of assigned training vectors	*/
			max_distance = 0.;
			max_distance_k = 0;
		}
		{
			/* look also for highest count */
			max_count = 0;
			max_k = 0;
		}

		/*
		 * compute new knots as average of training vectors 
		 * belonging to the cell
		 */

		//	for ( k = 0; k < num_contexts; k++)
		num_empty_knots = 0;

		for ( k = 0; k < num_contexts; k++) 
		{
			if (px_verbose)
			{
				fprintf( stdout, "\nk=%3d, %6d,", k, count_kn[k]);
			}
			if (count_kn[k]) /* exclude unused knots	*/
			{
				/* for all elements	*/
				for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
				{
					/* compute average of all assigned vectors	*/
					knLS[k][i] = sum_kn[k][i] / count_kn[k]; /* new centre of cell	*/
					if (px_verbose)
					{
						fprintf( stdout, " %5.1f", knLS[k][i]);
					}
				}
				{
					/* look for highest diversity	*/
					if ( max_distance < sum_distance_kn[k]/ count_kn[k])
					{
						max_distance = sum_distance_kn[k]/ count_kn[k];
						max_distance_k = k;	/* remember the knot	*/
					}
				}
				{
					/* look for highest count	*/
					if ( max_count < (signed)count_kn[k])
					{
						max_count = count_kn[k];
						max_k = k;	/* remember the knot	*/
					}
				}
			} /* if (count_kn[k]) */
			else num_empty_knots++;
		} /* for ( k */


		/*
		 * check whether training data fit the existing contexts
		 * if not, then increase number of contexts
		 * create new knots in the vicinity of knots with large deviation
		 * and sufficient number of assigned training vectors
		 */
		if ( 0 && !num_empty_knots /*&& count_kn[max_distance_k] > 150 */)
		{
			if ( sum_distance/numm > 1200)
			{
				int tmp = num_contexts;

				if (num_contexts < 8) num_contexts += num_contexts;
				else	num_contexts += 8;
				num_contexts = min( num_contexts, MAXNUM_OF_PRED_CONTEXTS);
				/* reset counts of not yet used knots	*/
				for ( k = tmp; k < num_contexts; k++)
				{
					count_kn[k] = 0;	/* number of assignements	*/
				}

				collect_flag = 1;
			}
		}

		/* save guard, 	*/
		if (iter > 200) 
		{
			convergence_flag = 1;
		}
		else
		{
			/* only, if it is not the last round	*/
			/* 
			 * set all unused knots close to the one 
			 * with highest count or diversity 
			 */
			for ( k = 0; k < num_contexts; k++) 
			{
				double max_element = 0.;
				if (count_kn[k]) continue; /* only for unused knots		*/
				empty_kn_flag = 1; /* at least one empty knot	*/
				if (px_verbose)
				{
					fprintf( stdout, "\n k=%d, %6d,", k, count_kn[k]);
				}
				/* for all elements copy from most used/diverse one	*/
				for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
				{
					/* mode of re-init has almost no influence	*/
					if ( max_flag)
					{
						knLS[k][i] = knLS[ max_distance_k][i]; /* knot with highest error	*/
					}
					else
					{
						knLS[k][i] = knLS[ max_k][i];		/* knot with highest count*/
					}
					if (px_verbose)
					{
						fprintf( stdout, " %5.1f", knLS[k][i]);
					}
					if (max_element < fabs( knLS[k][i])) max_element = fabs( knLS[k][i]);
				}
				/* select one element (based on k+iter) and disturb it	*/
				if ( (k+iter) % 3 == 0)
				{
					/* slight positive variation	*/
					knLS[k][(k+iter)%NUM_KNOT_ELEM_LS] += 
							1 + max_element*(k + 1) / num_contexts; 
				}
				else if ( (k+iter) % 3 == 1)
				{
					/* slight negative variation	*/
					knLS[k][(k+iter)%NUM_KNOT_ELEM_LS] -= 
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
					for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
					{
						/* combine with another knot	*/
						knLS[k][i] = fac * knLS[k1][i] + (1-fac) * knLS[k2][i];
					}
				}
				
				if (px_verbose)
				{
						fprintf( stdout, " %6.1f (%d)", knLS[k][k%NUM_KNOT_ELEM_LS], k%NUM_KNOT_ELEM_LS);
				}
			}
		}
		if (px_verbose)
		{
			fprintf( stdout, "\n");
			fprintf( stdout, "sumdiff: %f\n", (sum_distance_old - sum_distance) / sum_distance);
		}
		/* check the convergence criterion	*/
		if (sum_distance)
		{
			if ( ((fabs(sum_distance_old - sum_distance) / sum_distance) < EPS) && iter > 10)
			{
				/* go ahead as long as there are unassigned knots	*/
				if (!empty_kn_flag)	convergence_flag = 1;
			}
		}
		else
		{
				if (!empty_kn_flag)	convergence_flag = 1;
		}
		sum_distance_old = sum_distance;

		fprintf( stderr, "\r%d%%", 100 * (iter) / 200);
		max_flag = !max_flag; /* use opposite next round	*/
		//collect_flag = 0;
	}	/* while !convergence_flag)	*/
	/*
	 * ##############################################################################
	 */


	/* 
   * remove unused or seldomly used knots
	 * threshold id extrapolation of hits per context
	 * numm/num_contexts is averaged number per context
	 * im->size/numm is factor for real image
	 */
	//thresh =  850 * numm /
	thresh =  750 * numm /
		( (imExt->width - OFF_LEFT - OFF_RIGHT) *
							(imExt->height - OFF_TOP) );
	/* get median, avoid thresh being to high
	 * good for small images	
	 */
	{
		unsigned long count2[MAXNUM_OF_PRED_CONTEXTS];
		for ( k = 0; k < num_contexts; k++)
		{
			count2[k] = count_kn[k];
		}
		heap_sort( num_contexts, count2);
		/* taking the median would always remove 50% of contexts	*/
		thresh = min( thresh, count2[ num_contexts >> 1 ]);
	}

	for ( k = 0; k < num_contexts; k++)
	{
		if (count_kn[k] < thresh && num_contexts>1)
		{
			num_contexts--;
			for ( n = k; n < num_contexts; n++)
			{
				count_kn[n] = count_kn[n+1];
				for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
				{
					/* copy from next	*/
					knLS[n][i] = knLS[n+1][i];
				}
			}
			k--; /* check same index again since content is new	*/
		}
	}

	/* rounding of all vector elements (required for transmission)
   * it has almost no influence on prediction efficiency
	 */
	for ( k = 0; k < num_contexts; k++)
	{
			/* for all elements	*/
			for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
			{
				if (knLS[k][i] < 0) knLS[k][i] = -floor( -knLS[k][i] + 0.5);
				else knLS[k][i] = floor( knLS[k][i] + 0.5);
			}
	}

	/* bubble sort of knots, benefits the (differential) transmission of knot info	*/
	{
		int flag = 1;
		/* check first element */
		while ( flag)
		{
			flag = 0;
			for ( k = 1; k < num_contexts; k++)
			{
				if (knLS[k][0] < knLS[k-1][0])
				{
					long tmp;
					if (px_verbose)
					{
						tmp = count_kn[k]; /* exchange counts	*/
						count_kn[k] = count_kn[k-1];
						count_kn[k-1] = tmp;
					}
					flag = 1;
					/* exchange	all elements */
					for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
					{
						double tmpD;
						tmpD = knLS[k][i];
						knLS[k][i] = knLS[k-1][i];
						knLS[k-1][i] = tmpD;
					}
				}
			} /* for k	*/
		} /* while */

		flag = 1;
		/* check 2nd element */
		while ( flag)
		{
			flag = 0;
			for ( k = 1; k < num_contexts; k++)
			{
				if (knLS[k][0] == knLS[k-1][0] && knLS[k][1] < knLS[k-1][1])
				{
					long tmp;
					if (px_verbose)
					{
						tmp = count_kn[k]; /* exchange counts	*/
						count_kn[k] = count_kn[k-1];
						count_kn[k-1] = tmp;
					}
					flag = 1;
					/* exchange	all other elements elements */
					for ( i = 1; i < NUM_KNOT_ELEM_LS; i++)
					{
						/* first element i=0 is equal and must not be exchanged	*/
						double tmpD;
						tmpD = knLS[k][i];
						knLS[k][i] = knLS[k-1][i];
						knLS[k-1][i] = tmpD;
					}
				}
			} /* for k	*/
		} /* while */

		flag = 1;
		/* check 3rd element */
		while ( flag)
		{
			flag = 0;
			for ( k = 1; k < num_contexts; k++)
			{
				if (knLS[k][0] == knLS[k-1][0] && knLS[k][1] == knLS[k-1][1] && knLS[k][2] < knLS[k-1][2])
				{
					long tmp;
					if (px_verbose)
					{
						tmp = count_kn[k]; /* exchange counts	*/
						count_kn[k] = count_kn[k-1];
						count_kn[k-1] = tmp;
					}
					flag = 1;
					/* exchange	all other elements */
					for ( i = 2; i < NUM_KNOT_ELEM_LS; i++)
					{
						/* first elements i=0,1 are equal and must not be exchanged	*/
						double tmpD;
						tmpD = knLS[k][i];
						knLS[k][i] = knLS[k-1][i];
						knLS[k-1][i] = tmpD;
					}
				}
			} /* for k	*/
		} /* while */
	}
	if (px_verbose)
	{
		for ( k = 0; k < num_contexts; k++)
		{
			fprintf( stdout, "\nk=%3d, %6d,", k, count_kn[k]);
			for ( i = 0; i < NUM_KNOT_ELEM_LS; i++)
			{
				fprintf( stdout, " %5.0f", knLS[k][i]);
			}
		}
		fprintf( stdout, "\n");
	}						
	*num_px = num_contexts;
	free_matrix( &sum_kn);
	return;
}
