/**********************************************************************
 * File...: two_opt_heuristic.c
 * Purpose: 2-opt algorithm
 * Author.: Tilo Strutz
 * Date:..: 18.05.2015
 * changes:
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "k-opt.h"


/*----------------------------------------------------
 * two_opt_heuristic()
 * 
 *----------------------------------------------------*/
unsigned long two_opt_heuristic( int N, int *C, int *pv, 
											 unsigned long bestCosts)
{
	int pos, len, max_len;
	int stop_2opt;
	int *seg = NULL;
	int loops = 0, max_flips;
	unsigned long actualCosts;

	CALLOC( seg, N, int);

	if (N > 256) max_len = (int)( 23. * log( (double)N) );
	else max_len = N/2;
	max_flips = max_len * N/2;


	fprintf( stderr, "\n"); 
	/* process as long as a modification towards a shorter tour was possible	*/
	do
	{
		loops++;

		stop_2opt = 1;

		//for (len = 2; len < N / 2; len++)
		for (len = max_len; len > 1; len--) /* start with longest segment length	*/
		{ /* for all segment length shorter than full length	*/
			for (pos = 0; pos <= N-len; pos++) 
			{ /* cannt go to last position as length must be inside N	*/
				/* from first vector element to last-1 element	*/

				fprintf( stderr, "\r%2d: %2d/%3d \tcosts: %d  ", loops, 
					len, pos, bestCosts);
				/* check all possible length of segments	*/
				get_segment( /*N,*/ pv, pos, seg, len);
				put_segment( /*N,*/ pv, pos, seg, len, 1); /* reverse order	*/

				/* compute total costs */
				actualCosts = get_costs( N, C, pv);
				if (bestCosts > actualCosts)
				{
					bestCosts = actualCosts;
					stop_2opt = 0; /* another run is needed	*/
				}
				else
				{
					put_segment( /*N,*/ pv, pos, seg, len, 0); /* restore original order	*/
				}
			} /* for pos	*/
		} /* for len	*/
		if (max_len > 20) max_len = (int)floor(max_len * 0.7); 
		/* reduce max segment length because improvements 
									  * become more and more improbable
										*/
	} while ((stop_2opt == 0) );

	FREE( seg);
	return bestCosts;
}
