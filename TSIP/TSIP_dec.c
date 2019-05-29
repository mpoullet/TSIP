/*****************************************************
* Datei: TSIP_dec.c
* Desc.: implementation of TSIP decoder 
* Autor: Tilo Strutz
* Datum: 29.03.2014
* changes:
* 03.09.2015 new: HXC3 mode
******************************************************/
#include <limits.h>
#include <assert.h>
#include "adjmap.h"
#include "arithmeticB.h"
#include "bitbuf.h"
#include "codec.h"
#include "colour4.h"
#include "fibocode.h"
#include "header.h"
#include "huff.h"
#include "stats.h"
#include "ubwt.h"
#include "umtf.h"
#include "urle.h"
#include "ifc.h"
#include "hxc3.h"

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
* TSIP_dec()
* decoding of TSIP encoded images
*-------------------------------------------------------*/
void TSIP_dec( PicData *pd, PicHeader *ph, BitBuffer *bitbuf)
{
	int stop;
	unsigned int x, y, px, py, i, chan;

	unsigned int bufpos, *picbuf = NULL,
		bwt = 0, bufsize,
		marker = 0;
	unsigned int marker1;	/* marker symbol for RLC1	*/
	unsigned int max_bits; /* maximum number of bits neede for max_len */
	unsigned long max_len=0; /* maximum number of symbols */
	unsigned long len, cidx=0; /*  */

	if (pd->channel >= 3)
	{
		switch (ph->interleaving_mode)
		{
		case Y_U_V:
			max_len = ph->width * ph->height; cidx = 0; 
			break; /* Y U V */
		case YY_uvuv: 
			max_len = ph->width * ph->height * 2; cidx = 0; 
			break; /* YY|uvuv */
		case YY_uuvv: 
			max_len = ph->width * ph->height * 2; cidx = 0; 
			break; /* YY|uuvv */
		case YuvYuv: 
			max_len = ph->width * ph->height * 3;  /*cidx = 1; */ cidx = 0;
			break; /* YuvYuv */
		case YYuuvv: 
			max_len = ph->width * ph->height * 3;  /*cidx = 1;*/ cidx = 0;
			break; /* YYuuvv */
		}
	}
	else
	{
		max_len = ph->width * ph->height;
		cidx = 0;
	}

	/* get number of required bits, needed for receiving bwt-value and RLC2-marker */
	max_bits = 0;
	do
	{
		max_len >>= 1;
		max_bits++;
	}while ( max_len > 0);

	/* Arbeitspuffer allokieren *
	bufsize = (unsigned long)ph->width * 
	(unsigned long)ph->height * (unsigned long)ph->channel;
	*/
	bufsize = (unsigned long)ph->width * 
		(unsigned long)ph->height * (unsigned long)ph->channel;
	//bufsize = (unsigned long)ph->segmentWidth * 
	//	(unsigned long)ph->segmentHeight * (unsigned long)ph->channel;
	ALLOC( picbuf, bufsize, uint);

	/* for all vertical blocks*/
	//for (py = 0; py < pd->height; py+= ph->segmentHeight)
	//{
	//	/* all columns	*/
	//	for (px = 0; px < pd->width; px+= ph->segmentWidth)
	px = py = 0;
	{		
		chan = 0;	/* first component */
		stop = 0;
		while (!stop)
		{
			/* read token for RLC1	*/
			marker1 = GetFiboFromStream( bitbuf); 
			if (marker1)
			{

				if (ph->postbwt == HXC3)
				{
					AutoConfig autoCofig, *aC;
					aC = &autoCofig;

					len = ReadValFromStream( bitbuf, max_bits);	
					bwt = ReadValFromStream( bitbuf, max_bits);	
					/* copy required parameters	*/
					aC->HXC2increaseFac = ph->HXC2increaseFac;
					aC->HXC2tolerance[0][0] = ph->HXC2tolerance[0][0];
					aC->HXC2tolerance[0][1] = ph->HXC2tolerance[0][1];
					aC->HXC2tolerance[0][2] = ph->HXC2tolerance[0][2];
					aC->HXC2tolerance[0][3] = ph->HXC2tolerance[0][3];
					HXC3_coding( picbuf, len, marker1, bitbuf, aC, cidx, 0 /* decoder_flag*/);

					fprintf( stderr, "B");
					/* Burrows-Wheeler-Transformation invertieren */								
					BWTDecode( picbuf, len, bwt);									
					/* undo RLC1 */
					if (ph->skip_rlc1 == 0)
					{
						fprintf( stderr, "R1");
						int new_len;
						new_len = RLEDecode( picbuf, len, marker1, bufsize);
					}
				}
				else
				{
					/* BWT-Index, Marker der RLC und Menge der gespeicherten Daten
					dieses Blocks holen */
					if (ph->skip_precoding == 0)
					{
						/* if marker1 == 1, then all values in sequence are zero ==> nothing to do*/
						// bwt    = GetFiboFromStream( bitbuf);
						bwt = ReadValFromStream( bitbuf, max_bits);	
						/* how many bits are required for marker?? */
						marker = GetFiboFromStream( bitbuf); 
					}
					else marker = marker1;

					/* Entropy-Codierung rückgängig machen,
					dazu werden die Bits aus dem Dateistrom gelesen
					und in den Arbeitspuffer geschrieben */
					if (ph->separate_coding_flag == 1)
					{
						unsigned int *tmpbuf=NULL, *runbuf=NULL, lenSym, lenRun;

						lenSym = GetFiboFromStream( bitbuf);			
						ALLOC( tmpbuf, bufsize, uint);

						if (ph->entropy_coding_method == 0)
						{
							fprintf( stderr, "HC");	
							HuffDecode( tmpbuf, lenSym, bitbuf);
						}
						else
						{
							fprintf( stderr, "AC");	
							AC_Decode( tmpbuf, lenSym, bitbuf);
						}
						/* determine number of runs	*/
						lenRun = 0;
						for ( i = 0; i < lenSym; i++)
						{
							/* see below, token for RLC2 is marker+1*/
							if (tmpbuf[i] == marker+1) 	lenRun++;
						}

#ifdef dPrintOUT
						{
							/* get distribution of dRLC2 runs  */
							unsigned int h[1024];
							unsigned int maxPix = 0;
							unsigned int minPix = 0;
							int mi, mx=0;
							char filename[512];
							FILE *out=NULL;

							for ( mi =0; mi < 1024; mi++) h[mi] = 0;

							minPix = maxPix = tmpbuf[0];
							for ( mi = 1; mi < lenSym; mi++) 
							{
								if (minPix > tmpbuf[mi]) minPix = tmpbuf[mi];
								if (maxPix < tmpbuf[mi]) maxPix = tmpbuf[mi];
							}


							sprintf( filename, "distrib_dRLC2_woRuns_chan%d.txt", chan);
							out = fopen( filename, "wt");
							fprintf( out,"# lenSym: %5d\n", lenSym);
							fprintf( out,"# lenRun: %5d\n", lenRun);
							fprintf( out,"# minPix: %5d\n", minPix);
							fprintf( out,"# maxPix: %5d\n", maxPix);

							fclose( out);
						}
#endif

						if (lenRun) /* if there is at least one run */
						{
							ALLOC( runbuf, lenRun, uint); /* for run length values */
							if (ph->entropy_coding_method == 0)
							{
								HuffDecode( runbuf, lenRun, bitbuf);
							}
							else
							{
								AC_Decode( runbuf, lenRun, bitbuf);
							}
						}

						len = lenRun = 0;

						if (ph->use_RLC2zero) /* only zero runs	*/
						{
							for ( i = 0; i < lenSym; i++)
							{
								picbuf[len++] = tmpbuf[i];	/* copy symbol	*/
								if (tmpbuf[i] == marker+1) /* see above, token for RLC2 is marker+1*/
								{
									/* RLC2 - David Banko */
									picbuf[len++] = runbuf[lenRun++]; /* token|Run */
								}
							}
						}
						else
						{
							for ( i = 0; i < lenSym; i++)
							{
								picbuf[len++] = tmpbuf[i];	/* copy symbol	*/
								if (tmpbuf[i] == marker+1) /* see above, token for RLC2 is marker+1*/
								{
									picbuf[len++] = tmpbuf[i+1];	/* copy symbol	*/
									picbuf[len++] = runbuf[lenRun++]; /* token|Symbol|Run */
									i++;	/* jump over Symbol  */
								}
							}
						}

#ifdef dPrintOUT
						{
							/* get distribution of dRLC2 runs  */
							unsigned int h[1024];
							unsigned int maxPix = 0;
							unsigned int minPix = 0;
							int mi, mx=0;
							char filename[512];
							FILE *out=NULL;

							for ( mi =0; mi < 1024; mi++) h[mi] = 0;

							minPix = maxPix = tmpbuf[0];
							for ( mi = 1; mi < lenSym; mi++) 
							{
								if (minPix > tmpbuf[mi]) minPix = tmpbuf[mi];
								if (maxPix < tmpbuf[mi]) maxPix = tmpbuf[mi];
							}


							sprintf( filename, "distrib_dRLC2_chan%d.txt", chan);
							out = fopen( filename, "wt");
							fprintf( out,"# lenSym: %5d\n", lenSym);
							fprintf( out,"# lenRun: %5d\n", lenRun);
							fprintf( out,"# len: %5d\n", len);
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

						FREE(tmpbuf);
						if (lenRun) FREE(runbuf);
					}
					else
					{
						len = GetFiboFromStream( bitbuf);			
						if (ph->entropy_coding_method == 0)
						{
							fprintf(stderr, "HC");	
							HuffDecode( picbuf, len, bitbuf);		
						}
						else
						{
							fprintf(stderr, "AC");	
							AC_Decode( picbuf, len, bitbuf);
						}
					}

					if (ph->skip_precoding == 0)
					{				
						fprintf(stderr, "R2");	
						/* undo RLC2, return length of symbol sequence */
						if (ph->use_RLC2zero) /* only zero runs	*/
						{
							len = RLC2zero_Decode( picbuf, len, marker + 1u, bufsize);	
						}
						else
						{
							len = RLEDecode( picbuf, len, marker + 1u, bufsize);
						}
						fprintf( stderr, "L2");

#ifdef iRLC2
						{
							/* save string after iRLC2  */
							unsigned int i;
							char filename[512];
							FILE *out=NULL;

							sprintf( filename, "decoded_RLC2_%d.txt", chan);

							out = fopen( filename, "wt");
							fprintf( out,"# len: %5d\n", len);

							//for ( i = 0; i < _GETMIN(len,1000); i++) 
							for ( i = 0; i < len; i++) 
							{
								fprintf( out,"%4d %4d", i, picbuf[i]);
								if 	(picbuf[i] == marker + 1u)
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
						if (!ph->skip_postBWT)
						{
							/* PostBWT-Verfahren rückgängig machen */			
							if ( ph->postbwt == IFC) 
							{						
								fprintf( stderr, "I");	
								/* Weighted Frequency Count */
								//WFCDecode( picbuf, len, ph->pbwtparam, ph->maxIFCCount);	
								IFCdecode( picbuf, len, ph->pbwtparam, ph->maxIFCCount);	
							}									
							else 		
							{
								fprintf( stderr, "M");	
								/* Move-To-Front-Codierung */
								MTFDecode( picbuf, len, ph->pbwtparam);	
							}			
						}
#ifdef MTFOUT
						{
							/* save string after iBWT  */
							unsigned int i;
							char filename[512];
							FILE *out=NULL;

							if ( (PostBWT)ph->postbwt == IFC) 
								sprintf( filename, "decoded_IFC_%d.txt", chan);
							else
								sprintf( filename, "decoded_MTF_%d.txt", chan);

							out = fopen( filename, "wt");
							fprintf( out,"# len: %5d\n", len);
							fprintf( out,"# bwt: %5d\n", bwt);

							//for ( i = 0; i < _GETMIN(len,1000); i++) 
							for ( i = 0; i < len; i++) 
							{
								fprintf( out,"%4d %4d", i, picbuf[i]);
								if 	(picbuf[i] == marker + 1u)
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

						fprintf( stderr, "B");
						/* Burrows-Wheeler-Transformation invertieren */								
						BWTDecode( picbuf, len, bwt);									
					} /* Regular */

#ifdef BWTOUT
					{
						/* save string after iBWT  */
						unsigned int i;
						char filename[512];
						FILE *out=NULL;

						sprintf( filename, "decoded_BWT_%d.txt", chan);
						out = fopen( filename, "wt");
						fprintf( out,"# len: %5d\n", len);

						for ( i = 0; i < len; i++) 
						{
							fprintf( out,"%4d %4d", i, picbuf[i]);
							if 	(picbuf[i] == (1u << pd->bitperchan[cidx]))
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
					/* undo RLC1 */
					if (ph->skip_rlc1 == 0)
					{
						fprintf( stderr, "R1");
						int new_len;
						//							new_len = RLEDecode( picbuf, len, (1u << pd->bitperchan[cidx]), 
						//																	bufsize);
						new_len = RLEDecode( picbuf, len, marker1, bufsize);
#ifdef Q_DEBUG
						{
							long size = 0;
							if (ph->interleaving_mode == YuvYuv || ph->interleaving_mode == YYuuvv)
							{
								size = bufsize;
							}
							else if(ph->interleaving_mode == Y_U_V)
							{
								size = bufsize / pd->channel;
							}
							else if(ph->interleaving_mode == YY_uvuv)
							{
								size = bufsize / pd->channel;
								if (chan >0) size *=2;
							}
							else if(ph->interleaving_mode == YY_uuvv)
							{
								size = bufsize / pd->channel;
								if (chan >0) size *=2;
							}
							// Strutz is only the same if no segmentation is used	*/
							// assert( new_len == size);
						}
#endif
					} /* f (ph->skip_rlc1 == 0)*/
				} /* not HXC3	*/
			}
			else /* if marker1	*/
			{
				unsigned int value;
				unsigned long l;
				/* constant signal	*/
				value = GetFiboFromStream( bitbuf);
				for (l = 0; l < bufsize; l++)
				{
					picbuf[l] = value;
				}
			}

			if (ph->interleaving_mode == YuvYuv) /* YuvYuv */
			{
				unsigned int comp;

				stop = 1; /* all components are processed in a single scan */
				/* Strutz: Komponenten verschachteln*/
				/* all rows */
				bufpos = 0;	
				for (y = py; (y < pd->height) /*&& (y < py+ph->segmentHeight)*/; y++)
				{
					/* all columns	*/
					for (x = px; (x < pd->width) /*&& (x < px+ ph->segmentWidth)*/; x++)
					{					
						/* all colour components	*/
						for (comp = 0; comp < pd->channel; comp++)
						{						
							*GetXYValue(pd, comp, x, y) = picbuf[bufpos++];									
						}
					}
				}
			}
			else if (ph->interleaving_mode == YYuuvv)  /* |YYuuvv| */
			{
				unsigned int comp;

				stop = 1; /* all components are processed in a single scan */
				/* Strutz: Komponenten nicht verschachteln*/
				/* all rows */
				bufpos = 0;	
				/* all colour components	*/
				for (comp = 0; comp < pd->channel; comp++)
				{						
					for (y = py; (y < pd->height) /*&& (y < py+ph->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width) /*&& (x < px+ph->segmentWidth)*/; x++)
						{					
							*GetXYValue(pd, comp, x, y) = picbuf[bufpos++];									
						}
					}
				}
			}
			else if (ph->interleaving_mode == YY_uvuv)  /* |YY|uvuv| */
			{
				unsigned int comp;

				bufpos = 0;	
				if ( chan == 0) /* Y separate */
				{
					for (y = py; (y < pd->height) /*&& (y < py+ph->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width) /*&& (x < px+ph->segmentWidth)*/; x++)
						{					
							*GetXYValue( pd, chan, x, y) = picbuf[bufpos++];		
						}
					}
					chan++; /* next colour component in next scan */
					cidx++; /* next index for RLC1 marker */
				}
				else
				{
					for (y = py; (y < pd->height) /*&& (y < py+ph->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width) /*&& (x < px+ph->segmentWidth)*/; x++)
						{					
							/* all colour components	*/
							for (comp = 1; comp < pd->channel; comp++)
							{						
								*GetXYValue( pd, comp, x, y) = picbuf[bufpos++];									
							}
						}
					}
					chan++;
					chan++;
				}

				if (chan == pd->channel) stop = 1; /* all components are processed after this scan*/
			}
			/*
			*	Änderungen am Code Gruppe Geisler
			*/
			else if (ph->interleaving_mode == YY_uuvv)  /* |YY|uuvv| */
			{
				bufpos = 0;	
				if (chan == 0) /* Y separate */
				{
					for (y = py; (y < pd->height) /*&& (y < py+ph->segmentHeight)*/; y++)
					{
						/* all columns	*/
						for (x = px; (x < pd->width) /*&& (x < px+ph->segmentWidth)*/; x++)
						{					
							*GetXYValue( pd, chan, x, y) = picbuf[bufpos++];		
						}
					}

					chan++; /* next colour component in next scan */
					cidx++; /* index for intermediate values */
				}
				else  /* add u and y */
				{
					unsigned int comp;
					/* add all remaining colour components	seperatly */
					for (comp = 1; comp < pd->channel; comp++)
					{
						for (y = py; (y < pd->height)/* && (y < py+ph->segmentHeight)*/; y++)
						{
							/* all columns	*/
							for (x = px; (x < pd->width) /* && (x < px+ph->segmentWidth)*/; x++)
							{					
								*GetXYValue( pd, comp, x, y) = picbuf[bufpos++] ;		
							}
						}
						chan++;
					}
					if (chan == pd->channel) stop = 1;
				}
			}
			else /* Y U V */
			{
				bufpos = 0;	
				for (y = py; (y < pd->height) /*&& (y < py+ph->segmentHeight)*/; y++)
				{
					/* all columns	*/
					for (x = px; (x < pd->width) /*&& (x < px+ph->segmentWidth)*/; x++)
					{					
						*GetXYValue(pd, chan, x, y) = picbuf[bufpos++];									
					}
				}

				chan++; /* next colour component in next scan */
				if (chan == ph->channel) stop = 1; /* all components are processed after this scan*/
				cidx++; /* next index for RLC1 marker */
			}

			fprintf(stderr, ".");		

		}		/* while !stop} */
	}/* px */
	//} /* py */
	FREE( picbuf);

}




