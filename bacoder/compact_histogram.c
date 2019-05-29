/*****************************************************
* File...: compact_histogram.c
* Desc...: compaction of image histogram
* Author.: Tilo Strutz
* Date...: 10.09.2013
*
* 29.01.2015 count each bin as gap if it is empty
* 29.07.2015 use cnt_zeros*5 > cnt_bins as threshold
* 28.08.2015 do not use cnt_zeros in first gap, when prediction will be applied
******************************************************/
#include <assert.h>
#include "codec.h"
#include "ginterface.h"


/*--------------------------------------------------------
* compact_histogram()
*--------------------------------------------------------*/
unsigned int compact_histogram( PicData *pd, 
															 int comp,
															 //unsigned int *max_val,
															 unsigned int *idx2val,
															 int pred_flag)
{
	int flag_closed = 0;
	int flag_open = 0;
	int flag_active;
	unsigned int i, idx, idx2val_len;
	unsigned int cnt_bins, cnt_gaps, cnt_zeros, zero_run;
	unsigned int *histo = NULL;
	unsigned int hist_len;
	unsigned int *val2idx = NULL;
	unsigned long pos;

	/* number of different values in component */
	hist_len = (1u << pd->bitperchan[comp]);
	ALLOC( histo, hist_len, unsigned int);
	ALLOC( val2idx, hist_len, unsigned int);
	if (ERRCODE == MALLOC_FAILED) return 78; // suppresses warning in code analysis, but makes not very useful

	/* determine histogram of component	*/
	memset( histo, 0, hist_len * sizeof(unsigned int));
	for (i = 0; i < pd->size; i++)
	{
		histo[ pd->data[comp][i] ]++;
	}

	/* if prediction will be used then disable zero counting in first gap */
	flag_active = !pred_flag; 

	/* find gaps in histogram */
	idx = 0;
	cnt_gaps = cnt_bins = cnt_zeros = 0;
	zero_run = 0;
	for (i = 0; i < hist_len; i++)
	{
		val2idx[i] = idx;	/* LUT for compaction on encoder side	*/
		if (histo[i] > 0)
		{
			flag_active = 1; /* consider zeros only after first non-zero bin	*/
			/* LUT for compaction on encoder side	*/
			idx2val[idx++] = i;	
			cnt_bins++;
			/* makes sure that gap is closed and values can be mapped	*/
			if (flag_open == 1) 
			{
				flag_closed = 1;
				flag_open = 0;
				cnt_gaps++;
				cnt_zeros += zero_run; /* accumulate only zeros, which are followed by
															  * a non-zero element
																*/
				zero_run = 0;
			}
			//*max_val = i;		/* save maximal value	*/ /* What is the reason ??*/
		}
		else
		{
			if (flag_active) zero_run++; /* ignore zeros before first non-zero bin	*/
			/* gap in histogram found	*/
			if (flag_open == 0) 
				flag_open = 1;
		}
	}
	idx2val_len = idx;

	/* do the compaction	*/
	if ( /* at least a single gap	*/
		cnt_zeros*5 > cnt_bins)  /* minimum number of gaps must occur	XXX has to be checked XXX*/
	{
		for ( pos = 0; pos < pd->size; pos++)
		{
			pd->data[comp][pos] = val2idx[ pd->data[comp][pos] ];
		}
	}
	else idx2val_len = 0; /* reset,	no compaction for this component */

	FREE( histo);
	FREE( val2idx);

	return idx2val_len;
}

/*--------------------------------------------------------
* compact_histogram_X()
*--------------------------------------------------------*/
unsigned int compact_histogram_X( PicData *pd, 
															 int comp,
															 unsigned int *idx2val,
															 unsigned int *val2idx)
{
	int flag_closed = 0;
	int flag_open = 0;
	unsigned int i, idx, idx2val_len;
	unsigned int *histo = NULL;
	unsigned int hist_len;

	/* number of different values in component */
	hist_len = (1u << pd->bitperchan[comp]);
	ALLOC( histo, hist_len, unsigned int);

	/* determine histogram of component	*/
	memset( histo, 0, hist_len * sizeof(unsigned int));
	for (i = 0; i < pd->size; i++)
	{
		histo[ pd->data[comp][i] ]++;
	}

	/* find gaps in histogram */
	idx = 0;
	for (i = 0; i < hist_len; i++)
	{
		val2idx[i] = idx;	/* LUT for compaction on encoder side	*/
		if (histo[i] > 0)
		{
			/* LUT for compaction on encoder side	*/
			idx2val[idx++] = i;	
			/* makes sure that gap is closed and values can be mapped	*/
			if (flag_open == 1) 
			{
				flag_closed = 1;
				flag_open = 0;
			}
		}
		else
		{
			/* gap in histogram found	*/
			if (flag_open == 0) 
				flag_open = 1;
		}
	}
	idx2val_len = idx;


	FREE( histo);

	return idx2val_len;
}

/*--------------------------------------------------------
* restore_histogram_compact()
*--------------------------------------------------------*/
void restore_histogram_compact( PicData *pd, 
											 int comp,
											 unsigned int *idx2val)
{
	//unsigned int maxVal = 0;  is set outside
	unsigned long pos;

	for ( pos = 0; pos < pd->size; pos++)
	{
		pd->data[comp][pos] = idx2val[ pd->data[comp][pos] ];
	}

	/* restore maxVal in array	*/
	//for ( pos = 0; pos <= pd->maxVal[comp]; pos++)
	//{
	//	if (maxVal < idx2val[pos]) maxVal = idx2val[pos];
	//}
	//pd->maxVal[comp] = maxVal;
}


