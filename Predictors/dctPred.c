/*****************************************************
* File..: dctPred.c
* Desc..: DCT-based prediction
* Author: Tilo Strutz
* Date..: 08.08.2017
* ist Nonsense, weil bei der Extraopalation der DCT funktion immer der
* Randwert dupliziert wird, also A oder B genommen wir mit ggf. Wichtung
******************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "med.h" /* for max() and (min)*/
#include "ginterface.h"
#include "codec_CoBaLP.h" /* for CALLOC()*/
using namespace System::Runtime::InteropServices;

/* cos( (2n+1)*k*pi / (2*N))   , N=4	*/
#define COS92 946 /*cos( 1*1*pi/8) = 0.923879532 * 1024 */
#define COS71 724 /*cos( 2*1*pi/8) = 0.707106781 * 1024 */
#define COS38 392 /*cos( 3*1*pi/8) = 0.382683432 * 1024 */
#define COS92sq 874 /*COS92*COS92 = 0.853553391 * 1024 */
#define COS71sq 512 /*COS71*COS71 = 0.500000000 * 1024 */
#define COS38sq 150 /*COS38*COS38 = 0.146446609 * 1024 */
#define M_PI 3.141592654

/*
 Jacobi matrices
 k= 0:  J = (1 1 1 1)', J'*J = 4, inv(J'*J) = 1/4, 
 k= 1:  J = (COS92  COS38 -COS38 -COS92)', J'*J = 2, inv(J'*J) = 1/2, 
 k= 2:  J = (COS71 -COS71 -COS71  COS71)', J'*J = 2, inv(J'*J) = 1/2, 
 k= 3:  J = (COS38 -COS92  COS92 -COS38)', J'*J = 2, inv(J'*J) = 1/2, 
*/
/*---------------------------------------------------------------
*	medPredictor()
*---------------------------------------------------------------*/
MYINLINE void medPredictor_NoTUsed( PicData *pd, int chan, 
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
* dctPrediction() 
*-------------------------------------------------------------*/
void dctPrediction_NotUsed( PicData *pd, int encode_flag)
{
	uint x, y, chan;
	long 	epsilon, xHat, xHatHor, xHatVer;
	long range, offset;
	long 	Rx;
	unsigned long Mepsilon = 0;
	unsigned long pos, py, wHor, wVer;
	long *hError = NULL;	
	long *vError = NULL;	
	PicData *result = NULL;	

	/* allocate arrays for horizontal and vertical prediction erros */
	CALLOC(hError, pd->size, long);
	CALLOC(vError, pd->size, long);

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
//					medPredictor( pd, chan, &xHat, x, y);	
					xHatVer = xHatHor = xHat;
				}
				else 
				{/* do the least squares using 4-point DCT	*/
					long a[4]; /* DCT coefficients	*/
					long s[4];

					/* horizontal prediction	*/
					s[0] = pd->data[chan][pos-4];
					s[1] = pd->data[chan][pos-3];
					s[2] = pd->data[chan][pos-2];
					s[3] = pd->data[chan][pos-1];

					//s[0] = 100 + 7 * cos((2*0+1)*1*M_PI/8) + 15 * cos((2*0+1)*2*M_PI/8) + 30 * cos((2*0+1)*3*M_PI/8);
					//s[1] = 100 + 7 * cos((2*1+1)*1*M_PI/8) + 15 * cos((2*1+1)*2*M_PI/8) + 30 * cos((2*1+1)*3*M_PI/8);
					//s[2] = 100 + 7 * cos((2*2+1)*1*M_PI/8) + 15 * cos((2*2+1)*2*M_PI/8) + 30 * cos((2*2+1)*3*M_PI/8);
					//s[3] = 100 + 7 * cos((2*3+1)*1*M_PI/8) + 15 * cos((2*3+1)*2*M_PI/8) + 30 * cos((2*3+1)*3*M_PI/8);
					//long 
					//	 t = 100 + 7 * cos((2*4+1)*1*M_PI/8) + 15 * cos((2*4+1)*2*M_PI/8) + 30 * cos((2*4+1)*3*M_PI/8);
					a[0] = (s[0] + s[1] + s[2] + s[3] + 2) >> 2; /* mean value	*/
					a[1] = (COS92 * (s[0] - s[3]) + COS38 * (s[1] - s[2])) >> 6;	/* 6 of 11 shifts /2/1024	*/
					a[2] = (COS71 * (s[0] - s[1]  -          s[2] + s[3])) >> 6;						/* /2/1024	*/
					a[3] = (COS92 * (s[2] - s[1]) + COS38 * (s[0] - s[3])) >> 6;	/* /2/1024	*/

					/* division by 1024 for coefficients and the remaing 5 shifts from a[]	*/
					xHatHor = a[0] + ((-COS92 * a[1] + COS71 * a[2] - COS38 * a[3] + 1024) >> 15);
					xHatHor = s[3];

					/* vertial prediction	*/
					s[0] = pd->data[chan][pos-4*pd->width];
					s[1] = pd->data[chan][pos-3*pd->width];
					s[2] = pd->data[chan][pos-2*pd->width];
					s[3] = pd->data[chan][pos-pd->width];
					a[0] = (s[0] + s[1] + s[2] + s[3] + 2) >> 2; /* mean value	*/
					a[1] = (COS92 * (s[0] - s[3]) + COS38 * (s[1] - s[2])) >> 6;	/* 6 of 11 shifts /2/1024	*/
					a[2] = (COS71 * (s[0] - s[1]  -          s[2] + s[3])) >> 6;						/* /2/1024	*/
					a[3] = (COS92 * (s[2] - s[1]) + COS38 * (s[0] - s[3])) >> 6;	/* /2/1024	*/

					/* division by 1024 for coefficients and the remaing 5 shifts from a[]	*/
					xHatVer = a[0] + ((-COS92 * a[1] + COS71 * a[2] - COS38 * a[3] + 1024) >> 15);
					xHatVer = s[3];

					/* weighting of predictions*/
					wHor = abs( vError[pos-1]) + abs( vError[pos-pd->width]);
					wVer = abs( hError[pos-1]) + abs( hError[pos-pd->width]);
					//wHor = abs( vError[pos-1]);
					//wVer = abs( hError[pos-pd->width]);
					if (wHor == 0 && wVer == 0) 
						xHat = (xHatHor + xHatVer) >> 1;
					else
						xHat = (wHor * xHatHor + wVer * xHatVer) / (wHor + wVer);
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
				/* store prediction errors	*/
				hError[pos] = Rx - xHatHor;		    			    	
				vError[pos] = Rx - xHatVer;		    			    	

			}	/* x */
		}	/* y */	
		/* copy error erray in original component */
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
	}		
if (0)
{
	FILE * h_file = NULL;
	wchar_t *outFileName;
		for (y = 0, py = 0; y < pd->height; y++, py += pd->width)
		{
			for (x = 0, pos = py; x < pd->width; x++, pos++)
			{
				uint val;
			}
		}
	outFileName = (wchar_t*)(void*)Marshal::StringToHGlobalUni( "dct_errHor.pgm");
	/* open file for writing	*/
			h_file = _wfopen( outFileName, L"wb");
			if (h_file == NULL)
			{
				throw 14;
			}

			/* PPM-Writer benutzen */
//			WritePPM( hError, h_file);
			fclose( h_file);
	outFileName = (wchar_t*)(void*)Marshal::StringToHGlobalUni( "dct_errVer.pgm");
	/* open file for writing	*/
			h_file = _wfopen( outFileName, L"wb");
			if (h_file == NULL)
			{
				throw 14;
			}

			/* PPM-Writer benutzen */
//			WritePPM( vError, h_file);
			fclose( h_file);
}
			/* Zeichenfläche löschen */
	DeletePicData(result);	
	FREE(hError);	
	FREE(vError);	
}
