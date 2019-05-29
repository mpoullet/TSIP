/*****************************************************
* File...: TSIP_enc.c
* Desc...: implementation of TSIP encoder
* Authors: Tilo Strutz
* Data...: 29.03.2014
* changes;
* 03.09.2015 new: HXC3 mode
******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "arithmeticB.h"
#include "autoConfig.h"
#include "codec.h"
#include "bitbuf.h"
#include "colour4.h"
#include "fibocode.h"
#include "huff.h"
#include "ifc.h"
#include "imageFeatures.h"
#include "stats.h"
#include "ubwt.h"
#include "umtf.h"
#include "urle.h"
#include "ifc.h"
#include "hxc3.h"

#ifdef _DEBUG
#define Q_DEBUG
#endif

//#ifdef Q_DEBUG
//FILE *debug_out;
//#endif
//#define BWTOUT


#define PrintOUT 1
//
#undef PrintOUT
#define dPrintOUT 1
#undef dPrintOUT

#define MTFOUT 1
#undef MTFOUT
#define PALETTE
#undef PALETTE 

/*--------------------------------------------------------
* TSIP_enc()
*--------------------------------------------------------*/
void TSIP_enc( PicData *pd, 
							BitBuffer *bitbuf,
							AutoConfig *aC,
							INTERMEDIATE_VALS *iv)
{
	int stop; 
	unsigned int chan, i, cidx=0;
	unsigned int x, y, px, py, bufpos, *picbuf = NULL;
	unsigned int marker1=0, marker;	/* marker symbol for RLC1	*/
	unsigned int max_bits; /* maximum number of bits neede for max_len */
	unsigned long max_len; /* maximum number of symbols */
	unsigned long len=0, bwt;


	/* reset intermediate values, for evaluation only */
	for (chan = 0; chan < 3; chan++)
	{						
		iv->data_len[chan] = 0;
		iv->RLC1_len[chan] = 0;
		iv->RLC1_marker[chan] = 0;
		iv->RLC2_len[chan] = 0;
		iv->RLC2_marker[chan] = 0;
	}

	if (pd->channel >= 3)
	{
		/* Strutz: determine maximum numbers of symbols per scan */
		switch (aC->interleaving_mode)
		{
		case Y_U_V:
			max_len = pd->width * pd->height;
			break; /* Y U V */
		case YY_uvuv: 
			max_len = pd->width * pd->height * 2;
			break; /* YY|uvuv */
		case YY_uuvv: 
			max_len = pd->width * pd->height * 2;
			break; /* YY|uvuv */
		case YuvYuv: 
			max_len = pd->width * pd->height * 3; 
			break; /* YuvYuv */
		case YYuuvv: 
		default:
			max_len = pd->width * pd->height * 3; 
			break; /* YYuuvv */
		}
	}
	else
	{
		max_len = pd->width * pd->height;
	}

	/* Puffer für Kompressions-Zwischenschritte */
	/* decoder is not modified yet */
	ALLOC( picbuf, max_len, uint);	

	/* get number of required bits for max_len*/
	max_bits = 0;
	do
	{
		max_bits++;
		max_len >>= 1;
	}while ( max_len > 0);


	/* for all vertical blocks 
	* as image can be processed in tiles (segments)
	*/
	//for (py = 0; py < pd->height; py+= aC->segmentHeight)
	//{
	//	/* all columns	*/
	//	for (px = 0; px < pd->width; px+= aC->segmentWidth)
	py = px = 0;
	{		
		chan = 0;	/* first component */
		stop = 0;
		while (!stop)
		{
			if (aC->interleaving_mode == YuvYuv) /* |YuvYuv| */
			{
				unsigned int comp;

				stop = 1; /* all components are processed in a single scan */
				/* Strutz: Komponenten verschachteln*/
				/* all rows */
				bufpos = 0;	
				for (y = py; (y < pd->height) /* && (y < py+aC->segmentHeight)*/; y++)
				{
					/* all columns	*/
					for (x = px; (x < pd->width) /* && (x < px+aC->segmentWidth)*/ ; x++)
					{					
						/* all colour components	*/
						for (comp = 0; comp < pd->channel; comp++)
						{						
							picbuf[bufpos++] = *GetXYValue( pd, comp, x, y);		
						}
					}
				}
				cidx = 1; /* index for intermediate values and RLC1*/
				/* must not be zero, because max(bitperchan[] is needed  ?????*/
				cidx = 0; /* actual component	*/
			}
			else if (aC->interleaving_mode == YYuuvv)  /* |YYuuvv| */
			{
				unsigned int comp;

				stop = 1; /* all components are processed in a single scan */
				/* Strutz: Komponenten nicht verschachteln */
				/* all rows */
				bufpos = 0;	
				/* all colour components	*/
				for (comp = 0; comp < pd->channel; comp++)
				{						
					for (y = py; (y < pd->height) /* && (y < py+aC->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width)/* && (x < px+aC->segmentWidth)*/; x++)
						{					
							picbuf[bufpos++] = *GetXYValue( pd, comp, x, y);		
						}
					}
				}
				cidx = 1; /* index for intermediate values an RLC */
				/* must not be zero, because max(bitperchan[] is needed ????? */
				cidx = 0; /* actual component	*/
			}
			else if (aC->interleaving_mode == YY_uvuv)  /* |YY|uvuv| */
			{
				bufpos = 0;	
				if (chan == 0) /* Y separate */
				{
					for (y = py; (y < pd->height)/* && (y < py+aC->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width) /*&& (x < px+aC->segmentWidth)*/; x++)
						{					
							picbuf[bufpos++] = *GetXYValue( pd, chan, x, y);		
						}
					}

					chan++; /* next colour component in next scan */
					cidx = 0; /* index for intermediate values */
				}
				else if (chan == 1) /* interleave u and y */
				{
					unsigned int comp;

					for (y = py; (y < pd->height)/* && (y < py+aC->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width)/* && (x < px+aC->segmentWidth)*/; x++)
						{					
							/* all colour components	*/
							for (comp = 1; comp < pd->channel; comp++)
							{						
								picbuf[bufpos++] = *GetXYValue( pd, comp, x, y);		
							}
						}
					}
					chan++;
					chan++;
					cidx = 1; /* index for intermediate values */
				}
				if (chan == pd->channel) stop = 1; /* all components are processed after this scan*/
			}
			/*
			*	Änderungen am Code Gruppe Geisler
			*/
			else if (aC->interleaving_mode == YY_uuvv)  /* |YY|uuvv| */
			{
				bufpos = 0;	
				if (chan == 0) /* Y separate */
				{
					for (y = py; (y < pd->height)/* && (y < py+aC->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width) /*&& (x < px+aC->segmentWidth)*/; x++)
						{					
							picbuf[bufpos++] = *GetXYValue( pd, chan, x, y);		
						}
					}

					chan++; /* next colour component in next scan */
					cidx = 0; /* index for intermediate values */
				}
				else  /* add u and y */
				{
					unsigned int comp;
					/* add all remaining colour components	seperatly */
					for (comp = 1; comp < pd->channel; comp++)
					{
						for (y = py; (y < pd->height)/* && (y < py+aC->segmentHeight)*/; y++)
						{
							/* all columns	*/
							for (x = px; (x < pd->width)/* && (x < px+aC->segmentWidth)*/; x++)
							{					
								picbuf[bufpos++] = *GetXYValue( pd, comp, x, y);		
							}
						}
						chan++;
					}
					cidx = 1; /* index for intermediate values */
					if (chan == pd->channel) stop = 1;
				}
			}
			else  /* |Y|u|v| */
			{
				bufpos = 0;	
				for (y = py; (y < pd->height) /*&& (y < py+aC->segmentHeight)*/; y++)
				{
					/* all columns	*/
					for (x = px; (x < pd->width) /*&& (x < px+aC->segmentWidth)*/; x++)
					{					
						picbuf[bufpos++] = *GetXYValue( pd, chan, x, y);	
					}
				}

				cidx = chan; /* index for intermediate values */
				chan++; /* next colour component in next scan */
				if (chan == pd->channel) stop = 1; /* last scan*/
			}

			fprintf( stderr, " chan:%d ", cidx);	
#ifdef PrintOUT
			{
				/* get distribution of prediction erros */
				unsigned int h[1024];
				unsigned int maxPix = 0;
				unsigned int minPix = 0;
				unsigned int mi, num = 1024;
				unsigned int off = 1u << (pd->bitperchannel-1);
				int mx=0, val;
				char filename[512];
				FILE *out=NULL;

				for ( mi =0; mi < num; mi++) h[mi] = 0;

				minPix = maxPix = picbuf[0];

				for ( mi = 1; mi < bufpos; mi++) 
				{
					if (minPix > picbuf[mi]) minPix = picbuf[mi];
					if (maxPix < picbuf[mi]) maxPix = picbuf[mi];
				}


				sprintf( filename, "distrib_err_chan%d.txt", chan);
				out = fopen( filename, "wt");
				if (out == NULL)
				{
					mx = 1;
				}
				fprintf( out,"# bufpos: %5d\n", bufpos);
				fprintf( out,"# minPix: %5d\n", minPix);
				fprintf( out,"# maxPix: %5d\n", maxPix);

				for ( mi = 0; mi < bufpos; mi++) 
				{
					if (picbuf[mi] < num) 
					{
						val = picbuf[mi] - off; /* map into non-negative domain */
						if (val <= 0) val = -2*val;
						else val = 2*val - 1;
						//					val = picbuf[mi];
						h[ val]++;
					}
					else
					{
						fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
					}
				}
				fprintf( out,"# value\t count\n");

				for ( mi =0; mi < num; mi++)
				{
					fprintf( out,"%3d\t %5d\n", mi, h[mi]);
				}

				fclose( out);
			}
#endif

			/* determine greatest number in sequence */
			marker1 = picbuf[0];
			marker = picbuf[0];
			for (i = 1; i < bufpos; i++)	
			{
				if (picbuf[i] > marker1) marker1 = picbuf[i];
				if (picbuf[i] < marker) marker = picbuf[i];
			}			
			/* write number to stream */
			if (marker1 == marker)
			{
				marker1 = 0; /* constant signal	 ==> nothing else to do!*/
				WriteFiboToStream( bitbuf, marker1);
				WriteFiboToStream( bitbuf, marker);
			}
			else
			{
				marker1++;
				WriteFiboToStream( bitbuf, marker1);

				if (aC->postbwt == HXC3)
				{
					/* repeat som required operations */
					/* Lauflängencodierung anwenden und aktualisierte Zeichenzahl übernehmen,
					als Marker wird der größte Farbwert + 1 verwendet (z.B. 8Bit -> 0-255, Marker=256) */
					if (aC->skip_rlc1 == 1u)
					{
						len = bufpos;
					}
					else
					{
						fprintf( stderr, "R1");	
						if (aC->interleaving_mode == YuvYuv)
						{
							/* unlimited length of runs, if synthetic image */
							len = RLEEncode( picbuf, bufpos, marker1, pd->size);
						}
						else
						{
							/* set max. run length to one line of encoded block
							* this avoids sparseness in the histogram of symbols
							* and limits the number of different symbols
							*/
							//len = RLEEncode( picbuf, bufpos, marker1, aC->segmentWidth);
							len = RLEEncode( picbuf, bufpos, marker1, marker1 /* maximal length of run, HXC3!!*/);
						}
						iv->RLC1_marker[cidx] = marker1; /* for feedback */

					}
					iv->data_len[cidx] = bufpos;
					iv->RLC1_len[cidx] = len;

					fprintf( stderr, "B");				
					/* Burrows-Wheeler-Transformation anwenden
					und anschließend den Index der Originalzeile
					per Fibonacci-Coder speichern */
					bwt = BWTEncode( picbuf, len);
#ifdef BWTOUT
					if ((aC->skip_rlc1 == 1u))
						{
							/* save string after BWT  */
							unsigned int i;
							char filename[512];
							FILE *out=NULL;

							sprintf( filename, "encoded_BWT_%d.pgm", cidx);
							out = fopen( filename, "wt");
							fprintf( out,"P2\n");
							fprintf( out,"# len: %5d\n", len);
							fprintf( out,"# bwt: %5d\n", bwt);
							fprintf( out,"%5d %5d\n", pd->width, pd->height);
							fprintf( out,"%5d", marker1);

							//for ( i = 0; i < _GETMIN(len,1000); i++) 
							for ( i = 0; i < len; i++) 
							{
								if ( i % pd->width == 0) fprintf( out,"\n");
								fprintf( out," %4d", picbuf[i]);
							}
							fprintf( out,"\n");
							fclose( out);
						}
#endif

					/**/
					WriteValToStream( bitbuf, len, max_bits);
					WriteValToStream( bitbuf, bwt, max_bits);

					fprintf( stderr, "HX");				
					HXC3_coding( picbuf, len, marker1 /*maxVal*/, bitbuf, aC, cidx, 1 /* encoder_flag*/);
				}
				else
				{

					/* Lauflängencodierung anwenden und aktualisierte Zeichenzahl übernehmen,
					als Marker wird der größte Farbwert + 1 verwendet (z.B. 8Bit -> 0-255, Marker=256) */
					if (aC->skip_rlc1 == 1u)
					{
						len = bufpos;
					}
					else
					{
						fprintf( stderr, "R1");	
						if (aC->interleaving_mode == YuvYuv)
						{
							/* unlimited length of runs, if synthetic image */
							len = RLEEncode( picbuf, bufpos, marker1, pd->size);
						}
						else
						{
							/* set max. run length to one line of encoded block
							* this avoids sparseness in the histogram of symbols
							* and limits the number of different symbols
							*/
							//len = RLEEncode( picbuf, bufpos, marker1, aC->segmentWidth);
							len = RLEEncode( picbuf, bufpos, marker1, pd->width);
						}
						iv->RLC1_marker[cidx] = marker1; /* for feedback */

					}
					iv->data_len[cidx] = bufpos;
					iv->RLC1_len[cidx] = len;

#ifdef RLC1OUT
					{
						/* save string before BWT  */
						unsigned int i;
						char filename[512];
						FILE *out=NULL;

						sprintf( filename, "encoded_RLC1_%d.txt", chan);
						out = fopen( filename, "wt");
						fprintf( out,"# len: %5d\n", len);

						for ( i = 0; i < len; i++) 
						{
							fprintf( out,"%4d %4d", i, picbuf[i]);
							if 	(picbuf[i] == iv->RLC1_marker[cidx])
							{
								fprintf( out," *\n");
							}
							else
							{
								fprintf( out,"\n");
							}
						}
						fclose( out);
					}
#endif

#ifdef PrintOUT
					{
						/* get distribution of BWT input */
						unsigned int h[1024];
						unsigned int maxPix = 0;
						unsigned int minPix = 0;
						unsigned int mi, marker = 1u << pd->bitperchannel;
						unsigned int off = 1u << (pd->bitperchannel-1);
						int mx=0, val;
						char filename[512];
						FILE *out=NULL;

						for ( mi =0; mi <= marker; mi++) h[mi] = 0;

						minPix = maxPix = picbuf[0];

						for ( mi = 1; mi < len; mi++) 
						{
							if (minPix > picbuf[mi]) minPix = picbuf[mi];
							if (maxPix < picbuf[mi]) maxPix = picbuf[mi];
						}


						sprintf( filename, "distrib_rlc1_chan%d.txt", chan);
						out = fopen( filename, "wt");
						if (out == NULL)
						{
							mx = 1;
						}
						fprintf( out,"# values  are mapped into non-negative domain\n");
						fprintf( out,"# bufpos: %5d\n", bufpos);
						fprintf( out,"# len: %5d\n", len);
						fprintf( out,"# marker: %5d\n", (1u << pd->bitperchannel));
						fprintf( out,"# minPix: %5d\n", minPix);
						fprintf( out,"# maxPix: %5d\n", maxPix);

						for ( mi = 0; mi < len; mi++) 
						{
							if (picbuf[mi] == marker)
							{
								h[ picbuf[mi]]++;
								mi++;
								if (picbuf[mi] < marker) 
								{
									val = picbuf[mi] - off; /* map into non-negative domain */
									if (val <= 0) val = -2*val;
									else val = 2*val - 1;
									h[ val]++;
									fprintf( out,"# symbol: %5d\t", picbuf[mi]);
								}
								else
								{
									fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
								}
								mi++;
								fprintf( out," run[%d] = %5d\n", mi, picbuf[mi]);
								h[ picbuf[mi]]++;  /* Run */
							}
							else
							{
								if (picbuf[mi] < marker) 
								{
									val = picbuf[mi] - off; /* map into non-negative domain */
									if (val < 0) val = -2*val - 1;
									else val = 2*val;
									h[ val]++;
								}
								else
								{
									fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
								}
							}
						}
						fprintf( out,"# value\t count\n");

						for ( mi =0; mi <= marker; mi++)
						{
							fprintf( out,"%3d\t %5d\n", mi, h[mi]);
						}

						fclose( out);
					}
#endif

					if (aC->skip_precoding == 0)
					{	
						fprintf( stderr, "B");				
						/* Burrows-Wheeler-Transformation anwenden
						und anschließend den Index der Originalzeile
						per Fibonacci-Coder speichern */
						bwt = BWTEncode( picbuf, len);
						//WriteFiboToStream( bitbuf, bwt);

						/**/
						WriteValToStream( bitbuf, bwt, max_bits);

						/**/
						//fclose( out);
#ifdef BWTOUT
						{
							/* save string after BWT  */
							unsigned int i;
							char filename[512];
							FILE *out=NULL;

							sprintf( filename, "encoded_BWT_%d.txt", chan);
							out = fopen( filename, "wt");
							fprintf( out,"# len: %5d\n", len);
							fprintf( out,"# bwt: %5d\n", bwt);

							//for ( i = 0; i < _GETMIN(len,1000); i++) 
							for ( i = 0; i < len; i++) 
							{
								fprintf( out,"%4d %4d", i, picbuf[i]);
								if 	(picbuf[i] == iv->RLC1_marker[cidx])
								{
									fprintf( out," *\n");
								}
								else
								{
									fprintf( out,"\n");
								}
							}
							fclose( out);
						}
#endif
						if (!aC->skip_postBWT)
						{
							/* Welches Post-BWT-Verfahren? */							
							if (aC->postbwt == IFC)
							{					
								fprintf( stderr, "I");				
								/* incremental Frequency Count */
								IFCencode( picbuf, len, aC->IFCresetVal, aC->IFCmaxCount);
							}
							else
							{	
								fprintf( stderr, "M");				
								/* Move-To-Front-Codierung */					
								MTFEncode( picbuf, len, aC->MTF_val);
							}
						}

#ifdef MTFOUT
						{
							/* save string after BWT  */
							unsigned int i;
							char filename[512];
							FILE *out=NULL;

							if (pbwt == IFC)
								sprintf( filename, "encoded_IFC_%d.txt", chan);
							else
								sprintf( filename, "encoded_MTF_%d.txt", chan);
							out = fopen( filename, "wt");
							fprintf( out,"# len: %5d\n", len);
							fprintf( out,"# bwt: %5d\n", bwt);

							//for ( i = 0; i < _GETMIN(len,1000); i++) 
							for ( i = 0; i < len; i++) 
							{
								fprintf( out,"%4d %4d", i, picbuf[i]);
								if 	(picbuf[i] == iv->RLC1_marker[cidx])
								{
									fprintf( out," *\n");
								}
								else
								{
									fprintf( out,"\n");
								}
							}
							fclose( out);
						}
#endif
#ifdef PrintOUT
						{
							/* get distribution of IFC output */
							unsigned int h[513];
							unsigned int maxPix = 0;
							unsigned int minPix = 0;
							unsigned int mi, marker = 1u << pd->bitperchannel;
							int mx=0;
							char filename[512];
							FILE *out=NULL;

							for ( mi =0; mi <= marker; mi++) h[mi] = 0;

							minPix = maxPix = picbuf[0];

							for ( mi = 1; mi < len; mi++) 
							{
								if (minPix > picbuf[mi]) minPix = picbuf[mi];
								if (maxPix < picbuf[mi]) maxPix = picbuf[mi];
							}


							sprintf( filename, "distrib_IFC_chan%d.txt", chan);
							out = fopen( filename, "wt");
							if (out == NULL)
							{
								mx = 1;
							}
							fprintf( out,"# len: %5d\n", len);
							fprintf( out,"# minPix: %5d\n", minPix);
							fprintf( out,"# maxPix: %5d\n", maxPix);

							for ( mi = 0; mi < len; mi++) 
							{
								if (picbuf[mi] <= marker) 
									h[ picbuf[mi]]++;
								else
								{
									fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
								}
							}
							fprintf( out,"# value\t count\n");

							for ( mi =0; mi <= marker; mi++)
							{
								fprintf( out,"%3d\t %5d\n", mi, h[mi]);
							}

							fclose( out);
						}
#endif

						/* determine greatest number in sequence */
						for (i = 0, marker = 0; i < len; i++)	
						{
							if (picbuf[i] > marker) marker = picbuf[i];
						}			
						/* write number to stream */
						WriteFiboToStream( bitbuf, marker);

						fprintf(stderr, "R2");	
						/* set max. run length to maximal sequence length == unlimited */
						if (aC->use_RLC2zero) /* only zero runs	*/
						{
							len = RLC2zero_Encode( picbuf, len, marker + 1u, pd->size*3);
						}
						else
						{
							len = RLEEncode( picbuf, len, marker + 1u, pd->size*3);
						}

						iv->RLC2_len[cidx] = len;
						iv->RLC2_marker[cidx] = marker + 1u;


#ifdef PrintOUT
						{
							/* get distribution of RLC2 input */
							unsigned int mi, h[1024];
							unsigned int maxPix = 0;
							unsigned int minPix = 0;
							int mx=0;
							char filename[512];
							FILE *out=NULL;

							for ( mi =0; mi <= marker+1; mi++) h[mi] = 0;

							minPix = maxPix = picbuf[0];

							for ( mi = 1; mi < len; mi++) 
							{
								if (minPix > picbuf[mi]) minPix = picbuf[mi];
								if (maxPix < picbuf[mi]) maxPix = picbuf[mi];
							}


							sprintf( filename, "distrib_rlc2_chan%d.txt", chan);
							out = fopen( filename, "wt");
							if (out == NULL)
							{
								mx = 1;
							}
							fprintf( out,"# len: %5d\n", len);
							fprintf( out,"# marker: %5d\n", marker + 1u);
							fprintf( out,"# minPix: %5d\n", minPix);
							fprintf( out,"# maxPix: %5d\n", maxPix);

							for ( mi = 0; mi < len; mi++) 
							{
								if (picbuf[mi] == marker+1)
								{
									h[ picbuf[mi]]++;  /* Token */
									mi++;
									if (picbuf[mi] <= marker+1)   
									{
										h[ picbuf[mi]]++;/* Symbol */
										fprintf( out,"# symbol: %5d\t", picbuf[mi]);
									}
									else
									{
										fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
									}
									mi++;
									fprintf( out," run[%d] = %5d\n", mi, picbuf[mi]);
									h[ picbuf[mi]]++;  /* Run */
								}
								else
								{
									if (picbuf[mi] <= marker+1) 
										h[ picbuf[mi]]++;
									else
									{
										fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
									}
								}
							}
							fprintf( out,"# value\t count\n");

							for ( mi =0; mi <= marker+1; mi++)
							{
								fprintf( out,"%3d\t %5d\n", mi, h[mi]);
							}

							fclose( out);
						}
#endif
					}	/* REGULAR */
					else
					{
						/* if precoding was skipped, Huffman coder still need a value for marker */
						marker = marker1;
					}


					/* do the entropy coding */		
					if (aC->separate_coding_flag == 1) /* use  separate mode */
					{
						unsigned int *runbuf=NULL, lenSym, lenRun;

						/* in maximum every third symbol is a run length */
						ALLOC( runbuf, (len+2)/3, uint); /* +2 safe guard if len < 3	*/
						lenSym = lenRun = 0;

						if (aC->use_RLC2zero) /* only zero runs	*/
						{
							for ( i = 0; i < len; i++)
							{
								if (picbuf[i] == marker+1) /* see above, ESC token for RLC2 is marker+1*/
								{
									/* RLC2 - David Banko */
									picbuf[lenSym++] = picbuf[i];	/* copy token	*/
									runbuf[lenRun++] = picbuf[i+1]; /* token|Run */
									i++;	/* jump over Run */
								}
								else
								{
									picbuf[lenSym++] = picbuf[i];	/* copy token	*/
								}
							}
						}
						else
						{
							for ( i = 0; i < len; i++)
							{
								if (picbuf[i] == marker+1) /* see above, ESC token for RLC2 is marker+1*/
								{
									picbuf[lenSym++] = picbuf[i];	/* copy token	*/
									picbuf[lenSym++] = picbuf[i+1];	/* copy symbol	*/
									runbuf[lenRun++] = picbuf[i+2]; /* token|Symbol|Run */
									i+=2;	/* jump over Symbol and Run */
								}
								else
								{
									picbuf[lenSym++] = picbuf[i];	/* copy token	*/
								}
							}
						}
						/* Anzahl der übrigen Elemente schreiben */
						WriteFiboToStream( bitbuf, lenSym);	

						/* lenRun must not written to the bitstream, since it can be derived 
						* from the number of tokens (marker)
						*/
						if (aC->entropy_coding_method == 0)
						{
							fprintf(stderr, "HC");	
							HuffEncode( picbuf, lenSym, bitbuf);
							/* it may happen that there is no run for RLC2 */
							if (lenRun) HuffEncode( runbuf, lenRun, bitbuf);
						}
						else
						{
							fprintf(stderr, "AC");	
							AC_Encode( picbuf, lenSym, bitbuf, 'S');
							/* it may happen that there is no run for RLC2 */
							if (lenRun) AC_Encode( runbuf, lenRun, bitbuf, 'R');
						}

#ifdef PrintOUT
						{
							/* get distribution of RLC2 w/o runs output */
							unsigned int h[1024];
							unsigned int maxPix = 0;
							unsigned int mi, minPix = 0;
							int mx=0;
							char filename[512];
							FILE *out=NULL;

							for ( mi =0; mi <= marker+1; mi++) h[mi] = 0;

							minPix = maxPix = picbuf[0];

							for ( mi = 1; mi < lenSym; mi++) 
							{
								if (minPix > picbuf[mi]) minPix = picbuf[mi];
								if (maxPix < picbuf[mi]) maxPix = picbuf[mi];
							}

							sprintf( filename, "distrib_rlc2-woRuns_chan%d.txt", chan);
							out = fopen( filename, "wt");
							fprintf( out,"# lenSym: %5d\n", lenSym);
							fprintf( out,"# marker: %5d\n", marker + 1u);
							fprintf( out,"# minPix: %5d\n", minPix);
							fprintf( out,"# maxPix: %5d\n", maxPix);

							for ( mi = 0; mi < lenSym; mi++) 
							{
								{
									if (picbuf[mi] <= marker+1) 
										h[ picbuf[mi]]++;
									else
									{
										fprintf( out,"# picbuf[%d]: %5d\n", mi, picbuf[mi]);
									}
								}
							}
							fprintf( out,"# value\t count\n");

							for ( mi =0; mi <= marker+1; mi++)
							{
								fprintf( out,"%3d\t %5d\n", mi, h[mi]);
							}
							fclose( out);
						}
#endif

#ifdef PrintOUT
						{
							/* get distribution of RLC2 runs  */
							unsigned int h[1024];
							unsigned int maxPix = 0;
							unsigned int mi, minPix = 0;
							int mx=0;
							char filename[512];
							FILE *out=NULL;

							for ( mi =0; mi < 1024; mi++) h[mi] = 0;

							minPix = maxPix = runbuf[0];
							for ( mi = 1; mi < lenRun; mi++) 
							{
								if (minPix > runbuf[mi]) minPix = runbuf[mi];
								if (maxPix < runbuf[mi]) maxPix = runbuf[mi];
							}


							sprintf( filename, "distrib_rlc2_Runs_chan%d.txt", chan);
							out = fopen( filename, "wt");
							fprintf( out,"# lenRun: %5d\n", lenRun);
							fprintf( out,"# minPix: %5d\n", minPix);
							fprintf( out,"# maxPix: %5d\n", maxPix);

							for ( mi = 0; mi < lenRun; mi++) 
							{
								{
									if (picbuf[mi] < 1024) 
										h[ runbuf[mi]]++;
									else
									{
										fprintf( out,"# picbuf[%d]: %5d\n", mi, runbuf[mi]);
									}
								}
							}
							fprintf( out,"# value\t count\n");

							for ( mi =0; mi <= maxPix; mi++)
							{
								fprintf( out,"%3d\t %5d\n", mi, h[mi]);
							}
							fclose( out);
						}
#endif

						FREE(runbuf);
					}
					else  /* joint Huffman mode */
					{
						/* Anzahl der übrigen Elemente schreiben */
						WriteFiboToStream( bitbuf, len);	
						if (aC->entropy_coding_method == 0)
						{
							HuffEncode( picbuf, len, bitbuf);
						}
						else
						{
							//AC_Encode( picbuf, len, bitbuf, 'S');
							AC_Encode( picbuf, len, bitbuf, 'R');
						}
					}
				} /* not HXC3 */

			} /* if marker1 > 1	*/
		}	/* while ( !stop)	*/
	}/* px */
	//} /* py */

	FREE( picbuf);

}
