/*****************************************************************
 * File..: rice_e.c
 * Desc..: implementation of Rice coding encoder 
 * Author: T. Strutz
 * Date..: 14.02.2011
 * 05.07.2011 include T4
 * 03.02.2012 type cast
 * 14.11.2016 include locoImproveFlag
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <assert.h>
#include "loco.h"
#include "ginterface.h"
#include "bitbuf.h"

/* Passed globally for efficiency reasons. */
extern char *q9;


extern const int J[32] = { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 
5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15};
/* initialisation to zero is not necessary here, but satisfies the Linker
 * for unknown reason
 */
extern int RUNindex=0, RUNindex_old=0; 
/* wird in decode_runlength() und dencode_EndOfRun_nl()
verwendet	*/ 


void
free_qR( void),
init_contextsR( OPTION *op, CON *c),
init_constantsR( IMAGE_LOCO *im, OPTION *op, CON *pc),
predictor( IMAGE_LOCO *im, OPTION *op, CON *pc, PRED_STRUC *pp, 
					int r, int c);

unsigned long 
decode_runlength( unsigned long max, BitBuffer *bitbuf),
decode_EndOfRun_nl( CON *pc, long a, long b, BitBuffer *bitbuf),
rice_decode_R(    int k, int limit, int qbpp, BitBuffer *bitbuf),
rice_decode_escR( int k, int limit, int qbpp, BitBuffer *bitbuf),
(*rice_decodeR)( int k, int limit, int qbpp, BitBuffer *bitbuf);




/*---------------------------------------------------------------
*	locoDecoder() -- nearlossless
*---------------------------------------------------------------*/
void locoDecoder( PicData *pd, int T4_flag, int locoImproveFlag,
								 BitBuffer *bitbuf)
{
	unsigned int comp;
	int 	RUNcnt, k, r, c;
	long 	last_A, epsilon;
	unsigned long Mepsilon = 0;
	int 	i;
	long 	*data;
	CON	con, *pc;
	OPTION option, *op;
	PRED_STRUC pred_struc, *pp;
	IMAGE_LOCO	image, *im;

	pc = &con;	/*Pointer auf Struktur aller Konstanten	*/
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
	pp = &pred_struc;

	/* Allocate data array, set it up to look like there's two pixels of
	* zeros to the left and right - makes loops simpler.
	*/
	//CALLOC( data, (pd->width + 4) * 2, 1, long);
	ALLOC( data, (pd->width + 4) * 2, long);
	liner[0] = data + 2;
	liner[1] = data + 2 + (pd->width + 4);

	/* Read encoding options */
	op->esc_flag = rice_decode_R( 1, 0, 0, bitbuf);
	op->rlc_flag = rice_decode_R( 1, 0, 0, bitbuf);
	op->nearlossless = rice_decode_R( 1, 0, 0, bitbuf);
	op->T_flag = rice_decode_R( 1, 0, 0, bitbuf);
	if (op->T_flag)
	{
		op->T1 = rice_decode_R( 7, 0, 0, bitbuf);
		op->T2 = rice_decode_R( 7, 0, 0, bitbuf);
		op->T3 = rice_decode_R( 7, 0, 0, bitbuf);
	}
	op->reset_flag = rice_decode_R( 1, 0, 0, bitbuf);
	if (op->reset_flag)
		op->reset = rice_decode_R( 7, 0, 0, bitbuf);

	if (op->esc_flag)	rice_decodeR = rice_decode_escR;
	else		rice_decodeR = rice_decode_R;


		RUNindex = 0;
	/* Coding loop */
	for (comp = 0; comp < pd->channel; comp++) 
		/* all colour components*/
	{									
		/* clean up the two lines */
		memset( data, 0, (pd->width + 4) * 2 * sizeof(long));
		im->bit_depth = pd->bitperchan[comp];
		im->max_value = 
			(unsigned short)rice_decode_R( pd->bitperchan[comp], 0, 0, bitbuf);
		pd->maxVal_rct[comp] = im->max_value;
		//if (comp>1) op->T4_flag = 0;
		if (op->T4_flag)
		{
			op->T4 = 
			(unsigned short)rice_decode_R( pd->bitperchan[comp], 0, 0, bitbuf);
		}
		/* Initialize the model */
		init_constantsR( im, op, pc);
		init_contextsR( op, pc);
		for (r = 0; r < (signed)im->height; r++)	/* all rows	*/
		{
			for (c = 0; c < (signed)im->width; c++)	/* all columns	*/
			{
				predictor( im, op, pc, pp, r, c);
				if (pp->rlc_flag)	/* Runlength mode. */
				{
					last_A = Ra;

					RUNcnt = decode_runlength( im->width - c, bitbuf);
					for ( i = 0; i < RUNcnt; i++, c++)
					{
						Rx = last_A;
						//put_pgm_value( (unsigned long)last_A, out);
						*GetXYValue( pd, comp, c, r) = last_A;
					}
					if (c < (signed)im->width)
					{
						Rx = (long)( 
							decode_EndOfRun_nl( pc, last_A, Rb, bitbuf));
						//put_pgm_value( (unsigned long)Rx, out);
						*GetXYValue( pd, comp, c, r) = Rx;
					}
				}
				else /* Context mode. */
				{
					int cx;

					cx = pp->state;

					/* correction of xHat	*/
					if (pp->sgn > 0)	pp->xHat += Cr[cx]; 
					else 			pp->xHat -= Cr[cx]; 

					/* Bound xHat */
					pp->xHat = CLIP( pp->xHat, 0, pc->MAXVAL);

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

					/* Decode mapped epsilon. */
					Mepsilon = rice_decodeR( k, pc->LIMITq, pc->qbpp, bitbuf);

					/* Unmap to compute epsilon. */
					if ( (k == 0) && (pc->NEAR == 0) && 
						(2 * Br[cx] <= -Nr[cx]) )
					{
						if (Mepsilon & 0x1) epsilon =   Mepsilon >> 1;
						else 		epsilon = -(long)(Mepsilon >> 1) - 1;
					}
					else
					{
						if (Mepsilon & 0x1) epsilon = -(long)(Mepsilon >> 1) - 1;
						else 		epsilon =   Mepsilon >> 1;
					}

					/* Update the context. */
					Ar[cx] += abs(epsilon);
					epsilon *= pc->NEARm2p1;
					Br[cx] += epsilon;

					/* reconstruction of pixel value Rx	*/
					if (pp->sgn < 0) 	Rx = pp->xHat - epsilon;
					else 			Rx = pp->xHat + epsilon;

					Rx = MODULO_RANGE( Rx, -pc->NEAR,
						pc->MAXVAL + pc->NEAR, pc->RANGE_R)

						/* bound Rx */
						Rx = CLIP( Rx, 0, pc->MAXVAL);

					//put_pgm_value( (unsigned long)Rx, out);
					*GetXYValue( pd, comp, c, r) = Rx;

					/* Rescale N, A and B if the counts get too large. */
					if (Nr[cx] == pc->RESET) 
					{ 
						Ar[cx] >>= 1; 
						Nr[cx] >>= 1; 
						if (Br[cx] >= 0) Br[cx] >>= 1; 
						else 	Br[cx] = -((1 - Br[cx]) >> 1);
					}

					/* Increment occurence count. */
					Nr[cx]++; 

					/* Compute the bias correction term. */
					if (Br[cx] <= -Nr[cx])
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
			}	/* all columns	*/
		}	/* all rows	*/
		free_qR( );
	}	/* comp	*/
	FREE( data);
}

/*---------------------------------------------------------------
*	rice_decode_R()
*---------------------------------------------------------------*/
unsigned long rice_decode_R( int k, int limit, int qbpp, BitBuffer *bitbuf)
{
	register unsigned long r, mask;
	unsigned long 	 value;

	/* read zeros (unary code with terminating '1' */
	/*
	for (r = 0; !input_bit( in); r++)
	;
	*/
	for (r = 0; !BufferGetBit( bitbuf); r++)
		;

	/* read k least significant bits of 'value' */
	for (value = 0, mask = (1 << k) >> 1; mask; mask >>= 1)
	{
		/*
		if (input_bit( in)) value |= mask;
		*/
		if (BufferGetBit( bitbuf)) value |= mask;
	}

	value += (r << k); /* combine with most significant bits */

	return value;
}

/*---------------------------------------------------------------
*	rice_decode_escR()
*---------------------------------------------------------------*/
unsigned long rice_decode_escR( int k, int limit, int qbpp, BitBuffer *bitbuf)
{
	register unsigned long r, mask;
	unsigned long 	 value;

#ifndef SAVE_BITS
	/*
	for (r = 0; !input_bit( in); r++)
	;
	*/
	for (r = 0; !BufferGetBit( bitbuf); r++)
		;
#else
	/*
	for (r = 0; (r < limit) && !input_bit( in); r++)
	;
	*/
	for (r = 0; (r < limit) && !BufferGetBit( bitbuf); r++)
		;
#endif	
	assert ( (signed)r <=  limit);	

	if ((signed)r < limit)
	{   
		for (value = 0, mask = (1 << k) >> 1; mask; mask >>= 1)
		{
			/*
			if (input_bit( in)) value |= mask;
			*/
			if (BufferGetBit( bitbuf)) value |= mask;
		}

		value += (r << k);
	}
	else
	{
		for (value = 0, mask = (1 << qbpp) >> 1; mask; mask >>= 1)
		{
			/*
			if (input_bit( in)) value |= mask;
			*/
			if (BufferGetBit( bitbuf)) value |= mask;
		}
		value++;
	}

	return value;
}

/*---------------------------------------------------------------
*	input_bit()
*---------------------------------------------------------------*/
unsigned char input_bit_obsolete( FILE *in)
{
	/* start with empty byte */
	static unsigned char  mask = 0x0, value = 0x0, bit;

	if (mask == 0x0) /* if all bits have been used */
	{
		/* read a new byte from input stream, set MSB position in 'mask' */
		value = (unsigned char)getc( in); mask = 0x80;
	}
	/* extract one bit at position indicated by 'mask' */
	if (value & mask) bit = 1;
	else bit = 0;

	mask >>= 1; /* go to next position */

	return bit;
}

/*---------------------------------------------------------------
*	decode_runlength()
*---------------------------------------------------------------*/
unsigned long decode_runlength( unsigned long max, BitBuffer *bitbuf)
{
	int value, mask;
	unsigned long RUNcnt=0;

	/* merken des Index fuer encode_EndOfRun_nl()	*/
	RUNindex_old = RUNindex;

	/* RUNcnt muss zuerst getestet werden, bevor neues 
	* Bit eingelesen wird
	*/
	/*
	while ((RUNcnt < max) && input_bit( in) ) 
	*/
	while ((RUNcnt < max) && BufferGetBit( bitbuf) ) 
	{
		RUNcnt += (1 << J[RUNindex]);
		if (RUNindex < 31) RUNindex++;
	}

	if (RUNcnt < max)
	{
		for (value = 0, mask = (1 << J[RUNindex]) >> 1; mask; mask >>= 1)
		{
			/*
			if (input_bit( in)) value |= mask;
			*/
			if (BufferGetBit( bitbuf)) value |= mask;
		}
		RUNcnt += value;
		if (RUNindex) RUNindex--;
		return RUNcnt;
	}
	else if (RUNcnt == max) return RUNcnt;	/* RUNindex behalten	*/
	else 
	{
		if (RUNindex) RUNindex--;
		return max;
	}
}

/*---------------------------------------------------------------
*	decode_EndOfRun_nl()  -- nearlossless
*---------------------------------------------------------------*/
unsigned long decode_EndOfRun_nl( CON *pc, long a, long b, BitBuffer *bitbuf)
{
	int 	k, Rtype, sign, cx;
	long 	x, xHat, epsilon, temp;
	long	Mepsilon;

	if ( abs( a - b) <= pc->NEAR )
	{
		Rtype = 1;
		xHat = a;
		sign = 1;
		/* temp = Ar[366] + (Nr[366] >> 1); doubled number of contexts caused by T4*/
		temp = Ar[731] + (Nr[731] >> 1);
	}
	else
	{
		Rtype = 0;
		xHat = b;
		if (a > b)	sign = -1;
		else		sign = 1;
		/* temp = Ar[365]; */
		temp = Ar[730];
	}	

	/* cx = 365 + Rtype;	*/
	cx = 730 + Rtype;

	/* Compute Rice coding paramter */
	for (k = 0; (Nr[cx] << k) < temp; k++);

	/* Decode mapped epsilon. */
	Mepsilon = (long)rice_decodeR( k, pc->LIMITq - J[RUNindex_old]-1, 
		pc->qbpp, bitbuf);

	/* Unmap to compute epsilon */
	Mepsilon += Rtype;
	if ( Mepsilon & 1)
	{
		if (2 * Br[cx] >= Nr[cx])	epsilon = -(Mepsilon + 1 ) / 2;
		else if (k == 0)	epsilon =  (Mepsilon + 1 ) / 2;
		else			epsilon = -(Mepsilon + 1 ) / 2;
	}
	else if ( (k) || ((k == 0) && (2 * Br[cx] >=  Nr[cx])) )
		epsilon =  Mepsilon  / 2;
	else 
		epsilon = -Mepsilon  / 2;

	Mepsilon -= Rtype;

	/* Rekonstruktion des aktuellen Bildpunktes	*/
	if (sign < 0)	x = xHat - epsilon * pc->NEARm2p1;
	else		x = xHat + epsilon * pc->NEARm2p1;

	x = MODULO_RANGE( x, -pc->NEAR, pc->MAXVAL + pc->NEAR, 
		pc->RANGE_R);

	/* Bound Rx */
	x = CLIP( x, 0, pc->MAXVAL);

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
