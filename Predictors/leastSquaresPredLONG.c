/*****************************************************
* File..: leastSquaresPred.c
* Desc..: LS predictionbased on weighted comb. of simple predictors
* Author: Tilo Strutz
* Date..: 08.08.2017
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "med.h" /* for max() and (min)*/
#include "ginterface.h"
#include "codec_CoBaLP.h" /* for CALLOC()*/
using namespace System::Runtime::InteropServices;

/*---------------------------------------------------------------
*	medPredictor_LONG()
*---------------------------------------------------------------*/
MYINLINE void medPredictor( PicData *pd, int chan, 
														long *xHat, int x, int y)
{
	unsigned long maxAC, minAC;
	static long d1, d2, d3;

	long Ra, Rb, Rc, Rd;

	/*  C B D
	*  
	*/

	if (y == 0)
	{
		Rb = 1 << (pd->bitperchan[chan]-1);
		Rc = 1 << (pd->bitperchan[chan]-1);
		Rd = 1 << (pd->bitperchan[chan]-1);
		if (x == 0) Ra = 1 << (pd->bitperchan[chan]-1);
		else Ra = (int)*GetXYValue( pd, chan, x-1u, y);
		/* does not work since maxVal might be different in encoder and decoder
		Rb = (pd->maxVal[chan] + 1) >> 1;
		Rc = (pd->maxVal[chan] + 1) >> 1;
		Rd = (pd->maxVal[chan] + 1) >> 1;
		if (x == 0) Ra = (pd->maxVal[chan] + 1) >> 1;
		else Ra = (int)*GetXYValue( pd, chan, x-1u, y);
		*/
	}
	else 
	{
		Rb = (long)*GetXYValue( pd, chan, x, y-1u);
		if (x == 0)
		{
			Ra = Rb;
			Rc = Rb;
			Rd = (long)*GetXYValue( pd, chan, x+1u, y-1u);
		}
		else
		{
			Ra = (long)*GetXYValue( pd, chan, x-1u, y);
			Rc = (long)*GetXYValue( pd, chan, x-1u, y-1u);
			if (x == (signed)pd->width - 1) Rd = Rb;
			Rd = (long)*GetXYValue( pd, chan, x+1u, y-1u);
		}
	}


		/* Predict the next value. */
		maxAC = max( Ra, Rb); 
		minAC = min( Ra, Rb);

		if (Rc >= (long)maxAC)	*xHat = minAC;
		else if (Rc <= (long)minAC)	*xHat = maxAC;
		else			*xHat = Ra + Rb - Rc;
}

/*--------------------------------------------------------------
* lsPrediction_LONG() 
* for some reason, E3 and V2 have a reverse effect on barbara and ashton.. 
*-------------------------------------------------------------*/
void dctPrediction_LONG( PicData *pd, int encode_flag)
{
	uint x, y, chan;
	long 	epsilon, xHat;
	long range, offset;
	long 	Rx, n, m;
	long facH, facV, facD, facE;
	long facMin = 731/* 1024 / 1.4*/, facMax = 1434 /* 1.4 * 1024*/;
	long facD1, facD2, facD3, facE1, facE2, facE3;
	long facH2, facH3, facH4, facV2, facV3;
	long facD4, facE4;
	long nMax, mMax, mMin;
	unsigned int posWeigth;
	unsigned long sumSquares;
	unsigned long sumProductsH;
	unsigned long sumProductsH2;
	unsigned long sumProductsH3;
	unsigned long sumProductsH4;
	unsigned long sumProductsV;
	unsigned long sumProductsV2;
	unsigned long sumProductsV3;
	unsigned long sumProductsD;
	unsigned long sumProductsD1;
	unsigned long sumProductsD2;
	unsigned long sumProductsD3;
	unsigned long long varH, varV, varD, varE;
	unsigned long long varD1, varD2, varD3, varE1, varE2, varE3;
	unsigned long long varH2, varH3, varH4, varV2, varV3;
	unsigned long long varD4, varE4;
	unsigned long Mepsilon = 0;
	unsigned long pos, py;
	PicData *result = NULL;	

	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);

	/* Alle Kanäle zeilenweise links oben beginnend durchgehen */
	for (chan = 0; chan < pd->channel; chan++)
	{
		long RANGE = (pd->maxVal_rct[chan] + 1) ;
		long RANGEd2 = (pd->maxVal_rct[chan] + 1) >> 1;
		long RANGEm1d2 = (pd->maxVal_rct[chan]) >> 1;
		range = (1 << pd->bitperchan[chan]);
		offset = range >> 1;

		if (!encode_flag)
		{
			/* copy  original component */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			pd->offset[chan] = 0; /* overwrite old offset as it becomes obsolete here	*/
		}
		else
		{
			pd->offset[chan] = offset; /* overwrite old offset as it becomes obsolete here	*/
		}

		for (y = 0, py = 0; y < pd->height; y++, py += pd->width)
		{
			for (x = 0, pos = py; x < pd->width; x++, pos++)
			{				
				if (x < 4 || y < 4)
				{
					/* use MED for first lines and rows	*/
					medPredictor_LONG( pd, chan, &xHat, x, y);	
				}
				else 
				{/* do the least squares using 4-point DCT	*/
if ( y== 655 && x== 84)
{
	int z = 9;
}
					/* horizontal and diagonal prediction	
					**
					**  32109
					**  87554
					**  321X
					**
					*/
					sumSquares = 0;
					sumProductsH = 1; /* avoids division by zero	*/
					sumProductsD = 1;
					sumProductsD1 = 1;
					sumProductsD2 = 1;
					sumProductsD3 = 1;
					sumProductsH2 = 1;
					sumProductsH3 = 1;
					sumProductsH4 = 1;
					nMax = 3;
					mMax = 2;
					mMin = -2;
					for (n = 0; n <= nMax; n++)
					{
						for (m = mMin; m <= mMax; m++)
						{
							if (m >= 0 && n == 0) continue;
							if (x + m < 4) continue;
							if (x + m > pd->width - 1) continue;
							if (y - n < 2) continue;
							long target = pd->data[chan][pos + m - n * pd->width];
							long sourceH = pd->data[chan][pos + m - n * pd->width - 1];
							long sourceD = pd->data[chan][pos + m - (n+1) * pd->width - 1];
							long sourceD1 = pd->data[chan][pos + m - (n+1) * pd->width - 2];
							long sourceD2 = pd->data[chan][pos + m - (n+2) * pd->width - 1];
							long sourceD3 = pd->data[chan][pos + m - (n+2) * pd->width - 2];
							long sourceH2 = pd->data[chan][pos + m - n * pd->width - 2];
							long sourceH3 = pd->data[chan][pos + m - n * pd->width - 3];
							long sourceH4 = pd->data[chan][pos + m - n * pd->width - 4];
							posWeigth = 1+(nMax-n) + (mMax-abs(m));
							sumSquares += target * target * posWeigth;
							sumProductsH += target * sourceH * posWeigth;
							sumProductsD += target * sourceD * posWeigth;
							sumProductsD1 += target * sourceD1 * posWeigth;
							sumProductsD2 += target * sourceD2 * posWeigth;
							sumProductsD3 += target * sourceD3 * posWeigth;
							sumProductsH2 += target * sourceH2 * posWeigth;
							sumProductsH3 += target * sourceH3 * posWeigth;
							sumProductsH4 += target * sourceH4 * posWeigth;
						}
					}
					facH = (sumSquares << 10) / sumProductsH;
					facD = (sumSquares << 10) / sumProductsD;
					facD1 = (sumSquares << 10) / sumProductsD1;
					facD2 = (sumSquares << 10) / sumProductsD2;
					facD3 = (sumSquares << 10) / sumProductsD3;
					facH2 = (sumSquares << 10) / sumProductsH2;
					facH3 = (sumSquares << 10) / sumProductsH3;
					facH4 = (sumSquares << 10) / sumProductsH4;

					/* 1.2 * 1024 = 1229 *1; 1.5 * 1024 = 1536	*/
					facH = CLIP( facH, facMin, facMax);
					facD = CLIP( facD, facMin, facMax);
					facD1 = CLIP( facD1, facMin, facMax);
					facD2 = CLIP( facD2, facMin, facMax);
					facD3 = CLIP( facD3, facMin, facMax);
					facH2 = CLIP( facH2, facMin, facMax);
					facH3 = CLIP( facH3, facMin, facMax);
					facH4 = CLIP( facH4, facMin, facMax);

					/* get sample variance	*/
					varH = varD = varD1 = varD2 = varD3 = varH2 = varH3 = varH4 = 1; /* avoids division by zero	*/
					for (n = 0; n <= nMax; n++)
					{
						for (m = mMin; m <= mMax; m++)
						{
							if (m >=0 && n== 0) continue;
							if (x + m < 4) continue;
							if (x + m > pd->width - 1) continue;
							if (y - n < 2) continue;
							long target = pd->data[chan][pos + m - n * pd->width];
							long sourceH = pd->data[chan][pos + m - n * pd->width - 1];
							long sourceD = pd->data[chan][pos + m - (n+1) * pd->width - 1];
							long sourceD1 = pd->data[chan][pos + m - (n+1) * pd->width - 2];
							long sourceD2 = pd->data[chan][pos + m - (n+2) * pd->width - 1];
							long sourceD3 = pd->data[chan][pos + m - (n+2) * pd->width - 2];
							long sourceH2 = pd->data[chan][pos + m - n * pd->width - 2];
							long sourceH3 = pd->data[chan][pos + m - n * pd->width - 3];
							long sourceH4 = pd->data[chan][pos + m - n * pd->width - 4];
							long long diff = (target << 10) - (sourceH * facH);
							posWeigth = 1+(nMax-n) + (mMax-abs(m));
							//posWeigth = 1;
							varH += diff * diff * posWeigth;
							//if (m == 0 && n == 1) varH += diff * diff; /* doubling of influence of point above*/
							diff = (target << 10) - (sourceD * facD);
							varD += diff * diff * posWeigth;
							//if (m == 1 && n == 0) varD += diff * diff; /* doubling of influence of left point */
							diff = (target << 10) - (sourceD1 * facD1);
							varD1 += diff * diff * posWeigth;
							diff = (target << 10) - (sourceD2 * facD2);
							varD2 += diff * diff * posWeigth;
							diff = (target << 10) - (sourceD3 * facD3);
							varD3 += diff * diff * posWeigth;
							diff = (target << 10) - (sourceH2 * facH2);
							varH2 += diff * diff * posWeigth;
							diff = (target << 10) - (sourceH3 * facH3);
							varH3 += diff * diff * posWeigth;
							diff = (target << 10) - (sourceH4 * facH4);
							varH4 += diff * diff * posWeigth;
						}
					}

					/* vertical and diagonal prediction	
					**
					** 9012
					** 5678
					** 1234
					** 0X
					*/
					sumSquares = 0;
					sumProductsV = 1; /* avoids division by zero	*/
					sumProductsD = 1;
					sumProductsD1 = 1;
					sumProductsD2 = 1;
					sumProductsD3 = 1;
					sumProductsV2 = 1;
					nMax = 2;
					mMax = 2;
					mMin = -2;
					for (n = 0; n <= nMax; n++)
					{
						for (m = mMin; m <= mMax; m++)
						{
							if (m >= 0 && n == 0) continue;
							if (x + m < 0) continue;
							if (x + m > pd->width - 3) continue;
							if (y - n < 2) continue;
							long target = pd->data[chan][pos + m - n * pd->width];
							long sourceV = pd->data[chan][pos + m - (n+1) * pd->width];
							long sourceE = pd->data[chan][pos + m - (n+1) * pd->width + 1];
							long sourceE1 = pd->data[chan][pos + m - (n+1) * pd->width + 2];
							long sourceE2 = pd->data[chan][pos + m - (n+2) * pd->width + 1];
							long sourceE3 = pd->data[chan][pos + m - (n+2) * pd->width + 2];
							long sourceV2 = pd->data[chan][pos + m - (n+2) * pd->width];
							posWeigth = 1+(nMax-n) + (mMax-abs(m));
							sumSquares += target * target * posWeigth;
							sumProductsV += target * sourceV * posWeigth;
							sumProductsD += target * sourceE * posWeigth;
							sumProductsD1 += target * sourceE1 * posWeigth;
							sumProductsD2 += target * sourceE2 * posWeigth;
							sumProductsD3 += target * sourceE3 * posWeigth;
							sumProductsV2 += target * sourceV2 * posWeigth;
						}
					}
					facV = (sumSquares << 10) / sumProductsV;
					facE = (sumSquares << 10) / sumProductsD;
					facE1 = (sumSquares << 10) / sumProductsD1;
					facE2 = (sumSquares << 10) / sumProductsD2;
					facE3 = (sumSquares << 10) / sumProductsD3;
					facV2 = (sumSquares << 10) / sumProductsV2;
					/* limitation of factors, should be dependent on variance somehow	*/
					facV = CLIP( facV, facMin, facMax);
					facE = CLIP( facE, facMin, facMax);
					facE1 = CLIP( facE1, facMin, facMax);
					facE2 = CLIP( facE2, facMin, facMax);
					facE3 = CLIP( facE3, facMin, facMax);
					facV2 = CLIP( facV2, facMin, facMax);

					/* get sample variance	*/
					varV = varE = varE1 = varE2 = varE3 = varV2 = 1; /* avoids division by zero	*/
					for (n = 0; n <= nMax; n++)
					{
						for (m = mMin; m <= mMax; m++)
						{
							if (m >= 0 && n == 0) continue;
							if (x + m < 0) continue;
							if (x + m > pd->width - 3) continue;
							if (y - n < 2) continue;
							long target = pd->data[chan][pos + m - n * pd->width];
							long sourceV = pd->data[chan][pos + m - (n+1) * pd->width];
							long sourceE = pd->data[chan][pos + m - (n+1) * pd->width + 1];
							long sourceE1 = pd->data[chan][pos + m - (n+1) * pd->width + 2];
							long sourceE2 = pd->data[chan][pos + m - (n+2) * pd->width + 1];
							long sourceE3 = pd->data[chan][pos + m - (n+2) * pd->width + 2];
							long sourceV2 = pd->data[chan][pos + m - (n+2) * pd->width];
							long long diff = (target << 10) - (sourceV * facV);
							posWeigth = 1 + (nMax-n) + (mMax-abs(m));
							//posWeigth = 1;
							varV += diff * diff * (1+(nMax-n)+ (mMax-abs(m)));
							diff = (target << 10) - (sourceE * facE);
							varE += diff * diff * posWeigth; 
							diff = (target << 10) - (sourceE1 * facE1);
							varE1 += diff * diff * posWeigth; 
							diff = (target << 10) - (sourceE2 * facE2);
							varE2 += diff * diff * posWeigth; 
							diff = (target << 10) - (sourceE3 * facE3);
							varE3 += diff * diff * posWeigth; 
							diff = (target << 10) - (sourceV2 * facV2);
							varV2 += diff * diff * posWeigth; 
							//if (m == -1 && n == 0) varE += diff * diff; /* doubling of influence of left point*/
						}
					}

					/* weighting of predictions
					**
					**  a/wa + b/wb +c/wc +d/wc
					**  -----------------------
					**  1/wa +1/wb + 1/wc +1/wd
					*/
					{
							long sourceH = pd->data[chan][pos - 1];
							long sourceV = pd->data[chan][pos     - pd->width];
							long sourceD = pd->data[chan][pos - 1 - pd->width];
							long sourceD1 = pd->data[chan][pos - 2 -   pd->width];
							long sourceD2 = pd->data[chan][pos - 1 - 2*pd->width];
							long sourceD3 = pd->data[chan][pos - 2 - 2*pd->width];
							long sourceH2 = pd->data[chan][pos - 2];
							long sourceH3 = pd->data[chan][pos - 3];
							long sourceH4 = pd->data[chan][pos - 4];
							long sourceE = pd->data[chan][pos + 1 - pd->width];
							long sourceE1 = pd->data[chan][pos + 2 -   pd->width];
							long sourceE2 = pd->data[chan][pos + 1 - 2*pd->width];
							long sourceE3 = pd->data[chan][pos + 2 - 2*pd->width];
							long sourceV2 = pd->data[chan][pos     - 2*pd->width];
							long xHatH, xHatV, xHatD, xHatE;
							long xHatD1, xHatD2, xHatD3, xHatD4, xHatE1, xHatE2, xHatE3, xHatE4;
							long xHatH2, xHatH3, xHatH4, xHatV2, xHatV3;

							unsigned long long wH = ((unsigned long long)1 << 40) / varH;	
							unsigned long long wV = ((unsigned long long)1 << 40) / varV;
							unsigned long long wD = ((unsigned long long)1 << 40) / varD;
							unsigned long long wD1 = ((unsigned long long)1 << 40) / varD1;
							unsigned long long wD2 = ((unsigned long long)1 << 40) / varD2;
							unsigned long long wD3 = ((unsigned long long)1 << 40) / varD3;
							unsigned long long wH2 = ((unsigned long long)1 << 40) / varH2;
							unsigned long long wH3 = ((unsigned long long)1 << 40) / varH3;
							unsigned long long wH4 = ((unsigned long long)1 << 40) / varH4;
							unsigned long long wE = ((unsigned long long)1 << 40) / varE;
							unsigned long long wE1 = ((unsigned long long)1 << 40) / varE1;
							unsigned long long wE2 = ((unsigned long long)1 << 40) / varE2;
							unsigned long long wE3 = ((unsigned long long)1 << 40) / varE3;
							unsigned long long wV2 = ((unsigned long long)1 << 40) / varV2;
							unsigned long long wSum;
							
							wSum = wH + wV + wD + wD1 + wD2 + wD3 + wH2 + wE + wE1 + wE2 + wE3 + wV2;
							wSum = wH + wV + wD + wD1 + wD2 + wD3 + 
															 wE + wE1 + wE2 + wE3+  
								               wH2 + wH3 + wH4 + 
																					wV2/**/;

							if (wSum > ( (unsigned long long)1 << (63 - 9 - 10- 1)) )
							{
								int z=9;
							}
							assert( wSum <= ( (unsigned long long)1 << (63 - 9 - 10- 1)));
							if (wSum == 0)
							{
								wH = wV = wD = wD1 = wD2 = wD3 = wH2 = wH3 = wH4 =
									        wE = wE1 = wE2 = wE3 = wV2 = 1;
								wSum = 14;
								//wH = wV = wD = wD1 = wD2 = wD3 = wH2 = wH3 = 
								//	        wE = wE1 = wE2 = 1;
								//wE3 = wV2 = 0;
								//wSum = 11;
							}
							/* clip to actual range times 1024 */
							xHatH = CLIP( sourceH * facH, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatV = CLIP( sourceV * facV, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatD = CLIP( sourceD * facD, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatD1 = CLIP( sourceD1 * facD1, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatD2 = CLIP( sourceD2 * facD2, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatD3 = CLIP( sourceD3 * facD3, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatH2 = CLIP( sourceH2 * facH2, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatH3 = CLIP( sourceH3 * facH3, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatH4 = CLIP( sourceH4 * facH4, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatE = CLIP( sourceE * facE, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatE1 = CLIP( sourceE1 * facE1, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatE2 = CLIP( sourceE2 * facE2, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatE3 = CLIP( sourceE3 * facE3, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHatV2 = CLIP( sourceV2 * facV2, 0, ((long)pd->maxVal_rct[chan] << 10));
							xHat = (long)(
										 (xHatH * wH + xHatH2 * wH2 + xHatH3 * wH3 + xHatH4 * wH4 + 
											xHatV * wV + xHatV2 * wV2 + 
											xHatD * wD + xHatD1 * wD1 + xHatD2 * wD2 + xHatD3 * wD3 + 
											xHatE * wE + xHatE1 * wE1 + xHatE2 * wE2 + xHatE3 * wE3 +
																						/**/ (wSum>>1)) / wSum
											);
							xHat = (xHat + 512) >> 10;
					}
				}

				/* clip to actual range*/
				xHat = CLIP( xHat, 0, (int)pd->maxVal_rct[chan]);

				if (encode_flag)
				{
					Rx = (long)*GetXYValue( pd, chan, x, y);

					epsilon = Rx - xHat;

					/* Bound Rx. */		
					Rx = CLIP( Rx, 0, (int)pd->maxVal_rct[chan]);

					/* Praediktionsfehler modulo	*/
					epsilon = MODULO_RANGE( epsilon, -RANGEd2,
						RANGEm1d2, RANGE)

						/* ...und in den positiven Bereich verschieben */    	
						// orig: 
						// Mepsilon = epsilon + (range >> 1);	    	
						Mepsilon = epsilon + offset;	    	

						/* Schätzfehler einsetzen */		    	
						*GetXYValue( result, chan, x, y) = (uint)Mepsilon;		    			    	
				}
				else /* decoder */
				{
					/* get prediction-error value	*/
					Mepsilon = (unsigned long)*GetXYValue( result, chan, x, y);
					// epsilon = Mepsilon - (range >> 1);	    	
					epsilon = Mepsilon - offset;	    	
					/* reconstruction of pixel value Rx	*/
					Rx = xHat + epsilon;

					Rx = MODULO_RANGE( Rx, 0,	(int)pd->maxVal_rct[chan], RANGE)

						/* bound Rx */
						Rx = CLIP( Rx, 0, (int)pd->maxVal_rct[chan]);

					/* write reconstructed value */
					*GetXYValue( pd, chan, x, y) = (uint)Rx;		    			    	
				}

			}	/* x */
		}	/* y */	
		/* copy error erray in original component */
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
	}		

			/* Zeichenfläche löschen */
	DeletePicData(result);	
}
