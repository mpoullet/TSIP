/*****************************************************************
 *
 * File...:	estimate_noise.c
 * Purpose:	kind of noise quantification
 * Author.:	Tilo Strutz
 * Date...: 14.09.2011
 * changes:
 * 17.02.2012 other header file, use of Pict
 * 19.07.2013 modified for CoBaLP2
 *****************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "types_CoBaLP.h" /* definition of IMAGE	*/
#include "codec_CoBaLP.h" /* definition of min()	*/

//#define DEBUG_NOISE


/*---------------------------------------------------------------
 *	estimateNoise_CoBaLP()
 * application of directional Laplacian operator
 *          1       1          1          1        1  1 
 * 1 -2 1  -2    1 -2  1 -2   -2 1  -2 1   -2    -2  -2  -2 1
 *	   	    1             1          1       1  1   1   1
 *
 *---------------------------------------------------------------*/
unsigned short estimateNoise_CoBaLP( IMAGE *imExt, long *lptr)
{
	int cp, m;
	int h, v, d1, d2, c1, c2, c3, c4;
#ifdef EF_DIRECTION
	int e1, e2, e3, e4, f1, f2, f3, f4;
#endif
	unsigned long r, c, pos, pyExt;
	unsigned long long sum;
#ifdef DEBUG_NOISE
	static flag = 1;
	FILE *out=NULL;
	if (flag)
	{
		out = fopen( "noise.pgm", "wb");
		fprintf( out, "P2\n%d %d\n%d\n",imExt->width-2,imExt->height-2, 255);
	}
#endif

	sum = 0;
	{
		/* for all pixels exlusing the border positions	*/
		for (r = 1, pyExt = (OFF_TOP+1)*imExt->width; 
			r < (imExt->height-OFF_TOP)-1; 
			r++, pyExt+=imExt->width)	/* all rows	*/
		{
			for (c = 1, pos = pyExt+OFF_LEFT + 1; 
				c < (imExt->width - OFF_LEFT - OFF_RIGHT)-1; 
				c++, pos++)	/* all columns	*/
			{
				cp = -lptr[pos] - lptr[pos];
				h  = cp + lptr[pos-1] + lptr[pos+1];
				v  = cp + lptr[pos-imExt->width] + lptr[pos+imExt->width];
				d1 = cp + lptr[pos-1-imExt->width] + lptr[pos+1+imExt->width];
				d2 = cp + lptr[pos-1+imExt->width] + lptr[pos+1-imExt->width];
				c1 = cp + lptr[pos-1] + lptr[pos-imExt->width];
				c2 = cp + lptr[pos+1] + lptr[pos-imExt->width];
				c3 = cp + lptr[pos+1] + lptr[pos+imExt->width];
				c4 = cp + lptr[pos-1] + lptr[pos+imExt->width];
	#ifdef EF_DIRECTION
				e1 = cp +lptr[pos-1] +lptr[pos+1-imExt->width];
				e2 = cp +lptr[pos-1] +lptr[pos+1+imExt->width];
				e3 = cp +lptr[pos+1] +lptr[pos-1-imExt->width];
				e4 = cp +lptr[pos+1] +lptr[pos-1+imExt->width];

				f1 = cp +lptr[pos-imExt->width] +lptr[pos+1+imExt->width];
				f2 = cp +lptr[pos-imExt->width] +lptr[pos-1+imExt->width];
				f3 = cp +lptr[pos+imExt->width] +lptr[pos+1-imExt->width];
				f4 = cp +lptr[pos+imExt->width] +lptr[pos-1-imExt->width];
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
	#ifdef DEBUG_NOISE
			if (flag)
			{
				fprintf( out, " %3d", m);
			}
	#endif
			}
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
	return (unsigned short)((30 * sum) / (imExt->size));
}
