/*****************************************************************
 *
 * File..:	get_AC_thresholds.c
 * Descr.:	maps distribution of prediction errors to AC contetxs
 * Author.:	Tilo Strutz
 * Date..: 	02.04.2013
 * changes:
 * 09.04.2013 error distribution is externally determined 
 * 15.04.2013 complete new determination of thresholds
 * 16.04.2013 determination of thresholds based on std.deviation
 * 17.04.2013 adaptive determination of number of AC contexts
 * 06.05.2013 more than one model_AC allowed
 * 23.07.2013 minimum of two coding contexts (instead of three)
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"

#define DD 0

// #define CHECK 99

/*---------------------------------------------------------------
 *	get_AC_thresholds()
 *
 *---------------------------------------------------------------*/
unsigned short  get_AC_thresholds( IMAGE *im, CXE_STRUC *cx_struc, 
				unsigned int th_AC[],		/* container for final thresholds	*/
				unsigned int model_AC[],	/* model sizes for AC	*/
				unsigned long num_abs_scaled_errors)	/* RANGEd2 * TH_FAC +1 */
{
	long 	RANGEd2, RANGEm1d2, RANGE;
	double min_burden, min_burden_old, fac, logN;	
	double relative_burden, relative_burden_old = 0.0;
	double threshold = 0.35;
//	unsigned char *used_flag = NULL;
	unsigned int i, idx;
	unsigned short k, kcomb=0;
	unsigned short used_states;
	unsigned long num_states;

	RANGE = im->max_value + 1;
	RANGEd2 = RANGE / 2;
	RANGEm1d2 = (RANGE - 1) / 2;
	
	num_states = num_abs_scaled_errors;
#ifdef CHECK
	fprintf( stderr, "\n num_states = %d", num_states);
#endif

#ifndef 	FIXED_AC_CONTEXTS

	used_states = 0;
	/* for all possible contexts	*/
	for ( k = 0; k < num_states; k++)
	{
#ifdef CHECK
	fprintf( stderr, ", %d", k);
#endif

		cx_struc[k].num = k;
		/* determine total number of events for this context	*/
		cx_struc[k].num_events = 0;
		for ( i = 0; i < (unsigned)RANGE; i++)
		{
			cx_struc[k].num_events += cx_struc[k].distribution[i];
			/* distribution was set in prediction loop, encoder.c	*/
		}

		/* compute entropy for context k	*/
		/* H = 1. /(N*log(2)) * SUM {cnt * [ log(N) - log(cnt)]}	*/
		if (cx_struc[k].num_events)
		{
			logN = log( (double)cx_struc[k].num_events);
			fac = 1.0 / (cx_struc[k].num_events * LOG2);
			cx_struc[k].H = 0.;
			for ( i = 0; i < (unsigned)RANGE; i++)
			{
				unsigned long val = cx_struc[k].distribution[i];
				if (val)
				{
					cx_struc[k].H += val * (logN - log( (double)val) );
				}
			}
			cx_struc[k].H *= fac;
			used_states++;
		}
		else cx_struc[k].H = 0.;
	}
	if (coding_verbose)
	{
		unsigned long count = 0;
		for (k = 0; k < num_states; k++)
		{
			count += cx_struc[k].num_events;
		}
		printf( "\n total number of events: %d", count); 
	}

	/* remove empty states	*/
	idx = 0;
	/* for all possible contexts	*/
	//for (k = 0; k < num_states, idx < used_states; k++)
	for (k = 0; k < num_states && idx < used_states; k++)
	{
		while (cx_struc[k].num_events == 0 && k < num_states)
		{
			k++;	/* look for next non-empty state	*/
		}
		if (cx_struc[k].num_events == 0)
			break; /* nothing left	*/
		if (idx < k) /* only after gaps	*/
		{
			unsigned long *tmp;
			/* copy	*/
			cx_struc[idx].num = cx_struc[k].num;
			cx_struc[idx].num_events = cx_struc[k].num_events;
			cx_struc[idx].H = cx_struc[k].H;
			/* exchange pointer to distributions	*/
			tmp = cx_struc[k].distribution;
			cx_struc[k].distribution = cx_struc[idx].distribution;
			cx_struc[idx].distribution = tmp;
		}
		idx++;
	}

	/* compute entropies of combined states	*/
	for (k = 0; k < used_states-1; k++)
	{
		/* compute joint entropy of merged data	*/
		cx_struc[k].Hcomb = 0.0;
		/* H = 1. /(N*log(2)) * SUM {cnt * [ log(N) - log(cnt)]}	*/
		logN = log( (double)cx_struc[k].num_events + cx_struc[k+1].num_events);
		fac = 1.0 / ( (cx_struc[k].num_events + cx_struc[k+1].num_events) * LOG2);
		for ( i = 0; i < (unsigned)RANGE; i++)
		{
			unsigned long val;

			val = cx_struc[k].distribution[i] + cx_struc[k+1].distribution[i];
			if (val)
			{
				cx_struc[k].Hcomb += val * (logN - log( (double)val));
			}
		}
		cx_struc[k].Hcomb *= fac;

		/* combined H minus sum of single H	
		 * => determination of performance loss for merged data
		 */
		cx_struc[k].Hburden = 
			cx_struc[k].Hcomb * (cx_struc[k].num_events+cx_struc[k+1].num_events) -
					(cx_struc[k  ].H * cx_struc[k  ].num_events + 
					 cx_struc[k+1].H * cx_struc[k+1].num_events);

		if (cx_struc[k].Hburden < 0) 
			cx_struc[k].Hburden = 0; /* rounding error, should not happen	*/
		if (coding_verbose && DD)
			if (cx_struc[k].num_events) printf( "\n %2d: %2d %5d, %6.3f, %6.3f, %6.3f", k, 
									cx_struc[k].num, cx_struc[k].num_events, 
														cx_struc[k].H, cx_struc[k].Hcomb, cx_struc[k].Hburden);
	} /* for k*/
	if (coding_verbose && DD)
	{
		if (cx_struc[k].num_events) 
		{
			printf( "\n %2d: %2d %5d, %6.3f", k, cx_struc[k].num, 
							cx_struc[k].num_events,	cx_struc[k].H);
		}
	}

	/* try to combine adjacent states until max. number of contexts is reached	*/
	min_burden = 1e9; /* something large	*/
	while (1 /* used_states > NUM_AC_CONTEXTS */)
	{
		min_burden_old = min_burden;
		min_burden = 1e9; /* something large	*/

		for (k = 0; k < used_states-1; k++)
		{	/* look for smalest burden	*/
			if (min_burden > cx_struc[k].Hburden)
			{
				min_burden = cx_struc[k].Hburden;
				kcomb = k;
			}
		}
		if (coding_verbose && DD)
		{ /*  burden	*/
			fprintf( stderr, "\n burden: %6.2f", min_burden);
			fprintf( stderr, ", kcomb: %d", kcomb);
			if (min_burden_old > 0)
				fprintf( stderr, ", %6.1f%%", 100 * min_burden / min_burden_old);
			else
				fprintf( stderr, ", %6.1f%%", 0.);
		}


		/* linear determination of threshold value
		 * anchors: (20,0.2); (10,0.15)
		 */
		threshold = ((signed)used_states - 10) * 0.005 + 0.15;
		/* stop merging if number of states is smaller than 4 
		 * go ahead if 
		 *		-number is higher than allowed
		 *		-burden is negligible
		 *		-relation to previous burden is lower than 15 %
		 */
		relative_burden = min_burden / min_burden_old - 1.;
		if (
				(used_states < 3) || 
				//(used_states < 4) || 
			  (used_states <= NUM_AC_CONTEXTS &&
				 min_burden_old > 30 && 
//				 min_burden / min_burden_old > 1.15) ) /* > 1.15 */
//				 (min_burden / min_burden_old > 1.2) /* > 1.2 */
						//(relative_burden > threshold || 
						// relative_burden + relative_burden_old > threshold)
						(relative_burden > 0.2 || 
						 (relative_burden > 0.9*threshold &&
						  relative_burden_old > 0.7*threshold))
				  )
				 ) 
 			break;
		relative_burden_old = relative_burden;

		/* combine distributions with least	burden */
		/* keep number (=abs_err) from upper bin	*/
		cx_struc[kcomb].num = cx_struc[kcomb+1].num;
		/* add the number of events	*/
		cx_struc[kcomb].num_events += cx_struc[kcomb+1].num_events;
		/* new entropy = combined entropy	*/
		cx_struc[kcomb].H = cx_struc[kcomb].Hcomb;
		for ( i = 0; i < (unsigned)RANGE; i++)
		{
			cx_struc[kcomb].distribution[i] += cx_struc[kcomb+1].distribution[i];
		}
		
		/* move following states */
		used_states--;
		num_states--;
		{
			unsigned long *tmp;
			tmp = cx_struc[kcomb+1].distribution;
			for (k = kcomb+1; k < used_states /* is already one less*/; k++)
			{

				cx_struc[k].num = cx_struc[k+1].num;
				cx_struc[k].num_events = cx_struc[k+1].num_events;
				cx_struc[k].H = cx_struc[k+1].H;
				cx_struc[k].Hcomb = cx_struc[k+1].Hcomb;
				cx_struc[k].Hburden = cx_struc[k+1].Hburden;
				/* exchange pointer to distributions	*/
				cx_struc[k].distribution = cx_struc[k+1].distribution;
			}
			/* all pointers to distribution arrays must be kept for free-process */
			cx_struc[used_states].distribution = tmp;
		}

		/* compute entropy and burden of combined distributions and next one
		 */
		/* compute entropy of combined data	*/
		cx_struc[kcomb].Hcomb = 0.0;
		/* H = 1. /(N*log(2)) * SUM {cnt * [ log(N) - log(cnt)]}	*/
		logN = log( (double)cx_struc[kcomb].num_events + cx_struc[kcomb+1].num_events);
		fac = (cx_struc[kcomb].num_events + cx_struc[kcomb+1].num_events) * LOG2;
		fac = 1.0 / fac;

		for ( i = 0; i < (unsigned)RANGE; i++)
		{
			unsigned long val;

			val = cx_struc[kcomb].distribution[i] + cx_struc[kcomb+1].distribution[i];
			if (val)
			{
				cx_struc[kcomb].Hcomb += val * (logN - log( (double)val));
			}
		}
		cx_struc[kcomb].Hcomb *= fac;

		cx_struc[kcomb].Hburden = 
			cx_struc[kcomb].Hcomb * 
			(cx_struc[kcomb].num_events+cx_struc[kcomb+1].num_events) -
					(cx_struc[kcomb  ].H * cx_struc[kcomb  ].num_events + 
					 cx_struc[kcomb+1].H * cx_struc[kcomb+1].num_events);

		if (cx_struc[kcomb].Hburden < 0) 
		{	/* rounding error, should not happen	*/
			cx_struc[kcomb].Hburden = 0; 
		}


		if (kcomb) /* also predecessor (if available) must be updated	*/
		{
			k = kcomb - 1;
			/* compute combined entropy and burden between current and previous	*/
			cx_struc[k].Hcomb = 0.0;
			/* H = 1. /(N*log(2)) * SUM {cnt * [ log(N) - log(cnt)]}	*/
			logN = log( (double)cx_struc[k].num_events + cx_struc[kcomb].num_events);
			fac = 1.0 / ( (cx_struc[k].num_events + cx_struc[kcomb].num_events) * LOG2);
			for ( i = 0; i < (unsigned)RANGE; i++)
			{
				unsigned long val;

				val = cx_struc[k].distribution[i] + cx_struc[kcomb].distribution[i];
				if (val)
				{
					cx_struc[k].Hcomb += val * (logN - log( (double)val));
				}
			}
			cx_struc[k].Hcomb *= fac;

			cx_struc[k].Hburden = 
				cx_struc[k].Hcomb * 
				(cx_struc[k].num_events + cx_struc[kcomb].num_events) -
						(cx_struc[k    ].H * cx_struc[k    ].num_events + 
						 cx_struc[kcomb].H * cx_struc[kcomb].num_events);
			if (cx_struc[k].Hburden < 0) 
			{
				cx_struc[k].Hburden = 0; /* rounding error	*/
			}
		}

		if (coding_verbose && 0)
		{
			printf( "\n");	
			for (k = max(0, (int)kcomb-1); k < used_states; k++)
			{
				if (cx_struc[k].num_events) printf( "\n %2d: %2d %5d, %6.3f, %6.3f, %6.3f", 
					k, cx_struc[k].num, cx_struc[k].num_events,	
					cx_struc[k].H, cx_struc[k].Hcomb, cx_struc[k].Hburden);
			}
		}

	} /* while */


	/*
	 * determine thresholds for AC contexts and compute model width
	 *
	 */
	if (coding_verbose)
	{
		printf( "\n k: num, num_events H, Hcomb, Hburden, model_AC");
	}
	for (k = 0; k < used_states; k++)
	{
		unsigned long val;

		th_AC[k] = cx_struc[k].num;

		val = cx_struc[k].distribution[RANGEd2]; /* centre */
		if ( (k && val < (13 * cx_struc[k].num_events ) >> 4 ) ||
			   (     val < (13 * cx_struc[k].num_events ) >> 4))
		{
			for ( i = 1; i < (unsigned)RANGEd2; i++)
			{
				val += cx_struc[k].distribution[RANGEd2 + i];
				val += cx_struc[k].distribution[RANGEd2 - i];
				if ( (k && val >= (13 * cx_struc[k].num_events ) >> 4) ||
							    (val >= (13 * cx_struc[k].num_events ) >> 4))
					break;
			}
			model_AC[k] = i;
		}
		else
		{
			model_AC[k] = 0;
		}

		/* only the first is allowed to contain only zeros	*/
//		if (k && model_AC[k] == 0) model_AC[k] = 1; 

		/* ensure that model size is not decreasing
		 * required for differential coding	
		 */
		if (k && model_AC[k] < model_AC[k-1]) 
			model_AC[k] = model_AC[k-1];

		if (coding_verbose)
		{
				printf( "\n %2d: %3d %7d, %6.3f, %6.3f, %8.3f, %3d", k, cx_struc[k].num, 
					cx_struc[k].num_events, cx_struc[k].H, cx_struc[k].Hcomb, 
					cx_struc[k].Hburden,	model_AC[k]);
		}
	}
	if (coding_verbose)
	{
		unsigned long count = 0;
		for (k = 0; k < used_states; k++)
		{
			count += cx_struc[k].num_events;
		}
		printf( "\n total number of events: %d", count); 
	}

	/* distribution of prediction errors in all contexts	*/
	if (coding_verbose && 0)
	{
		FILE *out=NULL;

		out = fopen( "distrib.txt", "wt");
		fprintf( out, "# each col = one context");
		for (i = 0; i < (unsigned)RANGE; i++)
		{
			fprintf( out, "\n%4d", i - RANGEd2);
			for (k = 0; k < used_states; k++)
			{
				fprintf( out, " %6.3f", 
					(double)cx_struc[k].distribution[i] / cx_struc[k].num_events);
			}
		}
		fprintf( out, "\n");
		fclose( out);
	}
#else
	/* use fixed thresholds as in WuM97 */
	used_states = 8;
	th_AC[0] = 5;
	th_AC[1] = 15;
	th_AC[2] = 25;
	th_AC[3] = 42;
	th_AC[4] = 60;
	th_AC[5] = 85;
	th_AC[6] = 140;
	th_AC[7] = 256;
	model_AC[0] = 3;
	model_AC[1] = 8;
	model_AC[2] = 13;
	model_AC[3] = 21;
	model_AC[4] = 30;
	model_AC[5] = 43;
	model_AC[6] = 70;
	model_AC[7] = 128;
#endif

	return used_states;
}
