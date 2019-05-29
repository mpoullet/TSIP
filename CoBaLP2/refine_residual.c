/*****************************************************************
 *
 * File..:	refine_residual.c
 * Descr.:	maps prediction errors to refined erros based on prediction contexts
 * Author.:	Tilo Strutz
 * Date..: 	12.06.2013
 * changes:
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"


/*---------------------------------------------------------------
 *	get_sign_context()
 *
 *---------------------------------------------------------------*/
unsigned short get_sign_context( IMAGE *im, unsigned long pos, 
											unsigned int r,
											unsigned int c,
											long *eptr)
{
	unsigned short cxs;

	cxs = 0;
	if (c > 1 && r > 1 && c < im->width - 2)
	{
		if (eptr[pos-1] < 0) cxs += 1;
		if (eptr[pos-2] < 0) cxs += 2;
		if (eptr[pos-im->width] < 0) cxs += 4;
		if (eptr[pos-1-im->width] < 0) cxs += 8;
		if (eptr[pos-2-im->width] < 0) cxs += 16;
		if (eptr[pos+1-im->width] < 0) cxs += 32;
		if (eptr[pos+2-im->width] < 0) cxs += 64;
		if (eptr[pos  -im->width-im->width] < 0) cxs += 128;
		if (eptr[pos-1-im->width-im->width] < 0) cxs += 256;
		//if (eptr[pos-2-im->width-im->width] < 0) cxs += 512;
		if (eptr[pos+1-im->width-im->width] < 0) cxs += 1024;
		//if (eptr[pos+2-im->width-im->width] < 0) cxs += 2048;
	}

	return cxs;
}

/*---------------------------------------------------------------
 *	refine_residual()
 *
 *---------------------------------------------------------------*/
void refine_residual( IMAGE *im, unsigned long pos, int mode,
											long *in_ptr, long *out_ptr, 
											unsigned short *px_ptr)
{
	long sum, count, val;

	sum = 0;
	count = 0;
	if ( px_ptr[pos] ==  px_ptr[pos - 1])
	{
		sum += in_ptr[pos - 1];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos - 2])
	{
		sum += in_ptr[pos - 2];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos - im->width])
	{
		sum += in_ptr[pos - im->width];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos-1 - im->width])
	{
		sum += in_ptr[pos-1 - im->width];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos-2 - im->width])
	{
		sum += in_ptr[pos-2 - im->width];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos+1 - im->width])
	{
		sum += in_ptr[pos+1 - im->width];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos+2 - im->width])
	{
		sum += in_ptr[pos+2 - im->width];
		count++;
	}
	if ( px_ptr[pos] ==  px_ptr[pos - im->width - im->width])
	{
		sum += in_ptr[pos - im->width - im->width];
		count++;
	}

	if (count > 2)
	{
		sum = sum / (count );
		if (sum > 3)
		{
			if (mode) val = in_ptr[pos] - sum/2;
			else val = in_ptr[pos] + sum/2;
			//printf( "\n (r,c)=(%3d, %3d) : %+3d - %+3d = %+3d", 
			//	r, c, in_ptr[pos], sum, val);
		}
		else		if (sum < -3)
		{
			if (mode) val = in_ptr[pos] - sum/2;
			else val = in_ptr[pos] + sum/2;
			//printf( "\n (r,c)=(%3d, %3d) : %+3d - %+3d = %+3d", 
			//	r, c, in_ptr[pos], sum, val);
		}
		else
		{
			val = in_ptr[pos];
		}
	}
	else
	{
		val = in_ptr[pos];
	}
	out_ptr[pos] = val;
}
