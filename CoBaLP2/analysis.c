/*****************************************************
 * File..: analysis.c
 * Desc..: some functions for image analysis
 * Author: Tilo Strutz
 * Date..: 06.05.2013
 * 18.07.2013 major bugfix
 ******************************************************/
#include <stdlib.h>
#include <string.h>
// #include "med.h"

/*---------------------------------------------------------
 * HowSynthetic(),
 * result: 0-200,
 * the higher, the more synthetic
 *---------------------------------------------------------*/
unsigned short HowSynthetic_( unsigned int hist_size, 
									unsigned long *histo /* histogram	*/)
{
	unsigned int i, cnt;
	unsigned long pixel_cnt;
	unsigned int modal, modal_cnt;

	/* summation of differences between adjacent histogram values */
	modal = 0; modal_cnt = histo[ modal];
	cnt = 0;
	pixel_cnt = histo[0];
	for ( i = 1u; i < hist_size; i++)
	{
		cnt += abs( (long)histo[i - 1u] - (long)histo[i]);	
		if (histo[i] > modal_cnt)
		{
			modal_cnt = histo[i];	/* new modal value	*/
			modal = i;
		}
		pixel_cnt +=  histo[i];
	}
	/* wrap around histogram	*/
	cnt += abs( (long)histo[0] - (long)histo[hist_size-1]);
	
	/* remove influence of possible background colour */
	if (modal == 0)
	{
		int diff1, diff2;
		diff1 = abs( (long)histo[0] - (long)histo[1]);
		diff2 = abs( (long)histo[0] - (long)histo[hist_size-1]);
		cnt -= diff1;
		cnt -= diff2;
		cnt += abs( (long)histo[1] - (long)histo[hist_size-1]);
		pixel_cnt -= (long)histo[0];
	}
	else if (modal == hist_size-1)
	{
		int diff1, diff2;
		diff1 = abs( (long)histo[modal] - (long)histo[0]);
		diff2 = abs( (long)histo[modal] - (long)histo[modal-1]);
		cnt -= diff1;
		cnt -= diff2;
		cnt += abs( (long)histo[0] - (long)histo[modal-1]);
		pixel_cnt -= (long)histo[modal];
	}
	else
	{
		int diff1, diff2;
		diff1 = abs( (long)histo[modal] - (long)histo[modal+1]);
		diff2 = abs( (long)histo[modal] - (long)histo[modal-1]);
		cnt -= diff1;
		cnt -= diff2;
		cnt += abs( (long)histo[modal+1] - (long)histo[modal-1]);
		pixel_cnt -= (long)histo[modal];
	}

	/* very small cnt if very few different signal values	*/
	if (cnt < 10)	/* threshold not tested	*/
		return (200);
	return (unsigned short)(100.0 * (double)cnt / (double)pixel_cnt);
}
