/*****************************************************
* File..: med.c
* Desc..: Implementation of median edge detection prediction
* Author: Tilo Strutz
* Date..: 24.01.2011
* 26.03.2014 do not use pd->maxVal[] in med_predictor()
* 31.03.2014 use long type
* 28.10.2015 bugfix memory leak: free(q9) for each component
******************************************************/
#include <stdlib.h>
#include <string.h>
#include "med.h"
#include "ginterface.h"

char *q9;
long medN[NUM_OF_CONTEXTS+2], medC[NUM_OF_CONTEXTS+2];
long medA[NUM_OF_CONTEXTS+2], medB[NUM_OF_CONTEXTS+2];
/*
CBD
AX
*/
#ifdef Q_DEBUG
extern FILE *debug_out;
#endif

/*---------------------------------------------------------------
*	med_predictor()
*---------------------------------------------------------------*/
MYINLINE void med_predictor( PicData *pd, int chan, CON *pc, 
														PRED_STRUC *pp, int x, int y)
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

	/* Compute the context number */
	d1 = Rd - Rb;
	d2 = Rb - Rc;	/* muss immer berechnet werden, da nach RLC
								* keine gultiger Wert fuer d2 = d1	*/
	d3 = Rc - Ra;
	pp->state = 9 * (9 * q9[d1 + pc->MAXVAL]  + 
		q9[d2 + pc->MAXVAL]) + q9[d3 + pc->MAXVAL];

	/* Convert to sign/magnitude. */
	pp->sgn = pp->state < 0 ? -1 : (pp->state > 0 ? 1 : 0);
	pp->state = (short)abs( pp->state);

#ifdef MEDRLC_MODE
	/* Test auf RLC-Modus	*/
	if ( (abs( d1) <= pc->NEAR) && (abs( d2) <= pc->NEAR) &&
		(abs( d3) <= pc->NEAR) && op->rlc_flag) 
	{ 
		pp->rlc_flag = 1;
	}
	else /* Context mode. */
#endif
	{
		pp->rlc_flag = 0;

		/* Predict the next value. */
		maxAC = max( Ra, Rb); 
		minAC = min( Ra, Rb);

		if (Rc >= (long)maxAC)	pp->xHat = minAC;
		else if (Rc <= (long)minAC)	pp->xHat = maxAC;
		else			pp->xHat = Ra + Rb - Rc;
	}
}


/*--------------------------------------------------------------
* MED-Prädiktor auf PicData anwenden
*-------------------------------------------------------------*/
void MEDprediction( PicData *pd, int encode_flag)
{
	uint x, y, chan;
	short cx;
	long range, offset;
	long 	Rx;
	long 	epsilon;
	unsigned long Mepsilon = 0;
	PicData *result = NULL;	
	CON	con, *pc;
	OPTION	option, *op;
	IMAGE_LOCO		image, *im;
	PRED_STRUC pred_struc, *pp;



	pc = &con;	/*Pointer auf Struktur aller Konstanten	*/
	pp = &pred_struc;
	op = &option;
	im = &image;

	op->bpp_flag = 0;
	op->esc_flag = 1;
	op->rlc_flag = 1;
	op->nearlossless = 0;
	op->T1 = S1;
	op->T2 = S2;
	op->T3 = S3;
	op->T_flag = 1; /* use fixed thresholds */
	op->reset_flag = 1;
	op->reset = 64;
	op->reset_flag = 0; /* set reset adaptive to image size	*/

	im->width = pd->width;
	im->height = pd->height;
	im->size = im->width * im->height;
	im->bit_depth	= pd->bitperchannel;

	/* Größten Farbwert bestimmen und leere Zeichenfläche anlegen */
	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);

	/* Alle Kanäle zeilenweise links oben beginnend durchgehen */
	for (chan = 0; chan < pd->channel; chan++)
	{
		range = (1 << pd->bitperchan[chan]);
		offset = range >> 1;

		//maxval = pd->maxVal[chan];
		/* Initialize the model */
		im->max_value = range - 1;
		init_constants( im, op, pc);
		init_contexts( op, pc);
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

		for (y = 0; y < pd->height; y++)
		{
			for (x = 0; x < pd->width; x++)
			{				
				/* computes estimation value pp->xHat	*/
				med_predictor( pd, chan, pc, pp, x, y);	

				cx = pp->state;

				/* correct xHat, bound xHat, compensate context sign	*/
				if (cx > 0)
				{
					if (pp->sgn > 0)	pp->xHat += medC[cx];
					else			pp->xHat -= medC[cx];
				}

				/**/
				pp->xHat = CLIP( pp->xHat, 0, pc->MAXVAL);

				if (encode_flag)
				{
					Rx = (long)*GetXYValue( pd, chan, x, y);

					if (pp->sgn < 0) 	epsilon = pp->xHat - Rx;
					else 			epsilon = Rx - pp->xHat;

					/* Quantisierung des Praediktionsfehlers	*/
					if (epsilon > 0)
						epsilon =  (pc->NEAR + epsilon) / pc->NEARm2p1;
					else
						epsilon = -(pc->NEAR - epsilon) / pc->NEARm2p1;

					/* Rekonstruktion des aktuellen Bildpunktes	*/
					if (pp->sgn < 0) Rx = pp->xHat - epsilon * pc->NEARm2p1;
					else 		 Rx = pp->xHat + epsilon * pc->NEARm2p1;

					/* Bound Rx. */		
					Rx = CLIP( Rx, 0, pc->MAXVAL);

					/* Praediktionsfehler modulo	*/
					epsilon = MODULO_RANGE( epsilon, -pc->RANGEd2,
						pc->RANGEm1d2, pc->RANGE)

#ifdef Q_DEBUG
						if (debug_out != NULL)
						{
							fprintf( debug_out, "(%3d,%3d) x=%3d xHat=%3d ",	
								y, x, Rx, pp->xHat);
							fprintf( debug_out, "e=%3d cx=%3d A=%3d B=%3d C=%3d N=%3d\n",
								epsilon, cx, A[cx], B[cx], C[cx], N[cx]);
						}
#endif

						/* Remap epsilon, different for NEAR==0 ! *
						if ( (k == 0) && (pc->NEAR == 0) && 
						(2 * B[cx] <= -N[cx]) )
						{
						if (epsilon < 0) 	Mepsilon = -2 * (epsilon+1);
						else 		Mepsilon =  2 *  epsilon+1;
						}
						else
						{
						if (epsilon < 0) 	Mepsilon = -2 * epsilon-1;
						else 		Mepsilon =  2 * epsilon;
						}
						*/

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
					if (pp->sgn < 0) 	Rx = pp->xHat - epsilon;
					else 			Rx = pp->xHat + epsilon;

					Rx = MODULO_RANGE( Rx, -pc->NEAR,
						pc->MAXVAL + pc->NEAR, pc->RANGE_R)

						/* bound Rx */
						Rx = CLIP( Rx, 0, pc->MAXVAL);

					/* write reconstructed value */
					*GetXYValue( pd, chan, x, y) = (uint)Rx;		    			    	
				}

				/* update the context */
				medA[cx] += abs( epsilon);
				medB[cx] += epsilon * pc->NEARm2p1;

				/* Rescale N, A and B if the counts get too large */
				if (medN[cx] == pc->RESET)
				{ 
					medA[cx] >>= 1;
					medN[cx] >>= 1;
					if (medB[cx] >= 0) medB[cx] >>= 1;
					else 	medB[cx] = -((1 - medB[cx]) >> 1);
				}

				/* Increment occurence count */
				medN[cx]++;

				/* Compute the bias correction term */
				if (medB[cx] <= -medN[cx])
				{
					if (medC[cx] > pc->MIN_C) medC[cx]--;
					medB[cx] += medN[cx];
					if (medB[cx] <= -medN[cx]) medB[cx] = -medN[cx] + 1;
				} 
				else if (medB[cx] > 0)
				{
					if (medC[cx] < pc->MAX_C) medC[cx]++;
					medB[cx] -= medN[cx];
					if (medB[cx] > 0) medB[cx] = 0;
				} 
			}	/* x */
		}	/* y */	
		/* copy error erray in original component */
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
		free_q( ); /* for all components	*/
	}		

#ifdef DEBUG_MED
	WritePPM( pd, "pred_err.ppm");
#endif
	/* Zeichenfläche löschen */
	DeletePicData(result);	
}


/*---------------------------------------------------------------
*	init_constants()
*---------------------------------------------------------------*/
void init_constants( IMAGE_LOCO *im, OPTION *op, CON *pc)
{
	int bpp;

	pc->MAXVAL = im->max_value;
	pc->NEAR   = op->nearlossless;
	pc->NEARm2p1 = pc->NEAR * 2 + 1;
	pc->RANGE   = (pc->MAXVAL + 2 * pc->NEAR) / (pc->NEARm2p1) + 1;
	pc->RANGE_R   =  pc->RANGE * pc->NEARm2p1;
	pc->RANGEd2   =  pc->RANGE / 2;
	/*    pc->RANGEp1d2 = (pc->RANGE + 1) / 2; */
	pc->RANGEm1d2 = (pc->RANGE - 1) / 2;

	/* Bittiefe fuer RANGE	*/
	pc->qbpp = 0;
	bpp = pc->RANGE - 1;
	while (bpp >> pc->qbpp) { pc->qbpp++;}
	if (!pc->qbpp) pc->qbpp = 1;

	bpp = max( 2, im->bit_depth);
	if (bpp <  8)    	pc->LIMIT  = 2 * (bpp + 8);
	else	    	pc->LIMIT  = 4 * bpp;
	pc->LIMITq = pc->LIMIT - pc->qbpp - 1;

	pc->MAX_C =  127;
	pc->MIN_C = -128;

	/* Begrenzung des Adaptationsgedaechtnisses abhaengig von 
	* der Groesze des Bildes
	*/
	if (op->reset_flag == 0)
		pc->RESET = (im->width + im->height + 16) / 32 + 32;
	else pc->RESET = op->reset;
}

/*---------------------------------------------------------------
*	init_contexts()
*---------------------------------------------------------------*/
void init_contexts( OPTION *op, CON *pc)
{
	int i, t1, t2, t3, fac, setA;

	/* Initialize the contexts. All uninitialized values are 0. */
	setA = max( 2, (pc->RANGE + 32) / 64);
	for ( i = 0; i < NUM_OF_CONTEXTS+2; i++)
	{
		medA[i] = setA;
		medN[i] = 1;
		medB[i] = medC[i] = 0;
	}

	/* Adaptive Initialisierung der Kontextschwellen	*/
	if (!op->T_flag)
	{
		if (pc->MAXVAL > 4095)   	fac = (4096 + 128) / 256;
		else			fac = (pc->MAXVAL + 128) / 256;

		if (fac)
		{
			t1 = fac * (S1 - 2) + 2;
			t2 = fac * (S2 - 3) + 3;
			t3 = fac * (S3 - 4) + 4;
		}
		else
		{
			/* pc->MAXVAL < 128	*/
			t1 = S1 / (256 / pc->MAXVAL);	if (t1 < 2) t1 = 2;
			t2 = S2 / (256 / pc->MAXVAL);	if (t2 < 3) t2 = 3;
			t3 = S3 / (256 / pc->MAXVAL);	if (t3 < 4) t3 = 4;
		}
		t1 += pc->NEAR * 3;
		t2 += pc->NEAR * 5;
		t3 += pc->NEAR * 7;
		op->T1 = t1;
		op->T2 = t2;
		op->T3 = t3;
	}
	else
	{
		t1 = op->T1;
		t2 = op->T2;
		t3 = op->T3;
	}

	//    CALLOC( q9, pc->MAXVAL * 2 + 1, 1, char);
	ALLOC( q9, pc->MAXVAL * 2 + 1, char);

	if (pc->NEAR)
	{
		for ( i = -pc->MAXVAL; i <= pc->MAXVAL; i++)
		{
			if (i <= -t3)      		q9[i+pc->MAXVAL] = -4;
			else if (i <= -t2) 		q9[i+pc->MAXVAL] = -3;
			else if (i <= -t1) 		q9[i+pc->MAXVAL] = -2;
			else if (i <= -pc->NEAR-1)	q9[i+pc->MAXVAL] = -1;
			else if (i <=  pc->NEAR)	q9[i+pc->MAXVAL] = 0;
			else if (i <  t1)		q9[i+pc->MAXVAL] = 1;
			else if (i <  t2)		q9[i+pc->MAXVAL] = 2;
			else if (i <  t3)		q9[i+pc->MAXVAL] = 3;
			else			q9[i+pc->MAXVAL] = 4;
		}
	}
	else
	{
		for ( i = -pc->MAXVAL; i <= pc->MAXVAL; i++)
		{
			if (i <= -t3)	q9[i+pc->MAXVAL] = -4;
			else if (i <= -t2)	q9[i+pc->MAXVAL] = -3;
			else if (i <= -t1)	q9[i+pc->MAXVAL] = -2;
			else if (i <= -1)	q9[i+pc->MAXVAL] = -1;
			else if (i ==  0)	q9[i+pc->MAXVAL] = 0;
			else if (i <  t1)	q9[i+pc->MAXVAL] = 1;
			else if (i <  t2)	q9[i+pc->MAXVAL] = 2;
			else if (i <  t3)	q9[i+pc->MAXVAL] = 3;
			else		q9[i+pc->MAXVAL] = 4;
		}
	}
	// it must be checked, whether this entire fucntion must be called for
	// MED predictor (w/o LOCO coding)
	//fprintf( stderr,"\n\t T1=%d, T2=%d, T3=%d", t1, t2, t3);    
	//fprintf( stderr,"\t LIMIT=%d", pc->LIMITq);    
	//fprintf( stderr,"\t RESET=%d", pc->RESET);    
}

/*---------------------------------------------------------------
*	free_q()
*---------------------------------------------------------------*/
void free_q( void)
{
	FREE( q9);
}
