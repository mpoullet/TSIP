/*****************************************************************
 *
 * File...:	estimate_noise.c
 * Purpose:	kind of noise quantification
 * Author.:	Tilo Strutz
 * Date...: 14.09.2011
 * changes:
 * 17.02.2012 other header file, use of Pict
 * 13.02.2014 normalisation of noise to bitdepth
 * 09.07.2014 exclude possible alpha channel from computation
 * 20.07.2014 bugfix: division by numChan instead of pd->channel
 * 20.07.2014 compute constance in RGB space
 *****************************************************************/
#include <stdlib.h>
#include <stdio.h>
//#include "types.h"
#include "stats.h"
#include "codec.h" /* definition of min()	*/

//#define DEBUG_NOISE


/*---------------------------------------------------------------
 *	estimate_noise()
 * application of directional Laplacian operator
 *          1       1          1          1        1  1 
 * 1 -2 1  -2    1 -2  1 -2   -2 1  -2 1   -2    -2  -2  -2 1
 *	   	    1             1          1       1  1   1   1
 *
 *---------------------------------------------------------------*/
unsigned long estimateNoise( PicData *pd, ImageFeatures * iF)
{
	int incr_flag;
	int h, v, d1, d2, c1, c2, c3, c4;
	long cp, m;
#ifdef EF_DIRECTION
	int e1, e2, e3, e4, f1, f2, f3, f4;
#endif
	unsigned int comp, numChan;
	unsigned long x_step, y_step;
	unsigned long x, y, pos, py;
	unsigned long count_const=0;
	unsigned long count=0;
	unsigned long long sum;
#ifdef DEBUG_NOISE
	static flag = 1;
	FILE *out=NULL;
	if (flag)
	{
		out = fopen( "noise.pgm", "wb");
		fprintf( out, "P2\n%d %d\n%d\n", pd->width-2, pd->height-2, 255);
	}
#endif

	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	x_step = (unsigned long)(2 * pd->width / 1000);
	if (x_step == 0) x_step = 1;
	y_step = (unsigned long)(2 * pd->height / 1000);
	if (y_step == 0) y_step = 1;

	sum = 0;
	/* for all pixels exlusing the border positions	*/
	for (y = 1, py = pd->width; y < pd->height-1; y+=y_step, py+=pd->width*y_step)
	{
		for (x = 1, pos = py+1; x < pd->width-1; x+=x_step, pos+=x_step)
		{
			incr_flag = 1;
			for (comp = 0; comp < numChan; comp++)
			{
				if (pd->data[comp][pos] != pd->data[comp][pos-1])
					incr_flag = 0;
				if (pd->data[comp][pos] != pd->data[comp][pos-pd->width]) 
					incr_flag = 0;

				cp = pd->data[comp][pos] + pd->data[comp][pos];
				cp = -cp; /* because data are unsigned	*/
				h = cp + pd->data[comp][pos-1] + pd->data[comp][pos+1];
				v = cp + pd->data[comp][pos-pd->width] + pd->data[comp][pos+pd->width];
				d1 = cp + pd->data[comp][pos-1-pd->width] + pd->data[comp][pos+1+pd->width];
				d2 = cp + pd->data[comp][pos-1+pd->width] + pd->data[comp][pos+1-pd->width];
				c1 = cp + pd->data[comp][pos-1] + pd->data[comp][pos-pd->width];
				c2 = cp + pd->data[comp][pos+1] + pd->data[comp][pos-pd->width];
				c3 = cp + pd->data[comp][pos+1] + pd->data[comp][pos+pd->width];
				c4 = cp + pd->data[comp][pos-1] + pd->data[comp][pos+pd->width];
	#ifdef EF_DIRECTION
				e1 = cp + pd->data[comp][pos-1] + pd->data[comp][pos+1-pd->width];
				e2 = cp + pd->data[comp][pos-1] + pd->data[comp][pos+1+pd->width];
				e3 = cp + pd->data[comp][pos+1] + pd->data[comp][pos-1-pd->width];
				e4 = cp + pd->data[comp][pos+1] + pd->data[comp][pos-1+pd->width];

				f1 = cp + pd->data[comp][pos-pd->width] + pd->data[comp][pos+1+pd->width];
				f2 = cp + pd->data[comp][pos-pd->width] + pd->data[comp][pos-1+pd->width];
				f3 = cp + pd->data[comp][pos+pd->width] + pd->data[comp][pos+1-pd->width];
				f4 = cp + pd->data[comp][pos+pd->width] + pd->data[comp][pos-1-pd->width];
	#endif
				m = min( abs(h), abs(v));
				m = min( m, abs(d1));
				m = min( m, abs(d2));
				m = min( m, abs(c1));
				m = min( m, abs(c2));
				m = min( m, abs(c3));
				m = min( m, abs(c4));
	#ifdef EF_DIRECTION
				m = min( m, abs(e1));
				m = min( m, abs(e2));
				m = min( m, abs(e3));
				m = min( m, abs(e4));
				m = min( m, abs(f1));
				m = min( m, abs(f2));
				m = min( m, abs(f3));
				m = min( m, abs(f4));
	#endif
				sum += m;
				count++;
	#ifdef DEBUG_NOISE
				if (flag)
				{
					fprintf( out, " %3d", m);
				}
	#endif
			}	/* for channel	*/
			if (incr_flag) count_const++;
	#ifdef DEBUG_NOISE
			if (flag)
			{
				fprintf(out, "\n");
			}
	#endif
		}
	}
#ifdef DEBUG_NOISE
	if (flag)
	{
		fclose( out);
		flag = 0;
	}
#endif
	iF->constance[0] = (unsigned long)(100*count_const / count);
	/* normalisation to 8bit data	*/
	//return (unsigned long)((256 * 10 * sum) / ((1ul<<pd->bitperchannel)*pd->size * pd->channel));
	if (pd->bitperchannel >= 8)
		return _GETMIN( 200, (unsigned long)(( 10 * sum) / 
				((1ul<<(pd->bitperchannel-8)) * count)) );
	else
		return _GETMIN( 200, (unsigned long)((256 * 10 * sum) / 
				((1ul << pd->bitperchannel) * count)) );
}
