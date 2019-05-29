/*****************************************************
* File..: med_paeth.c
* Desc..: combined MED-Paeth prediction
* Author: Tilo Strutz
* Date..: 31.01.2011
* 31.03.2014 unsigned long range;
* 28.10.2015 bugfix memory leak: free(q9) for each component
******************************************************/
#include <stdlib.h>
#include <string.h>
#include "med.h"
#include "ginterface.h"

extern char *q9;
/*
CBD
AX
*/

/* proto-typing */
/* from med.c */
MYINLINE void med_predictor( PicData *pd, int chan, CON *pc, 
							 PRED_STRUC *pp, int x, int y);
void init_constants( IMAGE_LOCO *im, OPTION *op, CON *pc);
void init_contexts( OPTION *op, CON *pc);

/* from paeth.c */
MYINLINE int GetPredVal(PicData *pd, uint chan, uint x, uint y);


/*--------------------------------------------------------------
 * MED-Paeth prediction
 *-------------------------------------------------------------*/
void MEDPAETHprediction( PicData *pd, int encode_flag)
{
	uint x, y, chan;
	short cx;
	long 	Rx;
	long 	range, epsilon;
	unsigned long Mepsilon = 0;
	PicData *result = NULL;	
	CON	con, *pc;
	OPTION	option, *op;
	IMAGE_LOCO		image, *im;
	PRED_STRUC pred_struc, *pp;
	unsigned char *histo=NULL;


	ALLOC( histo, (1<<pd->bitperchannel), unsigned char);	

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

	/* leere Zeichenfläche anlegen */
	result = CreatePicData( pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);

	im->width = pd->width;
	im->height = pd->height;
	im->size = im->width * im->height;

	/* Alle Kanäle zeilenweise links oben beginnend durchgehen */
	for (chan = 0; chan < pd->channel; chan++)
	{
		im->bit_depth	= pd->bitperchan[chan];
		range = (1 << pd->bitperchan[chan]);

		/* Initialize the model */
		im->max_value = range - 1;
		init_constants( im, op, pc);
		init_contexts( op, pc);

		/* reset flags for processed values */
		for ( x = 0; x < (1u<<pd->bitperchannel); x++) histo[x] = 0;

		if (!encode_flag)
		{
			/* copy  original component */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			pd->offset[chan] = 0; /* remove offset as it becomes obsolete here	*/
		}
		else
		{
			pd->offset[chan] = pc->RANGEd2; /* overwrite old offset as it becomes obsolete here	*/
		}
		
		for (y = 0; y < pd->height; y++)
		{
			for (x = 0; x < pd->width; x++)
			{				
					/* computes estimation value pp->xHat	*/
					med_predictor( pd, chan, pc, pp, x, y);	
		    	
					cx = pp->state;

					/* correct xHat, bound xHat, compensate context sign	*
					if (pp->sgn > 0)	pp->xHat += C[cx]; 
					else			pp->xHat -= C[cx]; 

/**/
					pp->xHat = CLIP( pp->xHat, 0, pc->MAXVAL);

					if (histo[pp->xHat] == 0)
					{
						/* this value does not belong to the set of image values yet
						* get PAETH prediction value instead
						*/
						pp->xHat = GetPredVal( pd, chan, x, y); /* defined in paeth.c	*/
					}

					if (encode_flag)
					{
						Rx = (int)*GetXYValue( pd, chan, x, y);

						histo[Rx] = 1;

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
						//Mepsilon = epsilon + ((maxval + 1) >> 1);	    	
						Mepsilon = epsilon + pc->RANGEd2;	    	
			    	
		    		/* Schätzfehler einsetzen */		    	
		    		*GetXYValue( result, chan, x, y) = (uint)Mepsilon;		    			    	
					}
					else /* decoder */
					{
						/* get prediction-error value	*/
						Mepsilon = (int)*GetXYValue( result, chan, x, y);
						// epsilon = Mepsilon - ((maxval + 1) >> 1);	    	
						epsilon = Mepsilon - pc->RANGEd2;	    	
						/* reconstruction of pixel value Rx	*/
						if (pp->sgn < 0) 	Rx = pp->xHat - epsilon;
						else 			Rx = pp->xHat + epsilon;

						Rx = MODULO_RANGE( Rx, -pc->NEAR,
							pc->MAXVAL + pc->NEAR, pc->RANGE_R)

						/* bound Rx */
						Rx = CLIP( Rx, 0, pc->MAXVAL);

				   	/* write reconstructed value */
						*GetXYValue( pd, chan, x, y) = (uint)Rx;		    			    	
						histo[Rx] = 1;
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
	FREE( histo);
#ifdef DEBUG_MED
		WritePPM( pd, "pred_err.ppm");
#endif
	/* Zeichenfläche löschen */
	DeletePicData(result);	
}

