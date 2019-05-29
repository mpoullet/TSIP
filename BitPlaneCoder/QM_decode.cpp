/*********************************************************************
* File	 	: QM_decode.cpp
* Purpose	: to decode binary decisions;
*		   	  to rebuild image from bit planes
* Author 	: Mathias Roeppischer
* Date	 	: 06.12.2013
* Changes	: QM-coder (decoder part); - Roeppischer (18.10.2013)
* Last Modif: 01.02.2014
* 31.03.2014 Strutz: write into buffer array not into file
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitbuf.h"
#include "BP.h"
#include "ginterface.h"
#include "qm_coder.h"
#include "tstypes.h"

/*--------------------------------------------------------------------
* undo_map()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
int undo_map( int p)
{
	if ( p == 0 ) return p;

	if ( !(p & 1) ) p >>= 1;
	else p = -( (p + 1) >> 1 );
	return p;
}

/*--------------------------------------------------------------------
* G_decode()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
G_decode( uint *g)
{
	/* let *g be a number with n binary places */
	uint b = *g;

	b >>= 1; /* let b be a *g variant without LSB */

	/* until b is a zero bit vector... */
	while ( b)
	{
		/*
		* ...compare bit values of b and *g (n - 1) times whereas
		* opposite values yield TRUE
		*/
		*g ^= b;

		b >>= 1; /* replace b by its variant without LSB */
	}
}

/*--------------------------------------------------------------------
* byte_in()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
byte_in( unsigned char *bit_array, 
				CDC_REG *reg, 
				ulong	*byte_pos)
{
	byte
		cpr_byte;

	/* get another QM-compressed byte */
	cpr_byte = bit_array[*byte_pos]; 
	(*byte_pos)++;

	/* fill byte has been detected */
	if (cpr_byte == 0xff)
	{
		/* insert X'FF byte into code register */
		reg->c |= 0xff00;
	}
	/* no fill byte has been detected */
	else
	{
		/* insert byte in high order 8 bits of C-low */
		reg->c += (cpr_byte << 8);
	}
}

/*--------------------------------------------------------------------
* renorm_dec()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
renorm_dec( unsigned char *bit_array, 
						CDC_REG *reg, 
						ulong *byte_pos)
{
	do
	{
		/* code register has been shifted by 8 bits */
		if (reg->cnt == 0)
		{
			/* insert new byte into data bit section of C-low */
			byte_in( bit_array, reg, byte_pos);

			reg->cnt = 8; /* reset SLL countdown */
		}

		reg->a <<= 1; /* double augend */

		/*
		* move compressed bit from position 15 of C-low to position 0
		* of Cx
		*/
		reg->c <<= 1;

		/* count down remaining SLLs until byte_in() is called */
		reg->cnt--;
	}
	/* repeat until size of probability interval at least 0.75 */
	while ( reg->a < 0x8000);
}

/*--------------------------------------------------------------------
* BA_decode()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
cbool
BA_decode( unsigned char *bit_array, CDC_REG *reg, CTX_IDX
		  *ctx, ulong *byte_pos)
{
	cbool
		bit_out = ctx->mps; /* decoded bit */

	reg->a -= ctx->q_val; /* calculate MPS sub-interval */

	/*
	* SRL of code register C by 16 bits to gain register Cx (high
	* order bits of C) for comparison with probability register A
	*
	* decode MPS for context index S unless conditional exchange
	* required
	*/
	if ( (reg->c >> 16) < reg->a )
	{
		/*
		* renormalization (on MPS path) required (size of augend less
		* than 0.75)
		*/
		if (reg->a < 0x8000)
		{
			/*
			* size of MPS sub-interval smaller than LPS probability
			* estimate
			*/
			if (reg->a < ctx->q_val)
			{
				/* assign reverse sense of MPS to decoded bit */
				bit_out = !bit_out;

				est_q_lps( ctx);
			}
			/*
			* size of MPS sub-interval at least equal to LPS
			* probability estimate
			*/
			else est_q_mps( ctx);
		}
		/*
		* neither renormalization nor conditional exchange required
		*/
		else
		{
			return bit_out; /* MPS shall be decoded */
		}
	}
	/*
	* decode LPS for context index S unless conditional exchange
	* required
	*/
	else
	{
		/* subtract augend from register C-x */
		reg->c -= reg->a << 16;

		/*
		* size of MPS sub-interval at least equal to LPS probability
		* estimate
		*/
		if (reg->a >= ctx->q_val)
		{
			/*
			* shrink size of probability interval to LPS probability
			* estimate
			*/
			reg->a = ctx->q_val;

			/* assign reverse sense of MPS to decoded bit */
			bit_out = !bit_out;

			est_q_lps( ctx);
		}
		/*
		* size of MPS sub-interval smaller than LPS probability
		* estimate
		*/
		else
		{
			/*
			* expand size of probability interval to LPS probability
			* estimate
			*/
			reg->a = ctx->q_val;

			est_q_mps( ctx);
		}
	}

	renorm_dec( bit_array, reg, byte_pos);

	return bit_out;
}

/*--------------------------------------------------------------------
* init_dec()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
init_dec( unsigned char *bit_array, 
					CDC_REG *reg, 
					ulong *byte_pos)
{
	/*
	* precision of probability interval register assumed to be not
	* limited to 16 bits
	*/
	reg->a = 0x10000;

	/* clear code register */
	reg->c = 0;

	/* fill data bits of C-low */
	byte_in( bit_array, reg, byte_pos);

	reg->c <<= 8; /* shift 8 data bits from C-low into C-x */

	/* fill data bits of C-low */
	byte_in( bit_array, reg, byte_pos);

	reg->c <<= 8; /* shift 8 data bits from C-low into C-x */

	/*
	* set SLL countdown to zero so that new data byte can be fetched
	* by renorm_dec()
	*/
	reg->cnt = 0;
}
