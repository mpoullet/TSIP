/*****************************************************
* File..: rice_e.c
* Desc..: implementation of Rice coding encoder
* Author: T. Strutz
* Date..: 14.02.2011
* 04.07.2011 include T4
* 15.07.2011 include debug_out info
* 14.11.2016 include locoImproveFlag
******************************************************/
//#include "huff.h"
//#include "fibocode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "loco.h"
#include "ginterface.h"
#include "bitbuf.h"

#ifdef Q_DEBUG
extern FILE *debug_out;
#endif

long *liner[2];

long Nr[NUM_OF_CONTEXTS+2], Cr[NUM_OF_CONTEXTS+2]; 
long Ar[NUM_OF_CONTEXTS+2], Br[NUM_OF_CONTEXTS+2]; 
void (*rice_encodeR)(unsigned long value, int k, int limit, int qbpp, 
										 BitBuffer *bitbuf);


extern char *q9R;

const int J[32] = { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 
5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static int RUNindex, RUNindex_old;
/* wird in encode_runleghth() und encode_EndOfRun_nl()
verwendet	*/ 


/*--------------------------------------------------------
* locoEncoder()
*--------------------------------------------------------*/
void	locoEncoder( PicData *orig, PicData *pd, 
									int T4_flag, int locoImproveFlag, 
									BitBuffer *bitbuf)
{
	int 	RUNcnt, k, EOLine, r, c;
	long 	last_A, epsilon;
	long 	*data;
	long long sum_val;
	unsigned int comp;
	unsigned long Mepsilon = 0;
	unsigned long py, pos;
	CON	con, *pc;
	OPTION option, *op;
	PRED_STRUC pred_struc, *pp;
	IMAGE_LOCO	image, *im;

	pc = &con;	/* Pointer auf Struktur aller Konstanten	*/
	op = &option;
	im = &image;
	/*
	* Initialisieren
	*/
	op->esc_flag = 1;
	op->rlc_flag = 1;
	op->nearlossless = 0;
	op->T_flag = 0;
	op->T4_flag = T4_flag;
	op->reset_flag = 0;


	im->width = pd->width;
	im->height = pd->height;
	im->size = pd->size;


	/* Allocate data array, set it up to look like there's two pixels of
	* zeros to the left and right - makes loops simpler.
	*/
	//CALLOC( data, (pd->width + 4) * 2, 1, long);
	ALLOC( data, (pd->width + 4) * 2, long);
	liner[0] = data + 2;
	liner[1] = data + 2 + (pd->width + 4);

	/* Encode alle Parameter (pc is dummy) */
	rice_encode_R( (unsigned)op->esc_flag, 1, 0, 0, bitbuf);
	rice_encode_R( (unsigned)op->rlc_flag, 1, 0, 0, bitbuf);
	rice_encode_R( (unsigned)op->nearlossless, 1, 0, 0, bitbuf);
	rice_encode_R( op->T_flag, 1, 0, 0, bitbuf);
	if (op->T_flag)
	{
		rice_encode_R( op->T1, 7, 0, 0, bitbuf);
		rice_encode_R( op->T2, 7, 0, 0, bitbuf);
		rice_encode_R( op->T3, 7, 0, 0, bitbuf);
	}
	rice_encode_R( (unsigned)op->reset_flag, 1, 0, 0, bitbuf);
	if (op->reset_flag)
		rice_encode_R( (unsigned)op->reset, 7, 0, 0, bitbuf);

	/* wenn ESCAPE-Flag aktiviert, dann andere Funktion benutzen	*/
	if (op->esc_flag)	rice_encodeR = rice_encode_escR;
	else		rice_encodeR = rice_encode_R;

	//	CALLOC( cntd, pc->RANGE, 1, unsigned long);
	//	ALLOC( cntd, pc->RANGE, unsigned long);

	pp = &pred_struc;
	RUNindex = 0;
	for (comp = 0; comp < pd->channel; comp++) 
		/* all colour components*/
	{						
		/* clean up the two lines */
		memset( data, 0, (pd->width + 4) * 2 * sizeof(long));
		im->bit_depth = orig->bitperchan[comp];
		im->max_value = (long)*GetXYValue( orig, comp, 0, 0);
		sum_val = 0;
		for ( r = 0; r < (signed)pd->height; r++)
		{
			/* all columns	*/
			for ( c = 0; c < (signed)pd->width; c++)
			{		
				py =(unsigned long)*GetXYValue( orig, comp, c, r);
				if (im->max_value < (signed)py )
				{
					im->max_value = (long)py;
				}
				sum_val += py;
			}
		}
		op->T4 = (long)(sum_val / im->size); /* required for T4	*/
		//if (comp>1) op->T4_flag = 0;

		/* max. value must be transmitted if adaptive value is used */
		//im->max_value = (1 << orig->bitperchan[comp]) -1;
		rice_encode_R( im->max_value, orig->bitperchan[comp], 0, 0, bitbuf);
		if (op->T4_flag)
		{
			rice_encode_R( op->T4, orig->bitperchan[comp], 0, 0, bitbuf);
		}
		/* Initialize the model */
		init_constantsR( im, op, pc);
		init_contextsR( op, pc);
		for ( py = 0, r = 0; r < (signed)pd->height; r++, py+= pd->height)
		{
			/* all columns	*/
			for ( pos = py, c = 0; c < (signed)pd->width; pos++, c++)
			{		
				Rx = (long)*GetXYValue( orig, comp, c, r);
				predictor( im, op, pc, pp, r, c);
				if (pp->rlc_flag)	/* Runlength mode. */
				{
					/* The runlength is broken at the end of line */
					RUNcnt = 0;	EOLine = 0;	last_A = Ra;

					while (abs( Rx - last_A) <= pc->NEAR)
					{
						Rx = last_A;	/* Aktualisierung erforderlich,
													* wenn nearlossless Mode  
													*/
						RUNcnt++;
						if (c == (signed)(im->width-1) ) 
						{ 
							EOLine = 1; break;
						}
						c++;
						Rx = (long)*GetXYValue( orig, comp, c, r);
					};
					encode_runlength( RUNcnt, EOLine, bitbuf);
					if (!EOLine) 
					{
						/* Wertrueckgabe fuer near-lossless Modus	*/ 
						Rx = encode_EndOfRun_nl( pc, Rx, last_A,
							Rb, bitbuf);
					}
				}
				else	/* regular mode	*/
				{
					int cx;

					cx = pp->state;

					/* correct xHat, bound xHat, compensate context sign	*/
					if (cx != 0) /* normally cx==0 forces rlc mode and cx is here always >0	*/
					{
						if (pp->sgn > 0)	pp->xHat += Cr[cx]; 
						else			pp->xHat -= Cr[cx]; 
					}

					pp->xHat = CLIP( pp->xHat, 0, pc->MAXVAL);

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
					/*
					cntq9[ abs(q9[Rx-Ra + pc->MAXVAL]) ]++;
					cntq9[ abs(q9[Rc-Ra + pc->MAXVAL]) ]++;

					cntd[abs(Rx-Ra)]++;
					cntd[abs(Rx-Ra)]++;
					cntd[abs(Rc-Ra)]++;
					*/
					/* Praediktionsfehler modulo	*/
					epsilon = MODULO_RANGE( epsilon, -pc->RANGEd2,
						pc->RANGEm1d2, pc->RANGE)

						/* Compute Rice coding paramter */
if (locoImproveFlag)	
{
						/* for (k = 0; (N[cx] << k) < A[cx]; k++); 	*/
						/* "nst <= At" bringt für Y bessere Ergebnisse als nur "<"	*/
				  	if (Ar[cx] == Nr[cx])
						{ /* especially important for chrominance components	*/
							k = 0;
						}
						else
						{
							register int nst = Nr[cx], At = Ar[cx];
							//for ( k = 0; nst < At; nst <<= 1, k++, assert( k < 99));
							for ( k = 0; nst <= At; nst <<= 1, k++, assert( k < 99));
						}
}
else
				  	{
							register int nst = Nr[cx], At = Ar[cx];
							for ( k = 0; nst < At; nst <<= 1, k++, assert( k < 99));
						}

						/* Remap epsilon, different for NEAR==0 ! */
						if ( (k == 0) && (pc->NEAR == 0) && 
							(2 * Br[cx] <= -Nr[cx]) )
						{
							if (epsilon < 0) 	Mepsilon = -2 * (epsilon+1);
							else 		Mepsilon =  2 *  epsilon+1;
						}
						else
						{
							if (epsilon < 0) 	Mepsilon = -2 * epsilon-1;
							else 		Mepsilon =  2 * epsilon;
						}

#ifdef Q_DEBUG
						if (debug_out != NULL)
						{
							fprintf( debug_out, "(%3d,%3d) x=%3d xHat=%3d e=%3d cx=%3d A=%3d B=%3d C=%3d N=%3d\n",
								r, c, Rx, pp->xHat, epsilon, cx, Ar[cx], Br[cx], Cr[cx], Nr[cx]);
						}
#endif


						/* Rice encode epsilon. */
						rice_encodeR( Mepsilon, k, pc->LIMITq, pc->qbpp, bitbuf);

						/* update the context */
						Ar[cx] += abs( epsilon);
						Br[cx] += epsilon * pc->NEARm2p1;

						/* Rescale N, A and B if the counts get too large */
						if (Nr[cx] == pc->RESET) 
						{ 
							Ar[cx] >>= 1; 
							Nr[cx] >>= 1; 
							if (Br[cx] >= 0) Br[cx] >>= 1; 
							else 	Br[cx] = -((1 - Br[cx]) >> 1);
						}

						/* Increment occurence count */
						Nr[cx]++; 

						/* Compute the bias correction term */
						if (Br[cx] <= -Nr[cx] + 0)
						{
							if (Cr[cx] > pc->MIN_C) Cr[cx]--; 
							Br[cx] += Nr[cx];
							if (Br[cx] <= -Nr[cx]) Br[cx] = -Nr[cx] + 1;
						} 
						else if (Br[cx] > 0)
						{
							if (Cr[cx] < pc->MAX_C) Cr[cx]++; 
							Br[cx] -= Nr[cx];
							if (Br[cx] > 0) Br[cx] = 0;
						} 
				}	/* regular mode	*/

			} /* c */
		} /* r */
		free_qR( );
	} /* comp */


	/* Flush the last output byte, if necessary. */
	// this will be done in codec_e.c, Encode()
	//for (i = 0; i < 7; i++) BufferPutBit( bitbuf, 0);

	FREE( data);
}

/*---------------------------------------------------------------
*	rice_encode_R()
*---------------------------------------------------------------*/
void rice_encode_R( unsigned long value, int k, int limit, 
									 int qbpp, BitBuffer *bitbuf)
{
	/* varaiable limit is obsolete in this routine */
	register unsigned long mask, remainder;

	remainder = value >> k; /* get most significant bits */
	{
		/* unary coding of 'remainder' */
		/*
		while (remainder) { remainder--; output_bit( 0, out);}
		output_bit( 1, out);	/* terminating '1' 
		*/
		while (remainder) { remainder--; BufferPutBit( bitbuf, 0);}
		BufferPutBit( bitbuf, 1);	/* terminating '1' */

		/* send k least significant bits	of 'value' */
		for ( mask = (1 << k) >> 1; mask; mask >>= 1)
		{
			/* use mask for indication of bit position */
			/*
			if (value & mask)	output_bit( 1, out);
			else 		output_bit( 0, out);
			*/
			if (value & mask)	BufferPutBit( bitbuf, 1);
			else 		BufferPutBit( bitbuf, 0);
		}
	}
}

/*---------------------------------------------------------------
*	rice_encode_escR()
*---------------------------------------------------------------*/
void rice_encode_escR( 
											unsigned long value, /* zu codierender Wert */
											int k, /* Codierungsparameter */
											int limit, /* max. erlaubte Codewortlänge */
											int qbpp, /* Anz. Bits für größtmöglichen Wert */
											BitBuffer *bitbuf)
{
	register unsigned long mask, remainder;

	remainder = value >> k;    
	if (remainder < (unsigned)limit)
	{
		/*
		while (remainder) { remainder--; output_bit( 0, out);}
		output_bit( 1, out);
		*/
		while (remainder) { remainder--; BufferPutBit( bitbuf, 0);}
		BufferPutBit( bitbuf, 1);
		/* value mit k Bits senden	*/
		for ( mask = (1 << k) >> 1; mask; mask >>= 1)
		{
			/*
			if (value & mask)	output_bit( 1, out);
			else 		output_bit( 0, out);
			*/
			if (value & mask)	BufferPutBit( bitbuf, 1);
			else 		BufferPutBit( bitbuf, 0);
		}
	}
	else	/* Anzahl der Bits auf limit begrenzen	*/
	{
		/*		while (limit) { limit--; output_bit( 0, out);}*/
		while (limit) { limit--; BufferPutBit( bitbuf, 0);}
		/* superfluous, siehe [Wei00] p.1316 */
#ifndef SAVE_BITS
		BufferPutBit( bitbuf, 1);
#endif	
		/* value mit qbpp Bits senden	*/
		value--;
		for ( mask = (1 << qbpp) >> 1; mask; mask >>= 1)
		{
			/*
			if (value & mask)	output_bit( 1, out);
			else 		output_bit( 0, out);
			*/
			if (value & mask)	BufferPutBit( bitbuf, 1);
			else 		BufferPutBit( bitbuf, 0);
		}	
	}
}

/*---------------------------------------------------------------
*	output_bit()
*---------------------------------------------------------------*/
void output_bit( int bit, FILE *out)
{
	/* start with MSB and empty byte */
	static unsigned char mask = 0x80, value = 0x0;

	/* add bit at position indicated by mask */
	if (bit) value |= mask;

	/* if last (8th) position is reached, the output byte */
	if (mask == 0x1) 
	{ 
		putc( value, out); mask = 0x80; value = 0x0;
	}
	else mask >>= 1; /* go to next bit position */
}

/*---------------------------------------------------------------
*	encode_runlength()
*---------------------------------------------------------------*/
void encode_runlength( int RUNcnt, int EOLine, BitBuffer *bitbuf)
{
	register int mask;

	/* merken des Index fuer encode_EndOfRun_nl()	*/
	RUNindex_old = RUNindex;

	/* Segmente der Laenge (1 << J[RUNindex]) mit einem 
	* Bit codieren
	*/
	while (RUNcnt >= (1 << J[RUNindex]) )
	{
		BufferPutBit( bitbuf, 1);
		RUNcnt -= (1 << J[RUNindex]);
		if (RUNindex < 31) RUNindex++;
	}

	/* verbleibendes Segment des Runs codieren	*/
	if (!EOLine)
	{
		/* Runcnt codieren	*/
		BufferPutBit( bitbuf, 0);
		for ( mask = (1 << J[RUNindex]) >> 1; mask; mask >>= 1)
		{
			/*
			if (RUNcnt & mask)	output_bit( 1, out);
			else 		output_bit( 0, out);
			*/
			if (RUNcnt & mask)	BufferPutBit( bitbuf, 1);
			else 		BufferPutBit( bitbuf, 0);
		}
		if (RUNindex) RUNindex--;
	}
	else if (RUNcnt > 0)	BufferPutBit( bitbuf, 1);
}

/*---------------------------------------------------------------
*	encode_EndOfRun_nl()  -- nearlossless
*---------------------------------------------------------------*/
long encode_EndOfRun_nl( CON *pc, long x, long a, long b, 
												BitBuffer *bitbuf)
{
	int 	k, Rtype, sign, cx, map;
	long 	xHat, epsilon, temp;
	unsigned long Mepsilon;

	if ( abs( a - b) <= pc->NEAR )
	{
		Rtype = 1;
		xHat = a;
		epsilon = x - xHat;
		sign = 1;
	}
	else
	{
		Rtype = 0;
		xHat = b;
		epsilon = x - xHat;
		if (a > b)
		{
			epsilon = -epsilon;
			sign = -1;
		}
		else	sign = 1;
	}	

	/* Quantisierung des Praediktionsfehlers	*/
	if (epsilon > 0)
		epsilon =  (pc->NEAR + epsilon) / pc->NEARm2p1;
	else
		epsilon = -(pc->NEAR - epsilon) / pc->NEARm2p1;


	/* Rekonstruktion des aktuellen Bildpunktes	*/
	if (sign < 0)	x = xHat - epsilon * pc->NEARm2p1;
	else		x = xHat + epsilon * pc->NEARm2p1;

	/* Bound Rx. */		
	x = CLIP( x, 0, pc->MAXVAL);

	/* Praediktionsfehler modulo	*/
	epsilon = MODULO_RANGE( epsilon, -pc->RANGEd2,
		pc->RANGEm1d2, pc->RANGE)

		/*
		if (Rtype)	temp = Ar[366] + (Nr[366] >> 1);
		else 	temp = Ar[365];

		cx = 365 + Rtype;

		doubled number of contexts caused by T4
		*/
		if (Rtype)	temp = Ar[731] + (Nr[731] >> 1);
		else 	temp = Ar[730];

		cx = 730 + Rtype;

		/* Compute Rice coding paramter */
		for (k = 0; (Nr[cx] << k) < temp; k++);

		if ( (k == 0) && (epsilon > 0) && (2 * Br[cx] <  Nr[cx]) ) map = 1;
		else if (        (epsilon < 0) && (2 * Br[cx] >= Nr[cx]) ) map = 1;
		else if (        (epsilon < 0) && (k != 0) ) map = 1;
		else	map = 0;

		Mepsilon = 2 * abs( epsilon) - Rtype - map;
		assert ( Mepsilon >= 0);

		rice_encodeR( Mepsilon, k, pc->LIMITq - J[RUNindex_old] -1, 
			pc->qbpp, bitbuf);

		/* Update the context. */
		if (epsilon < 0)	Br[cx]++;
		Ar[cx] += (Mepsilon + 1 - Rtype) >> 1;

		/* Rescale N, A and B if the counts get too large */
		if (Nr[cx] == pc->RESET) 
		{ 
			Nr[cx] >>= 1; 
			Br[cx] >>= 1; 
			Ar[cx] >>= 1; 
		}
		Nr[cx]++;
		return x;
}
