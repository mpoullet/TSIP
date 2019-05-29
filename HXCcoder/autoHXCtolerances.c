/*****************************************************
 * File..: autoTolerances.c
 * Desc..: automatic tolerance-selections for HXC2 Colour-map Image Processing
 * Author: T.Strutz
 * Date..: 21.08.2014
 * changes
 * 29.08.2014 use stdDev of differences
 ******************************************************/
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "autoConfig.h"
#include "types.h"

/*--------------------------------------------------------
* autoTolerances()
* Auto HXC tolerances in Config dialog
*-------------------------------------------------------*/

void autoTolerances( PicData *pd,
										unsigned int range,
										AutoConfig *aC, 
										unsigned int comp)
{
	int d[6], i;
	long val;
	double fac;
	double sigma[6];
	unsigned int r, c;
	unsigned long pos, count;
	double sum[6], sum2[6];

	d[0] = -1;			/* position A	*/
	d[1] = -(signed)pd->width;	/* position B	*/
	d[2] = -1-(signed)pd->width;	/* position C	*/
	d[3] = +1-(signed)pd->width;	/* position D	*/
	d[4] = -2;	/* position E	*/
	d[5] = -(signed)pd->width-(signed)pd->width;	/* position F	*/

	/* reset the sums	*/
	for ( i = 0; i < 6; i++)
	{
		sum[i] = 0.;
		sum2[i] = 0.;
	}

	/* Process only pixels with a whole pattern inside the image and find the sums of the values in each position */
	count = 0;
	for( r = 2; r < pd->height; r++)
	{
		for( c = 2; c < ( pd->width -1); c++)
		{
			count++;

			pos = c + r * pd->width;
			for ( i = 0; i < 6; i++)
			{
				val = abs( (long)pd->data[comp][pos] - (long)pd->data[comp][pos + d[i]]);
				sum[i] += val;
				sum2[i] += val * val;
			}
		}
	}

	/* Find all sigmas */ 
	fac = 1. / count;
	for ( i = 0; i < 6; i++)
	{
		sum[i] = fac * sum[i];
		sum2[i] = fac * sum2[i];
		sigma[i] = sum2[i] - sum[i] * sum[i];
		if (sigma[i] > 0.)	sigma[i] = sqrt( sigma[i]);
		else								sigma[i] = 0;

		//aC->HXC2tolerance[comp][i] = (unsigned int)( sigma[i] + .5);
		aC->HXC2tolerance[comp][i] = (unsigned int)( 0.08 * sum[i] + .5);
	}
}