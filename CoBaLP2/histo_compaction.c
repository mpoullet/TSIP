/*****************************************************************
 *
 * FILE....:	histo_compaction.c
 * Function:	remove gaps in histogram
 * Auhtor..:	Tilo Strutz
 * Date....: 	24.05.2013
 * changes:
 * 26.07.2013 count (closed) gaps instead of free bins
 * 26.07.2013 external determination of image histogram
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "codec_CoBaLP.h"
#include "types_CoBaLP.h"
#include "bitbuf.h"


/*--------------------------------------------------------
 * compact_histogram()
 *--------------------------------------------------------*/
unsigned int compact_histogram( IMAGE *im, 
							 long *lptr, 
							 	unsigned long *histo,
								unsigned int hist_len,
							 unsigned int *idx2val)
{
	int flag_closed = 0;
	int flag_open = 0;
	unsigned int i, idx;
	unsigned int cnt_bins, cnt_gaps;
	unsigned int *val2idx = NULL;
	unsigned int new_max_val;
	unsigned long pos;

	/* number of different values in component */
	//CALLOC( histo, hist_len, unsigned int);
	/* mapping of values to index, LUT for compaction on encoder side	*/
	CALLOC( val2idx, hist_len, unsigned int);

	/* determine histogram of component	
	 * also borders must be taken into account, because the 
	 * decoder also uses thes values
	 */
	// memset( histo, 0, hist_len * sizeof(unsigned int));
	/* is set outside	*/
	//for (pos = 0; pos < im->size; pos++)
	//{
	//	histo[ lptr[pos] ]++;
	//}

	/* find gaps in histogram */
	idx = 0;
	cnt_gaps = cnt_bins = 0;
	for (i = 0; i < hist_len; i++)
	{
		val2idx[i] = idx;	/* LUT for compaction on encoder side	*/
		if (histo[i] > 0)
		{
			/* LUT for compaction on decoder side	*/
			idx2val[idx++] = i;	
			cnt_bins++;
			/* makes sure that gap is closed and values can be mapped	*/
			if (flag_open == 1) 
			{
				flag_closed = 1;
				flag_open = 0;
				cnt_gaps++;
			}
		}
		else
		{
			/* gap in histogram found	*/
			if (flag_open == 0) 
				flag_open = 1;
		}
	}
	new_max_val = idx-1;

	/* do the compaction	*/
	if ( /* at least a single gap	*/
		  cnt_gaps > cnt_bins/10)  /* minimum number of gaps must occur	XXX has to be checked XXX*/
	{
		for ( pos = 0; pos < im->size; pos++)
		{
			lptr[pos] = val2idx[ lptr[pos] ];
		}
	}
	else 
	{
		new_max_val = 0; /* reset,	no compaction for this component */
	}
	//FREE(histo);
	FREE(val2idx);

//	if (flag == 2) /* at least a single gap	*/
		return new_max_val;
//	else
//		return 0;
}

/*--------------------------------------------------------
 * restore_histogram()
 *--------------------------------------------------------*/
void restore_histogram( IMAGE *im, long *lptr, 
							 unsigned int *idx2val)
{
	unsigned long pos;

	for ( pos = 0; pos < im->size; pos++)
	{
		lptr[pos] = idx2val[ lptr[pos] ];
	}
}

/*--------------------------------------------------------
 * write_compaction_LUT()
 *--------------------------------------------------------*/
void write_compaction_LUT( IMAGE *im, unsigned int *idx2val, 
													unsigned int new_max_val,
													BitBuffer *bitbuf) 
{
		int i, q, k, b, val;

		/* write information to stream if required
		 */
		if (new_max_val)
		{
			/* transmit first entry	*/
			rice_encoding( bitbuf, idx2val[0], im->bit_depth-1);
			/* the higher the compaction, the higher the differences, the higher k must be	*/
			q = (int)( (1 << im->bit_depth) + (new_max_val >> 1)) / (new_max_val + 1);
			k = 1;
			while(q){k++; q = q >> 1;};	/* determine k	*/
			for ( i = 1; i < (signed)new_max_val+1; i++)
			{
				/* in compaction mode, entries are still in ascending order
				 * ==> transmission of differences is more efficient
				 */
				val = idx2val[i] - idx2val[i-1] - 1; /* difference must be at least 1*/
				assert( val >= 0);
				rice_encoding( bitbuf, val, k);
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*k)
					k = (k + b) >> 1;	/* fast adaptation	*/
				else
					k = (k * 7 + b) >> 3;	/* slower adaptation	*/
			}
		}
}


/*--------------------------------------------------------
 * read_compaction_LUT()
 *--------------------------------------------------------*/
void read_compaction_LUT( IMAGE *im, unsigned int *idx2val, 
												 unsigned int new_max_val,
													BitBuffer *bitbuf)
{
	int i, q, k, b, val;

		if (1)
		{
			idx2val[0] = rice_decoding( bitbuf, im->bit_depth-1);
			q = (int)( (1 << im->bit_depth) + (new_max_val >> 1)) / (new_max_val + 1);
			k = 1;
			while(q){k++; q = q >> 1;};	/*determine k	*/
			for ( i = 1; i < (signed)new_max_val+1; i++)
			{
				//idx2val0[i] = idx2val0[i-1] + GetFiboFromStream( bitbuf);
				val = rice_decoding( bitbuf, k);
				idx2val[i] = idx2val[i-1] + val + 1;
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*k)
					k = (k + b) >> 1;	/* fast adaptation	*/
				else
					k = (k * 7 + b) >> 3;	/* slower adaptation	*/
			}
		}
}
