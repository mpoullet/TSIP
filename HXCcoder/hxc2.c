/*****************************************************
* File..: hxc2.c
* Desc..: implementation of H(X|C) coder
* Author: T. Strutz
* Date..: 20.09.2013
* changes:
* 08.09.2013 new: max_similar_pattern
* 04.02.2012 allocate several pattern lists
* 13.03.2014 new HXC_MAX_MAXVAL
* 04.02.2015 bugfix: exchange.K_ was not initialised
* 04.02.2015 order of [i][comp] changed
*								adressing the pattern template
* 12.03.2015 watch theoretical information content and 
*		         number of symbols to be encoded
* xx.09.2015 predictive coding in stage 3 
* 14.10.2015 predictive coding in stage 3 using MED
* 16.10.2015 stage2: count modification based on estimated triple
* 20.10.2015 stage2/3: best predictor determination for U and V
* 28.10.2015 bugfix memory leaks free_imatrix( &tolerance);
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc.h"
#include "types.h"
#include "codec.h"	/* for min( a, b);	*/
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"
#include "paeth.h"

#ifdef _DEBUG
//  #define H_DEBUG
#endif

#ifdef HXC2_EST_INF
double inf_stage1, inf_stage2, inf_stage3;
unsigned long cnt_stage1, cnt_stage2, cnt_stage3;
#define invLOG2	1.442695041  /* 1 / ln(2.)  */
#endif

#define OUT_MODE
//
#undef OUT_MODE
#define MED_PRED
//#undef MED_PRED

#define LOCO_PRED
// does not generally improve compression
#undef LOCO_PRED


//#ifndef LOCO_PRED
/* Errechnet den Schätzwert für die Prädiktion */
MYINLINE long GetMEDVal( PicData *pd, uint chan, uint x, uint y,
												long *A, long *B, long *C, long *D, long *P)
{
	static long ca, cb, cc, Xhat;

	/* C B
	* A X
	*/

	if (x == 0) ca = 0; 
	else ca = (int)*GetXYValue(pd, chan, x - 1u, y);
	if (y == 0) cb = 0;
	else cb = (int)*GetXYValue(pd, chan, x, y - 1u);
	if (x == 0 || y == 0) cc = 0;
	else cc = (int)*GetXYValue(pd, chan, x - 1u, y - 1u);
	if ( (x == pd->width-1) || y == 0) *D = 0;
	else *D = (int)*GetXYValue(pd, chan, x + 1u, y - 1u);

	if (cc <= ca)
	{
		if (cc <= cb)
		{
			if (ca > cb)
			{
				Xhat = ca; /* C <= B < A	*/
			}
			else
			{
				Xhat = cb; /* C <= A <= B	*/
			}
		}
		else
		{
			/* B < C <= A	*/
			Xhat = ca + cb - cc;
		}
	}
	else
	{
		if (cc >= cb)
		{
			if (ca < cb)
			{
				Xhat = ca; /* A < B <= C	*/
			}
			else
			{
				Xhat = cb; /* B <= A <= C	*/
			}
		}
		else
		{
			/* A < C < B	*/
			Xhat = ca + cb - cc;
		}
	}
	*A = ca;
	*B = cb;
	*C = cc;
	*P = ca + cb - cc;
	return Xhat;	
}
//#endif


#ifndef OLD_HXC2

/*--------------------------------------------------------
* HXC2_coding()
*--------------------------------------------------------*/
void HXC2_coding( PicData *pd,		/* structure with image data	*/
								 BitBuffer *bitbuf, /* interface for file input and output*/
								 AutoConfig *aC,  /* parameter of codec */
								 int encoder_flag)	/* 1 ==> encoder; 0=> decoder	*/

{
	int X_in_merged_hist_flag; /* 1 ... current colour X is in the merged histogram	*/
	long **t_actual;	/* template of actual pattern	*/
	long **t_plist	/* template from pattern	list */;
	long **tolerance;	/* tolerence in pattern comparison	*/
	long X[4], Xhat[4], predTol[4], pred_err = 0;
	long ABC=0, A=0, B=0, C=0, D=0, P=0, min_abs_err;
	int pred_mode;
	//long A[4], B[4], C[4], Xhat[4];
	unsigned int adressing_mode = 0;
	unsigned int comp; /* number of channel or component	*/
	unsigned int r, c; /* row an d column	*/
	unsigned int max_similar_pattern;
	/* for re-scaling the count of the ESC symbol in pattern lists histograms */
	unsigned int maxCountESC; 
	unsigned int maxCountFac ;/* for re-scaling of histogram counts */ 
	unsigned int increaseFac; /* scaling of counts in merged stacked histogram	*/
	//unsigned int *similar_size = NULL;
	unsigned long /* *Histos = NULL,*/ *(Hist[4]), K_Hist[4];	/* zero order histograms, sizes	*/
	unsigned long i;
	unsigned long numLists;
	unsigned long symbol;
	unsigned long sum_abs_err[4], num_abs_err[4];

	//unsigned long K_;	/* number of symbols per component	*/
	unsigned long bytes_old; /* temporary number of written bytes	*/
	unsigned long numOfPaletteItems; /* refers to palette	*/

	/* pointer to array of pointers to single list elements	*/
	//PATTERN_LIST_ELEM2 **similar_patterns = NULL;	
	PATTERN_LIST2 *pat_list; /* top of dictionary (pattern list) */
	HIST_LIST_ELEM2 merged_stacked_histogram;
	HIST_LIST_ELEM2 palette_obj, *palette;
	EXCHANGE_STRUC exchange;

	acModell M;	/* normal AC context	*/
	acState acState_obj, *acSt;	/* state of arithmetic coder	*/

#ifdef LOCO_PRED
	/* LOCO variables	*/
	int state[4], sign[4];
	long 	*data = NULL;
	long Xloco[4], low_lim[4], up_lim[4], range[4];
	unsigned int kloco[4];
#endif

#ifdef OUT_MODE
	int mode_hxc2 = 0;
	FILE *mode_file=NULL;
	//mode_file = fopen( "mode_hxc2.pgm", "wb");
	// fprintf( mode_file, "P2\n%d %d\n2\n", pd->width, pd->height);
	mode_file = fopen( "estimate.ppm", "wt");
	fprintf( mode_file, "P3\n%d %d\n%d\n", pd->width, pd->height, 255);
#endif


	acSt = &acState_obj;
	palette = &palette_obj;

#ifdef HXC2_EST_INF
	inf_stage1 = inf_stage2 = inf_stage3 = 0.0;
	cnt_stage1 = cnt_stage2 = cnt_stage3 = 0;
#endif

	/* default colour	*/
	palette->count = 0;
	palette->next = NULL;
	palette->value[0] = -1;
	palette->value[1] = -1;
	palette->value[2] = -1;
	palette->value[3] = -1;
	numOfPaletteItems = 1;

	assert( aC->HXC2increaseFac >= 0);


	/* start with empty dictionary list, +1 because 0 has to be included */
	/* use channel 0 as indicator, whether vector or 
	* matrix list addressing must be used
	*/
	if (pd->maxVal_pred[0] > HXC_MAX_MAXVAL)
	{	/* linear array, slower search for matches	*/
		numLists = (pd->maxVal_pred[0] + 1);
		fprintf( stderr, "\n ! HXCcoder: number of colours too high (%d > %d) !", 
			numLists, HXC_MAX_MAXVAL);
		fprintf( stderr, "\n ! HXCcoder: must switch to linear adressing causing slow search!\n");
		adressing_mode = 0;
	}
	else
	{
		numLists = (pd->maxVal_pred[0] + 1) * (pd->maxVal_pred[0] + 1);
		adressing_mode = 1;
	}
	ALLOC( pat_list, numLists, PATTERN_LIST2);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXC2;
	}

	/* reset all allocated lists	*/
	for (i = 0; i < numLists; i++)
	{
		(pat_list[i]).pattern_list_size = 0;
		(pat_list[i]).top.next = NULL;
		(pat_list[i]).top.prev = NULL;
		(pat_list[i]).top.top_hist_elem = NULL; /* empty histogramm list	*/
		(pat_list[i]).top.r = 0;
		(pat_list[i]).top.c = 0;
	}


	/* allocate histograms for all channels	*/
	//ALLOC( Hist, pd->channel, unsigned long*);
	//{
	//	unsigned int total;
	//	total = 0;
	//	for (comp = 0; comp < pd->channel; comp++)
	//	{
	//		K_Hist[comp] = pd->maxVal_pred[comp] + 1;
	//		total += K_Hist[comp] + 1;
	//	}
	//	CALLOC( Histos, total, unsigned long);
	//	total = 0;
	//	comp = 0;
	//	Hist[comp] = &(Histos[total]);

	//	for (comp = 1; comp < pd->channel; comp++)
	//	{
	//		total += K_Hist[comp-1] + 1;
	//		Hist[comp] = &(Histos[total]);
	//	}
	//}
	for (comp = 0; comp < pd->channel; comp++)
	{
		K_Hist[comp] = pd->maxVal_pred[comp] + 1;
		CALLOC( Hist[comp], K_Hist[comp]+1, unsigned long);
	}

	/* allocate template for pattern (current + corresponding to pattern list	*/
	t_actual = imatrix( MAX_PATTERN_SIZE, 4);
	t_plist = imatrix( MAX_PATTERN_SIZE, 4);
	tolerance = imatrix( MAX_PATTERN_SIZE, 4);

	/* pre init
	*     F
	*   C B D
	* E A X
	*/
	/* tolerance defines the maximum difference between pixels of current template and
	* pixels of reference template. If difference is higher than tolerance, then
	* reference template is NOT regarded as match
	*/
	maxCountESC = 0; /* 	*/
	for (comp = 0; comp < pd->channel; comp++)
	{
		for (c = 0; c < MAX_PATTERN_SIZE; c++)
		{
			/* only the tolerance in first component are set, so far	
			* now lets check different tolerances for all components
			*/
			tolerance[c][comp] = aC->HXC2tolerance[comp][c];
			maxCountESC += tolerance[c][comp];
		}
	}

	//maxCountFac = 200 + maxCountESC / (pd->channel * MAX_PATTERN_SIZE);
	maxCountFac = 100; // not use in update, currently
	/* the smaller maxCountESC the earlier the count of the ESC symbol will be down-scaled	
	* assumption: the smaller the tolerances the less influence should have the count of the ESC symbol
	*/
	maxCountESC = 8 + min( 30, maxCountESC / (3 * pd->channel * MAX_PATTERN_SIZE));
	maxCountESC = 8;

#ifdef H_DEBUG
	fprintf( stdout, "\ntolerances (%d,%d,%d),(%d,%d,%d),(%d,%d,%d),", 
		tolerance[0][0], tolerance[0][1], tolerance[0][2], 
		tolerance[1][0], tolerance[1][1], tolerance[1][2], 
		tolerance[2][0], tolerance[2][1], tolerance[2][2]); 
	fprintf( stdout, "(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)",
		tolerance[3][0], tolerance[3][1], tolerance[3][2], 
		tolerance[4][0], tolerance[4][1], tolerance[4][2], 
		tolerance[5][0], tolerance[5][1], tolerance[5][2] 
	);

#endif

	/* compute maximal required size in dependence on possible similar pattern
	* the higher 'tolerance'	the more patterns are regareded as match, which 
	* have to be stored in pattern list
	*/
	//max_similar_pattern = 1; 
	//for (c = 0; c < PATTERN_SIZE; c++)
	//{
	//	max_similar_pattern *= tolerance[c]*2 + 1; 
	//}
	///* reasonable maximum number of similar pattern, save guard	*/
	//max_similar_pattern = min( max_similar_pattern, 2000); 
	max_similar_pattern = 2000;  /* set to maximum	*/

	/* allocate array for pointers, one more for list termination, two needed, Why??	*/
	/* Batev: One more for list termination, and the second one for the current pattern,  
	* if it is to be added, as well
	*/
	exchange.similar_patterns = NULL; /* suppress compiler warnings	*/
	ALLOC( exchange.similar_patterns, max_similar_pattern+2, PATTERN_LIST_ELEM2 *);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXC2;
	}
	exchange.similar_size = NULL; /* suppress compiler warnings	*/
	ALLOC( exchange.similar_size, max_similar_pattern+2, unsigned int);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXC2;
	}

	exchange.K_ = 0; /* used for merged histogram, number of elements	*/
	exchange.bestSimilarity = 0; /* is alos reset in get_distribution2()	*/

	increaseFac = aC->HXC2increaseFac * 2 + 1; /* '+1' is save guard	*/

	/* prepare arithmetic coding	*/
	if (encoder_flag)
	{
		start_encoding( acSt); /* reset internal values	*/
	}
	else
	{
		start_decoding( acSt, bitbuf); /* reset internal values	*/
	}

	/* allocate model of arithmetic coding
	* use maximum possible number of symbols
	* this is theoretically the number of colours in the image
	* which is equal to the number of pixels in maximum
	*/
	{
		unsigned long num;
		// num = min( pd->size, numColours) + 1;
		num = pd->size + 1;
		start_model( &M, num);
		M.K = 1; /* we start with empty histogram	*/
	}

	for( comp = 0; comp < pd->channel; comp++)
	{
		sum_abs_err[comp] = predTol[comp] = 0;
		num_abs_err[comp] = 0;
	}

#ifdef LOCO_PRED	/*
	* LOCO initialisation
	*/
	vec_pred_init( pd->width, pd->channel, low_lim, up_lim, range,
		pd->maxVal_pred, data);
#endif



	fprintf( stderr, "\n");
	/*************************
	* process image
	************************/
	for( r = 0; r < pd->height; r++)
	{
		bytes_old = numBytes; /* check bytes per row	*/
		for( c = 0; c < pd->width; c++) 
		{
			copy_pattern2( pd, r, c, t_actual);	/* get current pattern from image	*/

			//if ( r == 30 && c == 74)
			//{
			//	r = r;
			//}


#ifdef LOCO_PRED
			/* include LOCO prediction	*/
			vec_pred( pd->width, r, c, pd->maxVal_pred, pd->channel, Xloco, 
				state, sign, kloco);
#endif

#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, "\n\n#pos %d, %d:", r, c);
				fprintf( stdout, " actual pattern: (");
				for( i = 0; i < MAX_PATTERN_SIZE; i++)
				{
					for( comp = 0; comp < pd->channel; comp++)
					{
						fprintf( stdout, "%d,", t_actual[i][comp]);
					}
					fprintf( stdout, ";");
				}
				fprintf( stdout, ")");
				fflush( stdout);
				if (encoder_flag)
				{
					fprintf( stdout, "\n\t X=(");
					for( comp = 0; comp < pd->channel; comp++)
					{
						fprintf( stdout, "%d,", pd->data[comp][c + r * pd->width]);
					}
					fprintf( stdout, ")");
				}
			}
#endif

			/* initialise first elemnt of merged histogram	*/
			merged_stacked_histogram.count = 0; /* ESC item	*/
			merged_stacked_histogram.next = NULL;
			merged_stacked_histogram.value[0] = -1; /* no RGBA / YUVY quadruple assigned	*/
			merged_stacked_histogram.value[1] = -1;
			merged_stacked_histogram.value[2] = -1;
			merged_stacked_histogram.value[3] = -1;
			exchange.K_ = 0; /* merged_stacked_histogram is empty, do not count ESC item	*/

			/* search list for matching patterns,
			* collect information about count statistics,
			* build distribution if possible
			* return size of histogram K_
			*/
			get_distribution2( pd, r, c, t_actual, t_plist, pat_list, &exchange, 
				&merged_stacked_histogram, tolerance,
				max_similar_pattern, adressing_mode, aC->pattern_size, 0 /* only for SCF */);

			if (encoder_flag)
			{
#ifdef OUT_MODE
				mode_hxc2 = 0;
#endif
				/* get colour vector from current position	*/
				for( comp = 0; comp < pd->channel; comp++)
				{
					X[comp] = pd->data[comp][c + r * pd->width];
				}

				/* number of items in merged histogram	(excluding ESC symbol)*/
				if (exchange.K_ == 0)
				{
					X_in_merged_hist_flag = 0;
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, ", merged histogram is empty");
					}
#endif
				}
				else
				{
					/* there is at least one item in merged histogram */
					/* check, whether X is included in merged histogram	
					* created histogram and get symbol
					*/
					X_in_merged_hist_flag = X_in_merged_hist( &merged_stacked_histogram, X, pd->channel,
						&symbol, M.H, &(M.K), increaseFac);

					assert( M.K < MAX_HIST_SIZE);
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						HIST_LIST_ELEM2 *merged_hist_element;
						fprintf( stdout, "\n\t merged histo:");
						merged_hist_element = &merged_stacked_histogram;
						do
						{
							fprintf( stdout, " %d x (", merged_hist_element->count);
							for ( comp = 0; comp < pd->channel; comp++)
							{
								fprintf( stdout, "%d,", merged_hist_element->value[comp]);
							}
							fprintf( stdout, "),");
							merged_hist_element = merged_hist_element->next;
						} while(merged_hist_element != NULL);


						if (X_in_merged_hist_flag) fprintf( stdout, "\n\t X is in merged histo");
						else fprintf( stdout, "\n\t X is not in merged histo");
						fprintf( stdout, "\n\t encode (merged)symbol=%d", symbol);
						fprintf( stdout, "\n\t\t using %d-distribution: ", M.K);
						for (i = 1; i <= M.K; i++)
						{
							fprintf( stdout, "%d,", M.H[i]);
						}
					}
#endif

					create_cumulative_histogram( M.H, M.K);

					/* do the arithmetic encoding	*/
					encode_AC( &M, acSt, symbol, bitbuf);

#ifdef HXC2_EST_INF
					{
						double p;
						p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
						inf_stage1 -= log( p) * invLOG2;
						if (symbol) cnt_stage1++;
					}
#endif

				}

				if (!X_in_merged_hist_flag) /* enter stage 2	*/
				{
					if (pd->channel > 1 || 0 /*does not improve for all GREY images*/)
					{
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t X is not in merged histogram, enter stage 2");
						}
#endif
#ifdef OUT_MODE
						mode_hxc2 = 1;
#endif

						/* check whether X is in palette, fill histogram
						* returns zero if not in palette
						*/
						symbol = X_in_palette( palette, X, pd->channel);

						/* get estimate of triple	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							/* use MED	*/
							long val;

							val = GetMEDVal( pd, comp, c, r, &A, &B, &C, &D, &P); /* get Xhat as MED	*/
							Xhat[comp] = max( 0, min( (signed)K_Hist[comp], val) ); /* check range	*/
						} /* for ( comp */

						/* prepare histogram based on palette and increase count of estimated
						* triple +- tolerances
						*/
						if (numOfPaletteItems < 11) /* does not change performance very much	*/
						{
							palette_to_histogram( palette, M.H, Xhat, pd->channel, predTol, 0);
						}
						else
						{
							palette_to_histogram( palette, M.H, Xhat, pd->channel, predTol, 1);
						}
						M.K = numOfPaletteItems;
						create_cumulative_histogram( M.H, M.K); /* prepare histogram */	
						encode_AC( &M, acSt, symbol, bitbuf);

#ifdef HXC2_EST_INF
						{
							double p;
							p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
							inf_stage2 -= log( p) * invLOG2;
							if (symbol) cnt_stage2++; 
							/* exclude symbol == 0 as this pixel is counted in the next stage*/
						}
#endif
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							HIST_LIST_ELEM2 *current_hist_element;
							fprintf( stdout, "\n\t use palette histo:");
							current_hist_element = palette;
							while( current_hist_element != NULL) /* list is terminated with NULL	*/
							{
								fprintf( stdout, " %d x (", current_hist_element->count);
								for (comp=0; comp< pd->channel; comp++)
								{
									fprintf( stdout, " %d,", current_hist_element->value[comp]);
								}
								fprintf( stdout, ")");
								/* goto to the next histogram element	*/
								current_hist_element = current_hist_element->next;
							}
						}
#endif
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t encode (palette)symbol=%d", symbol);
							fprintf( stdout, "\n\t\t using cumul.%d-distribution: ", M.K);
							for (i = 1; i <= M.K; i++)
							{
								fprintf( stdout, "%d,", M.H[i]);
							}
						}
#endif
					}
					else /* pd->channel == 1; enter directly stage 3	*/
					{
						symbol = 0;
					}

					if (symbol)
					{
						/* update prediction error and tolerance
						* same must be done in decoder mode, stage 2
						*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							pred_err = X[comp] - Xhat[comp];
							sum_abs_err[comp] += abs( pred_err);
							num_abs_err[comp]++;
							predTol[comp] = sum_abs_err[comp] / num_abs_err[comp];
						}
					}
					else /* symbol == 0; enter stage 3	*/
					{
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t ==> X is also not in palette yet");
						}
#endif
#ifdef OUT_MODE
						mode_hxc2 = 2;
#endif
						palette->count++; /* count of symbol 0 (X not in palette)	 */

						/* X is not in palette, use separate coding	*/
						/* this implementation is bit consuming compared to the approach with 
						* indexed colours, since three values have to be encoded here
						* instead of only one symbol
						* after encoding R, the distribution of G should be dependent on R,
						* B should be dependent on R and G a.s.o.
						* however, this stage is only used one time for each colour
						*/
						//encode RGBA separately
						pred_mode = 0;
						for( comp = 0; comp < pd->channel; comp++)
						{
							/* maximum number of different values	*/
							M.K = K_Hist[comp];
							/* copy histogramm of pred errors of component into M	*/
							memcpy( M.H, Hist[comp], sizeof( unsigned long) * (K_Hist[comp] + 1) );
#ifdef H_DEBUG
							if ( r >= FIRSTCOL && r <= LASTCOL)
							{
								fprintf( stdout, "\n\t\t   using %d-distribution: ",  M.K);
								for (i = 1; i <= M.K; i++)
								{
									fprintf( stdout, "%d,", M.H[i]);
								}
							}
#endif

							create_cumulative_histogram( M.H, M.K);
							/* use PAETH or MED	*/
							{
								long clampVal1, clampVal2, val;
								clampVal1 = (M.K-1) >> 1;
								clampVal2 = M.K >> 1;

								val = GetMEDVal( pd, comp, c, r, &A, &B, &C, &D, &P);
								ABC = max( 0, min( (signed)M.K, val) );
								P = max( 0, min( (signed)M.K, P) );
								if (!pred_mode)	pred_err = X[comp] - ABC;
								else if (pred_mode == 1) pred_err = X[comp] - A;
								else if (pred_mode == 2) pred_err = X[comp] - B;
								else if (pred_mode == 3) pred_err = X[comp] - C;
								else if (pred_mode == 4) pred_err = X[comp] - D;
								else if (pred_mode == 5) pred_err = X[comp] - P;
								else if (pred_mode == 6) pred_err = X[comp] - ((A + B) >> 1);

								/* check best mode for next component */
								min_abs_err = abs( X[comp] - val);
								if (min_abs_err > abs( X[comp] - A) )
								{
									pred_mode = 1;
									min_abs_err = abs( X[comp] - A);
								}
								if (min_abs_err > abs( X[comp] - B) )
								{
									pred_mode = 2;
									min_abs_err = abs( X[comp] - B);
								}
								if (min_abs_err > abs( X[comp] - C) )
								{
									pred_mode = 3;
									min_abs_err = abs( X[comp] - C);
								}
								if (min_abs_err > abs( X[comp] - D) )
								{
									pred_mode = 4;
									min_abs_err = abs( X[comp] - D);
								}
								if (min_abs_err > abs( X[comp] - P) )
								{
									pred_mode = 5;
									min_abs_err = abs( X[comp] - P);
								}
								if (min_abs_err > abs( X[comp] - ((A + B) >> 1)) )
								{
									pred_mode = 6;
									min_abs_err = abs( X[comp] - ((A + B) >> 1));
								}
								//fprintf( stdout, "\n %d,%d: X=%d val=%d mode=%d", r, c, X[comp], val, pred_mode);

								/* clamp	*/
								if (pred_err > clampVal1) pred_err -= M.K;
								else if (pred_err < -clampVal2) pred_err += M.K;
								symbol = pred_err + clampVal2;
#ifdef H_DEBUG
								if ( r >= FIRSTCOL && r <= LASTCOL)
								{
									fprintf( stdout, "\n\t   encode prediction error=%d", pred_err);
									fprintf( stdout, "\t   encode symbol=%d", symbol);
								}
#endif
								encode_AC( &M, acSt, symbol, bitbuf);
							}

#ifdef HXC2_EST_INF
							{
								double p;
								p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
								inf_stage3 -= log( p) * invLOG2;
								if (comp == 0) cnt_stage3++; /* single count for pixel	*/
							}
#endif
							/* update histogram for this component, since this element
							* can be also a part of  another RGB or YUV vector 
							*/
							Hist[comp][ symbol+1 ]++;	/* increment count of X element	*/
						} /* for comp */
					} /* if (symbol == 0)*/
				} /* if (!included_flag) */
#ifdef OUT_MODE
				// fprintf( mode_file, "%d ", mode_hxc2);
				if (!X_in_merged_hist_flag)
				{
					fprintf( mode_file, "%3d %3d %3d ", Xhat[0], Xhat[1], Xhat[2]);
				}
				else
				{
#ifdef LOCO_PRED
					fprintf( mode_file, "%3d %3d %3d ", Xloco[0], Xloco[1], Xloco[2]);
#else
					fprintf( mode_file, "%3d %3d %3d ", 0, 0, 0);
#endif
				}
#endif
			}
			else /* decoding	*/
			{
				/* number of items in merged histogram	(excluding ESC symbol)*/
				if (exchange.K_ > 0)
				{
					/* there is at least one item in merged histogram */

					get_merged_hist( &merged_stacked_histogram, M.H, &(M.K), increaseFac);

					assert( M.K < MAX_HIST_SIZE);
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t\t using %d-distribution: ", M.K);
						for (i = 1; i <= M.K; i++)
						{
							fprintf( stdout, "%d,", M.H[i]);
						}
					}
#endif

					create_cumulative_histogram( M.H, M.K);
					/* do the arithmetic decoding	*/
					symbol = decode_AC( &M, acSt, bitbuf);
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t decode (merged)symbol=%d", symbol);
					}
#endif

					if (symbol != 0)
					{
						/* store decoded signal value	*/
						/* get X from merged histogram	*/
						get_X_from_merged_hist( &merged_stacked_histogram, X, pd->channel,
							symbol);
						X_in_merged_hist_flag = 1;
					}
					else
					{
						X_in_merged_hist_flag = 0;
					}
				}
				else
				{
					X_in_merged_hist_flag = 0;
				}

				if (!X_in_merged_hist_flag)  /* enter stage 3	*/
				{
					if (pd->channel > 1 || 0 /*does not improve for all REY images*/)
					{
						/* get estimate of triple	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							/* use MED	*/
							long val;

							val = GetMEDVal( pd, comp, c, r, &A, &B, &C, &D, &P); /* get Xhat as MED	*/
							Xhat[comp] = max( 0, min( (signed)K_Hist[comp], val) ); /* check range	*/
						} /* for ( comp */

						/* prepare histogram of symbols	*/
						if (numOfPaletteItems < 11)
						{
							palette_to_histogram( palette, M.H, Xhat, pd->channel, predTol, 0);
						}
						else
						{
							palette_to_histogram( palette, M.H, Xhat, pd->channel, predTol, 1);
						}
						M.K = numOfPaletteItems;
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							HIST_LIST_ELEM2 *current_hist_element;
							fprintf( stdout, "\n\t X is not in merged histogram");
							fprintf( stdout, "\n\t palette histo:");
							current_hist_element = palette;
							while( current_hist_element != NULL) /* list is terminated with NULL	*/
							{
								fprintf( stdout, " %d x (", current_hist_element->count);
								for (comp=0; comp< pd->channel; comp++)
								{
									fprintf( stdout, " %d,", current_hist_element->value[comp]);
								}
								fprintf( stdout, ")");
								/* goto to the next histogram element	*/
								current_hist_element = current_hist_element->next;
							}
						}
#endif
						create_cumulative_histogram( M.H, M.K);
						symbol = decode_AC( &M, acSt, bitbuf);
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t decode (palette)symbol=%d", symbol);
							fprintf( stdout, "\n\t\t using cumul. %d-distribution: ", M.K);
							for (i = 1; i <= M.K; i++)
							{
								fprintf( stdout, "%d,", M.H[i]);
							}
						}
#endif
					}
					else /* pd->channel ==1 */
					{
						symbol = 0; /* enter directly stage 3	*/
					}
					if (symbol != 0)
					{
						get_X_from_palette( palette, symbol, X, pd->channel);
						/* update prediction error	and tolerance */
						for( comp = 0; comp < pd->channel; comp++)
						{
							pred_err = X[comp] - Xhat[comp];
							sum_abs_err[comp] += abs( pred_err);
							num_abs_err[comp]++;
							predTol[comp] = sum_abs_err[comp] / num_abs_err[comp];

						}
					}
					else  /* stage 3	*/
					{
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t ==> X is also not in palette yet");
						}
#endif
						palette->count++; /* count of symbol 0 (X not in palette)*/
						//decode RGBA separately
						pred_mode = 0;
						for( comp = 0; comp < pd->channel; comp++)
						{
							/* maximum number of different values	*/
							M.K = K_Hist[comp];
							/* copy zero order hist into M	*/
							memcpy( M.H, Hist[comp], sizeof( unsigned long) * (K_Hist[comp] + 1) );
#ifdef H_DEBUG
							if ( r >= FIRSTCOL && r <= LASTCOL)
							{
								fprintf( stdout, "\n\t\t   using %d-distribution: ", M.K);
								for (i = 1; i <= M.K; i++)
								{
									fprintf( stdout, "%d,", M.H[i]);
								}
							}
#endif

							create_cumulative_histogram( M.H, M.K);

							/* do prediction	*/
							{
								long clampVal2, val;

								symbol = decode_AC( &M, acSt, bitbuf);
								clampVal2 = M.K >> 1;
								pred_err = symbol - clampVal2;
								val = GetMEDVal( pd, comp, c, r, &A, &B, &C, &D, &P);
								ABC = max( 0, min( (signed)M.K, val) );
								P = max( 0, min( (signed)M.K, P) );

								if (!pred_mode)	X[comp] = pred_err + ABC;
								else if (pred_mode == 1) X[comp] = pred_err + A;
								else if (pred_mode == 2) X[comp] = pred_err + B;
								else if (pred_mode == 3) X[comp] = pred_err + C;
								else if (pred_mode == 4) X[comp] = pred_err + D;
								else if (pred_mode == 5) X[comp] = pred_err + P;
								else if (pred_mode == 6) X[comp] = pred_err + ((A + B) >> 1);

								/* un-clamp	*/
								if (X[comp] >= (signed) M.K) X[comp] -= (signed)M.K;
								else if (X[comp] < 0) X[comp] += M.K;

								/* check best mode for next component */
								min_abs_err = abs( X[comp] - ABC);
								if (min_abs_err > abs( X[comp] - A) )
								{
									pred_mode = 1;
									min_abs_err = abs( X[comp] - A);
								}
								if (min_abs_err > abs( X[comp] - B) )
								{
									pred_mode = 2;
									min_abs_err = abs( X[comp] - B);
								}
								if (min_abs_err > abs( X[comp] - C) )
								{
									pred_mode = 3;
									min_abs_err = abs( X[comp] - C);
								}
								if (min_abs_err > abs( X[comp] - D) )
								{
									pred_mode = 4;
									min_abs_err = abs( X[comp] - D);
								}
								if (min_abs_err > abs( X[comp] - P) )
								{
									pred_mode = 5;
									min_abs_err = abs( X[comp] - P);
								}
								if (min_abs_err > abs( X[comp] - ((A + B) >> 1)) )
								{
									pred_mode = 6;
									min_abs_err = abs( X[comp] - ((A + B) >> 1));
								}
								//fprintf( stdout, "\n %d,%d: X=%d val=%d mode=%d", r, c, X[comp], val, pred_mode);
							}/* do prediction	*/
#ifdef H_DEBUG
							if ( r >= FIRSTCOL && r <= LASTCOL)
							{
								//fprintf( stdout, "\n\t   encode X[%d]=%d", comp, X[comp]);
								fprintf( stdout, "\n\t   decode symbol=%d", symbol);
								fprintf( stdout, "\n\t   ==> prediction error =%d", comp, pred_err);
							}
#endif

							Hist[comp][ symbol+1 ]++;	/* increment count of X element	*/
						} /* for( comp = 0; */
					} /* else  * stage 3	*/
				} /* if (!X_in_merged_hist_flag)  * stage 2	*/
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, "\n\t X=(");
					for( comp = 0; comp < pd->channel; comp++)
					{
						fprintf( stdout, "%d,", X[comp]);
					}
					fprintf( stdout, ")");
				}
#endif
				/* write decoded triple to image array	*/
				for ( comp= 0; comp < pd->channel; comp++)
				{
					pd->data[comp][c + r * pd->width] = X[comp];
				}
			} /* !encoder_flag	*/

			if (!X_in_merged_hist_flag /* || use_stage == 3 */)
			{
				/* integrate actual colour at top of palette or update frequency	*/
				integrate_X( palette, X, pd->channel, &numOfPaletteItems);
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					HIST_LIST_ELEM2 *current_hist_element;
					fprintf( stdout, "\n\t\t ==> new palette histo:");
					current_hist_element = palette;
					while( current_hist_element != NULL) /* list is terminated with NULL	*/
					{
						fprintf( stdout, " %d x (", current_hist_element->count);
						for (comp=0; comp< pd->channel; comp++)
						{
							fprintf( stdout, " %d,", current_hist_element->value[comp]);
						}
						fprintf( stdout, ")");
						/* goto to the next histogram element	*/
						current_hist_element = current_hist_element->next;
					}
				}
#endif
			}

			/* include information about true signal value in list for all
			* patterns, which had been found to be similar
			*/
			update_pattern_list2( &exchange, 
				X, pd->channel, X_in_merged_hist_flag, maxCountESC);
			/* if !included_flag then  also ESC count is incremented */

#ifdef LOCO_PRED
			/* LOCO update	*/
			vec_pred_update( r, c, X, Xloco, pd->channel, 
				sign, state, low_lim, up_lim, range);
#endif


			// free merged histogram
			free_stacked_hist( &merged_stacked_histogram);

		} /* for( c = 0 */

		/* show number of pattern in actual list */
		{
			unsigned long numPattern = 0;
			for (i = 0; i < numLists; i++)
			{
				numPattern += pat_list[i].pattern_list_size;
			}
			fprintf( stderr, "\r%3.2f  pattern: %d", 100.*(r+1) / pd->height, numPattern);
		}

		if (encoder_flag)
		{
			fprintf( stderr, "  Bytes: %d / %d  ", numBytes - bytes_old, numBytes);
#ifdef _DEBUG
			fprintf( stdout, "\n r=%d:  Bytes: %d / %d   ", r, numBytes - bytes_old, numBytes);
#endif
#ifdef OUT_MODE
			fprintf( mode_file, "\n");
#endif
		}
	} /* for( r = 0 */

	if (encoder_flag)
	{
		finish_encode( acSt, bitbuf);	/* finish arithmetic coding	*/
		fprintf( stderr, "\ntotal:  %d Bytes", numBytes);

		aC->num_HXC_patterns = 0;
		// for (i = 0; i <= pd->maxVal[comp]; i++ )
		for (i = 0; i < numLists; i++ )
		{
			aC->num_HXC_patterns += pat_list[i].pattern_list_size;
		}
	}
#ifdef H_DEBUG
	if ( r >= FIRSTCOL && r <= LASTCOL)
	{
		fprintf( stdout, "\n final zero order histograms\n");
		for( comp = 0; comp < pd->channel; comp++)
		{
			fprintf( stdout, "\n");
			for ( i = 0; i <= K_Hist[comp]; i++)
			{
				fprintf( stdout, "%d ", Hist[comp][i]);
			}
		}
		fprintf( stdout, "\n final palette\n");
		{
			HIST_LIST_ELEM2 *item;
			item = palette;
			do
			{
				fprintf( stdout, "%dx", item->count);
				fprintf( stdout, "(");
				for( comp = 0; comp < pd->channel; comp++)
				{
					fprintf( stdout, "%d,", item->value[comp]);
				}
				fprintf( stdout, ")");
				item = item->next;
			} while( item != NULL);
		}
	}
#endif

endHXC2:

#ifdef LOCO_PRED
	/* LOCO cleanup */
	vec_pred_finish( pd->channel, data);
#endif

	/* clear arithmetic coding	*/
	free_model( &M);
	FREE( exchange.similar_patterns);
	FREE( exchange.similar_size);
	free_imatrix( &t_actual);
	free_imatrix( &t_plist);
	free_imatrix( &tolerance);

	/* free all allocated patter list elements and the
	* corresponding histogram elements
	*/
	for (i = 0; i < numLists; i++ )
	{
		free_pattern_list2( &(pat_list[i].top));
	}
	FREE( pat_list);

	/* free all allocated  histograms	 */
	for (comp = 0; comp < pd->channel; comp++)
	{
		FREE( Hist[comp]);
	}
	//FREE( Hist);
	//FREE( Histos);

	/* free palette	*/
	free_stacked_hist( palette);
#ifdef OUT_MODE
	fprintf( mode_file, "\n");
	fclose( mode_file);
#endif

}

#endif