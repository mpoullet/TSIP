/*****************************************************
* File..: hxc3.c
* Desc..: implementation of H(X|C)3 coder
* Author: T. Strutz
* Date..: 02.09.2015
* changes:
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hxc3.h"
#include "types.h"
#include "codec.h"	/* for min( a, b);	*/
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"
#include "paeth.h"



#ifdef HXC3_EST_INF
double inf3_stage1, inf3_stage2, inf3_stage3;
unsigned long cnt3_stage1, cnt3_stage2, cnt3_stage3;
#ifndef invLOG2
	#define invLOG2	1.442695041  /* 1 / ln(2.)  */
#endif
#endif


/*--------------------------------------------------------
 * HXC3_coding()
 *--------------------------------------------------------*/
void HXC3_coding( unsigned int *lptr,		/* data line array	*/
								 unsigned long arr_length,
								long maxVal,
								BitBuffer *bitbuf, /* interface for file input and output*/
								AutoConfig *aC,  /* parameter of codec */
								 unsigned int chan,
								int encoder_flag)	/* 1 ==> encoder; 0=> decoder	*/

{
	int included_flag; /* 1 ... current colour X is in the merged histogram	*/
	long *t_actual;	/* template of actual pattern	*/
	long *t_plist	/* template from pattern	list */;
	long *tolerance;	/* tolerence in pattern comparison	*/
	long X = 999;
	unsigned int adressing_mode = 0;
	unsigned int max_similar_pattern;
	/* for re-scaling the count of the ESC symbol in pattern lists histograms */
	unsigned int maxCountESC; 
	unsigned int maxCountFac ;/* for re-scaling of histogram counts */ 
	unsigned int increaseFac; /* scaling of counts in merged stacked histogram	*/
	unsigned long pos; /* position in array	*/
	unsigned long *Hist = NULL, K_Hist;	/* zero order histograms, sizes	*/
	unsigned long i;
	unsigned long numLists;
	unsigned long symbol;
	unsigned long bytes_old; /* temporary number of written bytes	*/
	unsigned long numOfPaletteItems; /* refers to palette	*/

	/* pointer to array of pointers to single list elements	*/
	PATTERN_LIST3 *pat_list; /* top of dictionary (pattern list) */
	HIST_LIST_ELEM3 merged_stacked_histogram;
	HIST_LIST_ELEM3 palette_obj, *palette;
	EXCHANGE_STRUC3 exchange;

	acModell M;	/* normal AC context	*/
	acState acState_obj, *acSt;	/* state of arithmetic coder	*/

	acSt = &acState_obj;
	palette = &palette_obj;

#ifdef HXC3_EST_INF
	inf3_stage1 = inf3_stage2 = inf3_stage3 = 0.0;
	cnt3_stage1 = cnt3_stage2 = cnt3_stage3 = 0;
#endif

	/* default colour	*/
	palette->count = 0;
	palette->next = NULL;
	palette->value = -1;
	numOfPaletteItems = 1;

	assert( aC->HXC2increaseFac >= 0); /* #####*/


	/* start with empty dictionary list, +1 because 0 has to be included */
	/* use channel 0 as indicator, whether vector or 
	 * matrix list addressing must be used
	 */
	if (maxVal > HXC3_MAX_MAXVAL)
	{	/* linear array, slower search for matches	*/
		numLists = (maxVal + 1);
		fprintf( stderr, "\n ! HXCcoder: number of colours too high (%d > %d) !", 
			numLists, HXC3_MAX_MAXVAL);
		fprintf( stderr, "\n ! HXCcoder: must switch to linear adressing causing slow search!\n");
		adressing_mode = 0;
	}
	else
	{
		numLists = (maxVal + 1) * (maxVal + 1);
		adressing_mode = 1;
	}
	ALLOC( pat_list, numLists, PATTERN_LIST3);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXC3;
	}

	/* reset all allocated lists	*/
	for (i = 0; i < numLists; i++)
	{
		(pat_list[i]).pattern_list_size = 0;
		(pat_list[i]).top.next = NULL;
		(pat_list[i]).top.prev = NULL;
		(pat_list[i]).top.top_hist_elem = NULL; /* empty histogramm list	*/
		(pat_list[i]).top.pos = 0;
	}
	

	/* allocate histograms for all values	*/
	K_Hist = maxVal + 1;
	CALLOC( Hist, K_Hist+1, unsigned long); /* +1 as cumulative hist needs one element more	*/
	for (pos = 0; pos <= K_Hist ; pos++)
	{
		Hist[pos] = 255;/* start with large values, reduce after coding of each symbol	*/
	}

	/* allocate template for pattern (current + corresponding to pattern list	*/
	t_actual = lvector( PATTERN_SIZE3);
	t_plist = lvector( PATTERN_SIZE3);
	tolerance = lvector( PATTERN_SIZE3);

	/* pre init
	 *     
	 * D C B A X
	 */
	/* tolerance defines the maximum difference between pixels of current template and
	 * pixels of reference template. If difference is higher than tolerance, then
	 * reference template is NOT regarded as match
	 */
	maxCountESC = 0; /* 	*/
	for (pos = 0; pos < PATTERN_SIZE3; pos++)
	{
		/* only the tolerance in first component are set, so far	
		 * now lets check different tolerances for all components
		 */
		tolerance[pos] = aC->HXC2tolerance[chan][pos];
		maxCountESC += tolerance[pos];
	}

	maxCountFac = 200 + maxCountESC / (PATTERN_SIZE3);
	/* the smaller maxCountESC the earlier the count of the ESC symbol will be down-scaled	
	 * assumption: the smaller the tolerances the less influence should have the count of the ESC symbol
	 */
	maxCountESC = 8 + min( 30, maxCountESC / (3 * PATTERN_SIZE3));

#ifdef H_DEBUG
	fprintf( stdout, "\ntolerances (%d,%d,%d,%d)", 
				tolerance[0],  
				tolerance[1],  
				tolerance[2], 
				tolerance[3]
				);

#endif

	/* compute maximal required size in dependence on possible similar pattern
	 * the higher 'tolerance'	the more patterns are regareded as match, which 
	 * have to be stored in pattern list
	 */
	max_similar_pattern = 2000;  /* set to maximum	*/

	/* allocate array for pointers, one more for list termination, two needed, Why??	*/
	/* Batev: One more for list termination, and the second one for the current pattern,  
	 * if it is to be added, as well
	 */
	exchange.similar_patterns = NULL; /* suppress compiler warnings	*/
	ALLOC( exchange.similar_patterns, max_similar_pattern+2, PATTERN_LIST_ELEM3 *);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXC3;
	}
	exchange.similar_size = NULL; /* suppress compiler warnings	*/
	ALLOC( exchange.similar_size, max_similar_pattern+2, unsigned int);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endHXC3;
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
		num = arr_length + 1;
		start_model( &M, num);
		M.K = 1; /* we start with empty histogram	*/
	}
	fprintf( stderr, "\n");
	bytes_old = numBytes;
	/* process array	*/
	for( pos = 0; pos < arr_length; pos++)
	{
			if ( !(pos % 1000)) bytes_old = numBytes; /* check bytes per row	*/
			copy_pattern3( lptr, pos, t_actual);	/* get current pattern from image	*/


#ifdef H_DEBUG
			if ( pos < MAX_DEBUG_POS)
			{
				fprintf( stdout, "\n\n#pos %d:", pos);
				fprintf( stdout, " actual pattern: (");
				for( i = 0; i < PATTERN_SIZE3; i++)
				{
					fprintf( stdout, "%d,", t_actual[i]);
					fprintf( stdout, ";");
				}
				fprintf( stdout, ")");
				fflush( stdout);
				if (encoder_flag)
				{
					fprintf( stdout, "\n\t X=");
					fprintf( stdout, "%d,", lptr[pos]);
				}
			}
#endif

			/* initialise first elemnt of merged histogram	*/
			merged_stacked_histogram.count = 0; /* ESC item	*/
			merged_stacked_histogram.next = NULL;
			merged_stacked_histogram.value = -1; /* no RGBA / YUVY quadruple assigned	*/
			exchange.K_ = 0; /* merged_stacked_histogram is empty, do not count ESC item	*/

			/* search list for matching patterns,
			 * collect information about count statistics,
			 * build distribution if possible
			 * return size of histogram K_
			 */
			get_distribution3( lptr, pos, maxVal, t_actual, t_plist, pat_list, &exchange, 
				&merged_stacked_histogram, tolerance,
					max_similar_pattern, increaseFac, adressing_mode);

			// create histogramm with counts of merged histogram, + ESC symbol


			if (encoder_flag)
			{
				/* get colour vector from current position	*/
				X = lptr[pos];

				/* number of items in merged histogram	(excluding ESC symbol)*/
				if (exchange.K_ == 0)
				{
					included_flag = 0;
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
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
					included_flag = X_in_merged_hist3( &merged_stacked_histogram, X,
											&symbol, M.H, &(M.K));

					assert( M.K < MAX_HIST_SIZE3);
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
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

#ifdef HXC3_EST_INF
					{
						double p;
						p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
						inf3_stage1 -= log( p) * invLOG2;
						if (symbol) cnt3_stage1++;
					}
#endif

				}

				if (!included_flag)
				{
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
							fprintf( stdout, "\n\t X is not in merged histogram");
					}
#endif
					/* check whether X is in palette, fill histogram
					* returns zero if not in palette
					 */
					symbol = X_in_palette3( palette, X, M.H);
					M.K = numOfPaletteItems;
					create_cumulative_histogram( M.H, M.K);
					encode_AC( &M, acSt, symbol, bitbuf);

#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
						HIST_LIST_ELEM3 *current_hist_element;
						fprintf( stdout, "\n\t palette histo:");
						current_hist_element = palette;
						while( current_hist_element != NULL) /* list is terminated with NULL	*/
						{
							fprintf( stdout, " %d x ", current_hist_element->count);
							fprintf( stdout, " %d,", current_hist_element->value);
							/* goto to the next histogram element	*/
							current_hist_element = current_hist_element->next;
						}
					}
#endif
#ifdef H_DEBUG
					if ( pos < MAX_DEBUG_POS)
					{
							fprintf( stdout, "\n\t encode (palette)symbol=%d", symbol);
							fprintf( stdout, "\n\t\t using cumul.%d-distribution: ", M.K);
							for (i = 1; i <= M.K; i++)
							{
								fprintf( stdout, "%d,", M.H[i]);
							}
					}
#endif

#ifdef HXC3_EST_INF
					{
						double p;
						p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
						inf3_stage2 -= log( p) * invLOG2;
						if (symbol) cnt3_stage2++;
					}
#endif
					if (symbol == 0)
					{
#ifdef H_DEBUG
						if ( pos < MAX_DEBUG_POS)
						{
								fprintf( stdout, "\n\t ==> X is also not in palette yet");
						}
#endif
						palette->count++; /* count of symbol 0 (X not in palette)
															 * for debugging only
															 */
						/* X is not in palette, use separate coding	*/
						/* this implementation is bit consuming compared to the approach with 
						 * indexed colours, since three values have to be encoded here
						 * instead of only one index
						 * after encoding R, the distribution of G should be dependent on R,
						 * B should be dependent on R and G a.s.o.
						 */
						//encode RGBA separately
						{
							/* maximum number of different values	*/
							M.K = K_Hist;
							/* use flat distribution could be updated zero order	*/
							//for ( i = 0; i < M.K + 1; i++) M.H[i] = 0;
							/* copy zero order hist into M	*/
							memcpy( M.H, Hist, sizeof( unsigned long) * (K_Hist + 1) );
#ifdef H_DEBUG
							if ( pos < MAX_DEBUG_POS)
							{
								fprintf( stdout, "\n\t\t   using %d-distribution: ",  M.K);
								for (i = 1; i <= M.K; i++)
								{
									fprintf( stdout, "%d,", M.H[i]);
								}
							}
#endif

							create_cumulative_histogram( M.H, M.K);
							encode_AC( &M, acSt, X, bitbuf);
#ifdef H_DEBUG
							if (pos < MAX_DEBUG_POS)
							{
								fprintf( stdout, "\n\t   encode =%d", X);
							}
#endif

#ifdef HXC3_EST_INF
							{
								double p;
								p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
								inf3_stage3 -= log( p) * invLOG2;
								cnt3_stage3++; /* single count for pixel	*/
							}
#endif
							/* update zero order histogram for this component, since this element
							 * can be also a part of  another RGB or YUV vector 
							 */
							// Hist[ X+1 ]++;	/* increment count of X element, useless, as each value appears only once	*/
							Hist[ X+1 ] = 0; /* symbol never occurs again	*/
						}
					} /* if (symbol == 0)*/
				} /* if (!included_flag) */
			}
			else /* decoding	*/
			{
				/* number of items in merged histogram	(excluding ESC symbol)*/
				if (exchange.K_ > 0)
				{
					/* there is at least one item in merged histogram */

					get_merged_hist3( &merged_stacked_histogram, M.H, &(M.K));

					assert( M.K < MAX_HIST_SIZE3);
#ifdef H_DEBUG
					if (pos < MAX_DEBUG_POS)
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
					if (pos < MAX_DEBUG_POS)
					{
						fprintf( stdout, "\n\t decode (merged)symbol=%d", symbol);
					}
#endif

					if (symbol != 0)
					{
						/* store decoded signal value	*/
						/* get X from merged histogram	*/
						get_X_from_merged_hist3( &merged_stacked_histogram, &X, symbol);
						included_flag = 1;
					}
					else
					{
						included_flag = 0;
					}
				}
				else
				{
					included_flag = 0;
				}

				if (!included_flag)
				{
					unsigned long dummy;
					/* X is arbitrary, return value is nonsense, fill histogram
					 */
					dummy = X_in_palette3( palette, X, M.H);
					M.K = numOfPaletteItems;
#ifdef H_DEBUG
					if (pos < MAX_DEBUG_POS)
					{
						HIST_LIST_ELEM3 *current_hist_element;
						fprintf( stdout, "\n\t X is not in merged histogram");
						fprintf( stdout, "\n\t palette histo:");
						current_hist_element = palette;
						while( current_hist_element != NULL) /* list is terminated with NULL	*/
						{
							fprintf( stdout, " %d x (", current_hist_element->count);
							{
								fprintf( stdout, " %d,", current_hist_element->value);
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
					if (pos < MAX_DEBUG_POS)
					{
							fprintf( stdout, "\n\t decode (palette)symbol=%d", symbol);
							fprintf( stdout, "\n\t\t using cumul. %d-distribution: ", M.K);
							for (i = 1; i <= M.K; i++)
							{
								fprintf( stdout, "%d,", M.H[i]);
							}
					}
#endif
					if (symbol != 0)
					{
						get_X_from_palette3( palette, symbol, &X);
					}
					else
					{
#ifdef H_DEBUG
						if (pos < MAX_DEBUG_POS)
						{
								fprintf( stdout, "\n\t ==> X is also not in palette yet");
						}
#endif
						palette->count++; /* count of symbol 0 (X not in palette)*/
						//decode RGBA separately
						{
							/* maximum number of different values	*/
							M.K = K_Hist;
							/* use flat distribution could be updated zero order	*/
							//for ( i = 0; i < M.K + 1; i++) M.H[i] = 0;
							/* copy zero order hist into M	*/
							memcpy( M.H, Hist, sizeof( unsigned long) * (K_Hist + 1) );
#ifdef H_DEBUG
							if (pos < MAX_DEBUG_POS)
							{
								fprintf( stdout, "\n\t\t   using %d-distribution: ", M.K);
								for (i = 1; i <= M.K; i++)
								{
									fprintf( stdout, "%d,", M.H[i]);
								}
							}
#endif

							create_cumulative_histogram( M.H, M.K);
							X = decode_AC( &M, acSt, bitbuf);

#ifdef H_DEBUG
							if (pos < MAX_DEBUG_POS)
							{
								//fprintf( stdout, "\n\t   encode X[%d]=%d", comp, X[comp]);
								fprintf( stdout, "\n\t   decode X=%d", X);
							}
#endif

							//Hist[ X+1 ]++;	/* increment count of X element	*/
							Hist[ X+1 ] = 0; /* reset, symbol will never appear gain	*/ 
						}
					}
				}
#ifdef H_DEBUG
				if (pos < MAX_DEBUG_POS)
				{
					fprintf( stdout, "\n\t X=");
					{
						fprintf( stdout, "%d,", X);
					}
				}
#endif
				lptr[pos] = X;
			} /* !encoder_flag	*/

			if (!included_flag)
			{
				/* integrate actual colour at top of palette	*/
				integrate_X3( palette, X, &numOfPaletteItems);
#ifdef H_DEBUG
				if (pos < MAX_DEBUG_POS)
				{
					HIST_LIST_ELEM3 *current_hist_element;
					fprintf( stdout, "\n\t\t ==> new palette histo:");
					current_hist_element = palette;
					while( current_hist_element != NULL) /* list is terminated with NULL	*/
					{
						fprintf( stdout, " %d x (", current_hist_element->count);
						{
							fprintf( stdout, " %d,", current_hist_element->value);
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
			update_pattern_list3( &exchange, 
				X, included_flag, maxCountESC, maxCountFac, pos /* just for debugging*/);
			/* if !included_flag then  also ESC count is incremented */

			// free merged histogram
			free_stacked_hist3( &merged_stacked_histogram);

		//} /* for( c = 0 */

		/* show number of pattern in actual list */
		if ( !(pos % 1000)) 
		{
			unsigned long numPattern = 0;
			for (i = 0; i < numLists; i++)
			{
				numPattern += pat_list[i].pattern_list_size;
			}
			fprintf( stderr, "\r%3.2f  pattern: %d", 100.*(pos+1) / arr_length, numPattern);
			if (encoder_flag)
			{
				fprintf( stderr, "  %d Bytes  ", numBytes - bytes_old);
	#ifdef _DEBUG
				// fprintf( stdout, "\n r=%d:  %d Bytes  ", r, numBytes - bytes_old);
	#endif
			}
		}

	} /* for( pos = 0 */

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
	if (pos < MAX_DEBUG_POS)
	{
			fprintf( stdout, "\n final zero order histograms\n");
			{
				fprintf( stdout, "\n");
				for ( i = 0; i <= K_Hist; i++)
				{
					fprintf( stdout, "%d ", Hist[i]);
				}
			}
			fprintf( stdout, "\n final palette\n");
			{
				HIST_LIST_ELEM3 *item;
				item = palette;
				do
				{
					fprintf( stdout, "%dx", item->count);
					fprintf( stdout, "(");
					{
						fprintf( stdout, "%d,", item->value);
					}
					fprintf( stdout, ")");
					item = item->next;
				} while( item != NULL);
			}
	}
#endif

endHXC3:

	/* clear arithmetic coding	*/
	free_model( &M);
	FREE( exchange.similar_patterns);
	FREE( exchange.similar_size);
	free_lvector( &t_actual);
	free_lvector( &t_plist);
	free_lvector( &tolerance);
	
	/* free all allocated patter list elements and the
	 * corresponding histogram elements
	 */
	for (i = 0; i < numLists; i++ )
	{
		free_pattern_list3( &(pat_list[i].top));
	}

	/* free all allocated  histograms	 */
	//for (comp = 0; comp < pd->channel; comp++)
	//{
	//	FREE( Hist[comp]);
	//}
	FREE( Hist);

	/* free palette	*/
	free_stacked_hist3( palette);
	
}

