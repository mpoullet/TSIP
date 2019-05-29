/*****************************************************************
 *
 * File..:	contexts.c
 * Descr.:	Contexts for linear prediction based on VQ (LBG algorithm)
 * Author.:	Tilo Strutz
 * Date..: 	29.02.2013
 * changes:
 * 12.03.2013 new: get_AC_context()
 * 22.03.2013 reorganisation of #defines
 * 02.04.2013 get_AC_context(): use prediction-context information
 * 17.04.2013 exception handling at top left corner for AC contexts
 * 02.05.2013 get AC context based on mean abs error in prediction contexts
 * 06.05.2013 get_context() moved here from predictor2a.c 
 * 06.05.2013 initialisation of knots with signed values 
 * 06.05.2013 get_average_errors() w/o double computation
 * 16.05.2013 get_average_errors() check texture
 * 28.05.2013 Ra_shift, scaling of Ra in kn[][]
 * 07.06.2013 get_average_errors() larger template, 10* sum_abserr
 * 14.06.2013 get_average_errors() context-based adaptive averaging
 *             of neighbours for prediction-error magnitude forecast
 *						init_weights_e(), update_weights_e()
 * 19.06.2013 get_average_errors() context-based adaptive averaging
 *						between estimate of neighbours and estimate of pred.cx
 * 20.06.2013 Ra_shift replaced by * Ra_fac_numerator/Ra_fac_denominator 
 *						kn[0][NUM_ELEMENTS-1] adapted to average of R
 * 22.06.2013 get_average_errors() switchingwith weight_flag
 * 22.06.2013 update_weights_e() reset = image_width /2
 * 03.07.2013 exhaustive_flag
 * 23.07.2013 bugfix: Ra_fac_denominator must be greater than zero!
 * 25.07.2013 get_AC_context() hand over of px removed (unnessecary)
 * 13.06.2014 new thresh based on median count_k for small images
 * 27.07.2018 symmetric patterns, di[0] is always positive
 *            ==> LMS coefficients must be modified, aren't they?
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

#define EPS 1e-4
#ifdef _DEBUG
	 //#define px_verbose 1
	#define px_verbose 0
#else
	#define px_verbose 0
#endif

/* knots (cells) for vector quantisation of texture	*/
double kn[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM]; /* required in encoder.c	*/
//int Ra_shift;
int Ra_fac_numerator;
int Ra_fac_denominator;
//int di_stddev, Ra_stddev, Ra_average, di_average;

#define OK_COMP
//#undef OK_COMP

/*---------------------------------------------------------------
 *	get_context()
 *---------------------------------------------------------------*/
unsigned short get_context( int r, int c, 
												 unsigned short num_contexts,
												 TEMPLATE *templ)
{
	long di[NUM_KNOT_ELEM];
	long diff;
	unsigned long dist, min_dist; /* kn[][] are rounded to integer	*/
	unsigned short best_kn, k, i;

//#ifdef UIUIUII // check
	if (r == 0)
	{
		/* use left neighbour as prediction value
		 * and special context for signalling
		 */
		return ( num_contexts + 1);
	}
	else 
		if ( c == 0)
	{
		/* use top neighbour as prediction value
		 * and special context for signalling
		 */
		return ( num_contexts + 2);
	}
//#endif


	/* determine context number */
	/* differences must be same as in contexts()	 !!!*/
#ifdef DIFF_TEXTURE
	GET_DIFFS()
#else
	/* get pixel values */
	GET_VALS()
#endif

	/* 2018 symmetric patterns	*/
	if (di[0] < 0 && 0)
	{
		for ( i = 0; i < NUM_KNOT_ELEM-1; i++)
			di[i] = -di[i];
	}


	/* look for best matching knot */
	min_dist = 99999999;
	best_kn = 0;
	/* for all knots	*/
	for ( k = 0; k < num_contexts; k++)
	{
		dist = 0;
		/* for all elements	*/
#ifdef PCX_INCLUDE_RA
		/* include Ra into vector formation, see Ra_mod below !!	*/
		//diff = (long)kn[k][NUM_KNOT_ELEM-1] - (templ->R[0] >> Ra_shift); 
		diff = (long)kn[k][NUM_KNOT_ELEM-1] - 
						( (templ->R[0] * Ra_fac_numerator + (Ra_fac_denominator>>1)) / Ra_fac_denominator); 
		//diff = (long)kn[k][NUM_KNOT_ELEM-1] - 
		//				( (templ->R[0] - Ra_average) * di_stddev / (4*Ra_stddev)); 
	 
		dist += diff * diff;
		for ( i = 0; i < NUM_KNOT_ELEM-1; i++)
#else
		for ( i = 0; i < NUM_KNOT_ELEM; i++)
#endif
		{
#ifdef EUCLID_DIST
			diff = (long)kn[k][i] - di[i];
						/* down-weight differences with larger distance to current position
						 * must be the same in contextsExt()
						 */
						//if (i > 4) 	diff >>= 1; 
			dist += diff * diff;
#else
		#ifdef QUARTIC_DIST
						long diff;
						diff = (long)kn[k][i] - di[i];
						dist += diff * diff * diff * diff;
		#else
						/* city-block distance	*/
						dist += (long)fabs( kn[k][i] - di[i]);
		#endif
#endif
		}
		if ( min_dist > dist)
		{
			/* copy best knot	*/
			min_dist = dist;
			best_kn = k;
		}
	}

	return (best_kn);
}

/*---------------------------------------------------------------
 *	contextsExt()
 *
 *    nop
 *	 mghiq
 *	lfcbdjr
 * skeax
 *
 * differences
 *         n12o14p
 *           13
 *	    m  g  h  i q
 *     11  5  4  715
 *	 l  f 6c 1b 2d8j16r
 *  10     0
 *   k 9e 3a  x
 *---------------------------------------------------------------*/
void contextsExt( long *lptr, /* extended image array	*/
								 IMAGE *imExt, /* parameters of extended image	*/
								 unsigned short *num_px, /* number of prediction contexts	*/
						unsigned char *const_rows, 
						unsigned char *const_cols,
						int exhaustive_flag)
{
	int k, i, best_kn;
	int convergence_flag, empty_kn_flag;
	int da, db, dc, dd, de, dh /* 666*, dg /* 666*, df, dI, dj */;
	int collect_flag = 1;
//#ifdef MAX_DIVERGENCE
	int max_flag = 0;
	int max_distance_k;
	double max_distance;
	//double sum_distance_kn[MAXNUM_OF_PRED_CONTEXTS];
	double *sum_distance_kn = NULL;
	//#else
	int max_k;
//#endif
	long max_count;	/* highest frequency of all knots	*/
	long r, c, iter;
	long	di[NUM_KNOT_ELEM], Ra_modif = 0;
	double diff;
	double min_dist, dist;
	double max_of_min_dist, mean_of_min_dist;
	double sum_distance, sum_distance_old;
	double sum_kn[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM];
	//long di_sum;
	//double di_energy, Ra_energy;
	unsigned short num_empty_knots;
	unsigned short num_contexts; /* num of cells, knots, contexts	*/
	unsigned long	num_training_data;	/* counts the number of training vectors	*/
	unsigned long pos, numm = 0, n;
	unsigned long cnt_sum, di_abssum, Ra_sum, thresh;

	//unsigned long count_kn[MAXNUM_OF_PRED_CONTEXTS];	/* counts the usage of knots	*/
	unsigned long *count_kn = NULL;	/* counts the usage of knots	*/

	fprintf( stderr, "\n     context quantisation ...");


	CALLOC(sum_distance_kn, MAXNUM_OF_PRED_CONTEXTS, double);
	CALLOC(count_kn, MAXNUM_OF_PRED_CONTEXTS, unsigned long);

	num_contexts = *num_px;
	/* -3 since three contexts are reserved for 1st row, 1st column and RLC mode*/
	assert( num_contexts <= MAXNUM_OF_PRED_CONTEXTS-3);

	/* offsets	*/
	da = -1;
	db =    - (long)imExt->width;
	dc = -1 - (long)imExt->width;
	dd = +1 - (long)imExt->width;
	de = -2;
	dh =    - 2*(long)imExt->width;
	/* 666 *  dg = -1 - 2*(long)imExt->width; 
	/* 666 * df = -2 - (long)imExt->width; 
	/* 666 * dI = +1 - 2*(long)imExt->width; 
	/* 666 * dj = +2 - (long)imExt->width; /* */


	/* set knots to zero */
	for ( k = 0; k < num_contexts; k++)
	{
		if (px_verbose)
		{
			fprintf( stdout, "\n");
		}
		/* for all elements	*/
		for ( i = 0; i < NUM_KNOT_ELEM; i++)
		{
			int sign = (1 - 2 * (1 && (k & (1 << i) )));
			int val = (k+31) / 32;
			kn[k][i] = 2 * val * sign;
			if (px_verbose)
			{
				fprintf( stdout, " %8.4f", kn[k][i]);
			}
		}
	}

	cnt_sum = 0;
	di_abssum = 0;	/* average of difference magnitudes	*/
	Ra_sum = 0;	/* average of pixel values	*/
	//di_sum = 0;	/* average of difference 	*/
	//di_energy = 0.0;
	//Ra_energy = 0.0;
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
//#ifdef MAX_DIVERGENCE
			//if (max_flag)
				sum_distance_kn[k] = 0.; /* */
//#endif
			/* for all elements	*/
			for ( i = 0; i < NUM_KNOT_ELEM; i++)
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
#ifdef DIFF_TEXTURE
				/* differences must be same as in predictor2a.c, get_context()	 !!!*/
				di[0] = lptr[pos+da] - lptr[pos+dc]; /* Ra - Rc; */
				di[1] = lptr[pos+dc] - lptr[pos+db]; /* Rc - Rb;	 */
				di[2] = lptr[pos+db] - lptr[pos+dd]; /* Rb - Rd; */

				di[3] = lptr[pos+da] - lptr[pos+de]; /* Ra - Re; */
				di[4] = lptr[pos+db] - lptr[pos+dh]; /* Rb - Rh; */

				/* 2018 symmetric Patterns	*/
				if (di[0] < 0 && 0)
				{
					for ( i = 0; i < NUM_KNOT_ELEM-1; i++)
						di[i] = -di[i];
				}
				/* 666* di[5] = lptr[pos+dc] - lptr[pos+dg]; /* Rc - Rg; */
				/* 666* di[6] = lptr[pos+de] - lptr[pos+df]; /* Re - Rf; */
				/* 666* di[7] = lptr[pos+dd] - lptr[pos+dI]; /* Rd - Ri; */
				/* 666* di[7] = lptr[pos+dd] - lptr[pos+dj]; /* Rd - Rj; */
#else
				/* differences must be same as in predictor2a.c, get_context()	 !!!*/
				di[0] = lptr[pos+da]; /* Ra; */
				di[1] = lptr[pos+db]; /* Rb;	 */
				di[2] = lptr[pos+dc]; /* Rc; */

				di[3] = lptr[pos+dd]; /* Rd; */
				di[4] = lptr[pos+de]; /* Re; */
				di[5] = lptr[pos+dh]; /* Rh; */
#endif

#ifdef PCX_INCLUDE_RA
				if (collect_flag)
				{
					for (i = 0; i < 5; i++)
					{
						di_abssum += abs( di[i]);
						//di_sum += di[i];
						//di_energy += di[i] * di[i];
					}
					Ra_sum += lptr[pos+da];
					//Ra_energy += lptr[pos+da] * lptr[pos+da];
					cnt_sum++;
				}
				else 
				{
					/*  settings in get_context() must be the same	*/
					//Ra_modif = lptr[pos+da] >> Ra_shift;
					Ra_modif = (lptr[pos+da] * Ra_fac_numerator + (Ra_fac_denominator>>1)) / Ra_fac_denominator;
					//Ra_modif = (lptr[pos+da] - Ra_average) * di_stddev / (4*Ra_stddev);
				}
#endif
				/* look for best matching knot */
				min_dist = 99999999;
				best_kn = 0;
				
				/* compare to all knots	*/
				for ( k = 0; k < num_contexts; k++)
				{
					dist = 0;
					/* for all elements	*/
#ifdef PCX_INCLUDE_RA
					/* include Ra into vector formation	*/
					diff = kn[k][NUM_KNOT_ELEM-1] - Ra_modif; 
					dist += diff * diff;
					for ( i = 0; i < NUM_KNOT_ELEM-1; i++)
#else
					for ( i = 0; i < NUM_KNOT_ELEM; i++)
#endif
					{
	#ifdef EUCLID_DIST
						diff = kn[k][i] - di[i];
						/* down-weight differences with larger distance to current position
						 * must be the same in get_context()
						 */
						//if (i > 4) 	diff *= 0.5; 
						/* more weight for differences in near vincinity	
						 * must be the same in get_context()
						 */
						//if (i < 3) diff *= 2;
						dist += diff * diff; 
	#else
		#ifdef QUARTIC_DIST
						diff = kn[k][i] - di[i];
						dist += diff * diff * diff * diff;
		#else
						/* city-block distance	*/
						dist += fabs( kn[k][i] - di[i]);
		#endif
	#endif
					}
					if ( min_dist > dist)
					{
						/* copy best knot	*/
						min_dist = dist;
						best_kn = k;
					}
				} /* for ( k = 0; */

				/* remember extreme cases for increasing the knot number	*/
				if (max_of_min_dist < min_dist) 
					max_of_min_dist = min_dist;

				/* normalize distance by number of vector elements
				 * not necessary as only qualitative comparisons are performed
				 */
				//min_dist /= NUM_KNOT_ELEM;
//#ifdef MAX_DIVERGENCE
				/* used for computation of averaged distance:	*/
				//if (max_flag)
					sum_distance_kn[best_kn] += min_dist; 
//#endif
				/* assign values of current position to best knot*/
				count_kn[best_kn]++;


				/* accumulate vectors belonging to best knot	*/
#ifdef PCX_INCLUDE_RA
				sum_kn[best_kn][NUM_KNOT_ELEM-1] += Ra_modif;
				for ( i = 0; i < NUM_KNOT_ELEM-1; i++)
#else
				for ( i = 0; i < NUM_KNOT_ELEM; i++)
#endif
				{
					sum_kn[best_kn][i] += di[i];
				}
				/* accumulate absolute differences for convergence test	*/
				sum_distance += min_dist;
				num_training_data++;
			} /* if constant	*/
			else
			{
				if (!exhaustive_flag) n--;	/* try again with other coordinates	*/
			}
		} /* for ( pos, all training data	*/

#ifdef PCX_INCLUDE_RA
		if (collect_flag)
		{
			/* get standard deviation od values	*/
			//Ra_average = (Ra_sum + (cnt_sum>>2)) / cnt_sum;
			//Ra_stddev = (int)floor( sqrt(  Ra_energy/cnt_sum - Ra_average*Ra_average) + 0.5);
			//di_average = (di_sum + (long)(cnt_sum>>3)) / (long)(5*cnt_sum);
			//di_stddev = (int)floor( sqrt(  di_energy/cnt_sum - di_average*di_average) + 0.5);

			/* times 5 as 5 differences have been accumulated at once	*/
			di_abssum = (di_abssum + (5*cnt_sum >> 1)) / (5*cnt_sum);

			/* Ra should be on same scale as differences	*/
			//Ra_shift = (imExt->bit_depth-1) - di_abssum;
			//if (Ra_shift == 0) Ra_shift = 1;
			//if (Ra_shift < 0) Ra_shift = 0;
			/* is used for the first knot, which wont be updated	*/
			Ra_sum = (Ra_sum + (cnt_sum >> 1)) / cnt_sum;
			Ra_fac_numerator = di_abssum * 4; /* times 4, otherwise the values are too small	*/
			if ( !Ra_fac_numerator) Ra_fac_numerator++;
			Ra_fac_denominator = Ra_sum;
			if ( !Ra_fac_denominator) Ra_fac_denominator++; /* avoid division by zero	later on */
			collect_flag = 0;

		}
#endif

		if (px_verbose)
		{
			fprintf( stdout, "\n iter=%d", iter);
		}

//#ifdef MAX_DIVERGENCE
		//if (max_flag)
		{
			/* look for knot with highest diversity of assigned training vectors	*/
			max_distance = 0.;
			max_distance_k = 0;
		}
//#else
		//else
		{
			/* look also for highest count */
			max_count = 0;
			max_k = 0;
		}
//#endif

		/*
		 * compute new knots as average of training vectors 
		 * belonging to the cell
		 */

		//	for ( k = 0; k < num_contexts; k++)
		num_empty_knots = 0;
#ifdef PCX_INCLUDE_RA
		/* average only R contribution	*/
		if (count_kn[0])
		{
			kn[0][NUM_KNOT_ELEM-1] = sum_kn[0][NUM_KNOT_ELEM-1] / count_kn[0]; 
		}
		else
		{
			kn[0][NUM_KNOT_ELEM-1] = 0; 
			num_empty_knots++;
		}
#endif

		for ( k = 1; k < num_contexts; k++) /* leave first knot	unchanged */
		{
			if (px_verbose)
			{
				fprintf( stdout, "\nk=%3d, %6d,", k, count_kn[k]);
			}
			if (count_kn[k]) /* exclude unused knots	*/
			{
				/* for all elements	*/
				for ( i = 0; i < NUM_KNOT_ELEM; i++)
				{
					/* compute average of all assigned vectors	*/
					kn[k][i] = sum_kn[k][i] / count_kn[k]; /* new centre of cell	*/
					if (px_verbose)
					{
						fprintf( stdout, " %7.3f", kn[k][i]);
					}
				}
//#ifdef MAX_DIVERGENCE
				//if (max_flag)
				{
					/* look for highest diversity	*/
					if ( max_distance < sum_distance_kn[k]/ count_kn[k])
					{
						max_distance = sum_distance_kn[k]/ count_kn[k];
						max_distance_k = k;	/* remember the knot	*/
					}
				}
//#else
				//else
				{
					/* look for highest count	*/
					if ( max_count < (signed)count_kn[k])
					{
						max_count = count_kn[k];
						max_k = k;	/* remember the knot	*/
					}
				}
//#endif
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
			//for ( k = 0; k < num_contexts; k++)
			for ( k = 1; k < num_contexts; k++) /* leave first knot	unchanged */
			{
				double max_element = 0.;
				if (count_kn[k]) continue; /* only for unused knots		*/
				empty_kn_flag = 1; /* at least one empty knot	*/
				if (px_verbose)
				{
					fprintf( stdout, "\n k=%d, %6d,", k, count_kn[k]);
				}
				/* for all elements copy from most used/diverse one	*/
				for ( i = 0; i < NUM_KNOT_ELEM; i++)
				{
					/* mode of re-init has almost no influence	*/
	//#ifdef MAX_DIVERGENCE
					if ( max_flag)
					{
						kn[k][i] = kn[ max_distance_k][i]; /* knot with highest error	*/
					}
	//#else
					else
					{
						kn[k][i] = kn[ max_k][i];		/* knot with highest count*/
					}
	//#endif
					if (px_verbose)
					{
						fprintf( stdout, " %7.3f", kn[k][i]);
					}
					if (max_element < fabs( kn[k][i])) max_element = fabs( kn[k][i]);
				}
				/* select one element (based on k+iter) and disturb it	*/
				if ( (k+iter) % 3 == 0)
				{
					/* slight positive variation	*/
					//kn[k][(k+iter)%NUM_KNOT_ELEM] += 1 + 10.0*(k + 1) / num_contexts; 
					kn[k][(k+iter)%NUM_KNOT_ELEM] += 
							1 + max_element*(k + 1) / num_contexts; 
				}
				else if ( (k+iter) % 3 == 1)
				{
					/* slight negative variation	*/
					// kn[k][(k+iter)%NUM_KNOT_ELEM] -= 1 + 10.0*(k + 1) / num_contexts;
					kn[k][(k+iter)%NUM_KNOT_ELEM] -= 
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
					for ( i = 0; i < NUM_KNOT_ELEM; i++)
					{
						/* combine with another knot	*/
						kn[k][i] = fac * kn[k1][i] + (1-fac) * kn[k2][i];
					}
				}
				
				if (px_verbose)
				{
						fprintf( stdout, " %8.4f (%d)", kn[k][k%NUM_KNOT_ELEM], k%NUM_KNOT_ELEM);
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
	thresh =  850 * numm /
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
				for ( i = 0; i < NUM_KNOT_ELEM; i++)
				{
					/* copy from next	*/
					kn[n][i] = kn[n+1][i];
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
#ifdef PCX_INCLUDE_RA
			/* quantize Ra a little bit stronger	
			 *  must be in accordance with transmission of quantized Ra in encoder.c !!!
			 */
			// kn[k][NUM_KNOT_ELEM-1] /= 4; 
			kn[k][NUM_KNOT_ELEM-1] /= ((Ra_fac_numerator >> 2) + 1); 
#endif
			/* for all elements	*/
			for ( i = 0; i < NUM_KNOT_ELEM; i++)
			{
				if (kn[k][i] < 0) kn[k][i] = -floor( -kn[k][i] + 0.5);
				else kn[k][i] = floor( kn[k][i] + 0.5);
			}
#ifdef PCX_INCLUDE_RA
			/* quantize Ra a little bit stronger	*/
			//kn[k][NUM_KNOT_ELEM-1] *= 4; 
			kn[k][NUM_KNOT_ELEM-1] *= ((Ra_fac_numerator >> 2) + 1); 
#endif
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
				if (kn[k][0] < kn[k-1][0])
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
					for ( i = 0; i < NUM_KNOT_ELEM; i++)
					{
						double tmpD;
						tmpD = kn[k][i];
						kn[k][i] = kn[k-1][i];
						kn[k-1][i] = tmpD;
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
				if (kn[k][0] == kn[k-1][0] && kn[k][1] < kn[k-1][1])
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
					for ( i = 1; i < NUM_KNOT_ELEM; i++)
					{
						/* first element i=0 is equal and must not be exchanged	*/
						double tmpD;
						tmpD = kn[k][i];
						kn[k][i] = kn[k-1][i];
						kn[k-1][i] = tmpD;
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
				if (kn[k][0] == kn[k-1][0] && kn[k][1] == kn[k-1][1] && kn[k][2] < kn[k-1][2])
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
					for ( i = 2; i < NUM_KNOT_ELEM; i++)
					{
						/* first elements i=0,1 are equal and must not be exchanged	*/
						double tmpD;
						tmpD = kn[k][i];
						kn[k][i] = kn[k-1][i];
						kn[k-1][i] = tmpD;
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
			for ( i = 0; i < NUM_KNOT_ELEM; i++)
			{
				fprintf( stdout, " %7.3f", kn[k][i]);
			}
		}
		fprintf( stdout, "\n");
	}						
	*num_px = num_contexts;

	FREE(sum_distance_kn);
	FREE(count_kn);
	return;
}

#ifdef OK_COMP

/*---------------------------------------------------------------
 *	init_weights_e()
 *---------------------------------------------------------------*/
void init_weights_e( PREDE *p_predE)
{
	unsigned int i, k;

	for ( k = 0; k < NUM_PX_CONTEXTS; k++)
	{
		//for ( i = 0; i < TEMPLATE_E_SIZE; i++)
		for ( i = 0; i < NUM_ESTIMATORS; i++)
		{
			p_predE->varianceN[k][i] = 1; /* must be something greater than zero	*/
			p_predE->weights_e[k][i] = 0.1;	/* neighbours	*/
		}
		/* special initialisation of weights*/
		/* only [1] and [2] are used in this version	*/
		p_predE->weights_e[k][0] = 0.3;	/* pred. context	*/
		p_predE->weights_e[k][1] = 0.4;	/* neighbours	*/
		p_predE->weights_e[k][2] = 0.4;	/* neighbours	*/
		p_predE->cnt[k] = 0;
		p_predE->weight_flag[k] = 0;
	}
	p_predE->weight_flag_cnt = 3;
}

 /*---------------------------------------------------------------
 *	get_averaged_error()
 * based on template and context information
 * weighting:
 *       n o p
 *	   m g h i q
 *	 l f c b d j r
 *   k e a x
 *
 *---------------------------------------------------------------*/
unsigned long get_averaged_error( IMAGE *im, long *eptr, 
							unsigned long pos, int r, int c,
							unsigned short *px_ptr,
							unsigned long sum_abserr, /* averaged absolut error	*/
							unsigned long count_px,		/* in current context			*/
							PREDE *p_predE, TEMPLATE_E *templ,
							unsigned long num_abs_scaled_errors)
{
	int flag_A, flag_B;
	long	dAC, dBC, dBD;
	long	eHat;
	double sum_w, eHat_d;
	//unsigned int	W1 = 6, W1m = 3, W2 = 2, W3 = 2;
	unsigned int	W1 = 6, W1m = 2, W2 = 3, Wpx = 2;
	unsigned int	i, px;
	unsigned int	countL, countR;
	unsigned long sumL, sumR;

	sumL = sumR = 0;
	countL = countR = 0;

	/* exception handling	
	 * typically, error at (r, c) = (0,0) is high
	 * but not its neighbours, so exclude (0,0)
	 * context for (0,0) will be overwritten in coder_
	 */

	/* reset	*/
	for ( i = 0; i < TEMPLATE_E_SIZE; i++) templ->e[i] = -1;

	/* get prediction context	*/
	px = px_ptr[pos];
	assert( px < NUM_PX_CONTEXTS);

	if ( c + r == 0)
	{
		templ->e[0] = 0; /* position A	*/
	}

	/* TH_FAC == scaling for AC context quantisation	*/
	if ( c + r < 4)
	{
		for ( i = 0; i < NUM_ESTIMATORS; i++)
		{
			p_predE->estimate[i] = TH_FAC;
		}
		return (1 * TH_FAC); /* moderate estimate of 1	*/
	}

	if (c > 0)	/* not the first column	*/
	{
		templ->e[0] = abs( eptr[pos-1]);	/* A	*/
		sumL += W1 * templ->e[0];	/* left neighbour, weight 5 */
		countL += W1;

		if (c > 1)
		{
			templ->e[4] = abs( eptr[pos-2]);	/* E	*/
			sumL += W2 * templ->e[4];	/* second left neighbour, weight 2 */
			countL += W2;

			if (c > 2)
			{
				templ->e[10] = abs( eptr[pos-3]);	/* K	*/
				sumL += templ->e[10];	/* 3rd left neighbour, weight 1 */
				countL++;
			}
		}
	}
	if (r > 0)
	{
		templ->e[1] = abs( eptr[pos-im->width]);	/* B	*/
		sumR += W1 * templ->e[1]; 	/* top neighbour, weight 5*/
		countR += W1;
		if (c > 0)
		{
			templ->e[2] = abs( eptr[pos-1-im->width]);	/* C	*/
			sumL += W2 * templ->e[2];	/* topleft neighbour, weight 2*/
			countL += W2;
			if (c > 1)
			{
				templ->e[5] = abs( eptr[pos-2-im->width]);	/* F	*/
				sumL += templ->e[5];	/* second left-top  neighbour, weight 1*/
				countL++;
				if (c > 2)
				{
					templ->e[11] = abs( eptr[pos-3-im->width]);	/* L	*/
					sumL += templ->e[11];	/* second left-top  neighbour, weight 1*/
					countL++;
				}
			}
		}
		if (c < (signed)im->width-1)
		{
			templ->e[3] = abs( eptr[pos+1-im->width]);	/* D	*/
			sumR += W2 * templ->e[3];/* top-right neighbour, weight 2*/
			countR += W2;
			if (c < (signed)im->width-2)
			{
				templ->e[9] = abs( eptr[pos+2-im->width]);	/* J	*/
				sumR += templ->e[9];
				countR++;
				if (c < (signed)im->width-3)
				{
					templ->e[17] = abs( eptr[pos+3-im->width]);	/* R	*/
					sumR += templ->e[17];
					countR++;
				}
			}
		}
	}
	if (r > 1)
	{
		templ->e[7] = abs( eptr[pos  -im->width-im->width]);	/* H	*/
		sumR += W2 * templ->e[7];
		countR += W2;
		if (c > 0)
		{
			templ->e[6] = abs( eptr[pos-1-im->width-im->width]);	/* G	*/
			sumL += templ->e[6];
			countL++;
			if (c > 1)
			{
				templ->e[12] = abs( eptr[pos-2-im->width-im->width]);	/* M	*/
				sumL += templ->e[12];
				countL++;
			}
		}
		if (c < (signed)im->width-1)
		{
			templ->e[8] = abs( eptr[pos+1-im->width-im->width]);	/* I	*/
			sumR += templ->e[8];
			countR++;
			if (c < (signed)im->width-2)
			{
				templ->e[16] = abs( eptr[pos+2-im->width-im->width]);	/* Q	*/
				sumR += templ->e[16];
				countR++;
			}
		}
	}
	if (r > 2)
	{
		templ->e[14] = abs( eptr[pos  -im->width-im->width-im->width]);	/* O	*/
		sumR += templ->e[14];
		countR++;
		if (c > 0)
		{
			templ->e[13] = abs( eptr[pos-1-im->width-im->width-im->width]);	/* N	*/
			sumL += templ->e[13];
			countL++;
		}
		if (c < (signed)im->width-1)
		{
			templ->e[15] = abs( eptr[pos+1-im->width-im->width-im->width]);	/* P	*/
			sumR += templ->e[15];
			countR++;
		}
	}

	/* mean error in current context	*/
	//if (count_px)
	//{
	//	templ->e[TEMPLATE_E_SIZE-1] = (sum_abserr + (count_px >> 1) ) / count_px;
	//}
	//else
	//{
	//	templ->e[TEMPLATE_E_SIZE-1] = 0;
	//}

	//if (templ->e[0] >= 0) /* A is present	*/
	//	val = templ->e[0];
	//else
	//	val = templ->e[1];	/* B must be present	*/

	///* set non-existing positions to something useful	*/
	//for ( i = 0; i < TEMPLATE_E_SIZE; i++)
	//{
	//	if (templ->e[i] < 0)
	//		templ->e[i] = val;
	//}

	/* finetuning of weights	*/
	if (c > 0 && r > 0 )
	{
		/* direction based influence; does slightly improve bitrate */
		dAC = abs( templ->e[2] - templ->e[0]); /* abs( abs_C - abs_A) */
		dBC = abs( templ->e[2] - templ->e[1]); /* abs( abs_C - abs_B) */
		if (c < (signed)im->width-1)	dBD = abs( templ->e[3] - templ->e[1]);
		else dBD = dBC;

		flag_A = flag_B = 0;

		if ( dAC > dBC && countR > 2) /* kind of edge detection	*/
		{	/* horizontal edge	*/
			sumR -= W1m * templ->e[1];	/* B, reduce influence of top neighbour	*/
			countR -= W1m;
			flag_B = 0;
		}
		else if ( dBC > dAC && countL > 2)
		{	/* vertical edge	*/
			sumL -= W1m * templ->e[0];	/* A, reduce influence of left neighbour	*/
			countL -= W1m;
			flag_A = 0;
		}
		//else
		//{
		//	sumR -= 1 * templ->e[1];	/* B, reduce influence of top neighbour	*/
		//	countR -= 1;
		//	sumL -= 1 * templ->e[0];	/* A, reduce influence of left neighbour	*/
		//	countL -= 1;
		//}
		if ( dBC > 3*dBD && dBC <=  2*dAC && countL > 1) /* kind of edge detection	*/
		{	/* vertical edge	*/
			sumL -= 1 * templ->e[2];	/* C, reduce influence of topleft neighbour	*/
			countL -= 1;
			if(countL > 1)
			{
				sumL -= 1 * templ->e[0];	/* reduce influence of left neighbour	*/
				countL -= 1;
			}
		}

		/* check prediction contexts, increase influence of neighbours 
		 * with same prediction context
		 */
		if ( px ==  px_ptr[pos - 1] && flag_A)
		{
			sumL += Wpx * templ->e[0];	/* A */
			countL += Wpx;
		}
		if ( px ==  px_ptr[pos - im->width] && flag_B)
		{
			sumR += Wpx * templ->e[1];	/* B */
			countR += Wpx;
		}
		if ( px ==  px_ptr[pos - 1 - im->width])
		{
			sumL += Wpx * templ->e[2];	/* C */
			countL += Wpx;
		}
		if (c < (signed)im->width-1) /* in general not requires since px is is always different at top left column*/
		{
			if ( px ==  px_ptr[pos + 1 - im->width])
			{
				sumR += Wpx * templ->e[3];	/* D */
				countR += Wpx;
			}
			if (r > 1 )
			{
				if ( px ==  px_ptr[pos + 1 - im->width - im->width])
				{
					sumR += templ->e[8];	/* I */
					countR++;
				}
			}
		}
		if (c < (signed)im->width-2) /* */
		{
			if ( px ==  px_ptr[pos + 2 - im->width])
			{
				sumR += templ->e[9];	/* J */
				countR++;
			}
		}
		if (r > 1 )
		{
			if ( px ==  px_ptr[pos - im->width - im->width])
			{
				sumR += templ->e[7];	/* H */
				countR++;
			}
			if ( px ==  px_ptr[pos-1 - im->width - im->width])
			{
				sumL += templ->e[6];	/* G */
				countL++;
			}
			if (c > 1 )
			{
				if ( px ==  px_ptr[pos-2 - im->width - im->width])
				{
					sumL += templ->e[12];	/* M */
					countL++;
				}
			}
			if (c < (signed)im->width-2 )
			{
				if ( px ==  px_ptr[pos+2 - im->width - im->width])
				{
					sumR += templ->e[16];	/* Q */
					countR++;
				}
			}
		}
		if (c > 1 )
		{
			if ( px ==  px_ptr[pos-2- im->width])
			{
				sumL += templ->e[5];	/* F */
				countL++;
			}
			if (px ==  px_ptr[pos-2])
			{
				sumL += templ->e[4];	/* E */
				countL++;
			}
		}
	} /* if (c > 0 && r > 0 )*/
	
	/* estimate based on adjacent error magnitudes	*/
	if (countL)
	{
		p_predE->estimate[1] = (double)(sumL) / (countL);
	}
	else 	p_predE->estimate[1] = 0.;
	if (countR)
	{
		p_predE->estimate[2] = (double)(sumR) / (countR);
	}
	else 	p_predE->estimate[2] = 0.;

	/* get estimate from prediction context	*/
	if (count_px)
	{
		//long fac = (count / 3);
		/* sum_abserr / count = average abs error in this context
		 * 8 * gives prediction-context based influence a certain weight
		 * compared to influence of neighbours
		 */
		/* mean error in current context	*/
		p_predE->estimate[0] = (double)sum_abserr / count_px;

	}
	else
	{
		p_predE->estimate[0] = 0;
	}
	
	/* if a single estimate is extremely good, then use it exclusiveley	*/
	//if (p_predE->weights_e[px][0] > 0.7)
	//{
	//	eHat_d = p_predE->estimate[0];
	//	sum_w = 1.;
	//}
	//else 	if (p_predE->weights_e[px][1] > 0.7)
	//{
	//	eHat_d = p_predE->estimate[1];
	//	sum_w = 1.;
	//}
	//else 	if (p_predE->weights_e[px][2] > 0.7)
	//{
	//	eHat_d = p_predE->estimate[2];
	//	sum_w = 1.;
	//}


	//if (p_predE->weights_e[px][0] > 0.7 || p_predE->weights_e[px][1] > 0.7)
	//{
	//	eHat_d = p_predE->weights_e[px][0] * p_predE->estimate[0] + 
	//					 p_predE->weights_e[px][1] * p_predE->estimate[1];
	//	sum_w = p_predE->weights_e[px][0] + p_predE->weights_e[px][1];
	//}

	if (!p_predE->weight_flag_cnt)
	{
		/* use prediction-context based forecast	*/
		eHat_d = 4*p_predE->weights_e[px][0] * p_predE->estimate[0] + 
						   p_predE->weights_e[px][1] * p_predE->estimate[1] + 
						   p_predE->weights_e[px][2] * p_predE->estimate[2];
		sum_w = 4 * p_predE->weights_e[px][0] + 
							p_predE->weights_e[px][1] + p_predE->weights_e[px][2];
		p_predE->countdown--;
		if (!p_predE->countdown) 
			p_predE->weight_flag_cnt = 3;
	}
	else /* do some averaging	*/
	{
		//double fac = ((countL + countR) / 3);

		//eHat_d = fac * p_predE->estimate[0] + (countL + countR) * p_predE->estimate[1];
		//sum_w = fac + (countL + countR);
		/* the weight for the prediction-context based estimate gets a weight, 
		 * which is approximately one third compared to the sum weight of the other	
		 */
		eHat_d = (p_predE->weights_e[px][1] + p_predE->weights_e[px][2]) * 0.3 * 
																					 p_predE->estimate[0] + 
							p_predE->weights_e[px][1] * p_predE->estimate[1] + 
						  p_predE->weights_e[px][2] * p_predE->estimate[2];
		sum_w = 1.3 * (p_predE->weights_e[px][1] + p_predE->weights_e[px][2]);

#ifdef NIXXX
		eHat_d = p_predE->estimate[0];
		sum_w = p_predE->weights_e[px][0];
		for ( i = 1; i < NUM_ESTIMATORS; i++)
		{
			eHat_d += p_predE->weights_e[px][i] * p_predE->estimate[i];
			sum_w += p_predE->weights_e[px][i];
			//if (p_predE->weights_e[px][i] > p_predE->weights_e[px][i-1] )
			//{
			//	eHat_d = p_predE->weights_e[px][i] * p_predE->estimate[i];
			//	sum_w = p_predE->weights_e[px][i];
			//}
		}
#endif
			//if (p_predE->weights_e[px][0] > 0.7 && count_px > 50)
			//{
			//	eHat_d = (eHat_d + p_predE->estimate[0] * p_predE->weights_e[px][0]) /
			//				(1 + p_predE->weights_e[px][0]);
			//}

		//if (!p_predE->weight_flag[px]  && 1)
		{
			if (p_predE->weights_e[px][0] > 0.7 && count_px > 50)
			{
				/* switch to prediction-context based forecast	*/
				p_predE->weight_flag[px] = 1;
				if (p_predE->weight_flag_cnt)
				{
					p_predE->weight_flag_cnt--;
					if (!p_predE->weight_flag_cnt) 
						p_predE->countdown = im->width;
				}
			}
		}
	}

	eHat = (long)floor( eHat_d * TH_FAC / sum_w + 0.5);
	if ( (unsigned)eHat >= num_abs_scaled_errors)
	{
		eHat = num_abs_scaled_errors - 1;
	}
	assert( (unsigned)eHat < num_abs_scaled_errors);


	return eHat;
}

/*---------------------------------------------------------------
 *	update_weights_e()
 *---------------------------------------------------------------*/
void update_weights_e( PREDE *p_predE, unsigned short px, 
												 long abs_err, /* TEMPLATE_E *templ,*/
												 unsigned int reset)
{
	double *p_w = NULL;
	//double *p_mean = NULL;
	double sum_inverse_varN;
	double diff;
	unsigned int i;
	double min_var;

	p_predE->cnt[px]++; /* count the samples in the context  == N	*/
	p_w = p_predE->weights_e[px];

	/* compute variances 
	 * take abs(eptr[pos]) as mean,
	 * templ->e[i] = abs(eptr[pos-X]) is the deviation
	 */
	min_var = 9999999;
	for ( i = 0; i < NUM_ESTIMATORS; i++)
	{	   	 
		diff = p_predE->estimate[i] - abs_err; /* abs_err = abs(eptr[pos])	*/
		p_predE->varianceN[px][i] += diff * diff; /* variance times N	*/
		//p_predE->varianceN[px][i] += fabs(diff * diff * diff); /* variance times N	*/
		//p_predE->varianceN[px][i] += fabs( diff); /* variance times N	*/
		if (min_var > p_predE->varianceN[px][i]) min_var = p_predE->varianceN[px][i];
	}
	
	/*
	 *  sum{ 1/ var_i} = sum{ N / (var_i*N)}
	 *  sumN = sum{ 1 / (var_i*N)}
	 */
	sum_inverse_varN = 0.;
	for ( i = 0; i < NUM_ESTIMATORS; i++)
	{	   	 
		//sum_inverse_varN += 1. / p_predE->varianceN[px][i];
		sum_inverse_varN += 1. / (p_predE->varianceN[px][i] + 0.00001); /* save guard	*/
	}

	/*
	 *         1 / var_i         1/ (var_i * N)				1
	 *  w_i = -------------  = ------------------ = ------------------
	 *        sum{ 1/ var_i}   sum{ 1 / (var_i*N)}  (var_i*N) * sumN
	 */
	for ( i = 0; i < NUM_ESTIMATORS; i++)
	{	   	 
		p_w[i] = 1. / (p_predE->varianceN[px][i] * sum_inverse_varN); 
	}

	/* prevent overflow by regular down scaling	*/
	if ( (p_predE->cnt[px] % reset) == 0 && 
		min_var > 1) /* prevent scaling to zero (not required in double calculations)	*/
	{
		for ( i = 0; i < NUM_ESTIMATORS; i++)
		{	   	 
			p_predE->varianceN[px][i] *= 0.5;
		}
	}
}

#else

/*---------------------------------------------------------------
 *	get_averaged_error()
 * based on template and context information
 * weighting:
 *       n o p            1  1  1
 *	   m g h i q       1  1  2  1 1 
 *	 l f c b d j r   1 1  2 5/2 2 1 1
 *   k e a x         1 2 5/2 x
 *---------------------------------------------------------------*/
unsigned long get_averaged_error( IMAGE *im, long *eptr, 
							unsigned long pos, int r, int c,
							unsigned short *px_ptr,
							unsigned long sum_abserr, /* averaged absolut error	*/
							unsigned long count_px)		/* in current context			*/
{
	int flag_A, flag_B;
	unsigned long sum;
	long	dAC, dBC, dBD;
	unsigned int	count;
	unsigned long abs_A, abs_B, abs_C, abs_D;
	unsigned long abs_E, abs_F, abs_G, abs_H, abs_I, abs_J;
	unsigned long abs_K, abs_L, abs_M, abs_N;
	unsigned long abs_O, abs_P, abs_Q, abs_R;

	sum = 0;
	count = 0;

	/* exception handling	
	 * typically, error at (r, c) = (0,0) is high
	 * but not its neighbours, so exclude (0,0)
	 * context for (0,0) will be overwritten in coder_
	 */

	/* TH_FAC == scaling for AC context quantisation	*/
	if ( c + r < 4) return (1 * TH_FAC); /* moderate estimate of 1	*/

	if (c > 0)	/* not the first column	*/
	{
		abs_A = abs( eptr[pos-1]);
		sum += 5 * abs_A;	/* left neighbour, weight 4*/
		count += 5;
		if (c > 1)
		{
			abs_E = abs( eptr[pos-2]);
			sum += 2 * abs_E;	/* second left neighbour, weight 2 */
			count += 2;
			if (c > 2)
			{
				abs_K = abs( eptr[pos-3]);
				sum += abs_K;	/* 3rd left neighbour, weight 1 */
				count++;
			}
		}
	}
	if (r > 0)
	{
		abs_B = abs( eptr[pos-im->width]);
		sum += 5 * abs_B; 	/* top neighbour, weight 4*/
		count += 5;
		if (c > 0)
		{
			abs_C = abs( eptr[pos-1-im->width]);
			sum += 2 * abs_C;	/* topleft neighbour, weight 2*/
			count += 2;
			if (c > 1)
			{
				abs_F = abs( eptr[pos-2-im->width]);
				sum += abs_F;	/* second left-top  neighbour, weight 1*/
				count++;
				if (c > 2)
				{
					abs_L = abs( eptr[pos-3-im->width]);
					sum += abs_L;	/* second left-top  neighbour, weight 1*/
					count++;
				}
			}
		}
		if (c < (signed)im->width-1)
		{
			abs_D = abs( eptr[pos+1-im->width]);
			sum += 2 * abs_D;/* top-right neighbour, weight 2*/
			count += 2;
			if (c < (signed)im->width-2)
			{
				abs_J = abs( eptr[pos+2-im->width]);
				sum += abs_J;
				count++;
				if (c < (signed)im->width-3)
				{
					abs_R = abs( eptr[pos+3-im->width]);
					sum += abs_R;
					count++;
				}
			}
		}
	}
	if (r > 1)
	{
		abs_H = abs( eptr[pos  -im->width-im->width]);
		sum += 2 * abs_H;
		count += 2;
		if (c > 0)
		{
			abs_G = abs( eptr[pos-1-im->width-im->width]);
			sum += abs_G;
			count++;
			if (c > 1)
			{
				abs_M = abs( eptr[pos-2-im->width-im->width]);
				sum += abs_M;
				count++;
			}
		}
		if (c < (signed)im->width-1)
		{
			abs_I = abs( eptr[pos+1-im->width-im->width]);
			sum += abs_I;
			count++;
			if (c < (signed)im->width-2)
			{
				abs_Q = abs( eptr[pos+2-im->width-im->width]);
				sum += abs_Q;
				count++;
			}
		}
	}
	if (r > 2)
	{
		abs_O = abs( eptr[pos  -im->width-im->width-im->width]);
		sum += abs_O;
		count++;
		if (c > 0)
		{
			abs_N = abs( eptr[pos-1-im->width-im->width-im->width]);
			sum += abs_N;
			count++;
		}
		if (c < (signed)im->width-1)
		{
			abs_P = abs( eptr[pos+1-im->width-im->width-im->width]);
			sum += abs_P;
			count++;
		}
	}

	/* finetuning of weights	*/
	if (c > 0 && r > 0 )
	{
		/* direction based influence; does slightly improve bitrate */
		dAC = abs( abs_C - abs_A);
		dBC = abs( abs_C - abs_B);
		if (c < (signed)im->width-1)	dBD = abs( abs_D - abs_B);
		else dBD = dBC;

		flag_A = flag_B = 1;

		if ( dAC > dBC && count > 2) /* kind of edge detection	*/
		{	/* horizontal edge	*/
			sum -= 3 * abs_B;	/* reduce influence of top neighbour	*/
			count -= 3;
			flag_B = 0;
		}
		else if ( dBC > dAC && count > 2)
		{	/* vertical edge	*/
			sum -= 3 * abs_A;	/* reduce influence of left neighbour	*/
			count -= 3;
			flag_A = 0;
		}
		if ( dBC > 3*dBD && dBC <=  2*dAC && count > 1) /* kind of edge detection	*/
		{	/* vertical edge	*/
			sum -= 1 * abs_C;	/* reduce influence of topleft neighbour	*/
			count -= 1;
			if(count > 1)
			{
				sum -= 1 * abs_A;	/* reduce influence of left neighbour	*/
				count -= 1;
			}
		}
	
		/* check prediction contexts, increase influence of neighbours 
		 * with same prediction context
		 */
		if ( px_ptr[pos] ==  px_ptr[pos - 1] && flag_A)
		{
			sum += 2 * abs_A;
			count += 2;
		}
		if ( px_ptr[pos] ==  px_ptr[pos - im->width] && flag_B)
		{
			sum += 2 * abs_B;
			count += 2;
		}
		if ( px_ptr[pos] ==  px_ptr[pos - 1 - im->width])
		{
			sum += 2 * abs_C;
			count += 2;
		}
		if (c < (signed)im->width-1) /* in general not requires since px is is always different at top left column*/
		{
			if ( px_ptr[pos] ==  px_ptr[pos + 1 - im->width])
			{
				sum += 2 * abs_D;
				count += 2;
			}
			if (r > 1 )
			{
				if ( px_ptr[pos] ==  px_ptr[pos + 1 - im->width - im->width])
				{
					sum += abs_I;
					count++;
				}
			}
		}
		if (c < (signed)im->width-2) /* */
		{
			if ( px_ptr[pos] ==  px_ptr[pos + 2 - im->width])
			{
				sum += abs_J;
				count++;
			}
		}
		if (r > 1 )
		{
			if ( px_ptr[pos] ==  px_ptr[pos - im->width - im->width])
			{
				sum += abs_H;
				count++;
			}
			if ( px_ptr[pos] ==  px_ptr[pos-1 - im->width - im->width])
			{
				sum += abs_G;
				count++;
			}
			if (c > 1 )
			{
				if ( px_ptr[pos] ==  px_ptr[pos-2 - im->width - im->width])
				{
					sum += abs_M;
					count++;
				}
			}
			if (c < (signed)im->width-2 )
			{
				if ( px_ptr[pos] ==  px_ptr[pos+2 - im->width - im->width])
				{
					sum += abs_Q;
					count++;
				}
			}
		}
		if (c > 1 )
		{
			if ( px_ptr[pos] ==  px_ptr[pos-2- im->width])
			{
				sum += abs_F;
				count++;
			}
			if ( px_ptr[pos] ==  px_ptr[pos-2])
			{
				sum += abs_E;
				count++;
			}
		}
	}

	sum *= TH_FAC; /* scale already here as the mean magnitude of px should also be scaled */
	if (count_px)
	{
		long fac = (count / 3);
		/* sum_abserr / count = average abs error in this context
		 * 8 * gives prediction-context based influence a certain weight
		 * compared to influence of neighbours
		 */
		//if ( ((sum_abserr + (count_px >> 1) ) / count_px) > 0)
		{
			sum += ( fac * sum_abserr * TH_FAC + (count_px >> 1) ) / count_px;
			count += fac;
		}
	}
	if (count)  sum = (sum + (count >> 1) ) / count;

	return sum;
}

#endif /* INTEGER_COMP */

/*---------------------------------------------------------------
 *	get_AC_context()
 * based on weighted absolute errors
 *---------------------------------------------------------------*/
unsigned short get_AC_context( IMAGE *im, long *eptr, 
							unsigned long pos, int r, int c,
							unsigned short *px_ptr,
							unsigned int th_AC[],
							unsigned short used_AC_contexts,
							unsigned long sum_abserr, /* averaged absolut error	*/
							unsigned long count_px,		/* in current context			*/
							PREDE *p_predE, TEMPLATE_E *templ,
							unsigned long num_abs_scaled_errors,
							unsigned long *abs_err)
{
	unsigned short cx;

	/* get averaged prediction error in neighbourhood */
  //abs_err = get_averaged_error( im, eptr, pos, r, c, px_ptr, 
		//sum_abserr, count_px);
  *abs_err = get_averaged_error( im, eptr, pos, r, c, px_ptr, 
		sum_abserr, count_px, p_predE, templ, num_abs_scaled_errors);
#ifdef CHECK
	fprintf( stderr, "abs_err= %d", *abs_err);
#endif

	for (cx = 0; cx < used_AC_contexts-1; cx++)
	{	/* minus 1 for the case that if-clause is not fulfilled
		 * ==> cx = used_AC_contexts-1
		 */
		/* compare with thresholds	*/
		if ( *abs_err <= th_AC[cx])
			break;
	}

	assert( cx < NUM_AC_CONTEXTS);
	assert( cx < used_AC_contexts);
	return cx;
}
